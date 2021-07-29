// Scintilla source code edit control
/** @file Style.h
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef STYLE_H
#define STYLE_H

namespace Scintilla::Internal {

struct FontSpecification {
	const char *fontName;
	Scintilla::FontWeight weight;
	bool italic;
	int size;
	Scintilla::CharacterSet characterSet;
	Scintilla::FontQuality extraFontFlag;
	FontSpecification() noexcept :
		fontName(nullptr),
		weight(Scintilla::FontWeight::Normal),
		italic(false),
		size(10 * Scintilla::FontSizeMultiplier),
		characterSet(Scintilla::CharacterSet::Ansi),
		extraFontFlag(Scintilla::FontQuality::QualityDefault) {
	}
	bool operator==(const FontSpecification &other) const noexcept;
	bool operator<(const FontSpecification &other) const noexcept;
};

struct FontMeasurements {
	unsigned int ascent;
	unsigned int descent;
	XYPOSITION capitalHeight;	// Top of capital letter to baseline: ascent - internal leading
	XYPOSITION aveCharWidth;
	XYPOSITION spaceWidth;
	int sizeZoomed;
	FontMeasurements() noexcept;
	void ClearMeasurements() noexcept;
};

/**
 */
class Style : public FontSpecification, public FontMeasurements {
public:
	ColourRGBA fore;
	ColourRGBA back;
	bool eolFilled;
	bool underline;
	enum class CaseForce {mixed, upper, lower, camel};
	CaseForce caseForce;
	bool visible;
	bool changeable;
	bool hotspot;

	std::shared_ptr<Font> font;

	Style();
	Style(const Style &source) noexcept;
	Style(Style &&) noexcept = default;
	~Style();
	Style &operator=(const Style &source) noexcept;
	Style &operator=(Style &&) = delete;
	void Clear(ColourRGBA fore_, ColourRGBA back_,
	           int size_,
	           const char *fontName_, Scintilla::CharacterSet characterSet_,
	           Scintilla::FontWeight weight_, bool italic_, bool eolFilled_,
	           bool underline_, CaseForce caseForce_,
	           bool visible_, bool changeable_, bool hotspot_) noexcept;
	void ClearTo(const Style &source) noexcept;
	void Copy(std::shared_ptr<Font> font_, const FontMeasurements &fm_) noexcept;
	bool IsProtected() const noexcept { return !(changeable && visible);}
};

}

#endif
