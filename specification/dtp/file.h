#ifndef __DTP_FILE_H_INCLUDED__
#define __DTP_FILE_H_INCLUDED__
#include "dtp/dtp.h"

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

    int dtp_open_file(int dtp, FILE *file);

    int dtp_open_file2(int dtp, FILE *input, FILE *output);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_FILE_H_INCLUDED__