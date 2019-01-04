#ifndef SERIALPORTDIALOG_H
#define SERIALPORTDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "globalobject.h"
namespace Ui {
class SerialPortDialog;
}

class SerialPortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortDialog(QSerialPort *serial, QWidget *parent = nullptr);
    ~SerialPortDialog();
    static void QString2Hex(QString str,QByteArray &SendData);
    static char ConvertHexChar(char ch);
private slots:
    void on_Openserial_Button_clicked();



    void on_SendData_pushButton_clicked();

    void on_Command_comboBox_currentIndexChanged(int index);
signals:
    void serialOpened(QString name);

private:
    Ui::SerialPortDialog *ui;
    QSerialPort *serial_;
};
#endif // SERIALPORTDIALOG_H
