#include "dtp/dtp.h"
#include "dtp/buffer.h"
#include "dtp/nm6407.h"
#undef NDEBUG
#include "nmassert.h"
#include "stdio.h"

#ifndef __NM__
#include "mc12101load.h"
#endif



int main(){
    //int desc = dtpConnectSharedBuffer(0);
    int desc = dtpOpen(DTP_READ_WRITE);

    //int desc = dtpOpenNm6407Spi();
    //int desc = dtpOpenNm6407Link(2, DTP_NM6407_LINK_INPUT);

    int data[4];
#ifdef LISTEN
    printf("listen\n");
    int ok = dtpNm6407InitDefaultBuffer(desc, 0);
    //int ok = dtpListen(desc);
    printf("ok %d\n", ok);
    data[0] = 1;
    data[1] = 2;
    data[2] = 3;
    dtpSend(desc, data, 3);
    for(int i = 0; i < 10; i++){
        int old_value[2] = {data[0], data[1] };

        dtpSend(desc, data, 2);

        dtpRecv(desc, data, 2);
        NMASSERT(data[0] == old_value[0] + 1);
        NMASSERT(data[1] == old_value[1] + 1);
    }
#else
    printf("connect\n");
    int ok = dtpNm6407ConnectBuffer(desc, 0);
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
#endif 
    dtpClose(desc);
    return 0;
}