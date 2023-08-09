#include "dtp/mc12101-host.h"
#include "dtp/dtp.h"
#include "stdio.h"
#include "malloc.h"


const int DTP_MC12101_HOST_MESSAGE_SIZE = 2;
const int DTP_MC12101_STATUS_COUNT = 8;

struct Mc12101PloadFile{
    FILE *file;
    DtpAsync *pool[DTP_MC12101_STATUS_COUNT];
    int status[DTP_MC12101_STATUS_COUNT];
};




static int mc12101Send(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];

    message[0] = (int)aio->buf;
    

    bool founded = false;
    while(!founded){
        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            if(data->status[i] == DTP_ST_DONE){
                data->status[i] = DTP_ST_IN_PROCESS;
                data->pool[i] = aio;
                message[1] = (int)(&data->status[i]);
                founded = true;
                break;
            }
        }
    }

    fwrite(message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, file);
    fflush(file);

    return DTP_OK;
}

static int mc12101Recv(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    volatile int status = 0;

    message[0] = (int)aio->buf;
    
    
    bool founded = false;
    while(!founded){
        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            if(data->status[i] == DTP_ST_DONE){
                data->status[i] = DTP_ST_IN_PROCESS;
                data->pool[i] = aio;
                message[1] = (int)(&data->status[i]);
                founded = true;
                break;
            }
        }
    }

    fwrite(message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, file);
    fflush(file);

    return DTP_OK;
}

static int mc12101Status(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    FILE *file = (FILE *)data->file;

    int ind = -1;
    for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
        if(aio == data->pool[i]){
            ind = i;
            break;
        }
    }
    if(ind < 0) return DTP_ST_ERROR;

    if(data->status[ind] == DTP_ST_IN_PROCESS) return DTP_ST_IN_PROCESS;

    if(data->status[ind] == DTP_ST_DONE){
        if(aio->sigevent == DTP_EVENT_CALLBACK){
            aio->callback(aio->cb_data);
        }
        data->pool[ind] = 0;    
    }
    return DTP_ST_DONE;
}

static int mc12101Destroy(void *com_spec){
    //Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;
    //FILE *file = (FILE *)data->file;
    //fclose(file);
    //delete data;
    return 0;
}

static Mc12101PloadFile Com_spec;
extern "C"{

    int dtpOpenPloadFile(const char *filename){
        //Mc12101PloadFile *com_spec = new Mc12101PloadFile();
        Mc12101PloadFile *com_spec = &Com_spec;
        if(com_spec == 0) return -1;

        com_spec->file = fopen(filename, "wb");
        if(com_spec->file == 0) {
           // delete com_spec;
            return -1;
        }

        for(int i = 0; i < DTP_MC12101_STATUS_COUNT; i++){
            com_spec->status[i] = DTP_ST_DONE;
            com_spec->pool[i] = 0;
        }

        DtpImplementation impl;
        impl.send_func = mc12101Send;
        impl.recv_func = mc12101Recv;
        impl.get_status_func = mc12101Status;
        impl.destroy_func = mc12101Destroy;

        
        return dtpOpenCustom(com_spec, &impl);  
    }

}