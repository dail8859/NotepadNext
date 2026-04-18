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

#include "Sorter.h"

void CaseInsensitiveSorter::sort(QVector<QByteArrayView> &lines) const
{
    std::stable_sort(lines.begin(), lines.end(),
        [dir = direction](const QByteArrayView &a, const QByteArrayView &b) {
            int result = a.compare(b, Qt::CaseInsensitive);
            return dir == Direction::Ascending ? (result < 0) : (result > 0);
        });
}

void CaseSensitiveSorter::sort(QVector<QByteArrayView> &lines) const
{
    std::stable_sort(lines.begin(), lines.end(),
        [dir = direction](const QByteArrayView &a, const QByteArrayView &b) {
            int result = a.compare(b, Qt::CaseSensitive);
            return dir == Direction::Ascending ? (result < 0) : (result > 0);
        });
}

void LineLengthSorter::sort(QVector<QByteArrayView> &lines) const
{
    std::stable_sort(lines.begin(), lines.end(),
        [dir = direction](const QByteArrayView &a, const QByteArrayView &b) {
            return dir == Direction::Ascending ? (a.size() < b.size()) : (a.size() > b.size());
        });
}
