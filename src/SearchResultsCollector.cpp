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


#include "SearchResultsCollector.h"

SearchResultsCollector::SearchResultsCollector(ISearchResultsHandler *child) :
    child(child)
{
}

void SearchResultsCollector::newSearch(const QString searchTerm)
{
    child->newSearch(searchTerm);
}

void SearchResultsCollector::newFileEntry(ScintillaNext *editor)
{
    // There may be a result that was not passed along yet
    if (runningHitCount > 0) {
        child->newResultsEntry(prevLine, prevLineNumber, prevStartPositionFromBeginning, prevEndPositionFromBeginning, runningHitCount);
    }
    runningHitCount = 0;

    child->newFileEntry(editor);
}

void SearchResultsCollector::newResultsEntry(const QString line, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning, int hitCount)
{
    if (runningHitCount == 0) {
        // Save the previous results since there have not been any yet
        prevLine = line;
        prevLineNumber = lineNumber;
        prevStartPositionFromBeginning = startPositionFromBeginning;
        prevEndPositionFromBeginning = endPositionFromBeginning;

        runningHitCount = 1;
    }
    else if (lineNumber != prevLineNumber) {
        // Report the previous data
        child->newResultsEntry(prevLine, prevLineNumber, prevStartPositionFromBeginning, prevEndPositionFromBeginning, runningHitCount);

        // Save the new results
        prevLine = line;
        prevLineNumber = lineNumber;
        prevStartPositionFromBeginning = startPositionFromBeginning;
        prevEndPositionFromBeginning = endPositionFromBeginning;

        // Just one result now
        runningHitCount = 1;
    }
    else {
        runningHitCount += hitCount;
    }
}

void SearchResultsCollector::completeSearch()
{
    // There may be a result that was not passed along yet
    if (runningHitCount > 0) {
        child->newResultsEntry(prevLine, prevLineNumber, prevStartPositionFromBeginning, prevEndPositionFromBeginning, runningHitCount);
    }
    runningHitCount = 0;

    child->completeSearch();
}
