#ifndef DATATHREAD_H
#define DATATHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "globeobject.h"
//#include "mainwidget.h"
class DataThread:public QObject
{
    Q_OBJECT
public:
    explicit DataThread( QObject *parent = 0);
    void DataProcess(QString data);
    inline bool isRunning(){
        return m_bisRunning;
    }
    QString HexTimeToDec(QByteArray t);
    void SetRunning(bool tr);
    void DataProcess_Simple(QByteArray fore, QByteArray back,DeviceInfo *dev,int index);
    inline bool isBusy(){
        return m_bBusy;
    }
signals:
    void dataproDone(QString datainfo);
    void workDone(DeviceInfo *dev,int index);
private:
    bool m_bisRunning=false;
    bool m_bBusy=false;
};

#endif // DATATHREAD_H
