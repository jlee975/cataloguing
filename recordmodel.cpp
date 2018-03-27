#include "recordmodel.h"

RecordModel::RecordModel(marc::Record r_, QObject *parent)
    : QAbstractItemModel(parent)
{
    for (std::size_t i = 0; i < r_.num_leaders(); ++i)
    {
        tree_.add_root(derp{"Leader", QString::fromStdString(r_.get_leader(i).get_content())});
    }

    for (std::size_t i = 0; i < r_.num_controlfields(); ++i)
    {
        tree_.add_root(derp{QString::fromStdString(r_.get_controlfield(i).get_tag()), QString::fromStdString(r_.get_controlfield(i).get_content())});
    }

    for (std::size_t i = 0; i < r_.num_datafields(); ++i)
    {
        const marc::DataField& f = r_.get_datafield(i);
        const std::size_t j = tree_.add_root(derp{QString::fromStdString(f.get_tag()), QString::fromStdString(f.get_indicators())});
        for (std::size_t k = 0; k < r_.get_datafield(i).num_subfields(); ++k)
            tree_.add_child(j, derp{QString::fromStdString(f.get_subfield(k).get_code()), QString::fromStdString(f.get_subfield(k).get_content())});
    }
}

QVariant RecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return "Field";
        case 1:
            return "Content";
        }
    }

    return QVariant();
}

QModelIndex RecordModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, tree_.get_root(row));
    return createIndex(row, column, tree_.get_child(parent.internalId(), row));
}

QModelIndex RecordModel::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        const auto j = tree_.parent(index.internalId());
        if (j != -1)
            return createIndex(tree_.row(j), 0, j);
    }
    return QModelIndex();
}

int RecordModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return tree_.number_of_roots();

    return tree_.number_of_children(parent.internalId());
}

int RecordModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant RecordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const derp& d = tree_.value(index.internalId());

        if (index.column() == 0)
            return d.first;
        if (index.column() == 1)
            return d.second;
    }

    return QVariant();
}
