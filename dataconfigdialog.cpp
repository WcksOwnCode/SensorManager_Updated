#include "dataconfigdialog.h"
#include "ui_dataconfigdialog.h"


DataConfigDialog::DataConfigDialog(QList<DeviceInfo*> *CList,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataConfigDialog)
{
    ui->setupUi(this);
   model=new QStandardItemModel;
   QStringList list;

   list<<"id "<<"mac"<<"name"<<"time"<<"accx"<<"accy"<<"accz"<<"alpha"<<"beta"<<"gama"<<"distance"<<"motion"<<"temperature"
      <<"IR"<<"Iuv"<<"intensity"<<"rssi"<<"voltage";

       model->setHorizontalHeaderLabels(list);
       QHeaderView *header=new QHeaderView(Qt::Horizontal);
       header->setModel(model);
       ui->Data_tableWidget->setHorizontalHeader(header);
      DeviceList=CList;
      for(int i=0;i<DeviceList->length();i++){
          ui->Mac_comboBox->addItem((*DeviceList)[i]->mac);
          ui->Name_comboBox->addItem((*DeviceList)[i]->name);
      }

      db = QSqlDatabase::database("DeviceData"); //建立数据库连接
      if(db.isOpen()){
      }
      else{
          db.open();
      }

      ui->DataBaseStatu_label->setText("已经连接");
      ui->Data_tableWidget->setStyleSheet("QTableView { border: none;"
                                        "background-color: #8EDE21;"
                                        "selection-background-color: #999}");
      ui->Data_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


     ui->Data_tableWidget->horizontalHeader()->setStretchLastSection(true);
     ui->Data_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

}

DataConfigDialog::~DataConfigDialog()
{
    delete ui;
}

void DataConfigDialog::on_ConnectDB_pushButton_clicked()
{
    QSqlQuery query(db);

    //QSqlRecord rec = query.record();
    // qDebug() << QObject::tr("automobil表字段数：" ) << rec.count();
        qDebug()<<"select * from Dev where mac = \""+ui->Mac_comboBox->currentText()+"\"";
        if(query.exec("select * from Dev where mac = '"+ui->Mac_comboBox->currentText()+"'")){

        }
        else{
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText();
        }
    int rowcount=1;
    ui->Data_tableWidget->clear();
    ui->Data_tableWidget->setRowCount(1000);
    ui->Data_tableWidget->setColumnCount(17);
    while(query.next())
    {
        for(int index = 0; index <17; index++){
           ui->Data_tableWidget->setItem(rowcount,index,new QTableWidgetItem(query.value(index).toString()));
            qDebug()<<query.value(index).toString();
        }
        rowcount++;
    }
}

void DataConfigDialog::on_Input_Query_pushButton_clicked()
{
    QString inputMac=ui->Input_Query_pushButton->text();
    if(inputMac.length()!=12){
        QMessageBox::warning(this,"warning","invalid Mac address!");
        return;
    }



    QSqlQuery query(db);

    //QSqlRecord rec = query.record();
    // qDebug() << QObject::tr("automobil表字段数：" ) << rec.count();
        qDebug()<<"select * from Dev where mac = \""+inputMac+"\"";
        if(query.exec("select * from Dev where mac = \""+inputMac+"\"")){

        }
        else{
            QSqlError lastError = query.lastError();
            qDebug() << lastError.driverText();
        }
    int rowcount=1;
    while(query.next())
    {


        for(int index = 0; index <17; index++){
            model->setItem(rowcount,index,new QStandardItem(query.value(index).toString()));
        }



    }

}

void DataConfigDialog::on_Mac_comboBox_currentIndexChanged(int index)
{
    ui->Name_comboBox->setCurrentIndex(index);
}

void DataConfigDialog::on_Name_comboBox_currentIndexChanged(int index)
{
    ui->Mac_comboBox->setCurrentIndex(index);
}
