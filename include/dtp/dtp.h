#ifndef __DTP_H_INCLUDED__
#define __DTP_H_INCLUDED__

#include <stdlib.h>
#include "stdint.h"

typedef struct DtpRingBuffer32 DtpRingBuffer32;

typedef void (*DtpCallbackFuncT)(void *data);
typedef union {
    int   sigval_int;
    void *sigval_pointer;
} DtpSignalData;

typedef void (*DtpNotifyFunctionT)(void *data);

#define DTP_OPEN_MAX 16

//typedef void (*DtpWriteFunctionT)(void *user_data, const void *buf, size_t size);
//typedef void (*DtpReadFunctionT)(void *user_data, const void *buf, size_t size);

typedef struct {
    void *user_data;
    size_t (*send)(void *user_data, const void *buf, size_t size);
    size_t (*recv)(void *user_data, void *buf, size_t size);
    size_t (*send_matrix)(void *user_data, const void *buf, size_t size, int width, int stride);
    size_t (*recv_matrix)(void *user_data, void *buf, size_t size, int width, int stride);
    int (*wait_recv)(void *user_data);
    int (*wait_send)(void *user_data);
    int (*flush)(void *user_data);
    int (*destroy)(void *user_data);
} DtpImplementation;

enum DtpSignal{
    DTP_SIG_NONE,
    DTP_SIG_CALLBACK
};

typedef enum {
    DTP_TASK_NONE,
    DTP_TASK_1D,
    DTP_TASK_2D
} DtpAsyncType;

typedef struct {
    int desc;
    volatile void *buf;
    size_t nbytes;
    int width;
    int stride;

    DtpAsyncType type;
    void *event_data;
    DtpNotifyFunctionT callback;
} DtpASync;

//all sizes, widths, strides and offsets in bytes

#ifdef __cplusplus
extern "C" {
#endif

    //int dtpOpenBuffer(void *buffer, size_t size);

    int dtpOpenBuffer(DtpRingBuffer32 *ringbuffer);

    int dtpOpenCustom(DtpImplementation *implementation);


    size_t dtpSend(int desc, const void *data, size_t size);
    size_t dtpRecv(int desc, void *data, size_t size);
	size_t dtpSendM(int desc, const void *data, size_t size, int width, int stride);
    size_t dtpRecvM(int desc, void *data, size_t size, int width, int stride);

    void *dtpGetUserData(int desc);

    int dtpGetProp(int desc, int cmd);

    size_t dtpAsyncRecv(DtpASync *task);
    size_t dtpAsyncSend(DtpASync *task);
    int dtpWaitSend(int desc);
    int dtpWaitRecv(int desc);
    int dtpClose(int desc);



    // deprecated
    size_t dtpWrite(int desc, const void *data, size_t size);
    size_t dtpRead(int desc, void *data, size_t size);
	size_t dtpWriteM(int desc, const void *data, size_t size, int width, int stride);
    size_t dtpReadM(int desc, void *data, size_t size, int width, int stride);
    int dtpFlush(int desc);
    

#ifdef __cplusplus
}
#endif

#endif //__DTP_H_INCLUDED__