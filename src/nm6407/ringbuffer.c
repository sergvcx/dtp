#include "stddef.h"
#include <stdint.h>
#include "dtp/ringbuffer.h"
#include "malloc.h"
#include "stdio.h"
#include "dtp-core.h"

#include "dtp-nm6407-core.h"



void dtpCopyRisc(int *src, int* dst, int size){
    for(int i = 0; i < size; i++){
        dst[i] = src[i];
    }
}

int dtpRingBufferGetSizeOfElem(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


DtpRingBuffer32 *dtpRingBufferCreate(void *data, int capacity){
    DtpRingBuffer32 *rb = (DtpRingBuffer32 *)malloc(sizeof(DtpRingBuffer32));
    rb->data = (int*)data;
    rb->capacity = capacity;
    rb->read_semaphore = 0;
    rb->write_semaphore = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->is_inited = 1;
    return rb;
}

int dtpRingBufferGetTail(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->tail;
}

int dtpRingBufferGetHead(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head;
}
int *dtpRingBufferGetPtr(DtpRingBuffer32 *ring_buffer, int index){
    return ring_buffer->data + index % ring_buffer->capacity;
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
        int first_part = ring_buffer->capacity - head % ring_buffer->capacity;
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
        int first_part = ring_buffer->capacity - tail % ring_buffer->capacity;
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

