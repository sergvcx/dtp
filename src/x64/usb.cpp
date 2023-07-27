#include "dtp/ringbuffer.h"
#include "dtp/dtp-hal.h"
#include "hal/hal.h"

typedef struct {
    DtpRingBuffer32 *rb_in;
    DtpRingBuffer32 *rb_out;    
    int desc_in;
    int desc_out;
    HalAccess *access;
} DtpUsbData;

extern "C"{
    size_t usbRecv(void *user_data, void *buf, size_t size){
        DtpUsbData *data = (DtpUsbData*)user_data;
        int message[2];
        size_t header_size = dtpRecv(data->desc_in, message, 2 * 4);
        if(size < 8) return 0;

        halReadMemBlock(data->access, buf, message[0], message[1]);

        return size;
    }

    size_t usbSend(void *user_data, const void *buf, size_t size){
        DtpUsbData *data = (DtpUsbData*)user_data;
        int message[2];
        while(true){
            size_t header_size = dtpRecv(data->desc_in, message, 2 * 4);
            if(header_size > 0 && header_size != 8) return 0;
            if(header_size == 8) break;
        }

        halWriteMemBlock(data->access, buf, message[0], message[1]);

        message[0] = 1;
        dtpSend(data->desc_out, message, 4);

        return size;
    }

    int usbDestroy(void* user_data){
        DtpUsbData *data = (DtpUsbData*)user_data;
        free(data);
        return 0;
    }

    int dtpOpenPload(HalAccess *access, uintptr_t buffer_addr){
        DtpUsbData *data = new DtpUsbData();
        data->access = access;
        uintptr_t rb_nm_in_addr = 0;
        uintptr_t rb_nm_out_addr = 0;
        halReadMemBlock(access, &rb_nm_in_addr, buffer_addr, 1);
        halReadMemBlock(access, &rb_nm_out_addr, buffer_addr + 1, 1);
        // nm in is host out
        // nm out is host in
        data->rb_in = dtpRingBufferBind(access, rb_nm_out_addr);
        data->rb_out = dtpRingBufferBind(access, rb_nm_in_addr);

        data->desc_in = dtpOpenBuffer(data->rb_in);
        data->desc_out = dtpOpenBuffer(data->rb_out);    

        DtpImplemention impl;
        impl.send = usbSend;
        impl.recv = usbRecv;
        impl.flush = 0;
        impl.destroy = usbDestroy;
        return dtpOpen(data, &impl);
    }

}