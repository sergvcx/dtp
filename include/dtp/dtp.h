#ifndef __DTP_H_INCLUDED__
#define __DTP_H_INCLUDED__

#include <stdlib.h>

typedef void (*DtpNotifyFunctionT)(void *event_data);

#define DTP_OPEN_MAX 16

typedef enum{
    DTP_OK,
    DTP_ERROR,
    DTP_AGAIN
} DtpError;

typedef enum {
    DTP_TASK_NONE,
    DTP_TASK_1D,
    DTP_TASK_2D
} DtpAsyncType;

typedef enum {
    DTP_EVENT_NONE,
    DTP_EVENT_CALLBACK
} DtpSigEvent;

typedef struct {
    //int desc;
    volatile void *buf;
    size_t nwords;
    int width;
    int stride;

    DtpAsyncType type;
    DtpSigEvent sigevent;//?
    void *cb_data;
    DtpNotifyFunctionT callback;
} DtpAsync;

typedef enum {
    DTP_ST_IN_PROCESS,
    DTP_ST_DONE,
    DTP_ST_ERROR
} DtpAsyncStatus;


typedef struct {
    int (*send)(void *com_spec, DtpAsync *task);
    int (*recv)(void *com_spec, DtpAsync *task);
    int (*get_status)(void *com_spec, DtpAsync *task);    
    int (*destroy)(void *com_spec);
    int (*listen)(void *com_spec);
    int (*connect)(void *com_spec);
} DtpImplementation;



//all sizes, widths, strides and offsets in bytes

#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenCustom(void* com_spec, DtpImplementation *implementation);
    int dtpClose(int desc);

    int dtpConnect(int desc);
    int dtpListen(int desc);

    int dtpAsyncRecv(int desc, DtpAsync *task);
    int dtpAsyncSend(int desc, DtpAsync *task);
    int dtpAsyncStatus(int desc, DtpAsync *task);
    int dtpAsyncWait(int desc, DtpAsync *task);

    
    int dtpSend(int desc, const void *data, size_t nwords);
    int dtpRecv(int desc, void *data, size_t nwords);
	int dtpSendM(int desc, const void *data, size_t nwords, int width, int stride);
    int dtpRecvM(int desc, void *data, size_t nwords, int width, int stride);


    void *dtpGetComSpec(int desc);

#ifdef __cplusplus
}
#endif

#endif //__DTP_H_INCLUDED__