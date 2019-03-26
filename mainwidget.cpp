#pragma execution_character_set("utf-8")
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QTabWidget>
#include "globalobject.h"
#include <QTcpServer>
#include "treeviewmodel.h"
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QHostInfo>
#include <iostream>
#include <fstream>
#include <ostream>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    qDebug()<<"Mainwidget Thread id  "<<QThread::currentThreadId()<<QThread::currentThread();
    testtier=new QTimer(this);
    connect(testtier,&QTimer::timeout,this,&MainWidget::Initialize_Environment);
    testtier->start(300);
    QString localHostName = QHostInfo::localHostName();
    //qDebug() << "LocalHostName:" << localHostName;
    ip_info = QHostInfo::fromName(localHostName);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, QColor(255,255,255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    QString ver=QString::number(VERSION);
    WindowTitleText=tr("机器人传感器控制系统")+"(v"+ver+")";
    this->setWindowFlags(Qt::FramelessWindowHint);//隐藏windows标题栏
    ui->Title_Labal->setText(WindowTitleText);
    //this->setWindowTitle(WindowTitleText);

    QImage img;
    if(img.load(":/new/logo/rsc/resize.bmp"))//execute qmake before load
    {
        QImage img2= img.scaled(ui->biglogo_label->width(),ui->biglogo_label->height());
        QPixmap qp=QPixmap::fromImage(img2);
        ui->biglogo_label->setPixmap(qp);
    }
    QImage Iconimg;
    if(Iconimg.load(":/new/logo/rsc/logo.jpg"))//execute qmake before load
    {
        QImage Iconimg2= Iconimg.scaled(ui->Icon_Label->width(),ui->Icon_Label->height());
        QPixmap qp=QPixmap::fromImage(Iconimg2);
        ui->Icon_Label->setPixmap(qp);
    }
    ui->Title_Labal->setStyleSheet("background-color:transparent;");
    ui->Minimize_pushButton->setStyleSheet("background-color:transparent;");
    ui->Close_pushButton->setStyleSheet("background-color:transparent;");


    Sp=new SerialPortDialog;

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->QuantyCmbBox->addItem(serial.portName());
            serial.close();
        }
    }

    Quanty_thread = new ReadQuantity;

    connect(Sp,SIGNAL(Get_ret(QByteArray)),Quanty_thread,SLOT(on_Get_Ret(QByteArray)));
    connect(Sp,SIGNAL(Get_ret_2(QByteArray)),Quanty_thread,SLOT(on_Get_Ret(QByteArray)));
    connect(Quanty_thread,SIGNAL(Data_Thread_stop_sig()),this, SLOT(on_Data_Thread_stop()));
    connect(Quanty_thread,SIGNAL(Data_Thread_start_sig()),this,SLOT(on_Data_Thread_start()));

    ui->RefreshPortButton->setEnabled(false);
    ui->Start_Robt_Ctr->setEnabled(false);
    ui->Env_State->setText("尚未监控");
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    qDebug()<<QString::fromLocal8Bit("中文乱码输出测试  标点符号，。、‘；、】【】’");





}
MainWidget::~MainWidget()
{

    for(int q=0;q<GlobalObject::g_ConnectDevList->length();q++){
        DisconnetDeviceTrigged(GlobalObject::g_ConnectDevList->at(q)->mac,GlobalObject::g_ConnectDevList->at(q)->devPortnum);
    }


    if(testtier->isActive()){
        testtier->stop();
    };

    delete testtier;

    if(configIniWriter!=nullptr){
        delete configIniWriter;
    }

    for(int i=0;i<6;i++){
        if(chart[i]!=nullptr){
            delete chart[i];
        }
        if(series[i]!=nullptr){
            delete series[i];
        }

    }


    if(Data_Thread!=nullptr){
        delete Data_Thread;
        Data_Thread=nullptr;
    }

    if(Pthread!=nullptr){
        Pthread->quit();
        Pthread->wait();

    }


    Db.close();

    if(Distancechart!=nullptr){
        delete Distancechart;
        Distancechart=nullptr;
    }
    if(IRchart!=nullptr){
        delete IRchart;
        IRchart=nullptr;
    }
    if(IUVchart!=nullptr){
        delete IUVchart;
        IUVchart=nullptr;
    }
    if(Motionchart!=nullptr){
        delete Motionchart;
        Motionchart=nullptr;
    }
    if(Distanceseries!=nullptr){
        delete Distanceseries;
        Distanceseries=nullptr;
    }
    if(IRseries!=nullptr){
        delete IRseries;
        IRseries=nullptr;
    }

    if(IUVseries!=nullptr){
        delete IUVseries;
        IUVseries=nullptr;
    }

    if(Motionseries!=nullptr){
        delete Motionseries;
        Motionseries=nullptr;
    }

    if(Sc!=nullptr){
        delete Sc;
        Sc=nullptr;
    }

    if(env!=nullptr){
        delete env;
        env=nullptr;
    }

    for(int i=0;i<18;i++){
        if(Csv[i]!=nullptr){
            Csv[i]->Get__File()->close();
            delete Csv[i];
            Csv[i]=nullptr;
        }
    }
    if(spd!=nullptr){
        delete spd;
    }
    delete ui;
}
void MainWidget::BinaryConfigWrite_Read(_ConfigType ty,QString ConfigAddr){
    //用二进制的方式保存配置文件更加高效
    //没有使用这个 函数
    if(ty==READ_CON){
        //READ

    }
    if(ty==WRITE_CON){
        QString binaryFilename="F://banary.txt";
        QFile file(binaryFilename);
        if(!file.exists())return;
        file.open(QIODevice::WriteOnly);
        file.close();


    }
}
void MainWidget::Initialize_Environment(){
    PromptInfomation("初始化.");
    RssiInqTimer=new QTimer(this);
    connect(RssiInqTimer,&QTimer::timeout,this,[=]{
        CirculationTask(0);
    });
    RssiInqTimer->start(20000);

    AutoConnectTimer=new QTimer(this);
    connect( AutoConnectTimer,&QTimer::timeout,this,[=]{
        CirculationTask(1);
    });
    AutoConnectTimer->start(AutoConnectInterval*1000);
    AutoScan=new QTimer(this);
    ReconnectTimer=new QTimer(this);
    connect( AutoConnectTimer,&QTimer::timeout,this,[=]{
        CirculationTask(2);
    });
    connect(ReconnectTimer,&QTimer::timeout,this,[=]{
        CirculationTask(3);
    });
    AutoConnectTimer->start(AutoConnectInterval*1000);//自动搜索一次
    ReconnectTimer->start(ReconnectInterval*1000);//自动重连函数
    ConfigAddress=ConfigPath+"/"+QString::number(GlobalObject::port_)+"_Config.ini";
    testtier->stop();
    ThreadLunchar();
    DataBaseLuncher();
    TCPconnector("");



    disconnect(testtier,&QTimer::timeout,this,&MainWidget::Initialize_Environment);
    ui->Device_tabWidget->setTabText(0,tr("已连接设备"));
    ui->Device_tabWidget->setTabText(1,tr("扫描设备"));
    ui->ConnectedDevice_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->OtherDevicetree_Wiew->setContextMenuPolicy(Qt::CustomContextMenu);//开了这个才能开启菜单触发
    connect(ui->OtherDevicetree_Wiew,SIGNAL(customContextMenuRequested(const QPoint& )),this,SLOT(OtherDeviceMenu(const QPoint&)));//右键菜单槽
    connect(ui->ConnectedDevice_treeView,SIGNAL(customContextMenuRequested(const QPoint& )),this,SLOT(ConectedDeviceMenu(const QPoint&)));
    connect(ui->ConnectedDevice_treeView,&QTreeView::clicked,this,&MainWidget::ItemBeClicked);

    if(m_bConfigAcvated){
        QFile check(ConfigAddress);
        configIniWriter=new QSettings(ConfigAddress, QSettings::IniFormat);
        if(check.exists()){
            ConfigerFileSupvisor(READ_CON);
        }
    }
    ui->Chart_tabWidget->setTabText(0,tr("加速度"));
    ui->Chart_tabWidget->setTabText(1,tr("倾角"));
    ui->Chart_tabWidget->setTabText(2,tr("距离"));
    ui->Chart_tabWidget->setTabText(3,tr("红外"));
    ui->Chart_tabWidget->setTabText(4,tr("紫外"));
    ui->Chart_tabWidget->setTabText(5,tr("运动量"));
    m_bShowOnCharts=true;


    // queryAll();
}
void MainWidget::TCPconnector(QString HostAddress=""){
    QHostAddress HostAdd;
    if(HostAddress==""){
        int len=ip_info.addresses().size();
        for(int i=0;i<len;i++){
            if(ip_info.addresses()[i].toString().length()==15){
                HostAdd=ip_info.addresses()[i];
                break;
            }
        }
    }
    else{
        HostAdd=QHostAddress(HostAddress);
    }

    emit TCPConnectInfomation(HostAddress);
    return;

    // QHostAddress HostAdd(ip_info.addresses()[1]);
    qDebug()<<"ip_info.addresses()  "<<ip_info.addresses();
    if(GlobalObject::port_!=0){
        ui->ip_label->setText("本机ip   "+(HostAdd.toString()));
        GlobalObject::Server_->listen(HostAdd,GlobalObject::port_);
        connect(GlobalObject::Server_,&QTcpServer::newConnection,this,[=]{
            GlobalObject::Socket_=GlobalObject::Server_->nextPendingConnection();
            ui->Port1_State_label->setText(QString::number(GlobalObject::port_)+"  Connected");
            PromptInfomation("TCP connected");
            m_bTcp_1_con=true;
            CurrentSearchingCount=0;
            m_bTCPConnected=true;
            connect(GlobalObject::Socket_,&QTcpSocket::readyRead,this,[=]{
                qDebug()<<"1 read ready";
                QByteArray s=GlobalObject::Socket_->readAll();
                if(Data_Thread->isRunning()){
                    emit Handdleinfo(s,0);
                }
                else{
                    InfoHanddler_new(s,0);
                }

                qDebug()<<"father thread id  "<<QThread::currentThreadId();
            });
            emit ServerState(m_bTcp_1_con,m_bTcp_2_con);
        });
    }
    else{
        if(GlobalObject::port_2!=0&&GlobalObject::port_2!=GlobalObject::port_){
            GlobalObject::Server_2->listen(HostAdd,GlobalObject::port_2);
            connect(GlobalObject::Server_2,&QTcpServer::newConnection,this,[=]{
                GlobalObject::Socket_2=GlobalObject::Server_2->nextPendingConnection();
                // ui->ConnectState_Label->setText("已经连接网关 2");
                ui->Port2_State_label->setText(QString::number(GlobalObject::port_2)+"  Connected");
                //qDebug()<<"port2"<<GlobalObject::Socket_->isOpen()<<"  "<<GlobalObject::Socket_2->isOpen();
                //CurrentSearchingCount=0;
                m_bTcp_2_con=true;
                m_bTCPConnected=true;

                connect(GlobalObject::Socket_2,&QTcpSocket::readyRead,this,[=]{
                    qDebug()<<"2 read ready";
                    QByteArray s=GlobalObject::Socket_2->readAll();
                    //InfoHanddler(s);
                    if(Data_Thread->isRunning()){
                        emit Handdleinfo(s,1);
                    }
                    else{
                        InfoHanddler_new(s,1);
                    }
                });
                emit ServerState(m_bTcp_1_con,m_bTcp_2_con);
            });
        };
    }
    PromptInfomation("等待蓝牙网关");
}
void MainWidget::DataBaseLuncher(){
    if(m_bDatabaseAcvated){
        DataBaseThreadLuncher();
        PromptInfomation("DataBase is ready");
    }
}
void MainWidget::ConfigerFileSupvisor(int method){
    qDebug()<<"Config supervisor";
    QFile check(ConfigAddress);
    switch (method) {
    case READ_CON:
        ReadConfigFile();
        break;
    case WRITE_CON:
        if(check.exists()){
            check.remove();
            delete configIniWriter;
            configIniWriter=nullptr;
            configIniWriter=new QSettings(ConfigAddress, QSettings::IniFormat);
        }
        ConfigWriter();
        break;
    case READ_AND_WRITE:
        break;
    case DELETE_CON:
        break;
    }
}
void MainWidget::ConfigWriter(){
    qDebug()<<"WRITE_CON";
    ConfigeIniWrite("General/Time",QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    //write connecter device
    ConfigeIniWrite("General/DeviceCount",QString::number(GlobalObject::g_ConnectDevList->length()));
    //ConfigeIniWrite("General/RenameCount",QString::number(RenemeDeviceList.length()));
    ConfigeIniWrite("General/BlockCount",QString::number(BlockList.length()));
    ConfigeIniWrite("General/CSVpath",Csvpath);

    //device info write
    for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
        ConfigeIniWrite("/Device"+QString::number(i)+"/Mac",GlobalObject::g_ConnectDevList->at(i)->mac);
        ConfigeIniWrite("/Device"+QString::number(i)+"/Name",GlobalObject::g_ConnectDevList->at(i)->name);
        ConfigeIniWrite("/Device"+QString::number(i)+"/NewName",GlobalObject::g_ConnectDevList->at(i)->newname);
        ConfigeIniWrite("/Device"+QString::number(i)+"/RSSI",QString::number(GlobalObject::g_ConnectDevList->at(i)->rssi));
        //修正系数
        qDebug()<<"write correction value";
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAccX",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrAccX));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAccY",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrAccY));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAccZ",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrAccZ));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAlpha",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrAlpha));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrBeta",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrBeta));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrGama",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrGama));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrDistanceA",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrDistanceA));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrDistanceB",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrDistanceB));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrDistanceC",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrDistanceC));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrLight",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrLight));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrIR",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrIR));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrTempA",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrTempA));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrTempB",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrTempB));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrTempC",QString::number(GlobalObject::g_ConnectDevList->at(i)->Corr_Coe.CorrTempC));
        //控制策略
        qDebug()<<"write cotrol method";
        ConfigeIniWrite("/Device"+QString::number(i)+
                        "/Method_count",QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList.length()));
        for(int q=0;q<GlobalObject::g_ConnectDevList->at(i)->ControlMethodList.length();q++){
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sequece_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->sequece));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sym_one_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->i_sym_one));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_int_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->i_value));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_float_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->f_value));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sym_two_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->i_sym_two));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_2_int_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->i_value_2));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_2_float_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->f_value_2));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_Message__"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->Message_));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_Log_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->_Log));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Port_"+QString::number(q),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[q]->serialportnum));
        }
    }
    //write block list
    for(int i=0;i<BlockList.length();i+=2){
        ConfigeIniWrite("/Block"+QString::number(i)+"/"+BlockList[i],BlockList[i+1]);
    }
}
void MainWidget::ConfigeIniWrite(QString key,QString value){
    if(configIniWriter!=nullptr){
        configIniWriter->setValue(key,value);
    }
}

