#include "datathread.h"
#include <math.h>
DataThread::DataThread(QObject *parent)
{

}
QStringList DataThread::StringFileter(QString info,int ){
   /* info.remove(QChar('\n'), Qt::CaseInsensitive);
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
    qDebug()<<"cleared info  "<<info;
    qDebug()<<"info to list   "<<lists;
    while(pos<len){
        if(lists[pos]=="INQRESULT"|lists[pos]=="NOTIFY"
                |lists[pos]=="CHARACTERISTIC"|lists[pos]=="RSSI"
                |lists[pos]=="READCHAR"|lists[pos]=="DISC"
                |lists[pos]=="INQCOMPLETE"){


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

    return ret;*/
    return {""};
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
                for(int q=0;q<ConnectDevList.length();q++){
                    qDebug()<<"q "<<q<<"str "<<strList[2];
                    if(ConnectDevList[q]->mac==strList[1]){
                        ConnectDevList[q]->rssi=static_cast<short>(HexToDec(strList[2]))-256;
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
void DataThread::DataProcess_Simple(QByteArray fore, QByteArray back, DeviceInfo *dev, int index){
    m_bBusy=true;//
    qDebug()<<"data thread"<<fore<<"   "<<back;
    if(fore[0]='1'&&fore[1]=='2'&&fore[2]=='c'){
        //12c0 5a d9 47 26   00 4f   03 20   fd 72   00 13   00 00   ff d8   00 01
        //时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B
       qDebug()<<"时间4B+运动量2B+加速度x2B+加速度y2B+加速度z2B+倾角α2B+倾角β2B+倾角γ2B";
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
    else if(fore[0]='1'&&fore[1]=='1'&&fore[2]=='2'){
        //1120 5a dd 98 7a   00 00 00 04   00 00 01 00   01 84   00 01   00(时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B)
        //"\r\n+NOTIFY:E5AEFA129DDC,fff1,1120386f4fcd000000000000000001a3000101\r\n\r\n+NOTIFY:E5AEFA129DDC,fff1,06b0386f4fcd0000\r\n"
        //("E5AEFA129DDC", "fff1", "1120 386f558d 00000010 00000011 01a1 0001 01", "fff1", "06b0386f4fcd0000")
        qDebug()<<"时间4B+故障码4B+故障4B+电压2B+可用内存2B+工作模式1B";
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
        if(dev->faultcode>11|dev->faultcode<0){
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
        //0900 00 00   30 39   00   00 46   00 a8  光强2B+IR2B+iUV1B+距离2B+温度2B
        //        38 6d    e9 57   00   10 00 de f
        //qDebug()<<"SensorDataConvert"<<back<<"   size is  "<<back.length()<<"method 3";
        //09000090033e00002f0181
        //00 90    03 3e   00    00 2f    01 81
        //0900 00d1 03b4 00 0000 024e
        qDebug()<<"光强2B+IR2B+iUV1B+距离2B+温度2B";
        QByteArray  temp1,temp2;
        QString newbyte;
        //qDebug()<<"original "<<back;
        //qDebug()<<back[0]<<"  "<<back[1]<<" " <<back[2]<<" "<<back[3];

        dev->waitdata =false;
        dev->dataIntgrality=true;
        if(!dev->m_bCorrectValue){
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
            qDebug()<<temp1;
            dev->datainfo.temperature =HexToDec(temp1).toFloat()*0.1f;
            qDebug()<<"temo "<<dev->datainfo.temperature;
        }
        if(dev->m_bCorrectValue){
            dev->datainfo.IR +=static_cast<short>(dev->Corr_Coe.CorrIR);
            dev->datainfo.iUV+=static_cast<short>(dev->Corr_Coe.CorrLight);
            dev->datainfo.distance=dev->datainfo.distance*dev->datainfo.distance*dev->Corr_Coe.CorrDistanceA+dev->Corr_Coe.CorrDistanceB*dev->datainfo.distance+dev->Corr_Coe.CorrDistanceC;
            float tempe=dev->datainfo.temperature;

            dev->datainfo.temperature = tempe*tempe*dev->Corr_Coe.CorrTempA+tempe*dev->Corr_Coe.CorrTempB+dev->Corr_Coe.CorrTempC;
        }

    }
    if(back[1]=='0'&&back[2]=='6'){
        dev->waitdata =false;
        dev->dataIntgrality=true;
        qDebug()<<"data process ignore "<<back;
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

   if(arr[0]=='a'|arr[0]=='b'|arr[0]=='c'|arr[0]=='d'|arr[0]=='e'|arr[0]=='f'|arr[0]=='8'|arr[0]=='9'){
      qDebug()<<"more "<<value;
       value-=65536;
       qDebug()<<"adasd"<<value;
   }
    return QString::number(value);




}

