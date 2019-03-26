#ifndef TREEVIEWMODEL_H
#define TREEVIEWMODEL_H
#include <QStandardItemModel>

class TreeViewModel:public QStandardItemModel
{
public:
    explicit TreeViewModel(QStandardItemModel *parent=0);

    Q_OBJECT
public:

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QStringList mimeTypes() const;
    void ClearAll();
signals:

public slots:

private:
    int m_rowCount;
};

#endif // TREEVIEWMODEL_H
