#ifndef SENSORCONFIG_DIALOG_H
#define SENSORCONFIG_DIALOG_H

#include <QDialog>
#include "globalobject.h"
#include <QMessageBox>
#include <QComboBox>
#include <QMutex>
#include <QException>

namespace Ui {
class SensorConfig_Dialog;
}

class SensorConfig_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit SensorConfig_Dialog(QList<DeviceInfo*> DL, QWidget *parent = nullptr);
    ~SensorConfig_Dialog();
     void SetCorrEnabled(bool b);
     void SetValueFromDevice();
     void SetAllControlEnabled(bool b);
     void LoadCurrentControlMethod();
     void setAllControlToDefault();

private slots:
    void on_Mac_comboBox_currentIndexChanged(int index);

    void on_Name_comboBox_currentIndexChanged(int index);

    void on_EditEnablecheckBox_stateChanged(int arg1);

    void on_SetValue_pushButton_clicked();

    void on_Activated_1_checkBox_2_stateChanged(int arg1);

    void on_Activated_2_checkBox_2_stateChanged(int arg1);

    void on_Activated_3_checkBox_2_stateChanged(int arg1);

    void on_Activated_4_checkBox_stateChanged(int arg1);

    void on_Activated_5_checkBox_3_stateChanged(int arg1);

    void on_Logic_comboBox1_currentIndexChanged(int index);

    void on_Logic_comboBox2_currentIndexChanged(int index);

    void on_Logic_comboBox3_currentIndexChanged(int index);

    void on_Logic_comboBox4_currentIndexChanged(int index);

    void on_Logic_comboBox5_currentIndexChanged(int index);

    void on_Accept_pushButton_clicked();

    void on_ClearAllControlMethod_pushButton_clicked();

    void on_DataType_comboBox_currentIndexChanged(int index);

    void on_LoadMethod_pushButton_clicked();

    void on_ClearAllCorrection_pushButton_clicked();

signals:
    void Synchronization(QList<DeviceInfo *>DL);

private:
    Ui::SensorConfig_Dialog *ui;
    QList<DeviceInfo*> DL;
    QVector<QCheckBox*> ck;
    QVector<QComboBox*> Symbos;
    QVector<QComboBox*> LogicBox;
    QVector<QLineEdit*> LineEditBox;
    QVector<QComboBox*> CommandBox;
    QVector<QComboBox*> PortBox;
    bool m_buiready=false;
    bool m_bMethodChanged=false;
};

#endif // SENSORCONFIG_DIALOG_H
