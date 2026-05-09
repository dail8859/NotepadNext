/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


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
