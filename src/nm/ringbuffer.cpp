#include "stddef.h"
#include <stdint.h>
#include "malloc.h"
#include "stdio.h"
#include "dtp/mc12101.h"
#include "hal/ringbuffert.h"
#include "dtp/dtp.h"
#include "string.h"

#include "ringbuffer.h"

#include "mc12101load_nm.h"

#define ADDR_TABLE_ADDR (0xA8000 - 16)


typedef struct DtpRingBuffer32{
    int *data;
    volatile size_t capacity;

    volatile size_t read_semaphore;
    volatile size_t write_semaphore;
    volatile size_t head;
    volatile size_t tail;
    volatile int is_inited;
} DtpRingBuffer32;


void dtpCopyRisc(int *src, int* dst, int size){
    for(int i = 0; i < size; i++){
        dst[i] = src[i];
    }
}

__attribute__ ((section (".data.dtp.buffer0"))) int dtp_buffer0_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer0"))) int dtp_buffer0_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer1"))) int dtp_buffer1_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer1"))) int dtp_buffer1_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer2"))) int dtp_buffer2_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer2"))) int dtp_buffer2_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer3"))) int dtp_buffer3_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer3"))) int dtp_buffer3_data_out[DTP_BUFFER_SIZE];

typedef struct {
    DtpRingBuffer32 *rb_in;
    DtpRingBuffer32 *rb_out;
    int index;
} RingBufferData;

int dtpRingBufferImplSend(void *com_spec, DtpAsync *cmd){
    RingBufferData *data = (RingBufferData *)com_spec;
    int *src = (int *)cmd->buf;
    int size = cmd->nwords;

    int capacity = dtpRingBufferGetCapacity(data->rb_out);
    
    if(cmd->type == DTP_TASK_1D){
        while(size > 0){
            int head = dtpRingBufferGetHead(data->rb_out);
            int tail = dtpRingBufferGetTail(data->rb_out);
            int available = tail + capacity - head;

            int push_size = (available > size) ? size : available;
            dtpRingBufferPush(data->rb_out, src, push_size);
            src += push_size;
            size -= push_size;
        }
        if(cmd->sigevent == DTP_EVENT_CALLBACK){
            if(cmd->callback) cmd->callback(cmd->cb_data);
        }
    } else {
        return DTP_ERROR;
    }    
    return 0;
}

int dtpRingBufferImplRecv(void *com_spec, DtpAsync *cmd){
    RingBufferData *data = (RingBufferData *)com_spec;
    int *dst = (int *)cmd->buf;
    int size = cmd->nwords;

    int capacity = dtpRingBufferGetCapacity(data->rb_out);
    
    if(cmd->type == DTP_TASK_1D){
        while(size > 0){
            int head = dtpRingBufferGetHead(data->rb_out);
            int tail = dtpRingBufferGetTail(data->rb_out);
            int available = head - tail;

            int pop_size = (available > size) ? size : available;
            dtpRingBufferPop(data->rb_out, dst, pop_size);
            dst += pop_size;
            size -= pop_size;
        }
        if(cmd->sigevent == DTP_EVENT_CALLBACK){
            if(cmd->callback) cmd->callback(cmd->cb_data);
        }
    } else {
        return DTP_ERROR;
    }    
    return 0;
}

int dtpRingBufferImplListen(void *com_spec){
    RingBufferData *data = (RingBufferData *)com_spec;
    int *addr_table = (int*)(0xA8000 - 16);
    int addr = (int) data;
    if(addr < 0x80000){
        int offset = 0x40000 + ncl_getProcessorNo() << 18;
    }
    addr_table[data->index] = addr;

    switch (data->index)
    {
    case 0:
        data->rb_in =  dtpRingBufferAlloc(dtp_buffer0_data_in, DTP_BUFFER_SIZE);        
        data->rb_out = dtpRingBufferAlloc(dtp_buffer0_data_out, DTP_BUFFER_SIZE);
        break;
    case 1:
        data->rb_in =  dtpRingBufferAlloc(dtp_buffer1_data_in, DTP_BUFFER_SIZE);
        data->rb_out = dtpRingBufferAlloc(dtp_buffer1_data_out, DTP_BUFFER_SIZE);
        break;
    case 2:
        data->rb_in =  dtpRingBufferAlloc(dtp_buffer2_data_in, DTP_BUFFER_SIZE);
        data->rb_out = dtpRingBufferAlloc(dtp_buffer2_data_out, DTP_BUFFER_SIZE);
        break;
    case 3:
        data->rb_in =  dtpRingBufferAlloc(dtp_buffer3_data_in, DTP_BUFFER_SIZE);
        data->rb_out = dtpRingBufferAlloc(dtp_buffer3_data_out, DTP_BUFFER_SIZE);
        break;    
    default:
        break;
    }

    if(data->rb_in == 0 || data->rb_out == 0){
        if(data->rb_in) free(data->rb_in);
        if(data->rb_out) free(data->rb_out);
        return -1;
    }
    

    int handshake = 0;
    dtpRingBufferPop(data->rb_in, &handshake, 1);
    if(handshake != 0x12300123) return -1;
    handshake++;
    dtpRingBufferPush(data->rb_in, &handshake, 1);

    return 0;
}

