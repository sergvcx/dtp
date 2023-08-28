# DTP
Descriptor transport   protocol 
## Описание
Это библиотека является абстракцией асинхронной пересылки 32-разрядных данных между устройствами

## Использование

### Работа через файл
```c++
dtpOpenFile2("input.txt", "output.txt");
int buf[20];
dtpSend(buf, 20);       // write to "output" file
dtpRecv(buf, 20);       // read from "input" file
```

### Создание собственной реализации
Дескриптор можно выделить используя собственные функции записи-чтения.

```c++
struct UserData{
    // ...
};

int user_send(void *com_spec, DtpAsync *cmd){
    UserData *data = (UserData *)com_spec;
    // запуск асинхронной пересылки
    return DTP_OK; // если всё успешно
    return DTP_ERROR; // если произошла ошибка
    return DTP_AGAIN; // если требуется попробовать попозже (например, в буфере обмена недостаточно места для записи новых данных)
}

int user_recv(void *com_spec, DtpAsync *cmd){
    UserData *data = (UserData *)com_spec;
    // запуск асинхронного приёма
    return DTP_OK; // если всё успешно
    return DTP_ERROR; // если произошла ошибка
    return DTP_AGAIN; // если требуется попробовать попозже (например, из буфера нечего читать)
}

int user_get_status(void *com_spec, DtpAsync *cmd){
    UserData *data = (UserData *)com_spec;
    // получение статуса транзакции
    return DTP_ST_IN_PROCESS; // транзакция в процессе
    return DTP_ST_DONE; // транзакция завершена
    return DTP_ST_ERROR; // при транзакции произошла ошибка
}

int user_destroy(void *com_spec, DtpAsync *cmd){
    UserData *data = (UserData *)com_spec;
    // освобождение ресурсов (например удаление пользовательской структуры)
    return 0; // если всё успешно
    return -1; // если произошла ошибка
}

int main(){
    DtpImplementation impl;
    impl.send_func = user_send;
    impl.recv_func = user_recv;
    impl.get_status_func = user_get_status;
    impl.destroy_func = user_destroy;

    UserData com_spec;    
    // инициализация данных
    int mode = DTP_READ_WRITE; // или DTP_READ_ONLY или DTP_WRITE_ONLY
    int d = dtpOpen(mode);

    dtpBind(d, &com_spec, &impl);    
    // код программы
}
```


## Список задач
 - [ ] Добавление реализации функции dtpListen, dtpConnect
 - [ ] PC
    - [x] Сокет
    - [x] File
    - [ ] Host-Target
 - [ ] NM6407
    - [x] File
    - [ ] Общая память
        - [x] Кольцевой буфер
    - [x] DMA (пакетный режим)
    - [ ] DMA (режим одиночных транзакций)
    - [ ] Сервер DMA
    - [ ] LINK (внешние байтовые коммуникационные порты)
    - [ ] SPI 
    - [ ] Host-Target 
- [ ] Тесты
    - [ ] PC
        - [ ] host-target
    - [ ] NM6407
        - [ ] Файл
        - [ ] Сокет
        - [ ] host-target
        - [ ] DMA
        - [ ] LINK
