#pragma execution_character_set("utf-8")
#include "readquantity.h"
QString Portname;             //获取环境监控数据端口号
int startQuantity = 0;        //1 环境监控端口打开，开始监听串口  2 获取到开关量信息 3 环境监控端口由开启到关闭
int port1_send_oder_flg = 0;  //数据下发至机器人的串口打开，该标志位置1
int port2_send_oder_flg = 0;  //数据下发至机器人的串口打开，该标志位置1
int sendnext = 0;
QByteArray Ret_Data;

static int flag = 0;
static int flag_2 = 0;

#include <QObject>

ReadQuantity::ReadQuantity()
{

}


/********************************************************************************************************************************
*   function:       ByteArrayToHexString()
*   Description:    ByteArray转16进制
*   Input:
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/
QString ReadQuantity::ByteArrayToHexString(QByteArray data)
{
    QString ret(data.toHex().toUpper());
    int len = ret.length()/2;
    for(int i=1;i<len;i++)
    {
        ret.insert(2*i+i-1," ");
    }

    return ret;

}

/********************************************************************************************************************************
*   function:       ByteArrayToHexString()
*   Description:    16进制转ByteArray
*   Input:
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/
QByteArray ReadQuantity::HexStringToByteArray(QString HexString)
{
    bool ok;
    QByteArray ret;
    HexString = HexString.trimmed();
    HexString = HexString.simplified();
    QStringList sl = HexString.split(" ");

    foreach (QString s, sl) {
        if(!s.isEmpty())
        {
            char c = s.toInt(&ok,16)&0xFF;
            if(ok){
                ret.append(c);
            }else{
                qDebug()<<"errrorrrrrrreeee";
            }
        }
    }
    qDebug()<<ret;
    return ret;
}

/********************************************************************************************************************************
*   function:       run()
*   Description:    线程函数
*   Input:          clicked
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/
void ReadQuantity::run()
{
    Quantityserial = new QSerialPort;

    //环境监控串口配置
    Quantityserial->setPortName(Portname);
    Quantityserial->open(QIODevice::ReadWrite);
    Quantityserial->setBaudRate(9600);
    Quantityserial->setDataBits(QSerialPort::Data8);
    Quantityserial->setParity(QSerialPort::NoParity);
    Quantityserial->setStopBits(QSerialPort::OneStop);
    Quantityserial->setFlowControl(QSerialPort::NoFlowControl);
    qDebug()<<"port open";

    t = new QTimer(this);
    connect(t,SIGNAL(timeout()),this,SLOT(onTimerout()));
    t->start(5000);
}

/********************************************************************************************************************************
*   function:       write_Data()
*   Description:    串口发送数据
*   Input:
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/
void ReadQuantity::write_Data()
{
    QByteArray buf;
    QByteArray ret;
    QString str;
    QString tmp = "FE050002FF0039F5";
    QString tet = "FE 05 00 02 FF 00 39 F5";    //机器人停止指令


    buf = HexStringToByteArray(tet);    //将QString转成BYtearry发送
    if(port1_send_oder_flg == 1)        //判断哪个机器人控制串口打开
    {
        if(GlobalObject::serial->isOpen())
        {
            GlobalObject::serial->write(buf); //通过串口发送机器人控制指令
            while(GlobalObject::serial->waitForBytesWritten(10))    //等待串口发送完成
            {

            }
        }
        else{
            qDebug()<<"port not open";
        }
    }
    if(port2_send_oder_flg == 1)
    {
        if(GlobalObject::serial_2->isOpen())
        {
            GlobalObject::serial_2->write(buf);
            while(GlobalObject::serial_2->waitForBytesWritten(10))
            {

            }
        }
        else{
            qDebug()<<"port not open";
        }
    }
}

/********************************************************************************************************************************
*   function:       Read_Data()
*   Description:    读取查询到的开关量信息，如果满足控制条件则下发控制机器人的指令
*   Input:
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/
void ReadQuantity::Read_Data()
{
    QByteArray buf;
    QString str;

    buf = this->Quantityserial->readAll();
    if(!buf.isEmpty())
    {
        str += ByteArrayToHexString(buf);
        if(flag == 0)
        {
            if(str[0] == "F" && str[1] == "E" && str[3]=="0" &&str[4]=="2" &&str[6]=="0"&&str[7]=="1" && str[12] == "9" && str[13] == "0")                  //判断读取到的数据是否正确
            {
                if(str[9] == "0" && str[10] == "4" && str[15] == "5" && str[16] == "F")
                {
                    flag_2 = 1;
                    qDebug() << "开关量闭合";
                    emit Data_Thread_stop_sig();
                    write_Data();
                }
            }
            if(flag_2 == 1)
            {
                if(str[0] == "F" && str[1] =="E" && str[3] == "0" && str[4] == "2" && str[6] == "0" && str[7]=="1" && str[12] == "9" && str[13] == "1")
                {
                    if(str[9] == "0" && str[10] == "0" && str[15] == "9" && str[16] == "C")
                    {
                        qDebug() << "开关量断开";
                        emit Data_Thread_start_sig();
                        flag_2 = 0;
                    }
                }
            }

        }
        else if(flag == 1)
        {
            if(str[0] == "F" && str[1] =="E" && str[3] == "0" && str[4] == "2" && str[6] == "0" && str[7]=="1" && str[12] == "9" && str[13] == "1")
            {
                if(str[9] == "0" && str[10] == "0" && str[15] == "9" && str[16] == "C")
                {
                    qDebug() << "开关量关闭";
                    emit Data_Thread_start_sig();
                    flag = 0;
                }
            }
        }

    }
}

/********************************************************************************************************************************
*   function:       querydata()
*   Description:    发送指令读取开关量
*   Input:
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/
void ReadQuantity::querydata()
{
    QString data = "FE 02 00 00 00 08 6D C3";
    QByteArray tmp = HexStringToByteArray(data);
    Quantityserial->write(tmp);
}

/********************************************************************************************************************************
*   function:       Read_Ret()
*   Description:    定时重发机制，读取返回值，返回相同指令表示控制成功
*   Input:
*   Output:
*   Return:
*   Others:
*   Auther:
*   Creat Time:     2019/1/23
*--------------------------------------------------------------------------------------------------------------------------------
*   Modify
*   version         Auther          Date                Modifyication
*   v0.0            实唯          2019/1/23
*
*
********************************************************************************************************************************/

void ReadQuantity::on_Get_Ret(QByteArray ret)
{


    QByteArray buf;
    QString str;
    QString tet = "FE 05 00 02 FF 00 39 F5";    //机器人停止指令

//    qDebug() << "this is on_Get_Ret slot";
    if(port1_send_oder_flg == 1)        //判断哪个机器人控制串口打开
    {
        str = ByteArrayToHexString(ret);
        if( str == tet)
        {
            flag = 1;
        }
        else
        {
            t->stop();
            t->start(5000);
        }
    }
    if(port2_send_oder_flg == 1)
    {
        str = ByteArrayToHexString(ret);
        if( str == tet)
        {
            flag = 1;
        }
        else
        {
            t->stop();
            t->start(5000);
        }
    }
}

void ReadQuantity::onTimerout()
{

    querydata();
    Read_Data();
    qDebug() << "the time is out";
}
