#ifndef PORTDIALOG_H
#define PORTDIALOG_H

#include <QDialog>
#include "mainwidget.h"
#include <QTcpServer>
#include <QTcpSocket>
#include "globalobject.h"
namespace Ui {
class PortDialog;
}

class PortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortDialog(QWidget *parent = nullptr);
    ~PortDialog();

private slots:
    void on_Accept_pushButton_clicked();
    void on_Exit_pushButton_clicked();
private:
    Ui::PortDialog *ui;

};

#endif // PORTDIALOG_H

