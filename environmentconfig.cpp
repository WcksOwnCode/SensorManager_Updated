#include "environmentconfig.h"
#include "ui_environmentconfig.h"

EnvironmentConfig::EnvironmentConfig(QString &ConAddr, QString &CsvAddr,
                                     QString &DBaddr, bool &label, bool &multithread, bool &Csv,
                                     bool &database, bool &Con_ac, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnvironmentConfig)
{
    ui->setupUi(this);
    this->configaddr=ConAddr;
    this->DB_addr=DB_addr;
    this->Csv_addr=CsvAddr;
    this->m_bMt=multithread;
    this->m_bla=label;
    this->m_bcfa=Con_ac;
    this->m_bdba=database;
    this->m_bcsva=Csv;
    ui->CSVaddr_lineEdit->setText(CsvAddr);
    ui->ConfigFile_lineEdit->setText(ConAddr);
    ui->DataBase_lineEdit->setText(DBaddr);
    ui->Multithread_checkBox->setChecked(m_bMt);
    ui->ShowOnLabel_checkBox->setChecked(m_bla);
    ui->ConfigActivatied_checkBox->setChecked(Con_ac);
    ui->CsvActivatied_checkBox->setChecked(Csv);
    ui->DataBaseActivatied_checkbox->setChecked(database);

}

EnvironmentConfig::~EnvironmentConfig()
{
    delete ui;
}

void EnvironmentConfig::on_ChangeConfigAddrpushButton_clicked()
{
    this->configaddr=ui->ConfigFile_lineEdit->text();
}

void EnvironmentConfig::on_ChangeCsvAddrpushButton_clicked()
{
    this->Csv_addr=ui->CSVaddr_lineEdit->text();
}

void EnvironmentConfig::on_ChangeDBaddrpushButton_clicked()
{
    this->DB_addr=ui->DataBase_lineEdit->text();
}

void EnvironmentConfig::on_ConfigActivatied_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        m_bcfa=false;

    }
    else{
        m_bcfa=true;
    }
}

void EnvironmentConfig::on_CsvActivatied_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        m_bcsva=false;

    }
    else{
        m_bcsva=true;
    }
}

void EnvironmentConfig::on_DataBaseActivatied_checkbox_stateChanged(int arg1)
{
    if(arg1==0){
        m_bdba=false;

    }
    else{
        m_bdba=true;
    }
}

void EnvironmentConfig::on_Accept_pushButton_clicked()
{
    emit envdone( this->configaddr,
            this->DB_addr,
            this->Csv_addr,
            this->m_bMt,
            this->m_bla,
            this->m_bcfa,
            this->m_bdba,
            this->m_bcsva);
    this->close();
}

void EnvironmentConfig::on_Multithread_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        m_bMt=false;

    }
    else{
        m_bMt=true;
    }
}

void EnvironmentConfig::on_ShowOnLabel_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        m_bla=false;

    }
    else{
        m_bla=true;
    }
}

void EnvironmentConfig::on_ChangeIp_pushButton_clicked()
{
    emit changeIp(ui->IpInput_lineEdit->text());
}
