#ifndef __NMSEMAPHORE_H_INCLUDED__
#define __NMSEMAPHORE_H_INCLUDED__

typedef struct alignas(8) {
    int value;
} nm_sem_t;

#ifdef __cplusplus
extern "C" {
#endif
    void nm_sem_init(nm_sem_t *sem, int value);
    void nm_sem_wait(nm_sem_t *sem);
    //void nm_sem_try_wait(nm_sem_t *sem);
    void nm_sem_getvalue(nm_sem_t *sem, int *value);
    void nm_sem_post(nm_sem_t *sem);
#ifdef __cplusplus
}
#endif
#endif //__NMSEMAPHORE_H_INCLUDED__