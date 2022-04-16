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



#include <QFile>
#include <QDir>

#include "NppImporter.h"
#include "SciLexer.h"

#define FONTSTYLE_BOLD 1
#define FONTSTYLE_ITALIC 2
#define FONTSTYLE_UNDERLINE 4

static inline int color(int c) {
    return (c & 0xFF0000) >> 16 | (c & 0x00FF00) | (c & 0x0000FF) << 16;
}

const int LANG_INDEX_INSTR = 0;
const int LANG_INDEX_INSTR2 = 1;
const int LANG_INDEX_TYPE = 2;
const int LANG_INDEX_TYPE2 = 3;
const int LANG_INDEX_TYPE3 = 4;
const int LANG_INDEX_TYPE4 = 5;
const int LANG_INDEX_TYPE5 = 6;

static int keywordClassToIndex(const QStringView keywordClass)
{
    if (keywordClass == QStringLiteral("instre1")) return LANG_INDEX_INSTR;
    if (keywordClass == QStringLiteral("instre2")) return LANG_INDEX_INSTR2;
    if (keywordClass == QStringLiteral("type1")) return LANG_INDEX_TYPE;
    if (keywordClass == QStringLiteral("type2")) return LANG_INDEX_TYPE2;
    if (keywordClass == QStringLiteral("type3")) return LANG_INDEX_TYPE3;
    if (keywordClass == QStringLiteral("type4")) return LANG_INDEX_TYPE4;
    if (keywordClass == QStringLiteral("type5")) return LANG_INDEX_TYPE5;
    return -1; // TODO: not sure what is best
}
struct LanguageName {
    QString shortName;
    QString longName;
    int lexer_id;
};

