#ifndef __DTP_MC12101_HOST_H_INCLUDED__
#define __DTP_MC12101_HOST_H_INCLUDED__
#include "stdint.h"

typedef struct PL_Access PL_Access;

#define DTP_RING_BUFFER_SIZE_32 (4 * 128 * 128)

// typedef int (*DtpRingBufferReadFuncT)(void *copy_data, int *dst, uintptr_t src, int size32);
// typedef int (*DtpRingBufferWriteFuncT)(void *copy_data, const int *src, uintptr_t dst, int size32);

#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenPloadFileHost(PL_Access *access, const char *filename);

    int dtpOpenPloadRingbuffer(PL_Access *access, uintptr_t hal_ring_buffer_remote_addr);

    int dtpOpenMc12101Ringbuffer(int boardIndex, int coreIndex, uintptr_t hal_ring_buffer_remote_addr);

#ifdef __cplusplus
}
#endif
#endif //__DTP_MC12101_HOST_H_INCLUDED__