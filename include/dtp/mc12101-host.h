#ifndef __DTP_MC12101_HOST_H_INCLUDED__
#define __DTP_MC12101_HOST_H_INCLUDED__
typedef struct PL_Access PL_Access;

#ifdef __cplusplus
extern "C" {
#endif

    int dtpOpenPloadFileHost(PL_Access *access, const char *filename);

#ifdef __cplusplus
}
#endif
#endif //__DTP_MC12101_HOST_H_INCLUDED__