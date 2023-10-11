#ifndef __DTP_BUFFER_H_INCLUDED__
#define __DTP_BUFFER_H_INCLUDED__
#include "stdint.h"
#include "dtp/dtp.h"
#include "dtp/nm6407.h"

typedef int (*DtpBufferCopyFuncT)(void *user_data, void *buf, int remote_addr, int size32);



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

    /**
     * @brief Инициализирует кольцевые буфера с заданными буферами и привязывает их к дескриптору
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data_in Буфер для входных данных
     * @param capacity_in Размер входного буфера
     * @param data_out Буфер для выходных данных
     * @param capacity_out Размер буфера выходных данных
     * @param channel Номер канала к которому привыязывают кольцевые буфера (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpBufferInit(int desc, void *data_in, int capacity_in, void *data_out, int capacity_out, int channel);

    /**
     * @brief Инциализирует структуруы кольцевых буферов внутренними буферами и привязывает их к дескриптору . Размер этих буферов равен 64
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param channel Индекс, к которому привязываются кольцевые буферы (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpBufferInitDefault(int desc, int channel);

    int dtpBufferSetTableAddr(unsigned int addr);

    /**
     * @brief Подключение к буферу с заданным индексом
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param channel Номер канала (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpBufferConnect(int desc, int channel);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_BUFFER_H_INCLUDED__