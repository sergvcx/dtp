#include "dtp/file.h"
#include "dtp/mc12101.h"
#include "dtp/dtp.h"
#include "stdio.h"

#define FILE "exchange.bin"

int main(){
    int file_desc = 0;
    do{
        file_desc = dtpOpenFile(FILE,"rb");    
    }while(file_desc < 0);
    
    uintptr_t addr_write = 0;
    uintptr_t addr_read = 0;

    // pc-nm0 ring buffer addr, not needed
    dtpRecv(file_desc, &addr_write, 1);
    dtpRecv(file_desc, &addr_read, 1);
    printf("addr_write %p, addr_read %p\n", addr_write, addr_read);

    // nm0-nm1 ring buffer addr
    dtpRecv(file_desc, &addr_write, 1);
    dtpRecv(file_desc, &addr_read, 1);
    dtpClose(file_desc);
    printf("addr_write %p, addr_read %p\n", addr_write, addr_read);


    // -------------------------------------------------------------


    int rb_desc_r = dtpOpenRingbufferDefault((void*)addr_read);
    int rb_desc_w = dtpOpenRingbufferDefault((void*)addr_write);

    int data[2];
    dtpRecv(rb_desc_r, data, 2);
    printf("recv: %d, %d\n", data[0], data[1]);
    data[0]++;
    data[1]++;
    
    dtpSend(rb_desc_w, data, 2);

    dtpClose(rb_desc_r);
    dtpClose(rb_desc_w);

    return 0;
}