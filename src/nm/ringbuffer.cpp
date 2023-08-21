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

    nm_sem_t read_semaphore;
    nm_sem_t write_semaphore;
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
    nm_sem_init(&ringbuffer->read_semaphore, 0);
    //nm_sem_init(&rb->write_semaphore, capacity);
    nm_sem_init(&ringbuffer->write_semaphore, 1);
    int value_read;
    int value_write;
    nm_sem_getvalue(&ringbuffer->write_semaphore, &value_write);
    nm_sem_getvalue(&ringbuffer->read_semaphore, &value_read);    
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

void dtpRingBufferPush(DtpRingBuffer32 *ring_buffer, const void *data, int count){
    dtpRingBufferCapturedWrite(ring_buffer, count);
    int head = dtpRingBufferGetHead(ring_buffer);
    if(head % ring_buffer->capacity + count < ring_buffer->capacity){
        int *src = (int*)data;
        int *dst = dtpRingBufferGetPtr(ring_buffer, head);
        dtpCopyRisc(src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - (head & (ring_buffer->capacity - 1));
        int *src = (int*)data;
        int *dst = dtpRingBufferGetPtr(ring_buffer, head);
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
        int *src = dtpRingBufferGetPtr(ring_buffer, tail);
        int *dst = (int*)data;        
        dtpCopyRisc(src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - (tail & (ring_buffer->capacity - 1));
        int *src = dtpRingBufferGetPtr(ring_buffer, tail); 
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
    int value;
    nm_sem_getvalue(&ring_buffer->read_semaphore, &value);
    return value <= 0;
}


int dtpRingBufferIsFull(DtpRingBuffer32 *ring_buffer){    
    int value;
    nm_sem_getvalue(&ring_buffer->write_semaphore, &value);
    return value <= 0;
}


int dtpRingBufferAvailable(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head - ring_buffer->tail;
}


void dtpRingBufferCapturedRead(DtpRingBuffer32 *ring_buffer, int count){
    nm_sem_wait(&ring_buffer->read_semaphore);
}

void dtpRingBufferReleaseRead(DtpRingBuffer32 *ring_buffer, int count){
    nm_sem_post(&ring_buffer->read_semaphore);
}


void dtpRingBufferCapturedWrite(DtpRingBuffer32 *ring_buffer, int count){
    nm_sem_wait(&ring_buffer->write_semaphore);
}

void dtpRingBufferReleaseWrite(DtpRingBuffer32 *ring_buffer, int count){
    nm_sem_post(&ring_buffer->write_semaphore);
}



