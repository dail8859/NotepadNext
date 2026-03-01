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


#include "RangeAllocator.h"

RangeAllocator::RangeAllocator(int total) :
    resource(total)
{
}

void RangeAllocator::disableRange(int start, int end)
{
    resource.fill(true, start, end + 1);
}

int RangeAllocator::requestResource(const QString name)
{
    // Check to see if this key is allocated yet
    if (allocation.contains(name)) {
        return allocation[name];
    }

    for (int i = 0; i < resource.size(); ++i) {
        if (resource[i] == false) {
            resource[i] = true;
            allocation[name] = i;
            return i;
        }
    }

    Q_ASSERT(false);
    return -1;
}

