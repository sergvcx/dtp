#include "dtp/nm6407.h"
#include "dtp/dtp.h"
#include "nmsemaphore.h"
#include "stdio.h"
#include "nm6407int.h"
#include "nmassert.h"

typedef int(*get_proc_no_func_t)();


// Константы смещения регистров передающего канала относительно базового адреса
const static int TR_MAIN_COUNTER    = 0x0;
const static int TR_ADDRESS         = 0x2;
const static int TR_BIAS            = 0x4;
const static int TR_ROW_COUNTER     = 0x6;
const static int TR_ADDRESS_MODE    = 0x8;
const static int TR_CONTROL         = 0xA;
const static int TR_INTERRUPT_MASK  = 0xC;
const static int TR_STATE           = 0xE;

// Константы смещения регистров принимающего канала относительно базового адреса
const static int RC_MAIN_COUNTER    = 0x10;
const static int RC_ADDRESS         = 0x12;
const static int RC_BIAS            = 0x14;
const static int RC_ROW_COUNTER     = 0x16;
const static int RC_ADDRESS_MODE    = 0x18;
const static int RC_CONTROL         = 0x1A;
const static int RC_INTERRUPT_MASK  = 0x1C;
const static int RC_STATE           = 0x1E;

struct DmaInfo{
    //nm_sem_t *dma_sem;
    int *base;
    int is_used = 0;
    int is_packed = 0;
    get_proc_no_func_t get_proc;

    DtpNotifyFunctionT receive_cb;
    void *receive_cb_data;
    DtpNotifyFunctionT transfer_cb;
    void *transfer_cb_data;

    DtpAsync *processingTasks[2]; // 0 - send, 1 - recv
    int status[2];
    int ireserved;
};

struct LinkInfo{
    int *base;
    DtpNotifyFunctionT receive_cb[3];
    void *receive_cb_data[3];
    DtpNotifyFunctionT transfer_cb[3];
    void *transfer_cb_data[3];

    DtpAsync *currentTransfer;
    DtpAsync *currentReceive;
};

int getProcNoNm6407(){
    int *id = (int*)0x40000000;
    int no = *id;
    no >>= 24;
    return no;
}

static DmaInfo dma_info;
static LinkInfo link_info;

void dmaHandlerNm6407();
void dmaHandlerErrorNm6407();
static inline void linkHandlerNm6407Transfer0();
static inline void linkHandlerNm6407Receive0();
static inline void linkHandlerNm6407Transfer1();
static inline void linkHandlerNm6407Receive1();
static inline void linkHandlerNm6407Transfer2();
static inline void linkHandlerNm6407Receive2();


static inline void dmaLinkSetTransferRegisters(int *base, DtpAsync *cmd);
static inline void dmaLinkSetReceiveRegisters(int *base, DtpAsync *cmd);
static inline void linkSetIntTransfer(int *base, DtpAsync *cmd);
static inline void linkSetIntReceive(int *base, DtpAsync *cmd);
static inline void dmaSetInt(int *base, DtpAsync *cmd);
static inline void dmaLinkStartTransfer(int *base);
static inline void dmaLinkStartReceive(int *base);
static inline int dmaLinkStatus2DtpStatus(int *base);


static int dmaImplPacketSend(void *com_spec, DtpAsync *cmd){
    DmaInfo *info = (DmaInfo *)com_spec;
    
    //NMASSERT((cmd->buf    & 0xF) == 0);
    //NMASSERT((cmd->nwords & 0xF) == 0);

    dmaLinkSetTransferRegisters(info->base, cmd);    
    dmaSetInt(info->base, cmd);
    info->transfer_cb = cmd->callback;
    info->transfer_cb_data = cmd->cb_data;    

    dmaLinkStartTransfer(info->base);
    
    return DTP_OK;
}

static int dmaImplPacketRecv(void *com_spec, DtpAsync *cmd){
    DmaInfo *info = (DmaInfo *)com_spec;

    dmaLinkSetReceiveRegisters(info->base, cmd);
    dmaSetInt(info->base, cmd);
    info->receive_cb = cmd->callback;
    info->receive_cb_data = cmd->cb_data;

    dmaLinkStartReceive(info->base);    
    
    return DTP_OK;
}

static int dmaImplGetStatus(void *com_spec, DtpAsync *cmd){
    DmaInfo *info = (DmaInfo *)com_spec;

    return dmaLinkStatus2DtpStatus(info->base + 0x10);    
}

static int dmaImplDestroy(void *com_spec){
    return DTP_OK;
}

static int linkImplPacketSend(void *com_spec, DtpAsync *cmd){
    LinkInfo *info = (LinkInfo *)com_spec;
    

    dmaLinkSetTransferRegisters(info->base, cmd);    
    linkSetIntTransfer(info->base, cmd);
    info->transfer_cb[0] = cmd->callback;
    info->transfer_cb_data[0] = cmd->cb_data;    
    info->currentTransfer = cmd;

    dmaLinkStartTransfer(info->base);
    
    return DTP_OK;
}

