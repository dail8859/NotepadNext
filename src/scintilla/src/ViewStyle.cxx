// Scintilla source code edit control
/** @file ViewStyle.cxx
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cassert>
#include <cstring>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <memory>

#include "ScintillaTypes.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

#include "Position.h"
#include "UniqueString.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"

using namespace Scintilla;
using namespace Scintilla::Internal;

MarginStyle::MarginStyle(MarginType style_, int width_, int mask_) noexcept :
	style(style_), width(width_), mask(mask_), sensitive(false), cursor(CursorShape::ReverseArrow) {
}

bool MarginStyle::ShowsFolding() const noexcept {
	return (mask & MaskFolders) != 0;
}

FontRealised::FontRealised() noexcept = default;

FontRealised::~FontRealised() = default;

void FontRealised::Realise(Surface &surface, int zoomLevel, Technology technology, const FontSpecification &fs, const char *localeName) {
	PLATFORM_ASSERT(fs.fontName);
	sizeZoomed = fs.size + zoomLevel * FontSizeMultiplier;
	if (sizeZoomed <= 2 * FontSizeMultiplier)	// Hangs if sizeZoomed <= 1
		sizeZoomed = 2 * FontSizeMultiplier;

	const float deviceHeight = static_cast<float>(surface.DeviceHeightFont(sizeZoomed));
	const FontParameters fp(fs.fontName, deviceHeight / FontSizeMultiplier, fs.weight,
		fs.italic, fs.extraFontFlag, technology, fs.characterSet, localeName);
	font = Font::Allocate(fp);

	ascent = static_cast<unsigned int>(surface.Ascent(font.get()));
	descent = static_cast<unsigned int>(surface.Descent(font.get()));
	capitalHeight = surface.Ascent(font.get()) - surface.InternalLeading(font.get());
	aveCharWidth = surface.AverageCharWidth(font.get());
	spaceWidth = surface.WidthText(font.get(), " ");
}

ViewStyle::ViewStyle() : markers(MarkerMax + 1), indicators(static_cast<size_t>(IndicatorNumbers::Max) + 1) {
	Init();
}

// Copy constructor only called when printing copies the screen ViewStyle so it can be
// modified for printing styles.
ViewStyle::ViewStyle(const ViewStyle &source) : markers(MarkerMax + 1), indicators(static_cast<size_t>(IndicatorNumbers::Max) + 1) {
	Init(source.styles.size());
	styles = source.styles;
	for (size_t sty=0; sty<source.styles.size(); sty++) {
		// Can't just copy fontName as its lifetime is relative to its owning ViewStyle
		styles[sty].fontName = fontNames.Save(source.styles[sty].fontName);
	}
	nextExtendedStyle = source.nextExtendedStyle;
	markers = source.markers;
	CalcLargestMarkerHeight();

	indicators = source.indicators;

	indicatorsDynamic = source.indicatorsDynamic;
	indicatorsSetFore = source.indicatorsSetFore;

	selection = source.selection;

	foldmarginColour = source.foldmarginColour;
	foldmarginHighlightColour = source.foldmarginHighlightColour;

	hotspotUnderline = source.hotspotUnderline;

	controlCharSymbol = source.controlCharSymbol;
	controlCharWidth = source.controlCharWidth;
	selbar = source.selbar;
	selbarlight = source.selbarlight;
	caret = source.caret;
	caretLine = source.caretLine;
	someStylesProtected = false;
	someStylesForceCase = false;
	leftMarginWidth = source.leftMarginWidth;
	rightMarginWidth = source.rightMarginWidth;
	ms = source.ms;
	maskInLine = source.maskInLine;
	maskDrawInText = source.maskDrawInText;
	fixedColumnWidth = source.fixedColumnWidth;
	marginInside = source.marginInside;
	textStart = source.textStart;
	zoomLevel = source.zoomLevel;
	viewWhitespace = source.viewWhitespace;
	tabDrawMode = source.tabDrawMode;
	whitespaceSize = source.whitespaceSize;
	viewIndentationGuides = source.viewIndentationGuides;
	viewEOL = source.viewEOL;
	extraFontFlag = source.extraFontFlag;
	extraAscent = source.extraAscent;
	extraDescent = source.extraDescent;
	marginStyleOffset = source.marginStyleOffset;
	annotationVisible = source.annotationVisible;
	annotationStyleOffset = source.annotationStyleOffset;
	eolAnnotationVisible = source.eolAnnotationVisible;
	eolAnnotationStyleOffset = source.eolAnnotationStyleOffset;
	braceHighlightIndicatorSet = source.braceHighlightIndicatorSet;
	braceHighlightIndicator = source.braceHighlightIndicator;
	braceBadLightIndicatorSet = source.braceBadLightIndicatorSet;
	braceBadLightIndicator = source.braceBadLightIndicator;

	edgeState = source.edgeState;
	theEdge = source.theEdge;
	theMultiEdge = source.theMultiEdge;

	marginNumberPadding = source.marginNumberPadding;
	ctrlCharPadding = source.ctrlCharPadding;
	lastSegItalicsOffset = source.lastSegItalicsOffset;

	wrap = source.wrap;

	localeName = source.localeName;
}

ViewStyle::~ViewStyle() {
	styles.clear();
	fonts.clear();
}

void ViewStyle::CalculateMarginWidthAndMask() noexcept {
	fixedColumnWidth = marginInside ? leftMarginWidth : 0;
	maskInLine = 0xffffffff;
	int maskDefinedMarkers = 0;
	for (const MarginStyle &m : ms) {
		fixedColumnWidth += m.width;
		if (m.width > 0)
			maskInLine &= ~m.mask;
		maskDefinedMarkers |= m.mask;
	}
	maskDrawInText = 0;
	for (int markBit = 0; markBit < 32; markBit++) {
		const int maskBit = 1U << markBit;
		switch (markers[markBit].markType) {
		case MarkerSymbol::Empty:
			maskInLine &= ~maskBit;
			break;
		case MarkerSymbol::Background:
		case MarkerSymbol::Underline:
			maskInLine &= ~maskBit;
			maskDrawInText |= maskDefinedMarkers & maskBit;
			break;
		default:	// Other marker types do not affect the masks
			break;
		}
	}
}

void ViewStyle::Init(size_t stylesSize_) {
	AllocStyles(stylesSize_);
	nextExtendedStyle = 256;
	fontNames.Clear();
	ResetDefaultStyle();

	// There are no image markers by default, so no need for calling CalcLargestMarkerHeight()
	largestMarkerHeight = 0;

	indicators[0] = Indicator(IndicatorStyle::Squiggle, ColourRGBA(0, 0x7f, 0));
	indicators[1] = Indicator(IndicatorStyle::TT, ColourRGBA(0, 0, 0xff));
	indicators[2] = Indicator(IndicatorStyle::Plain, ColourRGBA(0xff, 0, 0));

	technology = Technology::Default;
	indicatorsDynamic = false;
	indicatorsSetFore = false;
	lineHeight = 1;
	lineOverlap = 0;
	maxAscent = 1;
	maxDescent = 1;
	aveCharWidth = 8;
	spaceWidth = 8;
	tabWidth = spaceWidth * 8;

	// Default is for no selection foregrounds
	elementColours.erase(Element::SelectionText);
	elementColours.erase(Element::SelectionAdditionalText);
	elementColours.erase(Element::SelectionSecondaryText);
	elementColours.erase(Element::SelectionInactiveText);
	// Shades of grey for selection backgrounds
	elementBaseColours[Element::SelectionBack] = ColourRGBA(0xc0, 0xc0, 0xc0, 0xff);
	elementBaseColours[Element::SelectionAdditionalBack] = ColourRGBA(0xd7, 0xd7, 0xd7, 0xff);
	elementBaseColours[Element::SelectionSecondaryBack] = ColourRGBA(0xb0, 0xb0, 0xb0, 0xff);
	elementBaseColours[Element::SelectionInactiveBack] = ColourRGBA(0x80, 0x80, 0x80, 0x3f);
	elementAllowsTranslucent.insert({
		Element::SelectionText,
		Element::SelectionBack,
		Element::SelectionAdditionalText,
		Element::SelectionAdditionalBack,
		Element::SelectionSecondaryText,
		Element::SelectionSecondaryBack,
		Element::SelectionInactiveText,
		Element::SelectionBack,
		Element::SelectionInactiveBack,
	});
	selection.layer = Layer::Base;
	selection.eolFilled = false;

	foldmarginColour.reset();
	foldmarginHighlightColour.reset();

	controlCharSymbol = 0;	/* Draw the control characters */
	controlCharWidth = 0;
	selbar = Platform::Chrome();
	selbarlight = Platform::ChromeHighlight();
	styles[StyleLineNumber].fore = ColourRGBA(0, 0, 0);
	styles[StyleLineNumber].back = Platform::Chrome();

	elementBaseColours[Element::Caret] = ColourRGBA(0, 0, 0);
	elementBaseColours[Element::CaretAdditional] = ColourRGBA(0x7f, 0x7f, 0x7f);
	elementAllowsTranslucent.insert({
		Element::Caret,
		Element::CaretAdditional,
	});
	caret.style = CaretStyle::Line;
	caret.width = 1;

	elementColours.erase(Element::CaretLineBack);
	elementAllowsTranslucent.insert(Element::CaretLineBack);
	caretLine.alwaysShow = false;
	caretLine.layer = Layer::Base;
	caretLine.frame = 0;

	someStylesProtected = false;
	someStylesForceCase = false;

	hotspotUnderline = true;
	elementColours.erase(Element::HotSpotActive);
	elementAllowsTranslucent.insert(Element::HotSpotActive);

	leftMarginWidth = 1;
	rightMarginWidth = 1;
	ms.resize(MaxMargin + 1);
	ms[0] = MarginStyle(MarginType::Number);
	ms[1] = MarginStyle(MarginType::Symbol, 16, ~MaskFolders);
	ms[2] = MarginStyle(MarginType::Symbol);
	marginInside = true;
	CalculateMarginWidthAndMask();
	textStart = marginInside ? fixedColumnWidth : leftMarginWidth;
	zoomLevel = 0;
	viewWhitespace = WhiteSpace::Invisible;
	tabDrawMode = TabDrawMode::LongArrow;
	whitespaceSize = 1;
	elementColours.erase(Element::WhiteSpace);
	elementAllowsTranslucent.insert(Element::WhiteSpace);

	viewIndentationGuides = IndentView::None;
	viewEOL = false;
	extraFontFlag = FontQuality::QualityDefault;
	extraAscent = 0;
	extraDescent = 0;
	marginStyleOffset = 0;
	annotationVisible = AnnotationVisible::Hidden;
	annotationStyleOffset = 0;
	eolAnnotationVisible = EOLAnnotationVisible::Hidden;
	eolAnnotationStyleOffset = 0;
	braceHighlightIndicatorSet = false;
	braceHighlightIndicator = 0;
	braceBadLightIndicatorSet = false;
	braceBadLightIndicator = 0;

	edgeState = EdgeVisualStyle::None;
	theEdge = EdgeProperties(0, ColourRGBA(0xc0, 0xc0, 0xc0));

	marginNumberPadding = 3;
	ctrlCharPadding = 3; // +3 For a blank on front and rounded edge each side
	lastSegItalicsOffset = 2;

	wrap.state = Wrap::None;
	wrap.visualFlags = WrapVisualFlag::None;
	wrap.visualFlagsLocation = WrapVisualLocation::Default;
	wrap.visualStartIndent = 0;
	wrap.indentMode = WrapIndentMode::Fixed;

	localeName = localeNameDefault;
}

