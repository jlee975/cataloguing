#include "collectionmodel.h"

CollectionModel::CollectionModel(marc::Database* db_, QObject *parent)
    : QAbstractItemModel(parent), db(db_)
{
}

QVariant CollectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return "Record";
        }
    }

    return QVariant();
}

QModelIndex CollectionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column);
    return QModelIndex();
}

QModelIndex CollectionModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int CollectionModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return db->size(0);
    return 0;
}

int CollectionModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;

    return 0;
}

QVariant CollectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return QString::fromStdString(db->label(0, index.row()));
    }

    return QVariant();
}