QStringList MainWidget::StringFileter(QString info,int ){
    info.remove(QChar('\n'), Qt::CaseInsensitive);
    info.remove(QChar('\\'), Qt::CaseInsensitive);
    info.remove(QChar('\r'), Qt::CaseInsensitive);
    info.remove(QChar('"'), Qt::CaseInsensitive);

    info.replace(":",",");
    info.remove(QString("OK"), Qt::CaseInsensitive);
    info.remove(QString("ERROR"), Qt::CaseInsensitive);
    info.replace("+",",");
    QStringList ret;
    QStringList lists=info.split(QRegExp(","));
    int len=lists.length();
    bool needpush=false;
    int pos=0;
    QString meg;
    //qDebug()<<"cleared info  "<<info;
    // qDebug()<<"info to list   "<<lists;
    while(pos<len){
        if(lists[pos]=="INQRESULT"||lists[pos]=="NOTIFY"
                ||lists[pos]=="CHARACTERISTIC"||lists[pos]=="RSSI"
                ||lists[pos]=="READCHAR"||lists[pos]=="DISC"
                ||lists[pos]=="INQCOMPLETE"){


            meg.push_back(";");
            meg.push_back(lists[pos]);
            meg.push_back(",");
        }
        else{
            meg.push_back(lists[pos]);
            meg.push_back(",");

        }
        pos++;
    }
    // qDebug()<<"meg   "<<meg;
    // meg.remove(meg.length()-1);

    ret=meg.split(";");

    // qDebug()<<"ret "<<ret;
    return ret;
}
void MainWidget::InfoHanddler_new(QString info, int p){
    //0----INQRESULT
    //1----NOTIFY
    //2----CHARACTERISTIC:
    //3----RSSI
    //4----READCHAR
    //5----DISC
    //6----ignore message
    //7-----INQCOMPLETE
    //-1---出错
    //qDebug()<<"info "<<info;
    QStringList strlist =StringFileter(info);
    for(int q=0;q<strlist.length();q++){
        QStringList strList;
        int ret = infoSpliter(strlist[q],strList);

        switch (ret) {
        case -1:
            qDebug()<<"error";
            break;
        case 0:
            InqueryProc(strList,p);
            break;
        case 1:
            NotifyProc(strList,p);
            break;
        case 2:
            CharacteristicProc(strList,p);
            break;
        case 3:
            //after sp  ("RSSI", "E5AEFA129DDC", "b4", "")
            for(int q=0;q<GlobalObject::g_ConnectDevList->length();q++){
                qDebug()<<"q "<<q<<"str "<<strList[2];
                if(GlobalObject::g_ConnectDevList->at(q)->mac==strList[1]){
                    GlobalObject::g_ConnectDevList->at(q)->rssi=static_cast<short>(HexToDec(strList[2]))-256;
                }
            }
            break;
        case 4:
            //do nothing;
            break;
        case 5:
            DiscProc(strList);
            break;
        case 6:

            break;
        case 7:
            PromptInfomation(tr("搜索完成"));
            m_bSearching=false;
            break;

        }

    }
}
int  MainWidget::infoSpliter(QString info,QStringList &returnstr){
    //0----INQRESULT
    //1----NOTIFY
    //2----CHARACTERISTIC:
    //3----RSSI
    //4----READCHAR
    //5----DISC
    //6----ignore message
    //7-----INQCOMPLETE
    //-1---error
    if(info.contains("INQRESULT",Qt::CaseInsensitive)){
        // qDebug()<<"\n"<<"info :"<<info<<"\n";
        //"INQRESULT,A80C6348591C,0,,0,031900000201060302121808ff7d020103002082,-74,"
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        //" INQRESULT; A80C6348591C; 0; name; 0; 031900000201060302121808ff7d020103002082; -74;"
        return 0;
    }
    if(info.contains("INQCOMPLETE",Qt::CaseInsensitive)){
        PromptInfomation("搜索完成");
        m_bSearching=false;
        return 7;
    }
    if(info.contains("OK",Qt::CaseInsensitive)){
        //来自数据
        qDebug()<<"handle ok";
        return 6;
    }
    if(info.contains("ERROR",Qt::CaseInsensitive)){
        qDebug()<<"handle error";
        return 6;
    }
    if(info.contains("CHARACTERISTIC",Qt::CaseInsensitive)){

        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        return 2;
    }
    if(info.contains("DISC")){
        // "\r\n+DISC:E5AEFA129DDC\r\n"
        qDebug()<<"before   "<<info;
        //info.remove(QString("DISC"), Qt::CaseInsensitive);
        qDebug()<<"after  "<<info;
        returnstr.clear();
        returnstr= info.split(QRegExp(","));

        return 5;
    }
    if(info.contains("NOTIFY",Qt::CaseInsensitive)){
        //由于网关的BLE MTU长度为23字节，也就是实际允许接收数据长度最大为20，所以会分两次收完
        returnstr.clear();
        returnstr= info.split(QRegExp(","));

        return 1;
    }
    if(info.contains("RSSI",Qt::CaseInsensitive)){

        qDebug()<<"befor sp "<<info;
        //befor sp  "RSSI,E5AEFA129DDC,b4,"
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        qDebug()<<"after sp "<<returnstr;
        //after sp  ("RSSI", "E5AEFA129DDC", "b4", "")
        return 3;
    }
    if(info.contains("READCHAR",Qt::CaseInsensitive)){
        // "\r\n+READCHAR:E5AEFA129DDC,fff2,012001\r\n"
        //info.remove(QString("READCHAR"), Qt::CaseInsensitive);
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        return 4;
    }
    else{
        qDebug()<<"other type info :\n"<<info<<"\n";
        return -1;
    }
}
void MainWidget::SensorDataConvert(QByteArray fore, QByteArray back,QString Mac){
    if(GlobalObject::g_ConnectDevList->length()==0){
        qDebug()<<"匿名设备data传入";
        return;
    }
    qDebug()<<" main thread convert ";
    DeviceInfo *newdevice=new DeviceInfo;
    int index=0;
    bool find=false;
    newdevice->datainfo=newdevice->datainfo;
    for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
        if(Mac==GlobalObject::g_ConnectDevList->at(i)->mac){
            find=true;
            index=i;
            break;
        }
    }
    if(find){
        delete newdevice;
        newdevice=nullptr;
    }
    if(!find){
        qDebug()<<"not find";
        if(Mac.length()==12){
            newdevice->mac=Mac;
            index=GlobalObject::g_ConnectDevList->length();
            GlobalObject::g_ConnectDevList->push_back(newdevice);
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
        }
        else{
            return ;
        }
    }

    DeviceInfo *dev=GlobalObject::g_ConnectDevList->at(index);

    if(fore[0]=='1'&&fore[1]=='2'&&fore[2]=='c'){
        //12c0 5a d9 47 26   00 4f   03 20   fd 72   00 13   00 00   ff d8   00 01
        //时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B
        // qDebug()<<"时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B";
        QByteArray  temp1;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        dev->time=HexTimeToDec(temp1);
        temp1.clear();

        QDateTime local(QDateTime::currentDateTime());
        QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
        //ui->ComputerTime_label->setText(localTime);


        dev->waitdata=true;
        for(int q=12;q<16;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.motion = HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=16;q<20;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.accelx = HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=20;q<24;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.accely =  HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=24;q<28;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.accelz = HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=28;q<32;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.tilt1 = HexTilteToDec(temp1).toFloat();
        temp1.clear();
        for(int q=32;q<36;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.tilt2 =HexTilteToDec(temp1).toFloat();
        temp1.clear();
        for(int q=36;q<40;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.tilt3 = HexTilteToDec(temp1).toFloat();
        temp1.clear();
        if(dev->m_bCorrectValue){

            dev->datainfo.accelx +=dev->Corr_Coe.CorrAccX;
            dev->datainfo.accely +=dev->Corr_Coe.CorrAccY;
            dev->datainfo.accelz +=dev->Corr_Coe.CorrAccZ;
            dev->datainfo.tilt1 +=dev->Corr_Coe.CorrAlpha;
            dev->datainfo.tilt2 +=dev->Corr_Coe.CorrBeta;
            dev->datainfo.tilt3 +=dev->Corr_Coe.CorrGama;
        }

    }
    else if(fore[0]=='1'&&fore[1]=='1'&&fore[2]=='2'){
        //1120 5a dd 98 7a   00 00 00 04   00 00 01 00   01 84   00 01   00(时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B)
        //"\r\n+NOTIFY:E5AEFA129DDC,fff1,1120386f4fcd000000000000000001a3000101\r\n\r\n+NOTIFY:E5AEFA129DDC,fff1,06b0386f4fcd0000\r\n"
        //("E5AEFA129DDC", "fff1", "1120 386f558d 00000010 00000011 01a1 0001 01", "fff1", "06b0386f4fcd0000")
        //qDebug()<<"时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B";
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        dev->time=HexTimeToDec(temp1);
        temp1.clear();
        for(int q=12;q<20;q++){
            temp1.push_back(fore[q]);
        }
        dev->faultcode=HexToDec(temp1).toShort();
        if(dev->faultcode>11||dev->faultcode<0){
            dev->fault=QString::number(dev->faultcode)+"error code";
        }else{
            dev->fault=GlobalObject::FalcutyCode[dev->faultcode];
        }

        temp1.clear();
        for(int q=28;q<32;q++){
            temp1.push_back(fore[q]);
        }
        int v=HexToDec(temp1).toInt();
        float vol=v/100.0f;
        dev->voltage = vol;//dat[7];
        temp1.clear();
        for(int q=33;q<37;q++){
            temp1.push_back(fore[q]);
        }
        dev->memory =HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=37;q<38;q++){
            temp1.push_back(fore[q]);
        }
        dev->workmode =HexToDec(temp1);

    }
    if(back[0]=='0'&&back[1]=='9'){

        QByteArray  temp1,temp2;
        QString newbyte;


        dev->waitdata =false;
        dev->dataIntgrality=true;

        for(int q=4;q<8;q++){
            temp1.push_back(back[q]);
        }
        dev->datainfo.intensity = HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=8;q<12;q++){
            temp1.push_back(back[q]);
        }
        dev->datainfo.IR =HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=12;q<14;q++){
            temp1.push_back(back[q]);
        }
        dev->datainfo.iUV = HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=14;q<18;q++){
            temp1.push_back(back[q]);
        }
        dev->datainfo.distance = HexToDec(temp1).toShort();
        temp1.clear();
        for(int q=18;q<22;q++){
            temp1.push_back(back[q]);
        }
        //qDebug()<<temp1;
        dev->datainfo.temperature =HexToDec(temp1).toFloat()*0.1f;
        //qDebug()<<"temo "<<dev->datainfo.temperature;
        if(dev->m_bCorrectValue){
            dev->datainfo.IR +=static_cast<short>(dev->Corr_Coe.CorrIR);
            dev->datainfo.iUV+=static_cast<short>(dev->Corr_Coe.CorrLight);
            dev->datainfo.distance=dev->datainfo.distance*dev->datainfo.distance*dev->Corr_Coe.CorrDistanceA+dev->Corr_Coe.CorrDistanceB*dev->datainfo.distance+dev->Corr_Coe.CorrDistanceC;
            float tempe=dev->datainfo.temperature;
            qDebug()<<"dev->Corr_Coe.CorrTempA"<<dev->Corr_Coe.CorrTempA<<"dev->Corr_Coe.CorrTempB "<<dev->Corr_Coe.CorrTempB<<"dev->Corr_Coe.CorrTempC "<<dev->Corr_Coe.CorrTempC;
            dev->datainfo.temperature = tempe*dev->Corr_Coe.CorrTempA*dev->Corr_Coe.CorrTempA+tempe*dev->Corr_Coe.CorrTempB+dev->Corr_Coe.CorrTempC;
        }

    }
    if(back[1]=='0'&&back[2]=='6'){
        dev->waitdata =false;
        dev->dataIntgrality=true;
        //qDebug()<<"data process ignore "<<back;
    }
    if(dev->m_bMethodexecRunable){
        dev->Method_exec();
    }


    dev->dataget[0]=false;
    dev->dataget[1]=false;
    bool lastState=dev->m_bIntime;
    dev->CheckDataTimeStamp();
    if(lastState!=dev->m_bIntime){
        AddDevicetoConnectedTree();
    }
    dev->lastDataTimeStamp=QDateTime::currentDateTime();


    ShowDataOnTable();
}
void MainWidget::InqueryProc(QStringList &strList,int px=0){
    //" INQRESULT; A80C6348591C; 0; name; 0; 031900000201060302121808ff7d020103002082; -74;"
    if(strList[1].length()==12&&strList[3]!=""){
        for(int q=0;q<BlockList.length();q++){
            if(strList[1]==BlockList[q]){
                return;
            }
        }
        bool find=false;//check is it in the scanlist
        for(int q=0;q<ScanDevList.length();q++){
            if(ScanDevList[q]->mac==strList[1]){
                find=true;
                break;
            }
        }

        if(!find){
            if(strList[1].length()==12){
                ScanListItem *SLI=new ScanListItem;
                SLI->mac=strList[1];
                SLI->name=strList[3];
                if(px==0){
                    SLI->devPortnum=GlobalObject::port_;
                }
                else{
                    SLI->devPortnum=GlobalObject::port_2;
                }
                SLI->rssi=static_cast<signed char>(strList[strList.size()-1].toInt());
                SLI->addrtype=static_cast<signed char>(1);
                if(ScanDevList.length()<MAX_SCAN_COUNT){
                    ScanDevList.push_back(SLI);
                }
                AddDeviceToNewDeviceTree();
            }
        }
        //配置文件中保存的列表 直接连接
        for(int q=0;q<DeviceList_Config.length();q+=2){
            if(DeviceList_Config[q]->mac==strList[1]){
                //直接连接
                signed char ty=static_cast<signed char>(strList[2].toInt());
                if(px==0){
                    ConnectActionTrigged(DeviceList_Config[q]->mac,GlobalObject::port_,ty);
                }
                else{
                    ConnectActionTrigged(DeviceList_Config[q]->mac,GlobalObject::port_2,ty);
                }

            }
        }
    }
}
void MainWidget::NotifyProc(QStringList &strList,int px=0){
    //NOTIFY;E5AEFA129DDC;fff1;12c0387042af000e0090ffcc04960045ffe70339;;
    // "NOTIFY,C4CC019C6A79,fff1,06b0386cd8410000,"//这是什么情况
    //"NOTIFY", "E5AEFA129DDC", "fff1", "1120387060c8000000100000001101a3000101", ""
    qDebug()<<"notify str list "<<strList<<QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
    if(strList.length()<3){
        return;
    }
    if(GlobalObject::g_ConnectDevList->length()==0){
        if(px==0){
            DisconnetDeviceTrigged(strList[1],GlobalObject::port_);
        }
        else{
            DisconnetDeviceTrigged(strList[1],GlobalObject::port_2);
        }

    }
    for(int q=0;q<GlobalObject::g_ConnectDevList->length();q++){
        if(GlobalObject::g_ConnectDevList->at(q)->mac==strList[1]&&GlobalObject::g_ConnectDevList->at(q)->name==""){
            GlobalObject::g_ConnectDevList->removeAt(q);
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
            return;
        }
        if(GlobalObject::g_ConnectDevList->at(q)->mac==strList[1]){
            if(strList[3].length()==40){
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_forepart=strList[3].toLatin1();
                qDebug()<<"fore get";
                GlobalObject::g_ConnectDevList->at(q)->dataget[0]=true;
            }
            else if(strList[3].length()==22){
                qDebug()<<"back get";
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_backend=strList[3].toLatin1();
                GlobalObject::g_ConnectDevList->at(q)->dataget[1]=true;
            }
            else if(strList[3].length()==38){
                qDebug()<<"fore get";
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_forepart=strList[3].toLatin1();
                GlobalObject::g_ConnectDevList->at(q)->dataget[0]=true;
            }
            else if(strList[3].length()==16){
                qDebug()<<"back get";
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_backend=strList[3].toLatin1();

                GlobalObject::g_ConnectDevList->at(q)->dataget[1]=true;

            }
            else{
                qDebug()<<"unknow data   "<<strList[3];
            }
            if(GlobalObject::g_ConnectDevList->at(q)->dataget[1]==false||GlobalObject::g_ConnectDevList->at(q)->dataget[0]==false){
                qDebug()<<"not two part data get";
                return;
            }
            else{
                break;
            }
        }
    }
    //传电压过来的时候是这样的
    //"NOTIFY", "E5AEFA129DDC", "fff1", "1120387060c8000000100000001101a3000101", ""
    if(multiThread){
        if(false){
            qDebug()<<"isBusy"<<Data_Thread->isBusy();
            //SensorDataConvert(DataInfo_forepart,DataInfo_backend,strList[0]);
        }
        else
        {
            if(GlobalObject::g_ConnectDevList->length()==0){
                qDebug()<<"无名设备data传入";
                return;
            }
            qDebug()<<"convert multithread ";
            DeviceInfo *newdevice=new DeviceInfo;
            int index=0;
            bool find=false;
            newdevice->datainfo=newdevice->datainfo;
            for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
                if(strList[1]==GlobalObject::g_ConnectDevList->at(i)->mac){
                    find=true;
                    index=i;
                    break;
                }
            }
            if(find){
                delete newdevice;
                newdevice=nullptr;
            }
            if(!find){
                qDebug()<<"not find";//这个情况不应该出现的
                if(strList[1].length()==12){
                    newdevice->mac=strList[1];
                    index=GlobalObject::g_ConnectDevList->length();
                    GlobalObject::g_ConnectDevList->push_back(newdevice);
                    AddDevicetoConnectedTree();
                    AddDeviceToNewDeviceTree();
                }
                else{
                    return ;
                }
            }
            QDateTime local(QDateTime::currentDateTime());
            QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
            ui->ComputerTime_label->setText(localTime);
            emit DataNeedtoDeal(GlobalObject::g_ConnectDevList->at(index)->DataInfo_forepart,GlobalObject::g_ConnectDevList->at(index)->DataInfo_backend,GlobalObject::g_ConnectDevList->at(index),index);
        }
    }
    else{
        SensorDataConvert(DataInfo_forepart,DataInfo_backend,strList[1]);
    }
}
void MainWidget::ReceiveFromthread(DeviceInfo *Dinfo, int index){
    //abandon
    GlobalObject::g_ConnectDevList->replace(index,Dinfo);
    if(m_bDatabaseAcvated){
        bool r=insert(GlobalObject::g_ConnectDevList->at(index));
    }
    if(FocusIndex<0){
        //qDebug()<<"Focus Index "<<FocusIndex;
        return;
    }
    //qDebug()<<"focus "<<FocusIndex;
    //DeviceInfo &df=GlobalObject::g_ConnectDevList->at(FocusIndex);
    if(FocusIndex>=GlobalObject::g_ConnectDevList->length()){
        return;
    }
    if(GlobalObject::g_ConnectDevList->length()==1){
        FocusIndex=0;
        emit FocusChanges(FocusIndex);
    }

    if(m_bShowOnCharts){
        DrawCount++;
        if(DrawCount>=DrawMaxCount+1){
            DataBuff_AccX.pop_front();
            DataBuff_AccY.pop_front();
            DataBuff_AccZ.pop_front();
            DataBuff_Tilt1.pop_front();
            DataBuff_Tilt2.pop_front();
            DataBuff_Tilt3.pop_front();
            DataBuff_Distance.pop_front();
            DataBuff_Motion.pop_front();
            DataBuff_IR.pop_front();
            DataBuff_IUV.pop_front();
            //qDebug()<<"Draw Count   "<<DrawCount;
        }
        DataBuff_AccX.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.accelx);
        DataBuff_AccY.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.accely);
        DataBuff_AccZ.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.accelz);
        DataBuff_Tilt1.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.tilt1);
        DataBuff_Tilt2.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.tilt2);
        DataBuff_Tilt3.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.tilt3);

        DataBuff_Distance.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.distance);
        DataBuff_Motion.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.motion);
        DataBuff_IR.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.IR);
        DataBuff_IUV.push_back(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.iUV);
    }
    ShowDataOnTable();
}
void MainWidget::CharacteristicProc(QStringList &strList,int px=0){
    //将设备加入列表
    //"CHARACTERISTIC;E5AEFA129DDC;fff0;fff1;10; ;"
    qDebug()<<"char  "<<strList;
    bool find=false;
    for(int q=0;q<GlobalObject::g_ConnectDevList->length();q++){
        if(strList[1]==GlobalObject::g_ConnectDevList->at(q)->mac){
            find=true;
            GlobalObject::g_ConnectDevList->at(q)->state=CONNECTED;
            if(GlobalObject::g_ConnectDevList->at(q)->name==""){
                for(int m=0;m<ScanDevList.size();m++){
                    if(ScanDevList[m]->mac==strList[1]){
                        GlobalObject::g_ConnectDevList->at(q)->name=ScanDevList[q]->name;
                        GlobalObject::g_ConnectDevList->at(q)->rssi=ScanDevList[q]->rssi;
                        GlobalObject::g_ConnectDevList->at(q)->addrtype=ScanDevList[q]->addrtype;
                        GlobalObject::g_ConnectDevList->at(q)->devPortnum=ScanDevList[q]->devPortnum;
                        ScanDevList.removeAt(m);
                        break;
                    }
                }
            }
            qDebug()<<"find in connecter ";
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
            break;
        }
    }
    if(!find){
        //正常情况下是到这里
        if(strList[1].length()!=12){
            return;
        }
        DeviceInfo *newde=new DeviceInfo;
        newde->mac=strList[1];
        int p=-1;
        bool find=false;
        for(int q=0;q<ScanDevList.size();q++){
            if(ScanDevList[q]->mac==strList[1]){
                p=q;
                newde->name=ScanDevList[q]->name;
                newde->rssi=ScanDevList[q]->rssi;
                newde->addrtype=ScanDevList[q]->addrtype;
                newde->devPortnum=ScanDevList[q]->devPortnum;
                find =true;
                break;
            }
        }
        if(p>=0){
            ScanDevList.removeAt(p);
        }
        //检查是否在DeviceList_Config中
        bool fid=false;
        for(int m=0;m<DeviceList_Config.length();m++){
            if(DeviceList_Config[m]->mac==strList[1]){
                fid=true;
                qDebug()<<"connection from config ";
                GlobalObject::g_ConnectDevList->push_back(DeviceList_Config[m]);
                if(px==0){
                    GlobalObject::g_ConnectDevList->last()->devPortnum=GlobalObject::port_;
                }
                else{
                    GlobalObject::g_ConnectDevList->last()->devPortnum=GlobalObject::port_2;
                }

                delete newde;
                newde=nullptr;
                break;
            }
        }
        if(!fid){
            GlobalObject::g_ConnectDevList->push_back(newde);
        }

        GlobalObject::g_ConnectDevList->last()->state=CONNECTED;
        PromptInfomation("连接"+strList[1]+"成功");
        if(m_bCsv){
            if(GlobalObject::g_ConnectDevList->length()>0){
                if(Csv[GlobalObject::g_ConnectDevList->length()-1]==nullptr){
                    Csv[GlobalObject::g_ConnectDevList->length()-1]=new CsvWriter(GlobalObject::g_ConnectDevList->last()->mac);//绑定写入器
                }
            }
        }
        if(px==0){
            ReadSingelDeviceData(strList[1],GlobalObject::port_);//这个发送了才会开始发送数据
        }
        else{
            ReadSingelDeviceData(strList[1],GlobalObject::port_2);
        }
        if(GlobalObject::g_ConnectDevList->length()==1){
            FocusIndex=0;
            emit FocusChanges(FocusIndex);
        }

    }
    AddDevicetoConnectedTree();
    AddDeviceToNewDeviceTree();


}
void MainWidget::DiscProc(QStringList &info){
    //剩下就是mac了
    //DISC,E5AEFA129DDC,
    PromptInfomation("断开连接"+info[1]);

    for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
        qDebug()<<GlobalObject::g_ConnectDevList->at(i)->mac<<"  "<<info[1];
        if(GlobalObject::g_ConnectDevList->at(i)->mac==info[1]){
            GlobalObject::g_ConnectDevList->removeAt(i);
            qDebug()<<"remove  "<<info[1];
            if(FocusIndex==i){
                FocusIndex=-1;
                emit FocusChanges(FocusIndex);
            }
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
            break;
        }
    }
    //解除绑定

    for(int i=0;i<18;i++){
        if(Csv[i]!=nullptr){
            if(Csv[i]->GetBindMac()==info[1]){
                qDebug()<<"解除绑定";
                delete Csv[i];
                Csv[i]=nullptr;
                qDebug()<<"解除绑定成功";
                break;
            }
        }

    }
}

