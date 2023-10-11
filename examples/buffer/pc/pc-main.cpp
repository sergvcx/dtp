#include "dtp/dtp.h"
#include "dtp/mc12101.h"
#include "stdio.h"
#include "mc12101load.h"
#include <iostream>

#include <chrono>
#include <thread>

#define FILE "exchange.bin"

int main(){
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds

    PL_Board *board;
    PL_Access *access;

    PL_GetBoardDesc(0, &board);
    PL_GetAccess(board, 0, &access);

    int desc = dtpOpen(DTP_READ_WRITE);
    if(desc < 0){
        std::cout << "Failed allocate descriptor" << std::endl;
        return -1;
    }

    dtpMc12101Connect(desc, access, 0);

    int data[2];
    int error = 0;
    for(int i = 0; i < 10; i++){
        std::cout << "iteration: " << i << std::endl;
        do{
            error = dtpRecv(desc, data, 2);
            if(error == DTP_AGAIN) {
                std::cout << "DTP_AGAIN" << std::endl;
                sleep_for(milliseconds(200));
            }
        }while(error == DTP_AGAIN);


        if(data[0] != i || data[1] != i + 1){
            std::cout << "Wrong values" << std::endl;
            return 0;
        }


        data[0]++;
        data[1]++;
        do{
            error = dtpSend(desc, data, 2);
            if(error) {
                std::cout << "DTP_AGAIN" << std::endl;
                sleep_for(milliseconds(200));
            }
        }while(error == DTP_AGAIN);
    }

    dtpClose(desc);

    PL_CloseAccess(access);
    PL_CloseBoardDesc(board);
    return 0;
    
}


