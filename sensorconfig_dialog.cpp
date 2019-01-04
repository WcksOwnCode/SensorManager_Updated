#include "sensorconfig_dialog.h"
#include "ui_sensorconfig_dialog.h"
#include "GlobalObject.h"

SensorConfig_Dialog::SensorConfig_Dialog(QList<DeviceInfo *> DL, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SensorConfig_Dialog)
{
    qDebug()<<"dadsda1";
    ui->setupUi(this);
    qDebug()<<"dadsda2";
    ui->Mac_comboBox->addItem("");

   ui->Name_comboBox->addItem("");

    ui->DataType_comboBox->addItem("");
    qDebug()<<"dadsda5";
    for(int i=0;i<DL.length();i++){
        qDebug()<<"i"<<DL.length()<<"  "<<i;
        ui->Mac_comboBox->addItem(DL[i]->mac);
        ui->Name_comboBox->addItem(DL[i]->name);
    }
    qDebug()<<"dadsda6";
    this->DL=DL;
    qDebug()<<"dadsda7";
    ui->DataType_comboBox->addItem("motion");
    ui->DataType_comboBox->addItem("AccX");
    ui->DataType_comboBox->addItem("AccY");
    ui->DataType_comboBox->addItem("AccZ");
    ui->DataType_comboBox->addItem("intensity");
    ui->DataType_comboBox->addItem("IR");
    ui->DataType_comboBox->addItem("IUV");
    ui->DataType_comboBox->addItem("distance");
    ui->DataType_comboBox->addItem("alpha");
    ui->DataType_comboBox->addItem("beta");
    ui->DataType_comboBox->addItem("gama");
    ui->DataType_comboBox->addItem("temperature");



    SetCorrEnabled(false);
    SetAllControlEnabled(false);
    ui->Activated_2_checkBox_2->setEnabled(false);
    ui->Activated_3_checkBox_2->setEnabled(false);
    ui->Activated_4_checkBox->setEnabled(false);
    ui->Activated_5_checkBox_3->setEnabled(false);
    //LoadCurrentControlMethod();
    ck.push_back(ui->Activated_1_checkBox_2);
    ck.push_back(ui->Activated_2_checkBox_2);
    ck.push_back(ui->Activated_3_checkBox_2);
    ck.push_back(ui->Activated_4_checkBox);
    ck.push_back(ui->Activated_5_checkBox_3);
    Symbos.push_back(ui->Symbol_comboBox1);
    Symbos.push_back(ui->Symbol_comboBox2);
    Symbos.push_back(ui->Symbol_comboBox3);
    Symbos.push_back(ui->Symbol_comboBox4);
    Symbos.push_back(ui->Symbol_comboBox5);
    Symbos.push_back(ui->Symbol_comboBox6);
    Symbos.push_back(ui->Symbol_comboBox7);
    Symbos.push_back(ui->Symbol_comboBox8);
    Symbos.push_back(ui->Symbol_comboBox9);
    Symbos.push_back(ui->Symbol_comboBox10);
    LogicBox.push_back(ui->Logic_comboBox1);
    LogicBox.push_back(ui->Logic_comboBox2);
    LogicBox.push_back(ui->Logic_comboBox3);
    LogicBox.push_back(ui->Logic_comboBox4);
    LogicBox.push_back(ui->Logic_comboBox5);
    LineEditBox.push_back(ui->valuelineEdit1);
    LineEditBox.push_back(ui->valuelineEdit2);
    LineEditBox.push_back(ui->valuelineEdit3);
    LineEditBox.push_back(ui->valuelineEdit4);
    LineEditBox.push_back(ui->valuelineEdit5);
    LineEditBox.push_back(ui->valuelineEdit6);
    LineEditBox.push_back(ui->valuelineEdit7);
    LineEditBox.push_back(ui->valuelineEdit8);
    LineEditBox.push_back(ui->valuelineEdit9);
    LineEditBox.push_back(ui->valuelineEdit10);
    CommandBox.push_back(ui->Command_comboBox1);
    CommandBox.push_back(ui->Command_comboBox2);
    CommandBox.push_back(ui->Command_comboBox3);
    CommandBox.push_back(ui->Command_comboBox4);
    CommandBox.push_back(ui->Command_comboBox5);
    QImage img;

    if(img.load(":/new/logo/rsc/logo.jpg"))//before load execute qmake
    {
        QImage img2= img.scaled(ui->Image_label->width(),ui->Image_label->height());
        QPixmap qp=QPixmap::fromImage(img2);

        ui->Image_label->setPixmap(qp);
    }
    else{
        qDebug()<<" Load failed ";
    }
    ui->DataType_comboBox->setEnabled(false);
    ui->ClearAllControlMethod_pushButton->setEnabled(false);
    ui->ClearAllCorrection_pushButton->setEnabled(false);
    ui->Accept_pushButton->setEnabled(false);
    qDebug()<<"over";

   m_buiready=true;
}
SensorConfig_Dialog::~SensorConfig_Dialog()
{
    delete ui;
}
void SensorConfig_Dialog::LoadCurrentControlMethod(){

    int Cindex=ui->Mac_comboBox->currentIndex()-1;
    if(DL.length()==0||Cindex>=DL.length()){
        return;
    }
    if(Cindex>0){
        if(ui->DataType_comboBox->currentIndex()>0){
            int dataindex=ui->DataType_comboBox->currentIndex();
            for(int i=0;i<DL[Cindex]->ControlMethodList.length()&&DL[Cindex]->ControlMethodList[i]!=nullptr;i++){
                if(DL[Cindex]->ControlMethodList[i]->sequece==dataindex-1){
                    //load
                    //mafan!!!!!
                   ck[i]->setChecked(true);
                   Symbos[i*2]->setCurrentIndex(DL[Cindex]->ControlMethodList[i]->i_sym_one);
                   LineEditBox[i*2]->setText(QString::number(DL[Cindex]->ControlMethodList[i]->i_value));
                   if(DL[Cindex]->ControlMethodList[i]->_Log!=LOGI_NONE){
                     Symbos[i*2+1]->setCurrentIndex(DL[Cindex]->ControlMethodList[i]->i_sym_two);
                     LineEditBox[i*2+1]->setText(QString::number(DL[Cindex]->ControlMethodList[i]->i_value_2));
                     LogicBox[i]->setEnabled(true);
                     LogicBox[i]->setCurrentIndex(DL[Cindex]->ControlMethodList[i]->_Log);
                   }

                }
            }
        }
    }


}
void SensorConfig_Dialog::on_Mac_comboBox_currentIndexChanged(int index)
{
    if(!m_buiready){
        return;
    }
    ui->Name_comboBox->setCurrentIndex(index);
    if(index>0){
        SetValueFromDevice();
        ui->DataType_comboBox->setEnabled(true);
        ui->ClearAllControlMethod_pushButton->setEnabled(true);
        ui->ClearAllCorrection_pushButton->setEnabled(true);
        ui->Accept_pushButton->setEnabled(true);
    }
    else{
        SetValueFromDevice();
        ui->DataType_comboBox->setEnabled(false);
        ui->ClearAllControlMethod_pushButton->setEnabled(false);
        ui->ClearAllCorrection_pushButton->setEnabled(false);
        ui->Accept_pushButton->setEnabled(false);
    }
}