int dtpRingBufferImplConnect(void *com_spec){
    RingBufferData *data = (RingBufferData *)com_spec;
    int *addr_table = (int*)ADDR_TABLE_ADDR;

    int handshake = 0x12300123;
    dtpRingBufferPush(data->rb_in, &handshake, 1);
    dtpRingBufferPop(data->rb_in, &handshake, 1);
    if(handshake != 0x12300124) return -1;

    return 0;
}

int dtpRingBufferImplGetStatus(void *com_spec, DtpAsync *cmd){
    return DTP_ST_DONE;
}

int dtpRingBufferImplDestroy(void *com_spec){
    RingBufferData *data = (RingBufferData *)com_spec;
    free(data->rb_in);
    free(data->rb_out);
    free(data);
    return 0;
}

int dtpOpenNmBuffer(int index){
    uintptr_t addr = ADDR_TABLE_ADDR;
    addr += index * 2;
    int *addr_table = (int *)addr;
    return -1;    
}


int dtpRingBufferGetSizeOfElem(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


DtpRingBuffer32 *dtpRingBufferAlloc(void *data, int capacity){
    DtpRingBuffer32 *rb = (DtpRingBuffer32 *)malloc(sizeof(DtpRingBuffer32));
    rb->data = (int*)data;
    rb->capacity = capacity;
    rb->read_semaphore = 0;
    rb->write_semaphore = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->is_inited = 1;
    return rb;
}


void dtpRingBufferFree(DtpRingBuffer32 *rb){
    free(rb);
}

int dtpRingBufferGetTail(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->tail;
}

int dtpRingBufferGetHead(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head;
}
int *dtpRingBufferGetPtr(DtpRingBuffer32 *ring_buffer, int index){
    return ring_buffer->data + index % ring_buffer->capacity;
}

int dtpRingBufferGetCapacity(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->capacity;
}


void dtpRingBufferConsume(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->tail += count;
}

void dtpRingBufferProduce(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->head += count;
}

void dtpRingBufferPush(DtpRingBuffer32 *ring_buffer, const void *data, int count){
    dtpRingBufferCapturedWrite(ring_buffer, count);
    int head = dtpRingBufferGetHead(ring_buffer);
    if(head % ring_buffer->capacity + count < ring_buffer->capacity){
        int *src = (int*)data;
        int *dst = dtpRingBufferGetPtr(ring_buffer, head);
        dtpCopyRisc(src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - head & (ring_buffer->capacity - 1);
        int *src = (int*)data;
        int *dst = dtpRingBufferGetPtr(ring_buffer, head);
        dtpCopyRisc(src, dst, first_part);

        int second_part = count - first_part;
        src = (int*)data + first_part;
        dst = ring_buffer->data;
        dtpCopyRisc(src, dst, second_part);
    }
    dtpRingBufferProduce(ring_buffer, count);    
    dtpRingBufferReleaseRead(ring_buffer, count);
}

void dtpRingBufferPop(DtpRingBuffer32 *ring_buffer, void *data, int count){        
    dtpRingBufferCapturedRead(ring_buffer, count);   
    int tail = dtpRingBufferGetTail(ring_buffer);
    if(tail % ring_buffer->capacity + count < ring_buffer->capacity){
        int *src = dtpRingBufferGetPtr(ring_buffer, tail);
        int *dst = (int*)data;        
        dtpCopyRisc(src, dst, count);
    } else {
        int first_part = ring_buffer->capacity - tail & (ring_buffer->capacity - 1);
        int *src = dtpRingBufferGetPtr(ring_buffer, tail); 
        int *dst = (int*)data;        
        dtpCopyRisc(src, dst, first_part);

        int second_part = count - first_part;
        src = ring_buffer->data;
        dst = (int*)data + first_part;
        dtpCopyRisc(src, dst, second_part);
    }
    dtpRingBufferConsume(ring_buffer, count);
    dtpRingBufferReleaseWrite(ring_buffer, count);
}

int dtpRingBufferIsEmpty(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->read_semaphore <= 0;
}


int dtpRingBufferIsFull(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->write_semaphore <= 0;
}


int dtpRingBufferAvailable(DtpRingBuffer32 *ring_buffer){
    return ring_buffer->head - ring_buffer->tail;
}


void dtpRingBufferCapturedRead(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->read_semaphore--;
    while(ring_buffer->read_semaphore < 0){
        //for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }        

}

void dtpRingBufferReleaseRead(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->read_semaphore++;
}


void dtpRingBufferCapturedWrite(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->write_semaphore--;
    while(ring_buffer->write_semaphore < 0){
        //for(int i = 0; i < CLOCKS_PER_SEC; i++);
    }
}

void dtpRingBufferReleaseWrite(DtpRingBuffer32 *ring_buffer, int count){
    ring_buffer->write_semaphore++;
}



