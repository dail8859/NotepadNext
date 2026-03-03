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


#include "HtmlConverter.h"

#include <QSet>
#include <QTextStream>


static QByteArray ScintillaColorToCssColor(int color)
{
    int r = color & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = (color >> 16) & 0xFF;

    return QStringLiteral("rgb(%1, %2, %3)").arg(r).arg(g).arg(b).toLatin1();
}

static void StyleToCss(QTextStream &stream, ScintillaNext *editor, int style)
{
    stream << ".s" << style << " {" << Qt::endl;
    stream << "  font-family: '" << editor->styleFont(style) << "';" << Qt::endl;
    stream << "  font-size: " << editor->styleSize(style) << "pt;" << Qt::endl;
    stream << "  color: " << ScintillaColorToCssColor(editor->styleFore(style)) << ";" << Qt::endl;
    stream << "  background: " << ScintillaColorToCssColor(editor->styleBack(style)) << ";" << Qt::endl;

    if (editor->styleBold(style))
        stream << "  font-weight: bold;" << Qt::endl;

    if (editor->styleItalic(style))
        stream << "  font-style: italic;" << Qt::endl;

    if (editor->styleUnderline(style))
        stream << "  text-decoration: underline;" << Qt::endl;

    stream << "}" << Qt::endl;
}

static void AddCharEscaped(QTextStream &stream, int ch)
{
    switch (ch) {
    case '<':
        stream << QByteArrayLiteral("&lt;");
        break;
    case '>':
        stream << QByteArrayLiteral("&gt;");
        break;
    case '&':
        stream << QByteArrayLiteral("&amp;");
        break;
    default:
        stream << static_cast<char>(ch);
    }
}

void HtmlConverter::convertRange(QTextStream &stream, int start, int end)
{
    ensureDocumentStyled(start, end);

    // Create the raw html and keep track of the used styles
    QByteArray html;
    QTextStream html_stream(&html);
    QSet<int> usedStyles;
    int currentStyle = -1;

    for (int i = start; i < end; ++i) {
        int style = editor->styleAt(i);

        if (style != currentStyle) {
            currentStyle = style;
            usedStyles.insert(currentStyle);

            html_stream << "</span><span class=\"s" << currentStyle << "\">";
        }

        const int ch = editor->charAt(i);
        if (ch == '\r'){
            if (editor->charAt(i + 1) != '\n') {
                AddCharEscaped(html_stream, ch);
            }
        }
        else {
            AddCharEscaped(html_stream, ch);
        }
    }
    html_stream.flush();


    // Generate the CSS for just the styles that were used
    QByteArray css;
    QTextStream css_stream(&css);
    for (const int style : usedStyles) {
        StyleToCss(css_stream, editor, style);
    }
    css_stream.flush();


    // Generate the entire document
    stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/1999/REC-html401-19991224/strict.dtd\">" << Qt::endl;
    stream << "<html>" << Qt::endl;
    stream << "<head>" << Qt::endl;
    stream << "<meta http-equiv=Content-Type content=\"text/html; charset=UTF-8\">" << Qt::endl;
    stream << "<title>Exported from Notepad Next</title>" << Qt::endl;
    stream << "<!--StartFragment-->" << Qt::endl;
    stream << "<style type=\"text/css\">" << Qt::endl;
    stream << "div.main {" << Qt::endl;
    stream << "  white-space: pre;" << Qt::endl;
    stream << "  line-height: 1;" << Qt::endl;
    stream << "}" << Qt::endl;
    stream << css;
    stream << "</style>" << Qt::endl;
    stream << "</head>" << Qt::endl;
    stream << "<body>" << Qt::endl;
    stream << "<div class=\"main\">";
    stream << html;
    stream << "</div>" << Qt::endl;
    stream << "</body>" << Qt::endl;
    stream << "<!--EndFragment-->" << Qt::endl;
    stream << "</html>" << Qt::endl;
    stream.flush();
}
