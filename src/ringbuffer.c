#include "stddef.h"

typedef struct{
    int *data;
    volatile size_t capacity;
    volatile size_t size_of_element;

    volatile size_t read_semaphore;
    volatile size_t write_semaphore;
    volatile size_t head;
    volatile size_t tail;
}  DtpRingBuffer;


void dtpInitRingBuffer(DtpRingBuffer *ring_buffer, void* data, int capacity){
    ring_buffer->data = (int*)data;
    ring_buffer->size_of_element = sizeOfElement;
    ring_buffer->capacity = capacity;
    ring_buffer->read_semaphore = 0;
    ring_buffer->write_semaphore = capacity;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;        
}

void dtpInitRingBuffer(DtpRingBuffer *ring_buffer, void* data, int capacity){
    ring_buffer->data = (int*)data;
    ring_buffer->size_of_element = sizeOfElement;
    ring_buffer->capacity = capacity;
    ring_buffer->read_semaphore = 0;
    ring_buffer->write_semaphore = capacity;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;        
}   

int dtpRingBufferGetTail(DtpRingBuffer *ring_buffer){
    return ring_buffer->sharedData->tail;
}

int dtpRingBufferGetHead(DtpRingBuffer *ring_buffer){
    return ring_buffer->sharedData->head;
}


void dtpRingBufferConsume(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->sharedData->tail += count;
}

void dtpRingBufferProduce(DtpRingBuffer *ring_buffer, int count){
    ring_buffer->sharedData->head += count;
}






void dtpRingBufferPush(DtpRingBuffer *ring_buffer, const void *data, int count){        
    for(int i = 0; i < count; i++){
        dtpRingBufferCapturedWrite(ring_buffer);
        int head = dtpRingBufferGetHead(ring_buffer);
        int *src = (int*)data + ring_buffer->size_of_element * i;
        int *dst = ring_buffer->data + ring_buffer->size_of_element * (head % ring_buffer->capacity);
        dtpCopyRisc(src, dst, ring_buffer->size_of_element);
        dtpRingBufferIncHead(ring_buffer);
        dtpRingBufferReleaseRead(ring_buffer);
    }
}

void dtpRingBufferPop(DtpRingBuffer *ring_buffer, void *data, int count){        
    for(int i = 0; i < count; i++){
        dtpRingBufferCapturedRead(ring_buffer);
        int tail = dtpRingBufferGetTail(ring_buffer);
        int *src = ring_buffer->data + ring_buffer->size_of_element * (tail % ring_buffer->capacity);
        int *dst = (int*)data + ring_buffer->size_of_element * i;            
        dtpCopyRisc(src, dst, ring_buffer->size_of_element);
        dtpRingBufferIncTail(ring_buffer);
        dtpRingBufferReleaseWrite(ring_buffer);
    }
}


int dtpRingBufferIsEmpty(DtpRingBuffer *ring_buffer){
    return ring_buffer->sharedData->read_semaphore <= 0;
}

int dtpRingBufferIsFull(DtpRingBuffer *ring_buffer){
    return ring_buffer->sharedData->write_semaphore <= 0;
}

int dtpRingBufferAvailable(DtpRingBuffer *ring_buffer){
    return ring_buffer->sharedData->head - ring_buffer->sharedData->tail;
}

void dtpRingBufferCapturedRead(DtpRingBuffer *ring_buffer){
    ring_buffer->sharedData->read_semaphore--;
    while(ring_buffer->sharedData->read_semaphore < 0){
        for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }        
}

void dtpRingBufferReleaseRead(DtpRingBuffer *ring_buffer){
    ring_buffer->sharedData->read_semaphore++;
}

void dtpRingBufferCapturedWrite(DtpRingBuffer *ring_buffer){
    ring_buffer->sharedData->write_semaphore--;
    while(ring_buffer->sharedData->write_semaphore < 0){
        for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }
}

void dtpRingBufferReleaseWrite(DtpRingBuffer *ring_buffer){
    ring_buffer->sharedData->write_semaphore++;
}