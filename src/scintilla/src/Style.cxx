// Scintilla source code edit control
/** @file Style.cxx
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdexcept>
#include <string_view>
#include <vector>
#include <optional>
#include <memory>

#include "ScintillaTypes.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

#include "Style.h"

using namespace Scintilla;
using namespace Scintilla::Internal;

bool FontSpecification::operator==(const FontSpecification &other) const noexcept {
	return fontName == other.fontName &&
	       weight == other.weight &&
	       italic == other.italic &&
	       size == other.size &&
	       characterSet == other.characterSet &&
	       extraFontFlag == other.extraFontFlag;
}

bool FontSpecification::operator<(const FontSpecification &other) const noexcept {
	if (fontName != other.fontName)
		return fontName < other.fontName;
	if (weight != other.weight)
		return weight < other.weight;
	if (italic != other.italic)
		return !italic;
	if (size != other.size)
		return size < other.size;
	if (characterSet != other.characterSet)
		return characterSet < other.characterSet;
	if (extraFontFlag != other.extraFontFlag)
		return extraFontFlag < other.extraFontFlag;
	return false;
}

FontMeasurements::FontMeasurements() noexcept {
	ClearMeasurements();
}

void FontMeasurements::ClearMeasurements() noexcept {
	ascent = 1;
	descent = 1;
	capitalHeight = 1;
	aveCharWidth = 1;
	spaceWidth = 1;
	sizeZoomed = 2;
}

Style::Style() : FontSpecification() {
	Clear(ColourRGBA(0, 0, 0), ColourRGBA(0xff, 0xff, 0xff),
	      Platform::DefaultFontSize() * FontSizeMultiplier, nullptr, CharacterSet::Default,
	      FontWeight::Normal, false, false, false, CaseForce::mixed, true, true, false);
}

Style::Style(const Style &source) noexcept : FontSpecification(), FontMeasurements() {
	Clear(ColourRGBA(0, 0, 0), ColourRGBA(0xff, 0xff, 0xff),
	      0, nullptr, CharacterSet::Ansi,
	      FontWeight::Normal, false, false, false, CaseForce::mixed, true, true, false);
	fore = source.fore;
	back = source.back;
	characterSet = source.characterSet;
	weight = source.weight;
	italic = source.italic;
	size = source.size;
	fontName = source.fontName;
	eolFilled = source.eolFilled;
	underline = source.underline;
	caseForce = source.caseForce;
	visible = source.visible;
	changeable = source.changeable;
	hotspot = source.hotspot;
}

Style::~Style() = default;

Style &Style::operator=(const Style &source) noexcept {
	if (this == &source)
		return * this;
	Clear(ColourRGBA(0, 0, 0), ColourRGBA(0xff, 0xff, 0xff),
	      0, nullptr, CharacterSet::Default,
	      FontWeight::Normal, false, false, false, CaseForce::mixed, true, true, false);
	fore = source.fore;
	back = source.back;
	characterSet = source.characterSet;
	weight = source.weight;
	italic = source.italic;
	size = source.size;
	fontName = source.fontName;
	eolFilled = source.eolFilled;
	underline = source.underline;
	caseForce = source.caseForce;
	visible = source.visible;
	changeable = source.changeable;
	return *this;
}

void Style::Clear(ColourRGBA fore_, ColourRGBA back_, int size_,
        const char *fontName_, CharacterSet characterSet_,
        FontWeight weight_, bool italic_, bool eolFilled_,
        bool underline_, CaseForce caseForce_,
        bool visible_, bool changeable_, bool hotspot_) noexcept {
	fore = fore_;
	back = back_;
	characterSet = characterSet_;
	weight = weight_;
	italic = italic_;
	size = size_;
	fontName = fontName_;
	eolFilled = eolFilled_;
	underline = underline_;
	caseForce = caseForce_;
	visible = visible_;
	changeable = changeable_;
	hotspot = hotspot_;
	font.reset();
	FontMeasurements::ClearMeasurements();
}

void Style::ClearTo(const Style &source) noexcept {
	Clear(
	    source.fore,
	    source.back,
	    source.size,
	    source.fontName,
	    source.characterSet,
	    source.weight,
	    source.italic,
	    source.eolFilled,
	    source.underline,
	    source.caseForce,
	    source.visible,
	    source.changeable,
	    source.hotspot);
}

void Style::Copy(std::shared_ptr<Font> font_, const FontMeasurements &fm_) noexcept {
	font = std::move(font_);
	(FontMeasurements &)(*this) = fm_;
}
