/*
 * This file is part of Notepad Next.
 * Copyright 2021 Justin Dailey
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
