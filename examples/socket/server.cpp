#include "dtp/socket.h"
#include "dtp/dtp.h"
#include "stdio.h"

int main(){
    int desc = dtpOpen(DTP_READ_WRITE);
    dtpListenSocket(desc, 23436);

    int data[2] = {0, 1};
    dtpRecv(desc, data, 2);
    printf("data: %d, %d\n", data[0], data[1]);
    data[0]++;
    data[1]++;
    dtpSend(desc, data, 2);

    dtpClose(desc);
    return 0;
}
