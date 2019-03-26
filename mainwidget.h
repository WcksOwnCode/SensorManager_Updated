#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "treeviewmodel.h"
#include <QList>
#include <QSerialPort>
#include <serialportdialog.h>
#include <QRegExp>
#include <QThread>
#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QtSql>
#include <QMenu>
#include <QObject>
#include <QAction>
#include "datathread.h"
#include "dataconfigdialog.h"
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QValueAxis>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>
#include "renamedialog.h"
#include "environmentconfig.h"
#include <QStringList>
#include <QHostInfo>
#include "sensorconfig_dialog.h"
#include "readquantity.h"


QT_CHARTS_USE_NAMESPACE;
enum
{
    SHOW_ADD_DEV,
    SHOW_DEL_DEV,
    SHOW_CTRL_DEV
};

enum
{
    BT_CONNECT_IDLE,
    BT_CONNECTING
};
enum //BLE state
{
    POWER_OFF,
    ADVERTISING,
    CONNECTED
};
enum _ConfigType{
    READ_CON,
    WRITE_CON,
    READ_AND_WRITE,
    DELETE_CON
};
namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void PromptInfomation(QString str);
    void AddDeviceToNewDeviceTree();
    void AddDevicetoConnectedTree();
    void UpdateTreeView();
    void PrepareInquire();
    void InquireDev(unsigned char action,unsigned char mod,unsigned char time);
    void BtnDeviceScan();
    bool BdaddrIsZero(QString addr);
    void TCPconnector(QString HostAddress);
    void DataBaseLuncher();
    void FileOutput(QString str, QString add, int method, DeviceInfo *df,CsvWriter *writer);
    void InfoHanddler_new(QString info,int p=0);
    int infoSpliter(QString info, QStringList &returnstr);
    QStringList StringFileter(QString info,int p=0);
    void InqueryProc(QStringList &strList, int px);
    void NotifyProc(QStringList &strList, int px);
    void CharacteristicProc(QStringList &strList, int p);
    void DiscProc(QStringList &strList);
    void DataParser(QByteArray data);

    void ConnectToDevice(QModelIndex curIndex);
    void DisconnetDeviceTrigged(QString Mac, quint16 p);
    void ConnectChoosedDevice();
    void ReadSingelDeviceData(QString Mac, quint16 po);
    void ReadDev(QString Mac);
    void SendDatatoDev(QString Mac, QString charc, QString dat, int po);
    void GetSingleSensorRSSI(QString Mac,quint16 po);

    void SensorDataConvert(QByteArray fore, QByteArray back, QString Mac);
    void TimerTimerOut();
    QString TimeStyleConvert(QByteArray time);
    void ShowDataOnTable(DeviceInfo df);
    void ShowDataOnTable();
    void ItemBeClicked(const QModelIndex &index);
    void Initialize_Environment();
    void ConfigWriter();
    void ConfigeIniWrite(QString key,QString value);
    QString HexTimeToDec(QByteArray t);
    unsigned int HexToDec(QString t);
    void ReadDevice(QString mac, quint16 po);
    void DeviceRename(QString Name, QString Mac);
    void PushRenametoList(QString Name,QString Mac);
    void ConfigerFileSupvisor(int method);
    void ChartDrawer();
    void InsertDatatoDB(DeviceInfo df);
    void ReadConfigFile();
    bool createTable();
    bool insert(const DeviceInfo *Dinfo);
    bool queryAll();
    bool updateById(int id,const DeviceInfo *Dinfo);
    bool deleteById(int id);
    bool sortById();
    bool createTable(const DeviceInfo *Dinfo);
    friend class DataConfigDialog;
    friend class DataThread;
    friend void DataProcess(QString data);
    void ReceiveFromthread(DeviceInfo *Dinfo,int index);
    void ThreadLunchar();
    void SerailState(QString name1, QString Name2);
    void BinaryConfigWrite_Read(_ConfigType ty,QString ConfigAddr);
    static QString HexToDec(QByteArray arr);
    void CharacteristicofDevice(QString mac, quint16 po=0);
    int SignalHexToDec(QString t);
    void SysinfoofDevice(QString Mac, quint16 po);
    void CirculationTask(int t=0);
    QString HexTilteToDec(QByteArray arr);
    void PushToblocklist(QString Mac,QString name);

    void ThrowToMainWindow();

    void _ShortInfoHanddler(QString info,int p);

    void _GetServer(bool b1,bool b2);

    void DataBaseThreadLuncher();
