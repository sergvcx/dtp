# DTP
Descriptor transport protocol 
## Описание
Это библиотека является абстракцией асинхронной пересылки 32-разрядных данных между устройствами

## Использование
Открытый дескриптор можно использовать для запуска чтения-записи

```c
int dtp = dtp_open(DTP_READ_WRITE);
int error;
// bind ...

int data[20];
error = dtp_recv(dtp, data, 20); // синхронное чтение
error = dtp_write(dtp, data, 20); // синхронная запись

dtp_async cmd;
// init cmd ...
error = dtp_async_read(dtp, &dtp_async);
// code ...
while(dtp_async_status(dtp, &dtp_async) == DTP_ST_IN_PROCESS);
```

## Открытие
Открытие дескриптора происходит через функцию dtp_open, а закрытие через dtp_close

```c
int dtp = dtp_open(DTP_READ_ONLY);
dtp_close(dtp);
```

Для того, чтобы можно было использовать дескриптор, к нему необходимо привязать какой-нибудь канал связи, например spi на процессоре nm6407

```c

int dtp = dtp_open(DTP_READ_ONLY);

dtp_nm6407_spi(dtp);

//...

dtp_close(dtp);
```


### Работа через файл
```c++
FILE *f = fopen("test.bin", "w");
int dtp = dtp_open(DTP_WRITE_ONLY)
dtp_open_file(dtp, f);
int buf[20];
dtp_send(buf, 20);       // write to "output" file
dtp_recv(buf, 20);       // read from "input" file
```

### Создание собственной реализации
Дескриптор можно выделить используя собственные функции записи-чтения.

```c
struct user_data{
    // ...
};

int user_send(void *com_spec, DtpAsync *cmd){
    // ...
}

int user_recv(void *com_spec, DtpAsync *cmd){
    // ...
}

int user_update_status(void *com_spec, DtpAsync *cmd){
    // ...
}

int main(){
    dtp_implementation impl;
    impl.send = user_send;
    impl.recv = user_recv;
    impl.update_status = user_update_status;    

    user_data com_spec;    
    // инициализация данных
    int mode = DTP_READ_WRITE; // или DTP_READ_ONLY или DTP_WRITE_ONLY
    int dtp = dtpOpen(mode);

    dtp_bind(dtp, &com_spec, &impl);

    int data[2];
    dtp_recv(dtp, data, 2);
    // код программы
}
```
