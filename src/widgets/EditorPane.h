/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
 *
 * A horizontal splitter that hosts the function list on the left and the
 * editor on the right. Because it lives INSIDE the editor dock widget
 * (not as a separate ADS dock), it can never overlap the "Folder as
 * Workspace" dock and it always follows the editor's size.
 *
 * This file is part of a feature licensed under the GNU General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 */

#ifndef EDITORPANE_H
#define EDITORPANE_H

#include <QSplitter>

#include "FunctionListWidget.h"

class ScintillaNext;

class EditorPane : public QSplitter
{
    Q_OBJECT

public:
    explicit EditorPane(ScintillaNext *editor, QWidget *parent = nullptr);

    ScintillaNext *editor() const { return ed; }
    FunctionListWidget *functionList() const { return funcList; }

    // Feature switch for this editor (mirrors the view menu action). Kept
    // separate from the widget's visibility because the panel is only shown for
    // files ctags claims it can parse: a plain text file then costs no screen
    // space instead of showing an empty box. Unlike isFunctionListVisible() this
    // stays true while the tab is in the background, so callers do not have to
    // care about tab visibility.
    void setFunctionListEnabled(bool enabled);
    bool isFunctionListEnabled() const { return funcListEnabled; }

    // Visibility of the panel widget itself. Only meaningful while enabled.
    void setFunctionListVisible(bool visible);
    bool isFunctionListVisible() const;

    // The requested state, which - unlike isFunctionListVisible() - does not
    // depend on the window or the tab actually being on screen, so it is also
    // meaningful while the window is hidden.
    bool isFunctionListRequested() const { return funcListRequested; }

    // Returns the pane wrapping the editor, or nullptr if the editor is unwrapped
    static EditorPane *paneForEditor(ScintillaNext *editor);

    // Resolves a dock widget content widget to its editor (unwrapped or pane-wrapped)
    static ScintillaNext *editorFromWidget(QWidget *widget);

private:
    FunctionListWidget *funcList = nullptr;
    ScintillaNext *ed = nullptr;
    bool funcListEnabled = false;   // view menu switch, independent of tab visibility
    bool funcListRequested = false; // requested panel state, independent of tab visibility
    int lastFuncListWidth = 230;
};

#endif // EDITORPANE_H