void SensorConfig_Dialog::on_Name_comboBox_currentIndexChanged(int index)
{
    if(!m_buiready){//构造函数中出发additem也会触发这个函数，会导致索引越界，用uiready来控制
        return;
    }
    ui->Mac_comboBox->setCurrentIndex(index);
    if(index>0){
        SetValueFromDevice();
        ui->DataType_comboBox->setEnabled(true);
        ui->ClearAllControlMethod_pushButton->setEnabled(true);
        ui->ClearAllCorrection_pushButton->setEnabled(true);
        ui->Accept_pushButton->setEnabled(true);
    }
    else{
        ui->DataType_comboBox->setEnabled(false);
        ui->ClearAllControlMethod_pushButton->setEnabled(false);
        ui->ClearAllCorrection_pushButton->setEnabled(false);
        ui->Accept_pushButton->setEnabled(false);
    }
}
void SensorConfig_Dialog::SetCorrEnabled(bool b){
    ui->CorIr->setEnabled(b);
    ui->CorAcc_A->setEnabled(b);
    ui->CorAcc_B->setEnabled(b);
    ui->CorAcc_C->setEnabled(b);
    ui->CorDis_A->setEnabled(b);
    ui->CorDis_B->setEnabled(b);
    ui->CorDis_C->setEnabled(b);
    ui->CorLight->setEnabled(b);
    ui->CorTemp_A->setEnabled(b);
    ui->CorTemp_B->setEnabled(b);
    ui->CorTemp_C->setEnabled(b);
    ui->CorTilt_A->setEnabled(b);
    ui->CorTilt_B->setEnabled(b);
    ui->CorTilt_C->setEnabled(b);
    ui->SetValue_pushButton->setEnabled(b);
}
void SensorConfig_Dialog::on_EditEnablecheckBox_stateChanged(int arg1)
{
    if(ui->EditEnablecheckBox->isChecked()){
        //选中
        SetCorrEnabled(true);
    }
    else {
        SetCorrEnabled(false);
    }
}
void SensorConfig_Dialog::SetValueFromDevice(){
    int index=ui->Name_comboBox->currentIndex()-1;

    if(index>=DL.length()){
        qDebug()<<"index out of range "<<index<<"   length is "<<DL.length();
        return;
    }
    ui->Datacorrelation_checkBox->setChecked(DL[index]->m_bCorrectValue);
    ui->CorIr->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrIR)));
    ui->CorAcc_A->setText(QString::number(DL[index]->Corr_Coe.CorrAccX));
    ui->CorAcc_B->setText(QString::number(DL[index]->Corr_Coe.CorrAccY));
    ui->CorAcc_C->setText(QString::number(DL[index]->Corr_Coe.CorrAccZ));
    ui->CorTemp_A->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrTempA)));
    ui->CorTemp_B->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrTempB)));
    ui->CorTemp_C->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrTempC)));
    ui->CorTilt_A->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrAlpha)));
    ui->CorTilt_B->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrBeta)));
    ui->CorTilt_C->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrGama)));
    ui->CorDis_A->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrDistanceA)));
    ui->CorDis_B->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrDistanceB)));
    ui->CorDis_C->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrDistanceC)));
    ui->CorLight->setText(QString::number(static_cast<double>(DL[index]->Corr_Coe.CorrLight)));
}

