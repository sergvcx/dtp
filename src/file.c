#include "dtp/dtp.h"
#include "stdio.h"
#include "malloc.h"

typedef struct {
    FILE *file_in;
    FILE *file_out;
}FileData;

static int fileRecv(void *com_spec, DtpAsync *aio){
    FILE *file = (FILE*)com_spec;
	
	size_t start = ftell(file);
	size_t tell;
	do {
		fseek(file, 0, SEEK_END);
		tell = ftell(file);
	}
    while (tell - start < aio->nwords * sizeof(int));  //fteel in bytes or words
	fseek(file, start, SEEK_SET);

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
	if (file == 0) {
		file = fopen(filename, "w");
		if (file) {
			fclose(file);
			file = fopen(filename, mode);
			if (file == 0) return -1;
		}
		else return -1;
	}

    impl.recv_func = fileRecv;
    impl.send_func = fileSend;
    impl.get_status_func = fileStatus;
    impl.destroy_func = fileDestroy;
    return dtpOpenCustom(file, &impl);
}


static int file2Recv(void *com_spec, DtpAsync *aio){
    FileData *fileData = (FileData*)com_spec;
    FILE *file = fileData->file_in;
	
	size_t start = ftell(file);
	size_t tell;
	do {
		fseek(file, 0, SEEK_END);
		tell = ftell(file);
	}
    while (tell - start < aio->nwords * sizeof(int));  //fteel in bytes or words
	fseek(file, start, SEEK_SET);

    size_t rsize = fread((void*)aio->buf, sizeof(int), aio->nwords, file);
    return 0;
}

static int file2Send(void *com_spec, DtpAsync *aio){
    FileData *fileData = (FileData*)com_spec;
    FILE *file = fileData->file_out;

    size_t wsize = fwrite((void*)aio->buf, sizeof(int), aio->nwords, file);
    fflush(file);
    return 0;
}

static int file2Destroy(void *com_spec){
    FileData *fileData = (FileData*)com_spec;
    int error_in = fclose(fileData->file_in);
    int error_out = fclose(fileData->file_out);
    free(fileData);
    return error_in | error_out;
}

static int file2Status(void *com_spec, DtpAsync *aio){
    return DTP_ST_DONE;
}

int dtpOpenFile2(const char *file_input, const char *file_output){
    DtpImplementation impl;

    FileData *fileData = malloc(sizeof(FileData));
    if(fileData == 0) return -1;

    fileData->file_in = fopen(file_input, "r");
    if(fileData->file_in == 0){
        return -1;
    }

    fileData->file_out = fopen(file_output, "w");
    if(fileData->file_out == 0){
        fclose(fileData->file_out);
        return -1;
    }


    impl.recv_func = file2Recv;
    impl.send_func = file2Send;
    impl.get_status_func = file2Status;
    impl.destroy_func = file2Destroy;
    return dtpOpenCustom(fileData, &impl);
}