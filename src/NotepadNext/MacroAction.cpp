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


#include <QMetaEnum>

#include "ScintillaNext.h"
#include "MacroAction.h"


using namespace Scintilla;

MacroAction::MacroAction(Message message, uptr_t wParam, sptr_t lParam) :
    message(message),
    wParam(wParam)
{
    if (message == Message::ReplaceSel || message == Message::InsertText) {
        // wParam is 0 for replace, and position for insert
        this->str = QByteArray(reinterpret_cast<const char*>(lParam));
    }
    else if (message == Message::AddText || message == Message::AppendText) {
        // wParam is length of text
        this->str = QByteArray(reinterpret_cast<const char*>(lParam), wParam);
    }
    else {
        this->lParam = lParam;
    }
}

MacroAction::~MacroAction()
{
}

QString MacroAction::toString() const
{
    QMetaEnum metaEnum = QMetaEnum::fromType<ScintillaNext::Message>();

    if (MacroAction::MessageHasString(message)) {
        return QString("MacroAction(%1, %2, \"%3\")")
                .arg(metaEnum.valueToKey(static_cast<int>(message)))
                .arg(wParam)
                .arg(str.constData());
    }
    else {
        return QString("MacroAction(%1, %2, %3)")
                .arg(metaEnum.valueToKey(static_cast<int>(message)))
                .arg(wParam)
                .arg(lParam);
    }
}

void MacroAction::replay(ScintillaNext *editor) const
{
    if (MacroAction::MessageHasString(message)) {
        editor->sends(static_cast<int>(message), wParam, str.constBegin());
    }
    else {
        editor->send(static_cast<int>(message), wParam, lParam);
    }
}

bool MacroAction::MessageHasString(Scintilla::Message message)
{
    return message == Message::ReplaceSel ||
            message == Message::InsertText ||
            message == Message::AddText ||
            message == Message::AppendText;
}

QDataStream &operator<<(QDataStream& stream, const MacroAction &macroAction)
{
    stream << macroAction.message << macroAction.wParam;

    if (MacroAction::MessageHasString(macroAction.message))
        return stream << macroAction.str;
    else
        return stream << macroAction.lParam;
}

QDataStream &operator>>(QDataStream& stream, MacroAction &macroAction)
{
    stream >> macroAction.message >> macroAction.wParam;

    if (MacroAction::MessageHasString(macroAction.message))
        return stream >> macroAction.str;
    else
        return stream >> macroAction.lParam;
}
