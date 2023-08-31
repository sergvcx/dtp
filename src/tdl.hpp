#ifndef __TDL_H_INCLUDED__
#define __TDL_H_INCLUDED__
	
template<class T, int CAPACITY> struct TdlPool{
private:
public:
    int size;
	int capacity;

	T data[CAPACITY];
	int enabled[CAPACITY];

	TdlPool(){
        init();
    }

	void init(){
		capacity = CAPACITY;
		clear();
	}

	T *alloc(){
        if(size == capacity) return 0;

        for(int i = 0; i < CAPACITY; i++){
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
        for(int i = 0; i < CAPACITY; i++){
            enabled[i] = 0;
        }
    }

    void free(T *t){
        for(int i = 0; i < CAPACITY; i++){
            if(t == data + i){
                enabled[i] = 0;
                size--;
                return;
            }
        }
    }
};

#endif //__TDL_H_INCLUDED__