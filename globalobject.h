#ifndef GlobalObject_H
#define GlobalObject_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QSerialPort>
#include <QTime>
#include <QCoreApplication>
#include <QFile>
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
#include <QMutex>
#include <QTextCodec>
enum _type{
    NONE,
    LESS,
    EQUAL,
    GREATER,
};
enum _Logi{
    LOGI_NONE,
    LOGI_AND,
    LOGI_OR
};

class Method{
public:
    Method(){

    }
    ~Method(){

    }
    void pushback_int(int seq,_type sym1,int value,_Logi lg,_type sym2,int value2,int meg,int spn=1){
        sequece=seq;
        i_sym_one=sym1;
        i_value=value;
        i_sym_two=sym2;
        i_value_2=value2;
        Message_=meg;
        _Log=lg;
        serialportnum=spn;
    }
    void pushback_float(int seq,_type sym1,float value,_Logi lg,_type sym2,float value2,int meg,int spn=1){
        sequece=seq;
        i_sym_one=sym1;
        f_value=value;
        i_sym_two=sym2;
        f_value_2=value2;
        Message_=meg;
        _Log=lg;
        serialportnum=spn;
    }
    int sequece;
    _type i_sym_one;
    int i_value;
    short s_value;
    float f_value;
    _type i_sym_two;
    int i_value_2;
    short s_value_2;
    float f_value_2;
    int Message_;
    _Logi _Log;
    int serialportnum;//使用的串口号 0 或者 1

};
//校正系数：
class CorrectionCoefficient
{
public:
    CorrectionCoefficient();
    ~CorrectionCoefficient();
    inline QString Getmac(){
        return Mac;
    }
    void Reset();
public:
    short CorrAccX=0;
    short CorrAccY=0;
    short CorrAccZ=0;
    float CorrAlpha=0.0f;
    float CorrBeta=0.0f;
    float CorrGama=0.0f;
    float CorrDistanceA=0.0f;
    float CorrDistanceB=1.0f;
    float CorrDistanceC=0.0f;
    float CorrLight=0.0f;
    float CorrIR=0.0f;
    float CorrTempA=0.0f;
    float CorrTempB=1.0f;
    float CorrTempC=0.0f;
private:
    QString Mac;
};
struct ScanListItem{//搜索到的设备
    QString mac;
    QString name;
    signed char rssi;
    signed char addrtype;
    quint16 devPortnum=0;
};
struct Datainfo{
    short motion=0;
    short accelx=0;
    short accely=0;
    short accelz=0;
    short intensity=0;
    short IR=0;
    short iUV=0;
    short distance=0;
    float tilt1=0;
    float tilt2=0;
    float tilt3=0;
    float temperature=0;
};
class DeviceInfo{
public:
    ~DeviceInfo();
    QString  mac="";
    QString name="";
    short rssi=0;
    short addrtype=0;
    uchar mode='0';
    uchar state='0';
    uchar connfailcount='0';
    QString time="";
    short faultcode=0;//modify at 20180417 uchar->uint
    QString fault=0;
    float voltage=0;//uchar->uint
    short memory=0;
    QString workmode="";
    bool waitdata=true;//表示是否在等待接收数据包最后的两个字节
    bool m_bCorrectValue=false;
    Datainfo datainfo; //数据集合
    CorrectionCoefficient Corr_Coe;//校正系数
    QVector<Method*> ControlMethodList;
    const int ControlMethodCount_Max=6;
    int MethodCount[12]={0};
    quint16 devPortnum=0;//该设备占用的端口号
    QString newname="";
    bool dataIntgrality=false;
    bool m_bMethodexecRunable=true;
    QByteArray DataInfo_forepart;
    QByteArray DataInfo_backend;
    bool dataget[2]={false};
    QDateTime lastDataTimeStamp;
    bool m_bIntime=false;
    QList<QVector4D> CommandSendList;//已经发送的控制策略索引，接收到就会移除  3d格式为  <串口号，参数索引，命令索引，发送次数>
    QList<uint>CommandTimeStamp;//命令发送时间戳
    const int maxSend=5;//同一指令最多发五次
    const int maxTimeCons=5;//第一次发送指令最长保存记录时间 单位秒
    QList<QVector2D>removedSendState;//移除的发送命令放在这里
    const int Reconnect_Count_Max=30;//连接设备断开后重连次数达到这个数就从列表删除
    int ReCon=0;
public:
    void OrderSend(int index, int port, int sequence, int Orderidx);
    void Method_exec();//这个函数执行控制策略检查
    void CheckDataTimeStamp();
    DeviceInfo* operator=(const DeviceInfo &another);//简单的拷贝构造,没有使用
    bool SendCountCheck(int index, int port, int squ, int &rType, int &cIndex);//检查命令发送状态,检查发送次数和发送时间间距
    void AddItemToCommandList(int port,int index,int Cindex,int SendCount,uint Timestamp,int ty);//添加CommandTimeStamp和CommandSendList数据


};


