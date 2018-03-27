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
        const std::string_view& s = r_.get_controlfield(i).get_tag();
        tree_.add_root(derp{QString::fromLatin1(s.data(), s.length()), QString::fromStdString(r_.get_controlfield(i).get_content())});
    }

    for (std::size_t i = 0; i < r_.num_datafields(); ++i)
    {
        const marc::DataField& f = r_.get_datafield(i);
        const std::string_view& t = f.get_tag();

        const std::string_view& ind1 = to_string(f.get_indicator1());
        const std::string_view& ind2 = to_string(f.get_indicator2());
        const std::size_t j = tree_.add_root(derp{QString::fromLatin1(t.data(), t.length()), "Data Field Description"});
        tree_.add_child(j, derp{"ind1", QString::fromLatin1(ind1.data(), ind1.length())});
        tree_.add_child(j, derp{"ind2", QString::fromLatin1(ind2.data(), ind2.length())});
        for (std::size_t k = 0; k < r_.get_datafield(i).num_subfields(); ++k)
        {
            const std::string_view& s = to_string(f.get_subfield(k).get_code());
            tree_.add_child(j, derp{QString::fromLatin1(s.data(), s.length()), QString::fromStdString(f.get_subfield(k).get_content())});
        }
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
