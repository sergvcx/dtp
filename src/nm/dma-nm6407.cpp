#include "dtp/nm6407.h"
#include "dtp/dtp.h"
#include "nmsemaphore.h"
#include "stdio.h"
#include "nm6407int.h"
#include "nmassert.h"
#include "tdl.hpp"
#include "nmassert.h"


// Константы смещения регистров передающего канала относительно базового адреса
const static int MAIN_COUNTER    = 0x0;
const static int ADDRESS         = 0x2;
const static int BIAS            = 0x4;
const static int ROW_COUNTER     = 0x6;
const static int ADDRESS_MODE    = 0x8;
const static int CONTROL         = 0xA;
const static int INTERRUPT_MASK  = 0xC;
const static int STATE           = 0xE;

const static int ADDRESS_MODE_1D = 0;
const static int ADDRESS_MODE_2D = 1;


struct DmaState{
    dtp_sem_t *dma_sem = &dma_sem_default;
    dtp_sem_t dma_sem_default = {1};
    int *base_tr;
    int *base_rc;    

    DtpAsync *receive_cmd;
    DtpAsync *transfer_cmd;
};

struct DmaMode{
    DmaState *state;
    int (*startFuncT)(int *base, DtpAsync *startFunc);
};

struct LinkInfo{
    int *base_tr;
    int *base_rc;
    DtpNotifyFunctionT receive_cb[3];
    void *receive_cb_data[3];
    DtpNotifyFunctionT transfer_cb[3];
    void *transfer_cb_data[3];

    DtpAsync *currentTransfer;
    DtpAsync *currentReceive;
};

// int getProcNoNm6407(){
//     int *id = (int*)0x40000000;
//     int no = *id;
//     no >>= 24;
//     return no;
// }



static void dmaHandlerNm6407();
static void dmaHandlerErrorNm6407();
static void linkHandlerNm6407Transfer0();
static void linkHandlerNm6407Receive0();
static void linkHandlerNm6407Transfer1();
static void linkHandlerNm6407Receive1();
static void linkHandlerNm6407Transfer2();
static void linkHandlerNm6407Receive2();

static int dmaStartTask(int *base, DtpAsync *cmd);
static int dmaStartTaskWithCheck(int *base, DtpAsync *cmd);
static int dmaStartTaskSingle(int *base, DtpAsync *cmd);


static inline int dmaLinkStatus2DtpStatus(int *base);
static inline void startDmaOrLink(int *base, void *buf, int size64, int row_counter64, int bias, int mode);

static inline int *nm6407MapAddr(int *addr){
    // mapped addr when inner memory
    int no = (*(int*)0x40000000) >> 24;
    int offset = (   (no) == 0) ? 0x40000: 0x80000;
    addr = ( (int)addr < 0x40000 ) ? addr + offset: addr;
    return addr;
}

static int dmaStartTask(int *base, DtpAsync *cmd){
    int size64 = cmd->nwords >> 1;
    //int bias = (cmd->stride - (cmd->width - 2) ) >> 1;
    int bias = (cmd->stride - (cmd->width - 2) );
    int row_counter64 = cmd->width >> 1;
    int mode = (cmd->type == DTP_TASK_1D) ? 0: 1;

    int *addr = nm6407MapAddr((int *) cmd->buf);

#ifndef NDEBUG    
    NMASSERT(  ( (int)addr & 0xF ) == 0);           // due to DMA error address should be a multiple 16
    NMASSERT(  ( size64 & 0x7 ) == 0);        // due to DMA error size64 should be a multiple 8
    if(mode == 1){
        NMASSERT(  ( bias & 0xF) == 2);          // due to DMA error bias should be a multiple 16 with remainder of 2
        NMASSERT(  ( row_counter64 & 0x7) == 0);   // due to DMA error size64 should be a multiple 8
    }
#endif //NDEBUG

    startDmaOrLink(base, addr, size64, row_counter64, bias, mode);
    return DTP_OK;
}

