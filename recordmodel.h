#ifndef RECORDMODEL_H
#define RECORDMODEL_H

#include <QAbstractItemModel>

#include "marc/collection.h"

#include "tree.h"

class RecordModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit RecordModel(marc::Record, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    struct derp
    {
        QString first;
        QString second;
    };

    tree< derp > tree_;
};

#endif // RECORDMODEL_H
