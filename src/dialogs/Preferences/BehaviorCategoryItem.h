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


#ifndef BEHAVIORCATEGORYITEM_H
#define BEHAVIORCATEGORYITEM_H

#include "PreferencesCategoryItem.h"

class BehaviorCategoryItem : public PreferencesCategoryItem
{
public:
    BehaviorCategoryItem() = default;
    virtual ~BehaviorCategoryItem() = default;

    virtual QString title() const override { return QObject::tr("Behavior"); }
    virtual QString iconPath() const override { return "://icons/audio-waveform.svg"; }
    virtual QWidget *contentView(ApplicationSettings *settings) const override;
};

#endif // BEHAVIORCATEGORYITEM_H
