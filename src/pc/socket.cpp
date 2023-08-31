#include "dtp/dtp.h"
#include "stdio.h"
#include "string.h"
#ifdef __linux__ 
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#endif

typedef struct{
    int desc;
    int port;
    int listener;
} SocketData;

static int socketRecv(void *com_spec, DtpAsync *cmd){
    SocketData *data = (SocketData *)com_spec;
    recv(data->desc, (char*)cmd->buf, cmd->nwords * sizeof(int), 0);
    return 0;
}

static int socketSend(void *com_spec, DtpAsync *cmd){
    SocketData *data = (SocketData *)com_spec;
    send(data->desc, (const char*)cmd->buf, cmd->nwords * sizeof(int), 0);
    return 0;
}

static int socketStatus(void *com_spec, DtpAsync *cmd){
    return DTP_ST_DONE;
}

static int socketDestroy(void *com_spec){
    SocketData *data = (SocketData *)com_spec;
#ifdef _WIN32
    closesocket(data->desc);
#else
    close(data->desc);
#endif

#ifdef _WIN32
    if(data->listener) closesocket(data->listener);
#else
    if(data->listener) close(data->listener);
#endif
    free((int*)com_spec);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

static int socketListen(void *com_spec){
    SocketData *sock = (SocketData *)com_spec;

#ifdef _WIN32 
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *result = NULL;
    
    char buffer[10];
    sprintf(buffer, "%d", sock->port);
    int iResult = getaddrinfo(NULL, buffer, &hints, &result);

    sock->listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    iResult = bind(sock->listener, result->ai_addr, (int)result->ai_addrlen);

    
#else
    struct sockaddr_in addr;
    sock->listener = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(sock->port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock->listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        return -1;
    }
#endif    

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
#ifdef _WIN32
    struct addrinfo *result = NULL;

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    char buffer[10];
    sprintf(buffer, "%d", sock->port);
    int iResult = getaddrinfo(NULL, buffer, &hints, &result);

    sock->desc = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    return connect( sock->desc, result->ai_addr, (int)result->ai_addrlen);
#else
    

    struct sockaddr_in serv_addr; 
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(sock->port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sock->desc = socket(AF_INET, SOCK_STREAM, 0); 
    return connect(sock->desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
#endif
    
}

extern "C"{

    int dtpConnectSocket(int desc, const char *ipAddr, int port){
        SocketData *sock = (SocketData*)malloc(sizeof(SocketData));
        if(sock == 0)return -1;
        sock->port = port;
    #ifdef _WIN32
        WSADATA wsaData = {0};
        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif



        DtpImplementation impl;
        impl.recv = socketRecv;
        impl.send = socketSend;
        impl.destroy = socketDestroy;
        impl.update_status = socketStatus;
        socketConnect(sock);
        return dtpBind(desc, sock, &impl);        
    }

    int dtpListenSocket(int desc, int port){
        SocketData *sock = (SocketData*)malloc(sizeof(SocketData));
        if(sock == 0)return -1;
        sock->port = port;
    #ifdef _WIN32
        WSADATA wsaData = {0};
        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif



        DtpImplementation impl;
        impl.recv = socketRecv;
        impl.send = socketSend;
        impl.destroy = socketDestroy;
        impl.update_status = socketStatus;        
        socketListen(sock);
        return dtpBind(desc, sock, &impl);
    }

    int dtpOpenSocket(const char *ipAddr, int port){
        SocketData *sock = (SocketData*)malloc(sizeof(SocketData));
        if(sock == 0)return -1;
        sock->port = port;
    #ifdef _WIN32
        WSADATA wsaData = {0};
        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif



        DtpImplementation impl;
        impl.recv = socketRecv;
        impl.send = socketSend;
        impl.destroy = socketDestroy;
        impl.update_status = socketStatus;
        return dtpOpenCustom(sock, &impl);
    }
}