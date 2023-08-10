#ifndef __DTP_MC12101_H_INCLUDED__
#define __DTP_MC12101_H_INCLUDED__
#include "stdint.h"

#define DTP_RING_BUFFER_SIZE_32 (4 * 128 * 128)

typedef void* (*DtpMemCopyFuncT)(const void *src, void *dst, unsigned int size32);

#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenPloadFile(const char *filename);

    int dtpOpenRingbuffer(void *hal_ring_buffer, DtpMemCopyFuncT push_func, DtpMemCopyFuncT pop_func);

#ifdef __cplusplus
}
#endif

#endif //__DTP_MC12101_H_INCLUDED__