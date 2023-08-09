#ifndef __DTP_FILE_H_INCLUDED__
#define __DTP_FILE_H_INCLUDED__

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

    int dtpOpenFile(const char *filename, const char *mode);
    int dtpOpenFile2(const char *filename_input, const char *filename_output);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__DTP_FILE_H_INCLUDED__