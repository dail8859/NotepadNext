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


#include "LuaConsoleDock.h"
#include "ui_LuaConsoleDock.h"

#include "ScintillaNext.h"
#include "ILexer.h"
#include "Lexilla.h"
#include "SciLexer.h"

#include "LuaState.h"

#include "lua.hpp"

#include "LuaExtension.h"

#include <QKeyEvent>
#include <QVBoxLayout>

#define INDIC_BRACEHIGHLIGHT INDIC_CONTAINER

static bool inline isBrace(int ch) {
    return strchr("[]{}()", ch) != NULL;
}

inline static void raise_error(lua_State *L, const char *errMsg=NULL) {
    luaL_where(L, 1);
    if (errMsg) {
        lua_pushstring(L, errMsg);
    } else {
        lua_insert(L, -2);
    }
    lua_concat(L, 2);
    lua_error(L);
}

static int cf_global_print(lua_State *L) {
    LuaConsoleDock *dock = static_cast<LuaConsoleDock *>(lua_touserdata(L, lua_upvalueindex(1)));
    int nargs = lua_gettop(L);

    lua_getglobal(L, "tostring");

    for (int i = 1; i <= nargs; ++i) {
        if (i > 1)
            dock->writeToOutput("\t");

        const char *argStr = lua_tostring(L, i);
        if (argStr) {
            dock->writeToOutput(argStr);
        } else {
            lua_pushvalue(L, -1); // tostring
            lua_pushvalue(L, i);
            lua_call(L, 1, 1);
            argStr = lua_tostring(L, -1);
            if (argStr) {
                dock->writeToOutput(argStr);
            } else {
                raise_error(L, "tostring (called from print) returned a non-string");
            }
            lua_settop(L, nargs + 1);
        }
    }

    dock->writeToOutput("\r\n");
    return 0;
}


