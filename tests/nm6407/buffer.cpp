#include "dtp/dtp.h"
#include "dtp/buffer.h"
#include "dtp/nm6407.h"
#undef NDEBUG
#include "nmassert.h"
#include "stdio.h"

void test_master_whenOpenedBuffer_shouldCorrect(){
    printf("%s\n", __FUNCTION__);
    int data[4];
    int ok;

    int desc[DTP_BUFFER_COUNT];
    for(int i = 0; i < DTP_BUFFER_COUNT; i++){
        desc[i] = dtpOpen(DTP_READ_WRITE);
        NMASSERT(desc[i] > 0);

        ok = dtpNm6407InitDefaultBuffer(desc[i], i);
        NMASSERT(ok == 0);
    }
    printf("descriptors inited\n");
    
    //int ok = dtpListen(desc);
    for(int d = 0; d < DTP_BUFFER_COUNT; d++){
        printf("buffer %d\n", d);
        
        data[0] = 1;
        data[1] = 2;
        data[2] = 3;
        dtpSend(desc[d], data, 3);
        for(int i = 0; i < 10; i++){    
            int old_value[2] = {data[0], data[1] };

            ok = dtpSend(desc[d], data, 2);
            NMASSERT(ok != DTP_ERROR);

            while(1){
                ok = dtpRecv(desc[d], data, 2);
                NMASSERT(ok != DTP_ERROR);
                if (ok == DTP_OK) break;
            }
        

            NMASSERT(data[0] == old_value[0] + 1);
            NMASSERT(data[1] == old_value[1] + 1);
        }
        ok = dtpSend(desc[d], data, 1);
        dtpClose(desc[d]);
    }   
    
}

void test_slave_whenOpenedBuffer_shouldCorrect(){
    printf("%s\n", __FUNCTION__);
    int desc[DTP_BUFFER_COUNT];
    int ok;
    int data[4];
    for(int i = 0; i < DTP_BUFFER_COUNT; i++){
        desc[i] = dtpOpen(DTP_READ_WRITE);
        NMASSERT(desc[i] > 0);

        ok = dtpNm6407ConnectBuffer(desc[i], i);
        NMASSERT(ok == 0);
    }
        
    for(int d = 0; d < DTP_BUFFER_COUNT; d++){
        printf("buffer %d\n", d);
        while(1){
            ok = dtpRecv(desc[d], data, 3);
            NMASSERT(ok != DTP_ERROR);
            if (ok == DTP_OK) break;
        }        
        NMASSERT(data[0] == 1);
        NMASSERT(data[1] == 2);
        NMASSERT(data[2] == 3);
        for(int i = 0; i < 10; i++){
            data[0] = 0xCDCDCDCD;
            data[1] = 0xCDCDCDCD;
            while(1){
                ok = dtpRecv(desc[d], data, 2);                
                NMASSERT(ok != DTP_ERROR);
                if (ok == DTP_OK) break;
            }
            data[0]++;
            data[1]++;
            ok = dtpSend(desc[d], data, 2);
            NMASSERT(ok == DTP_OK);
        }
        ok = dtpRecv(desc[d], data, 1);
        dtpClose(desc[d]);
    }    
}

void test_master_whenBigData_shouldCopyCorrect(){
    printf("%s\n", __FUNCTION__);
    int dtp = dtpOpen(DTP_READ_WRITE);
    NMASSERT(dtp > 0);

    dtpNm6407InitDefaultBuffer(dtp, 0);

    const int size = 1024;

    int data[size];
    for(int i = 0; i < size; i++){
        data[i] = 0xCDCDCDCD;
    }

    while(1){        
        int ok = dtpRecv(dtp, data, size);
        NMASSERT(ok != DTP_ERROR);
        if(ok == DTP_OK) break;
    }

    for(int i = 0; i < size; i++){
        NMASSERT(data[i] == i);
    }

    dtpClose(dtp);
}

void test_slave_whenBigData_shouldCopyCorrect(){
    printf("%s\n", __FUNCTION__);
    int dtp = dtpOpen(DTP_READ_WRITE);
    NMASSERT(dtp > 0);

    dtpNm6407ConnectBuffer(dtp, 0);

    const int size = 1024;

    int data[size];
    for(int i = 0; i < size; i++){
        data[i] = i;
    }

    int ok = dtpSend(dtp, data, size);
    NMASSERT(ok != DTP_ERROR);

    dtpClose(dtp);
}

int main(){
    
#ifdef LISTEN
    //test_master_whenOpenedBuffer_shouldCorrect();
    test_master_whenBigData_shouldCopyCorrect();
#else
    //test_slave_whenOpenedBuffer_shouldCorrect();
    test_slave_whenBigData_shouldCopyCorrect();
#endif 
    
    return 0;
}