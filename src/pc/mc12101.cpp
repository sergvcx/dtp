#include "dtp/mc12101-host.h"
#include "mc12101load.h"
#include "dtp/dtp.h"
#include "stdio.h"

const int DTP_MC12101_HOST_MESSAGE_SIZE = 2;
struct Mc12101PloadFile{
    FILE *message_file;
    PL_Access *access;
};

static void getMessageFromNm(Mc12101PloadFile *data, int *dst_message){
    size_t start = ftell(data->message_file);
	size_t tell;
	do {
		fseek(data->message_file, 0, SEEK_END);
		tell = ftell(data->message_file);
	}
    while (tell - start < DTP_MC12101_HOST_MESSAGE_SIZE * sizeof(int));  //fteel in bytes or words

	fseek(data->message_file, start, SEEK_SET);

    fread(dst_message, sizeof(int), DTP_MC12101_HOST_MESSAGE_SIZE, data->message_file);
}

static int mc12101Send(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    getMessageFromNm(data, message);

    PL_WriteMemBlock(data->access, (PL_Word *)aio->buf, message[0], aio->nwords);

    int status = DTP_ST_DONE;
    PL_WriteMemBlock(data->access, (PL_Word *)&status, message[1], 1);

    return DTP_OK;
}

static int mc12101Recv(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    int message[DTP_MC12101_HOST_MESSAGE_SIZE];
    getMessageFromNm(data, message);

    PL_ReadMemBlock(data->access, (PL_Word *)aio->buf, message[0], aio->nwords);

    int status = DTP_ST_DONE;
    PL_WriteMemBlock(data->access, (PL_Word *)&status, message[1], 1);

    return DTP_OK;
}

static int mc12101Status(void *com_spec, DtpAsync *aio){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    return DTP_ST_DONE;
}

static int mc12101Destroy(void *com_spec){
    Mc12101PloadFile *data = (Mc12101PloadFile *)com_spec;

    return 0;
}

extern "C"{

    int dtpOpenPloadFileHost(PL_Access *access, const char *filename){
        Mc12101PloadFile *com_spec = new Mc12101PloadFile();
        if(com_spec == 0) return -1;

        com_spec->message_file = fopen(filename, "wb+");
        com_spec->access = access;

        if(com_spec->message_file == 0) return -1;

        DtpImplementation impl;
        impl.send = mc12101Send;
        impl.recv = mc12101Recv;
        impl.get_status = mc12101Status;
        impl.destroy = mc12101Destroy;
        impl.connect = 0;
        impl.listen = 0;

        
        return dtpOpenCustom(com_spec, &impl);  
    }

}