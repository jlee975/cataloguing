#include "recordmodel.h"

RecordModel::RecordModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant RecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case FIELD:
            return "Field";
        case CONTENT:
            return "Content";
        }
    }

    return QVariant();
}

QModelIndex RecordModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column, tree_.child(to_key(parent), row));
}

QModelIndex RecordModel::parent(const QModelIndex &index) const
{
    if (index.isValid())
    {
        const auto j = tree_.parent(index.internalId());
        if (j != tree< derp >::INVALID)
            return createIndex(tree_.row(j), 0, j);
    }
    return QModelIndex();
}

int RecordModel::rowCount(const QModelIndex &parent) const
{
    return tree_.child_count(to_key(parent));
}

int RecordModel::columnCount(const QModelIndex &parent) const
{
    return NUMBER_OF_COLUMNS;
}

QVariant RecordModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const derp& d = tree_.at(index.internalId());

        switch (index.column())
        {
        case FIELD:
            return d.first;
        case CONTENT:
            return d.second;
        }
    }

    return QVariant();
}

void RecordModel::reset(const marc::Record & r_)
{
    tree< derp > tree2;

    for (std::size_t i = 0, n = r_.num_leaders(); i < n; ++i)
    {
        tree2.insert(derp{"Leader", QString::fromStdString(r_.get_leader(i).get_content())});
    }

    for (std::size_t i = 0, n = r_.num_controlfields(); i < n; ++i)
    {
        const auto& c = r_.get_controlfield(i);
        const std::string_view& s = c.get_tag().to_string();
        tree2.insert(derp{QString::fromLatin1(s.data(), s.length()), QString::fromStdString(c.get_content())});
    }

    for (std::size_t i = 0, n = r_.num_datafields(); i < n; ++i)
    {
        const marc::DataField& f = r_.get_datafield(i);
        const std::string_view& t = f.get_tag().to_string();

        const std::string_view& ind1 = to_string(f.get_indicator1());
        const std::string_view& ind2 = to_string(f.get_indicator2());
        const std::size_t j = tree2.insert(derp{QString::fromLatin1(t.data(), t.length()), "Data Field Description"});
        tree2.insert(j, derp{"ind1", QString::fromLatin1(ind1.data(), ind1.length())});
        tree2.insert(j, derp{"ind2", QString::fromLatin1(ind2.data(), ind2.length())});
        for (std::size_t k = 0, m = f.num_subfields(); k < m; ++k)
        {
            const auto& sf = f.get_subfield(k);
            const std::string_view& s = to_string(sf.get_code());
            tree2.insert(j, derp{QString::fromLatin1(s.data(), s.length()), QString::fromStdString(sf.get_content())});
        }
    }

    beginResetModel();
    tree_.swap(tree2);
    endResetModel();
}

void RecordModel::clear()
{
    tree< derp > tree2;

    beginResetModel();
    tree_.swap(tree2);
    endResetModel();
}

tree<RecordModel::derp>::key_type RecordModel::to_key(const QModelIndex & idx)
{
    return idx.isValid() ? idx.internalId() : tree<derp>::INVALID;
}
