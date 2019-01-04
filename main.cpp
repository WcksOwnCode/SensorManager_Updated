#include "mainwidget.h"
#include <QApplication>
#include "portdialog.h"
#include "globalobject.h"
#include <QDebug>
#include <QMessageBox>
#include <QSql>
#include <QSplashScreen>
#include <QTimer>
/*
目前的问题在于界面刷新数据太多，导致界面有点卡   2018-1-4
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PortDialog pd;
    if(QDialog::Accepted==pd.exec()){
        MainWidget w;
        QPixmap pixmap(":/new/logo/rsc/resize.bmp");

        QSplashScreen ss(pixmap);
        ss.show();
        QEventLoop s;
        QTimer::singleShot(1200,&s,SLOT(quit()));
        s.exec();

        ss.hide();
         w.show();
        return a.exec();
    }
    else {
        return 0;
    }





}
