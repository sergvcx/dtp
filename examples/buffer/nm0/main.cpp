#include "dtp/dtp.h"
#include "dtp/file.h"
#include "hal/ringbuffert.h"
#include "dtp/mc12101.h"
#include "mc12101load_nm.h"

#define FILE "exchange.bin"

int data_host_input[DTP_RING_BUFFER_SIZE_32];
int data_host_output[DTP_RING_BUFFER_SIZE_32];

int data_nm1_input[DTP_RING_BUFFER_SIZE_32];
int data_nm1_output[DTP_RING_BUFFER_SIZE_32];

HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> ring_host_input;
HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> ring_host_output;

HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> ring_nm1_input;
HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> ring_nm1_output;

int main(){

    int file_desc = dtpOpenFile(FILE,"wb");    

    ring_host_input.init();
    ring_host_input.data = data_host_input;
    ring_host_output.init();
    ring_host_output.data = data_host_output;

    ring_nm1_input.init();
    ring_nm1_input.data = data_nm1_input;
    ring_nm1_output.init();
    ring_nm1_output.data = data_nm1_output;
    printf("input: %p, output: %p\n", &ring_host_input, &ring_host_output);
    printf("data_host_input: %p, data_host_output: %p\n", data_host_input, data_host_output);

    // write to file addr of pc-nm0 ring buffers
    int ring_input_addr = (int)&ring_host_input;
    int ring_output_addr = (int)&ring_host_output;
    int offset = 0x40000 + 0x40000 * ncl_getProcessorNo();
    if(ring_input_addr < 0x80000) ring_input_addr+=offset;
    if(ring_output_addr < 0x80000) ring_output_addr+=offset;
    printf("ring_input_addr %p, ring_output_addr %p\n", ring_input_addr, ring_output_addr);
    dtpSend(file_desc, &ring_input_addr, 1);
    dtpSend(file_desc, &ring_output_addr, 1);

    // write to file addr of nm0-nm1 ring buffers
    ring_input_addr = (int)&ring_nm1_input;
    ring_output_addr = (int)&ring_nm1_output;
    if(ring_input_addr < 0x80000) ring_input_addr+=offset;
    if(ring_output_addr < 0x80000) ring_output_addr+=offset;
    printf("ring_input_addr %p, ring_output_addr %p\n", ring_input_addr, ring_output_addr);
    dtpSend(file_desc, &ring_input_addr, 1);
    dtpSend(file_desc, &ring_output_addr, 1);
    dtpClose(file_desc);

    
    //--------------pc-nm0----------------
    int rb_desc_r = dtpOpenRingbufferDefault(&ring_host_input);
    int rb_desc_w = dtpOpenRingbufferDefault(&ring_host_output);

    int data[2] = {0, 1};
    dtpSend(rb_desc_w, data, 2);

    dtpRecv(rb_desc_r, data, 2);
    printf("recv: %d, %d\n", data[0], data[1]);

    dtpClose(rb_desc_r);
    dtpClose(rb_desc_w);




    //--------------nm0-nm1----------------
    rb_desc_r = dtpOpenRingbufferDefault(&ring_nm1_input);
    rb_desc_w = dtpOpenRingbufferDefault(&ring_nm1_output);

    data[0] = 0;
    data[1] = 1;
    dtpSend(rb_desc_w, data, 2);

    dtpRecv(rb_desc_r, data, 2);
    printf("recv: %d, %d\n", data[0], data[1]);

    dtpClose(rb_desc_r);
    dtpClose(rb_desc_w);

    return 0;
}