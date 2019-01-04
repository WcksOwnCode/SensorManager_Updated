#ifndef DATACONFIGDIALOG_H
#define DATACONFIGDIALOG_H
#include <QStandardItemModel>
#include <QTableView>
#include <QDialog>
#include <QList>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSql>
#include <QSqlError>
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QMessageBox>
#include "globalobject.h"

QT_CHARTS_USE_NAMESPACE;

namespace Ui {
class DataConfigDialog;
}

class DataConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataConfigDialog(QList<DeviceInfo*> *CList,QWidget *parent = nullptr);
    ~DataConfigDialog();
    void CellClicked_(int r, int c);

private slots:
    void on_ConnectDB_pushButton_clicked();

    void on_Input_Query_pushButton_clicked();

    void on_Mac_comboBox_currentIndexChanged(int index);

    void on_Name_comboBox_currentIndexChanged(int index);

    void on_DrawOnchart_pushButton_clicked();

private:
    Ui::DataConfigDialog *ui;
    QStandardItemModel *model;
    QList<DeviceInfo*> *DeviceList;
    QSqlDatabase db;
    QLineSeries *chartSerial=nullptr;
    QChart *chart=nullptr;
    bool data_get=false;
    int focus_row=-1;
    int focus_col=-1;
   QVector< double> focusData;
   QStringList Headerlist;

};

#endif // DATACONFIGDIALOG_H