void MainWidget::ReadDevice(QString mac,quint16 po=0){
    if(mac.length()==12)
    {
        QString str="AT+READCHAR="+mac+","+gRxChar+"\r\n";
        if(m_bTcp_1_con&&po==GlobalObject::port_){
            GlobalObject::Socket_->write(str.toLatin1(),str.length());
        }
        else if(m_bTcp_2_con&&po==GlobalObject::port_2){
            GlobalObject::Socket_2->write(str.toLatin1(),str.length());
        }
    }
}
void MainWidget::CharacteristicofDevice(QString mac, quint16 po){
    if(mac.length()==12)
    {
        QString str="AT+CHAR="+mac+"\r\n";
        if(m_bTcp_1_con&&po==GlobalObject::port_){
            GlobalObject::Socket_->write(str.toLatin1(),str.length());
        }
        else if(m_bTcp_2_con&&po==GlobalObject::port_2){
            GlobalObject::Socket_2->write(str.toLatin1(),str.length());
        }
    }
}
void MainWidget::OtherDeviceMenu(const QPoint& pos){
    qDebug()<<pos;

    QModelIndex curIndex =ui->OtherDevicetree_Wiew->indexAt(pos);
    qDebug()<<curIndex;
    QModelIndex index = curIndex.sibling(curIndex.row(),0);
    //get mac
    QString IndexMac=index.data().toString();
    qDebug()<<IndexMac;
    if(IndexMac.length()!=12){
        qDebug()<<"mac length is not 12 ";
        return;
    }
    //get type
    signed char ty=0;
    quint16 Portnum=0;
    QString name;
    for(int q=0;q<GlobalObject::g_ScanDevList->length();q++){
        if(IndexMac==GlobalObject::g_ScanDevList->at(q)->mac){
            ty=GlobalObject::g_ScanDevList->at(q)->addrtype;
            Portnum=GlobalObject::g_ScanDevList->at(q)->devPortnum;
            name=GlobalObject::g_ScanDevList->at(q)->name;
            break;
        }
    }
    QMenu menu;
    QAction* action1 = new QAction(&menu);
    action1 ->setObjectName("action1");
    action1 ->setText(tr("Connect"));
    QAction* action2= new QAction(&menu);
    action2 ->setObjectName("action2");
    action2 ->setText(tr("Block"));
    menu.addAction(action1);
    menu.addAction(action2);
    QObject::connect(action1,&QAction::triggered,this,[=]{
        ConnectActionTrigged(IndexMac,Portnum,ty);
    });
    QObject::connect(action2,&QAction::triggered,this,[=]{
        PushToblocklist(IndexMac,name);
    });
    menu.exec(ui->OtherDevicetree_Wiew->mapToGlobal(pos));
}
void MainWidget::ConectedDeviceMenu(const QPoint&pos){
    qDebug()<<"ConnectedDeviceMenu";
    QModelIndex curIndex =ui->ConnectedDevice_treeView->indexAt(pos);//索引值
    QModelIndex index = curIndex.sibling(curIndex.row(),0);
    //获取点击设备的mac
    QString IndexMac=index.data().toString();
    QModelIndex index2 = curIndex.sibling(curIndex.row(),1);
    QString Name=index2.data().toString();
    qDebug()<<"test   name  "<<Name<<"MAC "<<IndexMac;
    qDebug()<<"original "<<curIndex;
    //qDebug()<<IndexMac.length();
    if(IndexMac.length()!=12){
        return;
    }
    //获取type
    signed char ty;

    for(int q=0;q<ScanDevList.length();q++){
        if(IndexMac==ScanDevList[q]->mac){
            ty=ScanDevList[q]->addrtype;
        }
    }
    quint16 po=0;
    for(int q=0;q<GlobalObject::g_ConnectDevList->length();q++){
        if(IndexMac==GlobalObject::g_ConnectDevList->at(q)->mac)
            po=GlobalObject::g_ConnectDevList->at(q)->devPortnum;
    }
    QMenu menu;
    QAction* action2 = new QAction(&menu);
    action2 ->setObjectName("action2");
    action2 ->setText(tr("DisConnect"));
    QAction* action3= new QAction(&menu);
    action3 ->setObjectName("action3");
    action3 ->setText(tr("ReName"));
    menu.addAction(action2);
    menu.addAction(action3);
    connect(action2,&QAction::triggered,this,[=]{
        DisconnetDeviceTrigged(IndexMac,po);
    });
    connect(action3,&QAction::triggered,this,[=]{
        DeviceRename(Name,IndexMac);//zhelixuyaoxiugsai
    });
    menu.exec(ui->OtherDevicetree_Wiew->mapToGlobal(pos));
}
void MainWidget::DeviceRename(QString Name,QString Mac){
    // RenemeDeviceList.push_back(Mac);

    ReDialog=new RenameDialog(Name,Mac,this);
    ReDialog->show();
    connect(ReDialog,&RenameDialog::NewName,this,&MainWidget::PushRenametoList);

}
void MainWidget::PushRenametoList(QString Name, QString Mac){
    RenemeDeviceList.push_back(Mac);
    RenemeDeviceList.push_back(Name);
    for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
        if(GlobalObject::g_ConnectDevList->at(i)->mac==Mac){
            GlobalObject::g_ConnectDevList->at(i)->newname=Name;
            qDebug()<<"change the name "<<Name;
            break;
        }
    }
    AddDevicetoConnectedTree();
}
void MainWidget::ConnectActionTrigged(QString Mac,quint16 port,signed char ty){
    //qDebug()<<"trigged!";
    // qDebug()<<Mac<<" "<<ty;

    if(Mac.length()==12)
    {
        QString strMsg="AT+CONN="+Mac+","+QString::number(ty)+"\r\n";//0表示没链接吗
        if(multiThread){
            qDebug()<<"tcp "<<m_bTcp_1_con<<" tcp 2 "<<m_bTcp_2_con;
            qDebug()<<"local port  "<<port<<"glabal port "<<GlobalObject::port_;
            if(port==GlobalObject::port_&&m_bTcp_1_con){
                qDebug()<<"emit  "<<strMsg;
                emit WriteInfo(strMsg,0);
                GetSingleSensorRSSI(Mac,port);
            }
            if(port==GlobalObject::port_2&&m_bTcp_2_con){
                emit WriteInfo(strMsg,1);
                GetSingleSensorRSSI(Mac,port);
            }


        }else{
            if(port==GlobalObject::port_&&m_bTcp_1_con){
                GlobalObject::Socket_->write(strMsg.toLatin1(),strMsg.length());//
                GetSingleSensorRSSI(Mac,port);
            }
            if(port==GlobalObject::port_2&&m_bTcp_2_con){
                GlobalObject::Socket_2->write(strMsg.toLatin1(),strMsg.length());//
                GetSingleSensorRSSI(Mac,port);
            }
        }
        PromptInfomation("正在连接"+Mac);
    }
}
void MainWidget::SysinfoofDevice(QString Mac, quint16 po=0){
    if(Mac.length()==12)
    {
        QString strMsg="AT+SYSINFO?\r\n";
        // qDebug()<<"本机发送断开请求"<<strMsg;
        if(po==GlobalObject::port_&&m_bTcp_1_con){
            GlobalObject::Socket_->write(strMsg.toLatin1(),strMsg.length());//
        }
        else if(m_bTcp_2_con){
            GlobalObject::Socket_2->write(strMsg.toLatin1(),strMsg.length());//
        }
    }
    else{
        qDebug()<<"mac is not 12 bit";
    }

}
void MainWidget::DisconnetDeviceTrigged(QString Mac,quint16 p=0){
    // qDebug()<<"dis connecte";
    if(Mac.length()==12)
    {
        QString strMsg="AT+DISC="+Mac+"\r\n";
        qDebug()<<"本机发送断开请求"<<strMsg;
        if(multiThread){
            if(p==GlobalObject::port_&&m_bTcp_1_con){
                emit WriteInfo(strMsg,0);
            }
            else if(m_bTcp_2_con){
                emit WriteInfo(strMsg,1);
            }
        }
        else{
            if(p==GlobalObject::port_&&m_bTcp_1_con){
                GlobalObject::Socket_->write(strMsg.toLatin1(),strMsg.length());//
            }
            else if(m_bTcp_2_con){
                GlobalObject::Socket_2->write(strMsg.toLatin1(),strMsg.length());//
            }
        }
    }
    else{
        qDebug()<<"mac is not 12 bit";
    }
}
void MainWidget::CirculationTask(int t){
    qDebug()<<"circulation task  "<<t;
    QVector<int>unConectIndex;
    int i,q;
    switch (t) {
    case 0:
        for(i=0;i<GlobalObject::g_ConnectDevList->length();i++){
            if(m_bTcp_1_con){
                if(GlobalObject::g_ConnectDevList->at(i)->devPortnum==GlobalObject::port_){
                    GetSingleSensorRSSI(GlobalObject::g_ConnectDevList->at(i)->mac,GlobalObject::port_);
                    ReadSingelDeviceData(GlobalObject::g_ConnectDevList->at(i)->mac,GlobalObject::port_);
                }
            }
            if(m_bTcp_2_con){
                if(GlobalObject::g_ConnectDevList->at(i)->devPortnum==GlobalObject::port_2){
                    GetSingleSensorRSSI(GlobalObject::g_ConnectDevList->at(i)->mac,GlobalObject::port_2);
                    ReadSingelDeviceData(GlobalObject::g_ConnectDevList->at(i)->mac,GlobalObject::port_2);
                }
            }
        }
        break;
    case 1:
        qDebug()<<"unConectIndex ";
        for(q=0;q<DeviceList_Config.length();q++){
            bool fid=false;
            for(i=0;i<GlobalObject::g_ConnectDevList->length();i++){
                qDebug()<<"q  "<<q<<"  i  "<<i<<" max "<<DeviceList_Config.length()<<"  maxx 2  "<<GlobalObject::g_ConnectDevList->length();
                if(GlobalObject::g_ConnectDevList->at(i)->mac==DeviceList_Config[q]->mac){
                    fid=true;
                    break;
                }
            }
            if(!fid){

                unConectIndex.push_back(q);
            }
        }


        for(int i=0;i<unConectIndex.length();i++){
            qDebug()<<"connect NO"<<i;
            if(m_bTcp_1_con){
                ConnectActionTrigged(DeviceList_Config[unConectIndex[i]]->mac,GlobalObject::port_,1);
            }
            if(m_bTcp_2_con){
                ConnectActionTrigged(DeviceList_Config[unConectIndex[i]]->mac,GlobalObject::port_2,1);
            }
        }
        break;
    case 3:
        emit ReconnectDevice();
        break;
    case 2:
        BtnDeviceScan();
        QString Countshow=QString(tr("扫描设备数量: %1;上限: %2")).arg(ScanDevList.size()).arg(MAX_SCAN_COUNT);
        ui->DeviceCount_label->setText(Countshow);
        break;

    }
}
void MainWidget::TimerTimerOut(){
    qDebug()<<"timer time out";
    // DisconnetDeviceTrigged(testuseMac);

}
void MainWidget::UpdateTreeView(){
    if(model_Con!=nullptr){

        ui->ConnectedDevice_treeView->setModel(model_Con);
        //ui->ConnectedDevice_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    }
    if(model_Scan!=nullptr){

        ui->OtherDevicetree_Wiew->setModel(model_Scan);
        //ui->OtherDevicetree_Wiew->setDragDropMode(QAbstractItemView::DropOnly);
    }
}
void MainWidget::AddDevicetoConnectedTree(){
    if(multiThread){
        GlobalObject::locker->lock();
        if(GlobalObject::g_ConnectDevList->size()>=0){

            if(model_Con!=nullptr){
                delete model_Con;
                model_Con=nullptr;

            }
            model_Con = new QStandardItemModel(ui->ConnectedDevice_treeView);
            model_Con->setHorizontalHeaderLabels(QStringList()<<tr("MAC")<<tr("name"));

            if(GlobalObject::port_!=0&&m_bTcp_1_con){
                QStandardItem* item_port_1 = new QStandardItem(QString::number(GlobalObject::port_));
                model_Con->appendRow(item_port_1);
                item_port_1->setEditable(false);

                for(int i=0;i<GlobalObject::g_ConnectDevList->size();i++){
                    if(GlobalObject::g_ConnectDevList->at(i)->devPortnum==GlobalObject::port_){
                        QStandardItem* itemchannel = new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->mac);
                        item_port_1->appendRow(itemchannel);
                        if(GlobalObject::g_ConnectDevList->at(i)->newname==""){
                            item_port_1->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->name));
                        }
                        else{
                            item_port_1->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->newname));
                        }

                        if(GlobalObject::g_ConnectDevList->at(i)->m_bIntime){
                            QIcon discicon(":/new/logo/rsc/connect.ico");
                            item_port_1->child(i)->setIcon(discicon);
                        }else
                        {
                            QIcon discicon(":/new/logo/rsc/disc.ico");
                            item_port_1->child(i)->setIcon(discicon);
                        }
                        item_port_1->setEditable(false);
                    }

                }
            }
            if(GlobalObject::port_2!=0&&m_bTcp_2_con){
                QStandardItem* item_port_2 = new QStandardItem(QString::number(GlobalObject::port_2));
                model_Con->appendRow(item_port_2);
                for(int i=0;i<GlobalObject::g_ConnectDevList->size();i++){
                    if(GlobalObject::g_ConnectDevList->at(i)->devPortnum==GlobalObject::port_2){
                        QStandardItem* itemchannel = new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->mac);
                        item_port_2->appendRow(itemchannel);
                        if(GlobalObject::g_ConnectDevList->at(i)->newname==""){
                            item_port_2->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->name));
                        }
                        else{
                            item_port_2->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->newname));
                        }
                        item_port_2->setEditable(false);
                    }

                }


            }
        }

        QString Countshow=QString("连接设备数量: %1;上限: %2").arg(GlobalObject::g_ConnectDevList->size()).arg(MAX_DEV_COUNT);
        ui->DeviceCount_label->setText(Countshow);
        UpdateTreeView();



        GlobalObject::locker->unlock();
        return;
    }



    if(GlobalObject::g_ConnectDevList->size()>=0){

        if(model_Con!=nullptr){
            delete model_Con;
            model_Con=nullptr;

        }
        model_Con = new QStandardItemModel(ui->ConnectedDevice_treeView);
        model_Con->setHorizontalHeaderLabels(QStringList()<<tr("MAC")<<tr("name"));

        if(GlobalObject::port_!=0&&m_bTcp_1_con){
            QStandardItem* item_port_1 = new QStandardItem(QString::number(GlobalObject::port_));
            model_Con->appendRow(item_port_1);
            item_port_1->setEditable(false);

            for(int i=0;i<GlobalObject::g_ConnectDevList->size();i++){
                if(GlobalObject::g_ConnectDevList->at(i)->devPortnum==GlobalObject::port_){
                    QStandardItem* itemchannel = new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->mac);
                    item_port_1->appendRow(itemchannel);
                    if(GlobalObject::g_ConnectDevList->at(i)->newname==""){
                        item_port_1->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->name));
                    }
                    else{
                        item_port_1->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->newname));
                    }

                    if(GlobalObject::g_ConnectDevList->at(i)->m_bIntime){
                        QIcon discicon(":/new/logo/rsc/connect.ico");
                        item_port_1->child(i)->setIcon(discicon);
                    }else
                    {
                        QIcon discicon(":/new/logo/rsc/disc.ico");
                        item_port_1->child(i)->setIcon(discicon);
                    }
                    item_port_1->setEditable(false);
                }

            }
        }
        if(GlobalObject::port_2!=0&&m_bTcp_2_con){
            QStandardItem* item_port_2 = new QStandardItem(QString::number(GlobalObject::port_2));
            model_Con->appendRow(item_port_2);
            for(int i=0;i<GlobalObject::g_ConnectDevList->size();i++){
                if(GlobalObject::g_ConnectDevList->at(i)->devPortnum==GlobalObject::port_2){
                    QStandardItem* itemchannel = new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->mac);
                    item_port_2->appendRow(itemchannel);
                    if(GlobalObject::g_ConnectDevList->at(i)->newname==""){
                        item_port_2->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->name));
                    }
                    else{
                        item_port_2->setChild(itemchannel->index().row(),1,new QStandardItem(GlobalObject::g_ConnectDevList->at(i)->newname));
                    }
                    item_port_2->setEditable(false);
                }

            }


        }
    }

    QString Countshow=QString("连接设备数量: %1;上限: %2").arg(GlobalObject::g_ConnectDevList->size()).arg(MAX_DEV_COUNT);
    ui->DeviceCount_label->setText(Countshow);
    UpdateTreeView();
}

