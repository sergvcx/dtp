#ifndef __DTP_H_INCLUDED__
#define __DTP_H_INCLUDED__

#include <stdlib.h>

typedef void (*dtp_notify_func_t)(void *event_data);

#define DTP_OPEN_MAX 16

#ifdef __GNUC__
#define DEPRECATED __attribute__ ((deprecated))
#else 
#define DEPRECATED 
#endif


typedef enum{
    DTP_OK = 0,
    DTP_ERROR = -1,
    DTP_AGAIN = -2
} dtp_error;

typedef enum {
    DTP_TASK_NONE,
    DTP_TASK_1D,
    DTP_TASK_2D
} dtp_async_type;

/**
 * @struct dtp_async
 * @brief This structure describe send/receive operation
 * @var buf Адрес буфера
 * 
 */
typedef struct {
    volatile void *buf;
    size_t nwords;
    int width;
    int stride;

    dtp_async_type type;
    void *cb_data;
    dtp_notify_func_t callback;

    // depended on implementation    
    struct {
        volatile int status;
    } DTP_ASYNC_PRIVATE_FIELDS; 
    
} dtp_async;

typedef enum {
    DTP_ST_IN_PROCESS,
    DTP_ST_DONE,
    DTP_ST_WAIT_ACCEPT,
    DTP_ST_ERROR
} dtp_async_status;

typedef enum {
    DTP_READ_ONLY = 0x1,
    DTP_WRITE_ONLY = 0x2,
    DTP_READ_WRITE = DTP_READ_ONLY | DTP_WRITE_ONLY
} dtp_async_mode;


typedef struct {
    int (*send)(void *com_spec, dtp_async *task);
    int (*recv)(void *com_spec, dtp_async *task);
    int (*update_status)(void *com_spec, dtp_async *task);    
} dtp_implementation;



//all sizes, widths, strides and offsets in words

#ifdef __cplusplus
extern "C" {
#endif

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
    int dtp_open(int mode);

    /**
     * @brief Получение режима работы дескриптора
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @return int Возвращает одно из значений DtpAsyncMode
     */
    int dtp_get_mode(int dtp);

    /**
     * @brief Связывает дескритор с интерфейсом
     * 
     * @param com_spec 
     * @param implementation 
     * @return int Возвращает DTP_OK при успехе и DTP_ERROR при возникновении ошибки
     */
    int dtp_bind(int dtp, void* com_spec, dtp_implementation *implementation);

    /**
     * @brief Функция получения указателя на спецификацию привязанного канал связи.
     * 
     * @param desc 
     * @return void* Указатель на данные канала связи. 0, если канал не привязан
     */
    void *dtp_get_com_spec(int dtp);

    /**
     * @brief Функция закрытия дескриптора ввода-вывода
     * 
     * @param desc 
     * @return int 
     */
    int dtp_close(int dtp);

    /**
     * @brief Функция асинхронного приема данных
     * 
     * @param desc Дескриптор ввода-вывода
     * @param task Структура, описывающая транзакцию
     * @return int Результат операции. Одно из значений dtp_error
     */
    int dtp_async_recv(int dtp, dtp_async *task);

    /**
     * @brief Функция запуска асинхронной передачи данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param task Структура с информацией о транзакции
     * @return int Результат операции. Одно из значений dtp_error
     */
    int dtp_async_send(int dtp, dtp_async *task);

    /**
     * @brief Функция получение статуса транзакции
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param task Структура с информацией о транзакции
     * @return int Статус транзакции. Одно из значений DtpAsyncStatus
     */
    int dtp_async_status(int dtp, dtp_async *task);

    int dtp_async_wait(int dtp, dtp_async *task);    

    /**
     * @brief Функция одномерной синхронной передачи данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords размер передаваемых данных в 32-битных словах
     * @return int Результат операции. Одно из значений dtp_error
     */
    int dtp_send(int dtp, const void *data, size_t nwords);
    
    /**
     * @brief Функция одномерной синхронного приема данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords Размер передаваемых данных в 32-битных словах
     * @return int Результат операции. Одно из значений dtp_error
     */
    int dtp_recv(int dtp, void *data, size_t nwords);

    /**
     * @brief Функция двумерной синхронной передачи данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords Размер передаваемых данных в 32-битных словах
     * @param width Размер строки в 32-битных словах
     * @param stride Шаг между строками в 32-битных словах
     * @return int Результат операции. Одно из значений dtp_error
     */
	int dtp_send_matr(int dtp, const void *data, size_t nwords, int width, int stride);

    /**
     * @brief Функция двумерного синхронного приема данных
     * 
     * @param desc Дескриптор потока ввода-вывода
     * @param data Указатель на буфер данных
     * @param nwords Размер передаваемых данных в 32-битных словах
     * @param width Размер строки в 32-битных словах
     * @param stride Шаг между строками в 32-битных словах
     * @return int Результат операции. Одно из значений dtp_error
     */
    int dtp_recv_matr(int dtp, void *data, size_t nwords, int width, int stride);

    
#ifdef __cplusplus
}
#endif

#endif //__DTP_H_INCLUDED__