#include "PreferencesCategoryListModel.h"

namespace
{
    using ItemPtr = std::unique_ptr<PreferencesCategoryItem>;

    constexpr unsigned int RowHeight = 32;
}

PreferencesCategoryListModel::PreferencesCategoryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    items.reserve(5);
}

QVariant PreferencesCategoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= items.size())
        return {};

    const auto &item = items.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole:
            return item->title();
        case Qt::DecorationRole:
            return QIcon(item->iconPath());
        case Qt::SizeHintRole:
            return QSize(0, RowHeight);
        case Qt::TextAlignmentRole:
            return Qt::AlignVCenter;
    }

    return {};
}

void PreferencesCategoryListModel::addCategory(PreferencesCategoryItem *category, int row)
{
    if (row < 0 || row > items.size())
        row = items.size();

    beginInsertRows(index(row), row, row + 1);
    items.insert(items.cbegin() + row, std::move(ItemPtr(category)));
    endInsertRows();
}

void PreferencesCategoryListModel::removeCategory(int row)
{
    if (row < 0 || row >= items.size())
        return;

    beginRemoveRows(index(row), row, row + 1);
    items.erase(items.cbegin() + row);
    endRemoveRows();
}

PreferencesCategoryItem *PreferencesCategoryListModel::category(int row) const
{
    if (row < 0 || row >= items.size())
        return nullptr;

    return items.at(row).get();
}