void MainWidget::AddDeviceToNewDeviceTree(){
    //scan and add the device to  ScanDevList

    if(multiThread){
        GlobalObject::locker->lock();
        if(GlobalObject::g_ScanDevList->size()>=0){

            if(model_Scan!=nullptr){
                delete model_Scan;
                model_Scan=nullptr;

            }
            model_Scan= new QStandardItemModel(ui->OtherDevicetree_Wiew);
            model_Scan->setColumnCount(2);
            model_Scan->setHorizontalHeaderLabels(QStringList()<<tr("MAC")<<tr("name"));
            for(int i=0;i<GlobalObject::g_ScanDevList->size();i++){
                QStandardItem* itemMac = new QStandardItem(GlobalObject::g_ScanDevList->at(i)->mac);
                itemMac->setEditable(false);
                QStandardItem* itemname = new QStandardItem(GlobalObject::g_ScanDevList->at(i)->name);
                itemname->setEditable(false);
                model_Scan->setItem(i,0,itemMac);
                model_Scan->setItem(i,1,itemname);

            }

            QString Countshow=QString("扫描设备数量: %1/%2").arg(GlobalObject::g_ScanDevList->size()).arg(MAX_SCAN_COUNT);
            ui->DeviceCount_label->setText(Countshow);
            UpdateTreeView();
        }
        GlobalObject::locker->unlock();
        return;
    }

    if(ScanDevList.size()>=0){

        if(model_Scan!=nullptr){
            delete model_Scan;
            model_Scan=nullptr;

        }
        model_Scan= new QStandardItemModel(ui->OtherDevicetree_Wiew);
        model_Scan->setColumnCount(2);
        model_Scan->setHorizontalHeaderLabels(QStringList()<<tr("MAC")<<tr("name"));
        for(int i=0;i<ScanDevList.size();i++){
            QStandardItem* itemMac = new QStandardItem(ScanDevList[i]->mac);
            itemMac->setEditable(false);
            QStandardItem* itemname = new QStandardItem(ScanDevList[i]->name);
            itemname->setEditable(false);
            model_Scan->setItem(i,0,itemMac);
            model_Scan->setItem(i,1,itemname);

        }

        QString Countshow=QString(tr("扫描设备数量: %1/%2")).arg(ScanDevList.size()).arg(MAX_SCAN_COUNT);
        ui->DeviceCount_label->setText(Countshow);
        UpdateTreeView();
    }

}
void MainWidget::PromptInfomation(QString str){
    ui->Prompt_Message_label->setText(str);
}
void MainWidget::BtnDeviceScan(){
    if(!m_bTCPConnected)
    {
        QMessageBox::information(this,"网络未连接,请稍候...","提示");
        return;
    }

    InquireDev(1,1,INQUIRE_SECONDS);
    // m_bSearching = true;
    PromptInfomation("正在搜索周围的传感器...");
}
void MainWidget::ItemBeClicked(const QModelIndex &index){
    QModelIndex info = index.sibling(index.row(),0);
    //get mac
    QString IndexMac=info.data().toString();
    qDebug()<<IndexMac.length()<<index;
    if(IndexMac.length()!=12){
        return;
    }
    int OriFocus=FocusIndex;
    //GetSingleSensorRSSI(IndexMac);
    bool find=false;
    for(int q=0;q<GlobalObject::g_ConnectDevList->size();q++){
        if(GlobalObject::g_ConnectDevList->at(q)->mac==IndexMac){
            FocusIndex=q;
            emit FocusChanges(FocusIndex);
            FocusDevice=GlobalObject::g_ConnectDevList->at(q);
            find=true;
            break;
        }
    }
    if(!find){
        FocusIndex=-1;
        emit FocusChanges(FocusIndex);
    }
    if(OriFocus!=-1&&OriFocus!=FocusIndex){
        DataBuff_AccX.clear();
        DataBuff_AccY.clear();
        DataBuff_AccZ.clear();
        DataBuff_Tilt1.clear();
        DataBuff_Tilt2.clear();
        DataBuff_Tilt3.clear();
        DataBuff_IUV.clear();
        DataBuff_AccX.clear();
        DataBuff_Motion.clear();
        DataBuff_Distance.clear();
        DrawCount=0;
    }
}
void MainWidget::PrepareInquire()
{
    int i;
    int maxcount=static_cast<int>(MAX_DEV_COUNT);
    for(i = 0;i <maxcount;i++)
    {
        if(GlobalObject::g_ConnectDevList->isEmpty()){
            break;
        }
        if(!BdaddrIsZero(GlobalObject::g_ConnectDevList->at(i)->mac)&&GlobalObject::g_ConnectDevList->at(i)->state==ADVERTISING)
        {
            GlobalObject::g_ConnectDevList->at(i)->state = POWER_OFF;
            GlobalObject::g_ConnectDevList->at(i)->connfailcount = 0;
        }
    }
}
void MainWidget::InquireDev(unsigned char action,unsigned char mod,unsigned char time)
{
    char cmdstr[64];
    if(!m_bTCPConnected){
        qDebug()<<"m_bsearching "<<m_bSearching<<" m_connected "<<m_bTCPConnected;
        return;
    }
    if(m_bTCPConnected)
    {
        sprintf(cmdstr,"AT+INQ=%d,%d,%d\r\n",action,mod,time);
        qDebug()<<cmdstr<<"inquire send";
        size_t len=strlen(cmdstr);
        qDebug()<<"port1 ";
        if(multiThread){
            qDebug()<<"debug 1";
            if(GlobalObject::port_!=0&&m_bTcp_1_con){
                qDebug()<<"debug 2";
                emit WriteInfo(cmdstr,0);
            }

            if(GlobalObject::port_2!=0&&m_bTcp_2_con){
                emit WriteInfo(cmdstr,1);
            }
        }
        else{
            if(GlobalObject::port_!=0&&m_bTcp_1_con){
                GlobalObject::Socket_->write(cmdstr,static_cast<int>(len));
            }
            qDebug()<<"port2 ";
            if(GlobalObject::port_2!=0&&m_bTcp_2_con){
                GlobalObject::Socket_2->write(cmdstr,static_cast<int>(len));
            }
        }





        if(action == 1)
        {
            PromptInfomation("正在搜索传感器设备...");
        }
        //m_bSearching=true;
    }
}
bool MainWidget::BdaddrIsZero(QString addr)
{
    int i;
    char*  ch;
    QByteArray ba = addr.toLatin1(); // must
    ch=ba.data();
    for(i = 0; i < 12; i++)
    {
        if(ch[i]!= 0)
        {
            return false;
        }
    }
    return true;
}
void MainWidget::on_SerialPortConfig_pushButton_clicked()
{
    if(spd==nullptr){
        spd=new SerialPortDialog(this);
        connect(spd,&SerialPortDialog::serialOpened,this,&MainWidget::SerailState);
        connect(spd,&SerialPortDialog::close_event,this,[=]{
            spd->hide();
            qDebug()<<"hide the serialportdialog window! not destory it";
        });
        spd->show();
    }
    else{
        spd->show();
    }

}
void MainWidget::SerailState(QString name1,QString Name2){
    ui->Serial_1_stateLabel->setText(name1);
    ui->Serial_2_stateLabel->setText(Name2);
    if(name1 != nullptr)
    {
        port1_send_oder_flg = 1;
    }
    if(Name2 != nullptr)
    {
        port2_send_oder_flg = 1;
    }
}
void MainWidget::FileOutput(QString str, QString add="", int method=0, DeviceInfo *df=nullptr,CsvWriter *writer=nullptr){
    //method ==0  覆盖
    //method ！=0 追加
    if(df!=nullptr&&writer!=nullptr){
        qDebug()<<"if case";
        QString Outstr;
        //"Time,Mac,Name,AccX,AccY,AccZ,alpha,beta,gama,motion,IR,iuv,distance,Intensity,temperture\n""
        Outstr+=df->time+","+df->mac+","+df->name+","+QString::number(df->datainfo.accelx)+
                ","+QString::number(df->datainfo.accely)+","+QString::number(df->datainfo.accelz)+","+
                QString::number(df->datainfo.tilt1)+","+QString::number(df->datainfo.tilt2)+","
                +QString::number(df->datainfo.tilt3)+","+QString::number(df->datainfo.motion)+
                ","+QString::number(df->datainfo.IR)+","+QString::number(df->datainfo.iUV)+","+
                QString::number(df->datainfo.distance)+","+QString::number(df->datainfo.intensity)+
                ","+QString::number(df->datainfo.temperature)+"\n";


        qDebug()<<"write  "<<Outstr;
        if(writer->m_bOpen==true){

            *(writer->GetOut())<<Outstr;
        }
    }
    else {
        qDebug()<<"else case ";
        if(add.isEmpty()){
            //   QMessageBox::information(NULL,"warning","no out put file address!");
            add="./outputfiles"+QString::number(qrand())+".txt";
        }

        QFile *outflie=new QFile;
        outflie->setFileName(add);
        bool ok=outflie->open(QIODevice::Text|QIODevice::ReadWrite);//加入QIODevice：：Text可以换行
        if(ok)
        {
            QTextStream out(outflie);

            if(method==0){
                out<<str;
            }
            else{
                QString Orifile;
                while(!outflie->atEnd())
                {
                    QByteArray line = outflie->readLine();
                    QString str=tr(line);

                    Orifile.append(str);

                }
                Orifile.append(str);
                out<<Orifile;
            }
        }
        outflie->close();
        delete outflie;
    }

}
void MainWidget::on_Device_tabWidget_tabBarClicked(int index)
{
    //切换到搜索列表就进行搜索

    if(index==1){//未连接列表
        if(!m_bTCPConnected){
            return;
        }
        PromptInfomation("Searching List clicked!");
        //开始搜索设备
        BtnDeviceScan();
        QString Countshow=QString(tr("扫描设备数量: %1;上限: %2")).arg(ScanDevList.size()).arg(MAX_SCAN_COUNT);
        ui->DeviceCount_label->setText(Countshow);
    }
    else{
        //这个页面则是数据提取
        PromptInfomation("Connected List clicked!");
        QString Countshow=QString(tr("连接设备数量: %1;上限: %2")).arg(GlobalObject::g_ConnectDevList->size()).arg(MAX_DEV_COUNT);
        ui->DeviceCount_label->setText(Countshow);

    }
}

