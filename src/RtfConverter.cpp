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


#include "RtfConverter.h"

#include <QTextStream>
#include <QMap>

static void AddCharEscaped(QTextStream &stream, int ch)
{
    switch (ch) {
    case '\n':
        stream << QByteArrayLiteral("\\par") << Qt::endl;
        break;
    case '{':
        stream << QByteArrayLiteral("\\{");
        break;
    case '}':
        stream << QByteArrayLiteral("\\}");
        break;
    case '\t':
        stream << QByteArrayLiteral("\\tab ");
        break;
    default:
        stream << static_cast<char>(ch);
    }
}

static QColor ScintillaColorToQColor(int color)
{
    int r = color & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = (color >> 16) & 0xFF;

    return QColor::fromRgb(r, g, b);
}

void RtfConverter::convertRange(QTextStream &stream, int start, int end)
{
    ensureDocumentStyled(start, end);

    QByteArray rtf_body;
    QTextStream rtf_stream(&rtf_body);
    QVector<QColor> styleColors;
    int prevStyle = -1;

    for (int i = start; i < end; ++i) {
        const int newStyle = editor->styleAt(i);

        if (newStyle != prevStyle) {
            const QColor c = ScintillaColorToQColor(editor->styleFore(newStyle));

            if (!styleColors.contains(c)) {
                styleColors.append(c);
            }

            // Turn bold off or non
            if (prevStyle != -1 && editor->styleBold(prevStyle) && !editor->styleBold(newStyle)) {
                rtf_stream << "\\b0";
            }
            else if (editor->styleBold(newStyle)) {
                rtf_stream << "\\b";
            }

            // Set the foreground color
            rtf_stream << "\\cf" << styleColors.indexOf(c) << ' ';

            prevStyle = newStyle;
        }

        // Output the character
        const int ch = editor->charAt(i);
        if (ch == '\r'){
            if (editor->charAt(i + 1) != '\n') {
                AddCharEscaped(rtf_stream, ch);
            }
        }
        else {
            AddCharEscaped(rtf_stream, ch);
        }
    }
    rtf_stream.flush();

    stream << R"({\rtf1\ansi\deff0\deftab480)" << Qt::endl;
    stream << R"({\fonttbl)" << Qt::endl;
    stream << R"({\f0 )" << editor->styleFont(STYLE_DEFAULT) << ";}" << Qt::endl;
    stream << R"(})" << Qt::endl;
    stream << Qt::endl;
    stream << R"({\colortbl)" << Qt::endl;
    for (const auto &c : styleColors) {
        stream << "\\red" << c.red() <<"\\green" << c.green() << "\\blue" << c.blue() << ";" << Qt::endl;
    }
    stream << R"(})" << Qt::endl;
    stream << Qt::endl;
    stream << "{" << Qt::endl;
    stream << rtf_body << Qt::endl;
    stream << "}" << Qt::endl;
    stream.flush();
}