static int linkImplPacketRecv(void *com_spec, DtpAsync *cmd){
    LinkInfo *info = (LinkInfo *)com_spec;

    dmaLinkSetReceiveRegisters(info->base, cmd);    
    linkSetIntReceive(info->base, cmd);
    info->receive_cb[0] = cmd->callback;
    info->receive_cb_data[0] = cmd->cb_data;    
    info->currentReceive = cmd;

    dmaLinkStartReceive(info->base);
    
    return DTP_OK;
}

static int linkImplGetStatus(void *com_spec, DtpAsync *cmd){
    LinkInfo *info = (LinkInfo *)com_spec;

    if(cmd == info->currentReceive){
        return dmaLinkStatus2DtpStatus(info->base + 0x10);
    }
    if(cmd == info->currentTransfer){
        return dmaLinkStatus2DtpStatus(info->base);
    }
    return DTP_ST_ERROR;
}

static int linkImplDestroy(void *com_spec){
    return DTP_OK;
}

int dtpNm6407Dma(int desc, int mask){
    DmaInfo *info = &dma_info;
    info->is_used = 1;
    //nm_sem_init(&info->dma_sem, 1);
    info->base = (int*)0x10010000;
    info->is_packed = 1;
    info->get_proc = getProcNoNm6407;
    info->processingTasks[0] = 0;
    info->processingTasks[1] = 0;
    info->base[0x0C] = 3;
    info->base[0x1C] = 3;

    EnableInterrupts_PSWR(I_MASK_INM);
    EnableInterrupts_IMR_High(1 << 0);
    EnableInterrupts_IMR_High(1 << 1);
    SetInterruptPeriphery(32, dmaHandlerNm6407);
    SetInterruptPeriphery(33, dmaHandlerErrorNm6407);
    
    DtpImplementation impl;
    impl.recv = dmaImplPacketRecv;
    impl.send = dmaImplPacketSend;
    impl.get_status = dmaImplGetStatus;
    impl.destroy = dmaImplDestroy;
    impl.connect = 0;
    impl.listen = 0;
    return dtpSetImplementation(desc, info, &impl);
}

int dtpNm6407Link(int desc, int port){
    LinkInfo *info = &link_info;
    int direction = dtpGetMode(desc);
    info->base = (int*)0x40001800;
    switch (port)
    {
    case 0:
        info->base = (int*)0x40001800;
        break;
    case 1:
        info->base = (int*)0x40001800 + 0x400;
        break;
    case 2:
        info->base = (int*)0x40001800 + ( 0x400 << 1 );
        break;
    
    default:
        return -1;
    }
    info->base[0x0C] = 3;
    info->base[0x1C] = 3;

    int mask = 1 << 24;
    mask <<= port;
    mask <<= direction;
    EnableInterrupts_IMR_Low(mask);
    switch (port)
    {
    case 0:
        if(direction == DTP_WRITE_ONLY){
            SetInterruptPeriphery(24, linkHandlerNm6407Transfer0); 
        } else {
            SetInterruptPeriphery(25, linkHandlerNm6407Receive0);    
        }
        break;
    case 1:
        if(direction == DTP_WRITE_ONLY){
            SetInterruptPeriphery(26, linkHandlerNm6407Transfer1);    
        } else {
            SetInterruptPeriphery(27, linkHandlerNm6407Receive1);    
        }
        break;
    case 2:
        if(direction == DTP_WRITE_ONLY){
            SetInterruptPeriphery(28, linkHandlerNm6407Transfer2);    
        } else {
            SetInterruptPeriphery(29, linkHandlerNm6407Receive2);    
        }
        break;
    
    default:
        break;
    }
        
    DtpImplementation impl;
    impl.recv = linkImplPacketRecv;
    impl.send = linkImplPacketSend;
    impl.get_status = linkImplGetStatus;
    impl.destroy = linkImplDestroy;
    impl.connect = 0;
    impl.listen = 0;
    return dtpSetImplementation(desc, info, &impl);    
}


