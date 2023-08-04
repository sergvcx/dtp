#include "dtp/dtp.h"

struct DtpObject{
    int fd;
    int is_used;
    void *com_spec;
    DtpImplementation implementaion;
};


static DtpObject dtp_objects[DTP_OPEN_MAX];

static int dtpIsInited = 0;

extern "C"{

    inline int getIndexFromDesc(int desc){
        return desc - 1;
    }


    void *dtpGetUserData(int desc){
        int i = getIndexFromDesc(desc);
        return dtp_objects[i].com_spec;
    }

    int dtpOpenCustom(void *com_spec, DtpImplementation *implementation){
        for(int i = 0; i < DTP_OPEN_MAX; i++){
            if(dtp_objects[i].is_used == 0){
                dtp_objects[i].fd = i + 1;
                dtp_objects[i].com_spec = com_spec;
                dtp_objects[i].implementaion.recv_func  = implementation->recv_func;
                dtp_objects[i].implementaion.send_func = implementation->send_func;
                dtp_objects[i].implementaion.status_func = implementation->status_func;                
                dtp_objects[i].implementaion.destroy_func = implementation->destroy_func;
                dtp_objects[i].is_used = 1;
                return dtp_objects[i].fd;
            }
        }
        return -1;
    }

    int dtpOpenDesc(int desc){
        int no = getIndexFromDesc(desc);
        return dtpOpenCustom(dtp_objects[no].com_spec, &dtp_objects[no].implementaion);
    }

    int dtpSend(int desc, const void *data, size_t size){
        DtpAsync task;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.sigevent = DTP_EVENT_NONE;
        dtpAsyncSend(desc, &task);        
        return 0;
    }

    int dtpRecv(int desc, void *data, size_t size){        
        DtpAsync task;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.sigevent = DTP_EVENT_NONE;
        dtpAsyncRecv(desc, &task);        
        return 0;
        
    }

	int dtpSendM(int desc, const void *data, size_t size, int width, int stride){
        DtpAsync task;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.sigevent = DTP_EVENT_NONE;
        task.stride = stride;
        task.width = width;        
        dtpAsyncSend(desc, &task);
        return dtpAsyncWait(desc, &task);
    }

    int dtpRecvM(int desc, void *data, size_t size, int width, int stride){
        DtpAsync task;
        task.buf = (volatile void*)data;
        task.sigevent = DTP_EVENT_NONE;
        task.nwords = size;
        task.stride = stride;
        task.width = width;
        dtpAsyncRecv(desc, &task);
        return dtpAsyncWait(desc, &task);
    }

    int dtpConnect(int desc){

    }

    int dtpListen(int desc){

    }


    int dtpClose(int desc){
        int no = getIndexFromDesc(desc);    
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        impl->destroy_func(com_spec);
        dtp_objects[no].is_used = 0;
        return 0;
    }

    //void dtpSetCallback(int desc, DtpNotifyFunctionT notifyFunc, DtpSignalData *signal);
    int dtpAsyncRecv(int desc, DtpAsync *task){    
        int no = getIndexFromDesc(desc);    
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return impl->recv_func(com_spec, task);
    }

    int dtpAsyncSend(int desc, DtpAsync *task){
        int no = getIndexFromDesc(desc);    
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return impl->send_func(com_spec, task);
    }

    int dtpAsyncStatus(int desc, DtpAsync *task){
        int no = getIndexFromDesc(desc);    
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return impl->status_func(com_spec, task);
    }

    int dtpAsyncWait(int desc, DtpAsync *task){
        int no = getIndexFromDesc(desc);    
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        int error = 0;
        while(1){
            error = impl->status_func(com_spec, task);
            if(error != DTP_ST_IN_PROCESS) break;
        };
        return error;
    }


}