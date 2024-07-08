// Scintilla source code edit control
/** @file ScintillaTypes.h
 ** Types used to communicate with Scintilla.
 **/
// Copyright 1998-2019 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

/* Most of this file is automatically generated from the Scintilla.iface interface definition
 * file which contains any comments about the definitions. ScintillaAPIFacer.py does the generation. */

#ifndef SCINTILLATYPES_H
#define SCINTILLATYPES_H

namespace Scintilla {

// Enumerations
//++Autogenerated -- start of section automatically generated from Scintilla.iface

enum class WhiteSpace {
	Invisible = 0,
	VisibleAlways = 1,
	VisibleAfterIndent = 2,
	VisibleOnlyInIndent = 3,
};

enum class TabDrawMode {
	LongArrow = 0,
	StrikeOut = 1,
};

enum class EndOfLine {
	CrLf = 0,
	Cr = 1,
	Lf = 2,
};

enum class IMEInteraction {
	Windowed = 0,
	Inline = 1,
};

enum class Alpha {
	Transparent = 0,
	Opaque = 255,
	NoAlpha = 256,
};

enum class CursorShape {
	Normal = -1,
	Arrow = 2,
	Wait = 4,
	ReverseArrow = 7,
};

enum class MarkerSymbol {
	Circle = 0,
	RoundRect = 1,
	Arrow = 2,
	SmallRect = 3,
	ShortArrow = 4,
	Empty = 5,
	ArrowDown = 6,
	Minus = 7,
	Plus = 8,
	VLine = 9,
	LCorner = 10,
	TCorner = 11,
	BoxPlus = 12,
	BoxPlusConnected = 13,
	BoxMinus = 14,
	BoxMinusConnected = 15,
	LCornerCurve = 16,
	TCornerCurve = 17,
	CirclePlus = 18,
	CirclePlusConnected = 19,
	CircleMinus = 20,
	CircleMinusConnected = 21,
	Background = 22,
	DotDotDot = 23,
	Arrows = 24,
	Pixmap = 25,
	FullRect = 26,
	LeftRect = 27,
	Available = 28,
	Underline = 29,
	RgbaImage = 30,
	Bookmark = 31,
	VerticalBookmark = 32,
	Bar = 33,
	Character = 10000,
};

enum class MarkerOutline {
	HistoryRevertedToOrigin = 21,
	HistorySaved = 22,
	HistoryModified = 23,
	HistoryRevertedToModified = 24,
	FolderEnd = 25,
	FolderOpenMid = 26,
	FolderMidTail = 27,
	FolderTail = 28,
	FolderSub = 29,
	Folder = 30,
	FolderOpen = 31,
};

enum class MarginType {
	Symbol = 0,
	Number = 1,
	Back = 2,
	Fore = 3,
	Text = 4,
	RText = 5,
	Colour = 6,
};

enum class StylesCommon {
	Default = 32,
	LineNumber = 33,
	BraceLight = 34,
	BraceBad = 35,
	ControlChar = 36,
	IndentGuide = 37,
	CallTip = 38,
	FoldDisplayText = 39,
	LastPredefined = 39,
	Max = 255,
};

enum class CharacterSet {
	Ansi = 0,
	Default = 1,
	Baltic = 186,
	ChineseBig5 = 136,
	EastEurope = 238,
	GB2312 = 134,
	Greek = 161,
	Hangul = 129,
	Mac = 77,
	Oem = 255,
	Russian = 204,
	Oem866 = 866,
	Cyrillic = 1251,
	ShiftJis = 128,
	Symbol = 2,
	Turkish = 162,
	Johab = 130,
	Hebrew = 177,
	Arabic = 178,
	Vietnamese = 163,
	Thai = 222,
	Iso8859_15 = 1000,
};

enum class CaseVisible {
	Mixed = 0,
	Upper = 1,
	Lower = 2,
	Camel = 3,
};

enum class FontWeight {
	Normal = 400,
	SemiBold = 600,
	Bold = 700,
};

enum class Element {
	List = 0,
	ListBack = 1,
	ListSelected = 2,
	ListSelectedBack = 3,
	SelectionText = 10,
	SelectionBack = 11,
	SelectionAdditionalText = 12,
	SelectionAdditionalBack = 13,
	SelectionSecondaryText = 14,
	SelectionSecondaryBack = 15,
	SelectionInactiveText = 16,
	SelectionInactiveBack = 17,
	SelectionInactiveAdditionalText = 18,
	SelectionInactiveAdditionalBack = 19,
	Caret = 40,
	CaretAdditional = 41,
	CaretLineBack = 50,
	WhiteSpace = 60,
	WhiteSpaceBack = 61,
	HotSpotActive = 70,
	HotSpotActiveBack = 71,
	FoldLine = 80,
	HiddenLine = 81,
};

enum class Layer {
	Base = 0,
	UnderText = 1,
	OverText = 2,
};

enum class IndicatorStyle {
	Plain = 0,
	Squiggle = 1,
	TT = 2,
	Diagonal = 3,
	Strike = 4,
	Hidden = 5,
	Box = 6,
	RoundBox = 7,
	StraightBox = 8,
	Dash = 9,
	Dots = 10,
	SquiggleLow = 11,
	DotBox = 12,
	SquigglePixmap = 13,
	CompositionThick = 14,
	CompositionThin = 15,
	FullBox = 16,
	TextFore = 17,
	Point = 18,
	PointCharacter = 19,
	Gradient = 20,
	GradientCentre = 21,
	PointTop = 22,
};

enum class IndicatorNumbers {
	Container = 8,
	Ime = 32,
	ImeMax = 35,
	HistoryRevertedToOriginInsertion = 36,
	HistoryRevertedToOriginDeletion = 37,
	HistorySavedInsertion = 38,
	HistorySavedDeletion = 39,
	HistoryModifiedInsertion = 40,
	HistoryModifiedDeletion = 41,
	HistoryRevertedToModifiedInsertion = 42,
	HistoryRevertedToModifiedDeletion = 43,
	Max = 43,
};

enum class IndicValue {
	Bit = 0x1000000,
	Mask = 0xFFFFFF,
};

enum class IndicFlag {
	None = 0,
	ValueFore = 1,
};

enum class AutoCompleteOption {
	Normal = 0,
	FixedSize = 1,
	SelectFirstItem = 2,
};

enum class IndentView {
	None = 0,
	Real = 1,
	LookForward = 2,
	LookBoth = 3,
};

enum class PrintOption {
	Normal = 0,
	InvertLight = 1,
	BlackOnWhite = 2,
	ColourOnWhite = 3,
	ColourOnWhiteDefaultBG = 4,
	ScreenColours = 5,
};

enum class FindOption {
	None = 0x0,
	WholeWord = 0x2,
	MatchCase = 0x4,
	WordStart = 0x00100000,
	RegExp = 0x00200000,
	Posix = 0x00400000,
	Cxx11RegEx = 0x00800000,
};

enum class ChangeHistoryOption {
	Disabled = 0,
	Enabled = 1,
	Markers = 2,
	Indicators = 4,
};

enum class FoldLevel {
	None = 0x0,
	Base = 0x400,
	WhiteFlag = 0x1000,
	HeaderFlag = 0x2000,
	NumberMask = 0x0FFF,
};

enum class FoldDisplayTextStyle {
	Hidden = 0,
	Standard = 1,
	Boxed = 2,
};

enum class FoldAction {
	Contract = 0,
	Expand = 1,
	Toggle = 2,
	ContractEveryLevel = 4,
};

enum class AutomaticFold {
	None = 0x0000,
	Show = 0x0001,
	Click = 0x0002,
	Change = 0x0004,
};

enum class FoldFlag {
	None = 0x0000,
	LineBeforeExpanded = 0x0002,
	LineBeforeContracted = 0x0004,
	LineAfterExpanded = 0x0008,
	LineAfterContracted = 0x0010,
	LevelNumbers = 0x0040,
	LineState = 0x0080,
};

enum class IdleStyling {
	None = 0,
	ToVisible = 1,
	AfterVisible = 2,
	All = 3,
};

enum class Wrap {
	None = 0,
	Word = 1,
	Char = 2,
	WhiteSpace = 3,
};

enum class WrapVisualFlag {
	None = 0x0000,
	End = 0x0001,
	Start = 0x0002,
	Margin = 0x0004,
};

enum class WrapVisualLocation {
	Default = 0x0000,
	EndByText = 0x0001,
	StartByText = 0x0002,
};

enum class WrapIndentMode {
	Fixed = 0,
	Same = 1,
	Indent = 2,
	DeepIndent = 3,
};

enum class LineCache {
	None = 0,
	Caret = 1,
	Page = 2,
	Document = 3,
};

enum class PhasesDraw {
	One = 0,
	Two = 1,
	Multiple = 2,
};

enum class FontQuality {
	QualityMask = 0xF,
	QualityDefault = 0,
	QualityNonAntialiased = 1,
	QualityAntialiased = 2,
	QualityLcdOptimized = 3,
};

enum class MultiPaste {
	Once = 0,
	Each = 1,
};

enum class Accessibility {
	Disabled = 0,
	Enabled = 1,
};

enum class EdgeVisualStyle {
	None = 0,
	Line = 1,
	Background = 2,
	MultiLine = 3,
};

enum class PopUp {
	Never = 0,
	All = 1,
	Text = 2,
};

enum class DocumentOption {
	Default = 0,
	StylesNone = 0x1,
	TextLarge = 0x100,
};

enum class Status {
	Ok = 0,
	Failure = 1,
	BadAlloc = 2,
	WarnStart = 1000,
	RegEx = 1001,
};

enum class VisiblePolicy {
	Slop = 0x01,
	Strict = 0x04,
};

enum class CaretPolicy {
	Slop = 0x01,
	Strict = 0x04,
	Jumps = 0x10,
	Even = 0x08,
};

enum class SelectionMode {
	Stream = 0,
	Rectangle = 1,
	Lines = 2,
	Thin = 3,
};

enum class CaseInsensitiveBehaviour {
	RespectCase = 0,
	IgnoreCase = 1,
};

enum class MultiAutoComplete {
	Once = 0,
	Each = 1,
};

enum class Ordering {
	PreSorted = 0,
	PerformSort = 1,
	Custom = 2,
};

enum class CaretSticky {
	Off = 0,
	On = 1,
	WhiteSpace = 2,
};

enum class CaretStyle {
	Invisible = 0,
	Line = 1,
	Block = 2,
	OverstrikeBar = 0,
	OverstrikeBlock = 0x10,
	Curses = 0x20,
	InsMask = 0xF,
	BlockAfter = 0x100,
};

enum class MarginOption {
	None = 0,
	SubLineSelect = 1,
};

enum class AnnotationVisible {
	Hidden = 0,
	Standard = 1,
	Boxed = 2,
	Indented = 3,
};

enum class UndoFlags {
	None = 0,
	MayCoalesce = 1,
};

enum class VirtualSpace {
	None = 0,
	RectangularSelection = 1,
	UserAccessible = 2,
	NoWrapLineStart = 4,
};

enum class Technology {
	Default = 0,
	DirectWrite = 1,
	DirectWriteRetain = 2,
	DirectWriteDC = 3,
};

enum class LineEndType {
	Default = 0,
	Unicode = 1,
};

enum class RepresentationAppearance {
	Plain = 0,
	Blob = 1,
	Colour = 0x10,
};

enum class EOLAnnotationVisible {
	Hidden = 0x0,
	Standard = 0x1,
	Boxed = 0x2,
	Stadium = 0x100,
	FlatCircle = 0x101,
	AngleCircle = 0x102,
	CircleFlat = 0x110,
	Flats = 0x111,
	AngleFlat = 0x112,
	CircleAngle = 0x120,
	FlatAngle = 0x121,
	Angles = 0x122,
};

enum class Supports {
	LineDrawsFinal = 0,
	PixelDivisions = 1,
	FractionalStrokeWidth = 2,
	TranslucentStroke = 3,
	PixelModification = 4,
	ThreadSafeMeasureWidths = 5,
};

enum class LineCharacterIndexType {
	None = 0,
	Utf32 = 1,
	Utf16 = 2,
};

enum class TypeProperty {
	Boolean = 0,
	Integer = 1,
	String = 2,
};

enum class ModificationFlags {
	None = 0x0,
	InsertText = 0x1,
	DeleteText = 0x2,
	ChangeStyle = 0x4,
	ChangeFold = 0x8,
	User = 0x10,
	Undo = 0x20,
	Redo = 0x40,
	MultiStepUndoRedo = 0x80,
	LastStepInUndoRedo = 0x100,
	ChangeMarker = 0x200,
	BeforeInsert = 0x400,
	BeforeDelete = 0x800,
	MultilineUndoRedo = 0x1000,
	StartAction = 0x2000,
	ChangeIndicator = 0x4000,
	ChangeLineState = 0x8000,
	ChangeMargin = 0x10000,
	ChangeAnnotation = 0x20000,
	Container = 0x40000,
	LexerState = 0x80000,
	InsertCheck = 0x100000,
	ChangeTabStops = 0x200000,
	ChangeEOLAnnotation = 0x400000,
	EventMaskAll = 0x7FFFFF,
};

enum class Update {
	None = 0x0,
	Content = 0x1,
	Selection = 0x2,
	VScroll = 0x4,
	HScroll = 0x8,
};

enum class FocusChange {
	Change = 768,
	Setfocus = 512,
	Killfocus = 256,
};

enum class Keys {
	Down = 300,
	Up = 301,
	Left = 302,
	Right = 303,
	Home = 304,
	End = 305,
	Prior = 306,
	Next = 307,
	Delete = 308,
	Insert = 309,
	Escape = 7,
	Back = 8,
	Tab = 9,
	Return = 13,
	Add = 310,
	Subtract = 311,
	Divide = 312,
	Win = 313,
	RWin = 314,
	Menu = 315,
};

enum class KeyMod {
	Norm = 0,
	Shift = 1,
	Ctrl = 2,
	Alt = 4,
	Super = 8,
	Meta = 16,
};

enum class CompletionMethods {
	FillUp = 1,
	DoubleClick = 2,
	Tab = 3,
	Newline = 4,
	Command = 5,
	SingleChoice = 6,
};

enum class CharacterSource {
	DirectInput = 0,
	TentativeInput = 1,
	ImeResult = 2,
};

enum class Bidirectional {
	Disabled = 0,
	L2R = 1,
	R2L = 2,
};

enum class Notification {
	StyleNeeded = 2000,
	CharAdded = 2001,
	SavePointReached = 2002,
	SavePointLeft = 2003,
	ModifyAttemptRO = 2004,
	Key = 2005,
	DoubleClick = 2006,
	UpdateUI = 2007,
	Modified = 2008,
	MacroRecord = 2009,
	MarginClick = 2010,
	NeedShown = 2011,
	Painted = 2013,
	UserListSelection = 2014,
	URIDropped = 2015,
	DwellStart = 2016,
	DwellEnd = 2017,
	Zoom = 2018,
	HotSpotClick = 2019,
	HotSpotDoubleClick = 2020,
	CallTipClick = 2021,
	AutoCSelection = 2022,
	IndicatorClick = 2023,
	IndicatorRelease = 2024,
	AutoCCancelled = 2025,
	AutoCCharDeleted = 2026,
	HotSpotReleaseClick = 2027,
	FocusIn = 2028,
	FocusOut = 2029,
	AutoCCompleted = 2030,
	MarginRightClick = 2031,
	AutoCSelectionChange = 2032,
};
//--Autogenerated -- end of section automatically generated from Scintilla.iface

using Position = intptr_t;
using Line = intptr_t;
using Colour = int;
using ColourAlpha = int;
using uptr_t = uintptr_t;
using sptr_t = intptr_t;

//++Autogenerated -- start of section automatically generated from Scintilla.iface
//**1 \(\*\n\)
constexpr Position InvalidPosition = -1;
constexpr int CpUtf8 = 65001;
constexpr int MarkerMax = 31;
constexpr int MaskFolders = 0xFE000000;
constexpr int MaxMargin = 4;
constexpr int FontSizeMultiplier = 100;
constexpr int TimeForever = 10000000;
constexpr int KeywordsetMax = 8;

//--Autogenerated -- end of section automatically generated from Scintilla.iface

constexpr int IndicatorMax = static_cast<int>(IndicatorNumbers::Max);

// Functions to manipulate fields from a MarkerOutline

inline int operator<<(int i, MarkerOutline marker) noexcept {
	return i << static_cast<int>(marker);
}

// Functions to manipulate fields from a FindOption

constexpr FindOption operator|(FindOption a, FindOption b) noexcept {
	return static_cast<FindOption>(static_cast<int>(a) | static_cast<int>(b));
}

inline FindOption &operator|=(FindOption &self, FindOption a) noexcept {
	self = self | a;
	return self;
}

// Functions to retrieve and manipulate fields from a FoldLevel

constexpr FoldLevel operator&(FoldLevel lhs, FoldLevel rhs) noexcept {
	return static_cast<FoldLevel>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

constexpr FoldLevel LevelNumberPart(FoldLevel level) noexcept {
	return level & FoldLevel::NumberMask;
}

constexpr int LevelNumber(FoldLevel level) noexcept {
	return static_cast<int>(LevelNumberPart(level));
}

constexpr bool LevelIsHeader(FoldLevel level) noexcept {
	return (level & FoldLevel::HeaderFlag) == FoldLevel::HeaderFlag;
}

constexpr bool LevelIsWhitespace(FoldLevel level) noexcept {
	return (level & FoldLevel::WhiteFlag) == FoldLevel::WhiteFlag;
}

// Functions to manipulate fields from a FoldFlag

constexpr FoldFlag operator|(FoldFlag a, FoldFlag b) noexcept {
	return static_cast<FoldFlag>(static_cast<int>(a) | static_cast<int>(b));
}

// Functions to manipulate fields from a FontQuality

constexpr FontQuality operator&(FontQuality a, FontQuality b) noexcept {
	return static_cast<FontQuality>(static_cast<int>(a) & static_cast<int>(b));
}

// Functions to manipulate fields from a DocumentOption

constexpr DocumentOption operator|(DocumentOption a, DocumentOption b) noexcept {
	return static_cast<DocumentOption>(static_cast<int>(a) | static_cast<int>(b));
}

// Functions to manipulate fields from a CaretPolicy

constexpr CaretPolicy operator|(CaretPolicy a, CaretPolicy b) noexcept {
	return static_cast<CaretPolicy>(static_cast<int>(a) | static_cast<int>(b));
}

// Functions to manipulate fields from a CaretStyle

constexpr CaretStyle operator|(CaretStyle a, CaretStyle b) noexcept {
	return static_cast<CaretStyle>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr CaretStyle operator&(CaretStyle a, CaretStyle b) noexcept {
	return static_cast<CaretStyle>(static_cast<int>(a) & static_cast<int>(b));
}

// Functions to manipulate fields from a LineEndType

constexpr LineEndType operator&(LineEndType a, LineEndType b) noexcept {
	return static_cast<LineEndType>(static_cast<int>(a) & static_cast<int>(b));
}

// Functions to manipulate fields from a RepresentationAppearance

constexpr RepresentationAppearance operator|(RepresentationAppearance a, RepresentationAppearance b) noexcept {
	return static_cast<RepresentationAppearance>(static_cast<int>(a) | static_cast<int>(b));
}

// Functions to manipulate fields from a LineCharacterIndexType

constexpr LineCharacterIndexType operator|(LineCharacterIndexType a, LineCharacterIndexType b) noexcept {
	return static_cast<LineCharacterIndexType>(static_cast<int>(a) | static_cast<int>(b));
}

// Functions to manipulate fields from a ModificationFlags

constexpr ModificationFlags operator|(ModificationFlags a, ModificationFlags b) noexcept {
	return static_cast<ModificationFlags>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr ModificationFlags operator&(ModificationFlags a, ModificationFlags b) noexcept {
	return static_cast<ModificationFlags>(static_cast<int>(a) & static_cast<int>(b));
}

inline ModificationFlags &operator|=(ModificationFlags &self, ModificationFlags a) noexcept {
	self = self | a;
	return self;
}

// Functions to manipulate fields from a Update

constexpr Update operator|(Update a, Update b) noexcept {
	return static_cast<Update>(static_cast<int>(a) | static_cast<int>(b));
}

// Functions to manipulate fields from a KeyMod

constexpr KeyMod operator|(KeyMod a, KeyMod b) noexcept {
	return static_cast<KeyMod>(static_cast<int>(a) | static_cast<int>(b));
}

constexpr KeyMod operator&(KeyMod a, KeyMod b) noexcept {
	return static_cast<KeyMod>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr KeyMod ModifierFlags(bool shift, bool ctrl, bool alt, bool meta=false, bool super=false) noexcept {
	return
		(shift ? KeyMod::Shift : KeyMod::Norm) |
		(ctrl ? KeyMod::Ctrl : KeyMod::Norm) |
		(alt ? KeyMod::Alt : KeyMod::Norm) |
		(meta ? KeyMod::Meta : KeyMod::Norm) |
		(super ? KeyMod::Super : KeyMod::Norm);
}

// Test if an enum class value has some bit flag(s) of test set.
template <typename T>
constexpr bool FlagSet(T value, T test) {
	return (static_cast<int>(value) & static_cast<int>(test)) != 0;
}

}

#endif
