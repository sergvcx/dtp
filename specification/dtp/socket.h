#ifndef __DTP_SOCKET_H_INCLUDED__
#define __DTP_SOCKET_H_INCLUDED__
#include "dtp/dtp.h"

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus    

    /**
     * @brief Функция привязки открытого сокета к дескриптору потока ввода-вывода
     * 
     * @param dtp 
     * @param socket 
     * @return int 
     */
    int dtp_socket(int dtp, int socket);

    /**
     * @brief Попытка соединения к открытому сокету
     * 
     * @param dtp 
     * @param ipAddr 
     * @param port 
     * @return int 
     */
    int dtp_socket_connect(int dtp, const char *ipAddr, int port);

    /**
     * @brief Открытие сокета
     * 
     * @param dtp 
     * @param port 
     * @return int 
     */
    int dtp_socket_listen(int dtp, int port);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_SOCKET_H_INCLUDED__