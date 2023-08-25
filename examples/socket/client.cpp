#include "dtp/socket.h"
#include "dtp/dtp.h"
#include "stdio.h"

int main(){
    int desc = dtpOpen(DTP_READ_WRITE);
    dtpListenSocket(desc, 23436);

    int ok = dtpConnect(desc);
    printf("ok %d\n", ok);
    if(ok < 0) return 0;

    int data[2] = {1, 2};
    dtpSend(desc, data, 2);

    dtpRecv(desc, data, 2);
    printf("data: %d, %d\n", data[0], data[1]);


    dtpClose(desc);
    return 0;
}
