#include "dtp/dtp.h"
#include "stdio.h"

static int fileRecv(void *com_spec, DtpAsync *aio){
    FILE *file = (FILE*)com_spec;
    size_t rsize = fread((void*)aio->buf, sizeof(int), aio->nwords, file);
    return 0;
}

static int fileSend(void *com_spec, DtpAsync *aio){
    FILE *file = (FILE*)com_spec;
    size_t wsize = fwrite((void*)aio->buf, sizeof(int), aio->nwords, file);
    fflush(file);
    return 0;
}

static int fileDestroy(void *com_spec){
    FILE *file = (FILE*)com_spec;
    return fclose(file);
}

static int fileStatus(void *com_spec, DtpAsync *aio){
    return DTP_ST_DONE;
}

int dtpOpenFile(const char *filename, const char *mode){
    DtpImplementation impl;

    FILE *file = fopen(filename, mode);

    impl.recv_func = fileRecv;
    impl.send_func = fileSend;
    impl.get_status_func = fileStatus;
    impl.destroy_func = fileDestroy;
    return dtpOpenCustom(file, &impl);
}