#include "serialportdialog.h"
#include "ui_serialportdialog.h"
#include <QMessageBox>
SerialPortDialog::SerialPortDialog(QSerialPort *serial, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortDialog)
{
    ui->setupUi(this);
    serial_=serial;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial2;
        serial2.setPort(info);
        if(serial2.open(QIODevice::ReadWrite))
        {
            ui->PortName->addItem(serial2.portName());
            serial2.close();
        }

    }
    ui->SendData_pushButton->setEnabled(false);

}

SerialPortDialog::~SerialPortDialog()
{
    delete ui;
}

void SerialPortDialog::on_Openserial_Button_clicked()
{
    if(ui->Openserial_Button->text()==tr("打开串口"))
    {
        serial_ = new  QSerialPort;
        //设置串口名
        serial_->setPortName(ui->PortName->currentText());
        //打开串口
        serial_->open(QIODevice::ReadWrite);
        //设置波特率
        serial_->setBaudRate(ui->BaudRate->currentText().toInt());
        //设置数据位数
        switch(ui->DataByte->currentIndex())
        {
        case 8: serial_->setDataBits( QSerialPort::Data8); break;
        default: break;
        }
        //设置奇偶校验
        switch(ui->ParityCheck->currentIndex())
        {
        case 0: serial_->setParity( QSerialPort::NoParity); break;
        default: break;
        }
        //设置停止位
        switch(ui->StopBit->currentIndex())
        {
        case 1: serial_->setStopBits( QSerialPort::OneStop); break;
        case 2: serial_->setStopBits( QSerialPort::TwoStop); break;
        default: break;
        }
        //设置流控制
        serial_->setFlowControl( QSerialPort::NoFlowControl);

        if(ui->RTS_checkBox->isChecked()){
            serial_->setRequestToSend(true);
        }
        //关闭设置菜单使能
        ui->PortName->setEnabled(false);
        ui->BaudRate->setEnabled(false);
        ui->DataByte->setEnabled(false);
        ui->StopBit->setEnabled(false);
        ui->ParityCheck->setEnabled(false);
        ui->Openserial_Button->setText(tr("关闭串口"));
        ui->SendData_pushButton->setEnabled(true);
        //连接信号槽
        QObject::connect(serial_, &QSerialPort::readyRead, this,[=]{
            QByteArray ReData=serial_->readAll();
            QString messa=ReData;
            ui->RplayDisplaylabel->setText(messa);
        });
        emit serialOpened(ui->PortName->currentText());

    }
    else
    {
        //关闭串口
        serial_->clear();
        serial_->close();
        serial_->deleteLater();

        //恢复设置使能
        ui->PortName->setEnabled(true);
        ui->BaudRate->setEnabled(true);
        ui->DataByte->setEnabled(true);
        ui->ParityCheck->setEnabled(true);
        ui->StopBit->setEnabled(true);
        ui->Openserial_Button->setText(tr("打开串口"));
        ui->SendData_pushButton->setEnabled(false);

    }


}
void SerialPortDialog::on_SendData_pushButton_clicked(){
    if(!serial_->isOpen()){
        return;
    }
    if(ui->HexData_radioButton->isChecked()){
        QString Data=ui->Input_textEdit->toPlainText();
        QByteArray Hexdata;
        QString2Hex(Data,Hexdata);
        serial_->write(Hexdata);
    }
    else if(ui->BinaryData_radioButton->isChecked()){
        QString Data=ui->Input_textEdit->toPlainText();
        serial_->write(Data.toLatin1());
    }
    else{
        QMessageBox::information(this,"Notice","Choose a coding method");
        return;
    }




}

void  SerialPortDialog::QString2Hex(QString str,QByteArray &SendData){

    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    SendData.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        SendData[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    SendData.resize(hexdatalen);
}
char SerialPortDialog::ConvertHexChar(char ch){
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else
        return (-1);

}

void SerialPortDialog::on_Command_comboBox_currentIndexChanged(int index)
{
    if(index==0)
    {
        return;
    }
    ui->Input_textEdit->clear();
    ui->Input_textEdit->setText(GlobalObject::ControlCommand[index-1]);
    ui->HexData_radioButton->setChecked(true);
    ui->BinaryData_radioButton->setChecked(false);

}
