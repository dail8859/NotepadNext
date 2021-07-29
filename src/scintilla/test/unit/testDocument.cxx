/** @file testDocument.cxx
 ** Unit Tests for Scintilla internal data structures
 **/

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <memory>

#include "ScintillaTypes.h"

#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"

#include "CharacterCategoryMap.h"
#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "CellBuffer.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"

#include "catch.hpp"

using namespace Scintilla;
using namespace Scintilla::Internal;

// Test Document.

TEST_CASE("Document") {

	const char sText[] = "Scintilla";
	const Sci::Position sLength = static_cast<Sci::Position>(strlen(sText));

	Document doc(DocumentOption::Default);

	SECTION("InsertOneLine") {
		const Sci::Position length = doc.InsertString(0, sText, sLength);
		REQUIRE(sLength == doc.Length());
		REQUIRE(length == sLength);
		REQUIRE(1 == doc.LinesTotal());
		REQUIRE(0 == doc.LineStart(0));
		REQUIRE(0 == doc.LineFromPosition(0));
		REQUIRE(sLength == doc.LineStart(1));
		REQUIRE(0 == doc.LineFromPosition(static_cast<int>(sLength)));
		REQUIRE(doc.CanUndo());
		REQUIRE(!doc.CanRedo());
	}

}
