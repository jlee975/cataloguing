#include "recordmodel.h"

#include "definitions.h"

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
        case CODE:
            return "Code";
        case DESCRIPTION:
            return "Description";
        case VALUE:
            return "Value";
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
        if (j != tree< field_info >::INVALID)
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
        const field_info& d = tree_.at(index.internalId());

        switch (index.column())
        {
        case CODE:
            return d.code;
        case DESCRIPTION:
            return d.description;
        case VALUE:
            return d.content;
        }
    }

    return QVariant();
}

void RecordModel::reset(const marc::Record & r_)
{
    tree< field_info > tree2;
    const auto root = tree< field_info >::INVALID;

    for (std::size_t i = 0, n = r_.num_leaders(); i < n; ++i)
    {
        tree2.emplace(root, "Leader", std::string_view(), r_.get_leader(i).get_content());
    }

    for (std::size_t i = 0, n = r_.num_controlfields(); i < n; ++i)
    {
        const marc::ControlField& c = r_.get_controlfield(i);
        tree2.emplace(root, c.get_tag().to_string(), std::string_view(), c.get_content());
    }

    for (std::size_t i = 0, n = r_.num_datafields(); i < n; ++i)
    {
        const marc::DataField& f = r_.get_datafield(i);

        const auto& t = f.get_tag().to_string();
        const auto& descriptor = marc::get_field_descriptor(t);

        const std::size_t j = tree2.emplace(root, t, descriptor.description);
        tree2.emplace(j, "ind1", descriptor.first_indicator.description,
                      descriptor.first_indicator.friendly(f.get_indicator1()));
        tree2.emplace(j, "ind2", descriptor.second_indicator.description,
                      descriptor.second_indicator.friendly(f.get_indicator2()));
        for (std::size_t k = 0, m = f.num_subfields(); k < m; ++k)
        {
            const marc::SubField& sf = f.get_subfield(k);
            tree2.emplace(j, to_string(sf.get_code()), std::string_view(), sf.get_content());
        }
    }

    beginResetModel();
    tree_ = std::move(tree2);
    endResetModel();
}

void RecordModel::clear()
{
    beginResetModel();
    tree_.clear();
    endResetModel();
}

tree<RecordModel::field_info>::key_type RecordModel::to_key(const QModelIndex & idx)
{
    return idx.isValid() ? idx.internalId() : tree<field_info>::INVALID;
}
