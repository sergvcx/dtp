#include "dtp/ringbuffer.h"
#include "dtp/dtp.h"
#include "dtp-core.h"
#include "dtp-nm6407-core.h"

#define DTP_DEFAULT_BUFFER_SIZE 0x200
#define DTP_DEFAULT_BUFFER_COUNT 4



static DtpRingBuffer32 __attribute__ ((section (".data.dtp.buffer.ringbuffers"))) d_ringbuffers[DTP_DEFAULT_BUFFER_COUNT];
static int __attribute__ ((section (".data.dtp.buffer0"))) d_buffer0[DTP_DEFAULT_BUFFER_SIZE];
static int __attribute__ ((section (".data.dtp.buffer1"))) d_buffer1[DTP_DEFAULT_BUFFER_SIZE];
static int __attribute__ ((section (".data.dtp.buffer2"))) d_buffer2[DTP_DEFAULT_BUFFER_SIZE];
static int __attribute__ ((section (".data.dtp.buffer3"))) d_buffer3[DTP_DEFAULT_BUFFER_SIZE];


int dtpNm6407OpenDefaultBuffer(int index){
    if(index < 0 || index >= DTP_DEFAULT_BUFFER_COUNT) return -1;

    if(d_ringbuffers[index].is_inited == 0){
        switch (index)
        {
        case 0:
            d_ringbuffers[index].data = d_buffer0;
            break;
        case 1:
            d_ringbuffers[index].data = d_buffer1;
            break;
        case 2:
            d_ringbuffers[index].data = d_buffer2;
            break;
        case 3:
            d_ringbuffers[index].data = d_buffer3;
            break;    
        default:
            break;
        }
        d_ringbuffers[index].capacity = DTP_DEFAULT_BUFFER_SIZE;
        d_ringbuffers[index].head = 0;
        d_ringbuffers[index].tail = 0;
        d_ringbuffers[index].read_semaphore = 0;
        d_ringbuffers[index].write_semaphore = DTP_DEFAULT_BUFFER_SIZE;
        d_ringbuffers[index].is_inited = 1;
    }

    return dtpOpenBuffer(&d_ringbuffers[index]);
}