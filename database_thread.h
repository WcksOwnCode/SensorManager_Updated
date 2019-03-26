#ifndef DATABASE_THREAD_H
#define DATABASE_THREAD_H

#include <QObject>
#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QList>
#include "globalobject.h"
class DataBase_thread:public QObject
{
    Q_OBJECT

public:
    DataBase_thread(QObject *parent=nullptr);
    ~DataBase_thread();
    bool sortById();
    bool updateById(int id,const DeviceInfo *Dinfo);
    bool deleteById(int id);
    bool queryAll();
    bool insert(const DeviceInfo *Dinfo);
    bool createTable(const DeviceInfo *Dinfo);
    bool createTable();
    QSqlDatabase Db;
    QSqlQuery query;
    QVariantList idList;
    void ConnectToDB();
    QSqlDatabase db;
    void QueryData(QString mac,int count);

    QList< QList<QString>*>*QueryResults;


    void init(QString name);
signals:
    void QueryReturn(QList<QList<QString>*>*);

};

#endif // DATABASE_THREAD_H
