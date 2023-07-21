#include "nmw/dtp.h"
#include <cstring>
#include "stdio.h"

#define MAX_SIZE 16


enum DtpType{
    DEBR_NONE,
    DEBR_LINK,
    DEBR_FILE,
    DEBR_CUSTOM,
    DEBR_SHM
};

struct DtpObject{
    int fd;
    int type;
    int is_enabled;
    void *user_data;
    DtpImplementaion implementaion;
    //DtpWriteFuncT writeFunc;
    //DtpReadFuncT readFunc;
    //DtpFlushFuncT flushFunc;
    //DtpCloseFuncT closeFunc;
    DtpObject(){
        is_enabled = 0;
    }
};


struct DtpFileData{
    char filename[256];
    FILE *file;
};

static size_t fileWrite(void *user_data, const void *buf, size_t size){
    DtpFileData *fileData = (DtpFileData *)user_data;
    return fwrite(buf, sizeof(int), size / 4, fileData->file);
}

static size_t fileRead(void *user_data, void *buf, size_t size){
    DtpFileData *fileData = (DtpFileData *)user_data;

    int fsize0 = ftell(fileData->file);
    fseek(fileData->file, 0, SEEK_END);
    int fsize = ftell(fileData->file);
    fseek(fileData->file, fsize0, SEEK_SET);

    int elements = fread(buf, sizeof(int), size / 4, fileData->file);
    int *dst = (int*)buf;
    if(dst[0] == 0x6370000) {
        fseek(fileData->file, fsize0, SEEK_SET);
        return 0;
    }

    return elements;
}

static int fileFlush(void *user_data){
    DtpFileData *fileData = (DtpFileData *)user_data;
    return fflush(fileData->file);
}

static int fileClose(void *user_data){
    DtpFileData *fileData = (DtpFileData *)user_data;
    fclose(fileData->file);
    delete fileData;
    return 0;
}

#ifdef __NM__
__attribute__ ((section (".data.nmw.dtp"))) 
#endif
static DtpObject objects[MAX_SIZE];

extern "C"{
    int dtpOpenCustom(void *user_data, DtpReadFuncT readFunc, DtpWriteFuncT writeFunc){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                objects[i].user_data = user_data;
                objects[i].type = DEBR_SHM;
                objects[i].implementaion.read = readFunc;
                objects[i].implementaion.write = writeFunc;
                objects[i].implementaion.flush = 0;
                objects[i].implementaion.close = 0;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }

    int dtpOpenLink(int port){
        return -1;
    }
    int dtpOpenDma(){
        return -1;
    }

    int dtpOpenDesc(int desc){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                int no = desc - 1;
                objects[i].user_data = objects[no].user_data;
                objects[i].type = objects[no].type;
                objects[i].implementaion.read = objects[no].implementaion.read;
                objects[i].implementaion.write = objects[no].implementaion.write;
                objects[i].implementaion.close = objects[no].implementaion.close;
                objects[i].implementaion.flush = objects[no].implementaion.flush;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }


    int dtpOpenFile(const char *filename, const char *mode){
        printf("open desc via file\n");
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                DtpFileData *data = new DtpFileData();
                if(data == 0){
                    return -1;
                }
                strcpy(data->filename, filename);
                data->file = fopen(filename, mode);                
                objects[i].user_data = data;
                objects[i].type = DEBR_FILE;
                objects[i].implementaion.read = fileRead;
                objects[i].implementaion.write = fileWrite;
                objects[i].implementaion.flush = fileFlush;
                objects[i].implementaion.close = fileClose;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }
#ifndef __NM__
    int dtpOpenFileDesc(int fd, const char *mode){
        printf("open desc via file desc\n");
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                DtpFileData *data = new DtpFileData();
                if(data == 0){
                    return -1;
                }
                data->file = fdopen(fd, mode);                
                objects[i].user_data = data;
                objects[i].type = DEBR_FILE;
                objects[i].implementaion.read = fileRead;
                objects[i].implementaion.write = fileWrite;
                objects[i].implementaion.flush = fileFlush;
                objects[i].implementaion.close = fileClose;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }
#endif //__NM__

    int dtpWrite(int desc, const void *data, size_t size){
        int no = desc - 1;
        return objects[no].implementaion.write(objects[no].user_data, data, size);
    }

    int dtpRead(int desc, void *data, size_t size){
        int no = desc - 1;
        return objects[no].implementaion.read(objects[no].user_data, data, size);
    }

	int dtpWriteM(int desc, const void *data, size_t size, int width, int stride){
		return -1;
	}

    int dtpReadM(int desc, void *data, size_t size, int width, int stride){
		return -1;
	}

	int dtpWriteP(int desc, const void *data, size_t size, int offset){
		return -1;
	}

    int dtpReadP(int desc, void *data, size_t size, int offset){
		return -1;
	}	

    int dtpFlush(int desc){
        int no = desc - 1;
        if(objects[no].implementaion.flush){
            return objects[no].implementaion.flush(objects[no].user_data);
        } else {
            return 0;
        }
    }

    int dtpClose(int desc){
        int no = desc - 1;
        if(objects[no].implementaion.close){
            objects[no].implementaion.close(objects[no].user_data);
        }
        objects[no].is_enabled = 0;
        return 0;
    }

    int dtpOpen(void *user_data, DtpImplementaion *implementation){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                objects[i].user_data = user_data;
                objects[i].type = DEBR_FILE;
                objects[i].implementaion.read = fileRead;
                objects[i].implementaion.write = fileWrite;
                objects[i].implementaion.flush = fileFlush;
                objects[i].implementaion.close = fileClose;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
    }
}