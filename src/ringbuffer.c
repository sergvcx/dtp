#include "stddef.h"
#include <stdint.h>
#include "ringbuffer.h"

struct DtpRingBuffer{
    int *data;
    volatile size_t capacity;
    volatile size_t size_of_element;

    volatile size_t read_semaphore;
    volatile size_t write_semaphore;
    volatile size_t head;
    volatile size_t tail;
};

void dtpCopyRisc(int *src, int* dst, int size){
    for(int i = 0; i < size / 4; i++){
        dst[i] = src[i];
    }
}


void dtpInitRingBuffer(DtpRingBuffer *ring_buffer, void* data, int size_of_element, int capacity){
    ring_buffer->data = (int*)data;
    ring_buffer->size_of_element = size_of_element;
    ring_buffer->capacity = capacity;
    ring_buffer->read_semaphore = 0;
    ring_buffer->write_semaphore = capacity;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;        
}

void dtpBindRingBuffer(uintptr_t pointer){
    //ring_buffer->data = (int*)data;
    //ring_buffer->size_of_element = size_of_element;
    //ring_buffer->capacity = capacity;
    //ring_buffer->read_semaphore = 0;
    //ring_buffer->write_semaphore = capacity;
    //ring_buffer->head = 0;
    //ring_buffer->tail = 0;        
}

int dtpRingBufferGetTail(DtpRingBuffer *ring_buffer){
    return ring_buffer->tail;
}

int dtpRingBufferGetHead(DtpRingBuffer *ring_buffer){
    return ring_buffer->head;
}


void dtpRingBufferConsume(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->tail += count;
}

void dtpRingBufferProduce(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->head += count;
}

void dtpRingBufferPush(DtpRingBuffer *ring_buffer, const void *data, int count){        
    for(int i = 0; i < count; i++){
        dtpRingBufferCapturedWrite(ring_buffer, 1);
        int head = dtpRingBufferGetHead(ring_buffer);
        int *src = (int*)data + ring_buffer->size_of_element * i;
        int *dst = ring_buffer->data + ring_buffer->size_of_element * (head % ring_buffer->capacity);
        dtpCopyRisc(src, dst, ring_buffer->size_of_element);
        dtpRingBufferProduce(ring_buffer, 1);
        dtpRingBufferReleaseRead(ring_buffer, 1);
    }
}

void dtpRingBufferPop(DtpRingBuffer *ring_buffer, void *data, int count){        
    for(int i = 0; i < count; i++){
        dtpRingBufferCapturedRead(ring_buffer, 1);
        int tail = dtpRingBufferGetTail(ring_buffer);
        int *src = ring_buffer->data + ring_buffer->size_of_element * (tail % ring_buffer->capacity);
        int *dst = (int*)data + ring_buffer->size_of_element * i;            
        dtpCopyRisc(src, dst, ring_buffer->size_of_element);
        dtpRingBufferConsume(ring_buffer, 1);
        dtpRingBufferReleaseWrite(ring_buffer, 1);
    }
}

int dtpRingBufferIsEmpty(DtpRingBuffer *ring_buffer){
    return ring_buffer->read_semaphore <= 0;
}


int dtpRingBufferIsFull(DtpRingBuffer *ring_buffer){
    return ring_buffer->write_semaphore <= 0;
}


int dtpRingBufferAvailable(DtpRingBuffer *ring_buffer){
    return ring_buffer->head - ring_buffer->tail;
}


void dtpRingBufferCapturedRead(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->read_semaphore--;
    while(ring_buffer->read_semaphore < 0){
        //for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }        

}

void dtpRingBufferReleaseRead(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->read_semaphore++;
}


void dtpRingBufferCapturedWrite(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->write_semaphore--;
    while(ring_buffer->write_semaphore < 0){
        //for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }
}

void dtpRingBufferReleaseWrite(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->write_semaphore++;
}

