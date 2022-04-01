/*
 * This file is part of Notepad Next.
 * Copyright 2021 Justin Dailey
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


#include <QEvent>
#include <QKeyEvent>

#include "BetterMultiSelection.h"



struct Selection {
    int caret;
    int anchor;

    Selection(int caret, int anchor) : caret(caret), anchor(anchor) {}

    int start() const { return qMin(caret, anchor); }
    int end() const { return qMax(caret, anchor); }
    int length() const { return end() - start(); }
    void set(int pos) { anchor = caret = pos; }
    void offset(int offset) { anchor += offset; caret += offset; }
};

template<typename It>
It uniquify(It begin, It const end)
{
    std::vector<It> v;
    v.reserve(static_cast<size_t>(std::distance(begin, end)));

    for (It i = begin; i != end; ++i)
        v.push_back(i);

    std::sort(v.begin(), v.end(), [](const auto &lhs, const auto &rhs) {
        return (*lhs).start() < (*rhs).start() || (!((*rhs).start() < (*lhs).start()) && (*lhs).end() < (*rhs).end());
    });

    v.erase(std::unique(v.begin(), v.end(), [](const auto &lhs, const auto &rhs) {
        return (*lhs).start() == (*rhs).start() && (*lhs).end() == (*rhs).end();
    }), v.end());

    std::sort(v.begin(), v.end());

    size_t j = 0;
    for (It i = begin; i != end && j != v.size(); ++i) {
        if (i == v[j]) {
            using std::iter_swap; iter_swap(i, begin);
            ++j;
            ++begin;
        }
    }
    return begin;
}

BetterMultiSelection::BetterMultiSelection(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    setObjectName("BetterMultiSelection");

    // Allow insertion of autocompletion at each cursor
    editor->autoCSetMulti(SC_MULTIAUTOC_EACH);

    editor->installEventFilter(this);
}

bool BetterMultiSelection::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        bool isAltPresssed = (keyEvent->modifiers() & Qt::AltModifier);
        bool isControlPresssed = (keyEvent->modifiers() & Qt::ControlModifier);
        bool isShiftPressed = (keyEvent->modifiers() & Qt::ShiftModifier);

        if (isAltPresssed)
            return false;

        if (editor->selections() > 1) {
            if (isControlPresssed) {
                if (keyEvent->key() == Qt::Key_Left) {
                    EditSelections(SimpleEdit(isShiftPressed ? SCI_WORDLEFTEXTEND : SCI_WORDLEFT));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Right) {
                    EditSelections(SimpleEdit(isShiftPressed ? SCI_WORDRIGHTENDEXTEND : SCI_WORDRIGHT));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Back) {
                    EditSelections(SimpleEdit(SCI_DELWORDLEFT));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Delete) {
                    EditSelections(SimpleEdit(SCI_DELWORDRIGHT));
                    return true;
                }
                //else if (keyEvent->key() == Qt::Key_X || keyEvent->key() == Qt::Key_C) {
                //    if (CopyToClipboard(editor)) {
                //        if (keyEvent->key() == Qt::Key_X) {
                //            EditSelections(SimpleEdit(SCI_DELETEBACK));
                //        }
                //        return true;
                //    }
                //}
                //else if (keyEvent->key() == Qt::Key_V) {
                //    if (Paste(editor)) {
                //        return true;
                //    }
                //}
            }
            else {
                if (keyEvent->key() == Qt::Key_Escape) {
                    int caret = editor->selectionNCaret(editor->mainSelection());
                    editor->setSelection(caret, caret);
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Left) {
                    EditSelections(SimpleEdit(isShiftPressed ? SCI_CHARLEFTEXTEND : SCI_CHARLEFT));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Right) {
                    EditSelections(SimpleEdit(isShiftPressed ? SCI_CHARRIGHTEXTEND : SCI_CHARRIGHT));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Home) {
                    EditSelections(SimpleEdit(isShiftPressed ? SCI_VCHOMEWRAPEXTEND : SCI_VCHOMEWRAP));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_End) {
                    EditSelections(SimpleEdit(isShiftPressed ? SCI_LINEENDWRAPEXTEND : SCI_LINEENDWRAP));
                    return true;
                }
                else if (keyEvent->key() == Qt::Key_Return) {
                    if (!editor->autoCActive()) {
                        EditSelections(SimpleEdit(SCI_NEWLINE));
                        return true;
                    }
                    // else just let Scintilla handle the insertion of autocompletion
                }
                else if (keyEvent->key() == Qt::Key_Up) {
                    if (!editor->autoCActive()) {
                        EditSelections(SimpleEdit(isShiftPressed ? SCI_LINEUPEXTEND : SCI_LINEUP));
                        return true;
                    }
                    // else just let Scintilla handle the navigation of autocompletion
                }
                else if (keyEvent->key() == Qt::Key_Down) {
                    if (!editor->autoCActive()) {
                        EditSelections(SimpleEdit(isShiftPressed ? SCI_LINEDOWNEXTEND : SCI_LINEDOWN));
                        return true;
                    }
                    // else just let Scintilla handle the navigation of autocompletion
                }
            }
        }
    }

    return false;
}

void BetterMultiSelection::notify(const Scintilla::NotificationData *pscn)
{
    Q_UNUSED(pscn);
}

QVector<Selection> BetterMultiSelection::GetSelections() {
    QVector<Selection> selections;

    int num = editor->selections();
    for (int i = 0; i < num; ++i) {
        int caret = editor->selectionNCaret(i);
        int anchor = editor->selectionNAnchor(i);
        selections.append(Selection{ caret, anchor });
    }

    return selections;
}

void BetterMultiSelection::SetSelections(const QVector<Selection> &selections) {
    for (size_t i = 0; i < selections.size(); ++i) {
        if (i == 0)
            editor->setSelection(selections[i].caret, selections[i].anchor);
        else
            editor->addSelection(selections[i].caret, selections[i].anchor);
    }
}

void BetterMultiSelection::EditSelections(std::function<void(Selection &selection)> edit) {
    auto selections = GetSelections();

    editor->clearSelections();

    std::sort(selections.begin(), selections.end(), [](const auto &lhs, const auto &rhs) {
        return lhs.start() < rhs.start() || (!(rhs.start() < lhs.start()) && lhs.end() < rhs.end());
    });

    editor->beginUndoAction();

    int totalOffset = 0;
    for (auto &selection : selections) {
        selection.offset(totalOffset);
        const int length = editor->length();

        edit(selection);

        totalOffset += editor->length() - length;
    }

    editor->endUndoAction();;

    selections.erase(uniquify(selections.begin(), selections.end()), selections.end());

    SetSelections(selections);
}

std::function<void(Selection &selection)> BetterMultiSelection::SimpleEdit(int message) {
    return [=](Selection &selection) {
        editor->setSelection(selection.caret, selection.anchor);
        editor->send(message);

        selection.caret = editor->selectionNCaret(0);
        selection.anchor = editor->selectionNAnchor(0);
    };
}
