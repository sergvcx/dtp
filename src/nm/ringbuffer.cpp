#include "stddef.h"
#include <stdint.h>
#include "malloc.h"
#include "stdio.h"
#include "dtp/mc12101.h"
#include "hal/ringbuffert.h"
#include "dtp/dtp.h"
#include "string.h"

#include "ringbuffer.h"

#include "nmsemaphore.h"


typedef struct DtpRingBuffer32{
    int *data;
    volatile size_t capacity;

    dtp_sem_t read_semaphore;
    dtp_sem_t write_semaphore;
    volatile size_t head;
    volatile size_t tail;
    volatile int is_inited;
} DtpRingBuffer32;


void dtpCopyRisc(int *src, int* dst, int size){
    for(int i = 0; i < size; i++){
        dst[i] = src[i];
    }
}



int dtpRingBufferGetSizeOfElem(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


DtpRingBuffer32 *dtpRingBufferAlloc(void *data, int capacity){
    DtpRingBuffer32 *rb = (DtpRingBuffer32 *)malloc(sizeof(DtpRingBuffer32));
    dtpRingBufferInit(rb, data, capacity);
    return rb;
}

void dtpRingBufferInit(DtpRingBuffer32 *ringbuffer, void *data, int capacity){
    ringbuffer->data = (int*)data;
    ringbuffer->capacity = capacity;
    dtp_sem_init(&ringbuffer->read_semaphore, 0);
    dtp_sem_init(&ringbuffer->write_semaphore, capacity);
    //dtp_sem_init(&ringbuffer->write_semaphore, 1);
    int value_read;
    int value_write;
    dtp_sem_getvalue(&ringbuffer->write_semaphore, &value_write);
    dtp_sem_getvalue(&ringbuffer->read_semaphore, &value_read);    
    ringbuffer->head = 0;
    ringbuffer->tail = 0;
    ringbuffer->is_inited = 1;
}


void dtpRingBufferFree(DtpRingBuffer32 *rb){
    free(rb);
}

int dtpRingBufferGetTail(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->tail;
}

int dtpRingBufferGetHead(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head;
}
int *dtpRingBufferGetPtr(DtpRingBuffer32 *ring_buffer, int index){
    return ring_buffer->data + (index & (ring_buffer->capacity - 1));
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

int dtpRingBufferPush(DtpRingBuffer32 *ring_buffer, const void *data, int count){
    int *src = (int*)data;
    for(int i = 0; i < count; i++){
        //dtp_sem_wait(&ring_buffer->write_semaphore);
        int head = dtpRingBufferGetHead(ring_buffer);
        int *dst = dtpRingBufferGetPtr(ring_buffer, head);
        *dst = *src;
        src++;
        dtpRingBufferProduce(ring_buffer, 1);
        //dtp_sem_post(&ring_buffer->read_semaphore);
    }
    return 0;
}

int dtpRingBufferPop(DtpRingBuffer32 *ring_buffer, void *data, int count){        
    int *dst = (int*)data;
    for(int i = 0; i < count; i++){
        int read;
        //dtp_sem_getvalue(&ring_buffer->read_semaphore, &read);
        //printf("read = %d\n", read);
        //dtp_sem_wait(&ring_buffer->read_semaphore);
        int tail = dtpRingBufferGetTail(ring_buffer);
        int *src = dtpRingBufferGetPtr(ring_buffer, tail);
        *dst = *src;
        dst++;
        dtpRingBufferConsume(ring_buffer, 1);
        //dtp_sem_post(&ring_buffer->write_semaphore);
    }
    return 0;
}

int dtpRingBufferIsEmpty(DtpRingBuffer32 *ring_buffer){
    int value;
    dtp_sem_getvalue(&ring_buffer->read_semaphore, &value);
    return value <= 0;
}


int dtpRingBufferIsFull(DtpRingBuffer32 *ring_buffer){    
    int value;
    dtp_sem_getvalue(&ring_buffer->write_semaphore, &value);
    return value <= 0;
}


int dtpRingBufferAvailable(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head - ring_buffer->tail;
}


void dtpRingBufferCapturedRead(DtpRingBuffer32 *ring_buffer, int count){    
    for(int i = 0; i < count; i++){
        dtp_sem_wait(&ring_buffer->read_semaphore);
    }
}

void dtpRingBufferReleaseRead(DtpRingBuffer32 *ring_buffer, int count){    
    for(int i = 0; i < count; i++){
        dtp_sem_post(&ring_buffer->read_semaphore);
    }
}


void dtpRingBufferCapturedWrite(DtpRingBuffer32 *ring_buffer, int count){    
    for(int i = 0; i < count; i++){
        dtp_sem_wait(&ring_buffer->write_semaphore);
    }
}

void dtpRingBufferReleaseWrite(DtpRingBuffer32 *ring_buffer, int count){    
    for(int i = 0; i < count; i++){
        dtp_sem_post(&ring_buffer->write_semaphore);
    }
}



