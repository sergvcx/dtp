#include "dtp/ringbuffer.h"
#include "dtp/dtp-hal.h"

#define DTP_USB_BUFFER_IN_SIZE_32 32
#define DTP_USB_BUFFER_OUT_SIZE_32 32

typedef struct {
    DtpRingBuffer32 *rb_in;
    DtpRingBuffer32 *rb_out;
    int data_in[DTP_USB_BUFFER_IN_SIZE_32];
    int data_out[DTP_USB_BUFFER_OUT_SIZE_32];
    int buffer_desc_in;
    int buffer_desc_out;
    int message[2];
} DtpUsbData;


int usbDestroy(void *user_data){
    DtpUsbData *data = (DtpUsbData *)user_data;
    delete data;
    return 0;
}

size_t usbRecv(void *user_data, void *buf, size_t size){
    DtpUsbData *data = (DtpUsbData *)user_data;
    data->message[0] = (int)buf;
    data->message[1] = (int)size / 4;
    
    
    int size_header = dtpSend(data->buffer_desc_out, data->message, 2 * 4);
    if(size_header != 8) return 0;

    data->message[0] = 0;
    do{
        size_header = dtpRecv(data->buffer_desc_in, data->message, 4);
    } while (data->message[0] == 0);

    return size;
}

size_t usbSend(void *user_data, const void *buf, size_t size){
    DtpUsbData *data = (DtpUsbData *)user_data;
    
    data->message[0] = (int)buf;
    data->message[1] = (int)size / 4;
    dtpSend(data->buffer_desc_out, data->message, 2 * 4);
    return size;
}


int dtpOpenPload(HalAccess *access, uintptr_t buffer_addr){
    DtpUsbData *data = new DtpUsbData();
    data->rb_in = dtpRingBufferCreate(data->data_in, DTP_USB_BUFFER_IN_SIZE_32);
    data->rb_out = dtpRingBufferCreate(data->data_out, DTP_USB_BUFFER_IN_SIZE_32);
    data->buffer_desc_in = dtpOpenBuffer(data->rb_in);
    data->buffer_desc_out = dtpOpenBuffer(data->rb_out);

    DtpImplemention impl;
    impl.recv = usbRecv;
    impl.send = usbSend;
    impl.flush = 0;
    impl.destroy = usbDestroy;
    return dtpOpen(data, &impl);
}

