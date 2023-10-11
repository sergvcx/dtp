#include "dtp/dtp.h"
#include "dtp/nm6407.h"
#include "stdio.h"
#include "time.h"

#define SIZE 4

int data_host_input[SIZE];
int data_host_output[SIZE];

int data_nm1_input[SIZE];
int data_nm1_output[SIZE];

#define DELAY 1000000


int 
__attribute__((optimize("O0")))
main(){
    int desc0 = dtpOpen(DTP_READ_WRITE);
    int desc1 = dtpOpen(DTP_READ_WRITE);

    dtpNm6407InitBuffer(desc0, data_host_input, SIZE,  data_host_output, SIZE, 0);
    dtpNm6407InitBuffer(desc1, data_nm1_input, SIZE,  data_nm1_output, SIZE, 1);

    int data[2];
    data[0] = 0;
    data[1] = 1;
    int error = 0;
    for(int i = 0; i < 10; i++){
        printf("iteration: %d\n", i);
        do{
            error = dtpSend(desc0, data, 2);
            if(error == DTP_AGAIN) {
                printf("DTP_AGAIN\n");
                for(int k = 0; k < DELAY; k++);
            }
        }while(error == DTP_AGAIN);


        do{
            error = dtpRecv(desc0, data, 2);
            if(error == DTP_AGAIN) {
                for(int k = 0; k < DELAY; k++);
                printf("DTP_AGAIN\n");
            }
        }while(error == DTP_AGAIN);


        if(data[0] != i + 1 || data[1] != i + 2){
            printf("Wrong values\n");
            return 0;
        }
    }


    return 0;
}