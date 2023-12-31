#include "dtp/dtp.h"
#include "dtp/mc12101.h"
#include "dtp/nm6407.h"
#include "stdio.h"
#undef NDEBUG
#include "nmassert.h"

#define SIZE32 8

extern "C" void halSleep(int msec){

}

void test_singleBuffer_shouldDoneCorrect(){
    //int desc = dtpOpenPloadTarget(0);
    int desc = dtpOpen(DTP_READ_WRITE);
    NMASSERT(desc >= 0);
    int ok = dtpNm6407InitDefaultBuffer(desc, 0);
    printf("buffer inited\n");
    int error;

    for(int i = 0; i < 200; i++){
        int data[2] = {0xCDCDCDCD,0xCDCDCDCD};
        //NMASSERT(ok == DTP_OK);

        while(true){
            error = dtpRecv(desc, data, 2);
            NMASSERT(error != DTP_ERROR);
            if(error == DTP_OK) break;
        }
        
        NMASSERT(data[0] == 1);
        NMASSERT(data[1] == 2);

        data[0]++;
        data[1]++;
        while(true){
            error = dtpSend(desc, data, 2);
            NMASSERT(error != DTP_ERROR);
            if(error == DTP_OK) break;
        }
        
    }

}

int main(){
    test_singleBuffer_shouldDoneCorrect();
    
    printf("ALL TESTS PASSED!!\n");
    return 0;
}