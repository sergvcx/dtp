#ifndef __DTP_SOCKET_H_INCLUDED__
#define __DTP_SOCKET_H_INCLUDED__
#include "dtp/dtp.h"

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

    int 
    DEPRECATED
    dtpOpenSocket(const char *ipAddr, int port);

    int dtpConnectSocket(int desc, const char *ipAddr, int port);
    int dtpListenSocket(int desc, int port);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_SOCKET_H_INCLUDED__