static int dmaStartTaskWithCheck(int *base, DtpAsync *cmd){
    int size64 = cmd->nwords >> 1;
    //int bias = (cmd->stride - (cmd->width - 2) ) >> 1;
    int bias = (cmd->stride - (cmd->width - 2) );
    int row_counter64 = cmd->width >> 1;
    int mode = (cmd->type == DTP_TASK_1D) ? ADDRESS_MODE_1D: ADDRESS_MODE_2D;

    int *addr = nm6407MapAddr((int *) cmd->buf);

    if( ( (int)addr & 0xF ) != 0) return DTP_ERROR;
    if( ( size64 & 0x7 ) != 0) return DTP_ERROR;
    if(mode == ADDRESS_MODE_2D){
        if( ( bias & 0xF ) != 2) return DTP_ERROR;
        if( ( row_counter64 & 0x7 ) != 0) return DTP_ERROR;
    }

    startDmaOrLink(base, addr, size64, row_counter64, bias, mode);

    return DTP_OK;
}

static int dmaStartTaskSingle(int *base, DtpAsync *cmd){
    int size64 = cmd->nwords >> 1;
    int bias = 2;
    int row_counter64 = 1;
    int mode = 1;

    int *addr = nm6407MapAddr((int *) cmd->buf);

    startDmaOrLink(base, (void *)cmd->buf, size64, row_counter64, bias, mode);

    return DTP_OK;
}

static DmaState dma_state;
static DmaMode dma_packet = {
    .state = &dma_state, 
    .startFuncT = dmaStartTask,
};

static DmaMode dma_packet_check = {
    .state = &dma_state, 
    .startFuncT = dmaStartTaskWithCheck,
};

static DmaMode dma_single = {
    .state = &dma_state,
    .startFuncT = dmaStartTaskSingle
};

static inline void dma_lock(){
    dtp_sem_wait(dma_state.dma_sem);
}
static inline void dma_unlock(){
    dtp_sem_post(dma_state.dma_sem);
}

static LinkInfo link_info;

static inline void startDmaOrLink(int *base, void *buf, int size64, int row_counter64, int bias, int mode){
    base[MAIN_COUNTER] = size64;    
    base[ADDRESS]      = (int)buf;
    base[BIAS]         = bias; //(cmd->stride - (cmd->width - 2) )
    base[ROW_COUNTER]  = row_counter64;             
    base[ADDRESS_MODE] = mode; //(cmd->type == DTP_TASK_1D) ? 0: 1;
    base[CONTROL] = 0;
    base[CONTROL] = 1;
}

static int dmaImplSend(void *com_spec, DtpAsync *cmd){
    DmaMode *info = (DmaMode *)com_spec;
    if(info->state->receive_cmd || info->state->transfer_cmd) return DTP_AGAIN;

    dma_lock();
    info->state->base_rc[INTERRUPT_MASK] = 0;
    info->state->transfer_cmd = cmd;
    int error = info->startFuncT(info->state->base_tr, cmd);
    dma_unlock();
    
    return error;
}

static int dmaImplRecv(void *com_spec, DtpAsync *cmd){
    DmaMode *info = (DmaMode *)com_spec; 
    if(info->state->receive_cmd || info->state->transfer_cmd) return DTP_AGAIN;

    dma_lock();    
    info->state->base_rc[INTERRUPT_MASK] = 0;    
    info->state->receive_cmd = cmd;
    int error = info->startFuncT(info->state->base_rc, cmd);
    dma_unlock();

    return error;
}


static int dmaImplGetStatus(void *com_spec, DtpAsync *cmd){
    DmaMode *info = (DmaMode *)com_spec;

    if(cmd == info->state->receive_cmd || cmd == info->state->transfer_cmd){        
        int status = dmaLinkStatus2DtpStatus(info->state->base_rc);
        cmd->DTP_ASYNC_PRIVATE_FIELDS.status = status;
        if(status == DTP_ST_DONE){
            if(cmd->callback) cmd->callback(cmd->cb_data);        
            info->state->receive_cmd = 0;
            info->state->transfer_cmd = 0;
        }
    }

    return cmd->DTP_ASYNC_PRIVATE_FIELDS.status;
}

static int dmaImplGetStatusInt(void *com_spec, DtpAsync *cmd){
    DmaMode *info = (DmaMode *)com_spec;
    //cmd->DTP_ASYNC_PRIVATE_FIELDS.status = dmaLinkStatus2DtpStatus(info->base + 0x10);

    return cmd->DTP_ASYNC_PRIVATE_FIELDS.status;
}

static int dmaImplDestroy(void *com_spec){
    return DTP_OK;
}

