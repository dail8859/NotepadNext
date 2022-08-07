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


#include "MacroStepTableModel.h"


MacroStepTableModel::MacroStepTableModel(Macro *m, QObject *parent) :
    QAbstractTableModel(parent),
    macro(m)
{
}

QVariant MacroStepTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return QStringList{"Name", "Data"}[section];
        }
    }

    return QVariant();
}

int MacroStepTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return macro->getSteps().size();
}

int MacroStepTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return 2;
}

QVariant MacroStepTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        switch (index.column()) {
            case 0:
                return macro->getSteps()[index.row()].name();
            case 1:
                return macro->getSteps()[index.row()].str;
            default:
                break;
        }
    }

    return QVariant();
}

bool MacroStepTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() == 1) {
            macro->getSteps()[index.row()].str = value.toString().toUtf8();

            emit dataChanged(index, index, QVector<int>() << role);

            return true;
        }
    }

    return false;
}

bool MacroStepTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r) {
        macro->getSteps().insert(row, MacroStep(Scintilla::Message::CharLeftExtend, 0, 0));
    }

    endInsertRows();

    return true;
}

bool MacroStepTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    macro->getSteps().remove(row, count);

    endRemoveRows();

    return true;
}

bool MacroStepTableModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    // Source code for this has been copied from  QStringListModel::moveRows()

    if (sourceRow < 0
            || sourceRow + count - 1 >= rowCount(sourceParent)
            || destinationChild <= 0
            || destinationChild > rowCount(destinationParent)
            || sourceRow == destinationChild - 1
            || count <= 0) {
            return false;
        }

    if (!beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild))
        return false;
    /*
    QList::move assumes that the second argument is the index where the item will end up to
    i.e. the valid range for that argument is from 0 to QList::size()-1
    QAbstractItemModel::moveRows when source and destinations have the same parent assumes that
    the item will end up being in the row BEFORE the one indicated by destinationChild
    i.e. the valid range for that argument is from 1 to QList::size()
    For this reason we remove 1 from destinationChild when using it inside QList
    */

    destinationChild--;

    const int fromRow = destinationChild < sourceRow ? (sourceRow + count - 1) : sourceRow;
    while (count--)
        macro->getSteps().move(fromRow, destinationChild);

    endMoveRows();

    return true;
}

Qt::ItemFlags MacroStepTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    //if (index.column() == 0)
    //    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

    if (index.column() == 1 && MacroStep::MessageHasString(macro->getSteps()[index.row()].message))
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

