#include "LanguageKeywordsModel.h"
#include "ScintillaNext.h"

LanguageKeywordsModel::LanguageKeywordsModel(ScintillaNext *editor, QObject *parent)
    : QAbstractTableModel(parent), editor(editor)
{
}

QVariant LanguageKeywordsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return QStringList{"ID", "Description"}[section];
        }
    }

    return QVariant();
}

int LanguageKeywordsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    const QString keyWordSets = QString(editor->describeKeyWordSets());

    if (keyWordSets.isEmpty())
        return 0;
    else
        return keyWordSets.split('\n').count();
}

int LanguageKeywordsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant LanguageKeywordsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        switch (index.column()) {
            case 0: // ID
                return QString::number(index.row());
            case 1: // Type
                return editor->describeKeyWordSets().split('\n')[index.row()];
            default:
                break;
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}