void SensorConfig_Dialog::on_SetValue_pushButton_clicked()
{
    int index=ui->Name_comboBox->currentIndex()-1;
    if(index>=DL.length()|index<0){
        qDebug()<<"index out of range "<<index<<"   length is "<<DL.length();
        return;
    }
    DL[index]->Corr_Coe.CorrIR=ui->CorIr->text().toFloat();
    DL[index]->Corr_Coe.CorrAccX=ui->CorAcc_A->text().toShort();
    DL[index]->Corr_Coe.CorrAccY=ui->CorAcc_B->text().toShort();
    DL[index]->Corr_Coe.CorrAccZ=ui->CorAcc_C->text().toShort();
    DL[index]->Corr_Coe.CorrTempA=ui->CorTemp_A->text().toShort();
    DL[index]->Corr_Coe.CorrTempB=ui->CorTemp_B->text().toShort();
    DL[index]->Corr_Coe.CorrTempC=ui->CorTemp_C->text().toShort();
    DL[index]->Corr_Coe.CorrAlpha=ui->CorTilt_A->text().toFloat();
    DL[index]->Corr_Coe.CorrBeta=ui->CorTilt_B->text().toFloat();
    DL[index]->Corr_Coe.CorrGama=ui->CorTilt_C->text().toFloat();
    DL[index]->Corr_Coe.CorrDistanceA=ui->CorDis_A->text().toFloat();
    DL[index]->Corr_Coe.CorrDistanceB=ui->CorDis_B->text().toFloat();
    DL[index]->Corr_Coe.CorrDistanceC=ui->CorDis_C->text().toFloat();
    DL[index]->Corr_Coe.CorrLight=ui->CorLight->text().toFloat();
    if(ui->Datacorrelation_checkBox->isChecked()){
        DL[index]->m_bCorrectValue=true;
    }
    else{
        DL[index]->m_bCorrectValue=false;
    }

    QMessageBox::information(this,"Success","Set Successed!");

}
void SensorConfig_Dialog::SetAllControlEnabled(bool b){

    ui->valuelineEdit1->setEnabled(b);
    ui->valuelineEdit2->setEnabled(b);
    ui->valuelineEdit3->setEnabled(b);
    ui->valuelineEdit4->setEnabled(b);
    ui->valuelineEdit5->setEnabled(b);
    ui->valuelineEdit6->setEnabled(b);
    ui->valuelineEdit7->setEnabled(b);
    ui->valuelineEdit8->setEnabled(b);
    ui->valuelineEdit9->setEnabled(b);
    ui->valuelineEdit10->setEnabled(b);
    ui->Symbol_comboBox1->setEnabled(b);
    ui->Symbol_comboBox2->setEnabled(b);
    ui->Symbol_comboBox3->setEnabled(b);
    ui->Symbol_comboBox4->setEnabled(b);
    ui->Symbol_comboBox5->setEnabled(b);
    ui->Symbol_comboBox6->setEnabled(b);
    ui->Symbol_comboBox7->setEnabled(b);
    ui->Symbol_comboBox8->setEnabled(b);
    ui->Symbol_comboBox9->setEnabled(b);
    ui->Symbol_comboBox10->setEnabled(b);
    ui->Logic_comboBox1->setEnabled(b);
    ui->Logic_comboBox2->setEnabled(b);
    ui->Logic_comboBox3->setEnabled(b);
    ui->Logic_comboBox4->setEnabled(b);
    ui->Logic_comboBox5->setEnabled(b);
    ui->Command_comboBox1->setEnabled(b);
    ui->Command_comboBox2->setEnabled(b);
    ui->Command_comboBox3->setEnabled(b);
    ui->Command_comboBox4->setEnabled(b);
    ui->Command_comboBox5->setEnabled(b);
}

