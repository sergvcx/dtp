#include "stddef.h"
#include <stdint.h>
#include "malloc.h"
#include "stdio.h"
#include "dtp/mc12101.h"
#include "hal/ringbuffert.h"
#include "dtp/dtp.h"
#include "string.h"


static int halRbRecv(void *com_spec, DtpAsync *task){
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *)com_spec;
    //printf("pop\n");
    //printf("task->nwords %d\n", task->nwords);
    connector->pop((int*)task->buf, task->nwords);
    //printf("end pop\n");
    return 0;
}
static int halRbSend(void *com_spec, DtpAsync *task){
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *)com_spec;
    //printf("push\n");
    //printf("task->nwords %d\n", task->nwords);
    connector->push((int*)task->buf, task->nwords);
    //printf("end push\n");
    return 0;
}

static int halRbStatus(void *com_spec, DtpAsync *task){
    return DTP_ST_DONE;
}
static int halRbDestroy(void *com_spec){
    //BoardData *board= (BoardData *) board;
    return 0;
}


static HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> ringbuffer_connector;

int dtpOpenRingbuffer(void *hal_ring_buffer, DtpMemCopyFuncT push_func, DtpMemCopyFuncT pop_func){
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = &ringbuffer_connector;

    connector->init((HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *) hal_ring_buffer, push_func, pop_func);

    DtpImplementation impl;
	impl.recv_func=halRbRecv;
	impl.send_func=halRbSend;
	impl.get_status_func=halRbStatus;
    return dtpOpenCustom(connector, &impl);
}

static void* memCopyDefault(const void *src, void *dst, unsigned int size32){
    memcpy(dst, src, size32 * sizeof(int));
    return 0;
}

void halSleep(int msec){

}
// where  free/delete ???
int dtpOpenRingbufferDefault(void *hal_ring_buffer){
    //HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = new HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32>();
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *) malloc( sizeof( HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32>));

    connector->init((HalRingBufferData<int, DTP_RING_BUFFER_SIZE_32> *) hal_ring_buffer, memCopyDefault, memCopyDefault);
	printf("bufferId :%x\n",connector->_bufferId);
	printf("sizeOfInt:%d\n",connector->sizeofBufferInt);
	printf("size     :%d\n",connector->size);
	printf("size1    :%d\n",connector->size1);
	printf("data     :%x\n",connector->data);
    DtpImplementation impl;
	impl.recv_func=halRbRecv;
	impl.send_func=halRbSend;
	impl.get_status_func=halRbStatus;
    impl.destroy_func = 0;
    return dtpOpenCustom(connector, &impl);
}

