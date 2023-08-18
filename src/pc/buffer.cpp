#include "dtp/buffer.h"
#include "ringbuffer.h"

const uintptr_t ringbuffer = 0xA8000 - 16;

struct RemoteSharedBufferData{
    void *user_data;
    DtpBufferCopyFuncT readFunc;
    DtpBufferCopyFuncT writeFunc;
    DtpRingBuffer32 *rb_in;
    DtpRingBuffer32 *rb_out;
    int index;
};

static int dtpBufferImplRecv(void *com_spec, DtpAsync *cmd){
    RemoteSharedBufferData *data = (RemoteSharedBufferData *)com_spec;
    int size = cmd->nwords;
    int *dst = (int *)cmd->buf;

    if(cmd->type == DTP_TASK_1D){            
        while(size > 0){
            int head = dtpRingBufferGetHead(data->rb_in);
            int tail = dtpRingBufferGetTail(data->rb_in);
            int available = head - tail;                

            int pop_size = (available > size) ? size : available;
            if(pop_size){
                dtpRingBufferPop(data->rb_in, dst, pop_size);                    
                dst += pop_size;
                size -= pop_size;
            }
        }
        if(cmd->sigevent == DTP_EVENT_CALLBACK){
            if(cmd->callback) cmd->callback(cmd->cb_data);
        }
    } else {
        return DTP_ERROR;
    }    
}

static int dtpBufferImplSend(void *com_spec, DtpAsync *cmd){
    RemoteSharedBufferData *data = (RemoteSharedBufferData *)com_spec;
    int size = cmd->nwords;
    int *src = (int *)cmd->buf;

    int capacity = dtpRingBufferGetCapacity(data->rb_out);
    if(cmd->type == DTP_TASK_1D){
        while(size > 0){
            int head = dtpRingBufferGetHead(data->rb_out);
            int tail = dtpRingBufferGetTail(data->rb_out);
            int available = tail + capacity - head;

            int push_size = (available > size) ? size : available;
            if(push_size){
                dtpRingBufferPush(data->rb_out, src, push_size);
                src += push_size;
                size -= push_size;
            }   
        }
        if(cmd->sigevent == DTP_EVENT_CALLBACK){
            if(cmd->callback) cmd->callback(cmd->cb_data);
        }
    } else {
        return DTP_ERROR;
    }    
    return 0;
}

static int dtpBufferImplGetStatus(void *com_spec, DtpAsync *cmd){
    RemoteSharedBufferData *data = (RemoteSharedBufferData *)com_spec;
    return DTP_ST_DONE;
}

static int dtpBufferImplConnect(void *com_spec){
    RemoteSharedBufferData *data = (RemoteSharedBufferData *)com_spec;
    uintptr_t rb = ringbuffer + 2 * data->index;
    int rb_in_addr = 0;
    int rb_out_addr = 0;

    data->readFunc(data->user_data, &rb_out_addr, (int)rb, 1);
    data->readFunc(data->user_data, &rb_in_addr, (int)rb + 1, 1);

    data->rb_in = dtpRingBufferBind(data->user_data, rb_in_addr, data->readFunc, data->writeFunc);
    data->rb_out = dtpRingBufferBind(data->user_data, rb_out_addr, data->readFunc, data->writeFunc);

    int handshake = 0x12300123;
    dtpRingBufferPush(data->rb_out, &handshake, 1);
    dtpRingBufferPop(data->rb_in, &handshake, 1);
    if(handshake != 0x12300124) return -1;
    return 0;
}

static int dtpBufferImplDestroy(void *com_spec){
    RemoteSharedBufferData *data = (RemoteSharedBufferData *)com_spec;
    free(data);
    return 0;
}

int dtpOpenRemoteSharedBuffer(int index, void *user_data, DtpBufferCopyFuncT readFunc, DtpBufferCopyFuncT writeFunc){

    RemoteSharedBufferData *data = (RemoteSharedBufferData *)malloc(sizeof(RemoteSharedBufferData));
    if(data == 0)return -1;

    data->user_data = user_data;
    data->readFunc = readFunc;
    data->writeFunc = writeFunc;
    data->index = index;

    DtpImplementation impl;
    impl.recv = dtpBufferImplRecv;
    impl.send = dtpBufferImplSend;
    impl.get_status = dtpBufferImplGetStatus;
    impl.destroy = dtpBufferImplDestroy;
    impl.listen = 0;
    impl.connect = dtpBufferImplConnect;

    return dtpOpenCustom(data, &impl);
}