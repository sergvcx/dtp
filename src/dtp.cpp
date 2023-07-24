#include "dtp/dtp.h"
#include <cstring>
#include "stdio.h"
#include "dtp/ringbuffer.h"

#define MAX_SIZE 16


struct DtpObject{
    int fd;
    int is_enabled;
    void *user_data;
    DtpImplementaion implementaion;
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
    return fread(buf, sizeof(int), size / 4, fileData->file);
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

static DtpObject objects[MAX_SIZE];

extern "C"{

    int dtpOpen(void *user_data, DtpImplementaion *implementation){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                objects[i].user_data = user_data;
                objects[i].implementaion.read  = implementation->read;
                objects[i].implementaion.write = implementation->write;
                objects[i].implementaion.read_matrix  = implementation->read_matrix;
                objects[i].implementaion.write_matrix = implementation->write_matrix;                
                objects[i].implementaion.flush = implementation->flush;
                objects[i].implementaion.close = implementation->close;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }

    int dtpOpenDesc(int desc){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                int no = desc - 1;
                objects[i].user_data = objects[no].user_data;
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
        DtpFileData *data = new DtpFileData();
        if(data == 0){
            return -1;
        }
        strcpy(data->filename, filename);

        DtpImplementaion impl;
        data->file = fopen(filename, mode);
        impl.read = fileRead;
        impl.write = fileWrite;
        impl.flush = fileFlush;
        impl.close = fileClose;
        return dtpOpen(data, &impl);
    }
#ifndef __NM__
    int dtpOpenFileDesc(int fd, const char *mode){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                DtpFileData *data = new DtpFileData();
                if(data == 0){
                    return -1;
                }
                data->file = fdopen(fd, mode);                
                objects[i].user_data = data;
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

    size_t dtpWrite(int desc, const void *data, size_t size){
        int no = desc - 1;
        return objects[no].implementaion.write(objects[no].user_data, data, size);
    }

    size_t dtpRead(int desc, void *data, size_t size){
        int no = desc - 1;
        return objects[no].implementaion.read(objects[no].user_data, data, size);
    }

	size_t dtpWriteM(int desc, const void *data, size_t size, int width, int stride){
		return -1;
	}

    size_t dtpReadM(int desc, void *data, size_t size, int width, int stride){
		return -1;
	}

	size_t dtpWriteP(int desc, const void *data, size_t size, int offset){
		return -1;
	}

    size_t dtpReadP(int desc, void *data, size_t size, int offset){
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


}