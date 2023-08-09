# DTP
Descriptor transport protocol

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
Дескриптор можно выделить используя собственные функции чтения-записи.
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
    // получение статус транзакции
    return DTP_ST_IN_PROCESS; // транзакция в процессе
    return DTP_ST_DONE; // транзакция завершена
    return DTP_ST_ERROR; // при транзакции произошла ошибка
}

int user_destroy(void *com_spec, DtpAsync *cmd){
    UserData *data = (UserData *)com_spec;
    // освобождение ресурсов (например удаление пользовательской структуры)
    return 0; // если всё успешно
    return 1; // если произошла ошибка
}

int main(){
    DtpImplementation impl;
    UserData com_spec;
    // инициализация данных
    int d = dtpOpenCustom(&com_spec, &impl)
    // код программы
}
```


## Список задач
 - [ ] MC12101
    - [x] Добавление поддержки обмена через файл
    - [ ] Добавление поддержки буффера на NeuroMatrix
    - [ ] Добавление поддержки БЗИО
    - [ ] Добавление поддержки DMA
    - [ ] Добавление поддержки внешних байтовых коммуникационных портов

