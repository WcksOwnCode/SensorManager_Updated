#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QTabWidget>
#include "globeobject.h"
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
    testtier=new QTimer(this);
    connect(testtier,&QTimer::timeout,this,&MainWidget::Initialize_Environment);
    testtier->start(300);
    QString localHostName = QHostInfo::localHostName();
    qDebug() << "LocalHostName:" << localHostName;
    ip_info = QHostInfo::fromName(localHostName);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, QColor(255,255,255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    QImage img;
    if(img.load(":/new/logo/rsc/resize.bmp"))//execute qmake before load
    {
        QImage img2= img.scaled(ui->biglogo_label->width(),ui->biglogo_label->height());
        QPixmap qp=QPixmap::fromImage(img2);
        ui->biglogo_label->setPixmap(qp);
    }

}
MainWidget::~MainWidget()
{
    qDebug()<<"xigouhanshu";
    for(int q=0;q<ConnectDevList.length();q++){
        DisconnetDeviceTrigged(ConnectDevList[q]->mac,ConnectDevList[q]->devPortnum);
    }
    qDebug()<<"1";
    GlobeObject::Server_->close();
    if(m_bTCPConnected==true){
        GlobeObject::Socket_->close();
    }
    qDebug()<<"2";
    if(testtier->isActive()){
        testtier->stop();
    };

    delete testtier;
    qDebug()<<"3";
    if(configIniWriter!=nullptr){
        delete configIniWriter;
    }
    qDebug()<<"4";
    for(int i=0;i<6;i++){
        if(chart[i]!=nullptr){
            delete chart[i];
        }
        if(series[i]!=nullptr){
            delete series[i];
        }

    }
    qDebug()<<"5";
    if(Pthread!=nullptr){
        Pthread->quit();
        Pthread->wait();

    }
    if(Data_Thread!=nullptr){
        delete Data_Thread;
        Data_Thread=nullptr;
    }
    qDebug()<<"6";
    Db.close();
    qDebug()<<"7";

    qDebug()<<"8";
    if(Sc!=nullptr){
        delete Sc;
        Sc=nullptr;
    }
    qDebug()<<"9";
    if(env!=nullptr){
        delete env;
        env=nullptr;
    }
    qDebug()<<"10";
    for(int i=0;i<18;i++){
        if(Csv[i]!=nullptr){
            Csv[i]->Get__File()->close();
            delete Csv[i];
            Csv[i]=nullptr;
        }
    }
    qDebug()<<"11";
    delete ui;
}
void MainWidget::BinaryConfigWrite_Read(_ConfigType ty,QString ConfigAddr){
    //用二进制的方式保存配置文件更加高效
    if(ty==READ_CON){
        //READ

    }
    if(ty==WRITE_CON){
        QString binaryFilename="F://banary.txt";
        QFile file(binaryFilename);
        if(!file.exists())return;
        file.open(QIODevice::WriteOnly);
        //List<T> *p=root;
        /*   while(p)
                {
                    file.write((char*)(&(p->data)),sizeof(T));
                    p=p->next;
                }*/
        file.close();


    }
}
void MainWidget::Initialize_Environment(){
    PromptInfomation("初始化.");

    ConfigAddress=QCoreApplication::applicationDirPath()+"/"+QString::number(GlobeObject::port_)+"_Config.ini";
    testtier->stop();

    DataBaseLuncher();
    disconnect(testtier,&QTimer::timeout,this,&MainWidget::Initialize_Environment);
    ui->Device_tabWidget->setTabText(0,"Con");
    ui->Device_tabWidget->setTabText(1,"Scan");
    TCPconnector();
    ui->ConnectedDevice_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->OtherDevicetree_Wiew->setContextMenuPolicy(Qt::CustomContextMenu);//开了这个才能开启菜单触发
    connect(ui->OtherDevicetree_Wiew,SIGNAL(customContextMenuRequested(const QPoint& )),this,SLOT(OtherDeviceMenu(const QPoint&)));//右键菜单草
    connect(ui->ConnectedDevice_treeView,SIGNAL(customContextMenuRequested(const QPoint& )),this,SLOT(ConectedDeviceMenu(const QPoint&)));//右键菜单草
    connect(ui->ConnectedDevice_treeView,&QTreeView::clicked,this,&MainWidget::ItemBeClicked);
    if(m_bConfigAcvated){
        QFile check(ConfigAddress);
        configIniWriter=new QSettings(ConfigAddress, QSettings::IniFormat);
        if(check.exists()){
            ConfigerFileSupvisor(READ_CON);
        }
    }
    ui->Chart_tabWidget->setTabText(0,"Acc");
    ui->Chart_tabWidget->setTabText(1,"Tilte");
    ui->Chart_tabWidget->setTabText(2,"distance");
    ui->Chart_tabWidget->setTabText(3,"IR");
    ui->Chart_tabWidget->setTabText(4,"IUv");
    ui->Chart_tabWidget->setTabText(5,"Motion");
    m_bShowOnCharts=true;

    ThreadLunchar();
    // queryAll();
}
void MainWidget::TCPconnector(){
    QHostAddress HostAdd(ip_info.addresses()[1]);
    if(GlobeObject::port_!=0){
        ui->ip_label->setText("本机ip"+(ip_info.addresses())[1].toString());
        GlobeObject::Server_->listen(HostAdd,GlobeObject::port_);
        connect(GlobeObject::Server_,&QTcpServer::newConnection,this,[=]{
            GlobeObject::Socket_=GlobeObject::Server_->nextPendingConnection();
            ui->Port1_State_label->setText(QString::number(GlobeObject::port_)+"  Connected");
            PromptInfomation("TCP connected");
            m_bTcp_1_con=true;
            CurrentSearchingCount=0;
            m_bTCPConnected=true;
            connect(GlobeObject::Socket_,&QTcpSocket::readyRead,this,[=]{
                QByteArray s=GlobeObject::Socket_->readAll();
                InfoHanddler_new(s,0);
            });

        });
    }
    else{
        if(GlobeObject::port_2!=0&&GlobeObject::port_2!=GlobeObject::port_){
            GlobeObject::Server_2->listen(HostAdd,GlobeObject::port_2);
            connect(GlobeObject::Server_2,&QTcpServer::newConnection,this,[=]{
                GlobeObject::Socket_2=GlobeObject::Server_2->nextPendingConnection();
                // ui->ConnectState_Label->setText("已经连接网关 2");
                ui->Port2_State_label->setText(QString::number(GlobeObject::port_2)+"  Connected");
                //qDebug()<<"port2"<<GlobeObject::Socket_->isOpen()<<"  "<<GlobeObject::Socket_2->isOpen();
                //CurrentSearchingCount=0;
                m_bTcp_2_con=true;
                m_bTCPConnected=true;

                connect(GlobeObject::Socket_2,&QTcpSocket::readyRead,this,[=]{
                    QByteArray s=GlobeObject::Socket_2->readAll();
                    //InfoHanddler(s);
                    InfoHanddler_new(s,1);
                });
            });
        };
    }




    PromptInfomation("等待蓝牙网关");
}
void MainWidget::DataBaseLuncher(){
    if(m_bDatabaseAcvated){
        Db=QSqlDatabase::addDatabase("QSQLITE","DeviceData");
        Db.setHostName("Host");
        Db.setDatabaseName("DeviceDataBase.db");
        Db.setUserName("wck");
        Db.setPassword("1232123");
        if(!Db.open()){
            QMessageBox::information(this,"DB","DB open failed!");
        }
        //Db.close();
        PromptInfomation("DataBase is ready");
        createTable();//创建表
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
    ConfigeIniWrite("General/DeviceCount",QString::number(ConnectDevList.length()));
    //ConfigeIniWrite("General/RenameCount",QString::number(RenemeDeviceList.length()));
    ConfigeIniWrite("General/BlockCount",QString::number(BlockList.length()));
    //device info write
    for(int i=0;i<ConnectDevList.length();i++){
        ConfigeIniWrite("/Device"+QString::number(i)+"/Mac",ConnectDevList[i]->mac);
        ConfigeIniWrite("/Device"+QString::number(i)+"/Name",ConnectDevList[i]->name);
        ConfigeIniWrite("/Device"+QString::number(i)+"/NewName",ConnectDevList[i]->newname);
        ConfigeIniWrite("/Device"+QString::number(i)+"/RSSI",QString::number(ConnectDevList[i]->rssi));
        //修正系数
        qDebug()<<"write correction value";
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAccX",QString::number(ConnectDevList[i]->Corr_Coe.CorrAccX));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAccY",QString::number(ConnectDevList[i]->Corr_Coe.CorrAccY));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAccZ",QString::number(ConnectDevList[i]->Corr_Coe.CorrAccZ));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrAlpha",QString::number(ConnectDevList[i]->Corr_Coe.CorrAlpha));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrBeta",QString::number(ConnectDevList[i]->Corr_Coe.CorrBeta));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrGama",QString::number(ConnectDevList[i]->Corr_Coe.CorrGama));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrDistanceA",QString::number(ConnectDevList[i]->Corr_Coe.CorrDistanceA));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrDistanceB",QString::number(ConnectDevList[i]->Corr_Coe.CorrDistanceB));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrDistanceC",QString::number(ConnectDevList[i]->Corr_Coe.CorrDistanceC));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrLight",QString::number(ConnectDevList[i]->Corr_Coe.CorrLight));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrIR",QString::number(ConnectDevList[i]->Corr_Coe.CorrIR));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrTempA",QString::number(ConnectDevList[i]->Corr_Coe.CorrTempA));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrTempB",QString::number(ConnectDevList[i]->Corr_Coe.CorrTempB));
        ConfigeIniWrite("/Device"+QString::number(i)+"/CorrTempC",QString::number(ConnectDevList[i]->Corr_Coe.CorrTempC));
        //控制策略
        qDebug()<<"write cotrol method";
        ConfigeIniWrite("/Device"+QString::number(i)+
                        "/Method_count",QString::number(ConnectDevList[i]->ControlMethodList.length()));
        for(int q=0;q<ConnectDevList[i]->ControlMethodList.length();q++){
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sequece",QString::number(ConnectDevList[i]->ControlMethodList[0]->sequece));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sym_one",QString::number(ConnectDevList[i]->ControlMethodList[0]->i_sym_one));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_int",QString::number(ConnectDevList[i]->ControlMethodList[0]->i_value));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_float",QString::number(ConnectDevList[i]->ControlMethodList[0]->f_value));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_sym_two",QString::number(ConnectDevList[i]->ControlMethodList[0]->i_sym_two));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_2_int",QString::number(ConnectDevList[i]->ControlMethodList[0]->i_value_2));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_value_2_float",QString::number(ConnectDevList[i]->ControlMethodList[0]->f_value_2));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_Message_",QString::number(ConnectDevList[i]->ControlMethodList[0]->Message_));
            ConfigeIniWrite("/Device"+QString::number(i)+
                            "/Method_Log",QString::number(ConnectDevList[i]->ControlMethodList[0]->_Log));
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
void MainWidget::InfoHanddler(QString info){
    //abandoned this


    //所有接收到的信息都在这里处理
    /*
    qDebug()<<"info  "<<info;
    if(multiThread){
        if(Pthread==nullptr){
            Pthread=new QThread;
        }
        if(Data_Thread==nullptr){
            Data_Thread=new DataThread(this);
            Data_Thread->moveToThread(Pthread);
            Data_Thread->SetRunning(true);
            connect(this,&MainWidget::DataPthreadStart,Data_Thread,&DataThread::DataProcess);
            connect(this,&MainWidget::DataNeedtoDeal,Data_Thread,&DataThread::DataProcess_Simple);

        }

        else{
            multiThread=false;
        }
    }
    else{

        if(info.contains("INQRESULT",Qt::CaseInsensitive)){
            // qDebug()<<"\n"<<"info :"<<info<<"\n";
            info.remove(QChar('\n'), Qt::CaseInsensitive);
            info.remove(QChar('\\'), Qt::CaseInsensitive);
            info.remove(QChar('\r'), Qt::CaseInsensitive);
            info.remove(QChar('"'), Qt::CaseInsensitive);
            info.remove(QString("+INQRESULT:"), Qt::CaseInsensitive);
            //  qDebug() << "\n"<<info << "return\n" << endl;
            QStringList strList= info.split(QRegExp(","));
            //  qDebug()<<"list is \n"<<strList<<"\n"<<strList.size()<<"\n";
            if(strList.size()>5&&strList[0].length()==12){
                for(int  q=0;q<BlockList.length();q++){
                    if(strList[0]==BlockList[q]){
                        return;
                    }
                }
                bool find=false;
                for(int q=0;q<ScanDevList.length();q++){
                    if(ScanDevList[q]->mac==strList[0]){
                        find=true;
                        break;
                    }
                }

                if(!find){
                    if(strList[0].length()==12&&strList[2]>4){
                        ScanListItem *SLI=new ScanListItem;
                        SLI->mac=strList[0];
                        SLI->name=strList[2];
                        SLI->rssi=static_cast<signed char>(strList[strList.size()-1].toInt());
                        SLI->addrtype=static_cast<signed char>(strList[1].toInt());
                        if(ScanDevList.length()<MAX_SCAN_COUNT){
                            ScanDevList.push_back(SLI);
                        }
                        // qDebug()<<ScanDevList.size();
                        AddDeviceToNewDeviceTree();
                    }
                }

                for(int q=0;q<NeedToConnected.length();q+=2){
                    if(NeedToConnected[q]==strList[0]){
                        //直接连接
                        signed char ty=static_cast<signed char>(strList[1].toInt());
                        ConnectActionTrigged(NeedToConnected[q],NeedToConnected[q],ty);
                    }
                }


            }
        }else if(info.contains("+INQCOMPLETE:",Qt::CaseInsensitive)){
            PromptInfomation("搜索完成");
            m_bSearching=false;
        }
        else if(info.contains("OK",Qt::CaseInsensitive)){
            //来自数据
            qDebug()<<"handle ok";
        }
        else if(info.contains("ERROR",Qt::CaseInsensitive)){
            qDebug()<<"handle error";
        }
        else if(info.contains("+CHARACTERISTIC:",Qt::CaseInsensitive)){
            //conncet success if recive this
            //qDebug()<<"conected device data";
            //"\r\n+CHARACTERISTIC:E5AEFA129DDC,fff0,fff2,0c\r\n"
            info.remove(QChar('\n'), Qt::CaseInsensitive);
            info.remove(QChar('\\'), Qt::CaseInsensitive);
            info.remove(QChar('\r'), Qt::CaseInsensitive);
            info.remove(QChar('"'), Qt::CaseInsensitive);
            info.remove(QString("+CHARACTERISTIC:"), Qt::CaseInsensitive);
            QStringList strList= info.split(QRegExp(","));
            qDebug()<<"+CHARACTERISTIC: sensor info is \n"<<strList<<"\n list size:"<<strList.size()<<"\n";
            //将设备加入列表
            bool find=false;
            for(int q=0;q<ConnectDevList.length();q++){
                if(strList[0]==ConnectDevList[q]->mac){
                    find=true;
                    ConnectDevList[q]->state=CONNECTED;
                    if(ConnectDevList[q]->name==""){
                        for(int m=0;m<ScanDevList.size();m++){
                            if(ScanDevList[m]->mac==strList[0]){
                                ConnectDevList[q]->name=ScanDevList[q]->name;
                                ConnectDevList[q]->rssi=ScanDevList[q]->rssi;
                                ConnectDevList[q]->addrtype=ScanDevList[q]->addrtype;
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
                if(strList[0].length()!=12){
                    return;
                }
                DeviceInfo *newde=new DeviceInfo;
                newde->mac=strList[0];
                int p=-1;
                bool find=false;
                for(int q=0;q<ScanDevList.size();q++){
                    if(ScanDevList[q]->mac==strList[0]){
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
                ConnectDevList.push_back(newde);
                ConnectDevList.last()->state=CONNECTED;
                PromptInfomation("连接"+strList[0]+"成功");
                if(m_bCsv){
                    if(ConnectDevList.length()>0){
                        if(Csv[ConnectDevList.length()-1]==nullptr){
                            Csv[ConnectDevList.length()-1]=new CsvWriter(ConnectDevList.last()->mac);//绑定写入器
                        }
                    }
                }

                for(int q=0;q<NeedToConnected.length();q+=2){
                    if(NeedToConnected[q]==strList[0]){
                        //直接连接
                        NeedToConnected.remove(q);
                    }
                }
                ReadSingelDeviceData(strList[0],newde->devPortnum);//这个发送了才会开始广播数据
            }
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
        }
        // ReadSingelDeviceData(strList[0]);
        //  this->close();

        else if(info.contains("+DISC:")){
            // "\r\n+DISC:E5AEFA129DDC\r\n"
            info.remove(QChar('\n'), Qt::CaseInsensitive);
            info.remove(QChar('\\'), Qt::CaseInsensitive);
            info.remove(QChar('\r'), Qt::CaseInsensitive);
            info.remove(QString("+DISC:"), Qt::CaseInsensitive);
            //剩下就是mac了
            PromptInfomation("断开连接"+info);
            for(int i=0;i<ConnectDevList.length();i++){
                if(ConnectDevList[i]->mac==info){
                    ConnectDevList.removeAt(i);
                    AddDevicetoConnectedTree();
                    AddDeviceToNewDeviceTree();
                    break;
                }
            }
            //解除绑定
            if(m_bCsv){
                for(int i=0;i<18;i++){
                    if(Csv[i]!=nullptr){
                        if(Csv[i]->GetBindMac()==info){
                            delete Csv[i];
                            Csv[i]=nullptr;
                        }
                    }

                }
            }

        }
        else if(info.contains("+NOTIFY:",Qt::CaseInsensitive)){
           // 由于网关的BLE MTU长度为23字节，也就是实际允许接收数据长度最大为20，所以会分两次收完
           // "\r\n+NOTIFY:C4CC019C6A79,fff1,12c0386cdbdb0065019001c7043100bd00d8025c\r\n"
           // "\r\n+NOTIFY:C4CC019C6A79,fff1,0900007a035c00000001fa\r\n"


            info.remove(QChar('\n'), Qt::CaseInsensitive);
            info.remove(QChar('\\'), Qt::CaseInsensitive);
            info.remove(QChar('\r'), Qt::CaseInsensitive);
            info.remove(QChar('"'), Qt::CaseInsensitive);
            info.remove(QChar('OK'), Qt::CaseInsensitive);
            info.remove(QString("+NOTIFY:"), Qt::CaseInsensitive);
            QStringList strList= info.split(QRegExp(","));


            if(ConnectDevList.length()==0){
                DisconnetDeviceTrigged(strList[0],);
            }
            for(int q=0;q<ConnectDevList.length();q++){
                if(ConnectDevList[q]->mac==strList[0]&&ConnectDevList[q]->name==""){
                    ConnectDevList.removeAt(q);
                    AddDevicetoConnectedTree();
                    AddDeviceToNewDeviceTree();
                    break;
                }
            }

            if(strList[2].length()==40){
                DataInfo_forepart=strList[2].toLatin1();
            }
            else{
                DataInfo_backend=strList[2].toLatin1();
                SensorDataConvert(DataInfo_forepart,DataInfo_backend,strList[0]);
            }
            // GetSingleSensorRSSI(strList[0]);

        }
        else if(info.contains("+RSSI:",Qt::CaseInsensitive)){
            info.remove(QChar('\n'), Qt::CaseInsensitive);
            info.remove(QChar('\\'), Qt::CaseInsensitive);
            info.remove(QChar('\r'), Qt::CaseInsensitive);
            info.remove(QChar('"'), Qt::CaseInsensitive);
            info.remove(QString("+RSSI:"), Qt::CaseInsensitive);
            QStringList strList= info.split(QRegExp(","));
            // qDebug()<<"List  "<<strList;
            for(int q=0;q<ConnectDevList.length();q++){
                if(ConnectDevList[q]->mac==strList[0]){
                    // qDebug()<<"before "<<strList[1];
                    ConnectDevList[q]->rssi=static_cast<short>(HexToDec(strList[1]));
                    //  qDebug()<<"then "<<ConnectDevList[q]->rssi;
                }
            }
        }
        else if(info.contains("+READCHAR:",Qt::CaseInsensitive)){
            // "\r\n+READCHAR:E5AEFA129DDC,fff2,012001\r\n"
            info.remove(QChar('\n'), Qt::CaseInsensitive);
            info.remove(QChar('\\'), Qt::CaseInsensitive);
            info.remove(QChar('\r'), Qt::CaseInsensitive);
            info.remove(QChar('"'), Qt::CaseInsensitive);
            info.remove(QString("+READCHAR:"), Qt::CaseInsensitive);
            QStringList strList= info.split(QRegExp(","));

        }
        else{
            qDebug()<<"other type info :\n"<<info<<"\n";
        }
    }*/
}
QStringList MainWidget::StringFileter(QString info,int ){
    //处理数据打包字符串分割
    info.remove(QChar('\n'), Qt::CaseInsensitive);
    info.remove(QChar('\\'), Qt::CaseInsensitive);
    info.remove(QChar('\r'), Qt::CaseInsensitive);
    info.remove(QChar('"'), Qt::CaseInsensitive);
    // info.remove(QChar(':'), Qt::CaseInsensitive);
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
    qDebug()<<"cleared info  "<<info;
    while(pos<len){
        if(lists[pos]=="INQRESULT"|lists[pos]=="NOTIFY"
                |lists[pos]=="CHARACTERISTIC"|lists[pos]=="RSSI"
                |lists[pos]=="READCHAR"|lists[pos]=="DISC"
                |lists[pos]=="INQCOMPLETE"){
            meg.push_back(";");
        }
        if(lists[pos]!=""&&lists[pos].length()>=2){
            meg.push_back(lists[pos]);
            meg.push_back(",");
        }
        pos++;

    }
        meg.remove(meg.length()-1);

    qDebug()<<"meg   "<<meg;
    ret=meg.split(";");
    for(int i=0;i<ret.length();i++){
       if(ret[i].length()<2){
           ret.removeAt(i);
       }

    }
    qDebug()<<"ret "<<ret;
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

    qDebug()<<"info "<<info;
    QStringList strlist =StringFileter(info);
    for(int q=0;q<strlist.length();q++){


        QStringList strList;
        switch (infoSpliter(strlist[q],strList)) {

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
            for(int q=0;q<ConnectDevList.length();q++){
                if(ConnectDevList[q]->mac==strList[0]){
                    // qDebug()<<"before "<<strList[1];
                    ConnectDevList[q]->rssi=static_cast<short>(HexToDec(strList[1]));
                    //  qDebug()<<"then "<<ConnectDevList[q]->rssi;
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
            //ok or error, just ignore it
            break;
        case 7:
            PromptInfomation("搜索完成");
            m_bSearching=false;
            break;

        }
        qDebug()<<"after spilter  "<<strList;
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
    //-1---出错
    if(info.contains("INQRESULT",Qt::CaseInsensitive)){
        // qDebug()<<"\n"<<"info :"<<info<<"\n";
        info.remove(QString("INQRESULT"), Qt::CaseInsensitive);
        //  qDebug() << "\n"<<info << "return\n" << endl;
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        for(int i=0;i<returnstr.length();i++){
            if(returnstr[i].length()<2){
                returnstr.removeAt(i);
            }
        }
        return 0;
    }
    if(info.contains("INQCOMPLETE:",Qt::CaseInsensitive)){
        PromptInfomation("搜索完成");
        m_bSearching=false;
        for(int i=0;i<returnstr.length();i++){
            if(returnstr[i].length()<2){
                returnstr.removeAt(i);
            }
        }
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
    if(info.contains("CHARACTERISTIC:",Qt::CaseInsensitive)){
        //conncet success if recive this
        //qDebug()<<"conected device data";
        //"\r\n+CHARACTERISTIC:E5AEFA129DDC,fff0,fff2,0c\r\n"
        info.remove(QString("CHARACTERISTIC:"), Qt::CaseInsensitive);
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        for(int i=0;i<returnstr.length();i++){
            if(returnstr[i].length()<2){
                returnstr.removeAt(i);
            }
        }
        return 2;
    }
    if(info.contains("DISC")){
        // "\r\n+DISC:E5AEFA129DDC\r\n"
        qDebug()<<"before   "<<info;
        info.remove(QString("DISC"), Qt::CaseInsensitive);
        qDebug()<<"after  "<<info;
        //剩下就是mac了
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        for(int i=0;i<returnstr.length();i++){
            if(returnstr[i].length()<2){
                returnstr.removeAt(i);
            }
        }
        return 5;
    }
    if(info.contains("NOTIFY",Qt::CaseInsensitive)){
        /*由于网关的BLE MTU长度为23字节，也就是实际允许接收数据长度最大为20，所以会分两次收完
        "\r\n+NOTIFY:C4CC019C6A79,fff1,12c0386cdbdb0065019001c7043100bd00d8025c\r\n"
        "\r\n+NOTIFY:C4CC019C6A79,fff1,0900007a035c00000001fa\r\n"
       */
        info.remove(QString("NOTIFY"), Qt::CaseInsensitive);
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        for(int i=0;i<returnstr.length();i++){
            if(returnstr[i].length()<2){
                returnstr.removeAt(i);
            }
        }
        return 1;
    }
    if(info.contains("RSSI",Qt::CaseInsensitive)){
        info.remove(QString("RSSI"), Qt::CaseInsensitive);
        QStringList strList= info.split(QRegExp(","));
        return 3;
    }
    if(info.contains("READCHAR",Qt::CaseInsensitive)){
        // "\r\n+READCHAR:E5AEFA129DDC,fff2,012001\r\n"
        info.remove(QString("READCHAR"), Qt::CaseInsensitive);
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        for(int i=0;i<returnstr.length();i++){
            if(returnstr[i].length()<2){
                returnstr.removeAt(i);
            }
        }
        return 4;
    }
    else{
        qDebug()<<"other type info :\n"<<info<<"\n";
        return -1;
    }
}
void MainWidget::SensorDataConvert(QByteArray fore, QByteArray back,QString Mac){
    if(ConnectDevList.length()==0){
        qDebug()<<"无名设备data传入";
        return;
    }
    qDebug()<<"convert ";
    DeviceInfo *newdevice=new DeviceInfo;
    int index=0;
    bool find=false;
    newdevice->datainfo=newdevice->datainfo;
    for(int i=0;i<ConnectDevList.length();i++){
        if(Mac==ConnectDevList[i]->mac){
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
        if(Mac.length()==12){
            newdevice->mac=Mac;
            index=ConnectDevList.length();
            ConnectDevList.push_back(newdevice);
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
        }
        else{
            return ;
        }
    }


    if(fore[0]='1'&&fore[1]=='2'&&fore[2]=='c'){
        //12c0 5a d9 47 26   00 4f   03 20   fd 72   00 13   00 00   ff d8   00 01
        //时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        ConnectDevList[index]->time=HexTimeToDec(temp1);
        temp1.clear();
        temp2.clear();
        QDateTime local(QDateTime::currentDateTime());
        QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
        //ui->ComputerTime_label->setText(localTime);
        for(int q=12;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        ConnectDevList[index]->waitdata=true;
        if(ConnectDevList[index]->m_bCorrectValue){
            ConnectDevList[index]->datainfo.motion = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            ConnectDevList[index]->datainfo.accelx = static_cast<short>((temp2[2]&0xFF)<<8|(temp2[3]&0xFF))+ConnectDevList[index]->Corr_Coe.CorrAccX;
            ConnectDevList[index]->datainfo.accely = static_cast<short>((temp2[4]&0xFF)<<8|(temp2[5]&0xFF))+ConnectDevList[index]->Corr_Coe.CorrAccY;
            ConnectDevList[index]->datainfo.accelz = static_cast<short>((temp2[6]&0xFF)<<8|(temp2[7]&0xFF))+ConnectDevList[index]->Corr_Coe.CorrAccZ;
            ConnectDevList[index]->datainfo.tilt1 = static_cast<float>((temp2[8]&0xFF)<<8|(temp2[9]&0xFF))/1000+ConnectDevList[index]->Corr_Coe.CorrAlpha;
            ConnectDevList[index]->datainfo.tilt2 = static_cast<float>((temp2[10]&0xFF)<<8|(temp2[11]&0xFF))/1000+ConnectDevList[index]->Corr_Coe.CorrBeta;
            ConnectDevList[index]->datainfo.tilt3 = static_cast<float>((temp2[12]&0xFF)<<8|(temp2[13]&0xFF))/1000+ConnectDevList[index]->Corr_Coe.CorrGama;
        }
        else{
            ConnectDevList[index]->datainfo.motion = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            ConnectDevList[index]->datainfo.accelx = static_cast<short>((temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
            ConnectDevList[index]->datainfo.accely = static_cast<short>((temp2[4]&0xFF)<<8|(temp2[5]&0xFF));
            ConnectDevList[index]->datainfo.accelz = static_cast<short>((temp2[6]&0xFF)<<8|(temp2[7]&0xFF));
            ConnectDevList[index]->datainfo.tilt1 = static_cast<float>((temp2[8]&0xFF)<<8|(temp2[9]&0xFF))/1000;;
            ConnectDevList[index]->datainfo.tilt2 = static_cast<float>((temp2[10]&0xFF)<<8|(temp2[11]&0xFF))/1000;
            ConnectDevList[index]->datainfo.tilt3 = static_cast<float>((temp2[12]&0xFF)<<8|(temp2[13]&0xFF))/1000;;
        }

    }
    else if(fore[0]='1'&&fore[1]=='1'&&fore[2]=='2'){
        //1120 5a dd 98 7a   00 00 00 04   00 00   01 00   01 84   00 01   00(时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B)
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        //temp2=QByteArray::fromHex(temp1);//time

        ConnectDevList[index]->time=HexTimeToDec(temp1);
        // qDebug()<<"time2 is   "<<ConnectDevList[index];
        temp1.clear();
        temp2.clear();
        for(int q=12;q<19;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        ConnectDevList[index]->faultcode=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        for(int q=19;q<26;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        ConnectDevList[index]->fault=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        temp1.clear();
        temp2.clear();
        for(int q=26;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        ConnectDevList[index]->voltage = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));//dat[7];
        ConnectDevList[index]->memory =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        ConnectDevList[index]->workmode = temp2[4];
    }
    if(back[1]=='9'&&back[2]=='0'){
        //0900 00 00   30 39   00   00 46   00 a8  光强2B+IR2B+iUV1B+距离2B+温度2B
        //        38 6d    e9 57   00   10 00 de f
        //qDebug()<<"SensorDataConvert"<<back<<"   size is  "<<back.length()<<"method 3";
        //09000090033e00002f0181
        //00 90    03 3e   00    00 2f    01 81
        QByteArray  temp1,temp2;
        QString newbyte;
        //qDebug()<<"original "<<back;
        //qDebug()<<back[0]<<"  "<<back[1]<<" " <<back[2]<<" "<<back[3];
        for(int q=4;q<22;q++){
            temp1.push_back(back[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        //qDebug()<<"temp1 "<<temp1;
        // qDebug()<<"temp2 "<<temp2;
        ConnectDevList[index]->waitdata =false;
        ConnectDevList[index]->dataIntgrality=true;
        if(ConnectDevList[index]->m_bCorrectValue){
            ConnectDevList[index]->datainfo.intensity = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            ConnectDevList[index]->datainfo.IR =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF))+static_cast<short>(ConnectDevList[index]->Corr_Coe.CorrIR);
            ConnectDevList[index]->datainfo.iUV = temp2[4]+static_cast<short>(ConnectDevList[index]->Corr_Coe.CorrLight);

            short temp= static_cast<short>((temp2[5]&0xFF)<<8|(temp2[6]&0xFF));
            ConnectDevList[index]->datainfo.distance=temp*temp*ConnectDevList[index]->Corr_Coe.CorrDistanceA+ConnectDevList[index]->Corr_Coe.CorrDistanceB*temp+ConnectDevList[index]->Corr_Coe.CorrDistanceC;
            float tempe=0.1f*static_cast<float>((temp2[7]&0xFF)<<8|(temp2[8]&0xFF));
            ConnectDevList[index]->datainfo.temperature = tempe*tempe*ConnectDevList[index]->Corr_Coe.CorrTempA+tempe*ConnectDevList[index]->Corr_Coe.CorrTempB+ConnectDevList[index]->Corr_Coe.CorrTempC;
            // qDebug()<<"temprature "<<ConnectDevList[index]->datainfo.temperature;
        }else{
            ConnectDevList[index]->datainfo.intensity = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            ConnectDevList[index]->datainfo.IR =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
            ConnectDevList[index]->datainfo.iUV = temp2[4];
            ConnectDevList[index]->datainfo.distance = static_cast<short>((temp2[5]&0xFF)<<8|(temp2[6]&0xFF));
            ConnectDevList[index]->datainfo.temperature = 0.1f*static_cast<float>((temp2[7]&0xFF)<<8|(temp2[8]&0xFF));
            // qDebug()<<"temprature "<<ConnectDevList[index]->datainfo.temperature;
        }

    }
    ConnectDevList[index]->Method_exec();














    if(fore[0]='1'&&fore[1]=='2'&&fore[2]=='c'){
        //12c0 5a d9 47 26   00 4f   03 20   fd 72   00 13   00 00   ff d8   00 01
        //时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        ConnectDevList[index]->time=HexTimeToDec(temp1);
        temp1.clear();
        temp2.clear();
        QDateTime local(QDateTime::currentDateTime());
        QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
        ui->ComputerTime_label->setText(localTime);
        for(int q=12;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);

        ConnectDevList[index]->waitdata=true;
        ConnectDevList[index]->datainfo.motion = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
        ConnectDevList[index]->datainfo.accelx = static_cast<short>((temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        ConnectDevList[index]->datainfo.accely = static_cast<short>((temp2[4]&0xFF)<<8|(temp2[5]&0xFF));
        ConnectDevList[index]->datainfo.accelz = static_cast<short>((temp2[6]&0xFF)<<8|(temp2[7]&0xFF));
        ConnectDevList[index]->datainfo.tilt1 = static_cast<float>((temp2[8]&0xFF)<<8|(temp2[9]&0xFF))/1000;;
        ConnectDevList[index]->datainfo.tilt2 = static_cast<float>((temp2[10]&0xFF)<<8|(temp2[11]&0xFF))/1000;
        ConnectDevList[index]->datainfo.tilt3 = static_cast<float>((temp2[12]&0xFF)<<8|(temp2[13]&0xFF))/1000;;
    }
    else if(fore[0]='1'&&fore[1]=='1'&&fore[2]=='2'){
        //1120 5a dd 98 7a   00 00 00 04   00 00   01 00   01 84   00 01   00(时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B)
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        //temp2=QByteArray::fromHex(temp1);//time

        ConnectDevList[index]->time=HexTimeToDec(temp1);
        // qDebug()<<"time2 is   "<<ConnectDevList[index];
        temp1.clear();
        temp2.clear();
        for(int q=12;q<19;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        ConnectDevList[index]->faultcode=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        for(int q=19;q<26;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        ConnectDevList[index]->fault=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        temp1.clear();
        temp2.clear();
        for(int q=26;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        ConnectDevList[index]->voltage = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));//dat[7];
        ConnectDevList[index]->memory =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        ConnectDevList[index]->workmode = temp2[4];
    }
    if(back[1]=='9'&&back[2]=='0'){
        //0900 00 00   30 39   00   00 46   00 a8  光强2B+IR2B+iUV1B+距离2B+温度2B
        //        38 6d    e9 57   00   10 00 de f
        //qDebug()<<"SensorDataConvert"<<back<<"   size is  "<<back.length()<<"method 3";
        //09000090033e00002f0181
        //00 90    03 3e   00    00 2f    01 81
        QByteArray  temp1,temp2;
        QString newbyte;
        //qDebug()<<"original "<<back;
        //qDebug()<<back[0]<<"  "<<back[1]<<" " <<back[2]<<" "<<back[3];
        for(int q=4;q<22;q++){
            temp1.push_back(back[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        //qDebug()<<"temp1 "<<temp1;
        // qDebug()<<"temp2 "<<temp2;
        ConnectDevList[index]->waitdata =false;
        ConnectDevList[index]->datainfo.intensity = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
        ConnectDevList[index]->datainfo.IR =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        ConnectDevList[index]->datainfo.iUV = temp2[4];
        ConnectDevList[index]->datainfo.distance = static_cast<short>((temp2[5]&0xFF)<<8|(temp2[6]&0xFF));
        ConnectDevList[index]->datainfo.temperature = 0.1f*static_cast<float>((temp2[7]&0xFF)<<8|(temp2[8]&0xFF));
        // qDebug()<<"temprature "<<ConnectDevList[index]->datainfo.temperature;
        if(m_bDatabaseAcvated){
            bool r=insert(ConnectDevList[index]);
        }
    }
    if(ConnectDevList[index]->waitdata==false&&m_bCsv){
        //写入数据
        // qDebug()<<"bind";
        if(ConnectDevList[index]->mac==Csv[index]->GetBindMac()){
            FileOutput(ConnectDevList[index]->mac,ConnectDevList[index]->mac,1,ConnectDevList[index],Csv[index]);
        }
        else{
            for(int i=0;i<18;i++){
                if(ConnectDevList[index]->mac==Csv[i]->GetBindMac()){
                    FileOutput(ConnectDevList[index]->mac,ConnectDevList[index]->mac,1,ConnectDevList[index],Csv[i]);
                }
            }
        }
    }

    ShowDataOnTable();
}
void MainWidget::InqueryProc(QStringList &strList,int px=0){
    //"A80C6348591C", "honor Band 3-91c", "020a041109686f6e6f722042616e6420332d393163", "-87"
    if(strList[0].length()==12){
        for(int  q=0;q<BlockList.length();q++){
            if(strList[0]==BlockList[q]){
                return;
            }
        }
        bool find=false;//查询是否在搜索列表里面
        for(int q=0;q<ScanDevList.length();q++){
            if(ScanDevList[q]->mac==strList[0]){
                find=true;
                break;
            }
        }

        if(!find){
            if(strList[0].length()==12){
                ScanListItem *SLI=new ScanListItem;
                SLI->mac=strList[0];
                SLI->name=strList[1];
                if(px==0){
                    SLI->devPortnum=GlobeObject::port_;
                }
                else{
                    SLI->devPortnum=GlobeObject::port_2;
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
            if(DeviceList_Config[q]->mac==strList[0]){
                //直接连接
                signed char ty=static_cast<signed char>(strList[1].toInt());
                if(px==0){
                    ConnectActionTrigged(DeviceList_Config[q]->mac,GlobeObject::port_,ty);
                }
                else{
                    ConnectActionTrigged(DeviceList_Config[q]->mac,GlobeObject::port_2,ty);
                }

            }
        }
    }
}
void MainWidget::NotifyProc(QStringList &strList,int px=0){
    qDebug()<<"notify str list "<<strList;
    if(ConnectDevList.length()==0){
        if(px==0){
            DisconnetDeviceTrigged(strList[0],GlobeObject::port_);
        }
        else{
            DisconnetDeviceTrigged(strList[0],GlobeObject::port_2);
        }

    }
    for(int q=0;q<ConnectDevList.length();q++){
        if(ConnectDevList[q]->mac==strList[0]&&ConnectDevList[q]->name==""){
            ConnectDevList.removeAt(q);
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
            break;
        }
    }

    if(strList[2].length()==40){
        DataInfo_forepart=strList[2].toLatin1();
    }
    else{
        DataInfo_backend=strList[2].toLatin1();
    }

    //传电压过来的时候是这样的
    //info  "\r\n+NOTIFY:E5AEFA129DDC,fff1,1120386f4fcd000000000000000001a3000101\r\n\r\n+NOTIFY:E5AEFA129DDC,fff1,06b0386f4fcd0000\r\n"
    //notify str list  ("E5AEFA129DDC", "fff1", "1120386f4fcd000000000000000001a3000101E5AEFA129DDC", "fff1", "06b0386f4fcd0000")
    if(strList.length()>3){
        DataInfo_forepart=strList[2].toLatin1();
        DataInfo_backend=strList[4].toLatin1();
    }

    if(multiThread){
        if(false){
            qDebug()<<"isBusy"<<Data_Thread->isBusy();
            //SensorDataConvert(DataInfo_forepart,DataInfo_backend,strList[0]);
        }
        else
        {
            if(ConnectDevList.length()==0){
                qDebug()<<"无名设备data传入";
                return;
            }
            qDebug()<<"convert multithread ";
            DeviceInfo *newdevice=new DeviceInfo;
            int index=0;
            bool find=false;
            newdevice->datainfo=newdevice->datainfo;
            for(int i=0;i<ConnectDevList.length();i++){
                if(strList[0]==ConnectDevList[i]->mac){
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
                if(strList[0].length()==12){
                    newdevice->mac=strList[0];
                    index=ConnectDevList.length();
                    ConnectDevList.push_back(newdevice);
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

            emit DataNeedtoDeal(DataInfo_forepart,DataInfo_backend,ConnectDevList[index],index);



        }

    }
    else{
        SensorDataConvert(DataInfo_forepart,DataInfo_backend,strList[0]);
    }


    if(px==0){
        GetSingleSensorRSSI(strList[0],GlobeObject::port_);
    }
    else{
        GetSingleSensorRSSI(strList[0],GlobeObject::port_2);
    }
    if(px==0){

    }
    else{
        GetSingleSensorRSSI(strList[0],GlobeObject::port_2);
    }



}
void MainWidget::ReceiveFromthread(DeviceInfo *Dinfo, int index){
    ConnectDevList[index]=Dinfo;
    if(m_bDatabaseAcvated){
        bool r=insert(ConnectDevList[index]);
    }
    if(ConnectDevList[index]->waitdata==false&&m_bCsv){
        //写入数据
        // qDebug()<<"bind";
        if(ConnectDevList[index]->mac==Csv[index]->GetBindMac()){
            FileOutput(ConnectDevList[index]->mac,ConnectDevList[index]->mac,1,ConnectDevList[index],Csv[index]);
        }
        else{
            for(int i=0;i<18;i++){
                if(ConnectDevList[index]->mac==Csv[i]->GetBindMac()){
                    FileOutput(ConnectDevList[index]->mac,ConnectDevList[index]->mac,1,ConnectDevList[index],Csv[i]);
                }
            }
        }
    }
    ShowDataOnTable();
}
void MainWidget::CharacteristicProc(QStringList &strList,int px=0){
    //将设备加入列表
    bool find=false;
    for(int q=0;q<ConnectDevList.length();q++){
        if(strList[0]==ConnectDevList[q]->mac){
            find=true;
            ConnectDevList[q]->state=CONNECTED;
            if(ConnectDevList[q]->name==""){
                for(int m=0;m<ScanDevList.size();m++){
                    if(ScanDevList[m]->mac==strList[0]){
                        ConnectDevList[q]->name=ScanDevList[q]->name;
                        ConnectDevList[q]->rssi=ScanDevList[q]->rssi;
                        ConnectDevList[q]->addrtype=ScanDevList[q]->addrtype;
                        ConnectDevList[q]->devPortnum=ScanDevList[q]->devPortnum;
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
        if(strList[0].length()!=12){
            return;
        }
        DeviceInfo *newde=new DeviceInfo;
        newde->mac=strList[0];
        int p=-1;
        bool find=false;
        for(int q=0;q<ScanDevList.size();q++){
            if(ScanDevList[q]->mac==strList[0]){
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
            if(DeviceList_Config[m]->mac==strList[0]){
                fid=true;
                qDebug()<<"connection from config ";
                ConnectDevList.push_back(DeviceList_Config[m]);
                if(px==0){
                    ConnectDevList.last()->devPortnum=GlobeObject::port_;
                }
                else{
                    ConnectDevList.last()->devPortnum=GlobeObject::port_2;
                }

                delete newde;
                newde=nullptr;
                break;
            }
        }
        if(!fid){
            ConnectDevList.push_back(newde);
        }

        ConnectDevList.last()->state=CONNECTED;
        PromptInfomation("连接"+strList[0]+"成功");
        if(m_bCsv){
            if(ConnectDevList.length()>0){
                if(Csv[ConnectDevList.length()-1]==nullptr){
                    Csv[ConnectDevList.length()-1]=new CsvWriter(ConnectDevList.last()->mac);//绑定写入器
                }
            }
        }
        if(px==0){
            ReadSingelDeviceData(strList[0],GlobeObject::port_);//这个发送了才会开始广播数据
        }
        else{
            ReadSingelDeviceData(strList[0],GlobeObject::port_2);
        }

    }
    AddDevicetoConnectedTree();
    AddDeviceToNewDeviceTree();


}
void MainWidget::DiscProc(QStringList &info){
    //剩下就是mac了
    PromptInfomation("断开连接"+info[0]);
    for(int i=0;i<ConnectDevList.length();i++){
        qDebug()<<ConnectDevList[i]->mac<<"  "<<info[0];
        if(ConnectDevList[i]->mac==info[0]){
            ConnectDevList.removeAt(i);
            qDebug()<<"remove  "<<info[0];
            if(FocusIndex==i){
                if(FocusIndex=-1);
            }
            AddDevicetoConnectedTree();
            AddDeviceToNewDeviceTree();
            break;
        }
    }
    //解除绑定
    if(m_bCsv){
        for(int i=0;i<18;i++){
            if(Csv[i]!=nullptr){
                if(Csv[i]->GetBindMac()==info[0]){
                    delete Csv[i];
                    Csv[i]=nullptr;
                }
            }

        }
    }
    //on_Exit_pushButton_clicked();

}
QString MainWidget::TimeStyleConvert(QByteArray tim){


    QString s;
    return s;
}
void MainWidget::GetDataFromProcessor(QStringList info){
    //用于数据显示
}
void MainWidget::ReadDevice(QString mac,quint16 po=0){
    if(mac.length()==12)
    {
        QString str="AT+READCHAR="+mac+","+gRxChar+"\r\n";
        if(m_bTcp_1_con&&po==GlobeObject::port_){
            GlobeObject::Socket_->write(str.toLatin1(),str.length());
        }
        else if(m_bTcp_2_con&&po==GlobeObject::port_2){
            GlobeObject::Socket_2->write(str.toLatin1(),str.length());
        }


    }
}
void MainWidget::OtherDeviceMenu(const QPoint& pos){
    //如何检查pos位置
    qDebug()<<pos;
    QModelIndex curIndex =ui->OtherDevicetree_Wiew->indexAt(pos);//索引值
    qDebug()<<curIndex;
    QModelIndex index = curIndex.sibling(curIndex.row(),0);
    //获取点击设备的mac
    QString IndexMac=index.data().toString();
    qDebug()<<IndexMac;
    if(IndexMac.length()!=12){
        qDebug()<<"mac is not 12 long";
        return;
    }
    //获取type
    signed char ty;
    quint16 Portnum=0;
    for(int q=0;q<ScanDevList.length();q++){
        if(IndexMac==ScanDevList[q]->mac){
            ty=ScanDevList[q]->addrtype;
            Portnum=ScanDevList[q]->devPortnum;
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
    //  qDebug()<<"choose  mac is "<<IndexMac;
    //获取type
    signed char ty;

    for(int q=0;q<ScanDevList.length();q++){
        if(IndexMac==ScanDevList[q]->mac){
            ty=ScanDevList[q]->addrtype;


        }
    }
    quint16 po=0;
    for(int q=0;q<ConnectDevList.length();q++){
        if(IndexMac==ConnectDevList[q]->mac)
            po=ConnectDevList[q]->devPortnum;
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
    for(int i=0;i<ConnectDevList.length();i++){
        if(ConnectDevList[i]->mac==Mac){
            ConnectDevList[i]->newname=Name;
            qDebug()<<"change the name "<<Name;
            break;
        }
    }
    AddDevicetoConnectedTree();
}
void MainWidget::ConnectActionTrigged(QString Mac,quint16 port,signed char ty){
    //qDebug()<<"trigged!";
    // qDebug()<<Mac<<" "<<ty;
    qDebug()<<"port   "<<port;
    qDebug()<<"port 1 "<<GlobeObject::port_<<"port 2 "<<GlobeObject::port_2;
    if(Mac.length()==12)
    {
        // index = GetSensorIndex(addr);
        //此处要获取设备信息，确定设备类型
        QString strMsg="AT+CONN="+Mac+","+QString::number(ty)+"\r\n";//0表示没链接吗
        // qDebug()<<strMsg;
        if(port==GlobeObject::port_&&m_bTcp_1_con){
            GlobeObject::Socket_->write(strMsg.toLatin1(),strMsg.length());//
            GetSingleSensorRSSI(Mac,port);
        }
        if(port==GlobeObject::port_2&&m_bTcp_2_con){
            GlobeObject::Socket_2->write(strMsg.toLatin1(),strMsg.length());//
            GetSingleSensorRSSI(Mac,port);
        }
        PromptInfomation("正在连接"+Mac);
    }
}
void MainWidget::DisconnetDeviceTrigged(QString Mac,quint16 p=0){
    // qDebug()<<"dis connecte";
    if(Mac.length()==12)
    {
        QString strMsg="AT+DISC="+Mac+"\r\n";
        qDebug()<<"本机发送断开请求"<<strMsg;
        if(p==GlobeObject::port_&&m_bTcp_1_con){
            GlobeObject::Socket_->write(strMsg.toLatin1(),strMsg.length());//
        }
        else if(m_bTcp_2_con){
            GlobeObject::Socket_2->write(strMsg.toLatin1(),strMsg.length());//
        }


    }
    else{
        qDebug()<<"mac is not 12 bit";
    }


}
void MainWidget::UI_initial(){

}
void MainWidget::TimerTimerOut(){
    qDebug()<<"timer time out";
    // DisconnetDeviceTrigged(testuseMac);

}
void MainWidget::ConnectChoosedDevice(){
    qDebug()<<"connect button clicked!";

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
    if(ConnectDevList.size()>=0){

        if(model_Con!=nullptr){
            delete model_Con;
            model_Con=nullptr;

        }
        model_Con = new QStandardItemModel(ui->ConnectedDevice_treeView);
        model_Con->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("MAC")<<QStringLiteral("name"));

        if(GlobeObject::port_!=0&&m_bTcp_1_con){
            QStandardItem* item_port_1 = new QStandardItem(QString::number(GlobeObject::port_));
            model_Con->appendRow(item_port_1);
            item_port_1->setEditable(false);

            for(int i=0;i<ConnectDevList.size();i++){
                if(ConnectDevList[i]->devPortnum==GlobeObject::port_){
                    QStandardItem* itemchannel = new QStandardItem(ConnectDevList[i]->mac);
                    item_port_1->appendRow(itemchannel);
                    if(ConnectDevList[i]->newname==""){
                        item_port_1->setChild(itemchannel->index().row(),1,new QStandardItem(ConnectDevList[i]->name));
                    }
                    else{
                        item_port_1->setChild(itemchannel->index().row(),1,new QStandardItem(ConnectDevList[i]->newname));
                    }

                    item_port_1->setEditable(false);
                }

            }
        }
        if(GlobeObject::port_2!=0&&m_bTcp_2_con){
            QStandardItem* item_port_2 = new QStandardItem(QString::number(GlobeObject::port_2));
            model_Con->appendRow(item_port_2);
            for(int i=0;i<ConnectDevList.size();i++){
                if(ConnectDevList[i]->devPortnum==GlobeObject::port_2){
                    QStandardItem* itemchannel = new QStandardItem(ConnectDevList[i]->mac);
                    item_port_2->appendRow(itemchannel);
                    if(ConnectDevList[i]->newname==""){
                        item_port_2->setChild(itemchannel->index().row(),1,new QStandardItem(ConnectDevList[i]->name));
                    }
                    else{
                        item_port_2->setChild(itemchannel->index().row(),1,new QStandardItem(ConnectDevList[i]->newname));
                    }
                    item_port_2->setEditable(false);
                }

            }


        }



    }

    QString Countshow=QString("连接设备数量: %1;上限: %2").arg(ConnectDevList.size()).arg(MAX_DEV_COUNT);
    ui->DeviceCount_label->setText(Countshow);
    UpdateTreeView();
}

void MainWidget::AddDeviceToNewDeviceTree(){
    //扫描设备并添加到未连接列表
    if(ScanDevList.size()>=0){

        if(model_Scan!=nullptr){
            delete model_Scan;
            model_Scan=nullptr;

        }
        model_Scan= new QStandardItemModel(ui->OtherDevicetree_Wiew);
        model_Scan->setColumnCount(2);
        model_Scan->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("MAC")<<QStringLiteral("name"));
        for(int i=0;i<ScanDevList.size();i++){
            QStandardItem* itemMac = new QStandardItem(ScanDevList[i]->mac);
            itemMac->setEditable(false);
            QStandardItem* itemname = new QStandardItem(ScanDevList[i]->name);
            itemname->setEditable(false);
            model_Scan->setItem(i,0,itemMac);
            model_Scan->setItem(i,1,itemname);

        }

        QString Countshow=QString("扫描设备数量: %1/%2").arg(ScanDevList.size()).arg(MAX_SCAN_COUNT);
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
    //获取点击设备的mac
    QString IndexMac=info.data().toString();
    qDebug()<<IndexMac.length()<<index;
    if(IndexMac.length()!=12){
        return;
    }
    int OriFocus=FocusIndex;
    //GetSingleSensorRSSI(IndexMac);
    bool find=false;
    for(int q=0;q<ConnectDevList.size();q++){
        if(ConnectDevList[q]->mac==IndexMac){
            FocusIndex=q;
            FocusDevice=ConnectDevList[q];
            find=true;
            break;
        }
    }
    if(!find){
        FocusIndex=-1;
    }
    if(OriFocus!=-1&&OriFocus!=FocusIndex){
        DataBuff_AccX.clear();
        DataBuff_AccY.clear();
        DataBuff_AccZ.clear();
        DataBuff_Tilt1.clear();
        DataBuff_Tilt2.clear();
        DataBuff_Tilt3.clear();
        DrawCount=0;
    }
}
void MainWidget::PrepareInquire()
{
    int i;
    int maxcount=static_cast<int>(MAX_DEV_COUNT);
    for(i = 0;i <maxcount;i++)
    {
        if(ConnectDevList.isEmpty()){
            break;
        }
        if(!BdaddrIsZero(ConnectDevList[i]->mac)&&ConnectDevList[i]->state==ADVERTISING)
        {
            ConnectDevList[i]->state = POWER_OFF;
            ConnectDevList[i]->connfailcount = 0;
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
        if(GlobeObject::port_!=0&&m_bTcp_1_con){
            GlobeObject::Socket_->write(cmdstr,static_cast<int>(len));
        }
        qDebug()<<"port2 ";
        if(GlobeObject::port_2!=0&&m_bTcp_2_con){
            GlobeObject::Socket_2->write(cmdstr,static_cast<int>(len));
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
    SerialPortDialog *spd=new SerialPortDialog(GlobeObject::serial,this);
    connect(spd,&SerialPortDialog::serialOpened,this,&MainWidget::SerailState);
    spd->show();
}
void MainWidget::SerailState(QString name){
    ui->Serial_stateLabel->setText(name);
}
void MainWidget::FileOutput(QString str, QString add="", int method=0, DeviceInfo *df=nullptr,CsvWriter *writer=nullptr){
    //method ==0  覆盖
    //method ！=0 追加
    if(df!=nullptr&&writer!=nullptr){
        QString Outstr;
        //"Time,Mac,Name,AccX,AccY,AccZ,alpha,beta,gama,motion,IR,iuv,distance,Intensity,temperture\n""
        Outstr+=df->time+","+df->mac+","+df->name+","+QString::number(df->datainfo.accelx)+
                ","+QString::number(df->datainfo.accely)+","+QString::number(df->datainfo.accelz)+","+
                QString::number(df->datainfo.tilt1)+","+QString::number(df->datainfo.tilt2)+","
                +QString::number(df->datainfo.tilt3)+","+QString::number(df->datainfo.motion)+
                ","+QString::number(df->datainfo.IR)+","+QString::number(df->datainfo.iUV)+","+
                QString::number(df->datainfo.distance)+","+QString::number(df->datainfo.intensity)+
                ","+QString::number(df->datainfo.temperature)+"\n";


        //qDebug()<<"write  "<<Outstr;
        if(writer->m_bOpen==true){

            *(writer->GetOut())<<Outstr;
        }
    }
    else {
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
                    QString str(line);

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
        QString Countshow=QString("扫描设备数量: %1;上限: %2").arg(ScanDevList.size()).arg(MAX_SCAN_COUNT);
        ui->DeviceCount_label->setText(Countshow);
    }
    else{
        //这个页面则是数据提取
        PromptInfomation("Connected List clicked!");
        QString Countshow=QString("连接设备数量: %1;上限: %2").arg(ConnectDevList.size()).arg(MAX_DEV_COUNT);
        ui->DeviceCount_label->setText(Countshow);

    }
}
void MainWidget::ConnectToDevice(QModelIndex curIndex){



}
void MainWidget::DataParser(QByteArray data){
    int len=data.length();
    //11205add987a00000004000001000184000100(时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B)


}
void MainWidget::ReadSingelDeviceData(QString Mac,quint16 po){
    if(Mac.length()==12)
    {
        if(po==GlobeObject::port_){
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
        if(po==GlobeObject::port_&&m_bTcp_1_con){

            GlobeObject::Socket_->write(inqRssi.toLatin1(),inqRssi.length());
        }
        else if(m_bTcp_2_con){
            GlobeObject::Socket_2->write(inqRssi.toLatin1(),inqRssi.length());
        }
    }
}
void MainWidget::SendDatatoDev(QString Mac,QString charc,QString dat,int po=0){
    if(Mac.length()==12)
    {

        QString sendData="AT+WRITECHAR="+Mac+","+charc+","+dat+"\r\n";
        if(po==0&&m_bTcp_1_con){
            GlobeObject::Socket_->write(sendData.toLatin1(),sendData.length());
        }
        else if(m_bTcp_2_con){
            GlobeObject::Socket_2->write(sendData.toLatin1(),sendData.length());
        }

    }

}
void MainWidget::on_Exit_pushButton_clicked()
{
    if(ConnectDevList.length()>0){
        for(int i=0;i<ConnectDevList.length();i++){
            DisconnetDeviceTrigged(ConnectDevList[i]->mac);
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
    ConfigerFileSupvisor(WRITE_CON);

    Db.close();

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
    ui->Fault_label->setText(QString::number(df.fault));
    ui->Movement_label->setText(QString::number(df.datainfo.motion));
}
void MainWidget::ShowDataOnTable(){
    if(FocusIndex<0){
        qDebug()<<"Focus Index "<<FocusIndex;
        return;
    }
    qDebug()<<"focus "<<FocusIndex;
    //DeviceInfo &df=ConnectDevList[FocusIndex];
    if(FocusIndex>=ConnectDevList.length()){
        return;
    }
    if(ConnectDevList.length()==1){
        FocusIndex=0;
    }
    ui->AccX_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.accelx));
    ui->AccY_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.accely));
    ui->AccZ_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.accelz));
    ui->Alpha_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.tilt1));
    ui->Beta_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.tilt2));
    ui->Gama_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.tilt3));
    ui->DeviceTime_label->setText(ConnectDevList[FocusIndex]->time);
    ui->DataVoltage_label->setText(QString::number(ConnectDevList[FocusIndex]->voltage));
    ui->DeviceMac_label->setText(ConnectDevList[FocusIndex]->mac);
    ui->DeviceMode_label->setText(QString(ConnectDevList[FocusIndex]->mode));
    ui->Temperature_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.temperature));
    ui->FaultCode_label->setText(QString::number(ConnectDevList[FocusIndex]->faultcode));

    //ui->DeviceName_label->setText(ConnectDevList[FocusIndex]->name);
    if(ConnectDevList[FocusIndex]->newname==""){
        ui->DeviceName_label->setText(ConnectDevList[FocusIndex]->name);

    }
    else{
        ui->DeviceName_label->setText(ConnectDevList[FocusIndex]->newname);
    }

    ui->IR_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.IR));
    ui->Fault_label->setText(QString::number(ConnectDevList[FocusIndex]->fault));
    ui->Movement_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.motion));
    ui->IUV_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.iUV));
    ui->Distance_label->setText(QString::number(ConnectDevList[FocusIndex]->datainfo.distance));
    ui->DeviceSignal_label->setText(QString::number(ConnectDevList[FocusIndex]->rssi));


    if(m_bShowOnCharts){
        DrawCount++;
        if(DrawCount>=DrawMaxCount){
            DataBuff_AccX.pop_front();
            DataBuff_AccY.pop_front();
            DataBuff_AccZ.pop_front();
            DataBuff_Tilt1.pop_front();
            DataBuff_Tilt2.pop_front();
            DataBuff_Tilt3.pop_front();
            //qDebug()<<"Draw Count   "<<DrawCount;
        }
        DataBuff_AccX.push_back(ConnectDevList[FocusIndex]->datainfo.accelx);
        DataBuff_AccY.push_back(ConnectDevList[FocusIndex]->datainfo.accely);
        DataBuff_AccZ.push_back(ConnectDevList[FocusIndex]->datainfo.accelz);
        DataBuff_Tilt1.push_back(ConnectDevList[FocusIndex]->datainfo.tilt1);
        DataBuff_Tilt2.push_back(ConnectDevList[FocusIndex]->datainfo.tilt2);
        DataBuff_Tilt3.push_back(ConnectDevList[FocusIndex]->datainfo.tilt3);
    }
    ChartDrawer();
}
void MainWidget::on_DataConfig_pushButton_clicked()
{
    DataConDialog=new DataConfigDialog(&ConnectDevList,this);
    DataConDialog->show();
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
    if(ConnectDevList.length()>0){
        for(int i=0;i<ConnectDevList.length();i++){
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
            SendDatatoDev(ConnectDevList[i]->mac,gRxChar,meg);
        }
    }

}
void MainWidget::ChartDrawer(){
    //ui->Acceler_Chart->setRenderHint(QPainter::Antialiasing);
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

    chart[0]->addSeries(series[0]);
    chart[0]->addSeries(series[1]);
    chart[0]->addSeries(series[2]);
    chart[0]->createDefaultAxes();
    chart[0]->axisX()->setRange(0,20);
    chart[0]->axisY()->setRange(-1200,1200);
    chart[0]->setMargins(QMargins(0,0,0,0));//铺满
    //chart[0]->setAnimationOptions(QChart::SeriesAnimations);  //开启会使绘制变成动画效果，但是不适合移动窗口
    chart[1]->addSeries(series[3]);
    chart[1]->addSeries(series[4]);
    chart[1]->addSeries(series[5]);
    chart[1]->createDefaultAxes();
    chart[1]->axisX()->setRange(0,20);
    chart[1]->axisY()->setRange(-1.2,1.2);
    chart[1]->setMargins(QMargins(0,0,0,0));//铺满
    // chart[1]->setAnimationOptions(QChart::SeriesAnimations);  //在缩放窗口大小时可以用动画进行美化


    ui->Acceler_Chart->setAutoFillBackground(true);

    series[0]->setColor(Qt::red);
    series[1]->setColor(Qt::blue);
    series[2]->setColor(Qt::green);

    series[3]->setColor(Qt::red);
    series[4]->setColor(Qt::blue);
    series[5]->setColor(Qt::green);



    // QTime current_time =QTime::currentTime();
    // int second = current_time.second();
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


    for(int i=0; i<DataBuff_AccX.length();i++)
    {
        series[0]->append(i,DataBuff_AccX[i]);
        series[1]->append(i,DataBuff_AccY[i]);
        series[2]->append(i,DataBuff_AccZ[i]);
        series[3]->append(i,DataBuff_Tilt1[i]);
        series[4]->append(i,DataBuff_Tilt2[i]);
        series[5]->append(i,DataBuff_Tilt3[i]);
        // qDebug()<<"data X "<<i<<"  "<<DataBuff_AccX[i]<<"  size  "<<DataBuff_AccX.length();
        //qDebug()<<"data Y "<<i<<"  "<<DataBuff_AccY[i]<<"  size  "<<DataBuff_AccY.length();
        // qDebug()<<"data Z "<<i<<"  "<<DataBuff_AccZ[i]<<"  size  "<<DataBuff_AccZ.length();
    }
    ui->Acceler_Chart->setChart(chart[0]);
    ui->Tilt_Chart->setChart(chart[1]);
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

            int methodcount=configIniWriter->value("/Device"+QString::number(i)+
                                                   "/Method_count").toString().toInt();
            for(int q=0;q<methodcount;q++){
                Method *M=new Method;

                M->_Log=_Logi(configIniWriter->value("/Device"+QString::number(i)+"/Method_Log").toString().toInt());
                M->i_sym_one=_type(configIniWriter->value("/Device"+QString::number(i)+"/Method_sym_one").toInt());
                M->i_sym_two=_type(configIniWriter->value("/Device"+QString::number(i)+"/Method_sym_two").toInt());
                M->i_value=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_int").toInt();
                M->i_value_2=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_2_int").toInt();
                M->f_value=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_float").toFloat();
                M->f_value_2=configIniWriter->value("/Device"+QString::number(i)+"/Method_value_2_float").toFloat();
                M->Message_=configIniWriter->value("/Device"+QString::number(i)+"/Method_Message_").toInt();
                M->sequece=configIniWriter->value("/Device"+QString::number(i)+"/Method_sequece").toInt();
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
        qDebug() << QObject::tr("数据库表创建成功!");
        return true;
    }
    else
    {
        qDebug()<<query.lastError();
        qDebug() << QObject::tr("数据库表创建失败!");
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
        qDebug() << QObject::tr("数据库表创建成功！\n");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("数据库表创建失败！\n");
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
        qDebug() << lastError.driverText() << QString(QObject::tr("插入失败"));
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
    query.prepare(QString("delete from automobil where id = %1").arg(id));
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
        qDebug() << lastError.driverText() << QString(QObject::tr("更新失败"));
    }
    return true;
}
//排序
bool MainWidget::sortById()
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    bool success=query.exec("select * from automobil order by id desc");
    if(success)
    {
        qDebug() << QObject::tr("Sort Success!");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("Sort Failed");
        return false;
    }
}
void MainWidget::on_EnvironmentConfig_pushButton_clicked()
{
    qDebug()<<"data before "<<ConfigAddress<<" \n "<<DBaddr<<"\n "<<CsvUpperAddr<<"\n "<<m_bShowOnCharts<<"\n"<<multiThread<<
              "\n"<<m_bConfigAcvated<<"\n"<<m_bCsv<<"\n"<<m_bDatabaseAcvated;
    env=new EnvironmentConfig(ConfigAddress,CsvUpperAddr,DBaddr,m_bShowOnCharts,
                              multiThread,m_bConfigAcvated,m_bCsv,m_bDatabaseAcvated);
    connect(env,&EnvironmentConfig::envdone,this,&MainWidget::FromEnvDialog);
    connect(env,&EnvironmentConfig::changeIp,this,&MainWidget::ResetIp);
    env->show();
}
void MainWidget::FromEnvDialog(QString ConAddr, QString CsvAddr,
                               QString DBaddr, bool label, bool multithread, bool Csv,
                               bool database, bool Con_ac){
    ConfigAddress=ConAddr;
    this->DBaddr=DBaddr;
    CsvUpperAddr=CsvAddr;
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
    GlobeObject::Server_->listen(QHostAddress(ipaddr),GlobeObject::port_);
    ui->ip_label->setText("重设ip"+ipaddr);
}
void MainWidget::on_Config_pushButton_clicked()
{
    Sc=new SensorConfig_Dialog(ConnectDevList,this);

    Sc->show();
}
void MainWidget::ThreadLunchar(){
    //open a thread for data processing
    if(multiThread){
        if(Pthread==nullptr){
            PromptInfomation("启动数据线程");
            Pthread=new QThread;
        }
        if(Data_Thread==nullptr){
            Data_Thread=new DataThread(this);
            Data_Thread->moveToThread(Pthread);
            Data_Thread->SetRunning(true);
            Pthread->start();
            // connect(this,&MainWidget::DataPthreadStart,Data_Thread,&DataThread::DataProcess);
            connect(this,&MainWidget::DataNeedtoDeal,Data_Thread,&DataThread::DataProcess_Simple);
            connect(Data_Thread,&DataThread::workDone,this,&MainWidget::ReceiveFromthread);
            //emit DataPthreadStart(info);
            qRegisterMetaType<DeviceInfo>("DeviceInfo");
        }
        if(Data_Thread->isRunning()){
            PromptInfomation("启动成功");
        }
        else{
            PromptInfomation("启动失败");
            multiThread=false;
        }
    }


}
void MainWidget::closeEvent(QCloseEvent *event){
    event->accept();
    on_Exit_pushButton_clicked();
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
