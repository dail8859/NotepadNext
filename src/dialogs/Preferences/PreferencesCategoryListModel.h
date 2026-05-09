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


#ifndef PREFERENCESCATEGORYLISTMODEL_H
#define PREFERENCESCATEGORYLISTMODEL_H

#include <QAbstractListModel>
#include <QPointer>

#include "PreferencesCategoryItem.h"

class PreferencesCategoryListModel : public QAbstractListModel
{
public:
    explicit PreferencesCategoryListModel(QObject *parent = nullptr);
    virtual ~PreferencesCategoryListModel() = default;

    inline virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return items.size();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @param item Specific item.
     * @param pos Item position. -1 will append to the end.
     * @note takes ownership of the item.
     */
    void addCategory(PreferencesCategoryItem *category, int row = -1);
    void removeCategory(int row);
    PreferencesCategoryItem *category(int row) const;

private:
    std::vector<std::unique_ptr<PreferencesCategoryItem>> items;
};

#endif // PREFERENCESCATEGORYLISTMODEL_H
