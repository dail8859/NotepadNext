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


QMap<Scintilla::Message, QString> messageData {
    { Scintilla::Message::Cut, "Cut" },
    { Scintilla::Message::Copy, "Copy" },
    { Scintilla::Message::Paste, "Paste" },
    { Scintilla::Message::Clear, "Clear" },
    { Scintilla::Message::ReplaceSel, "Replace Selection" },
    //{ Scintilla::Message::AddText, "Add Text" },
    //{ Scintilla::Message::InsertText, "Insert Text" },
    //{ Scintilla::Message::AppendText, "Append Text" },
    { Scintilla::Message::ClearAll, "Clear All" },
    { Scintilla::Message::SelectAll, "Select All" },
    //{ Scintilla::Message::GotoLine, "GotoLine" },
    //{ Scintilla::Message::GotoPos, "GotoPos" },
    { Scintilla::Message::SearchAnchor, "Search Anchor" },
    { Scintilla::Message::SearchNext, "Search Next" },
    { Scintilla::Message::SearchPrev, "Search Prev" },
    { Scintilla::Message::LineDown, "Line Down" },
    { Scintilla::Message::LineDownExtend, "Line Down Extend" },
    { Scintilla::Message::ParaDown, "Para Down" },
    { Scintilla::Message::ParaDownExtend, "Para Down Extend" },
    { Scintilla::Message::LineUp, "Line Up" },
    { Scintilla::Message::LineUpExtend, "Line Up Extend" },
    { Scintilla::Message::ParaUp, "Para Up" },
    { Scintilla::Message::ParaUpExtend, "Para Up Extend" },
    { Scintilla::Message::CharLeft, "Char Left" },
    { Scintilla::Message::CharLeftExtend, "Char Left Extend" },
    { Scintilla::Message::CharRight, "Char Right" },
    { Scintilla::Message::CharRightExtend, "Char Right Extend" },
    { Scintilla::Message::WordLeft, "Word Left" },
    { Scintilla::Message::WordLeftExtend, "Word Left Extend" },
    { Scintilla::Message::WordRight, "WordRight" },
    { Scintilla::Message::WordRightExtend, "Word Right Extend" },
    { Scintilla::Message::WordPartLeft, "Word Part Left" },
    { Scintilla::Message::WordPartLeftExtend, "Word Part Left Extend" },
    { Scintilla::Message::WordPartRight, "Word Part Right" },
    { Scintilla::Message::WordPartRightExtend, "Word Part Right Extend" },
    { Scintilla::Message::WordLeftEnd, "Word Left End" },
    { Scintilla::Message::WordLeftEndExtend, "Word Left End Extend" },
    { Scintilla::Message::WordRightEnd, "Word Right End" },
    { Scintilla::Message::WordRightEndExtend, "Word Right End Extend" },
    { Scintilla::Message::Home, "Home" },
    { Scintilla::Message::HomeExtend, "Home Extend" },
    { Scintilla::Message::LineEnd, "Line End" },
    { Scintilla::Message::LineEndExtend, "Line End Extend" },
    { Scintilla::Message::HomeWrap, "Home Wrap" },
    { Scintilla::Message::HomeWrapExtend, "Home Wrap Extend" },
    { Scintilla::Message::LineEndWrap, "Line End Wrap" },
    { Scintilla::Message::LineEndWrapExtend, "Line End Wrap Extend" },
    { Scintilla::Message::DocumentStart, "Document Start" },
    { Scintilla::Message::DocumentStartExtend, "Document Start Extend" },
    { Scintilla::Message::DocumentEnd, "Document End" },
    { Scintilla::Message::DocumentEndExtend, "Document End Extend" },
    //{ Scintilla::Message::StutteredPageUp, "Stuttered Page Up" },
    //{ Scintilla::Message::StutteredPageUpExtend, "Stuttered Page Up Extend" },
    //{ Scintilla::Message::StutteredPageDown, "Stuttered Page Down" },
    //{ Scintilla::Message::StutteredPageDownExtend, "Stuttered Page Down Extend" },
    { Scintilla::Message::PageUp, "Page Up" },
    { Scintilla::Message::PageUpExtend, "Page Up Extend" },
    { Scintilla::Message::PageDown, "Page Down" },
    { Scintilla::Message::PageDownExtend, "Page Down Extend" },
    { Scintilla::Message::EditToggleOvertype, "Edit Toggle Overtype" },
    { Scintilla::Message::Cancel, "Cancel" },
    { Scintilla::Message::DeleteBack, "Delete Back" },
    { Scintilla::Message::Tab, "Tab" },
    { Scintilla::Message::BackTab, "Back Tab" },
    { Scintilla::Message::FormFeed, "Form Feed" },
    { Scintilla::Message::VCHome, "VC Home" },
    { Scintilla::Message::VCHomeExtend, "VC Home Extend" },
    { Scintilla::Message::VCHomeWrap, "VC Home Wrap" },
    { Scintilla::Message::VCHomeWrapExtend, "VC Home Wrap Extend" },
    { Scintilla::Message::VCHomeDisplay, "VCHome Display" },
    { Scintilla::Message::VCHomeDisplayExtend, "VC Home Display Extend" },
    { Scintilla::Message::DelWordLeft, "Delete Word Left" },
    { Scintilla::Message::DelWordRight, "Delete Word Right" },
    { Scintilla::Message::DelWordRightEnd, "Delete Word Right End" },
    { Scintilla::Message::DelLineLeft, "Delete Line Left" },
    { Scintilla::Message::DelLineRight, "Delete Line Right" },
    { Scintilla::Message::LineCopy, "Line Copy" },
    { Scintilla::Message::LineCut, "Line Cut" },
    { Scintilla::Message::LineDelete, "Line Delete" },
    { Scintilla::Message::LineTranspose, "Line Transpose" },
    { Scintilla::Message::LineReverse, "Line Reverse" },
    { Scintilla::Message::LineDuplicate, "Line Duplicate" },
    { Scintilla::Message::LowerCase, "Lower Case" },
    { Scintilla::Message::UpperCase, "Upper Case" },
    { Scintilla::Message::LineScrollDown, "Line Scroll Down" },
    { Scintilla::Message::LineScrollUp, "Line Scroll Up" },
    { Scintilla::Message::DeleteBackNotLine, "Delete Back Not Line" },
    { Scintilla::Message::HomeDisplay, "Home Display" },
    { Scintilla::Message::HomeDisplayExtend, "Home Display Extend" },
    { Scintilla::Message::LineEndDisplay, "Line End Display" },
    { Scintilla::Message::LineEndDisplayExtend, "Line End Display Extend" },
    //{ Scintilla::Message::SetSelectionMode, "Set Selection Mode" },
    { Scintilla::Message::LineDownRectExtend, "Line Down Rect Extend" },
    { Scintilla::Message::LineUpRectExtend, "Line Up Rect Extend" },
    { Scintilla::Message::CharLeftRectExtend, "Char Left Rect Extend" },
    { Scintilla::Message::CharRightRectExtend, "Char Right Rect Extend" },
    { Scintilla::Message::HomeRectExtend, "Home Rect Extend" },
    { Scintilla::Message::VCHomeRectExtend, "VC Home Rect Extend" },
    { Scintilla::Message::LineEndRectExtend, "Line End Rect Extend" },
    { Scintilla::Message::PageUpRectExtend, "Page Up Rect Extend" },
    { Scintilla::Message::PageDownRectExtend, "Page Down Rect Extend" },
    { Scintilla::Message::SelectionDuplicate, "Selection Duplicate" },
    { Scintilla::Message::CopyAllowLine, "Copy Allow Line" },
    { Scintilla::Message::VerticalCentreCaret, "Vertical Centre Caret" },
    { Scintilla::Message::MoveSelectedLinesUp, "Move Selected Lines Up" },
    { Scintilla::Message::MoveSelectedLinesDown, "Move Selected Lines Down" },
    { Scintilla::Message::ScrollToStart, "Scroll To Start" },
    { Scintilla::Message::ScrollToEnd, "Scroll To End" }
};

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
                .arg(getName())
                .arg(wParam)
                .arg(str.constData());
    }
    else {
        return QString("MacroStep(%1, %2, %3)")
                .arg(getName())
                .arg(wParam)
                .arg(lParam);
    }
}

QString MacroStep::getName() const
{
    return MacroStep::NameOfMessage(message);
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

QString MacroStep::NameOfMessage(Scintilla::Message message)
{
    // Try to find the name in the known list
    if (messageData.contains(message)) {
        return messageData[message];
    }
    else {
        // Default to the list of enums
        QMetaEnum metaEnum = QMetaEnum::fromType<ScintillaNext::Message>();
        return metaEnum.valueToKey(static_cast<int>(message));
    }
}

QList<Message> MacroStep::RecordableMacroMessages()
{
    return messageData.keys();
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
