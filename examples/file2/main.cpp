#include "dtp/dtp.h"
#include "dtp/file.h"
#include "stdio.h"
#include "string.h"

int main(){
    int desc = dtpOpenFile("simple.txt", "w+");

    const char *str = "This is text   \n";
    dtpSend(desc, str, strlen(str) / sizeof(int));
    dtpClose(desc);
    return 0;
}