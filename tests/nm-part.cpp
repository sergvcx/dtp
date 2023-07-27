#include "dtp/dtp.h"
#include "dtp/ringbuffer.h"
#include "dtp/dtp-hal.h"
#undef NDEBUG
#include "uassert.h"

#define SIZE32 8

int dmaBuff0[SIZE32];
int dmaBuff1[SIZE32];


void test_open_whenChoosedShmem_shouldReturnGt0(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    

    // Act
    int d = dtpOpenBuffer(rb0);

    // Assert
    uassert(d > 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmem_shouldReturnCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d1 = dtpOpenBuffer(rb0);
    int d2 = dtpOpenBuffer(rb0);
    uassert(d1 > 0);
    uassert(d2 > 0);
    const int size32 = 4;
    int src[size32];
    int dst[size32];
    for(int i = 0; i < size32; i++){
        src[i] = i;
        dst[i] = 0xCDCDCDCD;
    }    

    // Act
    int writeSize = dtpSend(d1, src, size32 * 4);
    printf("writeSize %d\n", writeSize);

    int readSize = dtpRecv(d2, dst, size32 * 4);
    printf("readSize %d\n", readSize);

    // Assert
    uassert(writeSize == size32 * 4);
    uassert(readSize == size32 * 4);
    for(int i = 0; i < size32; i++){
        uassert(src[i] == dst[i]);
    }
}


void test_readwrite_whenChoosedShmemWriteGtThenSize_shouldReturnLtSize(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d = dtpOpenBuffer(rb0);
    uassert(d > 0);
    const int size32 = SIZE32 + 2;
    int src[size32];    

    // Act
    int writeSize = dtpSend(d, src, size32 * 4);


    // Assert
    printf("writeSize %d\n", writeSize);
    uassert(writeSize == SIZE32 * 4);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmemMultipleWriteRead_shouldReturnCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d1 = dtpOpenBuffer(rb0);
    int d2 = dtpOpenBuffer(rb0);
    uassert(d1 > 0);
    uassert(d2 > 0);
    const int size32 = 10 * SIZE32;
    const int step32 = 3;
    int src[size32];
    int dst[size32 + 1];
    for(int i = 0; i < size32; i++){
        src[i] = i + 1;
        dst[i] = 0xCDCDCDCD;
    }
    dst[size32] = 0xCDCDCDCD;

    // Act
    for(int i = 0; i < size32; i += step32){
        int localSize = (size32 - i < step32) ? size32 - i : step32;
        

        int writeSize = dtpSend(d1, (src + i), localSize * 4);
        int readSize = dtpRecv(d2, (dst + i), localSize * 4);

        uassert(writeSize == readSize);
    }


    // Assert
    uassert(dst[size32] == 0xCDCDCDCD);
    for(int i = 0; i < size32; i++){
        printf("%2d: src=0x%x, dst=0x%x\n", i, src[i], dst[i]);
        uassert(src[i] == dst[i]);
    }
    

    // Free
    dtpClose(d1);
    dtpClose(d2);
}

void test_readwrite_whenChoosedShmemNoRead_shouldReturnZero(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d = dtpOpenBuffer(rb0);
    uassert(d > 0);
    int dst[2];

    // Act
    int readSize = dtpRecv(d, dst, 2 * 4);

    // Asserts
    uassert(readSize == 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmemFullWrite_shouldReturnZero(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d = dtpOpenBuffer(rb0);
    int src[SIZE32];
    dtpSend(d, src, SIZE32 * 4);

    // Act
    int writeSize = dtpSend(d, src, SIZE32 * 4);

    // Asserts
    uassert(writeSize == 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenMultupleDesc_shouldDoneCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d1 = dtpOpenBuffer(rb0);
    int d2 = dtpOpenBuffer(rb0);
    uassert(d1 > 0);
    uassert(d2 > 0);
    const int size32 = 4;
    int src[size32];
    int dst[size32];
    for(int i = 0; i < size32; i++){
        src[i] = i;
        dst[i] = 0xCDCDCDCD;
    }    

    // Act
    int writeSize = dtpSend(d1, src, size32 * 4);
    printf("writeSize %d\n", writeSize);

    int readSize = dtpRecv(d2, dst, size32 * 4);
    printf("readSize %d\n", readSize);

    // Assert
    uassert(writeSize == size32 * 4);
    uassert(readSize == size32 * 4);
    for(int i = 0; i < size32; i++){
        uassert(src[i] == dst[i]);
    }
}

#include "hal/hal.h"

void test_openBuffer_whenHostRun_shouldDoneCorrect(){
    HalAccess * access = halGetAccess(0, 0, 0);
    DtpRingBuffer32 *rb = dtpRingBufferCreate(dmaBuff0, SIZE32);
    printf("ringbuffer: %p\n", rb);

    int d = dtpOpenBuffer(rb);

    int src[2];
    int dst[2];
    src[0] = 1;
    src[1] = 2;
    dst[0] = 0xCDCDCDCD;
    dst[1] = 0xCDCDCDCD;

    dtpSend(d, src, 2 * 4);

    halSync(access, (int)rb, 0);
    halSync(access, 0, 0);
    dtpRecv(d, dst, 2 * 4);

    uassert(dst[0] == 2);
    uassert(dst[1] == 3);

    dtpClose(d);
}

void test_openUsb_whenHostRun_shouldDoneCorrect(){
    HalAccess * access = halGetAccess(0, 0, 0);

    int d = dtpOpenPload(access, 0);
    void *user_data = dtpGetUserData(d);
    printf("user_data %p\n", user_data);

    int src[2];
    int dst[2];
    src[0] = 1;
    src[1] = 2;
    dst[0] = 0xCDCDCDCD;
    dst[1] = 0xCDCDCDCD;

    dtpSend(d, src, 2 * 4);

    halSync(access, (int)user_data, 0);
    
    dtpRecv(d, dst, 2 * 4);
    printf("readed\n");

    halSync(access,0x33, 0);

    uassert(dst[0] == 2);
    uassert(dst[1] == 3);

    dtpClose(d);
}

int main(){
    test_open_whenChoosedShmem_shouldReturnGt0();
    test_readwrite_whenChoosedShmem_shouldReturnCorrect();
    test_readwrite_whenChoosedShmemWriteGtThenSize_shouldReturnLtSize();
    test_readwrite_whenChoosedShmemMultipleWriteRead_shouldReturnCorrect();
    test_readwrite_whenChoosedShmemNoRead_shouldReturnZero();
    test_readwrite_whenChoosedShmemFullWrite_shouldReturnZero();
    test_readwrite_whenMultupleDesc_shouldDoneCorrect();
    test_openBuffer_whenHostRun_shouldDoneCorrect();
    test_openUsb_whenHostRun_shouldDoneCorrect();
    printf("ALL TESTS PASSED!!\n");
    return 0;
}