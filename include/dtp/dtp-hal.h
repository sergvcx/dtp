#ifndef __DTP_HOST_H_INCLUDED__
#define __DTP_HOST_H_INCLUDED__
#include "stdint.h"

typedef struct HalAccess HalAccess;

#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenPload(HalAccess *access, uintptr_t buffer_addr_in);

#ifdef __cplusplus
}
#endif

#endif //__DTP_HOST_H_INCLUDED__