#define TEXT(x) x
static const QMap<QString, LanguageName> langNamesToLexer = {
    {TEXT("normal"),		{TEXT("Normal text"),		TEXT("Normal text file"),								SCLEX_NULL}},
    {TEXT("php"),			{TEXT("PHP"),				TEXT("PHP Hypertext Preprocessor file"),				SCLEX_HTML}},
    {TEXT("c"),				{TEXT("C"),					TEXT("C source file"),									SCLEX_CPP}},
    {TEXT("cpp"),			{TEXT("C++"),				TEXT("C++ source file"),								SCLEX_CPP}},
    {TEXT("cs"),			{TEXT("C#"),					TEXT("C# source file"),								SCLEX_CPP}},
    {TEXT("objc"),			{TEXT("Objective-C"),		TEXT("Objective-C source file"),						SCLEX_CPP}},
    {TEXT("java"),			{TEXT("Java"),				TEXT("Java source file"),								SCLEX_CPP}},
    {TEXT("rc"),			{TEXT("RC"),					TEXT("Windows Resource file"),						SCLEX_CPP}},
    {TEXT("html"),			{TEXT("HTML"),				TEXT("Hyper Text Markup Language file"),				SCLEX_HTML}},
    {TEXT("xml"),			{TEXT("XML"),				TEXT("eXtensible Markup Language file"),				SCLEX_XML}},
    {TEXT("makefile"),		{TEXT("Makefile"),			TEXT("Makefile"),										SCLEX_MAKEFILE}},
    {TEXT("pascal"),		{TEXT("Pascal"),				TEXT("Pascal source file"),							SCLEX_PASCAL}},
    {TEXT("batch"),			{TEXT("Batch"),				TEXT("Batch file"),										SCLEX_BATCH}},
    {TEXT("ini"),			{TEXT("ini"),				TEXT("MS ini file"),									SCLEX_PROPERTIES}},
    {TEXT("nfo"),			{TEXT("NFO"),				TEXT("MSDOS Style/ASCII Art"),							SCLEX_NULL}},
    //{TEXT("udf"),			{TEXT("udf"),				TEXT("User Define File"),								SCLEX_USER}},
    {TEXT("asp"),			{TEXT("ASP"),				TEXT("Active Server Pages script file"),				SCLEX_HTML}},
    {TEXT("sql"),			{TEXT("SQL"),				TEXT("Structured Query Language file"),					SCLEX_SQL}},
    {TEXT("vb"),			{TEXT("Visual Basic"),		TEXT("Visual Basic file"),								SCLEX_VB}},
    {TEXT("javascript"),	{TEXT("JavaScript"),			TEXT("JavaScript file"),							SCLEX_CPP}},
    {TEXT("css"),			{TEXT("CSS"),				TEXT("Cascade Style Sheets File"),						SCLEX_CSS}},
    {TEXT("perl"),			{TEXT("Perl"),				TEXT("Perl source file"),								SCLEX_PERL}},
    {TEXT("python"),		{TEXT("Python"),				TEXT("Python file"),								SCLEX_PYTHON}},
    {TEXT("lua"),			{TEXT("Lua"),				TEXT("Lua source File"),								SCLEX_LUA}},
    {TEXT("tex"),			{TEXT("TeX"),				TEXT("TeX file"),										SCLEX_TEX}},
    {TEXT("fortran"),		{TEXT("Fortran free form"),	TEXT("Fortran free form source file"),					SCLEX_FORTRAN}},
    {TEXT("bash"),			{TEXT("Shell"),				TEXT("Unix script file"),								SCLEX_BASH}},
    {TEXT("actionscript"),	{TEXT("ActionScript"),		TEXT("Flash ActionScript file"),						SCLEX_CPP}},
    {TEXT("nsis"),			{TEXT("NSIS"),				TEXT("Nullsoft Scriptable Install System script file"),	SCLEX_NSIS}},
    {TEXT("tcl"),			{TEXT("TCL"),				TEXT("Tool Command Language file"),						SCLEX_TCL}},
    {TEXT("lisp"),			{TEXT("Lisp"),				TEXT("List Processing language file"),					SCLEX_LISP}},
    {TEXT("scheme"),		{TEXT("Scheme"),				TEXT("Scheme file"),								SCLEX_LISP}},
    {TEXT("asm"),			{TEXT("Assembly"),			TEXT("Assembly language source file"),					SCLEX_ASM}},
    {TEXT("diff"),			{TEXT("Diff"),				TEXT("Diff file"),										SCLEX_DIFF}},
    {TEXT("props"),			{TEXT("Properties file"),	TEXT("Properties file"),								SCLEX_PROPERTIES}},
    {TEXT("postscript"),	{TEXT("PostScript"),			TEXT("PostScript file"),							SCLEX_PS}},
    {TEXT("ruby"),			{TEXT("Ruby"),				TEXT("Ruby file"),										SCLEX_RUBY}},
    {TEXT("smalltalk"),		{TEXT("Smalltalk"),			TEXT("Smalltalk file"),									SCLEX_SMALLTALK}},
    {TEXT("vhdl"),			{TEXT("VHDL"),				TEXT("VHSIC Hardware Description Language file"),		SCLEX_VHDL}},
    {TEXT("kix"),			{TEXT("KiXtart"),			TEXT("KiXtart file"),									SCLEX_KIX}},
    {TEXT("autoit"),		{TEXT("AutoIt"),				TEXT("AutoIt"),										SCLEX_AU3}},
    {TEXT("caml"),			{TEXT("CAML"),				TEXT("Categorical Abstract Machine Language"),			SCLEX_CAML}},
    {TEXT("ada"),			{TEXT("Ada"),				TEXT("Ada file"),										SCLEX_ADA}},
    {TEXT("verilog"),		{TEXT("Verilog"),			TEXT("Verilog file"),									SCLEX_VERILOG}},
    {TEXT("matlab"),		{TEXT("MATLAB"),				TEXT("MATrix LABoratory"),							SCLEX_MATLAB}},
    {TEXT("haskell"),		{TEXT("Haskell"),			TEXT("Haskell"),										SCLEX_HASKELL}},
    {TEXT("inno"),			{TEXT("Inno Setup"),			TEXT("Inno Setup script"),							SCLEX_INNOSETUP}},
    //{TEXT("searchResult"),	{TEXT("Internal Search"),	TEXT("Internal Search"),								SCLEX_SEARCHRESULT}},
    {TEXT("cmake"),			{TEXT("CMake"),				TEXT("CMake file"),										SCLEX_CMAKE}},
    {TEXT("yaml"),			{TEXT("YAML"),				TEXT("YAML Ain't Markup Language"),						SCLEX_YAML}},
    {TEXT("cobol"),			{TEXT("COBOL"),				TEXT("COmmon Business Oriented Language"),				SCLEX_COBOL}},
    {TEXT("gui4cli"),		{TEXT("Gui4Cli"),			TEXT("Gui4Cli file"),									SCLEX_GUI4CLI}},
    {TEXT("d"),				{TEXT("D"),					TEXT("D programming language"),							SCLEX_D}},
    {TEXT("powershell"),	{TEXT("PowerShell"),			TEXT("Windows PowerShell"),							SCLEX_POWERSHELL}},
    {TEXT("r"),				{TEXT("R"),					TEXT("R programming language"),							SCLEX_R}},
    {TEXT("jsp"),			{TEXT("JSP"),				TEXT("JavaServer Pages script file"),					SCLEX_HTML}},
    {TEXT("coffeescript"),	{TEXT("CoffeeScript"),		TEXT("CoffeeScript file"),								SCLEX_COFFEESCRIPT}},
    {TEXT("json"),			{TEXT("json"),				TEXT("JSON file"),										SCLEX_CPP }},
    {TEXT("javascript.js"), {TEXT("JavaScript"),			TEXT("JavaScript file"),							SCLEX_CPP }},
    {TEXT("fortran77"),		{TEXT("Fortran fixed form"),	TEXT("Fortran fixed form source file"),				SCLEX_F77}},
    {TEXT("baanc"),			{TEXT("BaanC"),				TEXT("BaanC File"),										SCLEX_BAAN }},
    {TEXT("srec"),			{TEXT("S-Record"),			TEXT("Motorola S-Record binary data"),					SCLEX_SREC}},
    {TEXT("ihex"),			{TEXT("Intel HEX"),			TEXT("Intel HEX binary data"),							SCLEX_IHEX}},
    {TEXT("tehex"),			{TEXT("Tektronix extended HEX"),	TEXT("Tektronix extended HEX binary data"),		SCLEX_TEHEX}},
    {TEXT("swift"),			{TEXT("Swift"),              TEXT("Swift file"),									SCLEX_CPP}},
    {TEXT("asn1"),			{TEXT("ASN.1"),				TEXT("Abstract Syntax Notation One file"),				SCLEX_ASN1}},
    {TEXT("avs"),			{TEXT("AviSynth"),			TEXT("AviSynth scripts files"),							SCLEX_AVS}},
    {TEXT("blitzbasic"),	{TEXT("BlitzBasic"),			TEXT("BlitzBasic file"),							SCLEX_BLITZBASIC}},
    {TEXT("purebasic"),		{TEXT("PureBasic"),			TEXT("PureBasic file"),									SCLEX_PUREBASIC}},
    {TEXT("freebasic"),		{TEXT("FreeBasic"),			TEXT("FreeBasic file"),									SCLEX_FREEBASIC}},
    {TEXT("csound"),		{TEXT("Csound"),				TEXT("Csound file"),								SCLEX_CSOUND}},
    {TEXT("erlang"),		{TEXT("Erlang"),				TEXT("Erlang file"),								SCLEX_ERLANG}},
    {TEXT("escript"),		{TEXT("ESCRIPT"),			TEXT("ESCRIPT file"),									SCLEX_ESCRIPT}},
    {TEXT("forth"),			{TEXT("Forth"),				TEXT("Forth file"),										SCLEX_FORTH}},
    {TEXT("latex"),			{TEXT("LaTeX"),				TEXT("LaTeX file"),										SCLEX_LATEX}},
    {TEXT("mmixal"),		{TEXT("MMIXAL"),				TEXT("MMIXAL file"),								SCLEX_MMIXAL}},
    {TEXT("nimrod"),		{TEXT("Nimrod"),				TEXT("Nimrod file"),								SCLEX_NIMROD}},
    {TEXT("nncrontab"),		{TEXT("Nncrontab"),			TEXT("extended crontab file"),							SCLEX_NNCRONTAB}},
    {TEXT("oscript"),		{TEXT("OScript"),			TEXT("OScript source file"),							SCLEX_OSCRIPT}},
    {TEXT("rebol"),			{TEXT("REBOL"),				TEXT("REBOL file"),										SCLEX_REBOL}},
    {TEXT("registry"),		{TEXT("registry"),			TEXT("registry file"),									SCLEX_REGISTRY}},
    {TEXT("rust"),			{TEXT("Rust"),				TEXT("Rust file"),										SCLEX_RUST}},
    {TEXT("spice"),			{TEXT("Spice"),				TEXT("spice file"),										SCLEX_SPICE}},
    {TEXT("txt2tags"),		{TEXT("txt2tags"),			TEXT("txt2tags file"),									SCLEX_TXT2TAGS}},
    {TEXT("ext"),			{TEXT("External"),			TEXT("External"),										SCLEX_NULL}}
};

