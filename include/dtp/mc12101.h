#ifndef __DTP_MC12101_H_INCLUDED__
#define __DTP_MC12101_H_INCLUDED__
#include "stdint.h"
#include "dtp/buffer.h"

#define DTP_RING_BUFFER_SIZE_32 (4 * 128 * 128)

typedef void* (*DtpMemCopyFuncT)(const void *src, void *dst, unsigned int size32);

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief dtpOpenPloadTarget
     * 
     * @param index 
     * @return int Индекс буфера (0..(DTP_BUFFER_COUNT-1))
     * @details Режим пересылки команд чтения-записи через буфер обмена. В данном режиме чтение и запись целиком ложатся на хост часть.
     * - dtpGetStatus возвращает DTP_ST_DONE при завершении транзакции и DTP_ST_IN_PROCESS при незавершенной транзакции
     * - dtpSend возвращает DTP_OK при успешной загрузке команды в буфер и DTP_AGAIN в случае, когда нет места в буфере
     * - dtpRecv возвращают DTP_OK при успешной загрузке команды в буфер и DTP_AGAIN в случае, когда нет места в буфере
     */
    int dtpOpenPloadTarget(int index);

    //int dtpOpenPloadFile(int index, void *copy_data, DtpBufferCopyFuncT readFunc, DtpBufferCopyFuncT writeFunc);

    int dtpOpenRingbuffer(void *hal_ring_buffer, DtpMemCopyFuncT push_func, DtpMemCopyFuncT pop_func);

    int dtpOpenRingbufferDefault(void *hal_ring_buffer);

#ifdef __cplusplus
}
#endif

#endif //__DTP_MC12101_H_INCLUDED__