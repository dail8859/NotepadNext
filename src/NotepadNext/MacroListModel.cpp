/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "MacroListModel.h"

MacroListModel::MacroListModel(QObject *parent, MacroManager *mm) :
    QAbstractListModel(parent),
    macroManager(mm)
{
}

int MacroListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return macroManager->availableMacros().size();
}

QVariant MacroListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

   if (role == Qt::DisplayRole) {
       return macroManager->availableMacros()[index.row()]->getName();
   }

   return QVariant();
}

bool MacroListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    while (count--) {
        delete macroManager->availableMacros().takeAt(row);
    }

    endRemoveRows();

    return true;
}

bool MacroListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r) {
        macroManager->availableMacros().insert(row, new Macro());
    }

    endInsertRows();

    return true;
}

Macro *MacroListModel::macro(const QModelIndex &index)
{
    if (index.isValid())
        return macroManager->availableMacros()[index.row()];
    else
        return Q_NULLPTR;
}
