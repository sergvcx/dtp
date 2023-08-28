#include "dtp/dtp.h"
#include "stdio.h"
#include "malloc.h"

#include "hal/ringbuffert.h"
#include "string.h"
#include "dtp/mc12101.h"

#include "ringbuffer.h"

const int DTP_MC12101_HOST_MESSAGE_SIZE = 2;
const int DTP_MC12101_STATUS_COUNT = 8;

struct PloadTargetData{
    DtpAsync *pool[DTP_MC12101_STATUS_COUNT];
    int status[DTP_MC12101_STATUS_COUNT];
    int desc;
    int index;    
};

static int dtpOpenPloadTargetImplListen(void *com_spec){
    PloadTargetData *data = (PloadTargetData *)com_spec;
    return dtpListen(data->desc);
}

static int dtpOpenPloadTargetGetStatus(void *com_spec, DtpAsync *aio){
    PloadTargetData *data = (PloadTargetData *)com_spec;

    for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
        if(aio == data->pool[i]){
            if(data->status[i] == DTP_ST_WAIT_ACCEPT)
                data->status[i] = DTP_ST_DONE;
            return data->status[i];
        }
    }
    return DTP_ST_ERROR;
}

static int dtpOpenPloadTargetSendAddr(void *com_spec, DtpAsync *aio){
    PloadTargetData *data = (PloadTargetData *)com_spec;

    int message[2];
    bool founded = false;
    message[0] = (int)aio->buf;
    printf("message[0] 0x%x\n", message[0]);
    while(!founded){
        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            if(data->status[i] == DTP_ST_DONE){
                data->status[i] = DTP_ST_IN_PROCESS;
                data->pool[i] = aio;
                message[1] = (int)(&data->status[i]);
                founded = true;
                break;
            }
        }
    }
    if(founded){
        dtpSend(data->desc, message, 2);
        return DTP_OK;
    } else {
        return DTP_AGAIN;
    }
}

int dtpOpenPloadTargetImplDestroy(void *com_spec){
    PloadTargetData *data = (PloadTargetData *)com_spec;
    dtpClose(data->desc);
    free(data);
}


int dtpOpenPloadTarget(int index){
    PloadTargetData *com_spec = (PloadTargetData *)malloc(sizeof(PloadTargetData));
    if(com_spec == 0) return -1;

    com_spec->index = 0;
    for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
        com_spec->status[i] = DTP_ST_DONE;
        com_spec->pool[i] = 0;
    }
    com_spec->desc = dtpConnectSharedBuffer(index);

    DtpImplementation impl;
    impl.recv = dtpOpenPloadTargetSendAddr;
    impl.send = dtpOpenPloadTargetSendAddr;    
    impl.get_status = dtpOpenPloadTargetGetStatus;
    impl.destroy = dtpOpenPloadTargetImplDestroy;
    return dtpOpenCustom(com_spec, &impl);
}





struct Mc12101PloadFile{
    FILE *file;
    DtpAsync *pool[DTP_MC12101_STATUS_COUNT];
    int status[DTP_MC12101_STATUS_COUNT];
};

int ncl_getProcessorNo(void);



static int mc12101Send(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];

    message[0] = (int)aio->buf;
    

    bool founded = false;
    while(!founded){
        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            if(data->status[i] == DTP_ST_DONE){
                data->status[i] = DTP_ST_IN_PROCESS;
                data->pool[i] = aio;
                message[1] = (int)(&data->status[i]);
                founded = true;
                break;
            }
        }
    }

    fwrite(message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, file);
    fflush(file);

    return DTP_OK;
}

static int mc12101Recv(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    volatile int status = 0;

    message[0] = (int)aio->buf;
    
    
    bool founded = false;
    while(!founded){
        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            if(data->status[i] == DTP_ST_DONE){
                data->status[i] = DTP_ST_IN_PROCESS;
                data->pool[i] = aio;
                message[1] = (int)(&data->status[i]);
                founded = true;
                break;
            }
        }
    }

    fwrite(message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, file);
    fflush(file);

    return DTP_OK;
}

