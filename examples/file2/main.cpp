#include "dtp/dtp.h"
#include "dtp/file.h"
#include "stdio.h"
#include "string.h"

int main(){
    int dw = dtpOpenFile("simple.txt", "w");
    int dr = dtpOpenFile("simple.txt", "r");

    const char *str = "This is text   \n";
    char dst[20];
    dtpSend(dw, str, strlen(str) / sizeof(int));

    dtpRecv(dr, dst, strlen(str) / sizeof(int));

    printf(dst);


    dtpClose(dw);
    dtpClose(dr);
    return 0;
}