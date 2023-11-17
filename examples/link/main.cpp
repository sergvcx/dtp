#include "dtp/nm6407.h"
#include "stdio.h"

#define SIZE 2

__attribute__ ((section(".data.imu0"))) int data[SIZE];

int main(){
#ifdef CORE0
    int mode = DTP_WRITE_ONLY;
#else
    int mode = DTP_READ_ONLY;
#endif
    printf("mode: 0x%x\n", mode);
    printf("data: %p\n", data);

    int dtp = dtpOpen(mode);    

    int port = 2;

    dtpNm6407Link(dtp, port, 0);

    

#ifdef CORE0
    data[0] = 0xBABABABA;
    data[1] = 0xDEDADEDA;
    while(1){
        int error = dtpSend(dtp, data, SIZE);
        if (error == DTP_ERROR){
            printf("Failed send data\n");
            exit(1);
        }
        if(error == DTP_OK) break;
    }
#else
    data[0] = 0xCDCDCDCD;
    data[1] = 0xCDCDCDCD;
    while(1){
        int error = dtpRecv(dtp, data, SIZE);
        if (error == DTP_ERROR){
            printf("Failed recv data\n");
            exit(1);
        }
        if(error == DTP_OK) break;
    }
#endif
    dtpClose(dtp);

    return data[0];
}