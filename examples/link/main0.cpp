#include "dtp/dtp.h"
#include "dtp/nm6407.h"
#include "stdio.h"

#define SIZE 8

int src[SIZE];

int main(){    
    printf("src=%p\n", src);
    int desc = dtpOpenLink(1);    
    if(desc < 0){
        return 1;
    }
    
    for(int i = 0; i < SIZE; i++){
        src[i] = i;
        printf("%2d: 0x%x\n", i, src[i]);
    }

    dtpWrite(desc, src, SIZE * 4);
    dtpFlush(desc);
    dtpClose(desc);
    return 0;
}