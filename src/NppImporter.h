/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
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


#ifndef NPPIMPORTER_H
#define NPPIMPORTER_H

#include <QXmlStreamReader>
#include <QMap>

#include "ScintillaEdit.h"

struct LexerStyle {
    int id;
    int fgColor;
    int bgColor;
    QString fontName;
    int fontStyle;
    int fontSize;
    int keywordClass;
};

struct LexerType {
    QString description;
    QVector<LexerStyle> styles;
};

struct Language {
    QString name;
    QString shortName;
    QString longDescription;
    int lexer_id;
    QStringList extensions;
    LexerType lexer;
    QMap<int, QString> keywords;
};

class NppImporter
{
public:
    NppImporter(const QString &configPath, ScintillaEdit *_editor);

    QMap<QString, Language> languages;
    QStringList sortedLanguageKeys;

private:
    ScintillaEdit *editor;

    void readLangsModel(const QString &filePath);
    void readStylersModel(const QString &filePath);
    void readConfigModel(const QString &filePath);

    void readLexerStyles(QXmlStreamReader &xml);
    void readGlobalStyles(QXmlStreamReader &xml);
    void readGUIConfigs(QXmlStreamReader &xml);

    int foldFgColor;
    int foldBgColor;
    int activeFgColor;
};

#endif // NPPIMPORTER_H
