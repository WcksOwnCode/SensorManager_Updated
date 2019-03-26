#pragma execution_character_set("utf-8")
#include "portdialog.h"
#include "ui_portdialog.h"
#include <QMessageBox>
#include <QValidator>
PortDialog::PortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortDialog)
{
    ui->setupUi(this);
    QImage img;
    if(img.load(":/new/logo/rsc/logo.jpg"))
    {
        QImage img2= img.scaled(ui->logo_label->width(),ui->logo_label->height());
        QPixmap qp=QPixmap::fromImage(img2);
        ui->logo_label->setPixmap(qp);
    }
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, QColor(245,255,155));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->setWindowFlags(Qt::FramelessWindowHint);;
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, ui->Port_lineEdit);
    ui->Port_lineEdit->setValidator(validator);
    QRegExp regx2("[0-9]+$");
    QValidator *validator2 = new QRegExpValidator(regx2, ui->Port_lineEdit_2);
    ui->Port_lineEdit_2->setValidator(validator2);
}
PortDialog::~PortDialog()
{
    delete ui;
}
void PortDialog::on_Accept_pushButton_clicked()
{
    QString Portnum=ui->Port_lineEdit->text();
    QString Portnum2=ui->Port_lineEdit_2->text();
    if(Portnum==""&&Portnum2==""){
        QMessageBox::warning(this,"Port Error","check port please!");
        return;
    }
    bool ok1=false;
    bool ok2=false;
    if(Portnum!=""){
        quint16 portNum_16=static_cast<quint16>(Portnum.toInt());
        GlobalObject::Server_=new QTcpServer(this);
        QHostAddress HostAdd("127.1.1.1");
        if(!GlobalObject::Server_->listen(HostAdd,portNum_16)){
            QMessageBox::warning(this,"Warning","Port Num is invalid!");
            GlobalObject::port_=0;
        }
        else{
            ok1=true;
            GlobalObject::port_=portNum_16;
            GlobalObject::Server_->close();
        }
    }
    if(Portnum2!=""){
        quint16 portNum_16=static_cast<quint16>(Portnum2.toInt());
        GlobalObject::Server_2=new QTcpServer(this);
        QHostAddress HostAdd("127.1.2.1");
        if(!GlobalObject::Server_2->listen(HostAdd,portNum_16)){
            QMessageBox::warning(this,"Warning","Port 2 Num is invalid!");
            GlobalObject::port_2=0;
        }
        else{
            ok2=true;
            GlobalObject::port_2=portNum_16;
            GlobalObject::Server_2->close();
        }
    }

    if(ok1|ok2){
        this->accept();
    }
    else{
        QMessageBox::warning(this,"Warning","Check port num");
        return;
    }

}

void PortDialog::on_Exit_pushButton_clicked()
{
    this->close();
}