void MainWidget::ReadSingelDeviceData(QString Mac,quint16 po){
    if(Mac.length()==12)
    {

        if(po==GlobalObject::port_){
            SendDatatoDev(Mac,gRxChar,"012001",0);
        }
        else{
            SendDatatoDev(Mac,gRxChar,"012001",1);
        }

    }
}
void MainWidget::GetSingleSensorRSSI(QString Mac,quint16 po=0){
    if(Mac.length()==12)
    {
        QString inqRssi="AT+RSSI="+Mac+"\r\n";
        if(multiThread){
            if(po==GlobalObject::port_&&m_bTcp_1_con){
                emit WriteInfo(inqRssi,0);
            }
            else if(m_bTcp_2_con){
                emit WriteInfo(inqRssi,1);
            }
        }
        else{

            if(po==GlobalObject::port_&&m_bTcp_1_con){

                GlobalObject::Socket_->write(inqRssi.toLatin1(),inqRssi.length());
            }
            else if(m_bTcp_2_con){
                GlobalObject::Socket_2->write(inqRssi.toLatin1(),inqRssi.length());
            }
        }

    }
}
void MainWidget::SendDatatoDev(QString Mac,QString charc,QString dat,int po=0){
    if(Mac.length()==12)
    {

        QString sendData="AT+WRITECHAR="+Mac+","+charc+","+dat+"\r\n";
        if(multiThread){
            if(po==0&&m_bTcp_1_con){
                emit WriteInfo(sendData,0);
            }
            else if(m_bTcp_2_con){
                emit WriteInfo(sendData,1);
            }
        }
        else{
            if(po==0&&m_bTcp_1_con){
                GlobalObject::Socket_->write(sendData.toLatin1(),sendData.length());
            }
            else if(m_bTcp_2_con){
                GlobalObject::Socket_2->write(sendData.toLatin1(),sendData.length());
            }
        }
    }

}
void MainWidget::on_Exit_pushButton_clicked()
{
    qDebug()<<"exit ";
    this->close();

}
void MainWidget::ShowDataOnTable(DeviceInfo df){

    ui->AccX_label->setText(QString::number(df.datainfo.accelx));
    ui->AccY_label->setText(QString::number(df.datainfo.accely));
    ui->AccZ_label->setText(QString::number(df.datainfo.accelz));
    ui->Alpha_label->setText(QString::number(static_cast<double>(df.datainfo.tilt1)));
    ui->Beta_label->setText(QString::number(static_cast<double>(df.datainfo.tilt2)));
    ui->Gama_label->setText(QString::number(static_cast<double>(df.datainfo.tilt3)));
    ui->ComputerTime_label->setText(df.time);
    ui->DataVoltage_label->setText(QString::number(df.voltage));
    ui->DeviceMac_label->setText(df.mac);
    ui->DeviceMode_label->setText(QString(df.mode));
    ui->Temperature_label->setText(QString::number(df.datainfo.temperature));
    ui->FaultCode_label->setText(QString::number(df.faultcode));
    ui->DeviceName_label->setText(df.name);
    ui->IR_label->setText(QString::number(df.datainfo.IR));
    ui->Fault_label->setText(df.fault);
    ui->Movement_label->setText(QString::number(df.datainfo.motion));
}
void MainWidget::ShowDataOnTable(){
    if(windowLocked){
        return;
    }
    if(FocusIndex<0){
        qDebug()<<"Focus Index "<<FocusIndex;
        return;
    }
    qDebug()<<"focus "<<FocusIndex;
    //DeviceInfo &df=GlobalObject::g_ConnectDevList->at(FocusIndex);
    if(FocusIndex>=GlobalObject::g_ConnectDevList->length()){
        return;
    }
    if(GlobalObject::g_ConnectDevList->length()==1){
        FocusIndex=0;
        emit FocusChanges(FocusIndex);
    }
    ui->ComputerTime_label->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss"));
    ui->AccX_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.accelx));
    ui->AccY_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.accely));
    ui->AccZ_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.accelz));
    ui->Alpha_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.tilt1));
    ui->Beta_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.tilt2));
    ui->Gama_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.tilt3));
    ui->DeviceTime_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->time);
    ui->DataVoltage_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->voltage));
    ui->DeviceMac_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->mac);
    ui->DeviceMode_label->setText(QString(GlobalObject::g_ConnectDevList->at(FocusIndex)->mode));
    ui->Temperature_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.temperature));
    ui->FaultCode_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->faultcode));
    ui->LastDataTimeStamp_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->lastDataTimeStamp.toString(("yyyy-MM-dd:hh:mm:ss")));
    //ui->DeviceName_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->name);
    if(GlobalObject::g_ConnectDevList->at(FocusIndex)->newname==""){
        ui->DeviceName_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->name);

    }
    else{
        ui->DeviceName_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->newname);
    }

    ui->IR_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.IR));
    ui->Fault_label->setText(GlobalObject::g_ConnectDevList->at(FocusIndex)->fault);
    ui->Movement_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.motion));
    ui->IUV_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.iUV));
    ui->Distance_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.distance));
    ui->DeviceSignal_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->rssi));
    ui->Light_label->setText(QString::number(GlobalObject::g_ConnectDevList->at(FocusIndex)->datainfo.intensity));



    ChartDrawer();
}
void MainWidget::on_DataConfig_pushButton_clicked()
{
    DataConDialog=new DataConfigDialog(GlobalObject::g_ConnectDevList,this);
    connect(DataConDialog,&DataConfigDialog::QueryData,DB_T,&DataBase_thread::QueryData);
    connect(DB_T,&DataBase_thread::QueryReturn,DataConDialog,&DataConfigDialog::GetFromDataBase);
    DataConDialog->show();
}
int MainWidget::SignalHexToDec(QString t){
    int sec=0;
    int len=t.length();
    for(int q=1;q<len;q++){
        switch (t[q].toLatin1()) {
        case 'a':
            sec+=10*pow(16,t.length()-q-2);
            break;
        case 'b':
            sec+=11*pow(16,t.length()-q-2);
            break;
        case 'c':
            sec+=12*pow(16,t.length()-q-2);
            break;
        case 'd':
            sec+=13*pow(16,t.length()-q-2);
            break;
        case 'e':
            sec+=14*pow(16,t.length()-q-2);
            break;
        case 'f':
            sec+=15*pow(16,t.length()-q-2);
            break;
        default:
            sec+=(static_cast<int>(t[q].toLatin1())-48)*pow(16,t.length()-q-2);
            break;
        }
    }

    return -sec;


}
unsigned int MainWidget::HexToDec(QString t){
    unsigned int sec=0;
    for(int q=0;q<t.length();q++){
        switch (t[q].toLatin1()) {
        case 'a':
            sec+=10*pow(16,t.length()-q-1);
            break;
        case 'b':
            sec+=11*pow(16,t.length()-q-1);
            break;
        case 'c':
            sec+=12*pow(16,t.length()-q-1);
            break;
        case 'd':
            sec+=13*pow(16,t.length()-q-1);
            break;
        case 'e':
            sec+=14*pow(16,t.length()-q-1);
            break;
        case 'f':
            sec+=15*pow(16,t.length()-q-1);
            break;
        default:
            sec+=(static_cast<int>(t[q].toLatin1())-48)*pow(16,t.length()-q-1);
            break;
        }
    }
    return sec;
}
QString MainWidget::HexTimeToDec(QByteArray t){
    unsigned int sec=0;
    for(int q=0;q<t.length();q++){
        switch (t[q]) {
        case 'a':
            sec+=10*pow(16,7-q);
            break;
        case 'b':
            sec+=11*pow(16,7-q);
            break;
        case 'c':
            sec+=12*pow(16,7-q);
            break;
        case 'd':
            sec+=13*pow(16,7-q);
            break;
        case 'e':
            sec+=14*pow(16,7-q);
            break;
        case 'f':
            sec+=15*pow(16,7-q);
            break;
        default:
            sec+=(static_cast<int>(t[q])-48)*pow(16,7-q);
            break;
        }
    }
    QDateTime dt = QDateTime::fromTime_t (sec);
    QString strDate = dt.toString("yyyy-MM-dd:hh:mm:ss");
    return strDate;
}
void MainWidget::on_Synchronization_pushButton_clicked()
{
    if(GlobalObject::g_ConnectDevList->length()>0){
        for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
            QDateTime local(QDateTime::currentDateTime());
            QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
            unsigned int sec=local.toTime_t();
            QString timebuf[4];
            QString meg;
            timebuf[0] = QString::number((sec>>24)&0xFF,16).toUpper();
            timebuf[1] = QString::number((sec>>16)&0xFF,16).toUpper();
            timebuf[2] = QString::number((sec>>8)&0xFF,16).toUpper();
            timebuf[3] = QString::number(sec&0xFF,16).toUpper();
            meg=QString("0460%1%2%3%4").arg(timebuf[0])
                    .arg(timebuf[1]).arg(timebuf[2])
                    .arg(timebuf[3]);
            SendDatatoDev(GlobalObject::g_ConnectDevList->at(i)->mac,gRxChar,meg);

            //QString infoDevi="012001";
            ReadSingelDeviceData(GlobalObject::g_ConnectDevList->at(i)->mac,GlobalObject::g_ConnectDevList->at(i)->devPortnum);

        }

    }

}
void MainWidget::ChartDrawer(){
    //ui->Acceler_Chart->setRenderHint(QPainter::Antialiasing);
    if(windowLocked){
        return;
    }
    if(chart[0]==nullptr){
        chart[0]=new QChart;
    }
    if(chart[1]==nullptr){
        chart[1]=new QChart;
    }
    if(series[0]==nullptr){
        series[0]=new QLineSeries;
    }
    if(series[1]==nullptr){
        series[1]=new QLineSeries;
    }
    if(series[2]==nullptr){
        series[2]=new QLineSeries;
    }
    if(series[3]==nullptr){
        series[3]=new QLineSeries;
    }
    if(series[4]==nullptr){
        series[4]=new QLineSeries;
    }
    if(series[5]==nullptr){
        series[5]=new QLineSeries;
    }
    if(Distancechart==nullptr){
        Distancechart=new QChart;
        Distancechart->setMargins(QMargins(0,0,0,0));
    }
    if(IRchart==nullptr){
        IRchart=new QChart;
        IRchart->setMargins(QMargins(0,0,0,0));
    }
    if(IUVchart==nullptr){
        IUVchart=new QChart;
        IUVchart->setMargins(QMargins(0,0,0,0));
    }
    if(Motionchart==nullptr){
        Motionchart=new QChart;

    }
    if(Distanceseries==nullptr){
        Distanceseries=new QLineSeries;
    }
    if(IRseries==nullptr){
        IRseries=new QLineSeries;
    }
    if(IUVseries==nullptr){
        IUVseries=new QLineSeries;
    }
    if(Motionseries==nullptr){
        Motionseries=new QLineSeries;
    }
    qDebug()<<"add series ";
    chart[0]->addSeries(series[0]);
    chart[0]->addSeries(series[1]);
    chart[0]->addSeries(series[2]);
    Distancechart->addSeries(Distanceseries);
    IRchart->addSeries(IRseries);
    IUVchart->addSeries(IUVseries);
    Motionchart->addSeries(Motionseries);
    chart[1]->addSeries(series[3]);
    chart[1]->addSeries(series[4]);
    chart[1]->addSeries(series[5]);
    chart[0]->createDefaultAxes();
    chart[0]->axisX()->setRange(0,20);
    chart[0]->axisY()->setRange(-1200,1200);
    chart[0]->setMargins(QMargins(0,0,0,0));//铺满
    //chart[0]->setAnimationOptions(QChart::SeriesAnimations);  //开启会使绘制变成动画效果，但是不适合移动窗口
    chart[1]->createDefaultAxes();
    chart[1]->axisX()->setRange(0,20);
    chart[1]->axisY()->setRange(-1200,1200);
    chart[1]->setMargins(QMargins(0,0,0,0));//铺满
    // chart[1]->setAnimationOptions(QChart::SeriesAnimations);  //在缩放窗口大小时可以用动画进行美化


    ui->Acceler_Chart->setAutoFillBackground(true);

    series[0]->setColor(Qt::red);
    series[1]->setColor(Qt::blue);
    series[2]->setColor(Qt::green);

    series[3]->setColor(Qt::red);
    series[4]->setColor(Qt::blue);
    series[5]->setColor(Qt::green);
    Distanceseries->setColor(Qt::red);
    IRseries->setColor(Qt::red);
    IUVseries->setColor(Qt::red);
    Motionseries->setColor(Qt::red);
    series[0]->clear();
    series[1]->clear();
    series[2]->clear();
    series[3]->clear();
    series[4]->clear();
    series[5]->clear();
    series[0]->setName("AccX");
    series[1]->setName("AccY");
    series[2]->setName("AccZ");
    series[3]->setName("Tilt 1");
    series[4]->setName("Tilt 2");
    series[5]->setName("Tilt 3");
    Motionseries->setName("motion");
    Distanceseries->setName("Distance");
    IUVseries->setName("iUV");
    IRseries->setName("IR");
    qDebug()<<"set color and clear";
    Distanceseries->clear();
    IRseries->clear();
    IUVseries->clear();
    Motionseries->clear();
    qDebug()<<"get data";
    Distancechart->createDefaultAxes();
    IRchart->createDefaultAxes();
    IUVchart->createDefaultAxes();
    Motionchart->createDefaultAxes();

    short DisMax=0;
    short IRMax=0;
    short IUVMax=0;
    short MotMax=0;
    for(int i=0; i<DataBuff_AccX.length();i++)
    {
        series[0]->append(i,DataBuff_AccX[i]);
        series[1]->append(i,DataBuff_AccY[i]);
        series[2]->append(i,DataBuff_AccZ[i]);
        series[3]->append(i,DataBuff_Tilt1[i]);
        series[4]->append(i,DataBuff_Tilt2[i]);
        series[5]->append(i,DataBuff_Tilt3[i]);
        Distanceseries->append(i,DataBuff_Distance[i]);
        IRseries->append(i,DataBuff_IR[i]);;
        IUVseries->append(i,DataBuff_IUV[i]);
        Motionseries->append(i,DataBuff_Motion[i]);

        if(DataBuff_Distance[i]>DisMax)
            DisMax=DataBuff_Distance[i];
        if(DataBuff_IR[i]>IRMax)
            IRMax=DataBuff_IR[i];
        if(DataBuff_IUV[i]>IUVMax)
            IUVMax=DataBuff_IUV[i];
        if(DataBuff_Motion[i]>MotMax)
            MotMax=DataBuff_Motion[i];


    }
    IRchart->axisY()->setRange(0,IRMax);
    Motionchart->axisY()->setRange(0,MotMax);
    IUVchart->axisY()->setRange(0,IUVMax);
    Distancechart->axisY()->setRange(0,DisMax);

    IRchart->axisX()->setRange(0,20);
    Motionchart->axisX()->setRange(0,20);
    IUVchart->axisX()->setRange(0,20);
    Distancechart->axisX()->setRange(0,20);
    Motionchart->setMargins(QMargins(0,0,0,0));
    Distancechart->setMargins(QMargins(0,0,0,0));

    IRchart->setMargins(QMargins(0,0,0,0));
    IUVchart->setMargins(QMargins(0,0,0,0));
    ui->Acceler_Chart->setChart(chart[0]);
    ui->Tilt_Chart->setChart(chart[1]);
    ui->IR_Chart->setChart(IRchart);
    ui->IUV_Chart->setChart(IUVchart);
    ui->Motion_Chart->setChart(Motionchart);
    ui->Distance_Chart->setChart(Distancechart);
}
void MainWidget::InsertDatatoDB(DeviceInfo df){
    // query(Db);
    //QString datestr = ui->dateEdit->dateTime().toString("dd-MM-yyyy");
    // QString timestr = ui->dateEdit->time().toString("hh:mm:ss");
    /*QString sql=QString("select *from information");
    query.exec(sql);
    if(query.numRowsAffected() == 0)
    {
        QString savesql = QString("INSERT INTO information(userName,IP,storagePath,productName,date)");
         savesql += QString(" VALUES('%1','%2','%3','%4','%5')").arg(ui->userNameEdit->text())
                .arg(ui->ipAddressEdit->text())
                .arg(ui->storagePathEdit->text())
                .arg(ui->productNameEdit->text())
                .arg(datestr+' '+timestr);
        bool ok=query.exec(savesql);
        if(ok){
            QMessageBox::about(NULL, "Save", "save new database success");
        }
        else{
            QMessageBox::about(NULL, "Save", "error save new database");
        }
    }*/

}
void MainWidget::ReadConfigFile(){
    if(configIniWriter!=nullptr){
        PromptInfomation("Load config files....");
        int c=configIniWriter->value("General/DeviceCount").toString().toInt();
        Csvpath=configIniWriter->value("General/CSVpath").toString();
        if(Csvpath.length()==0)
            Csvpath=QCoreApplication::applicationDirPath()+"/";
        CsvUpperAddr=Csvpath;
        for(int i=0;i<c;i++){


            //NeedToConnected.push_back(mac);
            DeviceInfo *newdev=new DeviceInfo();
            newdev->mac=configIniWriter->value("Device"+QString::number(i)+"/Mac").toString();
            newdev->name=configIniWriter->value("Device"+QString::number(i)+"/Name").toString();
            newdev->newname=configIniWriter->value("Device"+QString::number(i)+"/NewName").toString();
            newdev->rssi=configIniWriter->value("/Device"+QString::number(i)+"/RSSI").toString().toShort();
            newdev->Corr_Coe.CorrAccX=configIniWriter->value("/Device"+QString::number(i)+"/CorrAccX").toString().toShort();
            newdev->Corr_Coe.CorrAccY=configIniWriter->value("/Device"+QString::number(i)+"/CorrAccY").toString().toShort();
            newdev->Corr_Coe.CorrAccZ=configIniWriter->value("/Device"+QString::number(i)+"/CorrAccZ").toString().toShort();
            newdev->Corr_Coe.CorrAlpha=configIniWriter->value("/Device"+QString::number(i)+"/CorrAlpha").toString().toShort();
            newdev->Corr_Coe.CorrBeta=configIniWriter->value("/Device"+QString::number(i)+"/CorrBeta").toString().toShort();
            newdev->Corr_Coe.CorrGama=configIniWriter->value("/Device"+QString::number(i)+"/CorrGama").toString().toShort();
            newdev->Corr_Coe.CorrDistanceA=configIniWriter->value("/Device"+QString::number(i)+"/CorrDistanceA").toString().toShort();
            newdev->Corr_Coe.CorrDistanceB=configIniWriter->value("/Device"+QString::number(i)+"/CorrDistanceB").toString().toShort();
            newdev->Corr_Coe.CorrDistanceC=configIniWriter->value("/Device"+QString::number(i)+"/CorrDistanceC").toString().toShort();
            newdev->Corr_Coe.CorrLight=configIniWriter->value("/Device"+QString::number(i)+"/CorrLight").toString().toShort();
            newdev->Corr_Coe.CorrIR=configIniWriter->value("/Device"+QString::number(i)+"/CorrIR").toString().toShort();
            newdev->Corr_Coe.CorrTempA=configIniWriter->value("/Device"+QString::number(i)+"/CorrTempA").toString().toShort();
            newdev->Corr_Coe.CorrTempB=configIniWriter->value("/Device"+QString::number(i)+"/CorrTempB").toString().toShort();
            newdev->Corr_Coe.CorrTempC=configIniWriter->value("/Device"+QString::number(i)+"/CorrTempC").toString().toShort();


            /* ConfigeIniWrite("/Device"+QString::number(i)+
                        "/Method_count",QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList.length()));
        for(int q=0;q<GlobalObject::g_ConnectDevList->at(i)->ControlMethodList.length();q++){
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sequece_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->sequece));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sym_one_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->i_sym_one));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_int_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->i_value));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_float_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->f_value));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sym_two_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->i_sym_two));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_2_int_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->i_value_2));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_2_float_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->f_value_2));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_Message__"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->Message_));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_Log_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->_Log));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Port_"+QString::number(i),QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->serialportnum));

        }*/

            int methodcount=configIniWriter->value("/Device"+QString::number(i)+
                                                   "/Method_count").toString().toInt();
            for(int q=0;q<methodcount;q++){
                Method *M=new Method;
                qDebug()<<"read method NO."<<q;
                M->_Log=_Logi(configIniWriter->value("/Device"+QString::number(i)+"/Method_Log_"+QString::number(q)).toString().toInt());
                M->i_sym_one=_type(configIniWriter->value("/Device"+QString::number(i)+"/Method_sym_one_"+QString::number(q)).toInt());
                M->i_sym_two=_type(configIniWriter->value("/Device"+QString::number(i)+"/Method_sym_two_"+QString::number(q)).toInt());
                M->i_value=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_int_"+QString::number(q)).toInt();
                M->i_value_2=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_2_int_"+QString::number(q)).toInt();
                M->f_value=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_float_"+QString::number(q)).toFloat();
                M->f_value_2=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_2_float_"+QString::number(q)).toFloat();
                M->Message_=configIniWriter->value("/Device"+QString::number(i)+"/Method_Message__"+QString::number(q)).toInt();
                M->sequece=configIniWriter->value("/Device"+QString::number(i)+"/Method_sequece_"+QString::number(q)).toInt();
                //ConfigeIniWrite("/Device"+QString::number(i)+
                //            "/Port",QString::number(GlobalObject::g_ConnectDevList->at(i)->ControlMethodList[0]->serialportnum));

                M->serialportnum=configIniWriter->value("/Device"+QString::number(i)+"/Port_"+QString::number(q)).toInt();

                newdev->ControlMethodList.push_back(M);
                if(M->sequece<12&&M->sequece>=0){
                    newdev->MethodCount[M->sequece]+=1;
                    if(newdev->MethodCount[M->sequece]>5){
                        newdev->MethodCount[M->sequece]=5;
                    }
                }


                //读取过程中也要加载控制参数总数，不然会在加载到界面的时候数组越界


            }

            if(methodcount!=0){
                newdev->m_bMethodexecRunable=true;
            }
            DeviceList_Config.push_back(newdev);
        }
        c=configIniWriter->value("General/RenameCount").toString().toInt();
        for(int i=0;i<c;i++){
            QString mac=configIniWriter->value("Device"+QString::number(i)+"/Mac").toString();
            QString name=configIniWriter->value("Device"+QString::number(i)+"/Name").toString();
            RenemeDeviceList.push_back(mac);
            RenemeDeviceList.push_back(name);
        }
        c=configIniWriter->value("General/BlockCount").toString().toInt();
        for(int i=0;i<c;i++){
            QString mac=configIniWriter->value("Device"+QString::number(i)+"/Mac").toString();
            BlockList.push_back(mac);
        }

        PromptInfomation("Load config files ok!");
    }


}
bool MainWidget::createTable()
{

    //弃用
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    db.open();
    QSqlQuery query(db);
    QStringList strTables = Db.tables();
    if(strTables.contains("Dev")){
        qDebug() << "Allready exit the table dev";
        //return true;
    }
    else{
        qDebug() << "no exit table";
    }
    bool ss=query.exec("create table Dev(id integer primary key,"
                       "Mac varchar(20),"
                       "name varchar(30),"
                       "Time varchar(50),"
                       "AccX short, "
                       "AccY short,"
                       "AccZ short,"
                       "Tilt_1 float,"
                       "Tilt_2 float,"
                       "Tilt_3 float,"
                       "Distance short,"
                       "motion short,"
                       "temperature float,"
                       "IR short,"
                       "IUV short,"
                       "Intensity short,"
                       "rssi short,"
                       "voltage float)");
    //bool ss= query.exec("create table user(id varchar primary key,passwd varchar,nickname varchar null)");
    if(ss)
    {
        qDebug() <<tr("数据库表创建成功!");
        return true;
    }
    else
    {
        qDebug()<<query.lastError();
        qDebug() <<tr("数据库表创建失败!");
        return false;
    }
}

