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
#include "MacroStep.h"


using namespace Scintilla;

MacroStep::MacroStep(Message message, uptr_t wParam, sptr_t lParam) :
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

MacroStep::~MacroStep()
{
}

QString MacroStep::toString() const
{
    if (MacroStep::MessageHasString(message)) {
        return QString("MacroStep(%1, %2, \"%3\")")
                .arg(name())
                .arg(wParam)
                .arg(str.constData());
    }
    else {
        return QString("MacroStep(%1, %2, %3)")
                .arg(name())
                .arg(wParam)
                .arg(lParam);
    }
}

QString MacroStep::name() const
{
    QMetaEnum metaEnum = QMetaEnum::fromType<ScintillaNext::Message>();

    return metaEnum.valueToKey(static_cast<int>(message));
}

void MacroStep::replay(ScintillaNext *editor) const
{
    if (MacroStep::MessageHasString(message)) {
        editor->sends(static_cast<int>(message), wParam, str.constBegin());
    }
    else {
        editor->send(static_cast<int>(message), wParam, lParam);
    }
}

bool MacroStep::MessageHasString(Scintilla::Message message)
{
    return message == Message::ReplaceSel ||
            message == Message::InsertText ||
            message == Message::AddText ||
            message == Message::AppendText;
}

QDataStream &operator<<(QDataStream& stream, const MacroStep &macroStep)
{
    stream << macroStep.message << macroStep.wParam;

    if (MacroStep::MessageHasString(macroStep.message))
        return stream << macroStep.str;
    else
        return stream << macroStep.lParam;
}

QDataStream &operator>>(QDataStream& stream, MacroStep &macroStep)
{
    stream >> macroStep.message >> macroStep.wParam;

    if (MacroStep::MessageHasString(macroStep.message))
        return stream >> macroStep.str;
    else
        return stream >> macroStep.lParam;
}
