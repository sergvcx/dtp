#include "dtp/dtp.h"
#include "dtp/ringbuffer.h"
#include "dtp/dtp-hal.h"
#include "dtp/nm6407.h"
#undef NDEBUG
#include "nmassert.h"

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
    NMASSERT(d > 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmem_shouldReturnCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d1 = dtpOpenBuffer(rb0);
    int d2 = dtpOpenBuffer(rb0);
    NMASSERT(d1 > 0);
    NMASSERT(d2 > 0);
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
    NMASSERT(writeSize == size32 * 4);
    NMASSERT(readSize == size32 * 4);
    for(int i = 0; i < size32; i++){
        NMASSERT(src[i] == dst[i]);
    }
}


void test_readwrite_whenChoosedShmemWriteGtThenSize_shouldReturnLtSize(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d = dtpOpenBuffer(rb0);
    NMASSERT(d > 0);
    const int size32 = SIZE32 + 2;
    int src[size32];    

    // Act
    int writeSize = dtpSend(d, src, size32 * 4);


    // Assert
    printf("writeSize %d\n", writeSize);
    NMASSERT(writeSize == SIZE32 * 4);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmemMultipleWriteRead_shouldReturnCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d1 = dtpOpenBuffer(rb0);
    int d2 = dtpOpenBuffer(rb0);
    NMASSERT(d1 > 0);
    NMASSERT(d2 > 0);
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

        NMASSERT(writeSize == readSize);
    }


    // Assert
    NMASSERT(dst[size32] == 0xCDCDCDCD);
    for(int i = 0; i < size32; i++){
        printf("%2d: src=0x%x, dst=0x%x\n", i, src[i], dst[i]);
        NMASSERT(src[i] == dst[i]);
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
    NMASSERT(d > 0);
    int dst[2];

    // Act
    int readSize = dtpRecv(d, dst, 2 * 4);

    // Asserts
    NMASSERT(readSize == 0);

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
    NMASSERT(writeSize == 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenMultupleDesc_shouldDoneCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 *rb0 = dtpRingBufferCreate(dmaBuff0, SIZE32);
    int d1 = dtpOpenBuffer(rb0);
    int d2 = dtpOpenBuffer(rb0);
    NMASSERT(d1 > 0);
    NMASSERT(d2 > 0);
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
    NMASSERT(writeSize == size32 * 4);
    NMASSERT(readSize == size32 * 4);
    for(int i = 0; i < size32; i++){
        NMASSERT(src[i] == dst[i]);
    }
}



int main(){
    test_open_whenChoosedShmem_shouldReturnGt0();
    test_readwrite_whenChoosedShmem_shouldReturnCorrect();
    test_readwrite_whenChoosedShmemWriteGtThenSize_shouldReturnLtSize();
    test_readwrite_whenChoosedShmemMultipleWriteRead_shouldReturnCorrect();
    test_readwrite_whenChoosedShmemNoRead_shouldReturnZero();
    test_readwrite_whenChoosedShmemFullWrite_shouldReturnZero();
    test_readwrite_whenMultupleDesc_shouldDoneCorrect();
    printf("ALL TESTS PASSED!!\n");
    return 0;
}