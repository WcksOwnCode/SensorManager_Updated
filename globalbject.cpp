#pragma execution_character_set("utf-8")
#include "globalobject.h"
#include "serialportdialog.h"
#include <QThread>
quint16  GlobalObject::port_=0;
QTcpServer * GlobalObject::Server_=nullptr;
QTcpSocket * GlobalObject::Socket_=nullptr;

quint16  GlobalObject::port_2=0;
QTcpServer * GlobalObject::Server_2=nullptr;
QTcpSocket * GlobalObject::Socket_2=nullptr;

QSerialPort *GlobalObject::serial=nullptr;
QSerialPort *GlobalObject::serial_2=nullptr;
QMutex *GlobalObject:: locker=new QMutex;

QList<ScanListItem*> *GlobalObject::g_ScanDevList=new QList<ScanListItem*>;//扫描设备列表
QList<DeviceInfo*> *GlobalObject::g_ConnectDevList=new QList<DeviceInfo*>;//连接设备列表
QList<DeviceInfo*> *GlobalObject::g_DeviceList_Config=new QList<DeviceInfo*>;//配置文件设备列表
QVector<QString> *GlobalObject:: g_BlockList=new QVector<QString>;
bool GlobalObject::m_bSend_Access[2]={true,true};

GlobalObject::GlobalObject()
{
    
}

