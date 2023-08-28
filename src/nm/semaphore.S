
.global _dtp_sem_init
.global _dtp_sem_wait
.global _dtp_sem_post
.global _dtp_sem_getvalue

.section .text.dtp,"ax",@progbits
//void nm_sem_init(nm_sem_t *sem, int value);
_dtp_sem_init:
    ar5 = [ar7 - 3];
    gr7 = [ar7 - 4];
    delayed return;
        [ar5] = gr7;
        gr7 = false;
        nul;
//void nm_sem_wait(nm_sem_t *sem);
_dtp_sem_wait:
    ar5 = [ar7 - 3];
wait_loop:
    gr7 = [ar5];
    with gr7;
    if =0 delayed skip wait_loop;
        nul;
        nul;
    delayed return;
        gr7--;
        [ar5] = gr7;
        gr7 = false;

//void nm_sem_getvalue(nm_sem_t *sem, int *value);
_dtp_sem_getvalue:
    ar5 = [ar7 - 3];
    gr7 = [ar5];
    ar5 = [ar7 - 4];
    
    delayed return;
        [ar5] = gr7;
        gr7 = false;
        nul;
//void nm_sem_try_wait(nm_sem_t *sem);
//_dtp_sem_try_wait:
//void nm_sem_post(nm_sem_t *sem);
_dtp_sem_post:
    ar5 = [ar7 - 3];    
    delayed return;
        gr7 = [ar5];
        gr7++;
        [ar5] = gr7 with gr7 = false;