static int linkImplSend(void *com_spec, DtpAsync *cmd){
    LinkInfo *info = (LinkInfo *)com_spec;

    info->base_tr[INTERRUPT_MASK] = 0;    
    info->transfer_cb[0] = cmd->callback;
    info->transfer_cb_data[0] = cmd->cb_data;    
    info->currentTransfer = cmd;
    dmaStartTask(info->base_tr, cmd);
    
    return DTP_OK;
}

static int linkImplRecv(void *com_spec, DtpAsync *cmd){
    LinkInfo *info = (LinkInfo *)com_spec;

    info->base_rc[INTERRUPT_MASK] = 0;    
    info->receive_cb[0] = cmd->callback;
    info->receive_cb_data[0] = cmd->cb_data;    
    info->currentReceive = cmd;
    dmaStartTask(info->base_tr, cmd);
    
    return DTP_OK;
}

static int linkImplGetStatus(void *com_spec, DtpAsync *cmd){
    LinkInfo *info = (LinkInfo *)com_spec;
    int status;
    if(cmd == info->currentReceive){
        status = dmaLinkStatus2DtpStatus(info->base_rc);
        return status;
    }
    if(cmd == info->currentTransfer){        
        status = dmaLinkStatus2DtpStatus(info->base_rc);
        return status;
    }
    return DTP_ST_ERROR;
}

static int linkImplDestroy(void *com_spec){
    return DTP_OK;
}

int dtpNm6407SetDmaMutex(int *shared_mutex){
    if((int)shared_mutex < 0xA0000) return DTP_ERROR;
    dma_state.dma_sem = (dtp_sem_t *)shared_mutex;
    //dtp_sem_init(dma_inint *base_rc;fo.dma_sem, 1);
}

int dtpNm6407Dma(int desc, int flags){    
    DmaState *info = &dma_state;    
    //nm_sem_init(&info->dma_sem, 1);
    info->base_tr = (int*)0x10010000;
    info->base_rc = (int*)0x10010010;    

    info->receive_cmd = 0;
    info->transfer_cmd = 0;

    info->base_tr[INTERRUPT_MASK] = 3;
    info->base_rc[INTERRUPT_MASK] = 3;

    DtpImplementation impl;
    if(flags & DTP_NM6407_DMA_STATUS_ONLY){
        impl.update_status = dmaImplGetStatus;
    } else {
        EnableInterrupts_PSWR(I_MASK_INM);
        EnableInterrupts_IMR_High(1 << 0);
        EnableInterrupts_IMR_High(1 << 1);
        SetInterruptPeriphery(32, dmaHandlerNm6407);
        SetInterruptPeriphery(33, dmaHandlerErrorNm6407);
        impl.update_status = dmaImplGetStatusInt;
    }
    
    DmaMode *dma_mode = 0;
    if(flags & DTP_NM6407_DMA_PACKET){
        if(flags & DTP_NM6407_DMA_WITHOUT_CHECK){
            dma_mode = &dma_packet;
        } else {
            dma_mode = &dma_packet_check;
        }
    } else {
        dma_mode = &dma_single;    
    }
    
    impl.recv = dmaImplRecv;
    impl.send = dmaImplSend;
    impl.destroy = dmaImplDestroy;
    return dtpBind(desc, dma_mode, &impl);
}

int dtpNm6407Link(int desc, int port, int flags){
    LinkInfo *info = &link_info;
    int direction = dtpGetMode(desc);
    if(port < 0 || port > 2) return -1;
    info->base_tr = (int*)0x40001800 + 0x400 * port;
    info->base_rc = (int*)0x40001810 + 0x400 * port;
    info->base_tr[INTERRUPT_MASK] = 3;
    info->base_rc[INTERRUPT_MASK] = 3;

    
    switch (port)
    {
    case 0:
        if(direction == DTP_WRITE_ONLY){
            EnableInterrupts_IMR_Low(1 << 24);
            SetInterruptPeriphery(24, linkHandlerNm6407Transfer0); 
        } else {
            EnableInterrupts_IMR_Low(1 << 25);
            SetInterruptPeriphery(25, linkHandlerNm6407Receive0);    
        }
        break;
    case 1:
        if(direction == DTP_WRITE_ONLY){
            EnableInterrupts_IMR_Low(1 << 26);
            SetInterruptPeriphery(26, linkHandlerNm6407Transfer1);    
        } else {
            EnableInterrupts_IMR_Low(1 << 27);
            SetInterruptPeriphery(27, linkHandlerNm6407Receive1);    
        }
        break;
    case 2:
        if(direction == DTP_WRITE_ONLY){
            EnableInterrupts_IMR_Low(1 << 28);
            SetInterruptPeriphery(28, linkHandlerNm6407Transfer2);    
        } else {
            EnableInterrupts_IMR_Low(1 << 29);
            SetInterruptPeriphery(29, linkHandlerNm6407Receive2);    
        }
        break;
    
    default:
        break;
    }
        
    DtpImplementation impl;
    impl.recv = linkImplRecv;
    impl.send = linkImplSend;
    impl.update_status = linkImplGetStatus;
    impl.destroy = linkImplDestroy;
    return dtpBind(desc, info, &impl);    
}


