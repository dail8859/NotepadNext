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


#pragma once

#include "EditorDecorator.h"


class BookMarkDecorator : public EditorDecorator
{
    Q_OBJECT

public:
    explicit BookMarkDecorator(ScintillaNext *editor);

    bool isBookmarkSet(int line) const;
    void addBookmark(int line);
    void removeBookmark(int line);
    void toggleBookmark(int line);

    int nextBookmarkAfter(int line);
    int previousBookMarkBefore(int line);

    void clearAllBookmarks();

    QList<int> bookMarkedLines() const;
    void setBookMarkedLines(QList<int> &lines);

    QString cutBookMarkedLines();
    QString copyBookMarkedLines();
    void deleteBookMarkedLines();

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;
};