LuaConsoleDock::LuaConsoleDock(LuaState *l, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LuaConsoleDock)
{
    L = l;

    // Override print to print to the console
    lua_pushlightuserdata(L->L, this);
    lua_pushcclosure(L->L, cf_global_print, 1);
    lua_setglobal(L->L, "print");

    ui->setupUi(this);

    output = new ScintillaNext(Q_NULLPTR, this);

    QFrame *line;
    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    input = new ScintillaNext(Q_NULLPTR, this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(output);
    layout->addWidget(line);
    layout->addWidget(input);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QWidget *widget = new QWidget(this);
    widget->setLayout(layout);
    setWidget(widget);

    output->setUndoCollection(false);
    output->setReadOnly(true);
    output->setILexer(reinterpret_cast<sptr_t>(CreateLexer("lua")));
    output->usePopUp(SC_POPUP_NEVER);
    output->setMarginWidthN(1, output->textWidth(STYLE_DEFAULT, ">") * 2);
    output->setMarginTypeN(1, SC_MARGIN_RTEXT);
    output->styleSetBold(STYLE_LINENUMBER, true);
    output->setScrollWidth(1);

    // Have it actually do the lexing
    input->setILexer(reinterpret_cast<sptr_t>(CreateLexer("lua")));

    // Setup Keywords
    input->setKeyWords(0, "and break do else elseif end false for function goto if in local nil not or repeat return then true until while");
    input->setKeyWords(1, "_ENV _G _VERSION assert collectgarbage dofile error getfenv getmetatable ipairs load loadfile loadstring module next pairs pcall print rawequal rawget rawlen rawset require select setfenv setmetatable tonumber tostring type unpack xpcall string table math bit32 coroutine io os debug package __index __newindex __call __add __sub __mul __div __mod __pow __unm __concat __len __eq __lt __le __gc __mode");
    input->setKeyWords(2, "byte char dump find format gmatch gsub len lower rep reverse sub upper abs acos asin atan atan2 ceil cos cosh deg exp floor fmod frexp ldexp log log10 max min modf pow rad random randomseed sin sinh sqrt tan tanh arshift band bnot bor btest bxor extract lrotate lshift replace rrotate rshift shift string.byte string.char string.dump string.find string.format string.gmatch string.gsub string.len string.lower string.match string.rep string.reverse string.sub string.upper table.concat table.insert table.maxn table.pack table.remove table.sort table.unpack math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.cosh math.deg math.exp math.floor math.fmod math.frexp math.huge math.ldexp math.log math.log10 math.max math.min math.modf math.pi math.pow math.rad math.random math.randomseed math.sin math.sinh math.sqrt math.tan math.tanh bit32.arshift bit32.band bit32.bnot bit32.bor bit32.btest bit32.bxor bit32.extract bit32.lrotate bit32.lshift bit32.replace bit32.rrotate bit32.rshift");
    input->setKeyWords(3, "close flush lines read seek setvbuf write clock date difftime execute exit getenv remove rename setlocale time tmpname coroutine.create coroutine.resume coroutine.running coroutine.status coroutine.wrap coroutine.yield io.close io.flush io.input io.lines io.open io.output io.popen io.read io.tmpfile io.type io.write io.stderr io.stdin io.stdout os.clock os.date os.difftime os.execute os.exit os.getenv os.remove os.rename os.setlocale os.time os.tmpname debug.debug debug.getfenv debug.gethook debug.getinfo debug.getlocal debug.getmetatable debug.getregistry debug.getupvalue debug.getuservalue debug.setfenv debug.sethook debug.setlocal debug.setmetatable debug.setupvalue debug.setuservalue debug.traceback debug.upvalueid debug.upvaluejoin package.cpath package.loaded package.loaders package.loadlib package.path package.preload package.seeall");

    // Highlight constants
    //input->setKeyWords(4, join(SciIFaceTable.GetAllConstantNames(), ' ').c_str());
    //input->setKeyWords(5, join(NppIFaceTable.GetAllConstantNames(), ' ').c_str());

    // Set up some autocomplete junk
    input->autoCSetIgnoreCase(true);
    input->autoCSetMaxHeight(8);
    input->autoCSetCancelAtStart(false);

    input->indicSetStyle(INDIC_BRACEHIGHLIGHT, INDIC_STRAIGHTBOX);
    input->indicSetUnder(INDIC_BRACEHIGHLIGHT, true);
    input->braceHighlightIndicator(true, INDIC_BRACEHIGHLIGHT);

    input->setIndentationGuides(SC_IV_LOOKBOTH);

    input->setHScrollBar(false);
    input->setMarginWidthN(1, input->textWidth(STYLE_DEFAULT, ">") * 2);
    input->setMarginTypeN(1, SC_MARGIN_RTEXT);
    input->styleSetBold(STYLE_LINENUMBER, true);
    input->setMarginLeft(2);

    input->clearCmdKey('Q' + (SCMOD_CTRL << 16));
    input->clearCmdKey('W' + (SCMOD_CTRL << 16));
    input->clearCmdKey('E' + (SCMOD_CTRL << 16));
    input->clearCmdKey('R' + (SCMOD_CTRL << 16));
    input->clearCmdKey('T' + (SCMOD_CTRL << 16));
    input->clearCmdKey('O' + (SCMOD_CTRL << 16));
    input->clearCmdKey('P' + (SCMOD_CTRL << 16));

    input->clearCmdKey('S' + (SCMOD_CTRL << 16));
    input->clearCmdKey('D' + (SCMOD_CTRL << 16));
    input->clearCmdKey('F' + (SCMOD_CTRL << 16));
    input->clearCmdKey('G' + (SCMOD_CTRL << 16));
    input->clearCmdKey('H' + (SCMOD_CTRL << 16));
    input->clearCmdKey('J' + (SCMOD_CTRL << 16));
    input->clearCmdKey('K' + (SCMOD_CTRL << 16));
    input->clearCmdKey('L' + (SCMOD_CTRL << 16));
    input->clearCmdKey('B' + (SCMOD_CTRL << 16));
    input->clearCmdKey('N' + (SCMOD_CTRL << 16));
    input->clearCmdKey('M' + (SCMOD_CTRL << 16));

    input->clearCmdKey(SCK_RETURN); // don't allow normal new lines

    input->marginSetText(0, ">");
    input->marginSetStyle(0, STYLE_LINENUMBER);

    setupStyle(input);
    setupStyle(output);

    output->styleSetFore(39, 0x0000FF); // For error messages

    input->setExtraAscent(2);
    input->setExtraDescent(2);
    input->setMaximumHeight(input->textHeight(0));
    input->installEventFilter(this);

    connect(input, &ScintillaNext::updateUi, [=](Scintilla::Update flags) {
        Q_UNUSED(flags);
        int curPos = input->currentPos();
        int bracePos = INVALID_POSITION;

        // Check on both sides
        if (isBrace(input->charAt(curPos - 1))) {
            bracePos = curPos - 1;
        }
        else if (isBrace(input->charAt(curPos))) {
            bracePos = curPos;
        }

        // See if we are next to a brace
        if (bracePos != INVALID_POSITION) {
            int otherPos = input->braceMatch(bracePos, 0);
            if (otherPos != INVALID_POSITION) {
                input->braceHighlight(bracePos, otherPos);
            }
            else {
                input->braceHighlight(INVALID_POSITION, INVALID_POSITION);
            }
        }
        else {
            input->braceHighlight(INVALID_POSITION, INVALID_POSITION);
        }
    });
}

LuaConsoleDock::~LuaConsoleDock()
{
    delete ui;
}

void LuaConsoleDock::writeToOutput(const char *s)
{
    output->setReadOnly(false);
    output->appendText(strlen(s), s);
    output->setReadOnly(true);
    output->documentEnd();
}

void LuaConsoleDock::writeErrorToOutput(const char *s)
{
    typedef struct {
        unsigned char c;
        unsigned char style;
    } cell;

    const int length = static_cast<int>(strlen(s));
    QVector<cell> cells(length + 1);

    for (auto i = 0; i < length; ++i) {
        cells[i].c = s[i];
        cells[i].style = 39;
    }

    cells[length].c = 0;
    cells[length].style = 0;

    output->setReadOnly(false);
    output->documentEnd();
    output->addStyledText(length * 2, (const char *) cells.constData());
    output->setReadOnly(true);
    output->documentEnd();
}

void LuaConsoleDock::historyNext()
{
    if (history.isEmpty()) return;

    if (currentHistoryIndex < history.size() - 1) {
        currentHistoryIndex++;
        input->setText(history[currentHistoryIndex].toLatin1().constData());
    }

    input->documentEnd();
    input->emptyUndoBuffer();
    input->scrollToEnd();
}

void LuaConsoleDock::historyPrevious()
{
    if (currentHistoryIndex == 0) return;

    currentHistoryIndex--;

    input->setText(history[currentHistoryIndex].toLatin1().constData());
    input->documentEnd();
    input->emptyUndoBuffer();
    input->scrollToEnd();
}

void LuaConsoleDock::historyAdd(QString line)
{
    if (!line.isEmpty()) {
        if (history.isEmpty() || history.last() != line) {
            history.append(line);
        }
    }

    currentHistoryIndex = history.size();
}

void LuaConsoleDock::historyEnd()
{
    currentHistoryIndex = history.size();
    input->emptyUndoBuffer(); // Empty first in case it was a mistake
    input->setText("");
}

void LuaConsoleDock::runCurrentCommand()
{
    int prevLastLine = output->lineCount();
    int newLastLine = 0;

    Sci_TextRange tr;
    tr.chrg.cpMin = 0;
    tr.chrg.cpMax = input->length();
    tr.lpstrText = new char[2 * (tr.chrg.cpMax - tr.chrg.cpMin) + 2]; // See documentation
    input->send(SCI_GETSTYLEDTEXT, 0, (sptr_t) &tr);

    output->documentEnd();
    output->setReadOnly(false);
    output->addStyledText(2 * (tr.chrg.cpMax - tr.chrg.cpMin), tr.lpstrText);

    writeToOutput("\r\n");

    delete[] tr.lpstrText;

    newLastLine = output->lineCount();

    for (int i = prevLastLine; i < newLastLine; ++i) {
        output->marginSetText(i - 1, ">");
        output->marginSetStyle(i - 1, STYLE_LINENUMBER);
    }

    QString text((const char *)input->characterPointer());
    historyAdd(text);

    input->clearAll();
    input->emptyUndoBuffer();
    input->marginSetText(0, ">");
    input->marginSetStyle(0, STYLE_LINENUMBER);

    LuaExtension::Instance().OnExecute(text.toLatin1().constData());
}

bool LuaConsoleDock::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::NoModifier) {
            runCurrentCommand();
            return true;
        }

        if (keyEvent->key() == Qt::Key_Up)
        {
            if (input->lineCount() == 1 || input->currentPos() == input->length()) {
                historyPrevious();
                return true;
            }
        }

        if (keyEvent->key() == Qt::Key_Down) {
            if (input->lineCount() == 1 || input->currentPos() == 0) {
                historyNext();
                return true;
            }
        }

        if (keyEvent->key() == Qt::Key_Escape) {
            historyEnd();
            return true;
        }
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }

    return false;
}

