#include "dtp/dtp.h"

#define MAX_SIZE 16


struct DtpObject{
    int fd;
    int is_enabled;
    void *user_data;
    DtpImplemention implementaion;
    DtpObject(){
        is_enabled = 0;
    }
};




static DtpObject objects[MAX_SIZE];

extern "C"{

    int dtpOpen(void *user_data, DtpImplemention *implementation){
        for(int i = 0; i < MAX_SIZE; i++){
            if(objects[i].is_enabled == 0){
                objects[i].fd = i + 1;
                objects[i].user_data = user_data;
                objects[i].implementaion.recv  = implementation->recv;
                objects[i].implementaion.send = implementation->send;
                objects[i].implementaion.recv_matrix  = implementation->recv_matrix;
                objects[i].implementaion.send_matrix = implementation->send_matrix;                
                objects[i].implementaion.flush = implementation->flush;
                objects[i].implementaion.destroy = implementation->destroy;
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
                objects[i].implementaion.recv = objects[no].implementaion.recv;
                objects[i].implementaion.send = objects[no].implementaion.send;
                objects[i].implementaion.destroy = objects[no].implementaion.destroy;
                objects[i].implementaion.flush = objects[no].implementaion.flush;
                objects[i].is_enabled = 1;
                return objects[i].fd;
            }
        }
        return -1;
    }

    size_t dtpWrite(int desc, const void *data, size_t size){
        return dtpSend(desc, data, size);
    }

    size_t dtpRead(int desc, void *data, size_t size){
        return dtpRecv(desc, data, size);
    }

	size_t dtpWriteM(int desc, const void *data, size_t size, int width, int stride){
        return dtpSendM(desc, data, size, width, stride);
	}

    size_t dtpReadM(int desc, void *data, size_t size, int width, int stride){
        return dtpRecvM(desc, data, size, width, stride);
	}

    size_t dtpSend(int desc, const void *data, size_t size){
        int no = desc - 1;
        if(objects[no].implementaion.send){
            return objects[no].implementaion.send(objects[no].user_data, data, size);
        } else {
            return 0;
        }
    }

    size_t dtpRecv(int desc, void *data, size_t size){
        int no = desc - 1;
        if(objects[no].implementaion.recv){
            return objects[no].implementaion.recv(objects[no].user_data, data, size);
        } else {
            return 0;
        }
    }

	size_t dtpSendM(int desc, const void *data, size_t size, int width, int stride){
        int no = desc - 1;
        if(objects[no].implementaion.send_matrix){
            return objects[no].implementaion.send_matrix(objects[no].user_data, data, size, width, stride);
        } else {
            return 0;
        }
		
    }

    size_t dtpRecvM(int desc, void *data, size_t size, int width, int stride){
        int no = desc - 1;
        if(objects[no].implementaion.recv_matrix){
		    return objects[no].implementaion.recv_matrix(objects[no].user_data, data, size, width, stride);
        } else {
            return 0;
        }
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
        if(objects[no].implementaion.destroy){
            objects[no].implementaion.destroy(objects[no].user_data);
        }
        objects[no].is_enabled = 0;
        return 0;
    }

    //void dtpSetCallback(int desc, DtpNotifyFunctionT notifyFunc, DtpSignalData *signal);
    size_t dtpAsyncRecv(DtpASync *task){
        
        //dtpSetCallback(task->desc, task->sigevent, task->sigval);
        //return objects[no].implementaion.read(objects[no].user_data, data, size);
        return 0;
    }

    size_t dtpAsyncSend(DtpASync *task){
        return 0;
    }


}