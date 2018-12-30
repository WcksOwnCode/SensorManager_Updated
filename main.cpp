#include "mainwidget.h"
#include <QApplication>
#include "portdialog.h"
#include "globeobject.h"
#include <QDebug>
#include <QMessageBox>
#include <QSql>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PortDialog pd;
    if(QDialog::Accepted==pd.exec()){
        MainWidget w;
         w.show();
        return a.exec();
    }
    else {
        return 0;
    }





}
