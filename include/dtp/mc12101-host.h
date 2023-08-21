#ifndef __DTP_MC12101_HOST_H_INCLUDED__
#define __DTP_MC12101_HOST_H_INCLUDED__
#include "stdint.h"
#include "dtp/buffer.h"

#include "mc12101load.h"

typedef struct PL_Access PL_Access;

#define DTP_RING_BUFFER_SIZE_32 (4 * 128 * 128)

// typedef int (*DtpRingBufferReadFuncT)(void *copy_data, int *dst, uintptr_t src, int size32);


#ifdef __cplusplus
extern "C" {
#endif    

    /**
     * @brief dtpOpenPloadHost
     * 
     * @param index Индекс буфера (0..(DTP_BUFFER_COUNT-1))
     * @param copy_data Указатель на PL_Access
     * @param readFunc Указатель на функцию чтения PL_ReadMemBlock
     * @param writeFunc Указатель на функцию чтения PL_WriteMemBlock
     * @return int Дескриптор управления записью-чтением. Возвращается -1 при ошибке
     * @details Режим пересылки команд чтения-записи через буфер обмена. В данном режиме чтение и запись целиком ложатся на хост часть.
     * - dtpGetStatus всегда возвращается DTP_ST_DONE, так как хост сразу читает или пишет данных
     * - dtpSend возвращает DTP_OK при успешной записи данных на target
     * - dtpRecv возвращают DTP_OK при успешном чтении данных и DTP_AGAIN при пустом буфере команд
     */
    int dtpOpenPloadHost(int index, PL_Access *access, DtpBufferCopyFuncT readFunc, DtpBufferCopyFuncT writeFunc);

    int dtpOpenPloadFileHost(PL_Access *access, const char *filename);

    int dtpOpenPloadRingbuffer(PL_Access *access, uintptr_t hal_ring_buffer_remote_addr);

    int dtpOpenMc12101Ringbuffer(int boardIndex, int coreIndex, uintptr_t hal_ring_buffer_remote_addr);

#ifdef __cplusplus
}
#endif
#endif //__DTP_MC12101_HOST_H_INCLUDED__