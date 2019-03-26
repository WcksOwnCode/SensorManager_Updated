#pragma execution_character_set("utf-8")
#include "database_thread.h"
#include <QMessageBox>
DataBase_thread::DataBase_thread(QObject*parent){


                QueryResults=new QList<QList<QString>*>;


}
DataBase_thread::~DataBase_thread(){

    while(QueryResults->length()>0){
        QueryResults->removeAt(0);
    }
    QueryResults->clear();
    delete QueryResults;

}


void DataBase_thread::ConnectToDB(){
    db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    if(db.isOpen()){
    }
    else{
        db.open();
    }
    if(db.isOpen()){
        qDebug()<<"database connect";
    }


   /* QSqlQuery query(db);

    //QSqlRecord rec = query.record();
    // qDebug() << QObject::tr("automobil表字段数：" ) << rec.count();
    if(query.exec("select * from Dev where mac = '"+ui->Mac_comboBox->currentText()+"'")){
    }
    else{
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText();
    }
    int rowcount=1;
    int maxRowcount=ui->MaxItemCount_spinBox->text().toInt();
    ui->Data_tableWidget->clear();

    while(query.next())
    {
        for(int index = 0; index <17; index++){
            ui->Data_tableWidget->setItem(rowcount,index,new QTableWidgetItem(query.value(index).toString()));
            qDebug()<<query.value(index).toString();

        }
        rowcount++;
    }
    if(rowcount<maxRowcount){
        maxRowcount=rowcount;
    }

    ui->Data_tableWidget->setRowCount(maxRowcount);
    ui->Data_tableWidget->setColumnCount(17);*********/

}
bool DataBase_thread::createTable()
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    db.open();
    QSqlQuery query(db);
    QStringList strTables = Db.tables();
    if(strTables.contains("Dev")){
        qDebug() << "Allready exit the table dev";
        //return true;
    }
    else{
        qDebug() << "no exit table";
    }
    bool ss=query.exec("create table Dev(id integer primary key,"
                       "Mac varchar(20),"
                       "name varchar(30),"
                       "Time varchar(50),"
                       "AccX short, "
                       "AccY short,"
                       "AccZ short,"
                       "Tilt_1 float,"
                       "Tilt_2 float,"
                       "Tilt_3 float,"
                       "Distance short,"
                       "motion short,"
                       "temperature float,"
                       "IR short,"
                       "IUV short,"
                       "Intensity short,"
                       "rssi short,"
                       "voltage float)");
    //bool ss= query.exec("create table user(id varchar primary key,passwd varchar,nickname varchar null)");
    if(ss)
    {
        qDebug() << QObject::tr("数据库表创建成功!");
        return true;
    }
    else
    {
        qDebug()<<query.lastError();
        qDebug() << QObject::tr("数据库表创建失败!");
        return false;
    }
}

