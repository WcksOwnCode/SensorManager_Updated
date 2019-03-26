#ifndef READQUANTITY_H
#define READQUANTITY_H

#include <QDebug>
#include <qdebug.h>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QObject>
#include "globalobject.h"
#include <QObject>
#include <QTimer>
#include <serialportdialog.h>
#include "globalobject.h"

extern QString Portname;
extern int startQuantity;
extern int port1_send_oder_flg;
extern int port2_send_oder_flg;
extern int sendnext;
extern QByteArray Ret_Data;

class ReadQuantity:public QThread
{
    Q_OBJECT
public:
    ReadQuantity();

    void run();
    QString ByteArrayToHexString(QByteArray data);
    QByteArray HexStringToByteArray(QString HexString);
    void write_Data();
    void Call_Write_Data(void);
    void Read_Data();
    void querydata();


    QSerialPort *Quantityserial;
    QTimer *t;
    DeviceInfo *Info;

public slots:
    void onTimerout();
    void on_Get_Ret(QByteArray ret);

signals:
    void Data_Thread_stop_sig();
    void Data_Thread_start_sig();

private:
    SerialPortDialog * my_serial;

};


#endif // READQUANTITY_H
