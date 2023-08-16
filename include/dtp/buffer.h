#ifndef __DTP_BUFFER_H_INCLUDED__
#define __DTP_BUFFER_H_INCLUDED__
#include "stdint.h"

typedef void (*DtpBufferCopyFuncT)(void *user_data, void *src, int remote_addr, int size32);

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
    int dtpOpenSharedBuffer(int index);

    int dtpOpenRemoteSharedBuffer(int index, DtpBufferCopyFuncT readFunc, DtpBufferCopyFuncT writeFunc);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_BUFFER_H_INCLUDED__