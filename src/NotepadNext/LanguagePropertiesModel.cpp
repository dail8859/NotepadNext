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

#include "LanguagePropertiesModel.h"
#include "ScintillaNext.h"

QString property_type_to_string(int type) {
    switch (type) {
    case SC_TYPE_BOOLEAN:
        return "Bool";
    case SC_TYPE_INTEGER:
        return "Int";
    case SC_TYPE_STRING:
        return "String";
    default:
        return "unknown";
    }
}

LanguagePropertiesModel::LanguagePropertiesModel(ScintillaNext *editor, QObject *parent)
    : QAbstractTableModel(parent), editor(editor)
{
}

QVariant LanguagePropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return QStringList{"Property", "Type", "Description", "Value"}[section];
        }
    }

    return QVariant();
}

int LanguagePropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    const QString propertyNames = editor->propertyNames();

    if (propertyNames.isEmpty())
        return 0;
    else
        return propertyNames.split('\n').count();
}

int LanguagePropertiesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}

QVariant LanguagePropertiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
        const QStringList properties = QString(editor->propertyNames()).split('\n');
        const QString property = properties[index.row()];

        switch (index.column()) {
            case 0: // Name
                return property;
            case 1: // Type
                return property_type_to_string(editor->propertyType(property.toLatin1().constData()));
            case 2: // Description
                return editor->describeProperty(property.toLatin1().constData());
            case 3: // Value
                return editor->property(property.toLatin1().constData());
            default:
                break;
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 3) {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

bool LanguagePropertiesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const QStringList properties = QString(editor->propertyNames()).split('\n');
        const QString property = properties[index.row()];

        editor->setProperty(property.toLatin1().constBegin(), value.toString().toLatin1().constBegin());

        editor->colourise(0, -1);

        emit dataChanged(index, index, QVector<int>() << role);

        return true;
    }

    return false;
}

Qt::ItemFlags LanguagePropertiesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() != 3)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}
