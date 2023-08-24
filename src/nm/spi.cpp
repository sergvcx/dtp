#include "dtp/dtp.h"
#include "stdio.h"

struct DtpSpiData{
    alignas(8) int tr_buf[2];
    alignas(8) int rc_buf[2];
    int *base;    
};

static int dtpSpiImplRecv(void *com_spec, DtpAsync *cmd){
    DtpSpiData *data = (DtpSpiData *) com_spec;
    int size = cmd->nwords;

    int *ssp_cr1 = data->base + 1;
    int *ssp_dr = data->base + 2;
    int *ssp_sr = data->base + 3;

    int *words = (int*)cmd->buf;
    while(size > 0){
        int result = 0;
        int tmp = 0;
        printf("size = %d\n", size);
        
        for(int i = 0; i < 4; i++){
            *ssp_cr1 = 0x2;
            
            while( (*ssp_sr) & 0x10);        
            tmp = *ssp_dr;
            result |= tmp;
            result <<= 8;
            printf("recv = 0x%x\n", tmp);
        }

        *words = result;
        words++;
        size--;

    }
    *ssp_cr1 = 0x2;
    return DTP_OK;
}

static int dtpSpiImplSend(void *com_spec, DtpAsync *cmd){
    DtpSpiData *data = (DtpSpiData *) com_spec;
    int size = cmd->nwords;

    int *ssp_cr1 = data->base + 1;
    int *ssp_dr = data->base + 2;
    int *ssp_sr = data->base + 3;
    
    int *words = (int*)cmd->buf;
    while(size > 0){
        printf("size = %d\n", size);
        int tmp = *words;
        for(int i = 0; i < 4; i++){
            *ssp_dr = tmp;
            tmp >>= 8;
            *ssp_cr1 = 0x2;
            printf("send = 0x%x\n", tmp);
            while( (*ssp_sr) & 0x10 );
        }
        words++;
        size--;
    }

    return DTP_OK;
}

static int dtpSpiImplGetStatus(void *com_spec, DtpAsync *cmd){
    return DTP_ST_DONE;
}

static int dtpSpiImplDestroy(void *com_spec){
    return DTP_OK;
}

extern "C"{
    int dtpOpenNm6407Spi(){
        DtpSpiData *com_spec = (DtpSpiData *) malloc( sizeof(DtpSpiData) );
        if(com_spec == 0) return -1;

        com_spec->base = (int *)0x10031000;
        int *ssp_cr0 = com_spec->base;
        int *ssp_dmacr = com_spec->base + 9;
        *ssp_dmacr = 0;
        //*ssp_cr0 = 0x0C7;
        *ssp_cr0 = 0x007;
        

        DtpImplementation impl;
        impl.recv = dtpSpiImplRecv;
        impl.send = dtpSpiImplSend;
        impl.get_status = dtpSpiImplGetStatus;
        impl.destroy = dtpSpiImplDestroy;
        impl.listen = 0;
        impl.connect = 0; 
        return dtpOpenCustom(com_spec, &impl);
    }
}