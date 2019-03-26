#ifndef DATATHREAD_H
#define DATATHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "globalobject.h"
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QValueAxis>
#include <QHostInfo>
class DataThread:public QObject
{
    Q_OBJECT
public:
    explicit DataThread( QObject *parent = 0);
    ~DataThread();
    void DataProcess(QString info, int p);
    int infoSpliter(QString info, QStringList &returnstr, int px);
    void Thread_infoHandler(QString info, int p);
    void InqueryProc(QStringList &strList,int px=0);
    QStringList StringFileter(QString info,int );
    void NotifyProc(QStringList &strList,int px=0);
    int infoSpliter(QString info,QStringList &returnstr);
    void CharacteristicProc(QStringList &strList,int px=0);
    void DiscProc(QStringList &info);


    void ConnectToDevice(QString Mac);
    void DisconnetDeviceTrigged(QString Mac, quint16 p);
    void ConnectChoosedDevice();
    void ReadSingelDeviceData(QString Mac, quint16 po);
    void ReadDev(QString Mac);
    void SendDatatoDev(QString Mac, QString charc, QString dat, int po);
    void GetSingleSensorRSSI(QString Mac,quint16 po);
    void BtnDeviceScan();
    bool BdaddrIsZero(QString addr);
    void ConnectDevice(QString Mac,quint16 port,signed char ty);
    void PrepareInquire();
    void InquireDev(unsigned char action,unsigned char mod,unsigned char time);


    void CharacteristicofDevice(QString mac, quint16 po);
    void ReadDevice(QString mac,quint16 po=0);

   // QStringList DataThread::StringFileter(QString info,int )
    inline bool isRunning(){
        return m_bisRunning;
    }
    QString HexTimeToDec(QByteArray t);
    void SetRunning(bool tr);
    void DataProcess_Simple(QByteArray fore, QByteArray back,DeviceInfo *dev,int index);
    inline bool isBusy(){
        return m_bBusy;
    }
    QString HexToDec(QByteArray arr);
    unsigned int HexToDec(QString t);
    QString HexTilteToDec(QByteArray arr);

    void GetWriteMsg(QString Info,int p);
    void _SocketWriter(QString info,int p);
    void ServerStateCheck(bool b1,bool b2);
    void SetFocusIndex(int idx);

    void FileOutput(QString str, QString add="", int method=0, DeviceInfo *df=nullptr,CsvWriter *writer=nullptr);
    void TCPconnector(QString HostAddress="");

    void CloseServer();
    void ReciveFromConnected(QString s);

    void ReconnectDevice();

    void synchronization();
signals:
    void dataproDone(QString datainfo);
    void workDone(DeviceInfo *dev,int index);
    void ToInqueryProc(QStringList &strList,int px=0);
    //void updateTree(QList<DeviceInfo>TreeItems);
    void updateTree();
    void _ShortInfo(QString info,int p);
    void ServerState(bool b1,bool b2);

    void ToUIshow(DeviceInfo dev);

    void InsertDataToDataBase(const DeviceInfo *Dinfo);

private:
    bool m_bisRunning=false;
    bool m_bBusy=false;
    bool m_bTcp_1_con=false;
    bool m_bTcp_2_con=false;
    bool m_bSerial_1_=false;
    bool m_bSerial_2_=false;
    bool m_bTCPConnected=false;
    bool m_bCsv=false;
    int FocusIndex;//当前界面选中的设备index

    const unsigned char INQUIRE_SECONDS=6;//搜索持续时间
    const int   MAX_SCAN_COUNT=32;
    const int	MAX_DEV_COUNT=18;
    const int	MAX_FAIL_COUNT=5 ;   //连接失败次数最大值
    const unsigned int	MAX_AUTO_SEARCH_TIMES=2;	 //自动搜索设备的次数

    QString gScanData = "0303F0FF";//扫描应答数据，包含服务UUID，根据此数据来筛选传感器
    QString gRxChar = "fff2";//往传感器写数据的特征值UUID

    CsvWriter *Csv[18]={nullptr};


    QList<short>DataBuff_AccX;
    QList<short>DataBuff_AccY;
    QList<short>DataBuff_AccZ;
    QList<float>DataBuff_Tilt1;
    QList<float>DataBuff_Tilt2;
    QList<float>DataBuff_Tilt3;

    QList<short>DataBuff_IR;
    QList<short>DataBuff_IUV;
    QList<short>DataBuff_Motion;
    QList<short>DataBuff_Distance;


     QTcpSocket *Socket_;//通信套接字
     QTcpServer *Server_;//监听套接字
     quint16 port_;//端口号
     QTcpSocket *Socket_2;//通信套接字
     QTcpServer *Server_2;//监听套接字
     quint16 port_2;//端口号


};

#endif // DATATHREAD_H
