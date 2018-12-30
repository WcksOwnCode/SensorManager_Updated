#ifndef ENVIRONMENTCONFIG_H
#define ENVIRONMENTCONFIG_H

#include <QDialog>

namespace Ui {
class EnvironmentConfig;
}

class EnvironmentConfig : public QDialog
{
    Q_OBJECT

public:
    explicit EnvironmentConfig(QString &ConAddr,QString &CsvAddr,
                               QString &DBaddr,bool &label,
                               bool &multithread,bool &Con_ac,bool &Csv,
                               bool &database,QWidget
                               *parent = nullptr);
    ~EnvironmentConfig();

private slots:
    void on_ChangeConfigAddrpushButton_clicked();

    void on_ChangeCsvAddrpushButton_clicked();

    void on_ChangeDBaddrpushButton_clicked();

    void on_ConfigActivatied_checkBox_stateChanged(int arg1);

    void on_CsvActivatied_checkBox_stateChanged(int arg1);

    void on_DataBaseActivatied_checkbox_stateChanged(int arg1);
    void on_Accept_pushButton_clicked();

    void on_Multithread_checkBox_stateChanged(int arg1);

    void on_ShowOnLabel_checkBox_stateChanged(int arg1);

    void on_ChangeIp_pushButton_clicked();

signals:
    void envdone(QString ConAddr, QString CsvAddr,
                 QString DBaddr, bool label, bool multithread, bool Csv,
                 bool database, bool Con_ac);
    void changeIp(QString newip);
private:
    Ui::EnvironmentConfig *ui;
    QString configaddr;
    QString Csv_addr;
    QString DB_addr;

    bool m_bMt;
    bool m_bla;
    bool m_bdba;
    bool m_bcsva;
    bool m_bcfa;
};

#endif // ENVIRONMENTCONFIG_H