void SensorConfig_Dialog::on_Activated_1_checkBox_2_stateChanged(int arg1)
{
    if(ui->Activated_1_checkBox_2->isChecked()){
        ui->valuelineEdit1->setEnabled(true);
        ui->Symbol_comboBox1->setEnabled(true);
        ui->Command_comboBox1->setEnabled(true);
        ui->Logic_comboBox1->setEnabled(true);
        ui->Activated_2_checkBox_2->setEnabled(true);
        ui->Activated_3_checkBox_2->setEnabled(false);
        ui->Activated_4_checkBox->setEnabled(false);
        ui->Activated_5_checkBox_3->setEnabled(false);

    }
    else{
        ui->valuelineEdit1->setEnabled(false);
        ui->Symbol_comboBox1->setEnabled(false);
        ui->Command_comboBox1->setEnabled(false);
        ui->valuelineEdit2->setEnabled(false);
        ui->Symbol_comboBox2->setEnabled(false);
        ui->Logic_comboBox1->setEnabled(false);
        ui->Activated_2_checkBox_2->setEnabled(false);
        ui->Activated_3_checkBox_2->setEnabled(false);
        ui->Activated_4_checkBox->setEnabled(false);
        ui->Activated_5_checkBox_3->setEnabled(false);

        ui->Activated_2_checkBox_2->setChecked(false);
        ui->Activated_3_checkBox_2->setChecked(false);
        ui->Activated_4_checkBox->setChecked(false);
        ui->Activated_5_checkBox_3->setChecked(false);
    }
}

