#include "dtp/dtp.h"
#include "dtp/buffer.h"
#undef NDEBUG
#include "nmassert.h"
#include "stdio.h"



int main(){
    int desc = dtpOpenSharedBuffer(0);
    int data[4];
#ifdef CORE0
    printf("listen\n");
    int ok = dtpListen(desc);
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
#endif //CORE0
#ifdef CORE1
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
#endif //CORE1
    dtpClose(desc);
    return 0;
}