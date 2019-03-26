#ifndef SERIALPORTDIALOG_H
#define SERIALPORTDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "globalobject.h"
#include <QCloseEvent>
#include <QTimer>
namespace Ui {
class SerialPortDialog;
}

class SerialPortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortDialog(QWidget *parent = nullptr);
    ~SerialPortDialog();
    static void QString2Hex(QString str,QByteArray &SendData);
    static char ConvertHexChar(char ch);
    void readcom(int port);
    void CheckReply(int port, QString code);
    QString ByteArrayToHexString(QByteArray data);
    void scanPort();
    void ReadPort_1();//for port1 read
    void ReadPOrt_2();//for port2 read
    void ReadDone_port_1();
    void ReadDone_port_2();
private slots:
    void on_Openserial_Button_clicked();



    void on_SendData_pushButton_clicked();

    void on_Command_comboBox_currentIndexChanged(int index);
    void on_SerialNO_comboBox_currentIndexChanged(int index);
protected:
     void closeEvent(QCloseEvent *sevent);
signals:
    void serialOpened(QString name1,QString name2);
    void Get_ret(QByteArray);
    void Get_ret_2(QByteArray);
    void close_event();

private:
    Ui::SerialPortDialog *ui;
    QSerialPort *serial_;
    QVector<QString>PortNum;
    QString SerialName1="",SerialName2="";
    QString ReadStr_1,ReadStr_2;
    bool m_bReadStart_1=false;
    bool m_bReadStart_2=false;
    bool m_bReceive_done_1=false;
    bool m_bReceive_done_2=false;

};
#endif // SERIALPORTDIALOG_H
