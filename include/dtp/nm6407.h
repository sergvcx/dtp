#ifndef __DTP_NM6407_H_INCLUDED__
#define __DTP_NM6407_H_INCLUDED__
#include "dtp/dtp.h"

#define DTP_BUFFER_COUNT 4

#define DTP_NM6407_DMA_PACKET           0x00000001
#define DTP_NM6407_DMA_STATUS_ONLY      0x00000002
#define DTP_NM6407_DMA_WITHOUT_CHECK    0x00000004

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

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
    int dtpNm6407InitBuffer(int desc, void *data_in, int capacity_in, void *data_out, int capacity_out, int channel);

    /**
     * @brief Инциализирует структуруы кольцевых буферов внутренними буферами и привязывает их к дескриптору . Размер этих буферов равен 64
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param channel Индекс, к которому привязываются кольцевые буферы (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpNm6407InitDefaultBuffer(int desc, int channel);

    /**
     * @brief Подключение к буферу с заданным индексом
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param channel Номер канала (0.. (DTP_BUFFER_COUNT - 1))
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpNm6407ConnectBuffer(int desc, int channel);

    /**
     * @brief Привязывает внешний байтовый коммуникационный порт к дескриптору
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param port Порт внешнего байтового коммуникационного порта
     * @param flags Флаги режимов работы коммуникационного порта
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpNm6407Link(int desc, int port, int flags);

    /**
     * @brief Привязывает ПДП MDMAC к дескриптору
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param flags Флаги режимов работы ПДП
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибке
     */
    int dtpNm6407Dma(int desc, int flags);

    /**
     * @brief Указывает адрес мьютекса для многопоточных програм
     * 
     * @param mutex 
     * @return int Выровненное число в общей памяти
     * @details В процессоре nm6407 есть два ядра, у каждого из которых есть доступ к ПДП. Функция позволяет задать
     * переменную синхронизации, которая будет выполнять роль мьютекса. Необходимо выполнить до функции dtpNm6407Dma
     */
    int dtpNm6407SetDmaMutex(int *shared_mutex);


    int 
    DEPRECATED
    dtpOpenNm6407Dma(int mask);       
    

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_NM6407_H_INCLUDED__