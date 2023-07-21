#ifndef __DEBR_H_INCLUDED__
#define __DEBR_H_INCLUDED__

#include <stdlib.h>
#include "stdint.h"

typedef void (*DtpCallbackFuncT)(void *data);

typedef size_t(*DtpWriteFuncT)(void *user_data, const void *buf, size_t size);
typedef size_t(*DtpReadFuncT)(void *user_data, void *buf, size_t size);
typedef int (*DtpFlushFuncT)(void *user_data);
typedef int (*DtpCloseFuncT)(void *user_data);

typedef struct {
    size_t (*write)(void *user_data, const void *buf, size_t size);
    size_t (*read)(void *user_data, void *buf, size_t size);
    int (*flush)(void *user_data);
    int (*close)(void *user_data);
} DtpImplementaion;


#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenFile(const char *filename, const char *mode);
    int dtpOpenDesc(int desc);
    int dtpOpenCustom(void *user_data, DtpReadFuncT readFunc, DtpWriteFuncT writeFunc);
    int dtpOpen(void *user_data, DtpImplementaion *implementation);
    int dtpOpenFileDesc(int fd, const char *mode);

    size_t dtpWrite(int desc, const void *data, size_t size);
    size_t dtpRead(int desc, void *data, size_t size);
	size_t dtpWriteM(int desc, const void *data, size_t size, int width, int stride);
    size_t dtpReadM(int desc, void *data, size_t size, int width, int stride);
	size_t dtpWriteP(int desc, const void *data, size_t size, int offset);
    size_t dtpReadP(int desc, void *data, size_t size, int offset);
	// int dtpSetWriteCallback(int desc, void *user_data); ?
	// int dtpSetReadCallback(int desc, void *user_data);  ?
    int dtpFlush(int desc);
    int dtpClose(int desc);

#ifdef __cplusplus
}
#endif

#endif //__DEBR_H_INCLUDED__