void ViewStyle::Refresh(Surface &surface, int tabInChars) {
	fonts.clear();

	selbar = Platform::Chrome();
	selbarlight = Platform::ChromeHighlight();

	// Apply the extra font flag which controls text drawing quality to each style.
	for (Style &style : styles) {
		style.extraFontFlag = extraFontFlag;
	}

	// Create a FontRealised object for each unique font in the styles.
	CreateAndAddFont(styles[StyleDefault]);
	for (const Style &style : styles) {
		CreateAndAddFont(style);
	}

	// Ask platform to allocate each unique font.
	for (const std::pair<const FontSpecification, std::unique_ptr<FontRealised>> &font : fonts) {
		font.second->Realise(surface, zoomLevel, technology, font.first, localeName.c_str());
	}

	// Set the platform font handle and measurements for each style.
	for (Style &style : styles) {
		const FontRealised *fr = Find(style);
		style.Copy(fr->font, *fr);
	}

	indicatorsDynamic = std::any_of(indicators.cbegin(), indicators.cend(),
		[](const Indicator &indicator) noexcept { return indicator.IsDynamic(); });

	indicatorsSetFore = std::any_of(indicators.cbegin(), indicators.cend(),
		[](const Indicator &indicator) noexcept { return indicator.OverridesTextFore(); });

	maxAscent = 1;
	maxDescent = 1;
	FindMaxAscentDescent();
	maxAscent += extraAscent;
	maxDescent += extraDescent;
	lineHeight = maxAscent + maxDescent;
	lineOverlap = lineHeight / 10;
	if (lineOverlap < 2)
		lineOverlap = 2;
	if (lineOverlap > lineHeight)
		lineOverlap = lineHeight;

	someStylesProtected = std::any_of(styles.cbegin(), styles.cend(),
		[](const Style &style) noexcept { return style.IsProtected(); });

	someStylesForceCase = std::any_of(styles.cbegin(), styles.cend(),
		[](const Style &style) noexcept { return style.caseForce != Style::CaseForce::mixed; });

	aveCharWidth = styles[StyleDefault].aveCharWidth;
	spaceWidth = styles[StyleDefault].spaceWidth;
	tabWidth = spaceWidth * tabInChars;

	controlCharWidth = 0.0;
	if (controlCharSymbol >= 32) {
		const char cc[2] = { static_cast<char>(controlCharSymbol), '\0' };
		controlCharWidth = surface.WidthText(styles[StyleControlChar].font.get(), cc);
	}

	CalculateMarginWidthAndMask();
	textStart = marginInside ? fixedColumnWidth : leftMarginWidth;
}

