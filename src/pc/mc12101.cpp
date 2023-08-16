#include "dtp/mc12101-host.h"
#include "mc12101load.h"
#include "dtp/dtp.h"
#include "stdio.h"
#include "hal/ringbuffert.h"

const int DTP_MC12101_HOST_MESSAGE_SIZE = 2;
struct Mc12101PloadFile{
    FILE *message_file;
    PL_Access *access;
};

static void getMessageFromNm(Mc12101PloadFile *data, int *dst_message){
    size_t start = ftell(data->message_file);
	size_t tell;
	do {
		fseek(data->message_file, 0, SEEK_END);
		tell = ftell(data->message_file);
	}
    while (tell - start < DTP_MC12101_HOST_MESSAGE_SIZE * sizeof(int));  //fteel in bytes or words

	fseek(data->message_file, start, SEEK_SET);

    fread(dst_message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, data->message_file);
}

static int mc12101Send(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    getMessageFromNm(data, message);

    PL_WriteMemBlock(data->access, (PL_Word *)aio->buf, message[0], aio->nwords);

    int status = DTP_ST_DONE;
    PL_WriteMemBlock(data->access, (PL_Word *)&status, message[1], 1);

    return DTP_OK;
}

static int mc12101Recv(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    getMessageFromNm(data, message);

    PL_ReadMemBlock(data->access, (PL_Word *)aio->buf, message[0], aio->nwords);

    int status = DTP_ST_DONE;
    PL_WriteMemBlock(data->access, (PL_Word *)&status, message[1], 1);

    return DTP_OK;
}

static int mc12101Status(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    return DTP_ST_DONE;
}

static int mc12101Destroy(void *com_spec){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    return 0;
}

extern "C"{

    int dtpOpenPloadFileHost(PL_Access *access, const char *filename){
        Mc12101PloadFile *com_spec = new Mc12101PloadFile();
        if(com_spec == 0) return -1;

        com_spec->message_file = fopen(filename, "wb+");
        com_spec->access = access;

        if(com_spec->message_file == 0) return -1;

        DtpImplementation impl;
        impl.send = mc12101Send;
        impl.recv = mc12101Recv;
        impl.get_status = mc12101Status;
        impl.destroy = mc12101Destroy;
        impl.connect = 0;
        impl.listen = 0;

        
        return dtpOpenCustom(com_spec, &impl);  
    }

    

}

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

// static BoardData board_data;

// void halSleep(int msec){

// }

int dtpOpenPloadRingbuffer(PL_Access *access, uintptr_t hal_ring_buffer_remote_addr){
    HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *ringbuffer_data = (HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *)hal_ring_buffer_remote_addr;

    BoardData *data = (BoardData *) malloc( sizeof(BoardData) );
    if(data == 0) return -1;

    data->access = access;
    global_access = access;
    data->connector.init(ringbuffer_data, rbMemCopyPush, rbMemCopyPop);

    DtpImplementation impl;
    impl.recv = halRbRecv;
    impl.send = halRbSend;
    impl.get_status = halRbStatus;
    impl.destroy = halRbDestroy;
    impl.connect = 0;
    impl.listen = 0;
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
    impl.send = halRbSend;
    impl.recv = halRbRecv;
    impl.get_status = halRbStatus;
    impl.destroy = halRbDestroy2;
    impl.connect = 0;
    impl.listen = 0;

    return dtpOpenCustom(com_spec, &impl); 
}

