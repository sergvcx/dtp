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
Дескриптор можно выделить использую собственные функции чтения-записи. Простейший способ создание собственную реализацию представлен ниже
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

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
Show your appreciation to those who have contributed to the project.

## License
For open source projects, say how it is licensed.