void ViewStyle::ReleaseAllExtendedStyles() noexcept {
	nextExtendedStyle = 256;
}

int ViewStyle::AllocateExtendedStyles(int numberStyles) {
	const int startRange = nextExtendedStyle;
	nextExtendedStyle += numberStyles;
	EnsureStyle(nextExtendedStyle);
	for (int i=startRange; i<nextExtendedStyle; i++) {
		styles[i].ClearTo(styles[StyleDefault]);
	}
	return startRange;
}

void ViewStyle::EnsureStyle(size_t index) {
	if (index >= styles.size()) {
		AllocStyles(index+1);
	}
}

void ViewStyle::ResetDefaultStyle() {
	styles[StyleDefault].Clear(ColourRGBA(0,0,0),
	        ColourRGBA(0xff,0xff,0xff),
	        Platform::DefaultFontSize() * FontSizeMultiplier, fontNames.Save(Platform::DefaultFont()),
	        CharacterSet::Default,
	        FontWeight::Normal, false, false, false, Style::CaseForce::mixed, true, true, false);
}

void ViewStyle::ClearStyles() {
	// Reset all styles to be like the default style
	for (size_t i=0; i<styles.size(); i++) {
		if (i != StyleDefault) {
			styles[i].ClearTo(styles[StyleDefault]);
		}
	}
	styles[StyleLineNumber].back = Platform::Chrome();

	// Set call tip fore/back to match the values previously set for call tips
	styles[StyleCallTip].back = ColourRGBA(0xff, 0xff, 0xff);
	styles[StyleCallTip].fore = ColourRGBA(0x80, 0x80, 0x80);
}

