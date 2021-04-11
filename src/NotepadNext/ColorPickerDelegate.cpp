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

#include "ColorPickerDelegate.h"

#include <QColorDialog>

ColorPickerDelegate::ColorPickerDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *ColorPickerDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QColorDialog* colorDialog = new QColorDialog(parent);
    colorDialog->setModal(true);

    return colorDialog;
}

void ColorPickerDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QColorDialog *colorDialog = static_cast<QColorDialog*>(editor);
    const QColor color = index.model()->data(index, Qt::DecorationRole).value<QColor>();

    colorDialog->setCurrentColor(color);
}

void ColorPickerDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QColorDialog *colorDialog = static_cast<QColorDialog*>(editor);
    QColor color = colorDialog->selectedColor();

    if (color.isValid()) {
        model->setData(index, color, Qt::EditRole);
    }
}