static const QMap<QString, QVector<int>> folderMarkers = {
    {"type", {SC_MARKNUM_FOLDEROPEN, SC_MARKNUM_FOLDER, SC_MARKNUM_FOLDERSUB, SC_MARKNUM_FOLDERTAIL, SC_MARKNUM_FOLDEREND,        SC_MARKNUM_FOLDEROPENMID,     SC_MARKNUM_FOLDERMIDTAIL}},
    {"simple", {SC_MARK_MINUS,         SC_MARK_PLUS,      SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY}},
    {"arrow", {SC_MARK_ARROWDOWN,     SC_MARK_ARROW,     SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY}},
    {"circle", {SC_MARK_CIRCLEMINUS,   SC_MARK_CIRCLEPLUS,SC_MARK_VLINE,        SC_MARK_LCORNERCURVE,  SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE}},
    {"box", {SC_MARK_BOXMINUS,      SC_MARK_BOXPLUS,   SC_MARK_VLINE,        SC_MARK_LCORNER,       SC_MARK_BOXPLUSCONNECTED,    SC_MARK_BOXMINUSCONNECTED,    SC_MARK_TCORNER}}
};

NppImporter::NppImporter(const QString &configPath, ScintillaEdit *_editor) :
    editor(_editor)
{
    qInfo("Import Notepad++ settings from: \"%s\"", qUtf8Printable(configPath));

    if (!QFile::exists(configPath)) {
        qInfo("import location not found");
        return;
    }

    QDir dir(configPath);

    readLangsModel(dir.filePath("langs.xml"));
    readStylersModel(dir.filePath("stylers.xml"));
    readConfigModel(dir.filePath("config.xml"));

    // Do some cleanup because I have no idea what Notepad++ is doing in some cases
    languages["cpp"].keywords[LANG_INDEX_INSTR2] = languages["cpp"].keywords.take(LANG_INDEX_TYPE);
    languages["cpp"].lexer.styles[16].keywordClass = LANG_INDEX_INSTR2;


    // Build a temporarly list of the key/value pairs
    QList<QPair<QString, const Language *>> sortedLanguages;
    for (const QString &key : languages.keys()) {
        sortedLanguages.append(qMakePair(key, &languages[key]));
    }
    // And sort it based on the values
    std::sort(sortedLanguages.begin(), sortedLanguages.end(), [](const QPair<QString, const Language *> &a, const QPair<QString, const Language *> &b) {
        return QString::compare(a.second->shortName, b.second->shortName, Qt::CaseInsensitive) < 0;
    });

    // Store the sorted keys
    for (const QPair<QString, const Language *> &pair : sortedLanguages) {
        sortedLanguageKeys.append(pair.first);
    }
}

