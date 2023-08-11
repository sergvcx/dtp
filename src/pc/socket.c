#include "dtp/dtp.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

typedef struct{
    int desc;
    struct sockaddr_in addr;
    int port;
    int listener;
} SocketData;

static int socketRecv(void *com_spec, DtpAsync *cmd){
    SocketData *data = (SocketData *)com_spec;
    recv(data->desc, cmd->buf, cmd->nwords * sizeof(int), 0);
    return 0;
}

static int socketSend(void *com_spec, DtpAsync *cmd){
    SocketData *data = (SocketData *)com_spec;
    send(data->desc, cmd->buf, cmd->nwords * sizeof(int), 0);
    return 0;
}

static int socketStatus(void *com_spec, DtpAsync *cmd){
    return DTP_ST_DONE;
}

static int socketDestroy(void *com_spec){
    SocketData *data = (SocketData *)com_spec;
    close(data->desc);

    if(data->listener) close(data->listener);
    free((int*)com_spec);
    return 0;
}

static int socketListen(void *com_spec){
    SocketData *sock = (SocketData *)com_spec;

    sock->listener = socket(AF_INET, SOCK_STREAM, 0);
    sock->addr.sin_family = AF_INET;
    sock->addr.sin_port = htons(sock->port);
    sock->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock->listener, (struct sockaddr *)&sock->addr, sizeof(sock->addr)) < 0)
    {
        return -1;
    }
    

    if(listen(sock->listener, 1) < 0){
        return -1;
    }

    sock->desc = accept(sock->listener, NULL, NULL);

    if(sock->desc < 0){
        return -1;
    } else {
        return 0;
    }
    
}

static int socketConnect(void *com_spec){
    SocketData *sock = (SocketData *)com_spec;
    sock->listener = 0;

    memset(&sock->addr, '0', sizeof(sock->addr)); 

    sock->addr.sin_family = AF_INET;
    sock->addr.sin_port = htons(sock->port);
    sock->addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sock->desc = socket(AF_INET, SOCK_STREAM, 0);    
    return connect(sock->desc, &sock->addr, sizeof(sock->addr));
}


int dtpOpenSocket(const char *ipAddr, int port){
    SocketData *sock = (SocketData*)malloc(sizeof(SocketData));
    if(sock == 0)return -1;
    sock->port = port;
    

    DtpImplementation impl;
    impl.recv = socketRecv;
    impl.send = socketSend;
    impl.destroy = socketDestroy;
    impl.get_status = socketStatus;
    impl.connect = socketConnect;
    impl.listen = socketListen;
    return dtpOpenCustom(sock, &impl);
}