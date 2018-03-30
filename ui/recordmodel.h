#ifndef RECORDMODEL_H
#define RECORDMODEL_H

#include <QAbstractItemModel>

#include "collection.h"

#include "tree/tree.h"

class RecordModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum { CODE, DESCRIPTION, VALUE, NUMBER_OF_COLUMNS };

    explicit RecordModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void reset(const marc::Record&);
    void clear();

private:
    struct field_info
    {
        field_info(std::string_view f, std::string_view c, std::string_view d = {})
            : code(QString::fromUtf8(f.data(), f.length())), description(QString::fromUtf8(c.data(), c.length())),
              content(QString::fromUtf8(d.data(),d.length()))
        {}

        QString code;
        QString description;
        QString content;
    };

    static tree< field_info >::key_type to_key(const QModelIndex&);

    tree< field_info > tree_;
};

#endif // RECORDMODEL_H