void LuaConsoleDock::setupStyle(ScintillaNext *editor)
{
    editor->setEOLMode(SC_EOL_CRLF);

    editor->styleSetFore(STYLE_DEFAULT, 0x000000);
    editor->styleSetBack(STYLE_DEFAULT, 0xFFFFFF);
    editor->styleSetFont(STYLE_DEFAULT, "Courier New");
    editor->styleSetSize(STYLE_DEFAULT, 10);
    editor->styleClearAll();

    // Setup the margins
    editor->setMarginWidthN(0, 0);
    editor->setMarginWidthN(2, 0);
    editor->setMarginWidthN(3, 0);
    editor->setMarginWidthN(4, 0);

    editor->setCodePage(SC_CP_UTF8);
    editor->styleSetFore(SCE_LUA_COMMENT, 0x008000);
    editor->styleSetFore(SCE_LUA_COMMENTLINE, 0x008000);
    editor->styleSetFore(SCE_LUA_COMMENTDOC, 0x808000);
    editor->styleSetFore(SCE_LUA_LITERALSTRING, 0x4A0095);
    editor->styleSetFore(SCE_LUA_PREPROCESSOR, 0x004080); // Technically not used since this is lua 5+
    editor->styleSetFore(SCE_LUA_WORD, 0xFF0000);
    editor->styleSetBold(SCE_LUA_WORD, 1); // for SCI_SETKEYWORDS, 0
    editor->styleSetFore(SCE_LUA_NUMBER, 0x0080FF);
    editor->styleSetFore(SCE_LUA_STRING, 0x808080);
    editor->styleSetFore(SCE_LUA_CHARACTER, 0x808080);
    editor->styleSetFore(SCE_LUA_OPERATOR, 0x800000);
    editor->styleSetBold(SCE_LUA_OPERATOR, 1);
    editor->styleSetFore(SCE_LUA_WORD2, 0xC08000);
    editor->styleSetBold(SCE_LUA_WORD2, 1); // for SCI_SETKEYWORDS, 1
    editor->styleSetFore(SCE_LUA_WORD3, 0xFF0080);
    editor->styleSetBold(SCE_LUA_WORD3, 1); // for SCI_SETKEYWORDS, 2
    editor->styleSetFore(SCE_LUA_WORD4, 0xA00000);
    editor->styleSetBold(SCE_LUA_WORD4, 1);
    editor->styleSetItalic(SCE_LUA_WORD4, 1); // for SCI_SETKEYWORDS, 3
    editor->styleSetFore(SCE_LUA_LABEL, 0x008080);
    editor->styleSetBold(SCE_LUA_LABEL, 1);
    editor->styleSetFore(SCE_LUA_WORD5, 0x004080); // for SCI_SETKEYWORDS, 4, Scintilla defines
    editor->styleSetBold(SCE_LUA_WORD5, 1);
    editor->styleSetFore(SCE_LUA_WORD6, 0x004080); // for SCI_SETKEYWORDS, 5, Notepad++ defines
    editor->styleSetBold(SCE_LUA_WORD6, 1);
}
