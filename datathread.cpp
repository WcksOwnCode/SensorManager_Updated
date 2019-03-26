#pragma execution_character_set("utf-8")
#include "datathread.h"
#include <math.h>
#include "mainwidget.h"
#include <QMessageBox>
DataThread::DataThread(QObject *parent)
{


    //MainWidget *ptr = (MainWidget*)parentWidget();
}
DataThread::~DataThread(){
    //qDebug()<<QThread::currentThreadId();
    //qDebug()<<"~DataThread() 1";
    //GlobalObject::DesTroy();
    //qDebug()<<"~DataThread() 2";
    //qDebug()<<"Thread id  "<<QThread::currentThreadId()<<QThread::currentThread();

}
void DataThread::CloseServer(){
    qDebug()<<"close server";
  if(m_bTcp_1_con==true){
     Socket_->close();
      Server_->close();

  }

  if(m_bTcp_2_con==true){
       Socket_2->close();
      Server_2->close();

  }
  qDebug()<<"close server done";
}
QStringList DataThread::StringFileter(QString info,int ){
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
    while(pos<len){
        if(lists[pos]=="INQRESULT"||lists[pos]=="NOTIFY"
                ||lists[pos]=="CHARACTERISTIC"||lists[pos]=="RSSI"
                ||lists[pos]=="READCHAR"|lists[pos]=="DISC"
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


    ret=meg.split(";");
    QStringList returnvalue;
    for(int i=0;i<ret.length();i++){
        infoSpliter(ret[i],returnvalue);
    }

    return ret;

}
void DataThread::DataProcess(QString info, int p){
    //0----INQRESULT
    //1----NOTIFY
    //2----CHARACTERISTIC:
    //3----RSSI
    //4----READCHAR
    //5----DISC
    //6----ignore message
    //7-----INQCOMPLETE
    //-1---出错
    /*  qDebug()<<"info "<<info;
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
                for(int q=0;q<GlobalObject::g_ConnectDevList.length();q++){
                    qDebug()<<"q "<<q<<"str "<<strList[2];
                    if(GlobalObject::g_ConnectDevList[q]->mac==strList[1]){
                        GlobalObject::g_ConnectDevList[q]->rssi=static_cast<short>(HexToDec(strList[2]))-256;
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
                PromptInfomation(tr("搜索完成"));
                m_bSearching=false;
                break;

            }
            qDebug()<<"after spilter  "<<strList;
        }

*/
}
void DataThread::SetRunning(bool tr){
    m_bisRunning=tr;
}


void DataThread::ReciveFromConnected(QString s){
     emit _ShortInfo(s,0);
}
void DataThread::DataProcess_Simple(QByteArray fore, QByteArray back, DeviceInfo *dev, int index){
    m_bBusy=true;//
    GlobalObject::locker->lock();
   // qDebug()<<"data thread"<<fore<<"   "<<back;
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
        dev->datainfo.tilt1 = HexTilteToDec(temp1).toFloat()/10.0f;
        temp1.clear();
        for(int q=32;q<36;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.tilt2 =HexTilteToDec(temp1).toFloat()/10.0f;
        temp1.clear();
        for(int q=36;q<40;q++){
            temp1.push_back(fore[q]);
        }
        dev->datainfo.tilt3 = HexTilteToDec(temp1).toFloat()/10.0f;
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
           // qDebug()<<"dev->Corr_Coe.CorrTempA"<<dev->Corr_Coe.CorrTempA<<"dev->Corr_Coe.CorrTempB "<<dev->Corr_Coe.CorrTempB<<"dev->Corr_Coe.CorrTempC "<<dev->Corr_Coe.CorrTempC;
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
        if(dev->removedSendState.length()!=0){
            for(int i=0;i<dev->removedSendState.length();i++){
                if(int(dev->removedSendState.at(i).y())==5){
                    _ShortInfo("设备"+dev->name+"发送次数超过限制,无应答",0);
                }
                if(int(dev->removedSendState.at(i).y())==6){
                    _ShortInfo("设备"+dev->name+"发送时间超过限制,无应答",0);
                }

            }
            dev->removedSendState.clear();
        }
    }
    qDebug()<<"exec done";
    m_bBusy=false;
    dev->dataget[0]=false;
    dev->dataget[1]=false;
    if(FocusIndex==index){
      emit workDone(dev,index);
    }

    bool lastState=dev->m_bIntime;
    dev->CheckDataTimeStamp();


    if(lastState!=dev->m_bIntime){
        emit updateTree();
    }
    dev->lastDataTimeStamp=QDateTime::currentDateTime();
    emit InsertDataToDataBase(dev);//write to database

    /*if(GlobalObject::g_ConnectDevList->at(index)->waitdata==false&&m_bCsv){
        //写入数据
        //qDebug()<<"bind";
        if(Csv[index]!=nullptr){
            if(GlobalObject::g_ConnectDevList->at(index)->mac==Csv[index]->GetBindMac()){
                FileOutput(GlobalObject::g_ConnectDevList->at(index)->mac,GlobalObject::g_ConnectDevList->at(index)->mac,1,GlobalObject::g_ConnectDevList->at(index),Csv[index]);
            }
            else{
                //qDebug()<<"to find writer ";
                for(int i=0;i<18;i++){
                    if(Csv[i]!=nullptr){
                        if(GlobalObject::g_ConnectDevList->at(index)->mac==Csv[i]->GetBindMac()){
                            FileOutput(GlobalObject::g_ConnectDevList->at(index)->mac,GlobalObject::g_ConnectDevList->at(index)->mac,1,GlobalObject::g_ConnectDevList->at(index),Csv[i]);
                            break;
                        }
                    }

                }
            }
        }

    }*/



    GlobalObject::locker->unlock();


}
QString DataThread::HexTimeToDec(QByteArray t){
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
QString DataThread::HexToDec(QByteArray arr)
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
unsigned int DataThread::HexToDec(QString t){
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
QString DataThread::HexTilteToDec(QByteArray arr){
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
        //qDebug()<<"more "<<value;
        value-=65536;
       // qDebug()<<"adasd"<<value;
    }
  //  qDebug()<<"sub thread id  "<<QThread::currentThreadId();
    return QString::number(value);





}
void DataThread::ReconnectDevice(){
    //这个函数是用来连接在已经连接的设备列表中断开的设备

    qDebug()<<tr("设备重连函数");

    int length=GlobalObject::g_ConnectDevList->length();
    for(int i=0;i<length;i++){
        if(GlobalObject::g_ConnectDevList->at(i)->m_bIntime==false){//确认已经断开的
            if(GlobalObject::g_ConnectDevList->at(i)->ReCon<GlobalObject::g_ConnectDevList->at(i)->Reconnect_Count_Max){
               ConnectDevice(GlobalObject::g_ConnectDevList->at(i)->mac,GlobalObject::g_ConnectDevList->at(i)->devPortnum,1); //第三参数我记不得是多少了
               GlobalObject::g_ConnectDevList->at(i)->ReCon++;
               emit _ShortInfo(tr("正在重连 ")+GlobalObject::g_ConnectDevList->at(i)->mac,0);
            }
            else{
                //已经超过重连次数
                GlobalObject::g_DeviceList_Config->push_back(GlobalObject::g_ConnectDevList->at(i));//放进备用链接列表，如果扫描到这个设备又会连接上
                GlobalObject::g_ConnectDevList->removeAt(i);
                length--;
            }
        }
    }
}
void DataThread::Thread_infoHandler(QString info, int p){
    //0----INQRESULT
    //1----NOTIFY
    //2----CHARACTERISTIC:
    //3----RSSI
    //4----READCHAR
    //5----DISC
    //6----ignore message
    //7-----INQCOMPLETE
    //-1---出错
   // qDebug()<<"thread  info "<<info;
    QStringList strlist =StringFileter(info,p);
    for(int q=0;q<strlist.length();q++){
        QStringList strList;
        int ret = infoSpliter(strlist[q],strList);

        switch (ret) {
        case -1:
            //qDebug()<<"ret error";
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
                //qDebug()<<"q "<<q<<"str "<<strList[2];
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
            //ok or error, just ignore it
            break;
        case 7:
            // PromptInfomation(tr("搜索完成"));
            //  m_bSearching=false;
            break;

        }
        //   qDebug()<<"after spilter  "<<strList;
    }
}
int  DataThread::infoSpliter(QString info,QStringList &returnstr){
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
        // PromptInfomation("搜索完成");
        // m_bSearching=false;
        return 7;
    }
    if(info.contains("OK",Qt::CaseInsensitive)){
        //来自数据
       // qDebug()<<"handle ok";
        return 6;
    }
    if(info.contains("ERROR",Qt::CaseInsensitive)){
        //qDebug()<<"handle error";
        return 6;
    }
    if(info.contains("CHARACTERISTIC",Qt::CaseInsensitive)){

        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        return 2;
    }
    if(info.contains("DISC")){
        // "\r\n+DISC:E5AEFA129DDC\r\n"
       // qDebug()<<"before   "<<info;
        //info.remove(QString("DISC"), Qt::CaseInsensitive);
       // qDebug()<<"after  "<<info;
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

        //qDebug()<<"befor sp "<<info;
        //befor sp  "RSSI,E5AEFA129DDC,b4,"
        returnstr.clear();
        returnstr= info.split(QRegExp(","));
        //qDebug()<<"after sp "<<returnstr;
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
       // qDebug()<<"other type info :\n"<<info<<"\n";
        return -1;
    }
}
void DataThread::InqueryProc(QStringList &strList,int px){
    //" INQRESULT; A80C6348591C; 0; name; 0; 031900000201060302121808ff7d020103002082; -74;"
    qDebug()<<"InqueryProc";
    if(strList[1].length()==12&&strList[3]!=""){
        for(int q=0;q<GlobalObject::g_BlockList->length();q++){
            if(strList[1]==GlobalObject::g_BlockList->at(q)){
                return;
            }
        }
        bool find=false;//check is it in the scanlist
        for(int q=0;q<GlobalObject::g_ScanDevList->length();q++){
            if((GlobalObject::g_ScanDevList->at(q))->mac==strList[1]){
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
                if(GlobalObject::g_ScanDevList->length()<32){
                    GlobalObject::g_ScanDevList->push_back(SLI);
                }
                //AddDeviceToNewDeviceTree();
                //need update
                emit updateTree();
            }
        }
        //配置文件中保存的列表 直接连接
        for(int q=0;q<GlobalObject::g_DeviceList_Config->length();q+=2){
            if((GlobalObject::g_DeviceList_Config->at(q))->mac==strList[1]){
                //直接连接
                signed char ty=static_cast<signed char>(strList[2].toInt());
                if(px==0){
                    ConnectDevice((GlobalObject::g_DeviceList_Config->at(q))->mac,GlobalObject::port_,ty);
                }
                else{
                    ConnectDevice((GlobalObject::g_DeviceList_Config->at(q))->mac,GlobalObject::port_2,ty);
                }

            }
        }
    }
}
void DataThread::NotifyProc(QStringList &strList,int px){
    //NOTIFY;E5AEFA129DDC;fff1;12c0387042af000e0090ffcc04960045ffe70339;;

    // "NOTIFY,C4CC019C6A79,fff1,06b0386cd8410000,"//这是什么情况
    //"NOTIFY", "E5AEFA129DDC", "fff1", "1120387060c8000000100000001101a3000101", ""
   // qDebug()<<"notify str list "<<strList<<QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss");
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
            emit updateTree();
            return;
        }
        if(GlobalObject::g_ConnectDevList->at(q)->mac==strList[1]){
            if(strList[3].length()==40){
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_forepart=strList[3].toLatin1();
                //qDebug()<<"fore get";
                GlobalObject::g_ConnectDevList->at(q)->dataget[0]=true;
            }
            else if(strList[3].length()==22){
               // qDebug()<<"back get";
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_backend=strList[3].toLatin1();
                GlobalObject::g_ConnectDevList->at(q)->dataget[1]=true;
            }
            else if(strList[3].length()==38){
               // qDebug()<<"fore get";
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_forepart=strList[3].toLatin1();
                GlobalObject::g_ConnectDevList->at(q)->dataget[0]=true;
            }
            else if(strList[3].length()==16){
                //qDebug()<<"back get";
                GlobalObject::g_ConnectDevList->at(q)->DataInfo_backend=strList[3].toLatin1();

                GlobalObject::g_ConnectDevList->at(q)->dataget[1]=true;

            }
            else{
                //qDebug()<<"unknow data   "<<strList[3];
            }
            if(GlobalObject::g_ConnectDevList->at(q)->dataget[1]==false||GlobalObject::g_ConnectDevList->at(q)->dataget[0]==false){
                //qDebug()<<"not two part data get";
                return;
            }
            else{
                break;
            }
        }


    }
    //传电压过来的时候是这样的
    //"NOTIFY", "E5AEFA129DDC", "fff1", "1120387060c8000000100000001101a3000101", ""

    if(GlobalObject::g_ConnectDevList->length()==0){
        qDebug()<<"无名设备data传入";
        return;
    }
    //qDebug()<<"convert multithread ";
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
        qDebug()<<"device not find";//这个情况不应该出现的
        if(strList[1].length()==12){
            newdevice->mac=strList[1];
            index=GlobalObject::g_ConnectDevList->length();
            GlobalObject::g_ConnectDevList->push_back(newdevice);
            emit updateTree();
        }
        else{
            return ;
        }
    }
    QDateTime local(QDateTime::currentDateTime());
    QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
    // ui->ComputerTime_label->setText(localTime);

    DataProcess_Simple(GlobalObject::g_ConnectDevList->at(index)->DataInfo_forepart,GlobalObject::g_ConnectDevList->at(index)->DataInfo_backend,GlobalObject::g_ConnectDevList->at(index),index);
}
void DataThread::CharacteristicProc(QStringList &strList,int px){
    //将设备加入列表
    //"CHARACTERISTIC;E5AEFA129DDC;fff0;fff1;10; ;"
   // qDebug()<<"char  "<<strList;
    bool find=false;
    for(int q=0;q<GlobalObject::g_ConnectDevList->length();q++){//检查是否在已经连接的设备列表中
        if(strList[1]==GlobalObject::g_ConnectDevList->at(q)->mac){
            find=true;
            GlobalObject::g_ConnectDevList->at(q)->state=CONNECTED;
            GlobalObject::g_ConnectDevList->at(q)->m_bIntime=true;//数据及时性设置为true
            if(GlobalObject::g_ConnectDevList->at(q)->name==""){
                for(int m=0;m<GlobalObject::g_ScanDevList->size();m++){
                    if(GlobalObject::g_ScanDevList->at(m)->mac==strList[1]){
                        GlobalObject::g_ConnectDevList->at(q)->name=GlobalObject::g_ScanDevList->at(q)->name;
                        GlobalObject::g_ConnectDevList->at(q)->rssi=GlobalObject::g_ScanDevList->at(q)->rssi;
                        GlobalObject::g_ConnectDevList->at(q)->addrtype=GlobalObject::g_ScanDevList->at(q)->addrtype;
                        GlobalObject::g_ConnectDevList->at(q)->devPortnum=GlobalObject::g_ScanDevList->at(q)->devPortnum;
                        GlobalObject::g_ScanDevList->removeAt(m);
                        break;
                    }
                }
            }
            //qDebug()<<"find in connecter ";
            emit updateTree();
            break;
        }
    }
    if(!find){
        //正常情况下是到这里
       // qDebug()<<tr("没有在连接列表中找到项目");
        if(strList[1].length()!=12){
            return;
        }
        DeviceInfo *newde=new DeviceInfo;
        newde->mac=strList[1];
        int p=-1;
        bool find=false;
        for(int q=0;q<GlobalObject::g_ScanDevList->size();q++){
            if(GlobalObject::g_ScanDevList->at(q)->mac==strList[1]){
                p=q;
                newde->name=GlobalObject::g_ScanDevList->at(q)->name;
                newde->rssi=GlobalObject::g_ScanDevList->at(q)->rssi;
                newde->addrtype=GlobalObject::g_ScanDevList->at(q)->addrtype;
                newde->devPortnum=GlobalObject::g_ScanDevList->at(q)->devPortnum;
                find =true;
                break;
            }
        }
        if(p>=0){
            GlobalObject::g_ScanDevList->removeAt(p);
        }
        //检查是否在DeviceList_Config中
       bool fid=false;
        /*for(int m=0;m<GlobalObject::g_DeviceList_Config->length();m++){
            if(GlobalObject::g_DeviceList_Config->at(m)->mac==strList[1]){
                fid=true;
                qDebug()<<"connection from config ";
                GlobalObject::g_ConnectDevList->push_back(GlobalObject::g_DeviceList_Config->at(m));
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
        }*/
        if(!fid){
            GlobalObject::g_ConnectDevList->push_back(newde);
        }

        GlobalObject::g_ConnectDevList->last()->state=CONNECTED;
        //qDebug()<<"连接"+strList[1]+"成功";
        emit _ShortInfo("连接"+strList[1]+"成功",0);



        if(GlobalObject::g_ConnectDevList->length()>0){
            //qDebug()<<"bind csv in thread 2";
            if(Csv[GlobalObject::g_ConnectDevList->length()-1]==nullptr){
                Csv[GlobalObject::g_ConnectDevList->length()-1]=new CsvWriter(GlobalObject::g_ConnectDevList->last()->mac);//绑定写入器
            }
        }

        if(px==0){
            ReadSingelDeviceData(strList[1],GlobalObject::port_);//这个发送了才会开始发送数据
        }
        else{
            ReadSingelDeviceData(strList[1],GlobalObject::port_2);
        }
        if(GlobalObject::g_ConnectDevList->length()==1){
            //当连接列表中只有一个设备的时候，直接在界面显示这个设备数据
            FocusIndex=0;
        }

    }
    emit updateTree();


}
void DataThread::ReadSingelDeviceData(QString Mac, quint16 po){
    if(Mac.length()==12)
    {

        //qDebug()<<"thread 2 send ReadSingelDeviceData";
        if(po==GlobalObject::port_){
            SendDatatoDev(Mac,gRxChar,"012001",0);
        }
        else{
            SendDatatoDev(Mac,gRxChar,"012001",1);
        }

    }
}
void DataThread::SendDatatoDev(QString Mac, QString charc, QString dat, int po){
    if(Mac.length()==12)
    {

        QString sendData="AT+WRITECHAR="+Mac+","+charc+","+dat+"\r\n";

        if(po==0&&m_bTcp_1_con){
            Socket_->write(sendData.toLatin1(),sendData.length());
        }
        else if(m_bTcp_2_con){
            Socket_2->write(sendData.toLatin1(),sendData.length());
        }
    }
}
void DataThread::DisconnetDeviceTrigged(QString Mac, quint16 p){
    // qDebug()<<"dis connecte";
    if(Mac.length()==12)
    {
        QString strMsg="AT+DISC="+Mac+"\r\n";
        //qDebug()<<"本机发送断开请求"<<strMsg;

        if(p==GlobalObject::port_&&m_bTcp_1_con){
            Socket_->write(strMsg.toLatin1(),strMsg.length());//
        }
        else if(m_bTcp_2_con){
            Socket_2->write(strMsg.toLatin1(),strMsg.length());//
        }

    }
    else{
        qDebug()<<"mac is not 12 bit";
    }


}
void DataThread::DiscProc(QStringList &info){
    //剩下就是mac了
    //DISC,E5AEFA129DDC,
    // PromptInfomation("断开连接"+info[1]);

    for(int i=0;i<GlobalObject::g_ConnectDevList->length();i++){
       // qDebug()<<GlobalObject::g_ConnectDevList->at(i)->mac<<"  "<<info[1];
        if((GlobalObject::g_ConnectDevList->at(i))->mac==info[1]){
            GlobalObject::g_ConnectDevList->removeAt(i);
           // qDebug()<<"remove  "<<info[1];
            if(FocusIndex==i){
                FocusIndex=-1;
            }
            emit updateTree();
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

    //on_Exit_pushButton_clicked();

}

void DataThread::ReadDevice(QString mac,quint16 po){
    if(mac.length()==12)
    {
        QString str="AT+READCHAR="+mac+","+gRxChar+"\r\n";
        if(m_bTcp_1_con&&po==GlobalObject::port_){
            Socket_->write(str.toLatin1(),str.length());
        }
        else if(m_bTcp_2_con&&po==GlobalObject::port_2){
            Socket_2->write(str.toLatin1(),str.length());
        }


    }
}
void DataThread::CharacteristicofDevice(QString mac, quint16 po){
    if(mac.length()==12)
    {
        QString str="AT+CHAR="+mac+"\r\n";
        if(m_bTcp_1_con&&po==GlobalObject::port_){
            Socket_->write(str.toLatin1(),str.length());
        }
        else if(m_bTcp_2_con&&po==GlobalObject::port_2){
            Socket_2->write(str.toLatin1(),str.length());
        }


    }
}
void DataThread::BtnDeviceScan(){
    if(!m_bTCPConnected)
    {
        QMessageBox::information(nullptr,"网络未连接,请稍候...","提示");
        return;
    }

    InquireDev(1,1,INQUIRE_SECONDS);
    // m_bSearching = true;

    emit _ShortInfo(tr("正在搜索传感器设备..."),0);
}

void DataThread::PrepareInquire()
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
void DataThread::InquireDev(unsigned char action,unsigned char mod,unsigned char time)
{
    char cmdstr[64];
    if(!m_bTCPConnected){
        // qDebug()<<"m_bsearching "<<m_bSearching<<" m_connected "<<m_bTCPConnected;
        return;
    }
    if(m_bTCPConnected)
    {
        sprintf(cmdstr,"AT+INQ=%d,%d,%d\r\n",action,mod,time);
        //qDebug()<<cmdstr<<"thread 2 inquire send";
        size_t len=strlen(cmdstr);
       /// qDebug()<<"port1";
        if(GlobalObject::port_!=0&&m_bTcp_1_con){
            Socket_->write(cmdstr,static_cast<int>(len));
        }
       // qDebug()<<"port2 ";
        if(GlobalObject::port_2!=0&&m_bTcp_2_con){
            Socket_2->write(cmdstr,static_cast<int>(len));
        }

        if(action == 1)
        {

            emit _ShortInfo(tr("正在搜索传感器设备..."),0);
        }

    }
}
bool DataThread::BdaddrIsZero(QString addr)
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
void DataThread::_SocketWriter(QString info,int p){
    if(p==0){
       // qDebug()<<"debug 4";
        if(Socket_!=nullptr){
            if(!m_bTcp_1_con){
                return;
            }
          //  qDebug()<<"Thread 2 and port 0 write  "<<info;
            Socket_->write(info.toLatin1(),info.length());
          //  qDebug()<<"debug 5";
        }
    }
    if(p==1){
        if(Socket_2!=nullptr){
            if(!m_bTcp_2_con){
                return;
            }
          //  qDebug()<<"Thread 2 and port 1 write  "<<info;
            Socket_2->write(info.toLatin1(),info.length());
        }
    }
}

void DataThread::ServerStateCheck(bool b1,bool b2){
    m_bTcp_1_con=b1;
    m_bTcp_2_con=b2;
}

void DataThread::ConnectDevice(QString Mac,quint16 port,signed char ty){
    //qDebug()<<"trigged!";
    // qDebug()<<Mac<<" "<<ty;

    if(Mac.length()==12)
    {
        // index = GetSensorIndex(addr);
        //此处要获取设备信息，确定设备类型
        QString strMsg="AT+CONN="+Mac+","+QString::number(ty)+"\r\n";//0表示没链接吗
        // qDebug()<<strMsg;


        if(port==GlobalObject::port_&&m_bTcp_1_con){
            Socket_->write(strMsg.toLatin1(),strMsg.length());//
            GetSingleSensorRSSI(Mac,port);
        }
        if(port==GlobalObject::port_2&&m_bTcp_2_con){
            Socket_2->write(strMsg.toLatin1(),strMsg.length());//
            GetSingleSensorRSSI(Mac,port);
        }

        emit _ShortInfo("正在连接"+Mac,0);

    }
}
void DataThread::GetSingleSensorRSSI(QString Mac,quint16 po){
    if(Mac.length()==12)
    {
        QString inqRssi="AT+RSSI="+Mac+"\r\n";


        if(po==GlobalObject::port_&&m_bTcp_1_con){

            Socket_->write(inqRssi.toLatin1(),inqRssi.length());
        }
        else if(m_bTcp_2_con){
            Socket_2->write(inqRssi.toLatin1(),inqRssi.length());
        }
    }


}

void DataThread::SetFocusIndex(int idx){
    FocusIndex=idx;
}
void DataThread::GetWriteMsg(QString Info,int p){
  // qDebug()<<"debug 3";
    _SocketWriter(Info,p);
}
void DataThread::TCPconnector(QString HostAddress){
    QHostAddress HostAdd;
    QString localHostName = QHostInfo::localHostName();

    QHostInfo ip_info = QHostInfo::fromName(localHostName);


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
    // QHostAddress HostAdd(ip_info.addresses()[1]);
    qDebug()<<"ip_info.addresses()  "<<ip_info.addresses();
    if(GlobalObject::port_!=0){
       // ui->ip_label->setText("本机ip   "+(HostAdd.toString()));
        Server_=new QTcpServer;
        Socket_=new QTcpSocket;
        Server_->listen(HostAdd,GlobalObject::port_);
        connect(Server_,&QTcpServer::newConnection,this,[=]{
            Socket_=Server_->nextPendingConnection();
            //ui->Port1_State_label->setText(QString::number(GlobalObject::port_)+"  Connected");

            emit _ShortInfo("TCP connected",0);
            qDebug()<<"TCP connecte";
            m_bTcp_1_con=true;

            m_bTCPConnected=true;
            connect(Socket_,&QTcpSocket::readyRead,this,[=]{
               // qDebug()<<"1 read ready";
                QByteArray s=Socket_->readAll();
                 Thread_infoHandler(s,0);

               // qDebug()<<"father thread id  "<<QThread::currentThreadId();
            });
            emit  ServerState(m_bTcp_1_con,m_bTcp_2_con);

        });
    }
    else{
        if(GlobalObject::port_2!=0&&GlobalObject::port_2!=GlobalObject::port_){
            Server_2=new QTcpServer;
            Socket_2=new QTcpSocket;
            Server_2->listen(HostAdd,GlobalObject::port_2);
            connect(Server_2,&QTcpServer::newConnection,this,[=]{
                Socket_2=Server_2->nextPendingConnection();
                // ui->ConnectState_Label->setText("已经连接网关 2");
                //ui->Port2_State_label->setText(QString::number(GlobalObject::port_2)+"  Connected");
                //qDebug()<<"port2"<<Socket_->isOpen()<<"  "<<Socket_2->isOpen();
                //CurrentSearchingCount=0;
                m_bTcp_2_con=true;
                m_bTCPConnected=true;

                connect(Socket_2,&QTcpSocket::readyRead,this,[=]{
                  //  qDebug()<<"2 read ready";
                    QByteArray s=Socket_2->readAll();
                    //InfoHanddler(s);
                    Thread_infoHandler(s,1);
                });
                emit  ServerState(m_bTcp_1_con,m_bTcp_2_con);
            });
        };
    }
        emit _ShortInfo("等待蓝牙网关",0);

}
void DataThread::FileOutput(QString str, QString add, int method, DeviceInfo *df,CsvWriter *writer){
    //method ==0  覆盖
    //method ！=0 追加
    if(df!=nullptr&&writer!=nullptr){
       // qDebug()<<"if case";
        QString Outstr;
        //"Time,Mac,Name,AccX,AccY,AccZ,alpha,beta,gama,motion,IR,iuv,distance,Intensity,temperture\n""
        Outstr+=df->time+","+df->mac+","+df->name+","+QString::number(df->datainfo.accelx)+
                ","+QString::number(df->datainfo.accely)+","+QString::number(df->datainfo.accelz)+","+
                QString::number(df->datainfo.tilt1)+","+QString::number(df->datainfo.tilt2)+","
                +QString::number(df->datainfo.tilt3)+","+QString::number(df->datainfo.motion)+
                ","+QString::number(df->datainfo.IR)+","+QString::number(df->datainfo.iUV)+","+
                QString::number(df->datainfo.distance)+","+QString::number(df->datainfo.intensity)+
                ","+QString::number(df->datainfo.temperature)+"\n";


       // qDebug()<<"write  "<<Outstr;
        if(writer->m_bOpen==true){

            *(writer->GetOut())<<Outstr;
        }
    }
    else {
       // qDebug()<<"else case ";
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
