
#include "ringbuffer.h"
#include "dtp/mc12101.h"
#include "stdio.h"

extern "C" int ncl_getProcessorNo(void);

#define ADDR_TABLE_ADDR (0xA8000 - 16)

__attribute__ ((section (".data.dtp.buffer0"))) int dtp_buffer0_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer0"))) int dtp_buffer0_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer1"))) int dtp_buffer1_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer1"))) int dtp_buffer1_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer2"))) int dtp_buffer2_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer2"))) int dtp_buffer2_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer3"))) int dtp_buffer3_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer3"))) int dtp_buffer3_data_out[DTP_BUFFER_SIZE];
static DtpRingBuffer32 **ringbuffers = (DtpRingBuffer32 **)(0xA8000 - 16);


typedef struct {
    DtpRingBuffer32 *rb_in;
    DtpRingBuffer32 *rb_out;
    int index;
} RingBufferData;

extern "C" {
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
                if(push_size){
                    dtpRingBufferPush(data->rb_out, src, push_size);
                    src += push_size;
                    size -= push_size;
                }   
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

        int capacity = dtpRingBufferGetCapacity(data->rb_in);

        if(cmd->type == DTP_TASK_1D){            
            while(size > 0){
                int head = dtpRingBufferGetHead(data->rb_in);
                int tail = dtpRingBufferGetTail(data->rb_in);
                int available = head - tail;                

                int pop_size = (available > size) ? size : available;
                if(pop_size){
                    dtpRingBufferPop(data->rb_in, dst, pop_size);                    
                    dst += pop_size;
                    size -= pop_size;
                }
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

        int offset = 0x40000;
        if(ncl_getProcessorNo()){
            offset += 0x40000;
        }
                
        int *rb_in_data = 0;
        int *rb_out_data = 0;
        switch (data->index)
        {
        case 0:
            if((int)dtp_buffer0_data_in < 0x100000)  rb_in_data  = dtp_buffer0_data_in + offset;
            if((int)dtp_buffer0_data_out < 0x100000) rb_out_data = dtp_buffer0_data_out + offset;        
            break;
        case 1:
            if((int)dtp_buffer1_data_in < 0x100000)  rb_in_data  = dtp_buffer1_data_in + offset;
            if((int)dtp_buffer1_data_out < 0x100000) rb_out_data = dtp_buffer1_data_out + offset;        
            break;
        case 2:
            if((int)dtp_buffer2_data_in < 0x100000)  rb_in_data  = dtp_buffer2_data_in + offset;
            if((int)dtp_buffer2_data_out < 0x100000) rb_out_data = dtp_buffer2_data_out + offset;        
            break;
        case 3:
            if((int)dtp_buffer3_data_in < 0x100000)  rb_in_data  = dtp_buffer3_data_in + offset;
            if((int)dtp_buffer3_data_out < 0x100000) rb_out_data = dtp_buffer3_data_out + offset;        
            break;    
        default:
            return -1;
            break;
        }

        data->rb_in =  dtpRingBufferAlloc(rb_in_data, DTP_BUFFER_SIZE);        
        data->rb_out = dtpRingBufferAlloc(rb_out_data, DTP_BUFFER_SIZE);

        ringbuffers[2 * data->index] = data->rb_in;
        ringbuffers[2 * data->index + 1] = data->rb_out;
        if((int) ringbuffers[2 * data->index] < 0x100000){
            ringbuffers[2 * data->index] = (DtpRingBuffer32 *)((int*)ringbuffers[2 * data->index] + offset);
        }
        if((int) ringbuffers[2 * data->index + 1] < 0x80000){
            ringbuffers[2 * data->index + 1] = (DtpRingBuffer32 *)((int*)ringbuffers[2 * data->index + 1] + offset);
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
        dtpRingBufferPush(data->rb_out, &handshake, 1);
        return 0;
    }

    int dtpRingBufferImplConnect(void *com_spec){
        RingBufferData *data = (RingBufferData *)com_spec;
        data->rb_in = ringbuffers[2 * data->index + 1];
        data->rb_out = ringbuffers[2 * data->index];

        int handshake = 0x12300123;
        dtpRingBufferPush(data->rb_out, &handshake, 1);
        dtpRingBufferPop(data->rb_in, &handshake, 1);
        if(handshake != 0x12300124) return -1;

        return 0;
    }

    int dtpRingBufferImplGetStatus(void *com_spec, DtpAsync *cmd){
        return DTP_ST_DONE;
    }

    int dtpRingBufferImplDestroy(void *com_spec){
        RingBufferData *data = (RingBufferData *)com_spec;
        ringbuffers[data->index * 2] = 0;
        ringbuffers[data->index * 2 + 1] = 0;
        free(data->rb_in);
        free(data->rb_out);
        free(data);
        return 0;
    }

    int dtpOpenSharedBuffer(int index){
        RingBufferData *data = (RingBufferData *)malloc( sizeof (RingBufferData) );
        if(data == 0) return -1;
        data->index = index;

        DtpImplementation impl;
        impl.recv = dtpRingBufferImplRecv;
        impl.send = dtpRingBufferImplSend;
        impl.listen = dtpRingBufferImplListen;
        impl.connect = dtpRingBufferImplConnect;
        impl.get_status = dtpRingBufferImplGetStatus;
        impl.destroy = dtpRingBufferImplDestroy;
        return dtpOpenCustom(data, &impl);
    }

}