#ifndef __DEBR_H_INCLUDED__
#define __DEBR_H_INCLUDED__

#include <stdlib.h>
#include "stdint.h"

typedef void (*DebrCallbackFuncT)(void *data);

typedef size_t(*DebrWriteFuncT)(void *user_data, const void *buf, size_t size);
typedef size_t(*DebrReadFuncT)(void *user_data, void *buf, size_t size);
typedef int (*DebrFlushFuncT)(void *user_data);
typedef int (*DebrCloseFuncT)(void *user_data);

typedef struct {
    DebrWriteFuncT write;
    DebrReadFuncT read;
    DebrFlushFuncT flush;
    DebrCloseFuncT close;
} DebrImplementaion;


#ifdef __cplusplus
extern "C" {
#endif

    int debrOpenLink(int port);
    int debrOpenDma();
    int debrOpenFile(const char *filename, const char *mode);
    int debrOpenDesc(int desc);
    int debrOpenCustom(void *user_data, DebrReadFuncT readFunc, DebrWriteFuncT writeFunc);
    int debrOpen(void *user_data, DebrImplementaion implementation);
    int debrOpenFileDesc(int fd, const char *mode);

    int debrWrite(int desc, const void *data, size_t size);
    int debrRead(int desc, void *data, size_t size);
    int debrWriteM(int desc, const void *data, size_t size, int width, int stride);
    int debrReadM(int desc, void *data, size_t size, int width, int stride);
    int debrWriteP(int desc, const void *data, size_t size, int offset);
    int debrReadP(int desc, void *data, size_t size, int offset);
    int debrSetWriteCallback(int desc, void *user_data);
    int debrSetReadCallback(int desc, void *user_data);
    int debrFlush(int desc);
    int debrClose(int desc);

#ifdef __cplusplus
}
#endif

#endif //__DEBR_H_INCLUDED__