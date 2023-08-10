#include "stddef.h"
#include <stdint.h>
#include "malloc.h"
#include "stdio.h"
#include "dtp/mc12101.h"
#include "hal/ringbuffert.h"
#include "dtp/dtp.h"


static int halRbRecv(void *com_spec, DtpAsync *task){
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *)com_spec;
    connector->pop((int*)task->buf, task->nwords);
    return 0;
}
static int halRbSend(void *com_spec, DtpAsync *task){
    HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *connector = (HalRingBufferConnector<int, DTP_RING_BUFFER_SIZE_32> *)com_spec;
    connector->push((int*)task->buf, task->nwords);
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

