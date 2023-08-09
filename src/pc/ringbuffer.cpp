#include "hal/ringbuffert.h"
#include "mc12101load.h"
#include "dtp/dtp.h"
#include "dtp/mc12101-host.h"

static PL_Access *global_access = 0;


struct BoardData{
    PL_Access *pload_access;
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> connector;
};

extern "C"{
    static void *rbMemCopyPop(const void *src, void *dst, unsigned int size32){
        PL_ReadMemBlock(global_access, (PL_Word *)dst, (PL_Addr)src, size32);
        return 0;
    }


    static void *rbMemCopyPush(const void *src, void *dst, unsigned int size32){
        PL_WriteMemBlock(global_access, (PL_Word *)src, (PL_Addr)dst, size32);
        return 0;
    }
}


static int halRbRecv(void *com_spec, DtpAsync *task){
    BoardData *board= (BoardData *) com_spec;
    global_access = board->pload_access;

    board->connector.pop((int*)task->buf, task->nwords);
    return 0;
}
static int halRbSend(void *com_spec, DtpAsync *task){
    BoardData *board= (BoardData *) com_spec;
    global_access = board->pload_access;
    board->connector.push((int*)task->buf, task->nwords);
    return 0;
}

static int halRbStatus(void *com_spec, DtpAsync *task){
    return DTP_ST_DONE;
}
static int halRbDestroy(void *com_spec){
    //BoardData *board= (BoardData *) board;
    return 0;
}

static BoardData board_data;

int dtpOpenPloadRingbuffer(PL_Access *access, uintptr_t hal_ring_buffer_remote_addr){
    HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *ringbuffer_data = (HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *)hal_ring_buffer_remote_addr;

    BoardData *data = &board_data; // new BoardData();
    if(data == 0) return -1;

    data->pload_access = access;
    global_access = access;
    data->connector.init(ringbuffer_data, rbMemCopyPush, rbMemCopyPop);

    DtpImplementation impl;
    impl.recv_func = halRbRecv;
    impl.send_func = halRbSend;
    impl.get_status_func = halRbStatus;
    impl.destroy_func = halRbDestroy;
    return dtpOpenCustom(data, &impl);
}