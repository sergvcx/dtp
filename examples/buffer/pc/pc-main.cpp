#include "mc12101load.h"
#include "dtp/dtp.h"
#include "dtp/file.h"
#include "dtp/mc12101-host.h"
#include "stdio.h"

#define FILE "../exchange.bin"

int main(){

    int file_desc = 0;
    do{
        file_desc = dtpOpenFile(FILE,"rb");    
    }while(file_desc < 0);
    
    uintptr_t addr_write = 0;
    uintptr_t addr_read = 0;
    dtpRecv(file_desc, &addr_write, 1);
    printf("addr_write %p\n", addr_write);
    dtpRecv(file_desc, &addr_read, 1);
    printf("addr_read %p\n", addr_read);
    dtpClose(file_desc);


    int dw = dtpOpenMc12101Ringbuffer(0, 0, addr_write);
    int dr = dtpOpenMc12101Ringbuffer(0, 0, addr_read);
    int data[2];
    dtpRecv(dr, data, 2);
    printf("recv: %d, %d\n", data[0], data[1]);
    data[0]++;
    data[1]++;
    dtpSend(dw, data, 2);
    
    
    dtpClose(dw);
    dtpClose(dr);
}


