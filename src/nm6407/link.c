#include "dtp/dtp.h"
#include "link-core.h"

typedef struct {
    int port;
} DtpLinkData;


static size_t linkRead(void* user_data, void *buf, size_t size){
    DtpLinkData *data = (DtpLinkData *)user_data;    
    dtpLinkStart(buf, size / 4, data->port, LINK_INPUT);
    while(!dtpLinkIsCompleted(data->port, LINK_INPUT));    
    return size;
}

static size_t linkWrite(void* user_data, const void *buf, size_t size){
    DtpLinkData *data = (DtpLinkData *)user_data;
    dtpLinkStart((void *)buf, size / 4, data->port, LINK_OUTPUT);
    return size;
}

static int linkFlush(void* user_data){
    DtpLinkData *data = (DtpLinkData *)user_data;    
    while(!dtpLinkIsCompleted(data->port, LINK_OUTPUT));
    return 0;
}

int dtpOpenLink(int port){
    DtpLinkData *data = (DtpLinkData *)malloc(sizeof(DtpLinkData *));
    if(data == 0){
        return -1;
    }

    data->port = port;
    //dtpLinkInit();
    DtpImplementation impl;
    impl.recv = linkRead;
    impl.send = linkWrite;
    impl.flush = linkFlush;
    impl.user_data = data;

    return dtpOpen(&impl);
}