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

#include "LanguageStylesModel.h"
#include "ScintillaNext.h"
#include "ComboBoxDelegate.h"

static inline QRgb convert_color(QRgb c) {
    const int red = qRed(c);
    const int green = qGreen(c);
    const int blue = qBlue(c);

    return qRgb(blue, green, red);
}

QList<ComboBoxItem> caseItems{
    {"SC_CASE_MIXED", SC_CASE_MIXED},
    {"SC_CASE_UPPER", SC_CASE_UPPER},
    {"SC_CASE_LOWER", SC_CASE_LOWER},
    {"SC_CASE_CAMEL", SC_CASE_CAMEL}
};

QString val_to_case_str(int val) {
    for (const ComboBoxItem &item : caseItems) {
        if (item.second == val) {
            return item.first;
        }
    }

    return QString();
}

QStringList columns{
    "ID",
    "Name",
    "Tags",
    "Description",
    "Font",
    "Size",
    "Fractional",
    "Bold",
    "Weight",
    "Italic",
    "Underline",
    "Fore",
    "Back",
    "EOL Filled",
    "Character Set",
    "Case",
    "Visible",
    "Changeable",
    "Hotspot",
};

LanguageStylesModel::LanguageStylesModel(ScintillaNext *editor, QObject *parent)
    : QAbstractTableModel(parent), editor(editor)
{
}

QVariant LanguageStylesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return columns[section];
        }
    }

    return QVariant();
}

int LanguageStylesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return editor->namedStyles();
}

int LanguageStylesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return columns.size();
}

QVariant LanguageStylesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::EditRole) {
        switch (index.column()) {
            case 0: // ID
                return QString::number(index.row());
            case 1: // Name
                return editor->nameOfStyle(index.row());
            case 2: // Tags
                return editor->tagsOfStyle(index.row());
            case 3: // Description
                return editor->descriptionOfStyle(index.row());
            case 4: // Font
                return editor->styleFont(index.row());
            case 5: // Size
                return editor->styleSize(index.row());
            case 6: // Fractional
                return editor->styleSizeFractional(index.row());
            case 7: // Bold
                if (role == Qt::EditRole) return editor->styleBold(index.row());
                else return editor->styleBold(index.row()) ? "True" : "False";
            case 8: // Weight
                return editor->styleWeight(index.row());
            case 9: // Italic
                if (role == Qt::EditRole) return editor->styleItalic(index.row());
                else return editor->styleItalic(index.row()) ? "True" : "False";
            case 10: // Underline
                if (role == Qt::EditRole) return editor->styleUnderline(index.row());
                else return editor->styleUnderline(index.row()) ? "True" : "False";
            case 11: // Fore
                return QColor(convert_color(editor->styleFore(index.row())));
            case 12: // Back
                return QColor(convert_color(editor->styleBack(index.row())));
            case 13: // EOL Filled
                if (role == Qt::EditRole) return editor->styleEOLFilled(index.row());
                else return editor->styleEOLFilled(index.row()) ? "True" : "False";
            case 14: // Character Set
                return editor->styleCharacterSet(index.row());
            case 15: // Case
                if (role == Qt::EditRole) return editor->styleCase(index.row());
                else return val_to_case_str(editor->styleCase(index.row()));
            case 16: // Visible
                if (role == Qt::EditRole) return editor->styleVisible(index.row());
                else return editor->styleVisible(index.row()) ? "True" : "False";
            case 17: // Changeable
                if (role == Qt::EditRole) return editor->styleChangeable(index.row());
                else return editor->styleChangeable(index.row()) ? "True" : "False";
            case 18: // Hotspot
                if (role == Qt::EditRole) return editor->styleHotSpot(index.row());
                else return editor->styleHotSpot(index.row()) ? "True" : "False";
            default:
                break;
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
            case 0:
            case 5:
            case 6:
            case 8:
                return Qt::AlignCenter;
        }
    }
    else if (role == Qt::DecorationRole) {
        if (index.column() == 11) {
            return QColor(convert_color(editor->styleFore(index.row())));
        }
        else if (index.column() == 12) {
            return QColor(convert_color(editor->styleBack(index.row())));
        }
    }

    return QVariant();
}

bool LanguageStylesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        switch (index.column()) {
            case 4: // Font
                editor->styleSetFont(index.row(), value.toString().toLatin1().constData());
                break;
            case 5: // Size
                editor->styleSetSize(index.row(), value.toInt());
                break;
            case 6: // Fractional
                editor->styleSetSizeFractional(index.row(), value.toInt());
                break;
            case 7: // Bold
                editor->styleSetBold(index.row(), value.toBool());
                break;
            case 8: // Weight
                editor->styleSetWeight(index.row(), value.toInt());
                break;
            case 9: // Italic
                editor->styleSetItalic(index.row(), value.toBool());
                break;
            case 10: // Underline
                editor->styleSetUnderline(index.row(), value.toBool());
                break;
            case 11: // Fore
                editor->styleSetFore(index.row(), convert_color(value.value<QColor>().rgb()));
                break;
            case 12: // Back
                editor->styleSetBack(index.row(), convert_color(value.value<QColor>().rgb()));
                break;
            case 13: // EOL Filled
                editor->styleSetEOLFilled(index.row(), value.toBool());
                break;
            case 15: // Case
                editor->styleSetCase(index.row(), value.toInt());
                break;
            case 16: // Visible
                editor->styleSetVisible(index.row(), value.toBool());
                break;
            case 17: // Changeable
                editor->styleSetChangeable(index.row(), value.toBool());
                break;
            case 18: // Hotspot
                editor->styleSetHotSpot(index.row(), value.toBool());
                break;
            default:
                return false;
        }

        editor->colourise(0, -1);

        emit dataChanged(index, index, QVector<int>() << role);

        return true;
    }

    return false;
}

Qt::ItemFlags LanguageStylesModel::flags(const QModelIndex &index) const
{
    const Qt::ItemFlags default_flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (!index.isValid())
        return Qt::NoItemFlags;

    switch (index.column()) {
        case 4: // Font
        case 5: // Size
        case 6: // Fractional
        case 7: // Bold
        case 8: // Weight
        case 9: // Italic
        case 10: // Underline
        case 11: // Fore
        case 12: // Back
        case 13: // EOL Filled
        case 15: // Case
        case 16: // Visible
        case 17: // Changeable
        case 18: // Hotspot
            return default_flags | Qt::ItemIsEditable;
    }

    return default_flags;
}