void ViewStyle::SetStyleFontName(int styleIndex, const char *name) {
	styles[styleIndex].fontName = fontNames.Save(name);
}

void ViewStyle::SetFontLocaleName(const char *name) {
	localeName = name;
}

bool ViewStyle::ProtectionActive() const noexcept {
	return someStylesProtected;
}

int ViewStyle::ExternalMarginWidth() const noexcept {
	return marginInside ? 0 : fixedColumnWidth;
}

int ViewStyle::MarginFromLocation(Point pt) const noexcept {
	int margin = -1;
	int x = marginInside ? 0 : -fixedColumnWidth;
	for (size_t i = 0; i < ms.size(); i++) {
		if ((pt.x >= x) && (pt.x < x + ms[i].width))
			margin = static_cast<int>(i);
		x += ms[i].width;
	}
	return margin;
}

bool ViewStyle::ValidStyle(size_t styleIndex) const noexcept {
	return styleIndex < styles.size();
}

void ViewStyle::CalcLargestMarkerHeight() noexcept {
	largestMarkerHeight = 0;
	for (const LineMarker &marker : markers) {
		switch (marker.markType) {
		case MarkerSymbol::Pixmap:
			if (marker.pxpm && marker.pxpm->GetHeight() > largestMarkerHeight)
				largestMarkerHeight = marker.pxpm->GetHeight();
			break;
		case MarkerSymbol::RgbaImage:
			if (marker.image && marker.image->GetHeight() > largestMarkerHeight)
				largestMarkerHeight = marker.image->GetHeight();
			break;
		default:	// Only images have their own natural heights
			break;
		}
	}
}

