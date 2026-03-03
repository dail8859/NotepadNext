/*
 * This file is part of Notepad Next.
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright 2024 Justin Dailey
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

#include "FadingIndicator.h"

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTimer>


namespace Internal {

class FadingIndicatorPrivate : public QWidget
{
    Q_OBJECT

public:
    FadingIndicatorPrivate(QWidget *parent, FadingIndicator::TextSize size)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        m_effect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(m_effect);
        m_effect->setOpacity(.999);

        m_label = new QLabel;
        QFont font = m_label->font();
        font.setPixelSize(size == FadingIndicator::LargeText ? 30 : 18);
        m_label->setFont(font);
        QPalette pal = palette();
        pal.setColor(QPalette::WindowText, pal.color(QPalette::Window));
        m_label->setPalette(pal);
        auto layout = new QHBoxLayout;
        setLayout(layout);
        layout->addWidget(m_label);
    }

    void setText(const QString &text)
    {
        m_pixmap = QPixmap();
        m_label->setText(text);
        m_effect->setOpacity(.6); // because of the fat opaque background color
        layout()->setSizeConstraint(QLayout::SetFixedSize);
        adjustSize();
        QWidget *parent = parentWidget();
        QPoint pos = parent ? (parent->rect().center() - rect().center()) : QPoint();
        if (pixmapIndicator && pixmapIndicator->geometry().intersects(QRect(pos, size())))
            pos.setY(pixmapIndicator->geometry().bottom() + 1);
        move(pos);
    }

    void setPixmap(const QString &uri)
    {
        m_label->hide();
        m_pixmap.load(uri);
        layout()->setSizeConstraint(QLayout::SetNoConstraint);
        resize(m_pixmap.size() / m_pixmap.devicePixelRatio());
        QWidget *parent = parentWidget();
        QPoint pos = parent ? (parent->rect().center() - rect().center()) : QPoint();
        if (textIndicator && textIndicator->geometry().intersects(QRect(pos, size())))
            pos.setY(textIndicator->geometry().bottom() + 1);
        move(pos);
    }

    void run(int ms)
    {
        show();
        raise();
        QTimer::singleShot(ms, this, &FadingIndicatorPrivate::runInternal);
    }

    static QPointer<FadingIndicatorPrivate> textIndicator;
    static QPointer<FadingIndicatorPrivate> pixmapIndicator;

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (!m_pixmap.isNull()) {
            p.drawPixmap(rect(), m_pixmap);
        } else {
            p.setBrush(palette().color(QPalette::WindowText));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(rect(), 15, 15);
        }
    }

private:
    void runInternal()
    {
        QPropertyAnimation *anim = new QPropertyAnimation(m_effect, "opacity", this);
        anim->setDuration(200);
        anim->setEndValue(0.);
        connect(anim, &QAbstractAnimation::finished, this, &QObject::deleteLater);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QGraphicsOpacityEffect *m_effect;
    QLabel *m_label;
    QPixmap m_pixmap;
};

QPointer<FadingIndicatorPrivate> FadingIndicatorPrivate::textIndicator;
QPointer<FadingIndicatorPrivate> FadingIndicatorPrivate::pixmapIndicator;

} // Internal


namespace FadingIndicator {

void showText(QWidget *parent, const QString &text, TextSize size)
{
    QPointer<Internal::FadingIndicatorPrivate> &indicator = Internal::FadingIndicatorPrivate::textIndicator;

    if (indicator)
        delete indicator;

    indicator = new Internal::FadingIndicatorPrivate(parent, size);
    indicator->setText(text);
    indicator->run(2500); // deletes itself
}

void showPixmap(QWidget *parent, const QString &pixmap)
{
    QPointer<Internal::FadingIndicatorPrivate> &indicator = Internal::FadingIndicatorPrivate::pixmapIndicator;

    if (indicator)
        delete indicator;

    indicator = new Internal::FadingIndicatorPrivate(parent, LargeText);
    indicator->setPixmap(pixmap);
    indicator->run(300); // deletes itself
}

}

#include "FadingIndicator.moc"
