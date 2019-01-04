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
    //添加折线图新加入一些必要设置
    ui->Data_tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//设置可多选 因为要求加折线图才加入的
    ui->Data_tableWidget->setStyleSheet("selection-background-color:lightblue;"); //设置选中之后的背景色
    ui->Data_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}"); //设置选中之后的表头背景色
    ui->Data_tableWidget->setSelectionBehavior(QAbstractItemView::SelectColumns); //选中为行

    connect(ui->Data_tableWidget,&QTableWidget::cellClicked,this,&DataConfigDialog::CellClicked_);
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
    data_get=true;
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
    qDebug()<<"choosed row "<<r<<"  choose colomn "<<c;

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

    //导入数据
    int maxrow= ui->Data_tableWidget->rowCount();
    int maxcol=ui->Data_tableWidget->columnCount();
    qDebug()<<"maxrow  "<<maxrow<<" maxcol  "<<maxcol;
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
    for(int i=0;i<items.length();i++){

        QTableWidgetItem*item=items.at(i);

       // qDebug()<<"index "<<i<<"value "<< item->text().toDouble();


        chartSerial->append(i,item->text().toDouble());

    }




    chart->addSeries(chartSerial);
    chart->createDefaultAxes();
    chart->setAnimationOptions(QChart::SeriesAnimations);  //在缩放窗口大小时可以用动画进行美化
    ui->ChartView->setChart(chart);


}
