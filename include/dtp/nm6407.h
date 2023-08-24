#ifndef __DTP_NM6407_H_INCLUDED__
#define __DTP_NM6407_H_INCLUDED__

typedef enum{
    DTP_NM6407_LINK_OUTPUT = 0,
    DTP_NM6407_LINK_INPUT = 1
} DtpNm6407LinkDirection;

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

    int dtpNm6407InitBuffer(int desc, void *data_in, int capacity_in, void *data_out, int capacity_out, int index);

    int dtpNm6407InitDefaultBuffer(int desc, int index);

    int dtpNm6407ConnectBuffer(int desc, int index);

    int dtpNm6407Link(int desc, int port);

    int dtpNm6407Dma(int desc, int mask);


    int dtpOpenNm6407Dma(int mask);    

    int dtpOpenNm6407Spi(); //not working
    

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_NM6407_H_INCLUDED__