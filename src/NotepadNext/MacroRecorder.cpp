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


#include "MacroRecorder.h"
#include "ScintillaNext.h"

using namespace Scintilla;

inline bool MessageHasString(Message message) {
    return message == Message::ReplaceSel ||
            message == Message::InsertText ||
            message == Message::AddText ||
            message == Message::AppendText;
}


class MacroAction {
public:
    MacroAction(Message message, uptr_t wParam,  sptr_t lParam);
    ~MacroAction();

    QString toString() const;

    Message message;
    uptr_t wParam;
    union {
        sptr_t lParam;
        QByteArray *str;
    };
};

MacroAction::MacroAction(Message message, uptr_t wParam, sptr_t lParam) :
    message(message),
    wParam(wParam)
{
    if (message == Message::ReplaceSel || message == Message::InsertText) {
        // wParam is 0 for replace, and position for insert
        this->str = new QByteArray(reinterpret_cast<const char*>(lParam));
    }
    else if (message == Message::AddText || message == Message::AppendText) {
        // wParam is length of text
        this->str = new QByteArray(reinterpret_cast<const char*>(lParam), wParam);
    }
    else {
        this->lParam = lParam;
    }
}

MacroAction::~MacroAction()
{
    if (MessageHasString(message))
        delete str;
}

QString MacroAction::toString() const
{
    if (MessageHasString(message)) {
        return QString("MacroAction(%1, %2, \"%3\")").arg(static_cast<int>(message))
                .arg(wParam)
                .arg(str->constData());
    }
    else {
        return QString("MacroAction(%1, %2, %3)").arg(static_cast<int>(message))
                .arg(wParam)
                .arg(lParam);
    }
}

Macro::Macro() : name("<Current Recorded Macro>") {}

Macro::~Macro()
{
    while(actions.size() > 0)
        delete actions.takeLast();
}

void Macro::addMacroStep(Message message, uptr_t wParam, sptr_t lParam)
{
    qInfo(Q_FUNC_INFO);

    // Combine ReplaceSel messages into a single string
    if (message == Message::ReplaceSel && !actions.empty() && actions.constLast()->message == Message::ReplaceSel) {
        actions.last()->str->append(reinterpret_cast<const char*>(lParam));
    }
    // Combine DeleteBack (backspace) with ReplaceSel
    else if (message == Message::DeleteBack && !actions.empty() && actions.constLast()->message == Message::ReplaceSel) {
        if (actions.last()->str->size() == 1) {
            // A single char left so just remvoe the action
            delete actions.takeLast();
        }
        else {
            actions.last()->str->chop(1);
        }
    }
    else {
        actions.append(new MacroAction(message, wParam, lParam));
    }

#ifdef QT_DEBUG
    for (const MacroAction *ma : actions) {
        qInfo("%s", qUtf8Printable(ma->toString()));
    }
#endif
}

void Macro::replay(ScintillaNext *editor, int n) const
{
    qInfo(Q_FUNC_INFO);

    editor->beginUndoAction();

    while (n > 0) {
        for (const MacroAction *ma : actions) {
            if (MessageHasString(ma->message)) {
                editor->sends(static_cast<int>(ma->message), ma->wParam, ma->str->constBegin());
            }
            else {
                editor->send(static_cast<int>(ma->message), ma->wParam, ma->lParam);
            }
        }

        --n;
    }

    editor->endUndoAction();
}

void Macro::replayTillEndOfFile(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    editor->beginUndoAction();

    do {
        int length = editor->length();
        int curPos = editor->currentPos();

        replay(editor);

        // The doc can shrink...yay
        if (editor->length() < length) {
            continue;
        }
        // The doc can grow...
        else if(editor->length() > length) {
            // are we going to catch up?
            int deltaLength = editor->length() - length;
            int deltaPos = editor->currentPos() - curPos;
            if (deltaPos > deltaLength) {
                // Cursor position is moving forward more than document is growing
                continue;
            }
        }
        // the file can stay the same
        else {
            // is the cursor moving?
            if (editor->currentPos() != curPos) {
                continue;
            }
        }

        break;
    } while (true);

    editor->endUndoAction();
}

QString Macro::getName() const
{
    return name;
}

void Macro::setName(const QString &value)
{
    name = value;
}


MacroRecorder::MacroRecorder(QObject *parent) :
    QObject(parent)
{
}

void MacroRecorder::startRecording(ScintillaNext *editor)
{
    this->editor = editor;
    connect(editor, &ScintillaNext::macroRecord, this, &MacroRecorder::recordMacroStep);

    // We don't "own" the macro so don't delete it if it is a valid pointer
    macro = new Macro();

    editor->startRecord();
}

Macro *MacroRecorder::stopRecording()
{
    disconnect(editor, &ScintillaNext::macroRecord, this, &MacroRecorder::recordMacroStep);

    editor->stopRecord();

    Macro *m = macro;
    macro = Q_NULLPTR;
    return m;
}

void MacroRecorder::recordMacroStep(Message message, uptr_t wParam, sptr_t lParam)
{
    qInfo(Q_FUNC_INFO);

    macro->addMacroStep(message, wParam, lParam);
}
