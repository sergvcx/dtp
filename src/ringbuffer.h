
typedef struct DtpRingBuffer DtpRingBuffer;
void dtpInitRingBuffer(DtpRingBuffer *ring_buffer, void* data, int capacity);

int dtpRingBufferGetTail(DtpRingBuffer *ring_buffer);
int dtpRingBufferGetHead(DtpRingBuffer *ring_buffer);


void dtpRingBufferConsume(DtpRingBuffer *ring_buffer, int count);
void dtpRingBufferProduce(DtpRingBuffer *ring_buffer, int count);


void dtpRingBufferPush(DtpRingBuffer *ring_buffer, const void *data, int count);
void dtpRingBufferPop(DtpRingBuffer *ring_buffer, void *data, int count);


int dtpRingBufferIsEmpty(DtpRingBuffer *ring_buffer);
int dtpRingBufferIsFull(DtpRingBuffer *ring_buffer);

int dtpRingBufferAvailable(DtpRingBuffer *ring_buffer);
void dtpRingBufferCapturedRead(DtpRingBuffer *ring_buffer);


void dtpRingBufferReleaseRead(DtpRingBuffer *ring_buffer);

void dtpRingBufferCapturedWrite(DtpRingBuffer *ring_buffer);
void dtpRingBufferReleaseWrite(DtpRingBuffer *ring_buffer);
