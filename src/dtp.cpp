#include "dtp/dtp.h"
#include "dtp-core.h"

#define DTP_OPEN_MAX 16

static DtpObject dtp_objects[DTP_OPEN_MAX];

static int dtpIsInited = 0;

extern "C"{

    void dtpInit(){
        for(int i = 0; i < DTP_OPEN_MAX; i++){
            dtp_objects[i].is_used = 0;
        }
    }

    inline int getIndexFromDesc(int desc){
        return desc - 1;
    }

    DtpObject *getDtpObject(int desc){
        int i = desc - 1;
        return &dtp_objects[i];
    }

    void *dtpGetUserData(int desc){
        int i = desc - 1;
        return dtp_objects[i].implementaion.user_data;
    }

    int dtpOpen(DtpImplementation *implementation){
        if(dtpIsInited == 0){
            dtpInit();
            dtpIsInited = 1;
        }
        for(int i = 0; i < DTP_OPEN_MAX; i++){
            if(dtp_objects[i].is_used == 0){
                dtp_objects[i].fd = i + 1;
                dtp_objects[i].implementaion.user_data = implementation->user_data;
                dtp_objects[i].implementaion.recv  = implementation->recv;
                dtp_objects[i].implementaion.send = implementation->send;
                dtp_objects[i].implementaion.recv_matrix  = implementation->recv_matrix;
                dtp_objects[i].implementaion.send_matrix = implementation->send_matrix;                
                dtp_objects[i].implementaion.flush = implementation->flush;
                dtp_objects[i].implementaion.destroy = implementation->destroy;
                dtp_objects[i].is_used = 1;
                return dtp_objects[i].fd;
            }
        }
        return -1;
    }

    int dtpOpenDesc(int desc){
        int no = desc - 1;
        return dtpOpen(&dtp_objects[no].implementaion);
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
        if(dtp_objects[no].implementaion.send){
            return dtp_objects[no].implementaion.send(dtp_objects[no].implementaion.user_data, data, size);
        } else {
            return 0;
        }
    }

    size_t dtpRecv(int desc, void *data, size_t size){
        int no = desc - 1;
        if(dtp_objects[no].implementaion.recv){
            return dtp_objects[no].implementaion.recv(dtp_objects[no].implementaion.user_data, data, size);
        } else {
            return 0;
        }
    }

	size_t dtpSendM(int desc, const void *data, size_t size, int width, int stride){
        int no = desc - 1;
        if(dtp_objects[no].implementaion.send_matrix){
            return dtp_objects[no].implementaion.send_matrix(dtp_objects[no].implementaion.user_data, data, size, width, stride);
        } else {
            return 0;
        }
		
    }

    size_t dtpRecvM(int desc, void *data, size_t size, int width, int stride){
        int no = desc - 1;
        if(dtp_objects[no].implementaion.recv_matrix){
		    return dtp_objects[no].implementaion.recv_matrix(dtp_objects[no].implementaion.user_data, data, size, width, stride);
        } else {
            return 0;
        }
    }

    int dtpFlush(int desc){
        int no = desc - 1;
        if(dtp_objects[no].implementaion.flush){
            return dtp_objects[no].implementaion.flush(dtp_objects[no].implementaion.user_data);
        } else {
            return 0;
        }
    }

    int dtpClose(int desc){
        int no = desc - 1;
        if(dtp_objects[no].implementaion.destroy){
            dtp_objects[no].implementaion.destroy(dtp_objects[no].implementaion.user_data);
        }
        dtp_objects[no].is_used = 0;
        return 0;
    }

    //void dtpSetCallback(int desc, DtpNotifyFunctionT notifyFunc, DtpSignalData *signal);
    size_t dtpAsyncRecv(DtpASync *task){
        
        //dtpSetCallback(task->desc, task->sigevent, task->sigval);
        //return dtp_objects[no].implementaion.read(dtp_objects[no].user_data, data, size);
        return 0;
    }

    size_t dtpAsyncSend(DtpASync *task){
        return 0;
    }


}