void NppImporter::readLangsModel(const QString &filePath)
{
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);

        xml.readNextStartElement();
        while (xml.readNextStartElement()) {
            if (xml.name() == QStringLiteral("Languages")) {
                //qInfo(qUtf8Printable(xml.name().toString()));

                while (xml.readNextStartElement()) {
                    auto attrs = xml.attributes();
                    if (!langNamesToLexer.contains(attrs.value("name").toString())) {
                        xml.skipCurrentElement();
                        continue;
                    }

                    Language language;
                    language.name = attrs.value("name").toString();
                    language.extensions = attrs.value("ext").toString().split(" ", Qt::SkipEmptyParts);
                    language.shortName = langNamesToLexer[language.name].shortName;
                    language.longDescription = langNamesToLexer[language.name].longName;
                    language.lexer_id = langNamesToLexer[language.name].lexer_id;
                    // TODO: read more
                    while (xml.readNextStartElement()) {
                        //qInfo(qUtf8Printable(language.name));
                        auto attrs = xml.attributes();
                        auto t = xml.readElementText();
                        //qInfo(qUtf8Printable(t));
                        language.keywords[keywordClassToIndex(attrs.value("name"))] = t;
                        //xml.skipCurrentElement();
                    }
                    languages.insert(language.name, language);
                }
            }
            else {
                xml.skipCurrentElement();
            }
        }
        if (xml.hasError()) {
            qWarning("%s", qUtf8Printable(xml.errorString()));
        }
    }
}

