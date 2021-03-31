#ifndef LANGUAGEPROPERTIESMODEL_H
#define LANGUAGEPROPERTIESMODEL_H

#include <QAbstractTableModel>

class ScintillaNext;

class LanguagePropertiesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit LanguagePropertiesModel(ScintillaNext *editor, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    ScintillaNext *editor;
};

#endif // LANGUAGEPROPERTIESMODEL_H
