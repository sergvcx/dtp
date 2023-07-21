#include "dtp/dtp.h"
#include "string.h"

int main(){
    int desc = dtpOpenFile("simple.txt", "w+");
    const char *str = "This is text\n";
    dtpWrite(desc, str, strlen(str) * 4);
    dtpFlush(desc);
    dtpClose(desc);
    return 0;
}