bool MainWidget::createTable(const DeviceInfo *Dinfo){

    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    QStringList strTables = Db.tables();
    if(strTables.contains(Dinfo->mac)){
        qDebug() << "Allready exit the table dev";
        //return true;
    }
    else{
        qDebug() << "no";
    }

    bool success = query.exec("create table"+Dinfo->mac+"(id int primary key,name string, Time string,"
                                                        " AccX short, AccY short, AccZ short, Tilt_1 float, Tilt_2 float,"
                                                        " Tilt_3 float, Distance short, motion short, temperature float,"
                                                        " IR short, IUV short, Intensity short, rssi short, voltage float)");
    if(success)
    {
        qDebug()<<query.lastError();
        qDebug() << tr("数据库表创建成功！\n");
        return true;
    }
    else
    {
        qDebug() << tr("数据库表创建失败！\n");
        return false;
    }
}
//向数据库中插入记录
bool MainWidget::insert(const DeviceInfo *Dinfo)
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //添加数据库驱动，并指定连接名称
    QSqlQuery query(db);
    //QString select_max_sql = "select max(id) from Dev";
    //int max_id=-1;
    // query.prepare(select_max_sql);
    /* if(!query.exec())
    {
        qDebug()<<query.lastError();
    }
    else
    {
        while(query.next())
        {
            max_id = query.value(0).toInt();
            qDebug()<<QString("max id:%1").arg(max_id);
        }
    }*/
    query.prepare("insert into Dev(Mac,name,Time,AccX,AccY,AccZ,Tilt_1,Tilt_2,Tilt_3,Distance,motion,temperature,IR,IUV,Intensity,rssi,voltage)"
                  "values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    //query.addBindValue(max_id+1);
    query.addBindValue(Dinfo->mac);
    query.addBindValue(Dinfo->name);
    query.addBindValue(Dinfo->time);
    query.addBindValue(Dinfo->datainfo.accelx);
    query.addBindValue(Dinfo->datainfo.accely);
    query.addBindValue(Dinfo->datainfo.accelz);
    query.addBindValue(Dinfo->datainfo.tilt1);
    query.addBindValue(Dinfo->datainfo.tilt2);
    query.addBindValue(Dinfo->datainfo.tilt3);
    query.addBindValue(Dinfo->datainfo.distance);
    query.addBindValue(Dinfo->datainfo.motion);
    query.addBindValue(Dinfo->datainfo.temperature);
    query.addBindValue(Dinfo->datainfo.IR);
    query.addBindValue(Dinfo->datainfo.iUV);
    query.addBindValue(Dinfo->datainfo.intensity);
    query.addBindValue(Dinfo->rssi);
    query.addBindValue(Dinfo->voltage);
    // DbCount+=17;
    bool success=query.exec();
    if(!success)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText() << tr("插入失败");
        return false;
    }
    else{
        // qDebug()<<"Insert success!";
        //  qDebug()<<query.executedQuery();

    }
    return true;
}
//查询所有信息
bool MainWidget::queryAll()
{
    qDebug()<<"query all";
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    query.exec("select * from Dev");
    QSqlRecord rec = query.record();
    // qDebug() << QObject::tr("automobil表字段数：" ) << rec.count();
    while(query.next())
    {
        for(int index = 0; index < 17; index++)
            qDebug() << query.value(index) << " ";
        qDebug() << "\n";
    }
    return true;
}
bool MainWidget::deleteById(int id)
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare(QString("delete from Dev where id = %1").arg(id));
    if(!query.exec())
    {
        qDebug() << "Delete Fail";
        return false;
    }
    return true;
}
//根据ID更新记录
bool MainWidget::updateById(int id,const DeviceInfo *Dinfo)
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    /* "create table Dev(id long primary key,Mac varchar[12] ,Time varchar[30],"
                                   "AccX short,AccY short,AccZ short,Tilt_1 float,Tilt_2 float,"
                                   "Tilt_3 short,Distance short,motion short,temperature float,"
                                   "IR short,IUV short,Intensity short, rssi short, voltage float)"*/
    query.prepare(QString("update Dev set Mac=?,Time=?,"
                          "AccX =?, AccY=?,"
                          "AccZ=?, Tilt_1=?,"
                          "Tilt_2=?, Tilt_3=?,"
                          "Distance=? ,motion=?"
                          "temperature=?"
                          "IR=? ,IUV=?"
                          "Intensity=?,rssi=?,voltage=?"
                          "where id=%1").arg(id));

    query.bindValue(0, DbCount);
    query.bindValue(1, Dinfo->mac);
    query.bindValue(2, Dinfo->time);
    query.bindValue(3, Dinfo->datainfo.accelx);
    query.bindValue(4, Dinfo->datainfo.accely);
    query.bindValue(5, Dinfo->datainfo.accelz);
    query.bindValue(6, Dinfo->datainfo.tilt1);
    query.bindValue(7, Dinfo->datainfo.tilt2);
    query.bindValue(8, Dinfo->datainfo.tilt3);
    query.bindValue(9, Dinfo->datainfo.distance);
    query.bindValue(10, Dinfo->datainfo.motion);
    query.bindValue(11, Dinfo->datainfo.temperature);
    query.bindValue(12, Dinfo->datainfo.IR);
    query.bindValue(13, Dinfo->datainfo.iUV);
    query.bindValue(14, Dinfo->datainfo.intensity);
    query.bindValue(15, Dinfo->rssi);
    query.bindValue(16, Dinfo->voltage);

    bool success=query.exec();
    if(!success)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText() << tr("更新失败");
    }
    return true;
}
//排序
bool MainWidget::sortById()
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    bool success=query.exec("select * from Dev order by id desc");
    if(success)
    {
        qDebug() << tr("Sort Success!");
        return true;
    }
    else
    {
        qDebug() << tr("Sort Failed");
        return false;
    }
}
void MainWidget::on_EnvironmentConfig_pushButton_clicked()
{
    qDebug()<<"data before "<<ConfigPath<<" \n "<<DBpath<<"\n "<< Csvpath<<"\n "<<m_bShowOnCharts<<"\n"<<multiThread<<
              "\n"<<m_bConfigAcvated<<"\n"<<m_bCsv<<"\n"<<m_bDatabaseAcvated;
    env=new EnvironmentConfig(ConfigPath,Csvpath,DBpath,m_bShowOnCharts,
                              multiThread,m_bConfigAcvated,m_bCsv,m_bDatabaseAcvated,BlockList);
    connect(env,&EnvironmentConfig::envdone,this,&MainWidget::FromEnvDialog);
    connect(env,&EnvironmentConfig::changeIp,this,&MainWidget::ResetIp);
    connect(env,&EnvironmentConfig::Remove,this,&MainWidget::RemoveBlockItem);
    env->show();
}
void MainWidget::RemoveBlockItem(QString Mac){
    for(int i=0;i<BlockList.length();i++){
        if(BlockList[i]==Mac){
            BlockList.removeAt(i);
            BlockList.removeAt(i);
        }
    }
}

