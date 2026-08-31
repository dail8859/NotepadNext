/*
 * This file is part of Notepad Next.
 * Copyright 2026 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "AutoInsertDecorator.h"

#include <QEvent>
#include <QKeyEvent>
#include <QVector>

#include <algorithm>

#include "UndoAction.h"

using namespace Scintilla;

AutoInsertDecorator::AutoInsertDecorator(ScintillaNext *editor)
    : EditorDecorator(editor)
{
    setObjectName("AutoInsertDecorator");
    editor->installEventFilter(this);
}

void AutoInsertDecorator::setParenthesesEnabled(bool enabled)
{
    parenthesesEnabled = enabled;
    updateEnabled();
}

void AutoInsertDecorator::setSquareBracketsEnabled(bool enabled)
{
    squareBracketsEnabled = enabled;
    updateEnabled();
}

void AutoInsertDecorator::setCurlyBracesEnabled(bool enabled)
{
    curlyBracesEnabled = enabled;
    updateEnabled();
}

void AutoInsertDecorator::setSingleQuotesEnabled(bool enabled)
{
    singleQuotesEnabled = enabled;
    updateEnabled();
}

void AutoInsertDecorator::setDoubleQuotesEnabled(bool enabled)
{
    doubleQuotesEnabled = enabled;
    updateEnabled();
}

bool AutoInsertDecorator::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (!isEnabled() || event->type() != QEvent::KeyPress || editor->readOnly())
        return false;

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->isAutoRepeat() ||
        keyEvent->modifiers().testFlag(Qt::ControlModifier) ||
        keyEvent->modifiers().testFlag(Qt::AltModifier) ||
        keyEvent->modifiers().testFlag(Qt::MetaModifier)) {
        return false;
    }

    struct Caret {
        int selection;
        int position;
    };

    QVector<Caret> carets;
    carets.reserve(editor->selections());
    for (int selection = 0; selection < editor->selections(); ++selection) {
        const int caret = editor->selectionNCaret(selection);
        if (caret != editor->selectionNAnchor(selection) ||
            editor->selectionNCaretVirtualSpace(selection) != 0 ||
            editor->selectionNAnchorVirtualSpace(selection) != 0) {
            return false;
        }
        carets.append({selection, caret});
    }

    if (editor->selectionMode() != SC_SEL_STREAM)
        return false;

    const int mainSelection = editor->mainSelection();
    std::sort(carets.begin(), carets.end(), [](const Caret &left, const Caret &right) {
        return left.position < right.position;
    });

    auto restoreCarets = [&](const QVector<int> &positions) {
        QVector<int> bySelection(positions.size());
        for (int i = 0; i < carets.size(); ++i)
            bySelection[carets[i].selection] = positions[i];

        editor->setSelection(bySelection[0], bySelection[0]);
        for (int selection = 1; selection < bySelection.size(); ++selection)
            editor->addSelection(bySelection[selection], bySelection[selection]);
        editor->setMainSelection(mainSelection);
    };

    if (keyEvent->key() == Qt::Key_Backspace) {
        QVector<bool> isPair;
        isPair.reserve(carets.size());
        bool hasPair = false;
        for (const Caret &caret : carets) {
            const bool pair = caret.position > 0 &&
                              generatedClosers.contains(caret.position) &&
                              editor->charAt(caret.position - 1) == matchingOpen(editor->charAt(caret.position));
            isPair.append(pair);
            hasPair |= pair;
        }
        if (!hasPair)
            return false;

        const UndoAction undo(editor);
        const int trackingToken = addTrackingUndoAction();
        QVector<int> positions;
        positions.reserve(carets.size());
        int offset = 0;
        for (int i = 0; i < carets.size(); ++i) {
            const Caret &caret = carets[i];
            const int position = caret.position + offset;
            const int oldLength = editor->length();
            if (isPair[i]) {
                editor->deleteRange(position - 1, 2);
                positions.append(position - 1);
            }
            else {
                editor->setSelection(position, position);
                editor->send(SCI_DELETEBACK);
                positions.append(editor->currentPos());
            }
            offset += editor->length() - oldLength;
        }
        restoreCarets(positions);
        trackingStates[trackingToken].after = generatedClosers;
        return true;
    }

    const QString text = keyEvent->text();
    if (text.size() != 1 || !editor->selectionEmpty())
        return false;

    const char typedCharacter = text.at(0).toLatin1();
    if (isOpeningCharacterEnabled(typedCharacter)) {
        QVector<bool> insertPair(carets.size(), true);
        if (typedCharacter == '\'' || typedCharacter == '"') {
            bool hasPair = false;
            for (int i = 0; i < carets.size(); ++i) {
                int position = carets[i].position;
                int backslashes = 0;
                while (position > 0 && editor->charAt(position - 1) == '\\') {
                    --position;
                    ++backslashes;
                }
                insertPair[i] = backslashes % 2 == 0;
                hasPair |= insertPair[i];
            }

            // Let Scintilla handle an ordinary escaped quote when no caret
            // needs an auto-inserted closer.
            if (!hasPair)
                return false;
        }

        const UndoAction undo(editor);
        const int trackingToken = addTrackingUndoAction();
        QVector<int> positions;
        positions.reserve(carets.size());
        int offset = 0;
        for (int i = 0; i < carets.size(); ++i) {
            const Caret &caret = carets[i];
            const int position = caret.position + offset;
            if (insertPair[i]) {
                QByteArray pair(1, typedCharacter);
                pair.append(matchingClose(typedCharacter));
                editor->insertText(position, pair.constData());
                generatedClosers.insert(position + 1);
                offset += 2;
            }
            else {
                editor->insertText(position, QByteArray(1, typedCharacter).constData());
                ++offset;
            }
            positions.append(position + 1);
        }
        restoreCarets(positions);
        trackingStates[trackingToken].after = generatedClosers;
        return true;
    }

    if (!isClosingCharacterEnabled(typedCharacter))
        return false;

    QVector<bool> isGeneratedCloser;
    isGeneratedCloser.reserve(carets.size());
    bool hasGeneratedCloser = false;
    for (const Caret &caret : carets) {
        const bool generated = editor->charAt(caret.position) == typedCharacter &&
                               generatedClosers.contains(caret.position);
        isGeneratedCloser.append(generated);
        hasGeneratedCloser |= generated;
    }
    if (!hasGeneratedCloser)
        return false;

    const bool insertsCharacter = std::any_of(isGeneratedCloser.cbegin(), isGeneratedCloser.cend(),
                                               [](bool generated) { return !generated; });
    const UndoAction undo(editor);
    const int trackingToken = insertsCharacter ? addTrackingUndoAction() : -1;
    QVector<int> positions;
    positions.reserve(carets.size());
    int offset = 0;
    for (int i = 0; i < carets.size(); ++i) {
        const int position = carets[i].position + offset;
        if (isGeneratedCloser[i]) {
            generatedClosers.remove(position);
        }
        else {
            editor->insertText(position, QByteArray(1, typedCharacter).constData());
            ++offset;
        }
        positions.append(position + 1);
    }
    restoreCarets(positions);
    if (trackingToken >= 0)
        trackingStates[trackingToken].after = generatedClosers;
    return true;
}

void AutoInsertDecorator::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::Modified) {
        if (FlagSet(pscn->modificationType, ModificationFlags::Container)) {
            const auto state = trackingStates.constFind(pscn->token);
            if (state != trackingStates.cend()) {
                if (FlagSet(pscn->modificationType, ModificationFlags::Undo))
                    generatedClosers = state->before;
                else if (FlagSet(pscn->modificationType, ModificationFlags::Redo)) {
                    pendingRedoState = state->after;
                    hasPendingRedoState = true;
                }
            }
            return;
        }

        if (FlagSet(pscn->modificationType, ModificationFlags::InsertText)) {
            QSet<int> updated;
            for (int closer : qAsConst(generatedClosers))
                updated.insert(closer >= pscn->position ? closer + pscn->length : closer);
            generatedClosers = updated;
        }
        else if (FlagSet(pscn->modificationType, ModificationFlags::DeleteText)) {
            QSet<int> updated;
            const int end = pscn->position + pscn->length;
            for (int closer : qAsConst(generatedClosers)) {
                if (closer < pscn->position)
                    updated.insert(closer);
                else if (closer - 1 >= pscn->position && closer - 1 < end)
                    continue;
                else if (closer >= end)
                    updated.insert(closer - pscn->length);
            }
            generatedClosers = updated;
        }

        if (FlagSet(pscn->modificationType, ModificationFlags::Redo) &&
            FlagSet(pscn->modificationType, ModificationFlags::LastStepInUndoRedo) &&
            hasPendingRedoState) {
            generatedClosers = pendingRedoState;
            pendingRedoState.clear();
            hasPendingRedoState = false;
        }
        return;
    }

}

int AutoInsertDecorator::addTrackingUndoAction()
{
    const int token = nextTrackingToken++;
    trackingStates.insert(token, {generatedClosers, generatedClosers});
    editor->addUndoAction(token, static_cast<int>(UndoFlags::None));
    return token;
}

bool AutoInsertDecorator::isOpeningCharacterEnabled(char ch) const
{
    switch (ch) {
    case '(':
        return parenthesesEnabled;
    case '[':
        return squareBracketsEnabled;
    case '{':
        return curlyBracesEnabled;
    case '\'':
        return singleQuotesEnabled;
    case '"':
        return doubleQuotesEnabled;
    default:
        return false;
    }
}

bool AutoInsertDecorator::isClosingCharacterEnabled(char ch) const
{
    switch (ch) {
    case ')':
        return parenthesesEnabled;
    case ']':
        return squareBracketsEnabled;
    case '}':
        return curlyBracesEnabled;
    case '\'':
        return singleQuotesEnabled;
    case '"':
        return doubleQuotesEnabled;
    default:
        return false;
    }
}

char AutoInsertDecorator::matchingClose(char ch)
{
    switch (ch) {
    case '(':
        return ')';
    case '[':
        return ']';
    case '{':
        return '}';
    case '\'':
        return '\'';
    case '"':
        return '"';
    default:
        return '\0';
    }
}

char AutoInsertDecorator::matchingOpen(char ch)
{
    switch (ch) {
    case ')':
        return '(';
    case ']':
        return '[';
    case '}':
        return '{';
    case '\'':
        return '\'';
    case '"':
        return '"';
    default:
        return '\0';
    }
}

void AutoInsertDecorator::updateEnabled()
{
    const bool enabled = parenthesesEnabled || squareBracketsEnabled || curlyBracesEnabled ||
                         singleQuotesEnabled || doubleQuotesEnabled;
    if (enabled != isEnabled())
        setEnabled(enabled);
}
