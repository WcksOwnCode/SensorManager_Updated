#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class RenameDialog;
}

class RenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameDialog(QString Name,const QString Mac,QWidget *parent = nullptr);
    ~RenameDialog();
signals:
    void NewName(QString Name,const QString Mac);
private slots:
    void on_Accept_pushButton_clicked();

    void on_Cancel_pushButton_clicked();

private:
    Ui::RenameDialog *ui;
    QString OldName;
    QString Mac;
};

#endif // RENAMEDIALOG_H
