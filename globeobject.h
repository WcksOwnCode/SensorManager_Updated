#ifndef GLOBEOBJECT_H
#define GLOBEOBJECT_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QSerialPort>
#include <QTime>
#include <QCoreApplication>
#include <QFile>
#include <QVector4D>
#include <QVector3D>
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
    void pushback_int(int seq,_type sym1,int value,_Logi lg,_type sym2,int value2,int meg){
        sequece=seq;
        i_sym_one=sym1;
        i_value=value;
        i_sym_two=sym2;
        i_value_2=value2;
        Message_=meg;
        _Log=lg;
    }
    void pushback_float(int seq,_type sym1,float value,_Logi lg,_type sym2,float value2,int meg){
        sequece=seq;
        i_sym_one=sym1;
        f_value=value;
        i_sym_two=sym2;
        f_value_2=value2;
        Message_=meg;
        _Log=lg;
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

class GlobeObject
{
public:
    GlobeObject();
    static QTcpSocket *Socket_;//通信套接字
    static QTcpServer *Server_;//监听套接字
    static quint16 port_;//端口号
    static  QStringList ControlCommand;
    //template <class T1,class T2>
    //static bool _Assert(T1 v1,T2 v2,_type ty);
    static bool _Assert(short v1,int v2,_type ty);
    static bool _Assert(float v1,int v2,_type ty);
    static QSerialPort *serial;//串口
};
struct ScanListItem{//搜索到的设备
    QString mac;
    QString name;
    signed char rssi;
    signed char addrtype;
};
struct Datainfo{
    short motion;
    short accelx;
    short accely;
    short accelz;   
    short intensity;
    short IR;
    short iUV;
    short distance;
    float tilt1;
    float tilt2;
    float tilt3;
    float temperature;
};

class DeviceInfo{
public:
    ~DeviceInfo();
    QString  mac;
    QString name;
    short rssi;
    short addrtype;
    uchar mode;
    uchar state;
    uchar connfailcount;
    QString time;
    short faultcode;//modify at 20180417 uchar->uint
    short fault;
    float voltage;//uchar->uint
    short memory;
    QString workmode;
    bool waitdata=true;//表示是否在等待接收数据包最后的两个字节
    bool m_bCorrectValue=false;
    Datainfo datainfo; //数据集合
    CorrectionCoefficient Corr_Coe;//校正系数
    void Method_exec();   
    QVector<Method*> ControlMethodList;
    const int ControlMethodCount_Max=5;
    int MethodCount[12]={0};
    /*
    0 short motion;
    1 short accelx;
    2 short accely;
    3 short accelz;
    4 short intensity;
    5 short IR;
    6 short iUV;
    7 short distance;
    8 float tilt1;
    9 float tilt2;
    10 float tilt3;
    11 float temperature;

*/

    void OrderSend(int index);
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
#endif // GLOBEOBJECT_H
