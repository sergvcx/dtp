#ifndef __DTP_BUFFER_H_INCLUDED__
#define __DTP_BUFFER_H_INCLUDED__
#include "stdint.h"
#include "dtp/dtp.h"

typedef int (*DtpBufferCopyFuncT)(void *user_data, void *buf, int remote_addr, int size32);

#define DTP_BUFFER_COUNT 4

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    /**
     * @brief dtpConnectSharedBuffer
     * 
     * @param index Индекс буфера (0..(DTP_BUFFER_COUNT-1))
     * @return int Дескриптор управления записью-чтением. Возвращается -1 при ошибке
     * @details Используя данный режим, устройства общаются через буфер обмена, максимальный размер которого 
     * равен 32 64-битным словам (или 256 байт). Поскольку данные пишутся напрямую процессором, в этом режиме запись и чтение блокируют
     * выполнение программы до окончания обмена данных, а dtpGetStatus всегда возвращает DTP_IS_DONE
     */
    int 
    DEPRECATED
    dtpConnectSharedBuffer(int index);

    // int dtpOpenUserSharedBuffer(int index, void *buf, int nwords);

    // int dtpOpenSharedBuffer(int index);

    /**
     * @brief dtpOpenRemoteSharedBuffer
     * 
     * @param index Индекс буфера (0..(DTP_BUFFER_COUNT-1))
     * @param copy_data Пользовательская структура данных для копирования (например PL_Access)
     * @param readFunc Указатель на функцию чтения
     * @param writeFunc Указатель на функцию записи
     * @return int Дескриптор управления записью-чтением. Возвращается -1 при ошибке
     * @details Расширенная функция общения через буфер обмена. Так же как и в dtpConnectSharedBuffer, чтение и записью являются
     * блокирующими операциями, а dtpGetStatus всегда возвращает DTP_IS_DONE
     */
    int 
    DEPRECATED
    dtpOpenRemoteSharedBuffer(int index, void *copy_data, DtpBufferCopyFuncT readFunc, DtpBufferCopyFuncT writeFunc);

    //int dtpGetImplementation()

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_BUFFER_H_INCLUDED__