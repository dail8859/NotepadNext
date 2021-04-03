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

#ifndef LANGUAGEINSPECTORDOCK_H
#define LANGUAGEINSPECTORDOCK_H

#include <QDockWidget>

class MainWindow;
class ScintillaNext;

namespace Ui {
class LanguageInspectorDock;
}

class LanguageInspectorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit LanguageInspectorDock(MainWindow *parent);
    ~LanguageInspectorDock();

private slots:
    void updateInformation(ScintillaNext *editor);

private:
    Ui::LanguageInspectorDock *ui;

    void updateLanguageName(ScintillaNext *editor);
    void updatePropertyInfo(ScintillaNext *editor);
    void updateKeywordInfo(ScintillaNext *editor);
    void updateStyleInfo(ScintillaNext *editor);
};

#endif // LANGUAGEINSPECTORDOCK_H
