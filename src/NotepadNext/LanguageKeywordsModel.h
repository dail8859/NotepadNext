#ifndef LANGUAGEKEYWORDSMODEL_H
#define LANGUAGEKEYWORDSMODEL_H

#include <QAbstractTableModel>

class ScintillaNext;

class LanguageKeywordsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit LanguageKeywordsModel(ScintillaNext *editor, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    ScintillaNext *editor;
};

#endif // LANGUAGEKEYWORDSMODEL_H
