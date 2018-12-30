#include "portdialog.h"
#include "ui_portdialog.h"
#include <QMessageBox>
PortDialog::PortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortDialog)
{
    ui->setupUi(this);


}
PortDialog::~PortDialog()
{
    delete ui;
}


void PortDialog::on_Accept_pushButton_clicked()
{
    QString Portnum=ui->Port_lineEdit->text();
    if(Portnum==""){
        QMessageBox::warning(this,"Port Error","check port please!");
        return;
    }
    quint16 portNum_16=static_cast<quint16>(Portnum.toInt());
    GlobeObject::Server_=new QTcpServer(this);
    QHostAddress HostAdd("127.1.1.1");
    if(!GlobeObject::Server_->listen(HostAdd,portNum_16)){
      QMessageBox::warning(this,"Warning","Port Num is invalid!");
      GlobeObject::port_=0;
      return ;
    }
    else{
        GlobeObject::port_=portNum_16;
        GlobeObject::Server_->close();
        //on_buttonBox_accepted();
        this->accept();
    }
}
