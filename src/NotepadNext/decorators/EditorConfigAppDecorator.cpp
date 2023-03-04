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

#include "EditorConfigAppDecorator.h"
#include "EditorManager.h"
#include "ScintillaNext.h"
#include "Finder.h"

#include "EditorConfig.h"

class PreventUnfolding
{
public:
    explicit PreventUnfolding(ScintillaNext *editor) : editor(editor) {
        foldFlags = editor->automaticFold();
        editor->setAutomaticFold(0);
    }

    ~PreventUnfolding() {
        editor->setAutomaticFold(foldFlags);
    }

private:
    ScintillaNext *editor;
    int foldFlags;
};


EditorConfigAppDecorator::EditorConfigAppDecorator(NotepadNextApplication *app)
     : ApplicationDecorator(app)
{
    EditorManager *manager = app->getEditorManager();

    connect(manager, &EditorManager::editorCreated, this, &EditorConfigAppDecorator::doEditorConfig);
    // TODO: on editor reload
}

void EditorConfigAppDecorator::doEditorConfig(ScintillaNext *editor)
{
    if (this->isEnabled()) {
        if (editor->isFile()) {
            EditorConfigSettings settings = EditorConfig::getFileSettings(editor->getFilePath());

            qDebug() << "EditorConfig settings for:" << editor->getFilePath();
            for(auto &setting : settings.toStdMap()) {
                qDebug() << setting.first << "=" << setting.second;
            }

            if (settings.contains(QStringLiteral("indent_style"))) {
                if (settings[QStringLiteral("indent_style")] == QStringLiteral("tab")) editor->setUseTabs(true);
                else if (settings[QStringLiteral("indent_style")] == QStringLiteral("space")) editor->setUseTabs(false);

                // Set a flag so that the tab/spaces won't get overridden
                editor->QObject::setProperty("nn_skip_usetabs", "EditorConfig");
            }

            if (settings.contains(QStringLiteral("indent_size")) && settings[QStringLiteral("indent_size")].toInt() > 0) {
                editor->setIndent(settings[QStringLiteral("indent_size")].toInt());
            }

            if (settings.contains(QStringLiteral("tab_width")) && settings[QStringLiteral("tab_width")].toInt() > 0) {
                editor->setTabWidth(settings[QStringLiteral("tab_width")].toInt());

                // Set a flag so that the tab width won't get overridden
                editor->QObject::setProperty("nn_skip_tabwidth", "EditorConfig");
            }

            if (settings.contains(QStringLiteral("end_of_line"))) {
                if (settings[QStringLiteral("end_of_line")] == QStringLiteral("lf")) editor->setEOLMode(SC_EOL_LF);
                else if (settings[QStringLiteral("end_of_line")] == QStringLiteral("cr")) editor->setEOLMode(SC_EOL_CR);
                else if (settings[QStringLiteral("end_of_line")] == QStringLiteral("crlf")) editor->setEOLMode(SC_EOL_CRLF);
            }

            if (settings.contains(QStringLiteral("trim_trailing_whitespace"))) {
                if (settings[QStringLiteral("trim_trailing_whitespace")] == QStringLiteral("true")) {
                    connect(editor, &ScintillaNext::aboutToSave, this, &EditorConfigAppDecorator::trimTrailingWhitespace);
                }
            }

            if (settings.contains(QStringLiteral("insert_final_newline"))) {
                if (settings[QStringLiteral("insert_final_newline")] == QStringLiteral("true")) {
                    connect(editor, &ScintillaNext::aboutToSave, this, &EditorConfigAppDecorator::ensureFinalNewline);
                }
                else if (settings[QStringLiteral("insert_final_newline")] == QStringLiteral("false")) {
                    connect(editor, &ScintillaNext::aboutToSave, this, &EditorConfigAppDecorator::ensureNoFinalNewline);
                }
            }
        }
    }
}

void EditorConfigAppDecorator::trimTrailingWhitespace()
{
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());
    Finder f(editor);
    const PreventUnfolding pu(editor);

    // NOTE: see note in QRegexSearch about (*ANYCRLF)
    f.setSearchText("(*ANYCRLF)[ \t]+$");
    f.setSearchFlags(SCFIND_REGEXP);
    f.replaceAll("");
}

void EditorConfigAppDecorator::ensureFinalNewline()
{
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());
    const PreventUnfolding pu(editor);
    const int lastLine = editor->lineCount() - 1;
    const int lastLineLength = editor->lineEndPosition(lastLine) - editor->positionFromLine(lastLine);

    if (lastLineLength != 0) {
        switch (editor->eOLMode()) {
            case SC_EOL_CRLF:
                editor->appendText(2, "\r\n");
                break;
            case SC_EOL_CR:
                editor->appendText(1, "\r");
                break;
            case SC_EOL_LF:
                editor->appendText(1, "\n");
                break;
        }
    }
}

void EditorConfigAppDecorator::ensureNoFinalNewline()
{
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(sender());
    const PreventUnfolding pu(editor);

    editor->deleteTrailingEmptyLines();
}
