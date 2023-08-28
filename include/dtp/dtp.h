#ifndef __DTP_H_INCLUDED__
#define __DTP_H_INCLUDED__

#include <stdlib.h>

typedef void (*DtpNotifyFunctionT)(void *event_data);

#define DTP_OPEN_MAX 16

#ifdef __GNUC__
#define DEPRECATED __attribute__ ((deprecated))
#else 
#define DEPRECATED 
#endif


typedef enum{
    DTP_OK,
    DTP_ERROR,
    DTP_AGAIN
} DtpError;

typedef enum {
    DTP_TASK_NONE,
    DTP_TASK_1D,
    DTP_TASK_2D
} DtpAsyncType;

typedef struct {
    volatile void *buf;
    size_t nwords;
    int width;
    int stride;

    DtpAsyncType type;
    void *cb_data;
    DtpNotifyFunctionT callback;
} DtpAsync;

typedef enum {
    DTP_ST_IN_PROCESS,
    DTP_ST_DONE,
    DTP_ST_WAIT_ACCEPT,
    DTP_ST_ERROR
} DtpAsyncStatus;

typedef enum {
    DTP_READ_ONLY,
    DTP_WRITE_ONLY,
    DTP_READ_WRITE
} DtpAsyncMode;


typedef struct {
    int (*send)(void *com_spec, DtpAsync *task);
    int (*recv)(void *com_spec, DtpAsync *task);
    int (*get_status)(void *com_spec, DtpAsync *task);    
    int (*destroy)(void *com_spec);
} DtpImplementation;

// typedef enum{
//     DTP_READ_ONLY,
//     DTP_WRITE_ONLY,
//     DTP_READWRITE
// } DtpMode;



//all sizes, widths, strides and offsets in words

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Универсальная функция получения дескриптора потока ввода-вывода
     * 
     * @param com_spec Указатель на пользовательскую структуру данных, который будет использоваться во всех функциях dtp
     * @param implementation Реализация интерфейса dtp
     * @return int >0 при успешном выделении дескриптора, -1 при провале
     */
    int 
    DEPRECATED
    dtpOpenCustom(void* com_spec, DtpImplementation *implementation);

    /**
     * @brief Выделяет дескриптор
     * 
     * @param mode Режим работы потока ввода-выводв (DTP_READ_ONLY, DTP_WRITE_ONLY, DTP_READ_WRITE)
     * 
     * @return int Дескриптор потока ввода-вывода
     * 
     * 
     * @details Функция выделяет пустой дескриптор для работы с потоком ввода-вывода
     */
    int dtpOpen(int mode);

    int dtpGetMode(int desc);

    /**
     * @brief Связывает дескритор с интерфейсом
     * 
     * @param com_spec 
     * @param implementation 
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     */
    int dtpBind(int desc, void* com_spec, DtpImplementation *implementation);

    //int dtpSetDestroyComSpecFunc(int desc, );

    /**
     * @brief Функция закрытия дескриптора ввода-вывода
     * 
     * @param desc 
     * @return int 
     */
    int dtpClose(int desc);

    int 
    DEPRECATED
    dtpConnect(int desc);

    int 
    DEPRECATED
    dtpListen(int desc);

    /**
     * @brief Функция асинхронного приема данных
     * 
     * @param desc Дескриптор ввода-вывода
     * @param task Структура, описывающая транзакцию
     * @return int Результат операции
     */
    int dtpAsyncRecv(int desc, DtpAsync *task);

    /**
     * @brief Функция запуска асинхронной передачи данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param task Структура с информацией о транзакции
     * @return int Результат запуска транзакции
     */
    int dtpAsyncSend(int desc, DtpAsync *task);

    /**
     * @brief Функция получение статуса транзакции
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param task Структура с информацией о транзакции
     * @return int Статус транзакции
     */
    int dtpAsyncStatus(int desc, DtpAsync *task);

    int dtpAsyncWait(int desc, DtpAsync *task);

    /**
     * @brief Функция обратного вызова по умолчанию
     * 
     * @param data Указатель на 32-разрядную переменную
     * @details Функция воспринимает data как int-ое значение и при завершении транзакции пишет в него значение DTP_ST_DONE
     */
    void dtpDefaultCallback(void *data);

    /**
     * @brief Функция одномерной синхронной передачи данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords размер передаваемых данных в 32-битных словах
     * @return int 
     */
    int dtpSend(int desc, const void *data, size_t nwords);
    
    /**
     * @brief Функция одномерной синхронного приема данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords Размер передаваемых данных в 32-битных словах
     * @return int 
     */
    int dtpRecv(int desc, void *data, size_t nwords);

    /**
     * @brief Функция двумерной синхронной передачи данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords Размер передаваемых данных в 32-битных словах
     * @param width Размер строки в 32-битных словах
     * @param stride Шаг между строками в 32-битных словах
     * @return int Результат операции
     */
	int dtpSendM(int desc, const void *data, size_t nwords, int width, int stride);

    /**
     * @brief Функция двумерного синхронного приема данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords Размер передаваемых данных в 32-битных словах
     * @param width Размер строки в 32-битных словах
     * @param stride Шаг между строками в 32-битных словах
     * @return int Результат операции
     */
    int dtpRecvM(int desc, void *data, size_t nwords, int width, int stride);


    void *dtpGetComSpec(int desc);

#ifdef __cplusplus
}
#endif

#endif //__DTP_H_INCLUDED__