private slots:
    void OtherDeviceMenu(const QPoint &pos);
    void ConectedDeviceMenu(const QPoint &pos);
    void on_SerialPortConfig_pushButton_clicked();
    void on_Device_tabWidget_tabBarClicked(int index);
    void ConnectActionTrigged(QString Mac, quint16 port, signed char ty);
    void on_Exit_pushButton_clicked();

    void on_DataConfig_pushButton_clicked();

    void on_Synchronization_pushButton_clicked();

    void on_EnvironmentConfig_pushButton_clicked();
    void FromEnvDialog(QString ConAddr, QString CsvAddr,
                       QString DBaddr, bool label, bool multithread, bool Csv,
                       bool database, bool Con_ac);

    void on_Config_pushButton_clicked();
    void ResetIp(QString ipaddr);
    void RemoveBlockItem(QString Mac);

    void sig_UpdateTree_();
    void on_Close_pushButton_clicked();

    void on_Minimize_pushButton_clicked();

    void on_Data_Thread_start();
    void on_Data_Thread_stop();


    void on_Start_Robt_Ctr_clicked();

    void on_QuantityStartBtn_clicked();

    void on_RefreshPortButton_clicked();

signals:
    void DataPthreadStart(QString s);
    void DataNeedtoDeal(QByteArray fore, QByteArray back,DeviceInfo *dev,int index);


    //void ShowDataOntable(DeviceInfo *dev);
    //void ShowDataBase(QList<DeviceInfo*>d_List);

    void Handdleinfo(QString info,int port=0);
    void WriteInfo(QString info,int port=0);

    void ServerState(bool t1,bool t2);

    void FocusChanges(int idx);
    void TCPConnectInfomation(QString HostAddress);

    void DB_CreatTable(QString name);

    void  ServerShutDown();

    void ReconnectDevice();//发送这个信号可以启动重连断开设备