void SensorConfig_Dialog::on_Activated_2_checkBox_2_stateChanged(int arg1)
{
    if(ui->Activated_2_checkBox_2->isChecked()){
        ui->valuelineEdit3->setEnabled(true);
        ui->Symbol_comboBox3->setEnabled(true);
        ui->Command_comboBox2->setEnabled(true);
        ui->Logic_comboBox2->setEnabled(true);
        ui->Activated_3_checkBox_2->setEnabled(true);
        ui->Activated_4_checkBox->setEnabled(false);
        ui->Activated_5_checkBox_3->setEnabled(false);
    }
    else{
        ui->valuelineEdit3->setEnabled(false);
        ui->Symbol_comboBox3->setEnabled(false);
        ui->Command_comboBox2->setEnabled(false);
        ui->valuelineEdit4->setEnabled(false);
        ui->Symbol_comboBox4->setEnabled(false);
        ui->Logic_comboBox2->setEnabled(false);
        ui->Activated_3_checkBox_2->setEnabled(false);
        ui->Activated_4_checkBox->setEnabled(false);
        ui->Activated_5_checkBox_3->setEnabled(false);
        ui->Activated_3_checkBox_2->setChecked(false);
        ui->Activated_4_checkBox->setChecked(false);
        ui->Activated_5_checkBox_3->setChecked(false);
    }
}

void SensorConfig_Dialog::on_Activated_3_checkBox_2_stateChanged(int arg1)
{
    if(ui->Activated_3_checkBox_2->isChecked()){
        ui->valuelineEdit5->setEnabled(true);
        ui->Symbol_comboBox5->setEnabled(true);
        ui->Command_comboBox3->setEnabled(true);
        ui->Logic_comboBox3->setEnabled(true);
        ui->Activated_4_checkBox->setEnabled(true);
        ui->Activated_5_checkBox_3->setEnabled(false);
    }
    else{
        ui->valuelineEdit5->setEnabled(false);
        ui->Symbol_comboBox5->setEnabled(false);
        ui->Command_comboBox3->setEnabled(false);
        ui->valuelineEdit6->setEnabled(false);
        ui->Symbol_comboBox6->setEnabled(false);
        ui->Logic_comboBox3->setEnabled(false);
        ui->Activated_4_checkBox->setEnabled(false);
        ui->Activated_5_checkBox_3->setEnabled(false);
        ui->Activated_4_checkBox->setChecked(false);
        ui->Activated_5_checkBox_3->setChecked(false);
    }
}

void SensorConfig_Dialog::on_Activated_4_checkBox_stateChanged(int arg1)
{
    if(ui->Activated_4_checkBox->isChecked()){
        ui->valuelineEdit7->setEnabled(true);
        ui->Symbol_comboBox7->setEnabled(true);
        ui->Command_comboBox4->setEnabled(true);
        ui->Logic_comboBox4->setEnabled(true);
        ui->Activated_5_checkBox_3->setEnabled(true);
    }
    else{
        ui->valuelineEdit7->setEnabled(false);
        ui->Symbol_comboBox7->setEnabled(false);
        ui->Command_comboBox4->setEnabled(false);
        ui->valuelineEdit8->setEnabled(false);
        ui->Symbol_comboBox8->setEnabled(false);
        ui->Logic_comboBox4->setEnabled(false);
        ui->Activated_5_checkBox_3->setEnabled(false);
        ui->Activated_5_checkBox_3->setChecked(false);
    }
}

