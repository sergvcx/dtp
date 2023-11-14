#include <iostream>
#include "dtp/dtp.h"
#include "dtp/mc12101-host.h"
#include "dtp/mc12101.h"
#include "mc12101load.h"

#undef NDEBUG
#include "assert.h"

extern "C" void halSleep(int msec){

}

void test_singleBuffer_shouldDoneCorrect(){
    printf("%s\n", __FUNCTION__);
    PL_Board *board;
    PL_Access *access;
    int error;
    PL_GetBoardDesc(0, &board);
    PL_GetAccess(board, 0, &access);

    int desc = dtpOpen(DTP_READ_WRITE);    
    assert(desc >= 0);
    //int desc = dtpOpenPloadHost(0, access, (DtpBufferCopyFuncT)PL_ReadMemBlock, (DtpBufferCopyFuncT)PL_WriteMemBlock);
    int ok = dtpMc12101Connect(desc, access, 0);

    //int ok = dtpConnect(desc);
    assert(ok == DTP_OK);

    for(int i = 0; i < 200; i++){
        int data[2] = {1, 2};
        while(true){
            error = dtpSend(desc, data, 2);
            assert(error != DTP_ERROR);
            if(error == DTP_OK) break;
        }

        while(true){
            error = dtpRecv(desc, data, 2);
            assert(error != DTP_ERROR);
            if(error == DTP_OK) break;
        }
        printf("%d\n", i);
        assert(data[0] == 2);
        assert(data[1] == 3);
    }

    dtpClose(desc);

}


int main(){
    printf("avsa");
    test_singleBuffer_shouldDoneCorrect();
    
    return 0;
}