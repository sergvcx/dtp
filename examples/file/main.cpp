#include "dtp/dtp.h"
#include "stdio.h"
#include "string.h"

int fileRecv(void *com_spec, DtpAsync *aio){
    FILE *file = (FILE*)com_spec;
	size_t rsize;
    do {
		 rsize = fread((void*)aio->buf, sizeof(int), aio->nwords, file);
	} while (rsize==0);
    return 0;
}

int fileSend(void *com_spec, DtpAsync *aio){
    FILE *file = (FILE*)com_spec;
    size_t wsize = fwrite((void*)aio->buf, sizeof(int), aio->nwords, file);
    fflush(file);
    return 0;
}

int fileDestroy(void *com_spec){
    FILE *file = (FILE*)com_spec;
    return fclose(file);
}

int fileStatus(void *com_spec, DtpAsync *aio){
    return DTP_ST_DONE;
}

int main(){
    FILE *file = fopen("simple.txt", "w");
    
    DtpImplementation impl;    
    impl.recv = fileRecv;
    impl.send = fileSend;
    impl.destroy = fileDestroy;    
    int desc = dtpOpenCustom(file, &impl);

    const char *str = "This is text\n";
    dtpSend(desc, str, strlen(str));
    dtpClose(desc);
    return 0;
}