void NppImporter::readStylersModel(const QString &filePath)
{
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);

        xml.readNextStartElement();
        while (xml.readNextStartElement()) {
            if (xml.name() == QStringLiteral("LexerStyles")) {
                readLexerStyles(xml);
            }
            else if (xml.name() == QStringLiteral("GlobalStyles")) {
                readGlobalStyles(xml);
            }
            else {
                xml.skipCurrentElement();
            }
        }
        if (xml.hasError()) {
            qWarning("%s", qUtf8Printable(xml.errorString()));
        }
    }
}

void NppImporter::readConfigModel(const QString &filePath)
{
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&file);

        xml.readNextStartElement();
        while (xml.readNextStartElement()) {
            if (xml.name() == QStringLiteral("GUIConfigs")) {
                readGUIConfigs(xml);
            }
            else {
                xml.skipCurrentElement();
            }
        }
        if (xml.hasError()) {
            qWarning("%s", qUtf8Printable(xml.errorString()));
        }
    }
}

void NppImporter::readLexerStyles(QXmlStreamReader &xml)
{
    qInfo("%s", qUtf8Printable(xml.name().toString()));

    while (xml.readNextStartElement()) {
        auto attrs = xml.attributes();
        if (!langNamesToLexer.contains(attrs.value("name").toString())) {
            xml.skipCurrentElement();
            continue;
        }

        LexerType *lexer = &languages[attrs.value("name").toString()].lexer;
        lexer->description = attrs.value("desc").toString();
        while (xml.readNextStartElement()) {
            //qInfo(qUtf8Printable(xml.name().toString()));
            auto attrs = xml.attributes();
            LexerStyle style;
            style.id = attrs.value("styleID").toInt();
            style.fgColor = color(attrs.value("fgColor").toInt(nullptr, 16));
            style.bgColor = color(attrs.value("bgColor").toInt(nullptr, 16));
            if (attrs.hasAttribute("keywordClass")) {
                style.keywordClass = keywordClassToIndex(attrs.value("keywordClass"));
            }
            else {
                style.keywordClass = -1;
            }
            lexer->styles.append(style);
            xml.skipCurrentElement();
        }
    }
}

