/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
 *
 * EditorPane implementation.
 *
 * This file is part of a feature licensed under the GNU General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 */

#include "EditorPane.h"
#include "ScintillaNext.h"

EditorPane::EditorPane(ScintillaNext *editor, QWidget *parent)
    : QSplitter(Qt::Horizontal, parent), ed(editor)
{
    setChildrenCollapsible(false);
    setHandleWidth(3);

    funcList = new FunctionListWidget(this);
    funcList->setMinimumWidth(120);
    addWidget(funcList);

    addWidget(ed); // takes all remaining space

    funcList->setVisible(false); // hidden until a supported file requests it

    // Function list gets a fixed-ish width, editor stretches
    setStretchFactor(0, 0);
    setStretchFactor(1, 1);
    setSizes({lastFuncListWidth, 1000});

    // Remember how wide the user dragged the panel so it can be restored
    connect(this, &QSplitter::splitterMoved, this, [this](int pos, int index) {
        Q_UNUSED(index)
        if (funcList->isVisible() && pos >= funcList->minimumWidth())
            lastFuncListWidth = pos;
    });
}

void EditorPane::setFunctionListEnabled(bool enabled)
{
    funcListEnabled = enabled;

    if (!enabled)
        setFunctionListVisible(false);
}

void EditorPane::setFunctionListVisible(bool visible)
{
    funcListRequested = visible;

    if (visible == funcList->isVisible()) {
        // Already in the requested state; make sure a panel that is meant to be
        // visible really is visible (the pane may have been created before the
        // window was shown)
        if (visible && !funcList->isVisible())
            funcList->setVisible(true);
        return;
    }

    if (visible) {
        funcList->setVisible(true);

        // A hidden splitter child collapses to zero width, so restore the
        // remembered width once the pane has been laid out.
        const int total = width();
        if (total > lastFuncListWidth + 80)
            setSizes({lastFuncListWidth, total - lastFuncListWidth});
    }
    else {
        if (funcList->width() > funcList->minimumWidth())
            lastFuncListWidth = qBound(funcList->minimumWidth(), funcList->width(), 600);
        funcList->setVisible(false);
    }
}

bool EditorPane::isFunctionListVisible() const
{
    return funcList->isVisible();
}

EditorPane *EditorPane::paneForEditor(ScintillaNext *editor)
{
    if (!editor)
        return nullptr;
    return qobject_cast<EditorPane *>(editor->parentWidget());
}

ScintillaNext *EditorPane::editorFromWidget(QWidget *widget)
{
    if (!widget)
        return nullptr;
    if (ScintillaNext *editor = qobject_cast<ScintillaNext *>(widget))
        return editor;
    if (EditorPane *pane = qobject_cast<EditorPane *>(widget))
        return pane->editor();
    return nullptr;
}
