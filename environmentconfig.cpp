#pragma execution_character_set("utf-8")
#include "environmentconfig.h"
#include "ui_environmentconfig.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>
EnvironmentConfig::EnvironmentConfig(QString &ConAddr, QString &CsvAddr,
                                     QString &DBaddr, bool &label, bool &multithread, bool &Con_ac, bool &Csv,
                                     bool &database, QVector<QString> BlockList,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnvironmentConfig)
{
    // env=new EnvironmentConfig(ConfigAddress,CsvUpperAddr,DBaddr,m_bShowOnCharts,
    // multiThread,m_bConfigAcvated,m_bCsv,m_bDatabaseAcvated);
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

    this->setWindowTitle("环境配置");
    ui->ConfigFile_lineEdit->hide();
    ui->ConfigActivatied_checkBox->hide();
    ui->Config_ChoosePath_pushButton->hide();
    ui->label->hide();
    ui->ChangeConfigAddrpushButton->hide();
    ui->Multithread_checkBox->hide();


    BlockItemList=BlockList;


    ui->BlockList_treeView->setContextMenuPolicy(Qt::CustomContextMenu);//开了这个才能开启菜单触发
    connect(ui->BlockList_treeView,SIGNAL(customContextMenuRequested(const QPoint& )),this,SLOT(BlockDeviceMenu(const QPoint&)));//右键菜单草



    UpdateTreeView();

}

EnvironmentConfig::~EnvironmentConfig()
{

    if(BlockItem!=nullptr){
        delete BlockItem;
        BlockItem=nullptr;

    }
    delete ui;
}
void EnvironmentConfig::UpdateTreeView(){
    if(BlockItem!=nullptr){
        delete BlockItem;
        BlockItem=nullptr;

    }
    BlockItem=new QStandardItemModel;
    BlockItem->setColumnCount(2);
    BlockItem->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("MAC")<<QStringLiteral("name"));
    for(int i=0;i<BlockItemList.size();i+=2){
        if(BlockItemList[i]=="")
            continue;
        QStandardItem* itemMac = new QStandardItem(BlockItemList[i]);
        itemMac->setEditable(false);
        QStandardItem* itemname = new QStandardItem(BlockItemList[i+1]);
        itemname->setEditable(false);
        BlockItem->setItem(i,0,itemMac);
        BlockItem->setItem(i,1,itemname);

    }
    ui->BlockList_treeView->setModel(BlockItem);

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

    this->close();
}
void EnvironmentConfig::closeEvent(QCloseEvent *event){
    emit envdone( this->configaddr,
                  this->Csv_addr,
                  this->DB_addr,
                  this->m_bMt,
                  this->m_bla,
                  this->m_bcfa,
                  this->m_bdba,
                  this->m_bcsva);

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

void EnvironmentConfig::on_Config_ChoosePath_pushButton_clicked()
{
    QString file_path = QFileDialog::getExistingDirectory(this, "Choose a path", "./");
   // qDebug()<<file_path;
    if(file_path.isEmpty())
    {
        QMessageBox::warning(this,"warning","invalid path");
        return;
    }
    configaddr=file_path;
    ui->ConfigFile_lineEdit->setText(configaddr);
}

void EnvironmentConfig::on_CSV_ChoosePath_pushButton_clicked()
{
    QString file_path = QFileDialog::getExistingDirectory(this, "Choose a path", "./");
   // qDebug()<<file_path;
    if(file_path.isEmpty())
    {
        QMessageBox::warning(this,"warning","invalid path");
        return;
    }
    Csv_addr=file_path;
    ui->CSVaddr_lineEdit->setText(Csv_addr);
}

void EnvironmentConfig::on_Database_ChoosePath_pushButton_clicked()
{
    QString file_path = QFileDialog::getExistingDirectory(this, "Choose a path", "./");
   // qDebug()<<file_path;
    if(file_path.isEmpty())
    {
        QMessageBox::warning(this,"warning","invalid path");
        return;
    }
    DB_addr=file_path;
    ui->DataBase_lineEdit->setText(DB_addr);
}
void EnvironmentConfig::BlockDeviceMenu(const QPoint& pos){
   // qDebug()<<pos;
    QModelIndex curIndex =ui->BlockList_treeView->indexAt(pos);
   // qDebug()<<curIndex;
    QModelIndex index = curIndex.sibling(curIndex.row(),0);

    //get mac
    QString IndexMac=index.data().toString();
   // qDebug()<<IndexMac;
    QMenu menu;
    QAction* action1 = new QAction(&menu);
    action1 ->setObjectName("action1");
    action1 ->setText(tr("Remove"));
    menu.addAction(action1);
    QObject::connect(action1,&QAction::triggered,this,[=]{
        RemoveBlockItem(IndexMac);
    });
    menu.exec(ui->BlockList_treeView->mapToGlobal(pos));


    //scan and add the device to  ScanDevList

}

void EnvironmentConfig::RemoveBlockItem(QString Mac){
    for(int i=0;i<BlockItemList.length();i+=2){
        if(BlockItemList[i]==Mac){
           // qDebug()<<"remove "<<i;
            BlockItemList.removeAt(i);
            BlockItemList.removeAt(i);
            emit Remove(Mac);
            UpdateTreeView();
            break;
        }
    }
}
