#include "dtp/mc12101.h"
#include "dtp/dtp.h"
#include "stdio.h"



int main(){
    int desc = dtpOpen(DTP_READ_WRITE);

    dtpNm6407ConnectBuffer(desc, 1);

    dtpClose(desc);

    return 0;
}