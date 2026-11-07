/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
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


#ifndef ENCODINGSPECS_H
#define ENCODINGSPECS_H

#include <QString>


// Single source of truth for the Encoding menu and for normalizing the
// charset names reported by uchardet. A "-" entry is a menu separator and the
// list ends with nullptr. UTF-16/UTF-32 without a BOM and stateful encodings
// such as ISO-2022-JP are deliberately absent: the document BOM model only
// understands None / Utf8 / Utf16LE / Utf16BE.
namespace EncodingSpecs {

inline const char *const *menu()
{
    static const char *const specs[] = {
        "UTF-8", "UTF-8 BOM", "UTF-16LE BOM", "UTF-16BE BOM",
        "-",
        "GB18030", "GBK", "GB2312", "Big5", "Big5-HKSCS", "Big5-ETen",
        "-",
        "Shift-JIS", "EUC-JP", "EUC-KR", "CP949",
        "-",
        "windows-1250", "windows-1251", "windows-1252", "windows-1253", "windows-1254",
        "windows-1255", "windows-1256", "windows-1257", "windows-1258",
        "-",
        "ISO 8859-1", "ISO 8859-2", "ISO 8859-3", "ISO 8859-4", "ISO 8859-5",
        "ISO 8859-6", "ISO 8859-7", "ISO 8859-8", "ISO 8859-9", "ISO 8859-10",
        "ISO 8859-13", "ISO 8859-14", "ISO 8859-15", "ISO 8859-16",
        "-",
        "MacRoman", "IBM 850", "IBM 866", "IBM 874",
        "KOI8-R", "KOI8-U", "KOI8-RU", "TIS-620",
        nullptr
    };

    return specs;
}


// Mirrors Qt's qTextCodecNameMatch(): only letters and digits are significant.
// Thus "Shift-JIS", "Shift_JIS" and "ISO 8859-1" / "ISO-8859-1" compare equal.
inline QString key(const QString &name)
{
    QString result;
    result.reserve(name.size());

    for (const QChar ch : name) {
        if (ch.isLetterOrNumber())
            result.append(ch.toLower());
    }

    return result;
}


// Return the menu spelling for a detected charset, so the status bar and the
// checked menu item always agree with what was detected on disk. An empty
// result means "uchardet reported a charset we cannot open"; the caller must
// then fall back to UTF-8 instead of handing an unknown name to QTextCodec.
inline QString canonical(const QString &name)
{
    if (name.isEmpty())
        return name;

    const QString normalized = key(name);
    for (const char *const *it = menu(); *it; ++it) {
        const QString spec = QString::fromLatin1(*it);

        if (spec == QLatin1String("-"))
            continue;
        if (key(spec) == normalized)
            return spec;
    }

    // uchardet spells a few code pages differently from Qt. These keys are
    // already normalized by key(), e.g. "x-mac-roman" -> "xmacroman".
    struct Alias { const char *from; const char *to; };
    static const Alias aliases[] = {
        { "macintosh",  "MacRoman" },
        { "xmacroman",  "MacRoman" },
        { "windows874", "IBM 874" },   // Thai
        { "cp874",      "IBM 874" },
        { "iso885911",  "TIS-620" },
        { "uhc",        "CP949" },     // Korean Unified Hangul Code
        { "cp932",      "Shift-JIS" }, // Qt has no separate CP932 codec
        { "windows31j", "Shift-JIS" },
    };
    for (const Alias &alias : aliases) {
        if (normalized == QLatin1String(alias.from))
            return QString::fromLatin1(alias.to);
    }

    // HZ-GB-2312, EUC-TW, ISO-2022-JP/KR, VISCII, Johab ... are stateful or
    // simply not available as QTextCodecs. Passing them through would make
    // readFromDisk() take its "No conversion necessary" branch and feed raw
    // legacy bytes to Scintilla as if they were UTF-8.
    return QString();
}

} // namespace EncodingSpecs

#endif // ENCODINGSPECS_H
