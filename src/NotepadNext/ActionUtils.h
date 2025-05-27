/*
 * This file is part of Notepad Next.
 * Copyright 2025 Justin Dailey
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

#pragma once

#include <QObject>
#include <QStringList>
#include <QAction>
#include <QApplication>
#include <QStyle>
#include <QPainter>

namespace ActionUtils {

template <typename Container>
void populateActionContainer(Container* container, QObject* context, const QStringList& actionNames) {
    for (const QString& actionName : actionNames) {
        if (actionName.isEmpty()) {
            container->addSeparator();
        } else {
            const QString fullObjectName = QStringLiteral("action") + actionName;
            QAction* action = context->findChild<QAction*>(fullObjectName, Qt::FindDirectChildrenOnly);

            if (action) {
                container->addAction(action);
            } else {
                qWarning() << "Cannot locate action named" << fullObjectName;
            }
        }
    }
}

QIcon createSolidIcon(QColor color)
{
    int iconSize = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);

    QPixmap pixmap(iconSize, iconSize);
    pixmap.fill(color);

    QPainter painter(&pixmap);
    painter.setPen(Qt::black); // 1-pixel black pen by default
    painter.drawRect(0, 0, iconSize - 1, iconSize - 1); // draw border inside the pixmap
    painter.end();

    return QIcon(pixmap);
}

}
