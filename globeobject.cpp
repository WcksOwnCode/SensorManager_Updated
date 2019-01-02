#include "globeobject.h"
#include "serialportdialog.h"

quint16  GlobeObject::port_=0;
QTcpServer * GlobeObject::Server_=nullptr;
QTcpSocket * GlobeObject::Socket_=nullptr;

quint16  GlobeObject::port_2=0;
QTcpServer * GlobeObject::Server_2=nullptr;
QTcpSocket * GlobeObject::Socket_2=nullptr;

QSerialPort *GlobeObject::serial=nullptr;
GlobeObject::GlobeObject()
{

}
QStringList GlobeObject::ControlCommand{
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

bool GlobeObject::_Assert(short v1,int v2,_type ty){
    qDebug()<<v1<<v2<<ty;
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
bool GlobeObject::_Assert(float v1,int v2,_type ty){
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
void DeviceInfo::Method_exec(){
    qDebug()<<"method exec";
    for(int i=0;i<ControlMethodList.length();i++){
        qDebug()<<"method exec index "<<i;
        qDebug()<<"method exec sequece "<<ControlMethodList[i]->sequece;
        bool ok1=false,ok2=false;
        switch(ControlMethodList[i]->sequece){
         case 0:  //short motion;  ------------------0
            ok1=GlobeObject::_Assert(this->datainfo.motion,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 1://short accelx;  ------------------1
            ok1=GlobeObject::_Assert(this->datainfo.accelx,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 2:// short accely;  ------------------2
            ok1=GlobeObject::_Assert(this->datainfo.accely,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 3://short accelz;  ------------------3
            ok1=GlobeObject::_Assert(this->datainfo.accelz,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 4:// short intensity;-----------------4
            ok1=GlobeObject::_Assert(this->datainfo.intensity,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 5://short IR;      ------------------5
            ok1=GlobeObject::_Assert(this->datainfo.IR,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 6://short iUV;     ------------------6
            ok1=GlobeObject::_Assert(this->datainfo.iUV,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 7://short distance;------------------7
            ok1=GlobeObject::_Assert(this->datainfo.distance,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 8://float tilt1;   ------------------8
            ok1=GlobeObject::_Assert(this->datainfo.tilt1,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 9://float tilt2;   ------------------9
            ok1=GlobeObject::_Assert(this->datainfo.tilt2,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 10://float tilt3;   ------------------10
            ok1=GlobeObject::_Assert(this->datainfo.tilt3,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        case 11://float temperature;---------------11
            ok1=GlobeObject::_Assert(this->datainfo.temperature,ControlMethodList[i]->i_value,ControlMethodList[i]->i_sym_one);
            break;
        }
        qDebug()<<"check value "<<ControlMethodList[i]->i_value<<"  dev   "<<this->datainfo.accelx;
        qDebug()<<"ok1 check "<<ok1;
        if(ok1){
            OrderSend(ControlMethodList[i]->Message_);
        }
        if(ControlMethodList[i]->_Log!=LOGI_NONE){
            //条件2 检查
            switch(ControlMethodList[i]->sequece){
             case 0:  //short motion;  ------------------0
                ok2=GlobeObject::_Assert(this->datainfo.motion,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 1://short accelx;  ------------------1
                ok2=GlobeObject::_Assert(this->datainfo.accelx,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 2:// short accely;  ------------------2
                ok2=GlobeObject::_Assert(this->datainfo.accely,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 3://short accelz;  ------------------3
                ok2=GlobeObject::_Assert(this->datainfo.accelz,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 4:// short intensity;-----------------4
                ok2=GlobeObject::_Assert(this->datainfo.intensity,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 5://short IR;      ------------------5
                ok2=GlobeObject::_Assert(this->datainfo.IR,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 6://short iUV;     ------------------6
                ok2=GlobeObject::_Assert(this->datainfo.iUV,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 7://short distance;------------------7
                ok2=GlobeObject::_Assert(this->datainfo.distance,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 8://float tilt1;   ------------------8
                ok2=GlobeObject::_Assert(this->datainfo.tilt1,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 9://float tilt2;   ------------------9
                ok2=GlobeObject::_Assert(this->datainfo.tilt2,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 10://float tilt3;   ------------------10
                ok2=GlobeObject::_Assert(this->datainfo.tilt3,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            case 11://float temperature;---------------11
                ok2=GlobeObject::_Assert(this->datainfo.temperature,ControlMethodList[i]->i_value_2,ControlMethodList[i]->i_sym_two);
                break;
            }

            if(ControlMethodList[i]->_Log==LOGI_AND){
                if(ok1&ok2){
                    qDebug()<<"ok1 & ok2";
                  OrderSend(ControlMethodList[i]->Message_);
                }
            }
            if(ControlMethodList[i]->_Log==LOGI_OR){
                qDebug()<<"ok1 or ok2";
                if(ok1|ok2){
                   OrderSend(ControlMethodList[i]->Message_);
                }
            }

        }


    }
}

void DeviceInfo::OrderSend(int index){
    if(GlobeObject::serial!=nullptr){
        QString Data=GlobeObject::ControlCommand[index];
        QByteArray Hexdata;
        SerialPortDialog::QString2Hex(Data,Hexdata);
        GlobeObject::serial->write(Hexdata);
        qDebug()<<"send info "<<GlobeObject::ControlCommand[index];
    }
    else{
        qDebug()<<"send info "<<GlobeObject::ControlCommand[index];
        qDebug()<<"串口没有打开";
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
