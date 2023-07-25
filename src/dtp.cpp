#include "dtp/dtp.h"

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



    size_t dtpWrite(int desc, const void *data, size_t size){
        int no = desc - 1;
        return objects[no].implementaion.write(objects[no].user_data, data, size);
    }

    size_t dtpRead(int desc, void *data, size_t size){
        int no = desc - 1;
        return objects[no].implementaion.read(objects[no].user_data, data, size);
    }

	size_t dtpWriteM(int desc, const void *data, size_t size, int width, int stride){
        int no = desc - 1;
		return objects[no].implementaion.write_matrix(objects[no].user_data, data, size, width, stride);
	}

    size_t dtpReadM(int desc, void *data, size_t size, int width, int stride){
		int no = desc - 1;
		return objects[no].implementaion.read_matrix(objects[no].user_data, data, size, width, stride);
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

    //void dtpSetCallback(int desc, DtpNotifyFunctionT notifyFunc, DtpSignalData *signal);
    size_t dtpReadA(DtpASync *task){
        
        //dtpSetCallback(task->desc, task->sigevent, task->sigval);
        //return objects[no].implementaion.read(objects[no].user_data, data, size);
        return 0;
    }

    size_t dtpWriteA(DtpASync *task){
        return 0;
    }


}