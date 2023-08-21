#include "dtp/dtp.h"
#include "dtp/mc12101.h"
#include "stdio.h"
#undef NDEBUG
#include "nmassert.h"

#define SIZE32 8

extern "C" void halSleep(int msec){

}

void test_singleBuffer_shouldDoneCorrect(){
    int desc = dtpOpenPloadTarget(0);

    int data[2] = {0xCDCDCDCD,0xCDCDCDCD};
    int ok = dtpListen(desc);
    NMASSERT(ok == DTP_OK);

    dtpRecv(desc, data, 2);
    NMASSERT(data[0] == 1);
    NMASSERT(data[1] == 2);

    data[0]++;
    data[1]++;
    dtpSend(desc, data, 2);

}

int main(){
    test_singleBuffer_shouldDoneCorrect();
    
    printf("ALL TESTS PASSED!!\n");
    return 0;
}