#include "dtp/ringbuffer.h"
#include "dtp-core.h"

size_t shmemRead(void *user_data, void *buf, size_t size){
    DtpShmemData *data = (DtpShmemData *)user_data;

    int available32 = dtpRingBufferGetHead(data->ringbuffer) - dtpRingBufferGetTail(data->ringbuffer);
    if(available32 == 0) return 0;

    available32 = (available32 < size / 4) ? available32 : size / 4;

    dtpRingBufferPop(data->ringbuffer, buf, available32);

    return available32 * 4;

}

size_t shmemWrite(void *user_data, const void *buf, size_t size){
    DtpShmemData *data = (DtpShmemData *)user_data;

    int available32 = dtpRingBufferGetTail(data->ringbuffer) + dtpRingBufferGetCapacity(data->ringbuffer) - dtpRingBufferGetHead(data->ringbuffer);
    if(available32 == 0) return 0;

    available32 = (available32 < size / 4) ? available32 : size / 4;    

    dtpRingBufferPush(data->ringbuffer, buf, available32);
    
    return available32 * 4;
}

int shmemClose(void *user_data){
    DtpShmemData *data = (DtpShmemData *)user_data;
    free(data);
    return 0;
};



int dtpOpenBuffer(DtpRingBuffer32 *ringbuffer){
    DtpShmemData *shmemData = (DtpShmemData *)malloc(sizeof(DtpShmemData));
    if(shmemData == 0){
        return -1;
    }
    shmemData->ringbuffer = ringbuffer;
    DtpImplementation impl;
    impl.send = shmemWrite;
    impl.recv = shmemRead;
    impl.destroy = shmemClose;
    impl.flush = 0;
    int fd = dtpOpen(shmemData, &impl);
    return fd;
}
