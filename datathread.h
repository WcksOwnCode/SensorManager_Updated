#ifndef DATATHREAD_H
#define DATATHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "globalobject.h"
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QValueAxis>
//#include "mainwidget.h"
class DataThread:public QObject
{
    Q_OBJECT
public:
    explicit DataThread( QObject *parent = 0);
    void DataProcess(QString info, int p);
    int infoSpliter(QString info, QStringList &returnstr, int px);

    QStringList StringFileter(QString info,int );
    inline bool isRunning(){
        return m_bisRunning;
    }
    QString HexTimeToDec(QByteArray t);
    void SetRunning(bool tr);
    void DataProcess_Simple(QByteArray fore, QByteArray back,DeviceInfo *dev,int index);
    inline bool isBusy(){
        return m_bBusy;
    }
    QString HexToDec(QByteArray arr);
    QString HexTilteToDec(QByteArray arr);
signals:
    void dataproDone(QString datainfo);
    void workDone(DeviceInfo *dev,int index);
    void ToInqueryProc(QStringList &strList,int px=0);
private:
    bool m_bisRunning=false;
    bool m_bBusy=false;
};

#endif // DATATHREAD_H
