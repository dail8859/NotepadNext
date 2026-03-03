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

#include "ComboBoxDelegate.h"

#include <QComboBox>
#include <QApplication>


ComboBoxDelegate::ComboBoxDelegate(const QList<ComboBoxItem> &items, QObject *parent)
    : QStyledItemDelegate(parent), comboBoxItems(items)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QComboBox *comboBox = new QComboBox(parent);

    for(const ComboBoxItem &item : comboBoxItems) {
        comboBox->addItem(item.first, item.second);
    }

    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::EditRole);
    QComboBox *comboBox = static_cast<QComboBox*>(editor);

    comboBox->setCurrentIndex(comboBox->findData(value));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QVariant value = comboBox->currentData();

    model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    editor->setGeometry(option.rect);
}


QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QFontMetrics fm = qApp->fontMetrics();
    QSize maxStringSize(0, 0);
    int &width = maxStringSize.rwidth();
    int &height = maxStringSize.rheight();

    for(const ComboBoxItem &item : comboBoxItems) {
        QRect rect = fm.boundingRect(item.first);

        height = rect.height(); // Heights *should* all be the same
        width = qMax(rect.width(), width);
    }

    QStyleOptionComboBox comobBoxStyleOptions;
    return qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comobBoxStyleOptions, maxStringSize, nullptr);
}
