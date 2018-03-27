#include "collectionmodel.h"

CollectionModel::CollectionModel(QObject *parent)
    : QAbstractItemModel(parent), db(nullptr)
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
    if (db && !parent.isValid())
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
    if (db && index.isValid())
    {
        if (role == Qt::DisplayRole)
        {
            if (index.column() == 0)
                return QString::fromStdString(db->label(0, index.row()));
        }
    }

    return QVariant();
}

void CollectionModel::clear()
{
    reset(nullptr);
}

void CollectionModel::reset(marc::Database * p)
{
    beginResetModel();
    db = p;
    endResetModel();
}
