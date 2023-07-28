#ifndef __DTP_CORE_H_INCLUDED__
#define __DTP_CORE_H_INCLUDED__
#include "dtp/ringbuffer.h"

typedef struct{
    DtpRingBuffer32 *ringbuffer;
} DtpShmemData;



typedef struct{
    int fd;
    int is_enabled;
    void *user_data;
    int type;
    DtpImplementation implementaion;
}  DtpObject;

#ifdef __cplusplus 
extern "C" {
#endif

    DtpObject *getDtpObject(int desc);

    size_t usbRecv(void *user_data, void *buf, size_t size);
    size_t usbSend(void *user_data, void *buf, size_t size);
    size_t shmemRead(void *user_data, void *buf, size_t size);
    size_t shmemWrite(void *user_data, const void *buf, size_t size);
    int shmemClose(void *user_data);

#ifdef __cplusplus
}
#endif

#endif //__DTP_CORE_H_INCLUDED__
