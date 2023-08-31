#include "dtp/dtp.h"
#include "dtp/nm6407.h"
#undef NDEBUG
#include "nmassert.h"
#include "stdio.h"

void lightup_callback(void *data){
    int *value = (int *)data;
    int *gpio_base = (int*)0x10030C00;
    gpio_base[4] = 0xFF;
    gpio_base[0] = *value;
}

__attribute__((section(".data.imu0"))) int data[2];
//__attribute
int main(){

#ifdef INPUT
    printf("input\n");
    int desc = dtpOpen(DTP_READ_ONLY);
    dtpNm6407Link(desc, 2, 0);    
    printf("data: %p\n", data);

    int light_value = 1;
    DtpAsync task;
    task.buf = data;
    task.nwords = 2;
    task.type = DTP_TASK_1D;
    task.callback = lightup_callback;
    task.cb_data = &light_value;

    
    data[0] = 0xCDCDCDCD;
    data[1] = 0xCDCDCDCD;

    int counter = 0;
    for(int i = 0; i < 10; i++){
        printf("%i\n", i);
        //dtpRecv(desc, dst, 2);
        dtpAsyncRecv(desc, &task);
        int status = 0;
        do{
            status = dtpAsyncStatus(desc, &task);
        }while(status == DTP_ST_IN_PROCESS);
    
        NMASSERT(data[0] == counter);
        NMASSERT(data[1] == counter + 1);
        counter++;
    }

#else
    printf("output\n");
    int desc = dtpOpen(DTP_WRITE_ONLY);
    dtpNm6407Link(desc, 2, 0);    
    printf("data: %p\n", data);

    data[0] = 0;
    data[1] = 1;

    int counter = 0;
    for(int i = 0; i < 10; i++){
        printf("%i\n", i);
        dtpSend(desc, data, 2);
        data[0]++;
        data[1]++;
    }

#endif
    return 0;
}