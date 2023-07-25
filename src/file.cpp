#include "dtp/dtp.h"
#include "stdio.h"
#include <cstring>

struct DtpFileData{
    char filename[256];
    FILE *file;
};

extern "C" {
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

}