#pragma execution_character_set("utf-8")
#include "dataconfigdialog.h"
#include "ui_dataconfigdialog.h"


DataConfigDialog::DataConfigDialog(QList<DeviceInfo*> *CList,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataConfigDialog)
{
    ui->setupUi(this);
    model=new QStandardItemModel;

    Headerlist<<"id "<<"mac"<<"name"<<"time"<<"accx"<<"accy"<<"accz"<<"alpha"<<"beta"<<"gama"<<"distance"<<"motion"<<"temperature"
             <<"IR"<<"Iuv"<<"intensity"<<"rssi"<<"voltage";

    model->setHorizontalHeaderLabels(Headerlist);
    QHeaderView *header=new QHeaderView(Qt::Horizontal);
    header->setModel(model);
    ui->Data_tableWidget->setHorizontalHeader(header);
    DeviceList=CList;
    for(int i=0;i<DeviceList->length();i++){
        ui->Mac_comboBox->addItem((*DeviceList)[i]->mac);
        ui->Name_comboBox->addItem((*DeviceList)[i]->name);
    }

   /* db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    if(db.isOpen()){
    }
    else{
        db.open();
    }

    ui->DataBaseStatu_label->setText("已经连接");*/
    ui->Data_tableWidget->setStyleSheet("QTableView { border: none;"
                                        "background-color: #8EDE21;"
                                        "selection-background-color: #999}");
    ui->Data_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Data_tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->Data_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //添加折线图新加入一些必要设置
    ui->Data_tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//设置可多选 因为要求加折线图才加入的
    ui->Data_tableWidget->setStyleSheet("selection-background-color:lightblue;"); //设置选中之后的背景色
    ui->Data_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置选中之后的表头背景色
    ui->Data_tableWidget->setSelectionBehavior(QAbstractItemView::SelectColumns); //选中为行

    connect(ui->Data_tableWidget,&QTableWidget::cellClicked,this,&DataConfigDialog::CellClicked_);

    this->setWindowTitle(QStringLiteral("DataBase"));



}

DataConfigDialog::~DataConfigDialog()
{
    if(chartSerial!=nullptr){
        delete chartSerial;
    }
    if(chart!=nullptr){
        delete chart;
    }
    delete ui;
}

void DataConfigDialog::on_ConnectDB_pushButton_clicked()
{  

    int maxRowcount=ui->MaxItemCount_spinBox->text().toInt();



    ui->Data_tableWidget->setRowCount(maxRowcount);
    ui->Data_tableWidget->setColumnCount(17);
    emit QueryData(ui->Mac_comboBox->currentText(),maxRowcount);

}

void DataConfigDialog::on_Input_Query_pushButton_clicked()
{
    QString inputMac=ui->Mac_Input_lineEdit->text();
    if(inputMac.length()!=12){
       // qDebug()<<inputMac<<"   length   "<<inputMac.length();
        QMessageBox::warning(this,"warning","invalid Mac address!");
        return;
    }
    QSqlQuery query(db);
    int maxRowcount=ui->MaxItemCount_spinBox->text().toInt();
    ui->Data_tableWidget->setRowCount(maxRowcount);
    ui->Data_tableWidget->setColumnCount(17);
    emit QueryData(inputMac,maxRowcount);
}

void DataConfigDialog::on_Mac_comboBox_currentIndexChanged(int index)
{
    ui->Name_comboBox->setCurrentIndex(index);
}

void DataConfigDialog::on_Name_comboBox_currentIndexChanged(int index)
{
    ui->Mac_comboBox->setCurrentIndex(index);
}
void DataConfigDialog::CellClicked_(int r,int c){
   // qDebug()<<"choosed row "<<r<<"  choose colomn "<<c;

    focus_col=c;
    focus_row=r;

}

void DataConfigDialog::on_DrawOnchart_pushButton_clicked()
{
    // 选中数据画图
    if(focus_col==-1|focus_row==-1||focus_col<4){
        QMessageBox::warning(this,"warning","invalid focus cells");
        return;
    }

    int Data_s=ui->IndexStart_spinBox->text().toInt();
    int Data_e=ui->IndexEnd_spinBox->text().toInt();
    if(Data_e<Data_s){
        QMessageBox::warning(nullptr,"error",QString::fromLocal8Bit("索引起始点大于终止点"));
        return;
    }
    if(Data_e>data_count){
        QMessageBox::warning(nullptr,"error",QString::fromLocal8Bit("索引超过数据总数"));

        return;
    }
    //导入数据
    int maxrow= ui->Data_tableWidget->rowCount();
    int maxcol=ui->Data_tableWidget->columnCount();
   // qDebug()<<"maxrow  "<<maxrow<<" maxcol  "<<maxcol;
    chart=new QChart();
    // chart->axisX()->setRange(0,maxrow-1);
    // chart->axisY()->setRange(-1200,1200);
    chart->setMargins(QMargins(0,0,0,0));//铺满
    chartSerial=new QLineSeries;
    // qDebug()<<"set name"<<ui->Data_tableWidget->takeHorizontalHeaderItem(focus_col);
    chartSerial->setName(Headerlist[focus_col]);
    chartSerial->setColor(Qt::red);
    QList<QTableWidgetItem*>items=ui->Data_tableWidget->selectedItems();
    ui->ChartView->setAutoFillBackground(true);
    // qDebug()<<items;

    // qDebug()<<"for to get data"<<"x "<<focus_col;

    for(int i=Data_s;i<Data_e;i++){

        QTableWidgetItem*item=items.at(i);

        // qDebug()<<"index "<<i<<"value "<< item->text().toDouble();


        chartSerial->append(i,item->text().toDouble());

    }
    chart->addSeries(chartSerial);
    chart->createDefaultAxes();
    chart->setAnimationOptions(QChart::SeriesAnimations);  //在缩放窗口大小时可以用动画进行美化
    ui->ChartView->setChart(chart);
}

void DataConfigDialog::on_Accept_pushButton_clicked()
{
    this->close();
}
void DataConfigDialog::GetFromDataBase(QList< QList<QString>*>*Data){

    qDebug()<<"get data from database";
    for(int i=0;i<Data->length();i++){
        for(int j=0;j<Data->at(i)->length();j++){
            if(j<17){
                ui->Data_tableWidget->setItem(i,j,new QTableWidgetItem(Data->at(i)->at(j)));
                //qDebug()<<"data at"<< i<<" "<<j<<Data->at(i)->at(j);
            }
        }
    }
    data_count=Data->length();
    ui->IndexEnd_spinBox->setValue(data_count);
    ui->Data_tableWidget->horizontalHeader()->setDragEnabled(true);
    ui->Data_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //为了防止在画曲线图时候数据数量选择越界，此处直接设置可选范围，后续报错越界也就不需要了
    ui->IndexStart_spinBox->setRange(1,data_count-1);
    ui->IndexEnd_spinBox->setRange(1,data_count);
}

void DataConfigDialog::on_IndexStart_spinBox_valueChanged(int arg1)
{
    if(arg1<0){
        ui->IndexStart_spinBox->setValue(0);
    }
    if(arg1>data_count){
        ui->IndexStart_spinBox->setValue(data_count);
    }
}
