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


#ifndef QREGEXSEARCH_H
#define QREGEXSEARCH_H

#include <QRegularExpressionMatch>

#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

#include "Sci_Position.h"
#include "Position.h"
#include "CharacterCategory.h"
#include "Scintilla.h"
#include "Platform.h"
#include "ILexer.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "CellBuffer.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "ILoader.h"
#include "Document.h"

class QRegexSearch : public Scintilla::RegexSearchBase
{
public:
    QRegexSearch();

    Sci::Position FindText(Scintilla::Document *doc, Sci::Position minPos, Sci::Position maxPos, const char *s, bool caseSensitive, bool word, bool wordStart, int flags, Sci::Position *length) override;
    const char *SubstituteByPosition(Scintilla::Document *doc, const char *text, Sci::Position *length) override;

private:
    QRegularExpressionMatch match;
    QByteArray *substituted = Q_NULLPTR;
};

#endif // QREGEXSEARCH_H
