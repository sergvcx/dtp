#ifndef __NMSEMAPHORE_H_INCLUDED__
#define __NMSEMAPHORE_H_INCLUDED__

typedef struct alignas(8) {
    int value;
} dtp_sem_t;

#ifdef __cplusplus
extern "C" {
#endif
    void dtp_sem_init(dtp_sem_t *sem, int value);
    void dtp_sem_wait(dtp_sem_t *sem);
    //void nm_sem_try_wait(nm_sem_t *sem);
    void dtp_sem_getvalue(dtp_sem_t *sem, int *value);
    void dtp_sem_post(dtp_sem_t *sem);
#ifdef __cplusplus
}
#endif
#endif //__NMSEMAPHORE_H_INCLUDED__