#include "dtp/mc12101-host.h"
#include "dtp/dtp.h"
#include "stdio.h"


const int DTP_MC12101_HOST_MESSAGE_SIZE = 2;
const int DTP_MC12101_STATUS_COUNT = 8;

struct Mc12101PloadFile{
    FILE *file;
};

static int mc12101Send(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    volatile int status = 0;

    message[0] = (int)aio->buf;
    message[1] = (int)(&status);

    fwrite(message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, file);
    fflush(file);

    while(status == 0);
    return DTP_OK;
}

static int mc12101Recv(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    volatile int status = 0;

    message[0] = (int)aio->buf;
    message[1] = (int)(&status);

    fwrite(message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, file);
    fflush(file);
    
    while(status == 0);

    return DTP_OK;
}

static int mc12101Status(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    return DTP_ST_DONE;
}

static int mc12101Destroy(void *com_spec){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;
    fclose(file);
    delete data;
    return 0;
}

extern "C"{

    int dtpOpenPloadFile(const char *filename){
        Mc12101PloadFile *com_spec = new Mc12101PloadFile();
        if(com_spec == 0) return -1;

        com_spec->file = fopen(filename, "r");
        if(com_spec->file == 0) {
            delete com_spec;
            return -1;
        }

        DtpImplementation impl;
        impl.send_func = mc12101Send;
        impl.recv_func = mc12101Recv;
        impl.get_status_func = mc12101Status;
        impl.destroy_func = mc12101Destroy;

        
        return dtpOpenCustom(com_spec->file, &impl);  
    }

}