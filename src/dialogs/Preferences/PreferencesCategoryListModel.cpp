#include "PreferencesCategoryListModel.h"

namespace {
    using ItemPtr = std::unique_ptr<PreferencesCategoryItem>;

    constexpr unsigned int RowHeight = 32;
}

PreferencesCategoryListModel::PreferencesCategoryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    mItems.reserve(5);
}

QVariant PreferencesCategoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mItems.size())
        return {};

    const auto &item = mItems.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return item->title();

    case Qt::DecorationRole:
        return QIcon(item->icon());

    case Qt::SizeHintRole:
        return QSize(0, RowHeight);

    case Qt::TextAlignmentRole:
        return Qt::AlignVCenter;
    }

    return {};
}

void PreferencesCategoryListModel::addCategory(PreferencesCategoryItem *category, int row)
{
    if (row < 0 || row > mItems.size())
        row = mItems.size();

    beginInsertRows(index(row), row, row + 1);
    mItems.insert(mItems.cbegin() + row, std::move(ItemPtr(category)));
    endInsertRows();
}

void PreferencesCategoryListModel::removeCategory(int row)
{
    if (row < 0 || row >= mItems.size())
        return;

    beginRemoveRows(index(row), row, row + 1);
    mItems.erase(mItems.cbegin() + row);
    endRemoveRows();
}

PreferencesCategoryItem *PreferencesCategoryListModel::category(int row) const
{
    if (row < 0 || row >= mItems.size())
        return nullptr;

    return mItems.at(row).get();
}
