#ifndef __DTP_RINGBUFFER_H_INCLUDED__
#define __DTP_RINGBUFFER_H_INCLUDED__

typedef struct DtpRingBuffer DtpRingBuffer;

#ifdef __cplusplus
extern "C" {
#endif

    void dtpInitRingBuffer(DtpRingBuffer *ring_buffer, void* data, int size_of_element, int capacity);

    int dtpRingBufferGetTail(DtpRingBuffer *ring_buffer);
    int dtpRingBufferGetHead(DtpRingBuffer *ring_buffer);


    void dtpRingBufferConsume(DtpRingBuffer *ring_buffer, int count);
    void dtpRingBufferProduce(DtpRingBuffer *ring_buffer, int count);


    void dtpRingBufferPush(DtpRingBuffer *ring_buffer, const void *data, int count);
    void dtpRingBufferPop(DtpRingBuffer *ring_buffer, void *data, int count);

    int dtpRingBufferIsEmpty(DtpRingBuffer *ring_buffer);
    int dtpRingBufferIsFull(DtpRingBuffer *ring_buffer);

    int dtpRingBufferAvailable(DtpRingBuffer *ring_buffer);

    void dtpRingBufferCapturedRead(DtpRingBuffer *ring_buffer, int count);
    void dtpRingBufferReleaseRead(DtpRingBuffer *ring_buffer, int count);
    void dtpRingBufferCapturedWrite(DtpRingBuffer *ring_buffer, int count);
    void dtpRingBufferReleaseWrite(DtpRingBuffer *ring_buffer, int count);

    void dtpRingBufferSync(DtpRingBuffer *ring_buffer);

#ifdef __cplusplus
}
#endif

#endif //__DTP_RINGBUFFER_H_INCLUDED__