static inline void linkHandlerNm6407Transfer0(){
    LinkInfo *info = &link_info;
    if(info->transfer_cb[0]){
        info->transfer_cb[0](info->transfer_cb_data[0]);
    }
    int *interrupt_mask = dma_info.base + 0x0C;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Receive0(){
    LinkInfo *info = &link_info;
    if(info->receive_cb[0]){
        info->receive_cb[0](info->receive_cb_data[0]);
    }
    int *interrupt_mask = dma_info.base + 0x1C;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Transfer1(){
    LinkInfo *info = &link_info;
    if(info->transfer_cb[1]){
        info->transfer_cb[1](info->transfer_cb_data[1]);
    }

    int *interrupt_mask = dma_info.base + 0x0C + 0x400;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Receive1(){
    LinkInfo *info = &link_info;
    if(info->receive_cb[1]){
        info->receive_cb[1](info->receive_cb_data[1]);
    }
    int *interrupt_mask = dma_info.base + 0x1C + 0x400;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Transfer2(){
    LinkInfo *info = &link_info;
    if(info->transfer_cb[2]){
        info->transfer_cb[2](info->transfer_cb_data[2]);
    }
    int *interrupt_mask = dma_info.base + 0x0C + 2 * 0x400;
    *interrupt_mask = 3;
}

static inline void linkHandlerNm6407Receive2(){
    LinkInfo *info = &link_info;
    if(info->receive_cb[2]){
        info->receive_cb[2](info->receive_cb_data[2]);
    }
    int *interrupt_mask = dma_info.base + 0x1C + 2 * 0x400;
    *interrupt_mask = 3;
}

void __attribute__((optimize("O0"))) dmaHandlerNm6407(){
    
    if(dma_info.receive_cb){
        dma_info.receive_cb(dma_info.receive_cb_data);
    }
    if(dma_info.transfer_cb){
        dma_info.transfer_cb(dma_info.transfer_cb_data);
    }

    
    int *dmatr_interrupt_mask = dma_info.base + 0x0C;
    int *dmarc_interrupt_mask = dma_info.base + 0x1C;
    *dmatr_interrupt_mask = 3;
    *dmarc_interrupt_mask = 3;

    int tmp = dma_info.base[0];
    tmp++;
    dma_info.ireserved = tmp;    


    int *IASH_CLR = (int*)0x4000045C;
    *IASH_CLR = 1;
    
}

void dmaHandlerErrorNm6407(){
    
    int *IASH_CLR = (int*)0x4000045C;
    *IASH_CLR = 1;
    
    int *dmatr_interrupt_mask = dma_info.base + 0x0C;
    int *dmarc_interrupt_mask = dma_info.base + 0x1C;
    *dmatr_interrupt_mask = 3;
    *dmarc_interrupt_mask = 3;
}

int checkDmaPacket(DtpAsync *cmd){
    int correct_addr = ((int)cmd->buf) & 0xF;
    int correct_size = ((int)cmd->nwords) & 0xF;
    int result = correct_addr | correct_size;
    if(cmd->type == DTP_TASK_2D){
        int correct_width = ((int)cmd->width) & 0xF;
        int correct_stride = ((int)cmd->width) & 0xF;
        result |= correct_width;
        result |= correct_stride;
    } 
    return result;
}




static inline int dmaLinkStatus2DtpStatus(int *base){
    int *control = base + 0x0A;
    int status = *control;
    if(status & 4){
        *control = 0;
        return DTP_ST_ERROR;  
    } 
    if(status & 2){
        *control = 0;
        return DTP_ST_DONE;  
    }
    return DTP_ST_IN_PROCESS;
}


static inline void dmaLinkSetTransferRegisters(int *base, DtpAsync *cmd){
// Поскольку байтовые внешние комуникационные порты и пдп имеют один интерфес и отличаются только базовым адресом, то функция
// задания параметров копирования у них одинаковая. Флаги прерываний задаются в отдельных функциях
    base[TR_MAIN_COUNTER] = cmd->nwords >> 1;    
    base[TR_ADDRESS]      = (int)cmd->buf;
    base[TR_BIAS]         = (cmd->stride - (cmd->width - 2) ) >> 1;        
    base[TR_ROW_COUNTER]  = cmd->width >> 1;             
    base[TR_ADDRESS_MODE] = (cmd->type == DTP_TASK_1D) ? 0: 1;
}

static inline void dmaLinkSetReceiveRegisters(int *base, DtpAsync *cmd){
// Поскольку байтовые внешние комуникационные порты и пдп имеют один интерфес и отличаются только базовым адресом, то функция
// задания параметров копирования у них одинаковая. Флаги прерываний задаются в отдельных функциях
    base[RC_MAIN_COUNTER] = cmd->nwords >> 1;                               // MainCounter
    base[RC_ADDRESS]      = (int)cmd->buf;                                  // Addr
    base[RC_BIAS]         = (cmd->stride - (cmd->width - 2) ) >> 1;         // Bias
    base[RC_ROW_COUNTER]  = cmd->width >> 1;                                // RowCounter
    base[RC_ADDRESS_MODE] = (cmd->type == DTP_TASK_1D) ? 0: 1;              // AddressMode
}



static inline void dmaLinkStartTransfer(int *base){
    base[TR_CONTROL] = 0;
    base[TR_CONTROL] = 1;
}

static inline void dmaLinkStartReceive(int *base){
    base[RC_CONTROL] = 0;
    base[RC_CONTROL] = 1;
}

static inline void linkSetIntReceive(int *base, DtpAsync *cmd){
    base[RC_INTERRUPT_MASK] = (cmd->callback == 0) ? 1: 0;
}

static inline void linkSetIntTransfer(int *base, DtpAsync *cmd){
    base[TR_INTERRUPT_MASK] = (cmd->callback == 0) ? 1: 0;
}

static inline void dmaSetInt(int *base, DtpAsync *cmd){
// Если хоть какой-то канал dma задал callback, то прерывание разрешается
    int old_value = base[RC_CONTROL];
    int value = (cmd->callback == 0) ? 1: 0; 
    base[TR_INTERRUPT_MASK] = old_value & value;
    base[RC_INTERRUPT_MASK] = old_value & value;
}
