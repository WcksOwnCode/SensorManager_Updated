#pragma execution_character_set("utf-8")
#include "serialportdialog.h"
#include "ui_serialportdialog.h"
#include <QMessageBox>
SerialPortDialog::SerialPortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortDialog)
{
    ui->setupUi(this);

    ui->SendData_pushButton->setEnabled(false);
    this->setWindowTitle("串口操作窗口");

}

SerialPortDialog::~SerialPortDialog()
{
    delete ui;
}
void SerialPortDialog::ReadPort_1(){//for port1 read
    readcom(1);
}

void SerialPortDialog::ReadPOrt_2(){//for port2 read
    readcom(2);
}

void SerialPortDialog::readcom(int port) //读串口函数
{
    //待测试函数2019-2-26

    QByteArray buff_1,buff_2;
    if(port==1){
        if(GlobalObject::serial!=nullptr){
            if(!GlobalObject::serial->isOpen()){
                QMessageBox::warning(this,"warnig","serial port not open");
                return;
            }
            //  GlobalObject::serial->waitForReadyRead(10);
            buff_1=GlobalObject::serial->readAll();
            if(!buff_1.isEmpty())
            {
                emit Get_ret(buff_1);
                qDebug() << "read from port 1  value is " << buff_1;
            }
            else{
                return;
            }
            QString str=ByteArrayToHexString(buff_1);
            if(str=="FE"){
                m_bReadStart_1=true;
                ReadStr_1.clear();
                if(m_bReadStart_1==false){
                    m_bReceive_done_1=false;
                    QTimer::singleShot(10,this,SLOT(ReadDone_port_1));
                }
                else{
                    //ignore
                    return;
                }

            }
            else if(str.length()>6&&m_bReadStart_1==false&&str.contains("FE")){
                ReadStr_1=str;
                ReadDone_port_1();
            }
            else if(str.length()>6&&m_bReadStart_1!=false){
                //ignore

            }
            if(m_bReadStart_1){

                ReadStr_1+=str;
            }

            //并不知道读取结果是什么格式，先这么写吧


        }
    }
    if(port==2){
        if(GlobalObject::serial_2!=nullptr){
            if(!GlobalObject::serial_2->isOpen()){
                QMessageBox::warning(this,"warnig","serial port not open");
                return;
            }
            buff_2=GlobalObject::serial->readAll();
            if(!buff_2.isEmpty())
            {
                emit Get_ret(buff_2);
                qDebug() << "read from port 2 value is " << buff_2;
            }
            else{
                return;
            }
            QString str=ByteArrayToHexString(buff_2);
            if(str=="FE"){
                m_bReadStart_2=true;
                ReadStr_2.clear();
                if(m_bReadStart_2==false){
                    m_bReceive_done_2=false;
                    QTimer::singleShot(10,this,SLOT(ReadDone_port_2));
                }
                else{
                    //ignore
                    return;
                }


            }
            else if(str.length()>6&&m_bReadStart_2==false&&str.contains("FE")){
                ReadStr_2=str;
                ReadDone_port_2();
            }
            else if(str.length()>6&&m_bReadStart_2!=false){
                //ignore

            }
            if(m_bReadStart_2){

                ReadStr_2+=str;
            }

        }
    }
}
void SerialPortDialog::ReadDone_port_1(){
    m_bReadStart_1=false;
    m_bReceive_done_1=true;
    qDebug()<<"Port 1 read done and value is "<<ReadStr_1;
    CheckReply(1,ReadStr_1);

}
void SerialPortDialog::ReadDone_port_2(){
  m_bReadStart_2=false;
    m_bReceive_done_2=true;
    qDebug()<<"Port 2 read done and value is "<<ReadStr_2;
    CheckReply(1,ReadStr_2);
}
void SerialPortDialog::CheckReply(int port,QString code){
    //其实此处有个问题，返回一个数据码，但是接收端并不会知道返回数据是对应于哪个设备的，只好一个一个查找
    //待测试函数2019-2-26
    qDebug()<<"check respond and message is "<<code;
    GlobalObject::locker->lock();
    int ind=-1;
    for(int q=0;q<GlobalObject::ControlCommand_rep.length();q++){
        if(GlobalObject::ControlCommand_rep.at(q)==code){
            qDebug()<<"code match to Control Command at "<<q;
            ind=q;
            break;
        }
    }
    if(ind==-1){
        qDebug()<<"haven't found the replay message in replay list----->return";
        return;
    }
    bool find=false;
    qDebug()<<"to find out which device send the corresponding command!";
    for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
        for(int j=0;j<GlobalObject::g_ConnectDevList->at(i)->CommandSendList.length();j++){
            if(int(GlobalObject::g_ConnectDevList->at(i)->CommandSendList.at(j).x())!=port){
                qDebug()<<"find the same command but port check failed!----->continue";
                continue;
            }
            if(int(GlobalObject::g_ConnectDevList->at(i)->CommandSendList.at(j).w())==ind){
                //移除这个命令
                qDebug()<<"find the corresponding command-------->remove it";
                if(j>0&&j<((*GlobalObject::g_ConnectDevList)[i])->ControlMethodList.length()){
                    ((*GlobalObject::g_ConnectDevList)[i])->ControlMethodList.removeAt(j);
                    ((*GlobalObject::g_ConnectDevList)[i])->CommandTimeStamp.removeAt(j);
                    if(((*GlobalObject::g_ConnectDevList)[i])->ControlMethodList.length()==0){
                       ((*GlobalObject::g_ConnectDevList)[i])->m_bMethodexecRunable=false;//close condition check
                    }

                }
                else {
                    qDebug()<<"invalid index j "<<j<<" length of list is "<<((*GlobalObject::g_ConnectDevList)[i])->ControlMethodList.length();
                }
                find=true;
                break;
            }

        }
        if(find){
            break;
        }
    }
    GlobalObject::locker->unlock();
}
void SerialPortDialog::on_Openserial_Button_clicked()
{
    if(ui->Openserial_Button->text()==tr("打开串口")&&ui->SerialNO_comboBox->currentIndex()==1)
    {
        if(GlobalObject::serial!=nullptr){
            GlobalObject::serial->close();
            delete  GlobalObject::serial;
            GlobalObject::serial=nullptr;
        }
        GlobalObject::serial = new  QSerialPort;
        //设置串口名
        GlobalObject::serial->setPortName(ui->PortName->currentText());
        //打开串口
        GlobalObject::serial->open(QIODevice::ReadWrite);
        //设置波特率
        GlobalObject::serial->setBaudRate(ui->BaudRate->currentText().toInt());
        //设置数据位数
        switch(ui->DataByte->currentIndex())
        {
        case 8: GlobalObject::serial->setDataBits( QSerialPort::Data8); break;
        default: break;
        }
        //设置奇偶校验
        switch(ui->ParityCheck->currentIndex())
        {
        case 0: GlobalObject::serial->setParity( QSerialPort::NoParity); break;
        default: break;
        }
        //设置停止位
        switch(ui->StopBit->currentIndex())
        {
        case 1: GlobalObject::serial->setStopBits( QSerialPort::OneStop); break;
        case 2: GlobalObject::serial->setStopBits( QSerialPort::TwoStop); break;
        default: break;
        }
        //设置流控制
        GlobalObject::serial->setFlowControl( QSerialPort::NoFlowControl);

        if(ui->RTS_checkBox->isChecked()){
            GlobalObject::serial->setRequestToSend(true);
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
        QObject::connect(GlobalObject::serial, &QSerialPort::readyRead, this,&SerialPortDialog::ReadPort_1);

        SerialName1=ui->PortName->currentText();
        emit serialOpened(SerialName1,SerialName2);

    }
    else if(ui->Openserial_Button->text()==tr("打开串口")&&ui->SerialNO_comboBox->currentIndex()==2)
    {

        if(GlobalObject::serial_2!=nullptr){
            GlobalObject::serial_2->close();
            delete  GlobalObject::serial_2;
            GlobalObject::serial_2=nullptr;
        }
        GlobalObject::serial_2 = new  QSerialPort;
        //设置串口名
        GlobalObject::serial_2->setPortName(ui->PortName->currentText());
        //打开串口
        GlobalObject::serial_2->open(QIODevice::ReadWrite);
        //设置波特率
        GlobalObject::serial_2->setBaudRate(ui->BaudRate->currentText().toInt());
        //设置数据位数
        switch(ui->DataByte->currentIndex())
        {
        case 8: GlobalObject::serial_2->setDataBits( QSerialPort::Data8); break;
        default: break;
        }
        //设置奇偶校验
        switch(ui->ParityCheck->currentIndex())
        {
        case 0: GlobalObject::serial_2->setParity( QSerialPort::NoParity); break;
        default: break;
        }
        //设置停止位
        switch(ui->StopBit->currentIndex())
        {
        case 1: GlobalObject::serial_2->setStopBits( QSerialPort::OneStop); break;
        case 2: GlobalObject::serial_2->setStopBits( QSerialPort::TwoStop); break;
        default: break;
        }
        //设置流控制
        GlobalObject::serial_2->setFlowControl( QSerialPort::NoFlowControl);

        if(ui->RTS_checkBox->isChecked()){
            GlobalObject::serial_2->setRequestToSend(true);
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
        QObject::connect(GlobalObject::serial_2, &QSerialPort::readyRead, this,&SerialPortDialog::ReadPOrt_2);


        SerialName2=ui->PortName->currentText();
        emit serialOpened(SerialName1,SerialName2);


    }
    else if(ui->Openserial_Button->text()==tr("关闭串口"))
    {
        //关闭串口
        if(ui->SerialNO_comboBox->currentIndex()==1&&GlobalObject::serial!=nullptr){
            if(GlobalObject::serial->isOpen()){
                //GlobalObject::serial->clear();
                GlobalObject::serial->close();

                ui->PortName->clear();
                ui->BaudRate->clear();
                ui->DataByte->clear();
                ui->ParityCheck->clear();
                ui->ParityCheck->setEnabled(false);
                ui->StopBit->clear();
                ui->SendData_pushButton->setEnabled(false);
                if(GlobalObject::serial->isOpen()){
                    QMessageBox::warning(this,"warning","close failed");
                }
                //delete GlobalObject::serial;
                GlobalObject::serial=nullptr;
            }

        }
        else if(ui->SerialNO_comboBox->currentIndex()==2&&GlobalObject::serial_2!=nullptr){
            if(GlobalObject::serial_2->isOpen()){
                //GlobalObject::serial_2->clear();
                GlobalObject::serial_2->close();
                GlobalObject::serial_2->deleteLater();
                ui->PortName->clear();
                ui->BaudRate->clear();
                ui->DataByte->clear();
                ui->ParityCheck->clear();
                ui->ParityCheck->setEnabled(false);
                ui->StopBit->clear();
                ui->SendData_pushButton->setEnabled(false);
                // delete GlobalObject::serial_2;
                GlobalObject::serial_2=nullptr;
            }

        }

        ui->Openserial_Button->setText(tr("打开串口"));
        //恢复设置使能
        /*ui->PortName->setEnabled(true);
        ui->BaudRate->setEnabled(true);
        ui->DataByte->setEnabled(true);
        ui->ParityCheck->setEnabled(true);
        ui->StopBit->setEnabled(true);
        ui->Openserial_Button->setText(tr("打开串口"));*/


    }
    /* else if(ui->Openserial_Button->text()==tr("扫描串口")){
        foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            QSerialPort serial2;
            serial2.setPort(info);
            if(serial2.open(QIODevice::ReadWrite))
            {
                //ui->PortName->addItem(serial2.portName());
                PortNum.push_back(serial2.portName());
                serial2.close();
            }

        }
        if(ui->SerialNO_comboBox->currentIndex()>0){
            for(int q=0;q<PortNum.length();q++){
                ui->PortName->addItem(PortNum[q]);
            }
            ui->Openserial_Button->setText("打开串口");
        }
        if(PortNum.length()==0){
            QMessageBox::warning(this,"warning","No available Serial port!");
        }


        ui->SendData_pushButton->setEnabled(false);
    }*/

}
void SerialPortDialog::scanPort(){
    qDebug()<<"scan port";
    PortNum.clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial2;
        serial2.setPort(info);
        if(serial2.open(QIODevice::ReadWrite))
        {
            //ui->PortName->addItem(serial2.portName());
            PortNum.push_back(serial2.portName());
            serial2.close();
        }

    }
}
void SerialPortDialog::on_SendData_pushButton_clicked(){
    if(ui->SerialNO_comboBox->currentIndex()==0){
        return;
    }

    if(ui->SerialNO_comboBox->currentIndex()==1&&GlobalObject::serial!=nullptr){
        if(!GlobalObject::serial->isOpen()){
            QMessageBox::warning(this,"warnig","serial port not open");
            return;
        }

        if(ui->HexData_radioButton->isChecked()){
            QString Data=ui->Input_textEdit->toPlainText();
            QByteArray Hexdata;
            QString2Hex(Data,Hexdata);
            qDebug()<<"serial 1 send "<<Hexdata;
            GlobalObject::serial->write(Hexdata);
        }
        else if(ui->BinaryData_radioButton->isChecked()){
            QString Data=ui->Input_textEdit->toPlainText();
            GlobalObject::serial->write(Data.toLatin1());
        }
        else{
            QMessageBox::information(this,"Notice","Choose a coding method");
            return;
        }


    }
    if(ui->SerialNO_comboBox->currentIndex()==2&&GlobalObject::serial_2!=nullptr){
        if(!GlobalObject::serial_2->isOpen()){
            QMessageBox::warning(this,"warnig","serial port not open");
            return;
        }

        if(ui->HexData_radioButton->isChecked()){
            QString Data=ui->Input_textEdit->toPlainText();
            QByteArray Hexdata;
            QString2Hex(Data,Hexdata);
            qDebug()<<"serial 2 send "<<Hexdata;
            GlobalObject::serial_2->write(Hexdata);
        }
        else if(ui->BinaryData_radioButton->isChecked()){
            QString Data=ui->Input_textEdit->toPlainText();
            GlobalObject::serial_2->write(Data.toLatin1());
        }
        else{
            QMessageBox::information(this,"Notice","Choose a coding method");
            return;
        }


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

void SerialPortDialog::on_SerialNO_comboBox_currentIndexChanged(int index)
{
    if(ui->SerialNO_comboBox->currentIndex()>0){
        //加载串口参数
        if(index==1){
            if(GlobalObject::serial==nullptr){
                scanPort();//scan available serial port
                for(int i=0;i<PortNum.length();i++){
                    ui->PortName->addItem(PortNum[i]);
                }
                if(PortNum.length()==0){
                    ui->PortName->addItem("NULL");
                }
                ui->Openserial_Button->setText("打开串口");
                ui->PortName->setEditable(true);
                ui->BaudRate->setEnabled(true);
                ui->DataByte->setEnabled(true);
                ui->ParityCheck->setEnabled(true);
                ui->StopBit->setEnabled(true);
                ui->RTS_checkBox->setEnabled(true);
            }
            else{
                if(GlobalObject::serial->isOpen()){

                    ui->PortName->clear();
                    ui->PortName->addItem(GlobalObject::serial->portName());
                    //设置波特率
                    ui->BaudRate->clear();
                    ui->BaudRate->addItem(QString::number(GlobalObject::serial->baudRate()));
                    ui->DataByte->clear();
                    ui->DataByte->addItem(QString::number(GlobalObject::serial->dataBits()));

                    ui->ParityCheck->clear();
                    ui->ParityCheck->addItem(QString::number(GlobalObject::serial->parity()));

                    ui->StopBit->clear();
                    ui->StopBit->addItem(QString::number(GlobalObject::serial->stopBits()));

                    if(GlobalObject::serial->isRequestToSend()){
                        ui->RTS_checkBox->setChecked(true);
                    }
                    else{
                        ui->RTS_checkBox->setChecked(false);
                    }
                    ui->Openserial_Button->setText("关闭串口");
                    ui->SendData_pushButton->setEnabled(true);


                }
                else{
                    GlobalObject::serial=nullptr;
                    scanPort();//scan available serial port
                    for(int i=0;i<PortNum.length();i++){
                        ui->PortName->addItem(PortNum[i]);
                    }
                    if(PortNum.length()==0){
                        ui->PortName->addItem("NULL");
                    }
                    ui->Openserial_Button->setText("打开串口");
                    ui->PortName->setEditable(true);
                    ui->BaudRate->setEnabled(true);
                    ui->DataByte->setEnabled(true);
                    ui->ParityCheck->setEnabled(true);
                    ui->StopBit->setEnabled(true);
                    ui->RTS_checkBox->setEnabled(true);
                }

            }


        }
        else if(index==2){
            if(GlobalObject::serial_2==nullptr){
                scanPort();//scan available serial port
                for(int i=0;i<PortNum.length();i++){
                    ui->PortName->addItem(PortNum[i]);
                }
                if(PortNum.length()==0){
                    ui->PortName->addItem("None port");
                }
                ui->Openserial_Button->setText("打开串口");
                ui->PortName->setEditable(true);
                ui->BaudRate->setEnabled(true);
                ui->DataByte->setEnabled(true);
                ui->ParityCheck->setEnabled(true);
                ui->StopBit->setEnabled(true);
                ui->RTS_checkBox->setEnabled(true);
            }
            else{
                if(GlobalObject::serial_2->isOpen()){

                    ui->PortName->clear();
                    ui->PortName->addItem(GlobalObject::serial_2->portName());
                    //设置波特率
                    ui->BaudRate->clear();
                    ui->BaudRate->addItem(QString::number(GlobalObject::serial_2->baudRate()));
                    ui->DataByte->clear();
                    ui->DataByte->addItem(QString::number(GlobalObject::serial_2->dataBits()));

                    ui->ParityCheck->clear();
                    ui->ParityCheck->addItem(QString::number(GlobalObject::serial_2->parity()));

                    ui->StopBit->clear();
                    ui->StopBit->addItem(QString::number(GlobalObject::serial_2->stopBits()));

                    if(GlobalObject::serial->isRequestToSend()){
                        ui->RTS_checkBox->setChecked(true);
                    }
                    else{
                        ui->RTS_checkBox->setChecked(false);
                    }
                    ui->Openserial_Button->setText("关闭串口");
                    ui->SendData_pushButton->setEnabled(true);


                }
                else{
                    GlobalObject::serial_2=nullptr;
                    scanPort();//scan available serial port
                    for(int i=0;i<PortNum.length();i++){
                        ui->PortName->addItem(PortNum[i]);
                    }
                    if(PortNum.length()==0){
                        ui->PortName->addItem("None port");
                    }
                    ui->Openserial_Button->setText("打开串口");
                    ui->PortName->setEditable(true);
                    ui->BaudRate->setEnabled(true);
                    ui->DataByte->setEnabled(true);
                    ui->ParityCheck->setEnabled(true);
                    ui->StopBit->setEnabled(true);
                    ui->RTS_checkBox->setEnabled(true);
                }

            }
        }
    }


    /*

        if(GlobalObject::serial!=nullptr&&index==1){//port 1 is used

            //设置串口名
            if(GlobalObject::serial->isOpen()){

                ui->PortName->clear();
                ui->PortName->addItem(GlobalObject::serial->portName());
                //设置波特率
                ui->BaudRate->clear();
                ui->BaudRate->addItem(QString::number(GlobalObject::serial->baudRate()));
                ui->DataByte->clear();
                ui->DataByte->addItem(QString::number(GlobalObject::serial->dataBits()));

                ui->ParityCheck->clear();
                ui->ParityCheck->addItem(QString::number(GlobalObject::serial->parity()));

                ui->StopBit->clear();
                ui->StopBit->addItem(QString::number(GlobalObject::serial->stopBits()));

                if(GlobalObject::serial->isRequestToSend()){
                    ui->RTS_checkBox->setChecked(true);
                }
                else{
                    ui->RTS_checkBox->setChecked(false);
                }
                ui->Openserial_Button->setText("关闭串口");
                ui->SendData_pushButton->setEnabled(true);
            }


        }
        else if(GlobalObject::serial==nullptr&&index==1)
        {
            ui->PortName->clear();
            for(int i=0;i<PortNum.length();i++){
                ui->PortName->addItem(PortNum[i]);
            }
            if(PortNum.length()==0){
                ui->Openserial_Button->setText("扫描串口");
            }
            else{
                ui->Openserial_Button->setText("打开串口");
            }

            //设置波特率
            ui->BaudRate->clear();
            ui->BaudRate->addItem("9600");
            ui->DataByte->clear();
            ui->DataByte->addItem("8");

            ui->ParityCheck->clear();
            ui->ParityCheck->addItem("0");

            ui->StopBit->clear();
            ui->StopBit->addItem("1");
            ui->RTS_checkBox->setChecked(false);
        }

        if(GlobalObject::serial_2!=nullptr&&index==2){
            if(GlobalObject::serial_2->isOpen()){
                //设置串口名
                ui->PortName->clear();
                ui->PortName->addItem(GlobalObject::serial_2->portName());

                //设置波特率
                ui->BaudRate->clear();
                ui->BaudRate->addItem(QString::number(GlobalObject::serial_2->baudRate()));
                ui->DataByte->clear();
                ui->DataByte->addItem(QString::number(GlobalObject::serial_2->dataBits()));

                ui->ParityCheck->clear();
                ui->ParityCheck->addItem(QString::number(GlobalObject::serial_2->parity()));

                ui->StopBit->clear();
                ui->StopBit->addItem(QString::number(GlobalObject::serial_2->stopBits()));

                if(GlobalObject::serial_2->isRequestToSend()){
                    ui->RTS_checkBox->setChecked(true);
                }
                else{
                    ui->RTS_checkBox->setChecked(false);
                }
                ui->Openserial_Button->setText("关闭串口");
                ui->SendData_pushButton->setEnabled(true);


            }

        }
        else{
            ui->PortName->clear();
            for(int i=0;i<PortNum.length();i++){
                ui->PortName->addItem(PortNum[i]);
            }
            if(PortNum.length()==0){
                ui->Openserial_Button->setText("扫描串口");
            }
            else{
                ui->Openserial_Button->setText("打开串口");
            }
            //设置波特率
            ui->BaudRate->clear();
            ui->BaudRate->addItem("9600");
            ui->DataByte->clear();
            ui->DataByte->addItem("8");

            ui->ParityCheck->clear();
            ui->ParityCheck->addItem("0");

            ui->StopBit->clear();
            ui->StopBit->addItem("1");
            ui->RTS_checkBox->setChecked(false);
        }
    }
    else{
        ui->PortName->clear();
        //设置波特率
        ui->BaudRate->clear();
        ui->DataByte->clear();
        ui->ParityCheck->clear();
        ui->StopBit->clear();
        ui->RTS_checkBox->setChecked(false);
        ui->Openserial_Button->setText("扫描串口");
        ui->SendData_pushButton->setEnabled(false);
    }*/

}
QString SerialPortDialog::ByteArrayToHexString(QByteArray data)
{
    QString ret(data.toHex().toUpper());
    int len = ret.length()/2;
    for(int i=1;i<len;i++)
    {
        ret.insert(2*i+i-1," ");
    }

    return ret;

}
void SerialPortDialog::closeEvent(QCloseEvent *sevent){
    emit close_event();
    sevent->ignore();
}
