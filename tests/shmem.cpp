#include "dtp/dtp.h"
#include "dtp/ringbuffer.h"
#undef NDEBUG
#include "uassert.h"

#define SIZE32 8

int dmaBuff[SIZE32];

void test_open_whenChoosedShmem_shouldReturnGt0(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);    

    // Act
    int d = dtpOpenShmem(&ringbuffer);

    // Assert
    uassert(d > 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmem_shouldReturnCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);
    int d = dtpOpenShmem(&ringbuffer);
    const int size32 = 4;
    int src[size32];
    int dst[size32];
    for(int i = 0; i < size32; i++){
        src[i] = i;
        dst[i] = 0xCDCDCDCD;
    }    

    // Act
    int writeSize = dtpWrite(d, src, size32 * 4);
    printf("writeSize %d\n", writeSize);

    int readSize = dtpRead(d, dst, size32 * 4);
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
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);
    int d = dtpOpenShmem(&ringbuffer);
    const int size32 = SIZE32 + 2;
    int src[size32];    

    // Act
    int writeSize = dtpWrite(d, src, size32 * 4);


    // Assert
    printf("writeSize %d\n", writeSize);
    uassert(writeSize == SIZE32 * 4);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmemMultipleWriteRead_shouldReturnCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);
    int d = dtpOpenShmem(&ringbuffer);
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
        

        int writeSize = dtpWrite(d, (src + i), localSize * 4);
        int readSize = dtpRead(d, (dst + i), localSize * 4);

        uassert(writeSize == readSize);
    }


    // Assert
    uassert(dst[size32] == 0xCDCDCDCD);
    for(int i = 0; i < size32; i++){
        printf("%2d: src=0x%x, dst=0x%x\n", i, src[i], dst[i]);
        uassert(src[i] == dst[i]);
    }
    

    // Free
    dtpClose(d);

}

void test_readwrite_whenChoosedShmemNoRead_shouldReturnZero(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);
    int d = dtpOpenShmem(&ringbuffer);
    int dst[2];

    // Act
    int readSize = dtpRead(d, dst, 2 * 4);

    // Asserts
    uassert(readSize == 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenChoosedShmemFullWrite_shouldReturnZero(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);
    int d = dtpOpenShmem(&ringbuffer);
    int src[SIZE32];
    dtpWrite(d, src, SIZE32 * 4);

    // Act
    int writeSize = dtpWrite(d, src, SIZE32 * 4);

    // Asserts
    uassert(writeSize == 0);

    // Free
    dtpClose(d);
}

void test_readwrite_whenMultupleDesc_shouldDoneCorrect(){
    printf("%s\n", __FUNCTION__);
    // Arrange
    DtpRingBuffer32 ringbuffer;
    dtpInitRingBuffer(&ringbuffer, dmaBuff, SIZE32);
    int d1 = dtpOpenShmem(&ringbuffer);
    int d2 = dtpOpenShmem(&ringbuffer);
    const int size32 = 4;
    int src[size32];
    int dst[size32];
    for(int i = 0; i < size32; i++){
        src[i] = i;
        dst[i] = 0xCDCDCDCD;
    }    

    // Act
    int writeSize = dtpWrite(d1, src, size32 * 4);
    printf("writeSize %d\n", writeSize);

    int readSize = dtpRead(d2, dst, size32 * 4);
    printf("readSize %d\n", readSize);

    // Assert
    uassert(writeSize == size32 * 4);
    uassert(readSize == size32 * 4);
    for(int i = 0; i < size32; i++){
        uassert(src[i] == dst[i]);
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