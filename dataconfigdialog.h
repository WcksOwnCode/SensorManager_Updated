#ifndef DATACONFIGDIALOG_H
#define DATACONFIGDIALOG_H
#include <QStandardItemModel>
#include <QTableView>
#include <QDialog>
#include <QList>
#include <QTableView>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSql>
#include <QSqlError>
#include <QMessageBox>
#include "globeobject.h"
namespace Ui {
class DataConfigDialog;
}

class DataConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataConfigDialog(QList<DeviceInfo*> *CList,QWidget *parent = nullptr);
    ~DataConfigDialog();


private slots:
    void on_ConnectDB_pushButton_clicked();

    void on_Input_Query_pushButton_clicked();

    void on_Mac_comboBox_currentIndexChanged(int index);

    void on_Name_comboBox_currentIndexChanged(int index);

private:
    Ui::DataConfigDialog *ui;
    QStandardItemModel *model;
    QList<DeviceInfo*> *DeviceList;
    QSqlDatabase db;
};

#endif // DATACONFIGDIALOG_H
