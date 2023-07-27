#include "hal/hal-host.h"
#include <iostream>
#include "dtp/dtp.h"
#include "dtp/dtp-hal.h"
#include "dtp/ringbuffer.h"

#undef NDEBUG
#include "assert.h"

HalAccess *global_access;

HalAccess *getTestAccess(){
    return global_access;
}

void test_singleBuffer_shouldDoneCorrect(){
    HalAccess *access = getTestAccess();
    int value = halSync(access, 0, NULL);

    DtpRingBuffer32 *rb = dtpRingBufferBind(access, (uintptr_t)value);

    int desc = dtpOpenBuffer(rb);

    int buf[2];
    buf[0] = 0xCDCDCDCD;
    buf[1] = 0xCDCDCDCD;
    int size = dtpRecv(desc, buf, 2 * 4);
    printf("buf: %d, %d\n", buf[0], buf[1]);
    assert(buf[0] == 1);
    assert(buf[1] == 2);   

    buf[0]++;
    buf[1]++;

    dtpSend(desc, buf, 2 * 4);

    halSync(access, 0, NULL);

    dtpClose(desc);

}

void test_openUsb_shouldDoneCorrect(){
    HalAccess *access = getTestAccess();
    int value = halSync(access, 0, NULL);

    int desc = dtpOpenPload(access, (uintptr_t)value);

    int buf[2];
    buf[0] = 0xCDCDCDCD;
    buf[1] = 0xCDCDCDCD;
    int size = dtpRecv(desc, buf, 2 * 4);
    printf("buf: %d, %d\n", buf[0], buf[1]);
    assert(buf[0] == 1);
    assert(buf[1] == 2);   

    buf[0]++;
    buf[1]++;

    dtpSend(desc, buf, 2 * 4);

    int sync = halSync(access, 0, NULL);
    assert(sync == 0x33);

    dtpClose(desc);

}

int main(){
    //halBoard *
    HalBoard *board = halAllocBoard();
    halBoardSetOption(board, HAL_BOARD_TYPE, HAL_MC12101);
    halBoardSetOption(board, HAL_BOARD_NUMBER, 0);

    if(halBoardOpen(board)){
        std::cout << "Failed get board" << std::endl;
        return 1;
    }

    HalCore core;
    core.core = 0;
    int error;
    global_access = halGetAccess(board, &core, &error);

    test_singleBuffer_shouldDoneCorrect();
    test_openUsb_shouldDoneCorrect();

    halAccessClose(global_access);
    halBoardClose(board);
    halFreeBoard(board);
    
    return 0;
}