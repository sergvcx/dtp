#include "dtp/dtp.h"
#include "dtp/nm6407.h"
#include "stdio.h"

#define SIZE 8

int dst[SIZE];

int main(){
    printf("dst=%p\n", dst);
    int desc = dtpOpenLink(0);    
    if(desc < 0){
        return 1;
    }
    
    for(int i = 0; i < SIZE; i++){
        dst[i] = 0xCDCDCDCD;
    }

    dtpRead(desc, dst, SIZE * 4);
    for(int i = 0; i < SIZE; i++){
        printf("%2d: 0x%x\n", i, dst[i]);
    }    
    dtpClose(desc);
    return 0;
}