int ViewStyle::GetFrameWidth() const noexcept {
	return std::clamp(caretLine.frame, 1, lineHeight / 3);
}

bool ViewStyle::IsLineFrameOpaque(bool caretActive, bool lineContainsCaret) const {
	return caretLine.frame && (caretActive || caretLine.alwaysShow) &&
		ElementColour(Element::CaretLineBack) &&
		(caretLine.layer == Layer::Base) && lineContainsCaret;
}

// See if something overrides the line background colour:  Either if caret is on the line
// and background colour is set for that, or if a marker is defined that forces its background
// colour onto the line, or if a marker is defined but has no selection margin in which to
// display itself (as long as it's not an MarkerSymbol::Empty marker).  These are checked in order
// with the earlier taking precedence.  When multiple markers cause background override,
// the colour for the highest numbered one is used.
std::optional<ColourRGBA> ViewStyle::Background(int marksOfLine, bool caretActive, bool lineContainsCaret) const {
	std::optional<ColourRGBA> background;
	if (!caretLine.frame && (caretActive || caretLine.alwaysShow) &&
		(caretLine.layer == Layer::Base) && lineContainsCaret) {
		background = ElementColour(Element::CaretLineBack);
	}
	if (!background && marksOfLine) {
		int marks = marksOfLine;
		for (int markBit = 0; (markBit < 32) && marks; markBit++) {
			if ((marks & 1) && (markers[markBit].markType == MarkerSymbol::Background) &&
				(markers[markBit].layer == Layer::Base)) {
				background = markers[markBit].back;
			}
			marks >>= 1;
		}
	}
	if (!background && maskInLine) {
		int marksMasked = marksOfLine & maskInLine;
		if (marksMasked) {
			for (int markBit = 0; (markBit < 32) && marksMasked; markBit++) {
				if ((marksMasked & 1) &&
					(markers[markBit].layer == Layer::Base)) {
					background = markers[markBit].back;
				}
				marksMasked >>= 1;
			}
		}
	}
	if (background) {
		return background->Opaque();
	} else {
		return {};
	}
}

bool ViewStyle::SelectionBackgroundDrawn() const noexcept {
	return selection.layer == Layer::Base;
}

