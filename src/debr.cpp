#include "nmw/debr.h"
#include <cstring>
#include "stdio.h"

#define MAX_SIZE 16


enum DebrType{
    DEBR_NONE,
    DEBR_LINK,
    DEBR_FILE,
    DEBR_CUSTOM,
    DEBR_SHM
};

struct DebrObject{
    int fd;
    int type;
    int is_enabled;
    void *user_data;
    DebrWriteFuncT writeFunc;
    DebrReadFuncT readFunc;
    DebrFlushFuncT flushFunc;
    DebrCloseFuncT closeFunc;
    DebrObject(){
        is_enabled = 0;
    }
};


struct DebrFileData{
    char filename[256];
    FILE *file;
};

static size_t fileWrite(void *user_data, const void *buf, size_t size){
    DebrFileData *fileData = (DebrFileData *)user_data;
    return fwrite(buf, sizeof(int), size / 4, fileData->file);
}

static size_t fileRead(void *user_data, void *buf, size_t size){
    DebrFileData *fileData = (DebrFileData *)user_data;

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
    DebrFileData *fileData = (DebrFileData *)user_data;
    return fflush(fileData->file);
}

#ifdef __NM__
__attribute__ ((section (".data.nmw.debr"))) 
#endif
static DebrObject objects[MAX_SIZE];

extern "C"{
    int debrOpenCustom(void *user_data, DebrReadFuncT readFunc, DebrWriteFuncT writeFunc){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                objects[i].user_data = user_data;
                objects[i].type = DEBR_SHM;
                objects[i].readFunc = readFunc;
                objects[i].writeFunc = writeFunc;
                objects[i].flushFunc = 0;
                objects[i].closeFunc = 0;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }

    int debrOpenLink(int port){
        return -1;
    }
    int debrOpenDma(){
        return -1;
    }

    int debrOpenDesc(int desc){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                int no = desc - 1;
                objects[i].user_data = objects[no].user_data;
                objects[i].type = objects[no].type;
                objects[i].readFunc = objects[no].readFunc;
                objects[i].writeFunc = objects[no].writeFunc;
                objects[i].closeFunc = 0;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }


    int debrOpenFile(const char *filename, const char *mode){
        printf("open desc via file\n");
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                DebrFileData *data = new DebrFileData();
                if(data == 0){
                    return -1;
                }
                strcpy(data->filename, filename);
                data->file = fopen(filename, mode);                
                objects[i].user_data = data;
                objects[i].type = DEBR_FILE;
                objects[i].readFunc = fileRead;
                objects[i].writeFunc = fileWrite;
                objects[i].flushFunc = fileFlush;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }
#ifndef __NM__
    int debrOpenFileDesc(int fd, const char *mode){
        printf("open desc via file desc\n");
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                DebrFileData *data = new DebrFileData();
                if(data == 0){
                    return -1;
                }
                data->file = fdopen(fd, mode);                
                objects[i].user_data = data;
                objects[i].type = DEBR_FILE;
                objects[i].readFunc = fileRead;
                objects[i].writeFunc = fileWrite;
                objects[i].flushFunc = fileFlush;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }
#endif //__NM__

    int debrWrite(int desc, const void *data, size_t size){
        int no = desc - 1;
        return objects[no].writeFunc(objects[no].user_data, data, size);
    }

    int debrRead(int desc, void *data, size_t size){
        int no = desc - 1;
        return objects[no].readFunc(objects[no].user_data, data, size);
    }

	int debrWriteM(int desc, const void *data, size_t size, int width, int stride){
		return -1;
	}

    int debrReadM(int desc, void *data, size_t size, int width, int stride){
		return -1;
	}

	int debrWriteP(int desc, const void *data, size_t size, int offset){
		return -1;
	}

    int debrReadP(int desc, void *data, size_t size, int offset){
		return -1;
	}	

    int debrFlush(int desc){
        int no = desc - 1;
        if(objects[no].flushFunc){
            return objects[no].flushFunc(objects[no].user_data);
        } else {
            return 0;
        }
    }

    int debrClose(int desc){
        int no = desc - 1;
        if(objects[no].closeFunc){
            objects[no].closeFunc(objects[no].user_data);
        }
        objects[no].is_enabled = 0;
        return 0;
    }
}