class GlobalObject
{
public:
    GlobalObject();


    static void DesTroy();
    static QTcpSocket *Socket_;//通信套接字
    static QTcpServer *Server_;//监听套接字
    static quint16 port_;//端口号
    static QTcpSocket *Socket_2;//通信套接字
    static QTcpServer *Server_2;//监听套接字
    static quint16 port_2;//端口号
    static QSerialPort *serial;
    static QSerialPort *serial_2;
    static QMutex *locker;
    static QStringList ControlCommand;//控制信息列表
    static QStringList ControlCommand_rep;//控制信息反馈列表
    static QStringList FalcutyCode;
    //template <class T1,class T2>
    //static bool _Assert(T1 v1,T2 v2,_type ty);
    static bool _Assert(short v1,int v2,_type ty);
    static bool _Assert(float v1,int v2,_type ty);

    static QList<ScanListItem*> *g_ScanDevList;//扫描设备列表
    static QList<DeviceInfo*> *g_ConnectDevList;//连接设备列表
    static QList<DeviceInfo*> *g_DeviceList_Config;//配置文件设备列表
    static QVector<QString>   *g_BlockList;//屏蔽设备
    static bool m_bSend_Access[2];//该变量设置为全局控制串口发送的变量，索引0、1分别对应串口1 2，如果该变量为true，串口才会发送数据

public:



};




class CsvWriter{
public:
    explicit CsvWriter(QString Mac,QString addr=QCoreApplication::applicationDirPath()){
        qDebug()<<"Mac writer";
        this->_Mac=Mac;
        QString time;
        QDateTime local(QDateTime::currentDateTime());

        unsigned int sec=local.toTime_t();
        time=QString::number(sec);
        this->_Addr=addr+"/"+_Mac+"_"+time+".csv";
        qDebug()<<"File created! add is \n"<<_Addr;
        this->_File=new QFile(_Addr);
        if(!_File->exists()){
           method=1;
           bool ok=_File->open(QIODevice::WriteOnly|QIODevice::Append);
           if(ok){
                _out=new QTextStream(_File);
                   //写入表头
               QString header("Time,Mac,Name,AccX,AccY,AccZ,alpha,beta,gama,motion,IR,iuv,distance,Intensity,temperture\n");
               qDebug()<<"header created!";
               _File->write(header.toLatin1());
              // *_out<<header;
               m_bOpen=true;
           }
           else{
               qDebug()<<"*************  Csv file open failed!   ************";
           }
        }
        else{
            qDebug()<<_Addr<<" is already exit ";
            bool ok=_File->open(QIODevice::WriteOnly|QIODevice::Append);
            if(ok){
                 _out=new QTextStream(_File);
                m_bOpen=true;
            }
            else{
                qDebug()<<"*************  Csv file open failed!   ************";
            }
        }



    }
    inline QString GetBindMac(){
        return _Mac;
    }
    ~CsvWriter(){
        if(_File!=nullptr){
            _File->close();
            delete _File;
            _File=nullptr;
        }
        if(_out!=nullptr){
            delete _out;
            _out=nullptr;
        }
    }
     inline QTextStream *GetOut(){
        return _out;
    }
     inline QFile* Get__File(){
         return _File;
     }

private:
  QString _Mac;
  QString _Addr;
  QFile *_File=nullptr;
  QTextStream * _out=nullptr;
  int method=-1;
public:
  bool m_bOpen=false;

};


void RedirectMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);//log file writer
#endif // GlobalObject_H