bool ViewStyle::SelectionTextDrawn() const {
	return
		ElementIsSet(Element::SelectionText) ||
		ElementIsSet(Element::SelectionAdditionalText) ||
		ElementIsSet(Element::SelectionSecondaryText) ||
		ElementIsSet(Element::SelectionInactiveText);
}

bool ViewStyle::WhitespaceBackgroundDrawn() const {
	return (viewWhitespace != WhiteSpace::Invisible) && (ElementIsSet(Element::WhiteSpaceBack));
}

bool ViewStyle::WhiteSpaceVisible(bool inIndent) const noexcept {
	return (!inIndent && viewWhitespace == WhiteSpace::VisibleAfterIndent) ||
		(inIndent && viewWhitespace == WhiteSpace::VisibleOnlyInIndent) ||
		viewWhitespace == WhiteSpace::VisibleAlways;
}

ColourRGBA ViewStyle::WrapColour() const {
	return ElementColour(Element::WhiteSpace).value_or(styles[StyleDefault].fore);
}

// Insert new edge in sorted order.
void ViewStyle::AddMultiEdge(uptr_t wParam, sptr_t lParam) {
	const int column = static_cast<int>(wParam);
	theMultiEdge.insert(
		std::upper_bound(theMultiEdge.begin(), theMultiEdge.end(), column,
			[](const EdgeProperties &a, const EdgeProperties &b) {
				return a.column < b.column;
			}),
		EdgeProperties(column, lParam));
}

std::optional<ColourRGBA> ViewStyle::ElementColour(Element element) const {
	ElementMap::const_iterator search = elementColours.find(element);
	if (search != elementColours.end()) {
		if (search->second.has_value()) {
			return search->second;
		}
	}
	ElementMap::const_iterator searchBase = elementBaseColours.find(element);
	if (searchBase != elementBaseColours.end()) {
		if (searchBase->second.has_value()) {
			return searchBase->second;
		}
	}
	return {};
}

bool ViewStyle::ElementAllowsTranslucent(Element element) const {
	return elementAllowsTranslucent.count(element) > 0;
}

bool ViewStyle::ResetElement(Element element) {
	ElementMap::const_iterator search = elementColours.find(element);
	const bool changed = (search != elementColours.end()) && (search->second.has_value());
	elementColours.erase(element);
	return changed;
}

bool ViewStyle::SetElementColour(Element element, ColourRGBA colour) {
	ElementMap::const_iterator search = elementColours.find(element);
	const bool changed =
		(search == elementColours.end()) ||
		(search->second.has_value() && !(*search->second == colour));
	elementColours[element] = colour;
	return changed;
}

bool ViewStyle::SetElementColourOptional(Element element, uptr_t wParam, sptr_t lParam) {
	if (wParam) {
		return SetElementColour(element, ColourRGBA::FromIpRGB(lParam));
	} else {
		return ResetElement(element);
	}
}

void ViewStyle::SetElementRGB(Element element, int rgb) {
	const ColourRGBA current = ElementColour(element).value_or(ColourRGBA(0, 0, 0, 0));
	elementColours[element] = ColourRGBA(ColourRGBA(rgb), current.GetAlpha());
}

void ViewStyle::SetElementAlpha(Element element, int alpha) {
	const ColourRGBA current = ElementColour(element).value_or(ColourRGBA(0, 0, 0, 0));
	elementColours[element] = ColourRGBA(current, std::min(alpha, 0xff));
}

bool ViewStyle::ElementIsSet(Element element) const {
	ElementMap::const_iterator search = elementColours.find(element);
	if (search != elementColours.end()) {
		return search->second.has_value();
	}
	return false;
}

bool ViewStyle::SetElementBase(Element element, ColourRGBA colour) {
	ElementMap::const_iterator search = elementBaseColours.find(element);
	const bool changed =
		(search == elementBaseColours.end()) ||
		(search->second.has_value() && !(*search->second == colour));
	elementBaseColours[element] = colour;
	return changed;
}

