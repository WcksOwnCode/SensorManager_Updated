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
Ŀǰ���������ڽ���ˢ������̫�࣬���½����е㿨   2018-1-4
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
