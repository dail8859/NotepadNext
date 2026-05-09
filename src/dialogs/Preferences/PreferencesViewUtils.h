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


#ifndef PREFERENCESVIEWUTILS_H
#define PREFERENCESVIEWUTILS_H

#include <QFileSystemModel>
#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QEvent>

#include "ApplicationSettings.h"

#define PREFERENCES_BIND_PROPERTY(name, Name) \
    &ApplicationSettings::name, \
    &ApplicationSettings::set##Name, \
    &ApplicationSettings::name##Changed

namespace Preferences
{
    inline const QMargins MARGINS_0;
    inline const QMargins MARGINS_6(6, 6, 6, 6);

    inline const int SPACING_0(0);
    inline const int SPACING_3(3);
    inline const int SPACING_6(6);

    class WheelEventSuppressFilter : public QObject
    {
    public:
        explicit WheelEventSuppressFilter(QObject *parent = nullptr)
            : QObject(parent) { }

        inline void setupFor(QWidget *widget)
        {
            widget->installEventFilter(this);
            widget->setFocusPolicy(Qt::ClickFocus);
        }

        inline virtual bool eventFilter(QObject *obj, QEvent *event) override
        {
            if (event->type() == QEvent::Wheel)
            {
                const auto widget = qobject_cast<QWidget*>(obj);
                if (widget && !widget->hasFocus())
                {
                    event->ignore();
                    return true;
                }
            }

            return QObject::eventFilter(obj, event);
        }
    };

    class NotificationLabel : public QWidget
    {
    public:
        NotificationLabel(const QString &text, QWidget *parent = nullptr)
            : QWidget(parent)
        {
            const auto iconSize = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);

            const auto icon = new QLabel;
            icon->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(iconSize, iconSize));

            label = new QLabel(text);
            label->setWordWrap(true);

            auto infoFont = label->font();
            infoFont.setPointSize(infoFont.pointSize() - 1);
            infoFont.setItalic(true);
            label->setFont(infoFont);

            const auto layout = new QHBoxLayout(this);
            layout->addWidget(icon, 0, Qt::AlignTop);
            layout->addWidget(label, 1);
            layout->setContentsMargins(9, 0, 3, 6);
            layout->setSpacing(SPACING_3);
        }

        inline const QLabel *getLabel() const {
            return label;
        }

    private:
        QLabel *label = nullptr;
    };

    template<typename G, typename S, typename N>
    inline QCheckBox *CreateCheckBox(const QString &title,
                                     ApplicationSettings *settings,
                                     G getter, S setter, N notifier,
                                     const QString &toolTip = QString())
    {
        const auto checkBox = new QCheckBox(title);

        if (!toolTip.isEmpty())
            checkBox->setToolTip(toolTip);

        checkBox->setChecked(std::bind(getter, settings)());

        QObject::connect(settings, notifier, checkBox, &QCheckBox::setChecked);
        QObject::connect(checkBox, &QCheckBox::toggled, settings, setter);

        return checkBox;
    }
}

#endif // PREFERENCESVIEWUTILS_H
