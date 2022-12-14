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


#include "EditorHexViewerTableModel.h"
#include "ScintillaNext.h"


static int IndexToPos(const QModelIndex &index)
{
    return index.row() * 16 + index.column();
}

EditorHexViewerTableModel::EditorHexViewerTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void EditorHexViewerTableModel::setEditor(ScintillaNext *e)
{
    editor = e;
}

QVariant EditorHexViewerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == 16) return QVariant();

            return QString("%1").arg(section, 2, 16, QChar('0')).toUpper();
        }
        else if (orientation == Qt::Vertical) {
            return QString("%1").arg(section * 16, 8, 16, QChar('0')).toUpper();;
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

int EditorHexViewerTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return (editor->length() / 16) + 1;
}

int EditorHexViewerTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 16 + 1;
}

QVariant EditorHexViewerTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        if (index.column() == 16) {
            QString str;

            for (int i = 0; i < 16 ; ++i) {
                int docPos = index.row() * 16 + i;
                if (docPos >= editor->length()) break;

                QChar c = QChar(static_cast<uchar>(editor->charAt(docPos)));
                str += c.isPrint() ? c : '.';
            }

            return str;
        }

        int docPos = IndexToPos(index);;
        if (docPos >= editor->length()) return QVariant();

        unsigned char ch = static_cast<unsigned char>(editor->charAt(docPos));
        return QString("%1").arg(ch, 2, 16, QChar('0')).toUpper();
    }
    else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 16) return Qt::AlignVCenter;
        else return Qt::AlignCenter;
    }

    return QVariant();
}

bool EditorHexViewerTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        bool ok;
        unsigned int charValue = value.toString().toInt(&ok, 16);

        if (ok && charValue <= 255) {
            QByteArray byte(1, static_cast<char>(charValue));
            int pos = IndexToPos(index);

            editor->setTargetRange(pos, pos + 1);
            editor->replaceTarget(1, byte.constData());

            emit dataChanged(index, index, QVector<int>() << role);

            return true;
        }
    }

    return false;
}

Qt::ItemFlags EditorHexViewerTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 16) {
        // The string displayed at the end
        return Qt::ItemIsEnabled;
    }
    else if (IndexToPos(index) >= editor->length()) {
        // The potentially empty cells past the end of the document
        return Qt::NoItemFlags;
    }
    else {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}
