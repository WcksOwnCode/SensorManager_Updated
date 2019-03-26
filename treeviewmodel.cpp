#pragma execution_character_set("utf-8")
#include "treeviewmodel.h"
#include <QDebug>
#include <QMimeData>
#include <QModelIndexList>

TreeViewModel::TreeViewModel(QStandardItemModel *parent)
    : QStandardItemModel(parent)
{
    m_rowCount=0;
}

Qt::ItemFlags TreeViewModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags = flags | Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;
    return flags;
}

QMimeData *TreeViewModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() <= 0)
        return 0;
    QMimeData *data = new QMimeData;
    data->setData("drag","鼠标拖拽");
    data->setData("name",item(indexes.at(0).row(),0)->text().toLocal8Bit());
    data->setData("score",item(indexes.at(0).row(),1)->text().toLocal8Bit());
    return data;
}

bool TreeViewModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    QString st=QString::fromLocal8Bit(data->data("name"));
    QString st1=QString::fromLocal8Bit(data->data("score"));
    this->setItem(m_rowCount,0,new QStandardItem(st));
    this->setItem(m_rowCount,1,new QStandardItem(st1));

    m_rowCount++;
    return true;
}

QStringList TreeViewModel::mimeTypes() const
{
    QStringList types;
    types << "drag";
    return types;
}
void TreeViewModel::ClearAll(){

        this->clear();

}
