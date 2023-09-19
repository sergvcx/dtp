#ifndef __DTP_NM6407_H_INCLUDED__
#define __DTP_NM6407_H_INCLUDED__
#include "dtp/dtp.h"

#define DTP_NM6407_DMA_PACKET           0x00000001
#define DTP_NM6407_DMA_STATUS_ONLY      0x00000002
#define DTP_NM6407_DMA_WITHOUT_CHECK    0x00000004

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus
    int dtp_nm6407_usb(int dtp);

    /**
     * @brief Привязывает внешний байтовый коммуникационный порт к дескриптору
     * 
     * @param dtp Дескриптор потока ввода-вывода
     * @param port Порт внешнего байтового коммуникационного порта
     * @param flags Флаги режимов работы коммуникационного порта
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     */
    int dtp_nm6407_link(int dtp, int port, int flags);

    /**
     * @brief Привязывает ПДП MDMAC к дескриптору
     * 
     * @param dtp Дескриптор потока ввода-вывода
     * @param flags Флаги режимов работы ПДП
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     */
    int dtp_nm6407_dma(int dtp, int flags);

    /**
     * @brief Указывает адрес мьютекса для многопоточных програм
     * 
     * @param mutex 
     * @return int Выровненное число в общей памяти
     * @details В процессоре nm6407 есть два ядра, у каждого из которых есть доступ к ПДП. Функция позволяет задать
     * переменную синхронизации, которая будет выполнять роль мьютекса. Необходимо выполнить до функции dtp_nm6407_dma
     */
    int dtp_nm6407_set_dma_mutex(int *shared_mutex);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_NM6407_H_INCLUDED__