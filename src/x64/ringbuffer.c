#include "stddef.h"
#include <stdint.h>
#include "dtp/ringbuffer.h"
#include "malloc.h"
#include "stdio.h"
#include "dtp-core.h"
#include "hal/hal.h"
#include "malloc.h"
#include "dtp/dtp-hal.h"
#include "hal/sleep.h"


typedef struct DtpRingBuffer32{
    uintptr_t data_addr;
    size_t capacity;

    uintptr_t read_semaphore_addr;
    uintptr_t write_semaphore_addr;
    uintptr_t head_addr;
    uintptr_t tail_addr;
    HalAccess *access;
} DtpRingBuffer32;



void dtpCopyRisc(int *src, int* dst, int size){
    for(int i = 0; i < size; i++){
        dst[i] = src[i];
    }
}

int dtpRingBufferGetSizeOfElem(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


DtpRingBuffer32 *dtpRingBufferBind(HalAccess *access, uintptr_t remoteRingBuffer){
    DtpRingBuffer32 *result = (DtpRingBuffer32*)malloc(sizeof(DtpRingBuffer32));
    if(result == 0) return 0;
    result->data_addr = 0;
    halReadMemBlock(access, &result->data_addr, remoteRingBuffer, 1);
    halReadMemBlock(access, &result->capacity, remoteRingBuffer + 1, 1);
    result->read_semaphore_addr = remoteRingBuffer + 2;
    result->write_semaphore_addr = remoteRingBuffer + 3;
    result->head_addr = remoteRingBuffer + 4;
    result->tail_addr = remoteRingBuffer + 5;
    result->access = access;
    return result;

}

int dtpRingBufferGetTail(DtpRingBuffer32 *ring_buffer){
    int tail = 0;
    halReadMemBlock(ring_buffer->access, &tail, ring_buffer->tail_addr, 1);
    return tail;
}

int dtpRingBufferGetHead(DtpRingBuffer32 *ring_buffer){
    int head = 0;
    halReadMemBlock(ring_buffer->access, &head, ring_buffer->head_addr, 1);
    return head;
}
uintptr_t dtpRingBufferGetPtr(DtpRingBuffer32 *ring_buffer, int index){
    return ring_buffer->data_addr + index % ring_buffer->capacity;
}

int dtpRingBufferGetCapacity(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


void dtpRingBufferConsume(DtpRingBuffer32 *ring_buffer, int count){
    int tail = dtpRingBufferGetTail(ring_buffer);
    tail += count;
    halWriteMemBlock(ring_buffer->access, &tail, ring_buffer->tail_addr, 1);
}

void dtpRingBufferProduce(DtpRingBuffer32 *ring_buffer, int count){
    int head = dtpRingBufferGetHead(ring_buffer);
    head += count;
    halWriteMemBlock(ring_buffer->access, &head, ring_buffer->head_addr, 1);
}

void dtpRingBufferPush(DtpRingBuffer32 *ring_buffer, const void *data, int count){
    dtpRingBufferCapturedWrite(ring_buffer, count);
    int head = dtpRingBufferGetHead(ring_buffer);
    if(head % ring_buffer->capacity + count < ring_buffer->capacity){
        int *src = (int*)data;
        uintptr_t dst = dtpRingBufferGetPtr(ring_buffer, head);
        halWriteMemBlock(ring_buffer->access, src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - head % ring_buffer->capacity;
        int *src = (int*)data;
        uintptr_t dst = dtpRingBufferGetPtr(ring_buffer, head);
        halWriteMemBlock(ring_buffer->access, src, dst, first_part);

        int second_part = count - first_part;
        src = (int*)data + first_part;
        dst = dtpRingBufferGetPtr(ring_buffer, 0);
        halWriteMemBlock(ring_buffer->access, src, dst, second_part);
    }
    dtpRingBufferProduce(ring_buffer, count);    
    dtpRingBufferReleaseRead(ring_buffer, count);
}

void dtpRingBufferPop(DtpRingBuffer32 *ring_buffer, void *data, int count){        
    dtpRingBufferCapturedRead(ring_buffer, count);   
    int tail = dtpRingBufferGetTail(ring_buffer);
    if(tail % ring_buffer->capacity + count < ring_buffer->capacity){
        uintptr_t src = dtpRingBufferGetPtr(ring_buffer, tail);
        int *dst = (int*)data;    
        halReadMemBlock(ring_buffer->access, dst, src, count);
    } else {
        int first_part = ring_buffer->capacity - tail % ring_buffer->capacity;
        uintptr_t src = dtpRingBufferGetPtr(ring_buffer, tail); 
        int *dst = (int*)data;        
        halReadMemBlock(ring_buffer->access, dst, src, count);        

        int second_part = count - first_part;
        src = dtpRingBufferGetPtr(ring_buffer, 0);
        dst = (int*)data + first_part;
        halReadMemBlock(ring_buffer->access, dst, src, count);        
    }
    dtpRingBufferConsume(ring_buffer, count);
    dtpRingBufferReleaseWrite(ring_buffer, count);
}


int dtpRingBufferAvailable(DtpRingBuffer32 *ring_buffer){
    return dtpRingBufferGetHead(ring_buffer) - dtpRingBufferGetTail(ring_buffer);
}


void dtpRingBufferCapturedRead(DtpRingBuffer32 *ring_buffer, int count){
    int sem = 0;
    do{
        halReadMemBlock(ring_buffer->access, &sem, ring_buffer->read_semaphore_addr, 1);
        halSleep(2);
    } while(sem == 0);
    sem--;
    halWriteMemBlock(ring_buffer->access, &sem, ring_buffer->read_semaphore_addr, 1);
}

void dtpRingBufferReleaseRead(DtpRingBuffer32 *ring_buffer, int count){
    int sem = 0;
    halReadMemBlock(ring_buffer->access, &sem, ring_buffer->read_semaphore_addr, 1);
    sem++;
    halWriteMemBlock(ring_buffer->access, &sem, ring_buffer->read_semaphore_addr, 1);    
}


void dtpRingBufferCapturedWrite(DtpRingBuffer32 *ring_buffer, int count){
    int sem = 0;
    do{
        halReadMemBlock(ring_buffer->access, &sem, ring_buffer->write_semaphore_addr, 1);
        halSleep(2);
    } while(sem == 0);
    sem--;
    halWriteMemBlock(ring_buffer->access, &sem, ring_buffer->write_semaphore_addr, 1);
}

void dtpRingBufferReleaseWrite(DtpRingBuffer32 *ring_buffer, int count){
    int sem = 0;
    halReadMemBlock(ring_buffer->access, &sem, ring_buffer->write_semaphore_addr, 1);
    sem++;
    halWriteMemBlock(ring_buffer->access, &sem, ring_buffer->write_semaphore_addr, 1);    
}

