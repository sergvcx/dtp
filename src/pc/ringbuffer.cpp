#include "hal/ringbuffert.h"
#include "mc12101load.h"
#include "dtp/dtp.h"
#include "dtp/mc12101-host.h"
#include "mc12101load.h"

static PL_Access *global_access = 0;


struct BoardData{
    PL_Access *access;

    PL_Board *board;
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
    global_access = board->access;

    board->connector.pop((int*)task->buf, task->nwords);
    return 0;
}
static int halRbSend(void *com_spec, DtpAsync *task){
    BoardData *board= (BoardData *) com_spec;
    global_access = board->access;
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

static int halRbDestroy2(void *com_spec){
    BoardData *board= (BoardData *) com_spec;
    int ok0 = PL_CloseAccess(board->access);
    int ok1 = PL_CloseBoardDesc(board->board);

    return ok1;
}

static BoardData board_data;

void halSleep(int msec){
    
}

int dtpOpenPloadRingbuffer(PL_Access *access, uintptr_t hal_ring_buffer_remote_addr){
    HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *ringbuffer_data = (HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *)hal_ring_buffer_remote_addr;

    BoardData *data = &board_data; // new BoardData();
    if(data == 0) return -1;

    data->access = access;
    global_access = access;
    data->connector.init(ringbuffer_data, rbMemCopyPush, rbMemCopyPop);

    DtpImplementation impl;
    impl.recv_func = halRbRecv;
    impl.send_func = halRbSend;
    impl.get_status_func = halRbStatus;
    impl.destroy_func = halRbDestroy;
    return dtpOpenCustom(data, &impl);
}

int dtpOpenMc12101Ringbuffer(int boardIndex, int coreIndex, uintptr_t hal_ring_buffer_remote_addr){
    HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *ringbuffer_data = (HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *)hal_ring_buffer_remote_addr;

    BoardData *com_spec = new BoardData();
    if(com_spec == 0) return -1;

    unsigned int count;
    PL_GetBoardCount(&count);

    if (count <= boardIndex) return -1;
        
    int ok = PL_GetBoardDesc(boardIndex, &com_spec->board);
    if(ok) return -1;

    ok = PL_GetAccess(com_spec->board, coreIndex, &com_spec->access);
    if(ok){
        PL_CloseBoardDesc(com_spec->board);
        return -1;
    }

    global_access = com_spec->access;
    com_spec->connector.init(ringbuffer_data, rbMemCopyPush, rbMemCopyPop);

    DtpImplementation impl;
    impl.send_func = halRbSend;
    impl.recv_func = halRbRecv;
    impl.get_status_func = halRbStatus;
    impl.destroy_func = halRbDestroy2;

    return dtpOpenCustom(com_spec, &impl); 
}