void NppImporter::readGlobalStyles(QXmlStreamReader &xml)
{
    editor->setMarginWidthN(0, 30);
    editor->styleResetDefault();
    editor->styleClearAll();

    while (xml.readNextStartElement()) {
        //qInfo(qUtf8Printable(xml.tokenString()));
        //qInfo(qUtf8Printable(xml.attributes().value("name").toString()));

        auto attrs = xml.attributes();
        int styleID = attrs.value("styleID").toInt();
        if (styleID >= 32 && styleID <= 39) {
            int fgColor = attrs.value("fgColor").toInt(nullptr, 16);
            int bgColor = attrs.value("bgColor").toInt(nullptr, 16);

            editor->styleSetFore(styleID, color(fgColor));
            editor->styleSetBack(styleID, color(bgColor));

            if (attrs.hasAttribute("fontName") && attrs.value("fontName").length() > 0) {
                editor->styleSetFont(styleID, attrs.value("fontName").toLocal8Bit().constData());
            }
            if (attrs.hasAttribute("fontSize") && attrs.value("fontSize").length() > 0) {
                int fontSize = attrs.value("fontSize").toInt();
                editor->styleSetSize(styleID, fontSize);
            }
            if (attrs.hasAttribute("fontStyle") && attrs.value("fontStyle").length() > 0) {
                int fontStyle = attrs.value("fontStyle").toInt();
                editor->styleSetBold(styleID, fontStyle & FONTSTYLE_BOLD);
                editor->styleSetItalic(styleID, fontStyle & FONTSTYLE_ITALIC);
                editor->styleSetUnderline(styleID, fontStyle & FONTSTYLE_UNDERLINE);
            }
            if (styleID == STYLE_DEFAULT) {
                editor->styleClearAll();
            }
        }
        else if (styleID > 0) {
            editor->indicSetFore(styleID, color(attrs.value("bgColor").toInt(nullptr, 16)));
        }
        else if (attrs.value("name") == QStringLiteral("Current line background colour")) {
            editor->setCaretLineBack(color(attrs.value("bgColor").toInt(nullptr, 16)));
        }
        else if (attrs.value("name") == QStringLiteral("Selected text colour")) {
            editor->setSelBack(true, color(attrs.value("bgColor").toInt(nullptr, 16)));
        }
        else if (attrs.value("name") == QStringLiteral("Caret colour")) {
            editor->setCaretFore(color(attrs.value("fgColor").toInt(nullptr, 16)));
        }
        else if (attrs.value("name") == QStringLiteral("Edge colour")) {
            editor->setEdgeColour(color(attrs.value("fgColor").toInt(nullptr, 16)));
        }
        else if (attrs.value("name") == QStringLiteral("White space symbol")) {
            editor->setWhitespaceFore(true, color(attrs.value("fgColor").toInt(nullptr, 16)));
        }
        else if (attrs.value("name") == QStringLiteral("Fold")) {
            // NOTE: Yes this is a bug in Notepad++ swapping the fg and bg colors
            foldFgColor = color(attrs.value("bgColor").toInt(nullptr, 16));
            foldBgColor = color(attrs.value("fgColor").toInt(nullptr, 16));
        }
        else if (attrs.value("name") == QStringLiteral("Fold active")) {
            activeFgColor = color(attrs.value("fgColor").toInt(nullptr, 16));
        }
        else if (attrs.value("name") == QStringLiteral("Fold margin")) {
            editor->setFoldMarginColour(true, color(attrs.value("fgColor").toInt(nullptr, 16)));
            editor->setFoldMarginHiColour(true, color(attrs.value("bgColor").toInt(nullptr, 16)));
        }
        else {
            //qWarning("Unhandled WidgetStyle \"%s\"", qUtf8Printable(xml.attributes().value("name").toString()));
        }
        xml.skipCurrentElement();
    }
}

void NppImporter::readGUIConfigs(QXmlStreamReader &xml)
{
    while (xml.readNextStartElement()) {
        auto attrs = xml.attributes();
        if (attrs.value("name") == QStringLiteral("ScintillaPrimaryView")) {
            if (attrs.value("indentGuideLine") == QStringLiteral("show")) {
                editor->setIndentationGuides(SC_IV_LOOKBOTH);
            }
            if (attrs.value("currentLineHilitingShow") == QStringLiteral("show")) {
                editor->setCaretLineVisible(true);
                editor->setCaretLineVisibleAlways(true);
            }
            if (attrs.value("Wrap") == QStringLiteral("yes")) {
                editor->setWrapMode(SC_WRAP_WHITESPACE);
            }
            if (attrs.value("edge") == QStringLiteral("line")) {
                editor->setEdgeMode(EDGE_LINE);
                editor->setEdgeColumn(attrs.value("edgeNbColumn").toInt());
            }
            if (attrs.value("whiteSpaceShow") == QStringLiteral("show")) {
                editor->setViewWS(SCWS_VISIBLEALWAYS);
            }
            if (attrs.value("eolShow") == QStringLiteral("show")) {
                editor->setViewEOL(true);
            }
            if (attrs.hasAttribute("folderMarkStyle")) {
                editor->setAutomaticFold(SC_AUTOMATICFOLD_SHOW | SC_AUTOMATICFOLD_CLICK | SC_AUTOMATICFOLD_CHANGE);
                editor->markerEnableHighlight(true);
                const auto markers = folderMarkers[attrs.value("folderMarkStyle").toString()];
                const auto types = folderMarkers["type"];
                for (int i = 0; i < types.size(); ++i) {
                    editor->markerDefine(types[i], markers[i]);
                    editor->markerSetFore(types[i], foldFgColor);
                    editor->markerSetBack(types[i], foldBgColor);
                    editor->markerSetBackSelected(types[i], activeFgColor);
                }
            }
        }
        xml.skipCurrentElement();
    }
}
