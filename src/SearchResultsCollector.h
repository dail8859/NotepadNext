/*
 * This file is part of Notepad Next.
 * Copyright 2023 Justin Dailey
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


#include "ISearchResultsHandler.h"

class SearchResultsCollector: public ISearchResultsHandler
{
public:
    SearchResultsCollector(ISearchResultsHandler *child);
    virtual ~SearchResultsCollector() {}

    void newSearch(const QString searchTerm) override;
    void newFileEntry(ScintillaNext *editor) override;
    void newResultsEntry(const QString line, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning, int hitCount=1) override;
    void completeSearch() override;

private:
    ISearchResultsHandler *child;
    int runningHitCount = 0;

    QString prevLine;
    int prevLineNumber;
    int prevStartPositionFromBeginning;
    int prevEndPositionFromBeginning;
};

