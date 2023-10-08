#ifndef __DTP_BUFFER_H_INCLUDED__
#define __DTP_BUFFER_H_INCLUDED__
#include "stdint.h"
#include "dtp/dtp.h"


struct dtp_buffer;

typedef struct {
    dtp_buffer *buffer0;
    dtp_buffer *buffer1;
} dtp_buffer_registry;

#define DTP_BUFFER_COUNT 4
#define DTP_SHM_SIZEOF


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
/**
     * @brief Устанавливает указатель на реестр буферов общей памяти
     * 
     * @param table_addr адрес в общей памяти, должно быть зарезезрви
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     * @details Для того, чтобы несколько ядер могли общаться через общую память, они должны договориться о том, по каком адресу находится
     * таблица буферов обмена.
     */
    int dtp_shm_set_registry_addr(struct dtp_buffer_registry *registry_addr, int count);

    /**
     * @brief Инициализирует кольцевые буфера с заданными буферами и привязывает их к дескриптору
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data_in Буфер для входных данных
     * @param capacity_in Размер входного буфера
     * @param data_out Буфер для выходных данных
     * @param capacity_out Размер буфера выходных данных
     * @param channel Номер канала к которому привыязывают кольцевые буфера (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     */
    int dtp_shm_init(int desc, void *data_in, int capacity_in, void *data_out, int capacity_out, int channel);    

    /**
     * @brief Подключение к буферу с заданным индексом
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param channel Номер канала (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     */
    int dtp_shm_connect(int desc, int channel);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_BUFFER_H_INCLUDED__