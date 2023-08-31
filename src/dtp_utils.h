#ifndef __DTP_UTILS_H_INCLUDED__
#define __DTP_UTILS_H_INCLUDED__


template<class T, int SIZE> struct DtpPoolT{
    T data[SIZE];
    int enabled[SIZE];
    int size;
    int capacity;

    void init(){
        capacity = SIZE;
        clear();
    }

    T *alloc(){
        if(size == capacity) return 0;

        for(int i = 0; i < SIZE; i++){
            if(enabled[i] == 0){
                enabled[i] = 1;
                T *result = data + i;
                size++;
                return result;
            }
        }
        return 0;
    }

    void clear(){
        size = 0;
        for(int i = 0; i < SIZE; i++){
            enabled[i] = 0;
        }
    }

    void free(T *t){
        for(int i = 0; i < SIZE; i++){
            if(t == data + i){
                enabled[i] = 0;
                size--;
                return;
            }
        }
    }
};



template<class T, int SIZE> struct DtpListT{

    struct DtpListNode{
        T *value;
        DtpListNode *next;
    };

    DtpListNode _first;

    int add(T* obj){
        return 0;
    }

    int remove(T* obj){
        return 0;
    }

    int begin(){

    }
};


#endif //__DTP_UTILS_H_INCLUDED__