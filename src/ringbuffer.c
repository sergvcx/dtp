#include "stddef.h"
#include <stdint.h>
#include "dtp/ringbuffer.h"
#include "malloc.h"
#include "stdio.h"

typedef struct{
    DtpRingBuffer32 *ringbuffer;
} DtpShmemData;

static size_t shmemRead(void *user_data, void *buf, size_t size){
    DtpShmemData *data = (DtpShmemData *)user_data;

    int available32 = dtpRingBufferGetHead(data->ringbuffer) - dtpRingBufferGetTail(data->ringbuffer);

    available32 = (available32 < size / 4) ? available32 : size / 4;

    dtpRingBufferPop(data->ringbuffer, buf, available32);

    return available32 * 4;

}

static size_t shmemWrite(void *user_data, const void *buf, size_t size){
    DtpShmemData *data = (DtpShmemData *)user_data;

    int available32 = dtpRingBufferGetTail(data->ringbuffer) + dtpRingBufferGetCapacity(data->ringbuffer) - dtpRingBufferGetHead(data->ringbuffer);

    available32 = (available32 < size / 4) ? available32 : size / 4;    

    dtpRingBufferPush(data->ringbuffer, buf, available32);
    
    return available32 * 4;
}

static int shmemClose(void *user_data){
    DtpShmemData *data = (DtpShmemData *)user_data;
    free(data);
    return 0;
};



int dtpOpenShmem(DtpRingBuffer32 *ringbuffer){
    DtpShmemData *shmemData = (DtpShmemData *)malloc(sizeof(DtpShmemData));
    if(shmemData == 0){
        return -1;
    }
    shmemData->ringbuffer = ringbuffer;
    DtpImplementaion impl;
    impl.write = shmemWrite;
    impl.read = shmemRead;
    impl.close = shmemClose;
    impl.flush = 0;
    int fd = dtpOpen(shmemData, &impl);
    return fd;
}

void dtpCopyRisc(int *src, int* dst, int size){
    for(int i = 0; i < size; i++){
        dst[i] = src[i];
    }
}

int dtpRingBufferGetSizeOfElem(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


void dtpInitRingBuffer(DtpRingBuffer32 *ring_buffer, void* data, int capacity){
    ring_buffer->data = (int*)data;
    ring_buffer->capacity = capacity;
    ring_buffer->read_semaphore = 0;
    ring_buffer->write_semaphore = capacity;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;        
}

int dtpRingBufferGetTail(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->tail;
}

int dtpRingBufferGetHead(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head;
}

int dtpRingBufferGetCapacity(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


void dtpRingBufferConsume(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->tail += count;
}

void dtpRingBufferProduce(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->head += count;
}

void dtpRingBufferPush(DtpRingBuffer32 *ring_buffer, const void *data, int count){
    dtpRingBufferCapturedWrite(ring_buffer, count);
    int head = dtpRingBufferGetHead(ring_buffer);
    if(head % ring_buffer->capacity + count < ring_buffer->capacity){
        int *src = (int*)data;
        int *dst = ring_buffer->data + head % ring_buffer->capacity;
        dtpCopyRisc(src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - head % ring_buffer->capacity;
        int *src = (int*)data;
        int *dst = ring_buffer->data + head % ring_buffer->capacity;            
        dtpCopyRisc(src, dst, first_part);

        int second_part = count - first_part;
        src = (int*)data + first_part;
        dst = ring_buffer->data;
        dtpCopyRisc(src, dst, second_part);
    }
    dtpRingBufferProduce(ring_buffer, count);    
    dtpRingBufferReleaseRead(ring_buffer, count);
}

void dtpRingBufferPop(DtpRingBuffer32 *ring_buffer, void *data, int count){        
    dtpRingBufferCapturedRead(ring_buffer, count);   
    int tail = dtpRingBufferGetTail(ring_buffer);
    if(tail % ring_buffer->capacity + count < ring_buffer->capacity){
        int *src = ring_buffer->data + tail % ring_buffer->capacity;
        int *dst = (int*)data;        
        dtpCopyRisc(src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - tail % ring_buffer->capacity;
        int *src = ring_buffer->data + tail % ring_buffer->capacity;            
        int *dst = (int*)data;        
        dtpCopyRisc(src, dst, first_part);

        int second_part = count - first_part;
        src = ring_buffer->data;
        dst = (int*)data + first_part;
        dtpCopyRisc(src, dst, second_part);
    }
    dtpRingBufferConsume(ring_buffer, count);
    dtpRingBufferReleaseWrite(ring_buffer, count);
}

int dtpRingBufferIsEmpty(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->read_semaphore <= 0;
}


int dtpRingBufferIsFull(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->write_semaphore <= 0;
}


int dtpRingBufferAvailable(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head - ring_buffer->tail;
}


void dtpRingBufferCapturedRead(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->read_semaphore--;
    while(ring_buffer->read_semaphore < 0){
        //for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }        

}

void dtpRingBufferReleaseRead(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->read_semaphore++;
}


void dtpRingBufferCapturedWrite(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->write_semaphore--;
    while(ring_buffer->write_semaphore < 0){
        //for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }
}

void dtpRingBufferReleaseWrite(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->write_semaphore++;
}