void SensorConfig_Dialog::on_Activated_5_checkBox_3_stateChanged(int arg1)
{
    if(ui->Activated_5_checkBox_3->isChecked()){
        ui->valuelineEdit9->setEnabled(true);
        ui->Symbol_comboBox9->setEnabled(true);
        ui->Command_comboBox5->setEnabled(true);
        ui->Logic_comboBox5->setEnabled(true);
    }
    else{
        ui->valuelineEdit9->setEnabled(false);
        ui->Symbol_comboBox9->setEnabled(false);
        ui->Command_comboBox5->setEnabled(false);
        ui->valuelineEdit10->setEnabled(false);
        ui->Symbol_comboBox10->setEnabled(false);
        ui->Logic_comboBox5->setEnabled(false);
    }
}

void SensorConfig_Dialog::on_Logic_comboBox1_currentIndexChanged(int index)
{
    bool st=false;
    if(index==0){
        ui->valuelineEdit2->setEnabled(st);
        ui->Symbol_comboBox2->setEnabled(st);
    }
    else{
        ui->valuelineEdit2->setEnabled(!st);
        ui->Symbol_comboBox2->setEnabled(!st);
        ui->Logic_comboBox1->setEnabled(!st);
    }
}

void SensorConfig_Dialog::on_Logic_comboBox2_currentIndexChanged(int index)
{

    bool st=false;
    if(index==0){

        ui->valuelineEdit4->setEnabled(false);
        ui->Symbol_comboBox4->setEnabled(false);

    }
    else{
        ui->valuelineEdit4->setEnabled(!st);
        ui->Symbol_comboBox4->setEnabled(!st);
        ui->Logic_comboBox2->setEnabled(!st);
    }
}

void SensorConfig_Dialog::on_Logic_comboBox3_currentIndexChanged(int index)
{
    bool st=false;
    if(index==0){

        ui->valuelineEdit6->setEnabled(false);
        ui->Symbol_comboBox6->setEnabled(false);

    }
    else{
        ui->valuelineEdit6->setEnabled(!st);
        ui->Symbol_comboBox6->setEnabled(!st);
        ui->Logic_comboBox3->setEnabled(!st);
    }
}

void SensorConfig_Dialog::on_Logic_comboBox4_currentIndexChanged(int index)
{
    bool st=false;
    if(index==0){

        ui->valuelineEdit8->setEnabled(false);
        ui->Symbol_comboBox8->setEnabled(false);

    }
    else{
        ui->valuelineEdit8->setEnabled(!st);
        ui->Symbol_comboBox8->setEnabled(!st);
        ui->Logic_comboBox4->setEnabled(!st);
    }
}

void SensorConfig_Dialog::on_Logic_comboBox5_currentIndexChanged(int index)
{
    bool st=false;
    if(index==0){

        ui->valuelineEdit10->setEnabled(false);
        ui->Symbol_comboBox10->setEnabled(false);

    }
    else{
        ui->valuelineEdit10->setEnabled(!st);
        ui->Symbol_comboBox10->setEnabled(!st);
        ui->Logic_comboBox5->setEnabled(!st);
    }
}