protected:
    void closeEvent(QCloseEvent *event);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::MainWidget *ui;
    const double VERSION=1.37;
    QString WindowTitleText;
    const unsigned char INQUIRE_SECONDS=6;//搜索持续时间
    const int   MAX_SCAN_COUNT=32;
    const int	MAX_DEV_COUNT=18;
    const int	MAX_FAIL_COUNT=5 ;   //连接失败次数最大值
    const unsigned int	MAX_AUTO_SEARCH_TIMES=2;	 //自动搜索设备的次数
    const int DEFAULT_ADDR_TYPE=1;  //默认的地址类型 0-public 1-random
    TreeViewModel* m_list1=nullptr;
    TreeViewModel* m_list2=nullptr;
    TreeViewModel* m_ScanList=nullptr;
    QList<ScanListItem*> ScanDevList;
    QList<DeviceInfo*> ConnectDevList;
    QList<DeviceInfo*> DeviceList_Config;
    bool m_bTCPConnected;
    bool m_bSearching;
    bool m_bBLEconnect_state;

    bool windowLocked=false;
    QPoint mousePos;
    QPoint windowPos;
    int CurrentSearchingCount;
    uint    gScanCount;
    uint    gDevCount;
    QString strTreeChosen;
    QString currDevAddr;//当前操作的设备地址
    QString currReadRSSIAddr;//当前读取RSSI的地址
    QString currReadInfoAddr;//当前读取装置信息的地址
    QString gScanData = "0303F0FF";//扫描应答数据，包含服务UUID，根据此数据来筛选传感器
    QString gRxChar = "fff2";//往传感器写数据的特征值UUID
    bool  m_bInitState;
    bool  bTCPConnected=false;
    bool  m_bBTConnected=false;
    //bool  m_bSearching;
    uchar  curShowType;
    uchar  gConnectState;
    uint  gCurDevIndex;//当前自动连接的设备索引
    uint  gReConnIndex;//重新连接的设备索引
    bool  gEnableAutoConnect;//使能自动连接任务
    bool  m_bManualDisc;//手动断开标志
    uint  gSearchTimes;
    //QString gScanData;//传感器扫描应答数据，其中包含完整的服务UUID,可能需要根据设备的实际数据修改
    //QString gRxChar;//往传感器写数据的特征值,可能需要根据设备的实际UUID修改
    //Timer
    QTimer *BLE_SearchingTimer=nullptr;
    QTimer *BLE_Connect_Timer=nullptr;
    QTimer *testtier;
    DataThread *Data_Thread = nullptr;//数据处理线程
    QThread *Pthread=nullptr;
    bool multiThread=true;
    QString testuseMac;
    QByteArray DataInfo_forepart;
    QByteArray DataInfo_backend;
    QStringList allData;
    DeviceInfo* FocusDevice;
    int FocusIndex=-1;
    DataConfigDialog *DataConDialog;
    QSettings *configIniWriter=nullptr;
    QString ConfigAddress=QCoreApplication::applicationDirPath()+"/Config.ini";
    QString ConfigPath=QCoreApplication::applicationDirPath();
    QString CsvUpperAddr=QCoreApplication::applicationDirPath()+"/";
    QString Csvpath=QCoreApplication::applicationDirPath()+"/";
    QString DBaddr=QCoreApplication::applicationDirPath()+"/";
    QString DBpath=QCoreApplication::applicationDirPath()+"/";
    QVector<QString>RenemeDeviceList;
    QVector<QString>BlockList;
    QSqlDatabase Db;
    QSqlQuery query;
    QVariantList idList;
    bool m_bShowOnCharts;

    int PenWidth=3;
    QValueAxis *axisX[6]={nullptr};
    QValueAxis *axisY[6]={nullptr};
    QChart *chart[6]={nullptr};
    QLineSeries *series[6]={nullptr};

    QChart *Distancechart=nullptr;
    QChart *IRchart=nullptr;
    QChart *IUVchart=nullptr;
    QChart *Motionchart=nullptr;
    QLineSeries *Distanceseries=nullptr;
    QLineSeries *IRseries=nullptr;
    QLineSeries *IUVseries=nullptr;
    QLineSeries *Motionseries=nullptr;

    // QValueAxis *axisY[6]={nullptr};
    int DrawCount=0;
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


    RenameDialog *ReDialog;
    const int DrawMaxCount=20;//图上只显示20个点
    CsvWriter *Csv[18]={nullptr};
    // QString CsvFileAddr[18]={QCoreApplication::applicationDirPath()+"/"};
    bool m_bCsv=true;
    QVector<QString>NeedToConnected;
    bool m_bConfigAcvated=true;
    bool m_bDatabaseAcvated=true;
    bool m_bCsvAcvated=false;
    EnvironmentConfig *env=nullptr;

    int DbCount=0;
    QHostInfo ip_info;
    SensorConfig_Dialog *Sc=nullptr;
    QStandardItemModel* model_Con =nullptr;
    QStandardItemModel* model_Scan =nullptr;
    bool m_bTcp_1_con=false;
    bool m_bTcp_2_con=false;
    bool m_bSerial_1_=false;
    bool m_bSerial_2_=false;
    QTimer *RssiInqTimer=nullptr;
    QTimer *AutoConnectTimer=nullptr;//自动连接配置列表中的设备
    QTimer *AutoScan=nullptr;//周期性搜索
    QTimer *ReconnectTimer=nullptr;//周期性重连设备计时器
    int AutoConnectInterval=30;//15秒连接一次
    int ReconnectInterval=6;//在连接设备列表中的设备断开后每6秒进行一次重连


    QSerialPort *serial;
    ReadQuantity * Quanty_thread;
    SerialPortDialog* Sp;

    QThread * DB_Pthread=nullptr;

    DataBase_thread *DB_T=nullptr;
    GlobalObject GB;


    SerialPortDialog *spd=nullptr;//串口界面类，不允许主界面析构前析构
};


#endif // MAINWIDGET_H