bool DataBase_thread::createTable(const DeviceInfo *Dinfo){

    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    QStringList strTables = Db.tables();
    if(strTables.contains(Dinfo->mac)){
        qDebug() << "Allready exit the table dev";
        //return true;
    }
    else{
        qDebug() << "no";
    }

    bool success = query.exec("create table"+Dinfo->mac+"(id int primary key,name string, Time string,"
                                                        " AccX short, AccY short, AccZ short, Tilt_1 float, Tilt_2 float,"
                                                        " Tilt_3 float, Distance short, motion short, temperature float,"
                                                        " IR short, IUV short, Intensity short, rssi short, voltage float)");
    if(success)
    {
        qDebug()<<query.lastError();
        qDebug() << QObject::tr("数据库表创建成功！\n");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("数据库表创建失败！\n");
        return false;
    }
}
//向数据库中插入记录
bool DataBase_thread::insert(const DeviceInfo *Dinfo)
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //添加数据库驱动，并指定连接名称
    QSqlQuery query(db);
    query.prepare("insert into Dev(Mac,name,Time,AccX,AccY,AccZ,Tilt_1,Tilt_2,Tilt_3,Distance,motion,temperature,IR,IUV,Intensity,rssi,voltage)"
                  "values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    //query.addBindValue(max_id+1);
    query.addBindValue(Dinfo->mac);
    query.addBindValue(Dinfo->name);
    query.addBindValue(Dinfo->time);
    query.addBindValue(Dinfo->datainfo.accelx);
    query.addBindValue(Dinfo->datainfo.accely);
    query.addBindValue(Dinfo->datainfo.accelz);
    query.addBindValue(Dinfo->datainfo.tilt1);
    query.addBindValue(Dinfo->datainfo.tilt2);
    query.addBindValue(Dinfo->datainfo.tilt3);
    query.addBindValue(Dinfo->datainfo.distance);
    query.addBindValue(Dinfo->datainfo.motion);
    query.addBindValue(Dinfo->datainfo.temperature);
    query.addBindValue(Dinfo->datainfo.IR);
    query.addBindValue(Dinfo->datainfo.iUV);
    query.addBindValue(Dinfo->datainfo.intensity);
    query.addBindValue(Dinfo->rssi);
    query.addBindValue(Dinfo->voltage);
    // DbCount+=17;
    bool success=query.exec();
    if(!success)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText() << QString(QObject::tr("插入失败"));
        return false;
    }
    else{
         qDebug()<<"Insert success!";
        //  qDebug()<<query.executedQuery();

    }
    return true;
}
//查询所有信息
bool DataBase_thread::queryAll()
{
    qDebug()<<"query all";
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    query.exec("select * from Dev");
    QSqlRecord rec = query.record();
    // qDebug() << QObject::tr("automobil表字段数：" ) << rec.count();
    while(query.next())
    {
        for(int index = 0; index < 17; index++)
            qDebug() << query.value(index) << " ";
        qDebug() << "\n";
    }
    return true;
}
bool DataBase_thread::deleteById(int id)
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare(QString("delete from Dev where id = %1").arg(id));
    if(!query.exec())
    {
        qDebug() << "Delete Fail";
        return false;
    }
    return true;
}
//根据ID更新记录
bool DataBase_thread::updateById(int id,const DeviceInfo *Dinfo)
{
    QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    /* "create table Dev(id long primary key,Mac varchar[12] ,Time varchar[30],"
                                   "AccX short,AccY short,AccZ short,Tilt_1 float,Tilt_2 float,"
                                   "Tilt_3 short,Distance short,motion short,temperature float,"
                                   "IR short,IUV short,Intensity short, rssi short, voltage float)"*/
    query.prepare(QString("update Dev set Mac=?,Time=?,"
                          "AccX =?, AccY=?,"
                          "AccZ=?, Tilt_1=?,"
                          "Tilt_2=?, Tilt_3=?,"
                          "Distance=? ,motion=?"
                          "temperature=?"
                          "IR=? ,IUV=?"
                          "Intensity=?,rssi=?,voltage=?"
                          "where id=%1").arg(id));
    int DbCount=0;
    query.bindValue(0, DbCount);
    query.bindValue(1, Dinfo->mac);
    query.bindValue(2, Dinfo->time);
    query.bindValue(3, Dinfo->datainfo.accelx);
    query.bindValue(4, Dinfo->datainfo.accely);
    query.bindValue(5, Dinfo->datainfo.accelz);
    query.bindValue(6, Dinfo->datainfo.tilt1);
    query.bindValue(7, Dinfo->datainfo.tilt2);
    query.bindValue(8, Dinfo->datainfo.tilt3);
    query.bindValue(9, Dinfo->datainfo.distance);
    query.bindValue(10, Dinfo->datainfo.motion);
    query.bindValue(11, Dinfo->datainfo.temperature);
    query.bindValue(12, Dinfo->datainfo.IR);
    query.bindValue(13, Dinfo->datainfo.iUV);
    query.bindValue(14, Dinfo->datainfo.intensity);
    query.bindValue(15, Dinfo->rssi);
    query.bindValue(16, Dinfo->voltage);

    bool success=query.exec();
    if(!success)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText() << QString(QObject::tr("更新失败"));
    }
    return true;
}
//排序


bool DataBase_thread::sortById()
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    bool success=query.exec("select * from Dev order by id desc");
    if(success)
    {
        qDebug() << QObject::tr("Sort Success!");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("Sort Failed");
        return false;
    }
}
void DataBase_thread::QueryData(QString mac,int count){
   qDebug()<<"query data";
   QSqlDatabase db = QSqlDatabase::database("DeviceData"); //建立数据库连接
    QSqlQuery query(db);
    if(query.exec("select * from Dev where mac = '"+mac+"'")){
    }
    else{
        QSqlError lastError = query.lastError();
        qDebug() <<"last error  "<< lastError.driverText();
        return;
    }
    int rowcount=1;
    while(query.next())
    {
        QList<QString> *rowData=new QList<QString>;
        for(int index = 0; index <17; index++){
          //  ui->Data_tableWidget->setItem(rowcount,index,new QTableWidgetItem(query.value(index).toString()));
            rowData->push_back(query.value(index).toString());
           // qDebug()<<query.value(index).toString();
        }
        QueryResults->push_back(rowData);
        if(rowcount==count){
            break;
        }
        rowcount++;
    }
    emit QueryReturn(QueryResults);
}

void DataBase_thread::init(QString name){
   qDebug()<<"database initial";
    Db=QSqlDatabase::addDatabase("QSQLITE","DeviceData");
    Db.setHostName("Host");
    Db.setDatabaseName("DeviceDataBase.db");
    Db.setUserName("wck");
    Db.setPassword("1232123");

    createTable();
}
