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

#include <QByteArray>
#include <QByteArrayView>
#include <QList>
#include <QSet>
#include <cstring>
#include <unordered_set>

namespace ByteArrayUtils
{

// Zero-copy split: returns views into 'source'.
// The source QByteArray must remain alive while using the views.
inline QList<QByteArrayView> split(const QByteArray& source, const QByteArray& delimiter)
{
    QList<QByteArrayView> out;

    const char* base = source.constData();
    qsizetype len = source.size();
    qsizetype dlen = delimiter.size();

    qsizetype start = 0;

    while (true) {
        qsizetype pos = source.indexOf(delimiter, start);
        if (pos < 0) {
            out.append(QByteArrayView(base + start, len - start));
            break;
        }

        out.append(QByteArrayView(base + start, pos - start));
        start = pos + dlen;
    }

    return out;
}

// Remove duplicates and preserve order.
inline void removeDuplicates(QList<QByteArrayView>& parts)
{
    std::unordered_set<QByteArray, std::hash<QByteArray>> seen;

    auto newEnd = std::remove_if(parts.begin(), parts.end(), [&seen](const QByteArrayView& v) {
        QByteArray key(v.data(), v.size()); // copy once per unique value
        if (seen.find(key) != seen.end())
            return true; // duplicate, remove
        seen.insert(std::move(key));
        return false;    // first occurrence, keep
    });

    parts.erase(newEnd, parts.end());
}


// Remove only *consecutive* duplicates.
// Example: A A B B B C A  →  A B C A
inline void removeConsecutiveDuplicates(QList<QByteArrayView>& parts)
{
    auto newEnd = std::unique(parts.begin(), parts.end(), [](const QByteArrayView& a, const QByteArrayView& b) {
        return a == b;
    });

    parts.erase(newEnd, parts.end());
}


inline QByteArray join(const QList<QByteArrayView>& parts, const QByteArray& delimiter)
{
    if (parts.isEmpty())
        return QByteArray();

    const qsizetype dlen = delimiter.size();

    // Compute total size
    qsizetype total = 0;
    for (const auto& v : parts)
        total += v.size();
    total += dlen * (parts.size() - 1);

    // Allocate final QByteArray
    QByteArray out;
    out.resize(total);

    // Copy segments
    char* dst = out.data();
    for (int i = 0; i < parts.size(); ++i) {
        const auto& v = parts[i];

        memcpy(dst, v.data(), v.size());
        dst += v.size();

        if (i + 1 < parts.size()) {
            memcpy(dst, delimiter.constData(), dlen);
            dst += dlen;
        }
    }

    return out;
}

} // namespace ByteArrayViewUtils
