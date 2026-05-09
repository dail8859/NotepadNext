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


#ifndef PREFERENCESCATEGORYITEMT_H
#define PREFERENCESCATEGORYITEMT_H

#include "PreferencesCategoryItem.h"

template<typename T>
class PreferencesCategoryItemT : public PreferencesCategoryItem
{
public:
    PreferencesCategoryItemT(const QString &title, const QString &icon)
        : PreferencesCategoryItem(), mTitle(title), mIcon(icon)
    { }
    virtual ~PreferencesCategoryItemT() = default;

    inline virtual QString title() const override { return mTitle; }
    inline virtual QString iconPath() const override { return mIcon; }
    inline virtual T *contentView(ApplicationSettings *settings) const override {
        return new T(settings);
    }

private:
    QString mTitle;
    QString mIcon;
};

#endif // PREFERENCESCATEGORYITEMT_H