void SensorConfig_Dialog::on_Accept_pushButton_clicked()
{
    //根据数据控制面板生成Method类并放置到对应Deviceinfo类中
    qDebug()<<"Accept_pushButton 1";
    Method * Mt=new Method();
    // int value1,value2;
    int C_index=ui->DataType_comboBox->currentIndex()-1;
    for(int i=0;i<5;i++){
        qDebug()<<"Accept_pushButton 1i"<<i;
        if(ck[i]->isChecked()){
            if(LogicBox[i]->currentIndex()!=0){
                switch(LogicBox[i]->currentIndex()){
                case 0:
                    Mt->_Log=LOGI_NONE;
                    break;
                case 1:
                    Mt->_Log=LOGI_AND;
                    break;
                case 2:
                    Mt->_Log=LOGI_OR;
                    break;
                }
            }
            else{
                Mt->_Log=LOGI_NONE;
            }

            switch(Symbos[i*2]->currentIndex()){
            case 0:
                Mt->i_sym_one=NONE;
                break;
            case 1://<
                Mt->i_sym_one=LESS;
                if(C_index<8){
                    Mt->i_value=LineEditBox[2*i]->text().toInt();
                }else{
                    Mt->f_value=LineEditBox[2*i]->text().toFloat();
                }
                qDebug()<<"Accept_pushButton less"<<i;
                break;
            case 2://=
                Mt->i_sym_one=EQUAL;
                if(C_index<8){
                    Mt->i_value=LineEditBox[2*i]->text().toInt();
                }else{
                    Mt->f_value=LineEditBox[2*i]->text().toFloat();
                }
                break;
            case 3://>
                Mt->i_sym_one=GREATER;
                if(C_index<8){
                    Mt->i_value=LineEditBox[2*i]->text().toInt();
                }else{
                    Mt->f_value=LineEditBox[2*i]->text().toFloat();
                }
                break;
            }
            if(Mt->_Log!=LOGI_NONE){
                //第二条件
                switch(Symbos[i*2]->currentIndex()){
                case 0:
                    Mt->i_sym_two=NONE;
                    break;
                case 1://<
                    Mt->i_sym_one=LESS;
                    if(C_index<8){
                        Mt->i_value_2=LineEditBox[2*i+1]->text().toInt();
                    }else{
                        Mt->f_value_2=LineEditBox[2*i+1]->text().toFloat();
                    }

                    break;
                case 2://=
                    Mt->i_sym_two=EQUAL;
                    if(C_index<8){
                        Mt->i_value_2=LineEditBox[2*i+1]->text().toInt();
                    }else{
                        Mt->f_value_2=LineEditBox[2*i+1]->text().toFloat();
                    }
                    break;
                case 3://>
                    Mt->i_sym_two=GREATER;
                    if(C_index<8){
                        Mt->i_value_2=LineEditBox[2*i+1]->text().toInt();
                    }else{
                        Mt->f_value_2=LineEditBox[2*i+1]->text().toFloat();
                    }
                    break;
                }
            }

            int id=ui->Mac_comboBox->currentIndex()-1;
            if(DL[id]->MethodCount[C_index]<DL[id]->ControlMethodCount_Max)
            {
                Mt->sequece=C_index;
                Mt->Message_=CommandBox[i]->currentIndex()-1;
                DL[ui->Mac_comboBox->currentIndex()-1]->ControlMethodList.push_back(Mt);
                DL[id]->MethodCount[C_index]++;
            }
            else{
                for(int i=0;i<DL[id]->ControlMethodList.length();i++){//替换最先创建的method
                    if(DL[id]->ControlMethodList[i]->sequece==C_index){
                        DL[id]->ControlMethodList.replace(i,Mt);
                    }
                }
                LoadCurrentControlMethod();
            }


        }
        else{
            break;
        }
    }

}

void SensorConfig_Dialog::on_ClearAllControlMethod_pushButton_clicked()
{
    if(ui->Mac_comboBox->currentIndex()>0){
        for(int i=0;i<DL[ui->Mac_comboBox->currentIndex()-1]->ControlMethodList.length();i++){
            if(DL[ui->Mac_comboBox->currentIndex()-1]->ControlMethodList[i]!=nullptr){
                delete DL[ui->Mac_comboBox->currentIndex()-1]->ControlMethodList[i];
                DL[ui->Mac_comboBox->currentIndex()-1]->ControlMethodList[i]=nullptr;
            }
        }
    }
}

void SensorConfig_Dialog::on_DataType_comboBox_currentIndexChanged(int index)
{
    if(index>0){
        ui->Activated_1_checkBox_2->setEnabled(true);
    }
    else{
        ui->Activated_1_checkBox_2->setEnabled(false);
    }
}
