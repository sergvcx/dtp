#include <iostream>
#include "dtp/dtp.h"
#include "dtp/mc12101-host.h"
#include "mc12101load.h"

#undef NDEBUG
#include "assert.h"

extern "C" void halSleep(int msec){

}

void test_singleBuffer_shouldDoneCorrect(){
    PL_Board *board;
    PL_Access *access;
    PL_GetBoardDesc(0, &board);
    PL_GetAccess(board, 0, &access);
    int desc = dtpOpenPloadHost(0, access, (DtpBufferCopyFuncT)PL_ReadMemBlock, (DtpBufferCopyFuncT)PL_WriteMemBlock);

    int ok = dtpConnect(desc);
    assert(ok == DTP_OK);

    int data[2] = {1, 2};
    dtpSend(desc, data, 2);

    dtpRecv(desc, data, 2);
    assert(data[0] == 2);
    assert(data[1] == 3);

    dtpClose(0);

}


int main(){
    test_singleBuffer_shouldDoneCorrect();
    
    return 0;
}