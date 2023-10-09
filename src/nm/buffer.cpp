
#include "ringbuffer.h"
#include "dtp/mc12101.h"
#include "stdio.h"

extern "C" int ncl_getProcessorNo(void);

const uintptr_t ADDR_TABLE_ADDR = (0xA8000 - 0x10);

typedef struct {
    DtpRingBuffer32 *rb_in;
    DtpRingBuffer32 *rb_out;
    int index;
} RingBufferData;

__attribute__ ((section (".data.dtp.buffer0"))) int dtp_buffer0_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer0"))) int dtp_buffer0_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer1"))) int dtp_buffer1_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer1"))) int dtp_buffer1_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer2"))) int dtp_buffer2_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer2"))) int dtp_buffer2_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer3"))) int dtp_buffer3_data_in [DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.buffer3"))) int dtp_buffer3_data_out[DTP_BUFFER_SIZE];
__attribute__ ((section (".data.dtp.ringbuffers"))) RingBufferData ringbuffers_static[DTP_BUFFER_COUNT];
static DtpRingBuffer32 **ringbuffers_table = (DtpRingBuffer32 **)ADDR_TABLE_ADDR;

void dtpRingBufferInit(DtpRingBuffer32 *ringbuffer, void *data, int capacity);




extern "C" {

    static int blockingPop(DtpRingBuffer32 *ringbuffer, int* data, int size){
        while(size > 0){
            int head = dtpRingBufferGetHead(ringbuffer);
            int tail = dtpRingBufferGetTail(ringbuffer);
            int available = head - tail;                

            int pop_size = (available > size) ? size : available;            
            if(pop_size > 0){
                dtpRingBufferPop(ringbuffer, data, pop_size);                    
                data += pop_size;
                size -= pop_size;
            }            
        }
    }

    static int blockingPush(DtpRingBuffer32 *ringbuffer, int* data, int size){        
        while(size > 0){
            int head = dtpRingBufferGetHead(ringbuffer);
            int tail = dtpRingBufferGetTail(ringbuffer);
            int capacity = dtpRingBufferGetCapacity(ringbuffer);
            int available = tail + capacity - head;

            int push_size = (available > size) ? size : available;            
            if(push_size > 0){
                dtpRingBufferPush(ringbuffer, data, push_size);
                data += push_size;
                size -= push_size;
            }               
        }
    }

    int dtpRingBufferImplSend(void *com_spec, DtpAsync *cmd){
        RingBufferData *data = (RingBufferData *)com_spec;
        int *src = (int *)cmd->buf;
        int size = cmd->nwords;
        int head, tail, available;

        int capacity = dtpRingBufferGetCapacity(data->rb_out);
        head = dtpRingBufferGetHead(data->rb_out);
        tail = dtpRingBufferGetTail(data->rb_out);
        //printf("rb send: head=%d, tail=%d\n", head, tail);
        available = tail + capacity - head;        
        if(available == 0) return DTP_AGAIN;

        if(cmd->type == DTP_TASK_1D){
            blockingPush(data->rb_out, src, size);    
        } else {
            for(int i = 0; i < size; i += cmd->width){
                blockingPush(data->rb_out, src, cmd->width);
                src += cmd->stride;
            }
        }   
        if(cmd->callback){
            cmd->callback(cmd->cb_data);
        } 
        return 0;
    }

    int dtpRingBufferImplRecv(void *com_spec, DtpAsync *cmd){
        RingBufferData *data = (RingBufferData *)com_spec;
        int *dst = (int *)cmd->buf;
        int size = cmd->nwords;
        int head, tail, available;

        int capacity = dtpRingBufferGetCapacity(data->rb_in);
        head = dtpRingBufferGetHead(data->rb_in);
        tail = dtpRingBufferGetTail(data->rb_in);
        available = head - tail;        
        //printf("rb recv: head=%d, tail=%d\n", head, tail);
        if(available == 0) return DTP_AGAIN;

        if(cmd->type == DTP_TASK_1D){            
            blockingPop(data->rb_in, dst, size);            
        } else {
            for(int i = 0; i < size; i += cmd->width){
                blockingPop(data->rb_in, dst, cmd->width);
                dst += cmd->stride;
            }
        }    
        if(cmd->callback){
            cmd->callback(cmd->cb_data);
        }
        return 0;
    }


    int dtpRingBufferImplGetStatus(void *com_spec, DtpAsync *cmd){
        cmd->DTP_ASYNC_PRIVATE_FIELDS.status = DTP_ST_DONE;
        return DTP_ST_DONE;
    }

    int dtpNm6407InitBuffer(int desc, void *data_in, int capacity_in, void *data_out, int capacity_out, int bufferIndex){
        dtpBufferInit(desc, data_in, capacity_in, data_out, capacity_out, bufferIndex);
    }

    int dtpBufferInit(int desc, void *data_in, int capacity_in, void *data_out, int capacity_out, int bufferIndex){    
    
        RingBufferData *data = ringbuffers_static + bufferIndex;
        data->rb_in = dtpRingBufferAlloc(data_in, capacity_in);
        data->rb_out = dtpRingBufferAlloc(data_out, capacity_out);        

        int offset = 0x40000;
        if(ncl_getProcessorNo()){
            offset += 0x40000;
        }

        // dtpRingBufferInit(data->rb_in, data_in, capacity_in);
        // dtpRingBufferInit(data->rb_out, data_out, capacity_out);
        data->index = bufferIndex;

        DtpImplementation impl;
        impl.recv = dtpRingBufferImplRecv;
        impl.send = dtpRingBufferImplSend;
        impl.update_status = dtpRingBufferImplGetStatus;
        impl.destroy = 0;

        
        ringbuffers_table[2 * data->index] = data->rb_in;
        ringbuffers_table[2 * data->index + 1] = data->rb_out;
        
        if((int) ringbuffers_table[2 * data->index] < 0xA0000){
            ringbuffers_table[2 * data->index] = (DtpRingBuffer32 *)(  (int*)ringbuffers_table[2 * data->index] + offset  );
        }
        if((int) ringbuffers_table[2 * data->index + 1] < 0xA0000){
            ringbuffers_table[2 * data->index + 1] = (DtpRingBuffer32 *)(  (int*)ringbuffers_table[2 * data->index + 1] + offset  );
        }           

        return dtpBind(desc, data, &impl);
    }

    
    int dtpNm6407InitDefaultBuffer(int desc, int index){
        dtpBufferInitDefault(desc, index);
    }

    int dtpBufferInitDefault(int desc, int index){
        switch(index){
            case 0:
            return dtpNm6407InitBuffer(desc, dtp_buffer0_data_in, DTP_BUFFER_SIZE, dtp_buffer0_data_out, DTP_BUFFER_SIZE, 0);
            case 1:
            return dtpNm6407InitBuffer(desc, dtp_buffer1_data_in, DTP_BUFFER_SIZE, dtp_buffer1_data_out, DTP_BUFFER_SIZE, 1);
            case 2:
            return dtpNm6407InitBuffer(desc, dtp_buffer2_data_in, DTP_BUFFER_SIZE, dtp_buffer2_data_out, DTP_BUFFER_SIZE, 2);
            case 3:
            return dtpNm6407InitBuffer(desc, dtp_buffer3_data_in, DTP_BUFFER_SIZE, dtp_buffer3_data_out, DTP_BUFFER_SIZE, 3);
            default:
            return -1;
        }
        
    }


    int dtpNm6407ConnectBuffer(int desc, int index){
        dtpBufferConnect(desc, index);
    }

    int dtpBufferConnect(int desc, int channel){
        RingBufferData *data = ringbuffers_static + channel;

        data->index = channel;
        data->rb_out = ringbuffers_table[2 * data->index];
        data->rb_in  = ringbuffers_table[2 * data->index + 1];

        DtpImplementation impl;
        impl.recv = dtpRingBufferImplRecv;
        impl.send = dtpRingBufferImplSend;
        impl.update_status = dtpRingBufferImplGetStatus;
        impl.destroy = 0;
        return dtpBind(desc, data, &impl);
    }

    int dtpBufferSetTableAddr(unsigned int addr){
        ringbuffers_table = (DtpRingBuffer32 **)addr;
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
            rb_in_data  = dtp_buffer0_data_in;
            rb_out_data = dtp_buffer0_data_out;
            if((int)dtp_buffer0_data_in < 0x100000)  rb_in_data  += offset;
            if((int)dtp_buffer0_data_out < 0x100000) rb_out_data += offset;
            break;
        case 1:
            rb_in_data  = dtp_buffer1_data_in;
            rb_out_data = dtp_buffer1_data_out;
            if((int)dtp_buffer1_data_in < 0x100000)  rb_in_data  += offset;
            if((int)dtp_buffer1_data_out < 0x100000) rb_out_data += offset;
            break;
        case 2:
            rb_in_data  = dtp_buffer2_data_in;
            rb_out_data = dtp_buffer2_data_out;
            if((int)dtp_buffer2_data_in < 0x100000)  rb_in_data  += offset;
            if((int)dtp_buffer2_data_out < 0x100000) rb_out_data += offset;
            break;
        case 3:
            rb_in_data  = dtp_buffer3_data_in;
            rb_out_data = dtp_buffer3_data_out;
            if((int)dtp_buffer3_data_in < 0x100000)  rb_in_data  += offset;
            if((int)dtp_buffer3_data_out < 0x100000) rb_out_data += offset;
            break;    
        default:
            return -1;
            break;
        }
        
        data->rb_in =  dtpRingBufferAlloc(rb_in_data, DTP_BUFFER_SIZE); 
        data->rb_out = dtpRingBufferAlloc(rb_out_data, DTP_BUFFER_SIZE);
        if(data->rb_in == 0 || data->rb_out == 0){
            if(data->rb_in) free(data->rb_in);
            if(data->rb_out) free(data->rb_out);
            return DTP_ERROR;
        }

        ringbuffers_table[2 * data->index] = data->rb_in;
        ringbuffers_table[2 * data->index + 1] = data->rb_out;
        if((int) ringbuffers_table[2 * data->index] < 0x100000){
            ringbuffers_table[2 * data->index] = (DtpRingBuffer32 *)((int*)ringbuffers_table[2 * data->index] + offset);
        }
        if((int) ringbuffers_table[2 * data->index + 1] < 0x80000){
            ringbuffers_table[2 * data->index + 1] = (DtpRingBuffer32 *)((int*)ringbuffers_table[2 * data->index + 1] + offset);
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
        data->rb_in = ringbuffers_table[2 * data->index + 1];
        data->rb_out = ringbuffers_table[2 * data->index];

        int handshake = 0x12300123;
        dtpRingBufferPush(data->rb_out, &handshake, 1);
        dtpRingBufferPop(data->rb_in, &handshake, 1);
        if(handshake != 0x12300124) return -1;

        return 0;
    }


    int dtpRingBufferImplDestroy(void *com_spec){
        RingBufferData *data = (RingBufferData *)com_spec;
        ringbuffers_table[data->index * 2] = 0;
        ringbuffers_table[data->index * 2 + 1] = 0;
        free(data->rb_in);
        free(data->rb_out);
        free(data);
        return 0;
    }

    int dtpConnectSharedBuffer(int index){
        RingBufferData *data = (RingBufferData *)malloc( sizeof (RingBufferData) );
        if(data == 0) return -1;
        data->index = index;

        DtpImplementation impl;
        impl.recv = dtpRingBufferImplRecv;
        impl.send = dtpRingBufferImplSend;        
        impl.update_status = dtpRingBufferImplGetStatus;
        impl.destroy = dtpRingBufferImplDestroy;
        return dtpOpenCustom(data, &impl);
    }

}