#include "dtp/nm6407.h"
#include "dtp/dtp.h"
#include "stdio.h"

const int SIZE = 1024;

alignas(64) int src[SIZE];
alignas(64) int dst[SIZE];

void lightup_callback(void *data){
    int *value = (int *)data;
    int *gpio_base = (int*)0x10030C00;
    gpio_base[4] = 0xFF;
    gpio_base[0] = *value;    
}


int main(){
    int desc = dtpOpen(DTP_READ_WRITE);
    
    printf("src: %p\n", src);
    printf("dst: %p\n", dst);
    if(desc < 0){
        printf("Failed open descriptor");
    }    

    //dtpNm6407Dma(desc, DTP_NM6407_DMA_STATUS_ONLY | DTP_NM6407_DMA_PACKET);    
    dtpNm6407Dma(desc, DTP_NM6407_DMA_PACKET);    
    

    int ok;
    int led_value = 0x1;
    
    DtpAsync send_cmd;
    send_cmd.buf = src;    
    send_cmd.nwords = SIZE;
    send_cmd.type = DTP_TASK_1D;
    send_cmd.cb_data = &led_value;
    send_cmd.callback = lightup_callback;

    for(int i = 0; i < SIZE; i++){
        src[i] = i;
        dst[i] = 0xCDCDCDCD;
    }
    for(int i = 0; i < 16; i++){        
        printf("src[%2d]: 0x%x\n", i, src[i]);
        printf("dst[%2d]: 0x%x\n", i, dst[i]);
    } 
    printf("========================\n");

    // async send because need wait recv
    ok = dtpAsyncSend(desc, &send_cmd);
    if(ok) printf("send error: %d\n", ok);

    ok = dtpRecv(desc, dst, SIZE);
    if(ok) printf("recv error: %d\n", ok);

    // wait send because enabled flag DTP_NM6407_DMA_STATUS_ONLY
    // ok = dtpAsyncWait(desc, &send_cmd);
    // if(ok) printf("wait error: %d\n", ok);       

    for(int i = 0; i < 16; i++){        
        printf("src[%2d]: 0x%x\n", i, src[i]);
        printf("dst[%2d]: 0x%x\n", i, dst[i]);
    } 
    printf("========================\n");

    dtpClose(desc);
    return 0;
}