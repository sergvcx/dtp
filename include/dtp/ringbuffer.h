#ifndef __DTP_RINGBUFFER_H_INCLUDED__
#define __DTP_RINGBUFFER_H_INCLUDED__
#include "dtp/dtp.h"


typedef struct DtpRingBuffer32{
    int *data;
    volatile size_t capacity;

    volatile size_t read_semaphore;
    volatile size_t write_semaphore;
    volatile size_t head;
    volatile size_t tail;
} DtpRingBuffer32;

#ifdef __cplusplus
extern "C" {
#endif
    //DtpRingBuffer32 *dtpRingBufferCreate(void *data, int capacity);

    void dtpInitRingBuffer(DtpRingBuffer32 *ring_buffer, void* data, int capacity);

    int dtpRingBufferGetTail(DtpRingBuffer32 *ring_buffer);
    int dtpRingBufferGetHead(DtpRingBuffer32 *ring_buffer);

    int dtpRingBufferGetCapacity(DtpRingBuffer32 *ring_buffer);
    int dtpRingBufferGetSizeOfElem(DtpRingBuffer32 *ring_buffer);


    void dtpRingBufferConsume(DtpRingBuffer32 *ring_buffer, int count);
    void dtpRingBufferProduce(DtpRingBuffer32 *ring_buffer, int count);


    void dtpRingBufferPush(DtpRingBuffer32 *ring_buffer, const void *data, int count);
    void dtpRingBufferPop(DtpRingBuffer32 *ring_buffer, void *data, int count);

    int dtpRingBufferIsEmpty(DtpRingBuffer32 *ring_buffer);
    int dtpRingBufferIsFull(DtpRingBuffer32 *ring_buffer);

    int dtpRingBufferAvailable(DtpRingBuffer32 *ring_buffer);

    void dtpRingBufferCapturedRead(DtpRingBuffer32 *ring_buffer, int count);
    void dtpRingBufferReleaseRead(DtpRingBuffer32 *ring_buffer, int count);
    void dtpRingBufferCapturedWrite(DtpRingBuffer32 *ring_buffer, int count);
    void dtpRingBufferReleaseWrite(DtpRingBuffer32 *ring_buffer, int count);

    void dtpRingBufferSync(DtpRingBuffer32 *ring_buffer);

#ifdef __cplusplus
}
#endif

#endif //__DTP_RINGBUFFER_H_INCLUDED__