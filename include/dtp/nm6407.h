#ifndef __DTP_NM6407_H_INCLUDED__
#define __DTP_NM6407_H_INCLUDED__
#include "dtp/ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    int dtpOpenFile(const char *filename, const char *mode);
    int dtpOpenLink(int port);
    int dtpOpenDma();
    int dtpNm6407OpenDefaultBuffer(int index);
    
    int dtpOpenDesc(int desc);
    //int dtpOpenFileDesc(int fd, const char *mode);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__DTP_NM6407_H_INCLUDED__