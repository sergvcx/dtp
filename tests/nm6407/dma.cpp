#include "dtp/nm6407.h"
#include "dtp/dtp.h"
#include "stdio.h"

#undef NDEBUG
#include "nmassert.h"

const int SIZE = 1024;

alignas(64) int src[SIZE + 4];
alignas(64) int dst[SIZE + 4];

void lightup_callback(void *data){
    int *value = (int *)data;
    int *gpio_base = (int*)0x10030C00;
    gpio_base[4] = 0xFF;
    gpio_base[0] = *value;
}

void __attribute__((optimize("O0"))) test_DmaPacket_whenCorrectArgs_shouldCopy(){
    DtpAsync send_cmd;

    int desc = dtpOpen(DTP_READ_WRITE);
    dtpNm6407Dma(desc, 0);    
    NMASSERT(desc > 0);   
    

    int ok;
    int led_value = 0x1;
    
    send_cmd.buf = src;
    send_cmd.nwords = SIZE;
    send_cmd.type = DTP_TASK_1D;
    send_cmd.cb_data = &led_value;
    send_cmd.callback = lightup_callback;

    for(int i = 0; i < 100; i++){
        for(int i = 0; i < SIZE + 4; i++){
            src[i] = i;
            dst[i] = 0xCDCDCDCD;
        }

        ok = dtpAsyncSend(desc, &send_cmd);
        NMASSERT(ok == DTP_OK);
        ok = dtpRecv(desc, dst, SIZE);
        NMASSERT(ok == DTP_OK);


        for(int i = 0; i < SIZE; i++){        
            //printf("dst[%02d] = 0x%x\n", i, dst[i]);
            NMASSERT(src[i] == dst[i]);
        }

        led_value++;

        for(int i = 0; i < 100000; i++);
    }
    led_value = 0;
    lightup_callback(&led_value);

    dtpClose(desc);
}

int main(){
    test_DmaPacket_whenCorrectArgs_shouldCopy();
    printf("ALL TESTS PASSED!\n");
    return 0;
}