#ifndef __DTP_H_INCLUDED__
#define __DTP_H_INCLUDED__

#include <stdlib.h>
#include "stdint.h"

typedef void (*DtpCallbackFuncT)(void *data);
typedef union {
    int   sigval_int;
    void *sigval_pointer;
} DtpSignalData;

typedef void (*DtpNotifyFunctionT)(DtpSignalData *data);

typedef struct {
    size_t (*write)(void *user_data, const void *buf, size_t size);
    size_t (*read)(void *user_data, void *buf, size_t size);
    size_t (*write_matrix)(void *user_data, const void *buf, size_t size, int width, int stride);
    size_t (*read_matrix)(void *user_data, const void *buf, size_t size, int width, int stride);
    int (*flush)(void *user_data);
    int (*close)(void *user_data);
} DtpImplementaion;

enum DtpSignal{
    DTP_SIG_NONE,
    DTP_SIG_SIGNAL,
    DTP_SIG_CALLBACK
};

typedef struct {
    int desc;
    volatile void *buf;
    size_t nbytes;
    int width;
    int stride;

    int sig_type;
    DtpSignalData sigval;
    void *user_data;
    DtpNotifyFunctionT sigevent;
} DtpASync;

//all sizes, widths, strides and offsets in bytes

#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenFile(const char *filename, const char *mode);
    int dtpOpenLink(int port);
    int dtpOpenDesc(int desc);
    int dtpOpen(void *user_data, DtpImplementaion *implementation);
    int dtpOpenFileDesc(int fd, const char *mode);

    size_t dtpWrite(int desc, const void *data, size_t size);
    size_t dtpRead(int desc, void *data, size_t size);
	size_t dtpWriteM(int desc, const void *data, size_t size, int width, int stride);
    size_t dtpReadM(int desc, void *data, size_t size, int width, int stride);
	size_t dtpWriteP(int desc, const void *data, size_t size, int offset);
    size_t dtpReadP(int desc, void *data, size_t size, int offset);

    size_t dtpReadA(DtpASync *task);
    size_t dtpWriteA(DtpASync *task);
    int dtpFlush(int desc);
    int dtpClose(int desc);

#ifdef __cplusplus
}
#endif

#endif //__DTP_H_INCLUDED__