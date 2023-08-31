#include "dtp/dtp.h"
#include "dtp_utils.h"


struct DtpObject{
    int fd;
    int mode;
    int is_used;
    void *com_spec;
    DtpImplementation implementaion;
};

typedef struct {
    DtpObject dtp_objects[DTP_OPEN_MAX];    
}DtpContext;

static DtpContext dtp_context;

static int dtpIsInited = 0;

extern "C"{

    void dtpDefaultCallback(void *data){
        int *status = (int *)data;
        *status = DTP_ST_DONE;
    }

    inline int getIndexFromDesc(int desc){
        return desc - 1;
    }

    int dtpOpen(int mode){
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        for(int i = 0; i < DTP_OPEN_MAX; i++){
            if(dtp_objects[i].is_used == 0){
                dtp_objects[i].fd = i + 1;
                dtp_objects[i].is_used = 1;
                dtp_objects[i].mode = mode;
                return dtp_objects[i].fd;
            }
        }
        return -1;
    }

    int dtpBind(int desc, void* com_spec, DtpImplementation *implementation){
        int i = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        dtp_objects[i].com_spec = com_spec;
        dtp_objects[i].implementaion.recv  = implementation->recv;
        dtp_objects[i].implementaion.send = implementation->send;
        dtp_objects[i].implementaion.update_status = implementation->update_status;                
        dtp_objects[i].implementaion.destroy = implementation->destroy;        
        return DTP_OK;
    }


    void *dtpGetComSpec(int desc){
        int i = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        return dtp_objects[i].com_spec;
    }

    int dtpOpenCustom(void *com_spec, DtpImplementation *implementation){
        int desc = dtpOpen(DTP_READ_WRITE);
        if(desc < 0) return -1;
        int ok = dtpBind(desc, com_spec, implementation);
        return desc;        
    }

    int dtpOpenDesc(int desc){
        int no = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        return dtpOpenCustom(dtp_objects[no].com_spec, &dtp_objects[no].implementaion);
    }

    int dtpSend(int desc, const void *data, size_t size){
        DtpAsync task;
        int status = DTP_ST_IN_PROCESS;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.callback = 0;
        task.type = DTP_TASK_1D;
        int error = dtpAsyncSend(desc, &task);
        if(error) return error;
        return dtpAsyncWait(desc, &task);
    }

    int dtpRecv(int desc, void *data, size_t size){        
        DtpAsync task;
        int status = DTP_ST_IN_PROCESS;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.callback = 0;
        task.type = DTP_TASK_1D;
        int error = dtpAsyncRecv(desc, &task);
        if(error) return error;
        return dtpAsyncWait(desc, &task);   
    }

	int dtpSendM(int desc, const void *data, size_t size, int width, int stride){
        DtpAsync task;
        int status = DTP_ST_IN_PROCESS;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.stride = stride;
        task.width = width;
        task.callback = 0;
        task.type = DTP_TASK_2D;
        int error = dtpAsyncSend(desc, &task);
        if(error) return error;
        return dtpAsyncWait(desc, &task);
    }

    int dtpRecvM(int desc, void *data, size_t size, int width, int stride){
        DtpAsync task;
        int status = DTP_ST_IN_PROCESS;
        task.buf = (volatile void*)data;
        task.nwords = size;
        task.stride = stride;
        task.width = width;
        task.callback = 0;
        task.type = DTP_TASK_2D;
        int error = dtpAsyncRecv(desc, &task);
        if(error) return error;
        return dtpAsyncWait(desc, &task);
    }

    int dtpConnect(int desc){
        int no = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return DTP_ERROR;
    }

    int dtpListen(int desc){
        int no = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return DTP_ERROR;
    }

    int dtpGetMode(int desc){
        int no = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        return dtp_objects[no].mode;
    }


    int dtpClose(int desc){
        int no = getIndexFromDesc(desc);
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        int error = 0;
        if(impl->destroy){
            error = impl->destroy(com_spec);
        }        
        dtp_objects[no].is_used = 0;
        return error;
    }
    
    int dtpAsyncRecv(int desc, DtpAsync *task){    
        int no = getIndexFromDesc(desc);    
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        if( (dtp_objects[no].mode & DTP_READ_ONLY) == 0) return DTP_ERROR;
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return impl->recv(com_spec, task);
    }

    int dtpAsyncSend(int desc, DtpAsync *task){
        int no = getIndexFromDesc(desc);            
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        if( (dtp_objects[no].mode & DTP_WRITE_ONLY) == 0) return DTP_ERROR;
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return impl->send(com_spec, task);
    }

    int dtpAsyncStatus(int desc, DtpAsync *task){
        int no = getIndexFromDesc(desc);    
        DtpObject *dtp_objects = dtp_context.dtp_objects;
        DtpImplementation *impl = &dtp_objects[no].implementaion;
        void* com_spec = dtp_objects[no].com_spec;
        return impl->update_status(com_spec, task);
    }

    int dtpAsyncWait(int desc, DtpAsync *task){        
        int error = 0;
        while(1){
            error = dtpAsyncStatus(desc, task);
            if(error == DTP_ST_ERROR) return -1;
            if(error == DTP_ST_DONE) return 0;
        };
    }


}