void MainWidget::FromEnvDialog(QString ConAddr, QString CsvAddr,
                               QString DBaddr,  bool multithread,bool label, bool Csv,
                               bool database, bool Con_ac){
    /*
    emit envdone( this->configaddr,
            this->DB_addr,
            this->Csv_addr,
            this->m_bMt,
            this->m_bla,
            this->m_bcfa,
            this->m_bdba,
            this->m_bcsva);*/


    ConfigPath=ConAddr;

    this->DBpath=DBaddr;
    Csvpath=CsvAddr;
    m_bShowOnCharts=label;
    multiThread=multithread;
    m_bConfigAcvated=Con_ac;
    m_bDatabaseAcvated=database;
    m_bCsv=Csv;
}
void MainWidget::ResetIp(QString ipaddr){
    if(m_bTCPConnected==true){
        return;
    }

    GlobalObject::Server_->listen(QHostAddress(ipaddr),GlobalObject::port_);
    ui->ip_label->setText("重设ip"+ipaddr);
}
void MainWidget::on_Config_pushButton_clicked()
{
    //DeviceInfo *x=new DeviceInfo;
   // GlobalObject::g_ConnectDevList->push_back(x);

   /* QList<DeviceInfo*> copyDL;
    try {
        for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
            DeviceInfo *x=new DeviceInfo;
            x=GlobalObject::g_ConnectDevList->at(i);
            copyDL.push_back(x);
        }
    } catch (QException &ex) {
        qDebug()<<ex.what();
    }*/

    Sc=new SensorConfig_Dialog(*GlobalObject::g_ConnectDevList,this);

    Sc->show();
}
void MainWidget::ThreadLunchar(){
    //open a thread for data processing
    if(multiThread){
        if(Pthread==nullptr){
            PromptInfomation("启动数据线程");
            Pthread=new QThread();
        }
        if(Data_Thread==nullptr){
            Data_Thread=new DataThread();
            Data_Thread->moveToThread(Pthread);
            Data_Thread->SetRunning(true);
            Pthread->start();
            // connect(this,&MainWidget::DataPthreadStart,Data_Thread,&DataThread::DataProcess);
            connect(this,&MainWidget::DataNeedtoDeal,Data_Thread,&DataThread::DataProcess_Simple);
            connect(Data_Thread,&DataThread::workDone,this,&MainWidget::ReceiveFromthread);
            connect(Data_Thread,&DataThread::ToInqueryProc,this,&MainWidget::InqueryProc);
            connect(Data_Thread,&DataThread::updateTree,this,&MainWidget::sig_UpdateTree_);
            connect(this,&MainWidget::WriteInfo,Data_Thread,&DataThread::GetWriteMsg);//UI需要操控端口写数据的时候
            connect(this,&MainWidget::ServerState,Data_Thread,&DataThread::ServerStateCheck);
            connect(this,&MainWidget::FocusChanges,Data_Thread,&DataThread::SetFocusIndex);
            connect(this,&MainWidget::TCPConnectInfomation,Data_Thread,&DataThread::TCPconnector);
            connect(Data_Thread,&DataThread::_ShortInfo,this,&MainWidget::_ShortInfoHanddler);
            connect(Data_Thread,&DataThread::ServerState,this,&MainWidget::_GetServer);
            connect(this,&MainWidget::ServerShutDown,Data_Thread,&DataThread::CloseServer);//必须确保在主线程退出前关闭server否则主线程回收子线程过程中QTCPserver不允许跨线程终止
            //emit DataPthreadStart(info);
            connect(this,&MainWidget::ReconnectDevice,Data_Thread,&DataThread::ReconnectDevice);//重连断开设备
            qRegisterMetaType<DeviceInfo>("DeviceInfo");

            //connect(GlobalObject::g_ConnectDevList,&DeviceInfo::megsend,Data_Thread,&DataThread::ReciveFromConnected);

            //在此处启动串口监听

        }
        if(Pthread->isRunning()){
            qDebug()<<"pthread";
            PromptInfomation("启动成功");
        }
        else{
            PromptInfomation("启动失败");
            multiThread=false;
        }
    }


}
void MainWidget::_ShortInfoHanddler(QString info,int p){
    if(p==0){
        PromptInfomation(info);
    }
}
void MainWidget::closeEvent(QCloseEvent *event){

    qDebug()<<"close event";
    QMessageBox msgBox;
    msgBox.setText("Close");
    msgBox.setInformativeText(tr("确定要退出程序?"));
    msgBox.setStandardButtons(QMessageBox::Yes  | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
    case QMessageBox::Cancel:
        event->ignore();
        return;
    case QMessageBox::Yes:
        qDebug()<<"yes ";
        break;

    }
    emit ServerShutDown();

    QMessageBox msgBox2;
    msgBox2.setText("ConfigWriter");
    msgBox2.setInformativeText(tr("需要写入到配置文件吗"));
    msgBox2.setStandardButtons(QMessageBox::Yes  | QMessageBox::No);
    msgBox2.setDefaultButton(QMessageBox::Yes);
    int ret2 = msgBox2.exec();
    switch (ret2) {
    case QMessageBox::Yes:
        ConfigerFileSupvisor(WRITE_CON);

        break;
    case  QMessageBox::No:
        break;
    }

    if(GlobalObject::g_ConnectDevList->length()>0){
        for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
            DisconnetDeviceTrigged(GlobalObject::g_ConnectDevList->at(i)->mac);
        }
    }

    for(int i=0;i<18;i++){

        if(Csv[i]!=nullptr){
            if(Csv[i]->Get__File()->isOpen()){
                Csv[i]->Get__File()->close();
            }
            delete Csv[i];
            Csv[i]=nullptr;
        }
    }

    Db.close();
    event->accept();

}
QString MainWidget::HexToDec(QByteArray arr)
{
    unsigned int value=0;
    int len=arr.length();
    for(int q=0;q<len;q++){
        switch (arr[q]) {
        case 'a':
            value+=10*pow(16,len-1-q);
            break;
        case 'b':
            value+=11*pow(16,len-1-q);
            break;
        case 'c':
            value+=12*pow(16,len-1-q);
            break;
        case 'd':
            value+=13*pow(16,len-1-q);
            break;
        case 'e':
            value+=14*pow(16,len-1-q);
            break;
        case 'f':
            value+=15*pow(16,len-1-q);
            break;
        default:
            value+=(static_cast<int>(arr[q])-48)*pow(16,len-1-q);
            break;
        }
    }

    return QString::number(value);
}
void MainWidget::mousePressEvent(QMouseEvent* event){
    if(event->button()==Qt::LeftButton){

        if(event->pos().x()<ui->Title_Labal->width()&&event->pos().y()<ui->Title_Labal->height()+20){

            this->setUpdatesEnabled(false);
            windowLocked=true;
            mousePos=event->globalPos();
            windowPos = this->frameGeometry().topLeft();
        }

    }
}
void MainWidget::mouseMoveEvent(QMouseEvent *event){
    if(windowLocked){
        QPoint relativePos = event->globalPos() - mousePos;
        this->move(windowPos + relativePos );
    }
}
void MainWidget::mouseReleaseEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        if(event->pos().x()<ui->Title_Labal->width()&&event->pos().y()<ui->Title_Labal->height()+20){
            this->setUpdatesEnabled(true);
            windowLocked=false;
        }
        // qDebug()<<ui->Title_Labal->frameGeometry().x()<<ui->Title_Labal->frameGeometry().y()<<"wid "<<ui->Title_Labal->width()<<" h "<<ui->Title_Labal->height();


    }
}
QString MainWidget::HexTilteToDec(QByteArray arr){
    //00 13
    //第一个数据大于7就是负数
    int value=0;
    int len=arr.length();

    for(int q=0;q<len;q++){
        switch (arr[q]) {
        case 'a':
            value+=10*pow(16,len-1-q);
            break;
        case 'b':
            value+=11*pow(16,len-1-q);
            break;
        case 'c':
            value+=12*pow(16,len-1-q);
            break;
        case 'd':
            value+=13*pow(16,len-1-q);
            break;
        case 'e':
            value+=14*pow(16,len-1-q);
            break;
        case 'f':
            value+=15*pow(16,len-1-q);
            break;
        default:
            value+=(static_cast<int>(arr[q])-48)*pow(16,len-1-q);
            break;
        }

    }

    if(arr[0]=='a'||arr[0]=='b'||arr[0]=='c'||arr[0]=='d'||arr[0]=='e'||arr[0]=='f'||arr[0]=='8'||arr[0]=='9'){
        qDebug()<<"more "<<value;
        value-=65536;
        qDebug()<<"adasd"<<value;
    }
    return QString::number(value);




}
void MainWidget::PushToblocklist(QString Mac,QString name){
    qDebug()<<"mac "<<Mac<<"name "<<name;
    bool fid=false;
    for(int i=0;i<BlockList.length();i+=2){
        if(Mac==BlockList[i]&&name==BlockList[i+1]){
            fid=true;
            break;
        }
    }
    if(!fid){
        qDebug()<<"push to block list";
        BlockList.push_back(Mac);
        BlockList.push_back(name);

        for(int q=0;q<ScanDevList.length();q++){
            if(ScanDevList[q]->mac==Mac){
                ScanDevList.removeAt(q);
            }
        }
        AddDeviceToNewDeviceTree();
    }
    qDebug()<<"show block list ";
    for(int i=0;i<BlockList.length();i+=2){
        qDebug()<<BlockList[i]<<BlockList[i+1];
    }

}
void MainWidget::_GetServer(bool b1,bool b2){
    m_bTcp_1_con=b1;
    m_bTcp_2_con=b2;
    if(m_bTcp_1_con){
        ui->Port1_State_label->setText(QString::number(GlobalObject::port_)+"  Connected");
        m_bTCPConnected=true;
    }
    if(m_bTcp_2_con){
        ui->Port2_State_label->setText(QString::number(GlobalObject::port_2)+"  Connected");
        m_bTCPConnected=true;
    }

}
void MainWidget::sig_UpdateTree_(){
    AddDevicetoConnectedTree();
    AddDeviceToNewDeviceTree();
}