void GlobalObject:: DesTroy(){

    if(serial!=nullptr){
        serial->close();
        
    }
    

    if(serial_2!=nullptr){
        serial_2->close();
        
    }
    
   // qDebug()<<"glabal Thread id  "<<QThread::currentThreadId()<<QThread::currentThread();
    /*if(Server_2!=nullptr){
        Server_2->close();
        
    }
    if(Socket_2!=nullptr){
        Socket_2->close();
        
    }*/
    
    /* if(Server_!=nullptr){
        Server_->close();
    }
    if(Socket_!=nullptr){
        Socket_->close();
    }*/
    

    if(locker!=nullptr){
        delete locker;
        locker=nullptr;
    }

    while(g_ScanDevList->length()>0){
        g_ScanDevList->removeFirst();
    }
    

    while(g_ConnectDevList->length()>0){
        g_ConnectDevList->removeFirst();
    }

    while(g_DeviceList_Config->length()>0){
        g_DeviceList_Config->removeFirst();
    }

    while(g_BlockList->length()>0){
        g_BlockList->removeFirst();
    }
    
    
    
}
QStringList GlobalObject::FalcutyCode{//显示在界面中的错误码
    "NONE",
    "BATTERY_LOW",
    "BATTERY_EMPTY",
    "MEMORY_LOW",
    "MEMORY_FULL",
    "SELF_TEST_FAILD ",
    "FLASH_TEST_FAILD",
    "UV_TEST_FAILD ",
    "LIGHT_TEST_FAILD",
    "RANGING_TEST_FAILD",
    "MOTION_TEST_FAILD",
    "VOICE_TEST_FAILD "
};
QStringList GlobalObject::ControlCommand{
    "FE 01 00 00 00 08 29 C3",//查询八路状态
    "FE 05 00 00 FF 00 98 35",//控制第一路开
    "FE 05 00 00 00 00 D9 C5",//控制第一路关
    "FE 05 00 01 FF 00 C9 F5",//控制第二路开
    "FE 05 00 01 00 00 88 05",//控制第二路关
    "FE 05 00 02 FF 00 39 F5",//控制第三路开
    "FE 05 00 02 00 00 78 05",//控制第三路关
    "FE 05 00 03 FF 00 68 35",//控制第四路开
    "FE 05 00 03 00 00 29 C5",//控制第四路关
    "FE 05 00 04 FF 00 D9 F4",//控制第五路开
    "FE 05 00 04 00 00 98 04",//控制第五路关
    "FE 05 00 05 FF 00 88 34",//控制第六路开
    "FE 05 00 05 00 00 C9 C4",//控制第六路关
    "FE 05 00 06 FF 00 78 34",//控制第七路开
    "FE 05 00 06 00 00 39 C4",//控制第七路关
    "FE 05 00 07 FF 00 29 F4",//控制第八路开
    "FE 05 00 07 00 00 68 04",//控制第八路关
    "FE 02 00 00 00 08 6D C3",//查询八路光耦状态
    "FE 02 01 00 91 9C" ,     //查询返回信息
    "FE 0F 00 00 00 08 01 FF F1 D1",//全开
    "FE 0F 00 00 00 08 01 00 B1 91",//全断
    "FE 10 00 03 00 02 04 00 04 00 0A 41 6B",//闪开
    "FE 10 00 03 00 02 04 00 02 00 14 21 62" //闪断
};
QStringList GlobalObject::ControlCommand_rep{//与上面对应
   /* "FE 01 01 00 61 9C",
    "FE 05 00 00 FF 00 98 35",
    "FE 05 00 00 00 00 D9 C5",
    "FE 05 00 01 FF 00 C9 F5",//控制第二路开
    "FE 05 00 01 00 00 88 05",//控制第二路关
    "FE 05 00 02 FF 00 39 F5",//控制第三路开
    "FE 05 00 02 00 00 78 05",//控制第三路关
    "FE 05 00 03 FF 00 68 35",//控制第四路开
    "FE 05 00 03 00 00 29 C5",//控制第四路关
    "FE 05 00 04 FF 00 D9 F4",//控制第五路开
    "FE 05 00 04 00 00 98 04",//控制第五路关
    "FE 05 00 05 FF 00 88 34",//控制第六路开
    "FE 05 00 05 00 00 C9 C4",//控制第六路关
    "FE 05 00 06 FF 00 78 34",//控制第七路开
    "FE 05 00 06 00 00 39 C4",//控制第七路关
    "FE 05 00 07 FF 00 29 F4",//控制第八路开
    "FE 05 00 07 00 00 68 04",//控制第八路关
    "FE 02 00 00 00 08 6D C3",//查询八路光耦状态
    "FE 02 01 00 91 9C" ,     //查询返回信息
    "FE 0F 00 00 00 0A C1 C3",//全开返回码
    "FE 0F 00 00 00 08 40 02",//全断返回码
    "FE 10 00 03 00 02 A5 C7",//闪开返回
    "FE 10 00 03 00 02 A5 C7" //闪断返回*/
    //备注:我不知道这个返回码是不是完全正确
    "FE 01 00 00 00 08 29 C3",//查询八路状态
    "FE 05 00 00 FF 00 98 35",//控制第一路开
    "FE 05 00 00 00 00 D9 C5",//控制第一路关
    "FE 05 00 01 FF 00 C9 F5",//控制第二路开
    "FE 05 00 01 00 00 88 05",//控制第二路关
    "FE 05 00 02 FF 00 39 F5",//控制第三路开
    "FE 05 00 02 00 00 78 05",//控制第三路关
    "FE 05 00 03 FF 00 68 35",//控制第四路开
    "FE 05 00 03 00 00 29 C5",//控制第四路关
    "FE 05 00 04 FF 00 D9 F4",//控制第五路开
    "FE 05 00 04 00 00 98 04",//控制第五路关
    "FE 05 00 05 FF 00 88 34",//控制第六路开
    "FE 05 00 05 00 00 C9 C4",//控制第六路关
    "FE 05 00 06 FF 00 78 34",//控制第七路开
    "FE 05 00 06 00 00 39 C4",//控制第七路关
    "FE 05 00 07 FF 00 29 F4",//控制第八路开
    "FE 05 00 07 00 00 68 04",//控制第八路关
    "FE 02 00 00 00 08 6D C3",//查询八路光耦状态
    "FE 02 01 00 91 9C" ,     //查询返回信息
    "FE 0F 00 00 00 08 01 FF F1 D1",//全开
    "FE 0F 00 00 00 08 01 00 B1 91",//全断
    "FE 10 00 03 00 02 04 00 04 00 0A 41 6B",//闪开
    "FE 10 00 03 00 02 04 00 02 00 14 21 62" //闪断
};
CorrectionCoefficient::CorrectionCoefficient(){
    
}CorrectionCoefficient::~CorrectionCoefficient(){
}
void CorrectionCoefficient::Reset(){
    CorrAccX=0;
    CorrAccY=0;
    CorrAccZ=0;
    CorrAlpha=0.0f;
    CorrBeta=0.0f;
    CorrGama=0.0f;
    CorrDistanceA=0.0f;
    CorrDistanceB=1.0f;
    CorrDistanceC=0.0f;
    CorrLight=0.0f;
    CorrIR=0.0f;
    CorrTempA=0.0f;
    CorrTempB=1.0f;
    CorrTempC=0.0f;
}
bool GlobalObject::_Assert(short v1,int v2,_type ty){
    
    //v1 是监控值 v2 是 范围值   ty是符号
    qDebug()<<QString::fromLocal8Bit("监控值 value 1 ")<<v1<<QString::fromLocal8Bit("监控值 value 2  ")<<v2<<" type   "<<ty;
    if(ty==NONE){
        return false;
    }
    else if(ty==GREATER){
        if(v1>v2){
            return true;
        }
        return false;
    }
    else if (ty==EQUAL) {
        if(abs(v1-v2)<0.001){
            return true;
        }
        return false;
    }
    else if(ty==LESS){
        if(v1<v2){
            return true;
        }
        return false;
    }
}
bool GlobalObject::_Assert(float v1,int v2,_type ty){
    if(ty==NONE){
        return false;
    }
    else if(ty==GREATER){
        if(v1>v2){
            return true;
        }
        return false;
    }
    else if (ty==EQUAL) {
        if(abs(v1-v2)<0.001f){
            return true;
        }
        return false;
    }
    else if(ty==LESS){
        if(v1<v2){
            return true;
        }
        return false;
    }
}
void DeviceInfo::Method_exec(){
    qDebug()<<QString::fromLocal8Bit("实行控制策略检查.....")<<"Control condition check (void DeviceInfo::Method_exec)";;
    for(int i=0;i<ControlMethodList.length();i++){
        qDebug()<<"method exec index "<<i;
        qDebug()<<"method exec sequece "<<ControlMethodList[i]->sequece;
        
        bool ok1=false,ok2=false;
        switch(ControlMethodList[i]->sequece){//查看是哪个参数的控制命令
        case 0:  //short motion;  ------------------0
            ok1=GlobalObject::_Assert(this->datainfo.motion,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 1://short accelx;  ------------------1
            ok1=GlobalObject::_Assert(this->datainfo.accelx,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 2:// short accely;  ------------------2
            ok1=GlobalObject::_Assert(this->datainfo.accely,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 3://short accelz;  ------------------3
            ok1=GlobalObject::_Assert(this->datainfo.accelz,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 4:// short intensity;-----------------4
            ok1=GlobalObject::_Assert(this->datainfo.intensity,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 5://short IR;      ------------------5
            ok1=GlobalObject::_Assert(this->datainfo.IR,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 6://short iUV;     ------------------6
            ok1=GlobalObject::_Assert(this->datainfo.iUV,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 7://short distance;------------------7
            ok1=GlobalObject::_Assert(this->datainfo.distance,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 8://float tilt1;   ------------------8
            ok1=GlobalObject::_Assert(this->datainfo.tilt1,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 9://float tilt2;   ------------------9
            ok1=GlobalObject::_Assert(this->datainfo.tilt2,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 10://float tilt3;   ------------------10
            ok1=GlobalObject::_Assert(this->datainfo.tilt3,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 11://float temperature;---------------11
            ok1=GlobalObject::_Assert(this->datainfo.temperature,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        }
        //qDebug()<<"check value "<<ControlMethodList[i]->i_value<<"  dev   "<<this->datainfo.accelx;
        //qDebug()<<"ok1 check "<<ok1;
        if(ok1&&ControlMethodList[i]->_Log==LOGI_NONE){
            qDebug()<<QString::fromLocal8Bit("条件 1 满足 无条件 2")<<"Satisfy the first condition (none)";;
            OrderSend(ControlMethodList[i]->Message_,ControlMethodList[i]->serialportnum,ControlMethodList[i]->sequece,i);
        }

        if(ControlMethodList[i]->_Log!=LOGI_NONE){//不等于空就是有条件2
            //条件2 检查
            switch(ControlMethodList[i]->sequece){
            case 0:  //short motion;  ------------------0
                ok2=GlobalObject::_Assert(this->datainfo.motion,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 1://short accelx;  ------------------1
                ok2=GlobalObject::_Assert(this->datainfo.accelx,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 2:// short accely;  ------------------2
                ok2=GlobalObject::_Assert(this->datainfo.accely,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 3://short accelz;  ------------------3
                ok2=GlobalObject::_Assert(this->datainfo.accelz,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 4:// short intensity;-----------------4
                ok2=GlobalObject::_Assert(this->datainfo.intensity,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 5://short IR;      ------------------5
                ok2=GlobalObject::_Assert(this->datainfo.IR,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 6://short iUV;     ------------------6
                ok2=GlobalObject::_Assert(this->datainfo.iUV,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 7://short distance;------------------7
                ok2=GlobalObject::_Assert(this->datainfo.distance,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 8://float tilt1;   ------------------8
                ok2=GlobalObject::_Assert(this->datainfo.tilt1,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 9://float tilt2;   ------------------9
                ok2=GlobalObject::_Assert(this->datainfo.tilt2,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 10://float tilt3;   ------------------10
                ok2=GlobalObject::_Assert(this->datainfo.tilt3,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 11://float temperature;---------------11
                ok2=GlobalObject::_Assert(this->datainfo.temperature,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            }
            //qDebug()<<"*****************ok1  "<<ok1<<"   *******************ok2  "<<ok2<<"********************";
            
            if(ControlMethodList[i]->_Log==LOGI_AND){
                if(ok1&ok2){
                    qDebug()<<QString::fromLocal8Bit("两个条件 同时满足")<<"Satisfy both two condition (and)";
                    OrderSend(ControlMethodList[i]->Message_,ControlMethodList[i]->serialportnum,ControlMethodList[i]->sequece,i);
                }
            }
            if(ControlMethodList[i]->_Log==LOGI_OR){
                qDebug()<<QString::fromLocal8Bit("两个条件 满足一个")<<"Satisfy one condition (or)";
                if(ok1|ok2){
                    OrderSend(ControlMethodList[i]->Message_,ControlMethodList[i]->serialportnum,ControlMethodList[i]->sequece,i);
                }
            }
            
        }
        
        
    }
}
bool DeviceInfo::SendCountCheck(int index,int port,int squ,int &rType,int &cIndex){
    //检查发送次数
    //<串口号，参数索引，命令索引，发送次数>
    bool ok=false;
    int ty=-1;
    //ty 检查结果标志 数值如下
    //-1--端口没有记录->全部数据没有记录
    //1 -- 端口有记录，命令号没有记录
    //2 --参数号没有记录
    //3 --端口号 参数号 命令号 都有记录
    //4 --端口号 参数号 命令号 都有记录，发送次数满足小于max_Send，继续检查发送时间
    //5 --端口号 参数号 命令号 都有记录，但是发送次数超过限制 返回false
    //6 --端口号 参数号 命令号 都有记录,发送次数满足小于max_Send，发送超时，返回false
    //7 --端口号 参数号 命令号 都有记录,发送次数满足小于max_Send，没有发送时间记录，返回false
    //8 --端口号 参数号 命令号 都有记录,发送次数满足小于max_Send，发送时间未超时，返回true,检查通过

    int Command_index=-1;
    for(int i=0;i<CommandSendList.length();i++){
        if(int(CommandSendList[i].x())!=port) {//端口号检查
            continue;
        }
        ty=1;
        if(int(CommandSendList[i].y())!=index)//命令号检查
        {
            continue;
        }
        ty=2;
        if(int(CommandSendList[i].z())!=squ)//参数号检查
        {
            continue;
        }
        ty=3;
        if(int(CommandSendList[i].w())<=maxSend){//发送次数检查
            ok=true;//通过检查
            Command_index=i;//获取索引，下面可用
            cIndex=i;//返回索引
            ty=4;
            break;
        }
        else{
            ty=5;
            break;
        }

        
    }
    if(ty!=4){
        rType=ty;
        return false;
    }
    //检查发送时间  仅仅检测第一次发送时间
    uint curTime_s=QDateTime::currentDateTime().toTime_t();
    
    if(Command_index!=-1&&Command_index<CommandTimeStamp.length()){
        if(int(curTime_s-CommandTimeStamp[Command_index])>maxTimeCons){
            ty=6;
            rType=ty;
            return false;
        }
    }
    else{
        ty=7;
        qDebug()<<"no time record, error!!";
        rType=ty;
        return false;
    }
    ty=8;
    rType=ty;
    return ok;
}
void DeviceInfo::AddItemToCommandList(int port,int index,int Cindex,int SendCount,uint Timestamp,int ty=0){
    //ty==0 添加一条在尾部
    //ty==1 更新对应数据,不更新时间
    //ty==2 更新对应数据,更新时间
    if(ty==0){
        QVector4D newItem(port,index,Cindex,SendCount);
        CommandSendList.push_back(newItem);

        CommandTimeStamp.push_back(Timestamp);
    }
    if(ty>0){
        for(int i=0;i<CommandSendList.length();i++){
            if(port==int(CommandSendList[i].x())&&index==int(CommandSendList[i].y())
                    &&Cindex==int(CommandSendList[i].z())){
                CommandSendList[i].setW(SendCount);
                if(ty==2){
                    CommandTimeStamp[i]=Timestamp;
                }

            }
        }
    }


}
void DeviceInfo::OrderSend(int index,int port,int sequence,int Orderidx){
    //发送串口控制指令
   //之前串口发送不出去的问题可能出自这里 我把port的值记错了
    /*
      index ----->index in Commandlist
      port ------> port number 1 or 2
      sequence----> which parameter
      Orderidx---->index in device control methodlist
*/



   /* if(port==0){
        qDebug()<<"串口发送入口检查失败 传入参数 port == 0";
        qDebug()<<"serial port  send fail    port==0 return";
        return;
    }*/
qDebug()<<"order send ";
    if(port==1){
        if(!GlobalObject::m_bSend_Access[0]){
            qDebug()<<"port 1 m_bSend_Access check failed,order will never send!";
            return;
        }
        if(GlobalObject::serial!=nullptr){
            if(GlobalObject::serial->isOpen()){
                
                //命令列表检查
                int ty=-1;
                int ind=-1;
                bool ok=SendCountCheck(index,port,sequence,ty,ind);
                //ty 检测结果标志  如下
                //-1--端口没有记录->全部数据没有记录
                //1 -- 端口有记录，命令号没有记录
                //2 --参数号没有记录
                //3 --端口号 参数号 命令号 都有记录
                //4 --端口号 参数号 命令号 都有记录，发送次数满足小于max_Send，继续检查发送时间
                //5 --端口号 参数号 命令号 都有记录，但是发送次数超过限制 返回false
                //6 --端口号 参数号 命令号 都有记录,发送次数满足小于max_Send，发送超时，返回false
                //7 --端口号 参数号 命令号 都有记录,发送次数满足小于max_Send，没有发送时间记录，返回false
                //8 --端口号 参数号 命令号 都有记录,发送次数满足小于max_Send，发送时间未超时，返回true,检查通过
                if(ty==8){
                    QString Data=GlobalObject::ControlCommand[index];
                    QByteArray Hexdata;
                    SerialPortDialog::QString2Hex(Data,Hexdata);
                    GlobalObject::serial->write(Hexdata);
                    while(GlobalObject::serial->waitForBytesWritten(10))    //等待串口发送完成
                    {

                    }
                    qDebug()<<QString::fromLocal8Bit("串口1 发送数据 ")<<GlobalObject::ControlCommand[index];
                    qDebug()<<"serial port 1 send "<<GlobalObject::ControlCommand[index];
                    if(ind!=-1&&ind<CommandTimeStamp.length()){
                        AddItemToCommandList(port,index,sequence,int(CommandSendList[ind].w())+1,0,1);
                    }

                }
                if(ty==7){//缺少发送时间戳
                    qDebug()<<QString::fromLocal8Bit("缺少发送时间戳");
                    qDebug()<<"no time stamp ";
                    uint timestamp=QDateTime::currentDateTime().toTime_t();
                    //add time stamp
                    AddItemToCommandList(port,index,sequence,int(CommandSendList[ind].w())+1,timestamp,2);
                }
                if(ty<7&&ty!=6&&ty!=5){
                    uint timestamp=QDateTime::currentDateTime().toTime_t();
                    AddItemToCommandList(port,index,sequence,1,timestamp,0);
                }
                if(ty==5){
                    //发送次数超过限制

                    // emit megsend(QString::fromLocal8Bit("发送次数超过限制,无应答"));
                    qDebug()<<"ty==5";
                    if(ind>0&&ind<CommandSendList.length()){
                        CommandSendList.removeAt(ind);
                        CommandTimeStamp.removeAt(ind);
                        removedSendState.push_back(QVector2D(CommandSendList[ind].z(),5));
                        //ControlMethodList.removeAt(Orderidx);


                    }
                    else{
                        qDebug()<<"error ind  "<<ind;
                    }
                   ControlMethodList.removeAt(Orderidx);
                   MethodCount[sequence]=0;
                }
                if(ty==6){
                    //发送超过限制时间
                     qDebug()<<"ty==6";
                    //  emit megsend(QString::fromLocal8Bit("发送时间超过限制,无应答"));
                     if(ind>0&&ind<CommandSendList.length()){
                         CommandSendList.removeAt(ind);
                         CommandTimeStamp.removeAt(ind);
                         removedSendState.push_back(QVector2D(CommandSendList[ind].z(),6));
                     }
                     else{
                         qDebug()<<"error ind  "<<ind;
                     }
                    ControlMethodList.removeAt(Orderidx);
                    MethodCount[sequence]=0;
                }
            }
            else{
                qDebug()<<QString::fromLocal8Bit("串口1 没打开");
                qDebug()<<"serial port 1 not open!";
            }
        }
    }
    else if(port==2){
        if(!GlobalObject::m_bSend_Access[1]){
            qDebug()<<"port 2 m_bSend_Access check failed,order will never send!";
            return;
        }
        if(GlobalObject::serial_2!=nullptr){
            if(GlobalObject::serial_2->isOpen()){
                int ty=-1;
                int ind=-1;
                bool ok=SendCountCheck(index,port,sequence,ty,ind);

                if(ty==8){
                    QString Data=GlobalObject::ControlCommand[index];
                    QByteArray Hexdata;
                    SerialPortDialog::QString2Hex(Data,Hexdata);
                    GlobalObject::serial_2->write(Hexdata);
                    qDebug()<<"port 2 send info "<<GlobalObject::ControlCommand[index];
                    if(ind!=-1&&ind<CommandTimeStamp.length()){
                        AddItemToCommandList(port,index,sequence,int(CommandSendList[ind].w())+1,0,1);
                    }

                }
                if(ty==7){//缺少发送时间戳
                    uint timestamp=QDateTime::currentDateTime().toTime_t();
                    AddItemToCommandList(port,index,sequence,int(CommandSendList[ind].w())+1,timestamp,2);
                }
                if(ty<7&&ty!=6&&ty!=5){
                    uint timestamp=QDateTime::currentDateTime().toTime_t();
                    AddItemToCommandList(port,index,sequence,1,timestamp,0);
                }
                if(ty==5){
                    //发送次数超过限制

                    // emit megsend(QString::fromLocal8Bit("发送次数超过限制,无应答"));
                    qDebug()<<"ty==5";
                    if(ind>0&&ind<CommandSendList.length()){
                        CommandSendList.removeAt(ind);
                        CommandTimeStamp.removeAt(ind);
                        removedSendState.push_back(QVector2D(CommandSendList[ind].z(),5));
                        //ControlMethodList.removeAt(Orderidx);


                    }
                    else{
                        qDebug()<<"error ind  "<<ind;
                    }
                   ControlMethodList.removeAt(Orderidx);
                   MethodCount[sequence]=0;

                }
                if(ty==6){
                    //发送超过限制时间
                     qDebug()<<"ty==6";
                    //  emit megsend(QString::fromLocal8Bit("发送时间超过限制,无应答"));
                     if(ind>0&&ind<CommandSendList.length()){
                         CommandSendList.removeAt(ind);
                         CommandTimeStamp.removeAt(ind);
                         removedSendState.push_back(QVector2D(CommandSendList[ind].z(),6));
                     }
                     else{
                         qDebug()<<"error ind  "<<ind;
                     }
                    ControlMethodList.removeAt(Orderidx);
                    MethodCount[sequence]=0;
                }
            }
            else{
                qDebug()<<"port 2 not open";
            }
        }
    }
    
    
    
}
DeviceInfo::~DeviceInfo(){
    for(int i=0;i<this->ControlMethodList.length();i++){
        if(this->ControlMethodList[i]!=nullptr){
            delete ControlMethodList[i];
            ControlMethodList[i]=nullptr;
        }
    }
}

void DeviceInfo::CheckDataTimeStamp(){
    //检查设备数据接收到时间和当前时间的间隔
    long long systime=QDateTime::currentDateTime().toTime_t();
    long long devtime=this->lastDataTimeStamp.toTime_t();
    
    if(systime-devtime>5){//超过5s没接到数据表示断开
        
        this->m_bIntime=false;

        return;
    }
    this->m_bIntime=true;
    
    
    
}
DeviceInfo* DeviceInfo::operator=(const DeviceInfo &another){
    qDebug()<<"======= operator =======";
    this->mac=another.mac;
    this->name=another.name;
    this->rssi=another.rssi;
    this->time=another.time;
    this->mode=another.mode;
    this->fault=another.fault;
    this->state=another.state;
    this->memory=another.memory;
    this->newname=another.newname;
    this->voltage=another.voltage;
    this->addrtype=another.addrtype;
    this->devPortnum=another.devPortnum;
    this->lastDataTimeStamp=another.lastDataTimeStamp;
    for(int i=0;i<another.CommandSendList.length();i++){
        this->ControlMethodList.push_back(another.ControlMethodList[i]);
    }
    for(int i=0;i<12;i++){
        this->MethodCount[i]=another.MethodCount[i];
    }
    this->addrtype=another.addrtype;
    this->connfailcount=another.connfailcount;
    this->faultcode=another.faultcode;
    this->workmode=another.workmode;
    this->waitdata=another.waitdata;
    this->m_bCorrectValue=another.m_bCorrectValue;
    this->datainfo=another.datainfo;
    this->Corr_Coe=another.Corr_Coe;
    this->dataIntgrality=another.dataIntgrality;
    this->m_bMethodexecRunable=another.m_bMethodexecRunable;
    this->DataInfo_backend=another.DataInfo_backend;
    this->DataInfo_forepart=another.DataInfo_forepart;
    this->dataget[0]=another.dataget[0];
    this->dataget[1]=another.dataget[1];
    this->lastDataTimeStamp=another.lastDataTimeStamp;

    this->m_bIntime=another.m_bIntime;
    for (int i=0;i<CommandSendList.length();i++) {
        this->CommandSendList.push_back(another.CommandSendList[i]);
    }
    for(int i=0;i<CommandTimeStamp.length();i++){
        this->CommandTimeStamp.push_back(another.CommandTimeStamp[i]);
    }
    for(int i=0;i<removedSendState.length();i++){
        this->removedSendState.push_back(another.removedSendState[i]);
    }
    this->ReCon=another.ReCon;
    return  this;
}
void RedirectMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    //type should be    QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg

    static QMutex Meg_mutex;
    Meg_mutex.lock();
    QByteArray localMsg = msg.toLocal8Bit();
    QString strMsg("");
    int flag=0;
    if(type==QtDebugMsg){
        //qdebug 的东西不想写入的，但是这里先暂时写进去吧
        strMsg = QString("qDebug :");
        flag=1;
    }
    if(type==QtWarningMsg){
        strMsg = QString("Warning:");
    }
    if(type==QtCriticalMsg){
        strMsg = QString("Critical:");
    }
    if(type==QtFatalMsg){
        strMsg =QString("Fatal:");
    }
    if(type==QtInfoMsg){
        strMsg =QString("Info:");
    }
    if(type==QtSystemMsg){
        strMsg =QString("SystemMsg:");
    }


    QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("\r\nDateTime:%5\r\n%1\r\nFile:%2\r\nLine:%3\r\nFunction:%4\r\n") .arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strTime);
    strMessage=strMsg+strMessage;


    //QFile file("/home/wck/log.txt");
    if(flag==0){

        QString log_fileaddr=QCoreApplication::applicationDirPath()+"/log_file.txt";
        QFile log_file(log_fileaddr);
        log_file.open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream stream(&log_file);
        stream << strMessage << "\r\n";
        log_file.flush();
        log_file.close();
    }
    else{
        //qDebug()  output ,这个文件可能很繁杂
        QString qDbug_fileaddr=QCoreApplication::applicationDirPath()+"/qDebug_log_file.txt";
        QFile qDebug_log_file(qDbug_fileaddr);
        qDebug_log_file.open(QIODevice::ReadWrite | QIODevice::Append);
        QTextStream stream(&qDebug_log_file);
        stream << strMessage << "\r\n";
        qDebug_log_file.flush();
        qDebug_log_file.close();
    }





    Meg_mutex.unlock();
}
