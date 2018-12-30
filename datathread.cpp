#include "datathread.h"
#include <math.h>
DataThread::DataThread(QObject *parent)
{

}
void DataThread::DataProcess(QString info){

    m_bBusy=true;//
#ifdef ss
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
        dev->time=HexTimeToDec(temp1);
        temp1.clear();
        temp2.clear();
        QDateTime local(QDateTime::currentDateTime());
        QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
        ui->ComputerTime_label->setText(localTime);
        for(int q=12;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);

        dev->waitdata=true;
        dev->datainfo.motion = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
        dev->datainfo.accelx = static_cast<short>((temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        dev->datainfo.accely = static_cast<short>((temp2[4]&0xFF)<<8|(temp2[5]&0xFF));
        dev->datainfo.accelz = static_cast<short>((temp2[6]&0xFF)<<8|(temp2[7]&0xFF));
        dev->datainfo.tilt1 = static_cast<float>((temp2[8]&0xFF)<<8|(temp2[9]&0xFF))/1000;;
        dev->datainfo.tilt2 = static_cast<float>((temp2[10]&0xFF)<<8|(temp2[11]&0xFF))/1000;
        dev->datainfo.tilt3 = static_cast<float>((temp2[12]&0xFF)<<8|(temp2[13]&0xFF))/1000;;
    }
    else if(fore[0]='1'&&fore[1]=='1'&&fore[2]=='2'){
        //1120 5a dd 98 7a   00 00 00 04   00 00   01 00   01 84   00 01   00(时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B)
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        //temp2=QByteArray::fromHex(temp1);//time

        dev->time=HexTimeToDec(temp1);
        // qDebug()<<"time2 is   "<<ConnectDevList[index];
        temp1.clear();
        temp2.clear();
        for(int q=12;q<19;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        dev->faultcode=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        for(int q=19;q<26;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        dev->fault=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        temp1.clear();
        temp2.clear();
        for(int q=26;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        dev->voltage = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));//dat[7];
        dev->memory =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        dev->workmode = temp2[4];
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
        dev->waitdata =false;
        dev->datainfo.intensity = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
        dev->datainfo.IR =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        dev->datainfo.iUV = temp2[4];
        dev->datainfo.distance = static_cast<short>((temp2[5]&0xFF)<<8|(temp2[6]&0xFF));
        dev->datainfo.temperature = 0.1f*static_cast<float>((temp2[7]&0xFF)<<8|(temp2[8]&0xFF));

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
#endif

}
void DataThread::SetRunning(bool tr){
    m_bisRunning=tr;
}
void DataThread::DataProcess_Simple(QByteArray fore, QByteArray back, DeviceInfo *dev, int index){
    m_bBusy=true;//
    qDebug()<<"data thread";
    if(fore[0]='1'&&fore[1]=='2'&&fore[2]=='c'){
        //12c0 5a d9 47 26   00 4f   03 20   fd 72   00 13   00 00   ff d8   00 01
        //时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B
        QByteArray  temp1,temp2;
        QString newbyte;
        for(int q=4;q<12;q++){
            temp1.push_back(fore[q]);
        }
        dev->time=HexTimeToDec(temp1);
        temp1.clear();
        temp2.clear();
        QDateTime local(QDateTime::currentDateTime());
        QString localTime = local.toString("yyyy-MM-dd:hh:mm:ss");
        //ui->ComputerTime_label->setText(localTime);
        for(int q=12;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        dev->waitdata=true;
        if(dev->m_bCorrectValue){
            dev->datainfo.motion = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            dev->datainfo.accelx = static_cast<short>((temp2[2]&0xFF)<<8|(temp2[3]&0xFF))+dev->Corr_Coe.CorrAccX;
            dev->datainfo.accely = static_cast<short>((temp2[4]&0xFF)<<8|(temp2[5]&0xFF))+dev->Corr_Coe.CorrAccY;
            dev->datainfo.accelz = static_cast<short>((temp2[6]&0xFF)<<8|(temp2[7]&0xFF))+dev->Corr_Coe.CorrAccZ;
            dev->datainfo.tilt1 = static_cast<float>((temp2[8]&0xFF)<<8|(temp2[9]&0xFF))/1000+dev->Corr_Coe.CorrAlpha;
            dev->datainfo.tilt2 = static_cast<float>((temp2[10]&0xFF)<<8|(temp2[11]&0xFF))/1000+dev->Corr_Coe.CorrBeta;
            dev->datainfo.tilt3 = static_cast<float>((temp2[12]&0xFF)<<8|(temp2[13]&0xFF))/1000+dev->Corr_Coe.CorrGama;
        }
        else{
            dev->datainfo.motion = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            dev->datainfo.accelx = static_cast<short>((temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
            dev->datainfo.accely = static_cast<short>((temp2[4]&0xFF)<<8|(temp2[5]&0xFF));
            dev->datainfo.accelz = static_cast<short>((temp2[6]&0xFF)<<8|(temp2[7]&0xFF));
            dev->datainfo.tilt1 = static_cast<float>((temp2[8]&0xFF)<<8|(temp2[9]&0xFF))/1000;;
            dev->datainfo.tilt2 = static_cast<float>((temp2[10]&0xFF)<<8|(temp2[11]&0xFF))/1000;
            dev->datainfo.tilt3 = static_cast<float>((temp2[12]&0xFF)<<8|(temp2[13]&0xFF))/1000;;
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

        dev->time=HexTimeToDec(temp1);
        // qDebug()<<"time2 is   "<<ConnectDevList[index];
        temp1.clear();
        temp2.clear();
        for(int q=12;q<19;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        dev->faultcode=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        for(int q=19;q<26;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);
        dev->fault=static_cast<short>((temp2[0]&0xFF)<<24|(temp2[1]&0xFF)<<16|(temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        temp1.clear();
        temp2.clear();
        for(int q=26;q<40;q++){
            temp1.push_back(fore[q]);
        }
        temp2=QByteArray::fromHex(temp1);//guzhangma
        dev->voltage = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));//dat[7];
        dev->memory =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
        dev->workmode = temp2[4];
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
        dev->waitdata =false;
        if(dev->m_bCorrectValue){
            dev->datainfo.intensity = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            dev->datainfo.IR =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF))+static_cast<short>(dev->Corr_Coe.CorrIR);
            dev->datainfo.iUV = temp2[4]+static_cast<short>(dev->Corr_Coe.CorrLight);

            short temp= static_cast<short>((temp2[5]&0xFF)<<8|(temp2[6]&0xFF));
            dev->datainfo.distance=temp*temp*dev->Corr_Coe.CorrDistanceA+dev->Corr_Coe.CorrDistanceB*temp+dev->Corr_Coe.CorrDistanceC;
            float tempe=0.1f*static_cast<float>((temp2[7]&0xFF)<<8|(temp2[8]&0xFF));
            dev->datainfo.temperature = tempe*tempe*dev->Corr_Coe.CorrTempA+tempe*dev->Corr_Coe.CorrTempB+dev->Corr_Coe.CorrTempC;
            // qDebug()<<"temprature "<<ConnectDevList[index]->datainfo.temperature;
        }else{
            dev->datainfo.intensity = static_cast<short>((temp2[0]&0xFF)<<8|(temp2[1]&0xFF));
            dev->datainfo.IR =static_cast<short>( (temp2[2]&0xFF)<<8|(temp2[3]&0xFF));
            dev->datainfo.iUV = temp2[4];
            dev->datainfo.distance = static_cast<short>((temp2[5]&0xFF)<<8|(temp2[6]&0xFF));
            dev->datainfo.temperature = 0.1f*static_cast<float>((temp2[7]&0xFF)<<8|(temp2[8]&0xFF));
            // qDebug()<<"temprature "<<ConnectDevList[index]->datainfo.temperature;
        }

    }
    dev->Method_exec();
     m_bBusy=false;
    emit workDone(dev,index);


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
