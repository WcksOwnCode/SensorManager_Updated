#include "renamedialog.h"
#include "ui_renamedialog.h"

RenameDialog::RenameDialog(QString Name, const QString Mac, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Rename");
    ui->OldName_label->setText(Name);
    this->Mac=Mac;
}

RenameDialog::~RenameDialog()
{
    delete ui;
}

void RenameDialog::on_Accept_pushButton_clicked()
{
    emit NewName(ui->Rename_lineEdit->text(),Mac);
    this->close();
}

void RenameDialog::on_Cancel_pushButton_clicked()
{
    this->close();
}
