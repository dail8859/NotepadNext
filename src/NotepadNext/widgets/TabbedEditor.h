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


#ifndef TABBDEDITOR_H
#define TABBDEDITOR_H

#include <QTabBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QList>

#include "ScintillaBuffer.h"
#include "ScintillaNext.h"

class TabbedEditor : public QWidget
{
    Q_OBJECT

public:
    enum { INVALID_INDEX = -1 };

    explicit TabbedEditor(QWidget *parent = Q_NULLPTR);
    ~TabbedEditor();

    int count() const;
    int currentIndex() const;
    ScintillaBuffer *getCurrentBuffer();

    void switchToIndex(int index);
    void switchToBuffer(const ScintillaBuffer *buffer);

    ScintillaBuffer *getBufferFromIndex(int index);
    int getIndexFromBuffer(const ScintillaBuffer *buffer);

    QTabBar *getTabBar() { return tabBar; }
    ScintillaNext *getEditor() { return editor; }

signals:
    void allBuffersClosed();
    void bufferSwitched(ScintillaBuffer *buffer);

public slots:
    void addBuffer(ScintillaBuffer *buffer);
    void removeBuffer(ScintillaBuffer *buffer);
    void renamedBuffer(ScintillaBuffer *buffer);

private slots:
    void setBufferSaveIcon(bool atSavePoint);

private:
    QTabBar *tabBar;

    ScintillaNext *editor;

    bool ignoreNextTabIndexChanged = false;
    bool forceSwitch = false;
    int previousIndex = -1;

    void forceSwitchToIndex(int index) {
        forceSwitch = true;
        switchToIndex(index);
        forceSwitch = false;
    }
};

#endif // TABBDEDITOR_H