bool ViewStyle::SetWrapState(Wrap wrapState_) noexcept {
	const bool changed = wrap.state != wrapState_;
	wrap.state = wrapState_;
	return changed;
}

bool ViewStyle::SetWrapVisualFlags(WrapVisualFlag wrapVisualFlags_) noexcept {
	const bool changed = wrap.visualFlags != wrapVisualFlags_;
	wrap.visualFlags = wrapVisualFlags_;
	return changed;
}

bool ViewStyle::SetWrapVisualFlagsLocation(WrapVisualLocation wrapVisualFlagsLocation_) noexcept {
	const bool changed = wrap.visualFlagsLocation != wrapVisualFlagsLocation_;
	wrap.visualFlagsLocation = wrapVisualFlagsLocation_;
	return changed;
}

bool ViewStyle::SetWrapVisualStartIndent(int wrapVisualStartIndent_) noexcept {
	const bool changed = wrap.visualStartIndent != wrapVisualStartIndent_;
	wrap.visualStartIndent = wrapVisualStartIndent_;
	return changed;
}

bool ViewStyle::SetWrapIndentMode(WrapIndentMode wrapIndentMode_) noexcept {
	const bool changed = wrap.indentMode != wrapIndentMode_;
	wrap.indentMode = wrapIndentMode_;
	return changed;
}

bool ViewStyle::IsBlockCaretStyle() const noexcept {
	return ((caret.style & CaretStyle::InsMask) == CaretStyle::Block) ||
		FlagSet(caret.style, CaretStyle::OverstrikeBlock);
}

bool ViewStyle::IsCaretVisible() const noexcept {
	return caret.width > 0 && caret.style != CaretStyle::Invisible;
}

bool ViewStyle::DrawCaretInsideSelection(bool inOverstrike, bool imeCaretBlockOverride) const noexcept {
	if (FlagSet(caret.style, CaretStyle::BlockAfter))
		return false;
	return ((caret.style & CaretStyle::InsMask) == CaretStyle::Block) ||
		(inOverstrike && FlagSet(caret.style, CaretStyle::OverstrikeBlock)) ||
		imeCaretBlockOverride;
}

ViewStyle::CaretShape ViewStyle::CaretShapeForMode(bool inOverstrike) const noexcept {
	if (inOverstrike) {
		return (FlagSet(caret.style, CaretStyle::OverstrikeBlock)) ? CaretShape::block : CaretShape::bar;
	}

	const CaretStyle caretStyle = caret.style & CaretStyle::InsMask;
	return (caretStyle <= CaretStyle::Block) ? static_cast<CaretShape>(caretStyle) : CaretShape::line;
}

void ViewStyle::AllocStyles(size_t sizeNew) {
	size_t i=styles.size();
	styles.resize(sizeNew);
	if (styles.size() > StyleDefault) {
		for (; i<sizeNew; i++) {
			if (i != StyleDefault) {
				styles[i].ClearTo(styles[StyleDefault]);
			}
		}
	}
}

void ViewStyle::CreateAndAddFont(const FontSpecification &fs) {
	if (fs.fontName) {
		FontMap::iterator it = fonts.find(fs);
		if (it == fonts.end()) {
			fonts[fs] = std::make_unique<FontRealised>();
		}
	}
}

FontRealised *ViewStyle::Find(const FontSpecification &fs) {
	if (!fs.fontName)	// Invalid specification so return arbitrary object
		return fonts.begin()->second.get();
	FontMap::iterator it = fonts.find(fs);
	if (it != fonts.end()) {
		// Should always reach here since map was just set for all styles
		return it->second.get();
	}
	return nullptr;
}

void ViewStyle::FindMaxAscentDescent() {
	for (const auto &font : fonts) {
		if (maxAscent < font.second->ascent)
			maxAscent = font.second->ascent;
		if (maxDescent < font.second->descent)
			maxDescent = font.second->descent;
	}
}