static int mc12101Status(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int ind = -1;
    for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
        if(aio == data->pool[i]){
            ind = i;
            break;
        }
    }
    if(ind < 0) return DTP_ST_ERROR;

    if(data->status[ind] == DTP_ST_IN_PROCESS) return DTP_ST_IN_PROCESS;

    if(data->status[ind] == DTP_ST_DONE){
        if(aio->callback){
            aio->callback(aio->cb_data);
        }
        data->pool[ind] = 0;    
    }
    return DTP_ST_DONE;
}

static int mc12101Destroy(void *com_spec){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    //FILE *file = (FILE *)data->file;
    fclose(data->file);
    //delete data;
    return 0;
}

static Mc12101PloadFile Com_spec;
extern "C"{

    int dtpOpenPloadFile(const char *filename){
        //Mc12101PloadFile *com_spec = new Mc12101PloadFile();
        Mc12101PloadFile *com_spec = &Com_spec;
        if(com_spec == 0) return -1;

        com_spec->file = fopen(filename, "wb");
        if(com_spec->file == 0) {
           // delete com_spec;
            return -1;
        }

        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            com_spec->status[i] = DTP_ST_DONE;
            com_spec->pool[i] = 0;
        }

        DtpImplementation impl;
        impl.send = mc12101Send;
        impl.recv = mc12101Recv;
        impl.get_status = mc12101Status;
        impl.destroy = mc12101Destroy;
        return dtpOpenCustom(com_spec, &impl);  
    }


    static int halRbRecv(void *com_spec, DtpAsync *task){
        HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *)com_spec;
        //printf("pop\n");
        //printf("task->nwords %d\n", task->nwords);
        connector->pop((int*)task->buf, task->nwords);
        //printf("end pop\n");
        return 0;
    }
    static int halRbSend(void *com_spec, DtpAsync *task){
        HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *)com_spec;
        //printf("push\n");
        //printf("task->nwords %d\n", task->nwords);
        connector->push((int*)task->buf, task->nwords);
        //printf("end push\n");
        return 0;
    }

    static int halRbStatus(void *com_spec, DtpAsync *task){
        return DTP_ST_DONE;
    }
    static int halRbDestroy(void *com_spec){
        //BoardData *board= (BoardData *) board;
    	free(com_spec);
        return 0;
    }


//static HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> ringbuffer_connector;

int dtpOpenRingbuffer(void *hal_ring_buffer, DtpMemCopyFuncT push_func, DtpMemCopyFuncT pop_func){
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector =  (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *) malloc( sizeof( HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32>));

    connector->init((HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *) hal_ring_buffer, push_func, pop_func);

    DtpImplementation impl;
	impl.recv=halRbRecv;
	impl.send=halRbSend;
	impl.get_status=halRbStatus;
    return dtpOpenCustom(connector, &impl);
}

    static void* memCopyDefault(const void *src, void *dst, unsigned int size32){
        memcpy(dst, src, size32 * sizeof(int));
        return 0;
    }

    // void halSleep(int msec){

    // }
// where  free/delete ???
    int dtpOpenRingbufferDefault(void *hal_ring_buffer){
        //HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = new HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32>();
        HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *) malloc( sizeof( HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32>));

        connector->init((HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *) hal_ring_buffer, memCopyDefault, memCopyDefault);
    	printf("bufferId :%x\n",connector->_bufferId);
    	printf("sizeOfInt:%d\n",connector->sizeofBufferInt);
    	printf("size     :%d\n",connector->size);
    	printf("size1    :%d\n",connector->size1);
    	printf("data     :%x\n",connector->data);

        DtpImplementation impl;
    	impl.recv=halRbRecv;
    	impl.send=halRbSend;
    	impl.get_status=halRbStatus;
        impl.destroy = halRbDestroy;
        return dtpOpenCustom(connector, &impl);
    }


extern int dtp_buffer0_data_in [DTP_BUFFER_SIZE];
extern int dtp_buffer0_data_out[DTP_BUFFER_SIZE];
extern int dtp_buffer1_data_in [DTP_BUFFER_SIZE];
extern int dtp_buffer1_data_out[DTP_BUFFER_SIZE];
extern int dtp_buffer2_data_in [DTP_BUFFER_SIZE];
extern int dtp_buffer2_data_out[DTP_BUFFER_SIZE];
extern int dtp_buffer3_data_in [DTP_BUFFER_SIZE];
extern int dtp_buffer3_data_out[DTP_BUFFER_SIZE];

  

}