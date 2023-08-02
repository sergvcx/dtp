#ifndef __DTP_NM6407_CORE_H_INCLUDED__
#define __DTP_NM6407_CORE_H_INCLUDED__
#include "stddef.h"


typedef struct DtpRingBuffer32{
    int *data;
    volatile size_t capacity;

    volatile size_t read_semaphore;
    volatile size_t write_semaphore;
    volatile size_t head;
    volatile size_t tail;
    volatile int is_inited;
} DtpRingBuffer32;

#endif //__DTP_NM6407_CORE_H_INCLUDED__