void MainWidget::on_Close_pushButton_clicked()
{
    this->close();
}

void MainWidget::on_Minimize_pushButton_clicked()
{
    this->showMinimized();
}
void MainWidget::DataBaseThreadLuncher(){
    if(DB_Pthread==nullptr){
        PromptInfomation(tr("启动数据库线程"));
        DB_Pthread=new QThread();
    }
    if(DB_T==nullptr){
        DB_T=new DataBase_thread();
        DB_T->moveToThread(DB_Pthread);
        DB_Pthread->start();
    }
    connect(this,&MainWidget::DB_CreatTable,DB_T,&DataBase_thread::init);
    connect(Data_Thread,&DataThread::InsertDataToDataBase,DB_T,&DataBase_thread::insert);
    emit DB_CreatTable("");
}

void MainWidget::on_Data_Thread_start()
{
    ui->Start_Robt_Ctr->setEnabled(true);
    ui->Env_State->setText("报警解除");
}

void MainWidget::on_Data_Thread_stop()
{
    qDebug() << "the signal is emit====";
    ui->Start_Robt_Ctr->setEnabled(false);
    ui->Env_State->setText("报警");
    GlobalObject::m_bSend_Access[0] = false;
    GlobalObject::m_bSend_Access[1] = false;

}

void MainWidget::on_Start_Robt_Ctr_clicked()
{
    ui->Env_State->setText("正常");
    ui->Start_Robt_Ctr->setEnabled(false);
    GlobalObject::m_bSend_Access[0] = true;
    GlobalObject::m_bSend_Access[1] = true;
}

void MainWidget::on_QuantityStartBtn_clicked()
{
    ui->Env_State->setText("正常");
    ui->QuantityStartBtn->setEnabled(false);
    ui->RefreshPortButton->setEnabled(true);
    Portname = ui->QuantyCmbBox->currentText(); //获取环境监控串口号
    startQuantity = 1;                          //开始监听端口数据
    qDebug() << "*****************************";
    Quanty_thread->run();
}

void MainWidget::on_RefreshPortButton_clicked()
{
    ui->QuantityStartBtn->setEnabled(true);
    ui->RefreshPortButton->setEnabled(false);
    ui->QuantyCmbBox->clear();
    ui->Env_State->setText("尚未监控");

    if(Quanty_thread->Quantityserial->isOpen())
    {
        Quanty_thread->Quantityserial->close();
        Quanty_thread->t->stop();
        Quanty_thread->quit();
        Quanty_thread->terminate();
        qDebug() << "close++++++++";
    }

    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())  //查找可用端口
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->QuantyCmbBox->addItem(serial.portName());
            serial.close();
        }
    }
}
