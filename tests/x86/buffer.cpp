#include "dtp/dtp.h"
#include "dtp/buffer.h"
#undef NDEBUG
#include "nmassert.h"
#include "stdio.h"


#include "mc12101load.h"




int main(){    
    PL_Board *board;
    PL_Access *access;
    PL_GetBoardDesc(0, &board);
    PL_GetAccess(board, 0, &access);

    int desc = dtpOpenRemoteSharedBuffer(0, access, (DtpBufferCopyFuncT)PL_ReadMemBlock, (DtpBufferCopyFuncT)PL_WriteMemBlock);
    int data[4];

    printf("connect\n");
    int ok = dtpConnect(desc);
    printf("ok %d\n", ok);

    dtpRecv(desc, data, 3);
    NMASSERT(data[0] == 1);
    NMASSERT(data[1] == 2);
    NMASSERT(data[2] == 3);
    for(int i = 0; i < 10; i++){
        data[0] = 0xCDCDCDCD;
        data[1] = 0xCDCDCDCD;
        dtpRecv(desc, data, 2);
        data[0]++;
        data[1]++;
        dtpSend(desc, data, 2);
    }

    dtpClose(desc);
    return 0;
}