static inline void linkHandlerNm6407Transfer0(){
    LinkInfo *info = &link_info;
    if(info->transfer_cb[0]){
        info->transfer_cb[0](info->transfer_cb_data[0]);
    }
    int *interrupt_mask = dma_state.base_tr + 0x0C;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Receive0(){
    LinkInfo *info = &link_info;
    if(info->receive_cb[0]){
        info->receive_cb[0](info->receive_cb_data[0]);
    }
    int *interrupt_mask = dma_state.base_tr + 0x1C;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Transfer1(){
    LinkInfo *info = &link_info;
    if(info->transfer_cb[1]){
        info->transfer_cb[1](info->transfer_cb_data[1]);
    }

    int *interrupt_mask = dma_state.base_tr + 0x0C + 0x400;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Receive1(){
    LinkInfo *info = &link_info;
    if(info->receive_cb[1]){
        info->receive_cb[1](info->receive_cb_data[1]);
    }
    int *interrupt_mask = dma_state.base_tr + 0x1C + 0x400;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Transfer2(){
    LinkInfo *info = &link_info;
    if(info->transfer_cb[2]){
        info->transfer_cb[2](info->transfer_cb_data[2]);
    }
    int *interrupt_mask = dma_state.base_tr + 0x0C + 2 * 0x400;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Receive2(){
    LinkInfo *info = &link_info;
    if(info->receive_cb[2]){
        info->receive_cb[2](info->receive_cb_data[2]);
    }
    int *interrupt_mask = dma_state.base_tr + 0x1C + 2 * 0x400;
    *interrupt_mask = 3;
}

void __attribute__((optimize("O0"))) dmaHandlerNm6407(){
    
    if(dma_state.receive_cmd){
        if(dma_state.receive_cmd->callback){
            dma_state.receive_cmd->callback(dma_state.receive_cmd->cb_data);
        }
        dma_state.receive_cmd->DTP_ASYNC_PRIVATE_FIELDS.status = DTP_ST_DONE;
        dma_state.receive_cmd = 0;
    }
    if(dma_state.transfer_cmd){
        if(dma_state.transfer_cmd->callback){
            dma_state.transfer_cmd->callback(dma_state.transfer_cmd->cb_data);
        }
        dma_state.transfer_cmd->DTP_ASYNC_PRIVATE_FIELDS.status = DTP_ST_DONE;
        dma_state.transfer_cmd = 0;
    }

    dma_state.base_tr[INTERRUPT_MASK] = 3;
    dma_state.base_rc[INTERRUPT_MASK] = 3;

    int tmp = dma_state.base_tr[0];
    tmp++;
    int *reserved = (int *)0x40000406;
    *reserved = tmp;    


    int *IASH_CLR = (int*)0x4000045C;
    *IASH_CLR = 1;
    
}

void dmaHandlerErrorNm6407(){
    
    dma_state.base_tr[0x0C] = 3;
    dma_state.base_tr[0x1C] = 3;

    int tmp = dma_state.base_tr[0];
    tmp++;
    int *reserved = (int *)0x40000406;
    *reserved = tmp;    

    int *IASH_CLR = (int*)0x4000045C;
    *IASH_CLR = 1;
}

static inline int dmaLinkStatus2DtpStatus(int *base){    
    int status = base[CONTROL];
    if(status & 4){
        base[CONTROL] = base[CONTROL] & 0xB;
        return DTP_ST_ERROR;
    } 
    if(status & 2){       
        return DTP_ST_DONE; 
    }
    return DTP_ST_IN_PROCESS;
}


