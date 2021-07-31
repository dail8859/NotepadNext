// Scintilla source code edit control
/** @file MarginView.cxx
 ** Defines the appearance of the editor margin.
 **/
// Copyright 1998-2014 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cmath>

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
#include "ScintillaMessages.h"
#include "ScintillaStructures.h"
#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

#include "CharacterCategoryMap.h"
#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "UniConversion.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"
#include "MarginView.h"
#include "EditView.h"

using namespace Scintilla;

namespace Scintilla::Internal {

void DrawWrapMarker(Surface *surface, PRectangle rcPlace,
	bool isEndMarker, ColourRGBA wrapColour) {

	const XYPOSITION extraFinalPixel = surface->SupportsFeature(Supports::LineDrawsFinal) ? 0.0f : 1.0f;

	const PRectangle rcAligned = PixelAlignOutside(rcPlace, surface->PixelDivisions());

	const XYPOSITION widthStroke = std::floor(rcAligned.Width() / 6);

	constexpr XYPOSITION xa = 1; // gap before start
	const XYPOSITION w = rcAligned.Width() - xa - widthStroke;

	// isEndMarker -> x-mirrored symbol for start marker

	const XYPOSITION x0 = isEndMarker ? rcAligned.left : rcAligned.right - widthStroke;
	const XYPOSITION y0 = rcAligned.top;

	const XYPOSITION dy = std::floor(rcAligned.Height() / 5);
	const XYPOSITION y = std::floor(rcAligned.Height() / 2) + dy;

	struct Relative {
		XYPOSITION xBase;
		int xDir;
		XYPOSITION yBase;
		int yDir;
		XYPOSITION halfWidth;
		Point At(XYPOSITION xRelative, XYPOSITION yRelative) noexcept {
			return Point(xBase + xDir * xRelative + halfWidth, yBase + yDir * yRelative + halfWidth);
		}
	};

	Relative rel = { x0, isEndMarker ? 1 : -1, y0, 1, widthStroke / 2.0f };

	// arrow head
	const Point head[] = {
		rel.At(xa + dy, y - dy),
		rel.At(xa, y),
		rel.At(xa + dy + extraFinalPixel, y + dy + extraFinalPixel)
	};
	surface->PolyLine(head, std::size(head), Stroke(wrapColour, widthStroke));

	// arrow body
	const Point body[] = {
		rel.At(xa, y),
		rel.At(xa + w, y),
		rel.At(xa + w, y - 2 * dy),
		rel.At(xa, y - 2 * dy),
	};
	surface->PolyLine(body, std::size(body), Stroke(wrapColour, widthStroke));
}

MarginView::MarginView() noexcept {
	wrapMarkerPaddingRight = 3;
	customDrawWrapMarker = nullptr;
}

void MarginView::DropGraphics() noexcept {
	pixmapSelMargin.reset();
	pixmapSelPattern.reset();
	pixmapSelPatternOffset1.reset();
}

void MarginView::RefreshPixMaps(Surface *surfaceWindow, const ViewStyle &vsDraw) {
	if (!pixmapSelPattern) {
		constexpr int patternSize = 8;
		pixmapSelPattern = surfaceWindow->AllocatePixMap(patternSize, patternSize);
		pixmapSelPatternOffset1 = surfaceWindow->AllocatePixMap(patternSize, patternSize);
		// This complex procedure is to reproduce the checkerboard dithered pattern used by windows
		// for scroll bars and Visual Studio for its selection margin. The colour of this pattern is half
		// way between the chrome colour and the chrome highlight colour making a nice transition
		// between the window chrome and the content area. And it works in low colour depths.
		const PRectangle rcPattern = PRectangle::FromInts(0, 0, patternSize, patternSize);

		// Initialize default colours based on the chrome colour scheme.  Typically the highlight is white.
		ColourRGBA colourFMFill = vsDraw.selbar;
		ColourRGBA colourFMStripes = vsDraw.selbarlight;

		if (!(vsDraw.selbarlight == ColourRGBA(0xff, 0xff, 0xff))) {
			// User has chosen an unusual chrome colour scheme so just use the highlight edge colour.
			// (Typically, the highlight colour is white.)
			colourFMFill = vsDraw.selbarlight;
		}

		if (vsDraw.foldmarginColour) {
			// override default fold margin colour
			colourFMFill = *vsDraw.foldmarginColour;
		}
		if (vsDraw.foldmarginHighlightColour) {
			// override default fold margin highlight colour
			colourFMStripes = *vsDraw.foldmarginHighlightColour;
		}

		pixmapSelPattern->FillRectangle(rcPattern, colourFMFill);
		pixmapSelPatternOffset1->FillRectangle(rcPattern, colourFMStripes);
		for (int y = 0; y < patternSize; y++) {
			for (int x = y % 2; x < patternSize; x += 2) {
				const PRectangle rcPixel = PRectangle::FromInts(x, y, x + 1, y + 1);
				pixmapSelPattern->FillRectangle(rcPixel, colourFMStripes);
				pixmapSelPatternOffset1->FillRectangle(rcPixel, colourFMFill);
			}
		}
		pixmapSelPattern->FlushDrawing();
		pixmapSelPatternOffset1->FlushDrawing();
	}
}

static MarkerOutline SubstituteMarkerIfEmpty(MarkerOutline markerCheck, MarkerOutline markerDefault, const ViewStyle &vs) noexcept {
	if (vs.markers[static_cast<size_t>(markerCheck)].markType == MarkerSymbol::Empty)
		return markerDefault;
	return markerCheck;
}

void MarginView::PaintMargin(Surface *surface, Sci::Line topLine, PRectangle rc, PRectangle rcMargin,
	const EditModel &model, const ViewStyle &vs) {

	PRectangle rcSelMargin = rcMargin;
	rcSelMargin.right = rcMargin.left;
	if (rcSelMargin.bottom < rc.bottom)
		rcSelMargin.bottom = rc.bottom;

	const Point ptOrigin = model.GetVisibleOriginInMain();
	const Font *fontLineNumber = vs.styles[StyleLineNumber].font.get();
	for (size_t margin = 0; margin < vs.ms.size(); margin++) {
		if (vs.ms[margin].width > 0) {

			rcSelMargin.left = rcSelMargin.right;
			rcSelMargin.right = rcSelMargin.left + vs.ms[margin].width;

			if (vs.ms[margin].style != MarginType::Number) {
				if (vs.ms[margin].ShowsFolding()) {
					// Required because of special way brush is created for selection margin
					// Ensure patterns line up when scrolling with separate margin view
					// by choosing correctly aligned variant.
					const bool invertPhase = static_cast<int>(ptOrigin.y) & 1;
					surface->FillRectangle(rcSelMargin,
						invertPhase ? *pixmapSelPattern : *pixmapSelPatternOffset1);
				} else {
					ColourRGBA colour;
					switch (vs.ms[margin].style) {
					case MarginType::Back:
						colour = vs.styles[StyleDefault].back;
						break;
					case MarginType::Fore:
						colour = vs.styles[StyleDefault].fore;
						break;
					case MarginType::Colour:
						colour = vs.ms[margin].back;
						break;
					default:
						colour = vs.styles[StyleLineNumber].back;
						break;
					}
					surface->FillRectangle(rcSelMargin, colour);
				}
			} else {
				surface->FillRectangle(rcSelMargin, vs.styles[StyleLineNumber].back);
			}

			const Sci::Line lineStartPaint = static_cast<Sci::Line>(rcMargin.top + ptOrigin.y) / vs.lineHeight;
			Sci::Line visibleLine = model.TopLineOfMain() + lineStartPaint;
			Sci::Position yposScreen = lineStartPaint * vs.lineHeight - static_cast<Sci::Position>(ptOrigin.y);
			// Work out whether the top line is whitespace located after a
			// lessening of fold level which implies a 'fold tail' but which should not
			// be displayed until the last of a sequence of whitespace.
			bool needWhiteClosure = false;
			if (vs.ms[margin].ShowsFolding()) {
				const FoldLevel level = model.pdoc->GetFoldLevel(model.pcs->DocFromDisplay(visibleLine));
				if (LevelIsWhitespace(level)) {
					Sci::Line lineBack = model.pcs->DocFromDisplay(visibleLine);
					FoldLevel levelPrev = level;
					while ((lineBack > 0) && LevelIsWhitespace(levelPrev)) {
						lineBack--;
						levelPrev = model.pdoc->GetFoldLevel(lineBack);
					}
					if (!LevelIsHeader(levelPrev)) {
						if (LevelNumber(level) < LevelNumber(levelPrev))
							needWhiteClosure = true;
					}
				}
				if (highlightDelimiter.isEnabled) {
					const Sci::Line lastLine = model.pcs->DocFromDisplay(topLine + model.LinesOnScreen()) + 1;
					model.pdoc->GetHighlightDelimiters(highlightDelimiter,
						model.pdoc->SciLineFromPosition(model.sel.MainCaret()), lastLine);
				}
			}

			// Old code does not know about new markers needed to distinguish all cases
			const MarkerOutline folderOpenMid = SubstituteMarkerIfEmpty(MarkerOutline::FolderOpenMid,
				MarkerOutline::FolderOpen, vs);
			const MarkerOutline folderEnd = SubstituteMarkerIfEmpty(MarkerOutline::FolderEnd,
				MarkerOutline::Folder, vs);

			while ((visibleLine < model.pcs->LinesDisplayed()) && yposScreen < rc.bottom) {

				PLATFORM_ASSERT(visibleLine < model.pcs->LinesDisplayed());
				const Sci::Line lineDoc = model.pcs->DocFromDisplay(visibleLine);
				PLATFORM_ASSERT(model.pcs->GetVisible(lineDoc));
				const Sci::Line firstVisibleLine = model.pcs->DisplayFromDoc(lineDoc);
				const Sci::Line lastVisibleLine = model.pcs->DisplayLastFromDoc(lineDoc);
				const bool firstSubLine = visibleLine == firstVisibleLine;
				const bool lastSubLine = visibleLine == lastVisibleLine;

				int marks = model.pdoc->GetMark(lineDoc);
				if (!firstSubLine)
					marks = 0;

				bool headWithTail = false;

				if (vs.ms[margin].ShowsFolding()) {
					// Decide which fold indicator should be displayed
					const FoldLevel level = model.pdoc->GetFoldLevel(lineDoc);
					const FoldLevel levelNext = model.pdoc->GetFoldLevel(lineDoc + 1);
					const FoldLevel levelNum = LevelNumberPart(level);
					const FoldLevel levelNextNum = LevelNumberPart(levelNext);
					if (LevelIsHeader(level)) {
						if (firstSubLine) {
							if (levelNum < levelNextNum) {
								if (model.pcs->GetExpanded(lineDoc)) {
									if (levelNum == FoldLevel::Base)
										marks |= 1 << MarkerOutline::FolderOpen;
									else
										marks |= 1 << folderOpenMid;
								} else {
									if (levelNum == FoldLevel::Base)
										marks |= 1 << MarkerOutline::Folder;
									else
										marks |= 1 << folderEnd;
								}
							} else if (levelNum > FoldLevel::Base) {
								marks |= 1 << MarkerOutline::FolderSub;
							}
						} else {
							if (levelNum < levelNextNum) {
								if (model.pcs->GetExpanded(lineDoc)) {
									marks |= 1 << MarkerOutline::FolderSub;
								} else if (levelNum > FoldLevel::Base) {
									marks |= 1 << MarkerOutline::FolderSub;
								}
							} else if (levelNum > FoldLevel::Base) {
								marks |= 1 << MarkerOutline::FolderSub;
							}
						}
						needWhiteClosure = false;
						const Sci::Line firstFollowupLine = model.pcs->DocFromDisplay(model.pcs->DisplayFromDoc(lineDoc + 1));
						const FoldLevel firstFollowupLineLevel = model.pdoc->GetFoldLevel(firstFollowupLine);
						const FoldLevel secondFollowupLineLevelNum = LevelNumberPart(model.pdoc->GetFoldLevel(firstFollowupLine + 1));
						if (!model.pcs->GetExpanded(lineDoc)) {
							if (LevelIsWhitespace(firstFollowupLineLevel) &&
								(levelNum > secondFollowupLineLevelNum))
								needWhiteClosure = true;

							if (highlightDelimiter.IsFoldBlockHighlighted(firstFollowupLine))
								headWithTail = true;
						}
					} else if (LevelIsWhitespace(level)) {
						if (needWhiteClosure) {
							if (LevelIsWhitespace(levelNext)) {
								marks |= 1 << MarkerOutline::FolderSub;
							} else if (levelNextNum > FoldLevel::Base) {
								marks |= 1 << MarkerOutline::FolderMidTail;
								needWhiteClosure = false;
							} else {
								marks |= 1 << MarkerOutline::FolderTail;
								needWhiteClosure = false;
							}
						} else if (levelNum > FoldLevel::Base) {
							if (levelNextNum < levelNum) {
								if (levelNextNum > FoldLevel::Base) {
									marks |= 1 << MarkerOutline::FolderMidTail;
								} else {
									marks |= 1 << MarkerOutline::FolderTail;
								}
							} else {
								marks |= 1 << MarkerOutline::FolderSub;
							}
						}
					} else if (levelNum > FoldLevel::Base) {
						if (levelNextNum < levelNum) {
							needWhiteClosure = false;
							if (LevelIsWhitespace(levelNext)) {
								marks |= 1 << MarkerOutline::FolderSub;
								needWhiteClosure = true;
							} else if (lastSubLine) {
								if (levelNextNum > FoldLevel::Base) {
									marks |= 1 << MarkerOutline::FolderMidTail;
								} else {
									marks |= 1 << MarkerOutline::FolderTail;
								}
							} else {
								marks |= 1 << MarkerOutline::FolderSub;
							}
						} else {
							marks |= 1 << MarkerOutline::FolderSub;
						}
					}
				}

				marks &= vs.ms[margin].mask;

				const PRectangle rcMarker(
					rcSelMargin.left,
					static_cast<XYPOSITION>(yposScreen),
					rcSelMargin.right,
					static_cast<XYPOSITION>(yposScreen + vs.lineHeight));
				if (vs.ms[margin].style == MarginType::Number) {
					if (firstSubLine) {
						std::string sNumber;
						if (lineDoc >= 0) {
							sNumber = std::to_string(lineDoc + 1);
						}
						if (FlagSet(model.foldFlags, (FoldFlag::LevelNumbers | FoldFlag::LineState))) {
							char number[100] = "";
							if (FlagSet(model.foldFlags, FoldFlag::LevelNumbers)) {
								const FoldLevel lev = model.pdoc->GetFoldLevel(lineDoc);
								sprintf(number, "%c%c %03X %03X",
									LevelIsHeader(lev) ? 'H' : '_',
									LevelIsWhitespace(lev) ? 'W' : '_',
									LevelNumber(lev),
									static_cast<int>(lev) >> 16
									);
							} else {
								const int state = model.pdoc->GetLineState(lineDoc);
								sprintf(number, "%0X", state);
							}
							sNumber = number;
						}
						PRectangle rcNumber = rcMarker;
						// Right justify
						const XYPOSITION width = surface->WidthText(fontLineNumber, sNumber);
						const XYPOSITION xpos = rcNumber.right - width - vs.marginNumberPadding;
						rcNumber.left = xpos;
						DrawTextNoClipPhase(surface, rcNumber, vs.styles[StyleLineNumber],
							rcNumber.top + vs.maxAscent, sNumber, DrawPhase::all);
					} else if (FlagSet(vs.wrap.visualFlags, WrapVisualFlag::Margin)) {
						PRectangle rcWrapMarker = rcMarker;
						rcWrapMarker.right -= wrapMarkerPaddingRight;
						rcWrapMarker.left = rcWrapMarker.right - vs.styles[StyleLineNumber].aveCharWidth;
						if (!customDrawWrapMarker) {
							DrawWrapMarker(surface, rcWrapMarker, false, vs.styles[StyleLineNumber].fore);
						} else {
							customDrawWrapMarker(surface, rcWrapMarker, false, vs.styles[StyleLineNumber].fore);
						}
					}
				} else if (vs.ms[margin].style == MarginType::Text || vs.ms[margin].style == MarginType::RText) {
					const StyledText stMargin = model.pdoc->MarginStyledText(lineDoc);
					if (stMargin.text && ValidStyledText(vs, vs.marginStyleOffset, stMargin)) {
						if (firstSubLine) {
							surface->FillRectangle(rcMarker,
								vs.styles[stMargin.StyleAt(0) + vs.marginStyleOffset].back);
							PRectangle rcText = rcMarker;
							if (vs.ms[margin].style == MarginType::RText) {
								const int width = WidestLineWidth(surface, vs, vs.marginStyleOffset, stMargin);
								rcText.left = rcText.right - width - 3;
							}
							DrawStyledText(surface, vs, vs.marginStyleOffset, rcText,
								stMargin, 0, stMargin.length, DrawPhase::all);
						} else {
							// if we're displaying annotation lines, colour the margin to match the associated document line
							const int annotationLines = model.pdoc->AnnotationLines(lineDoc);
							if (annotationLines && (visibleLine > lastVisibleLine - annotationLines)) {
								surface->FillRectangle(rcMarker, vs.styles[stMargin.StyleAt(0) + vs.marginStyleOffset].back);
							}
						}
					}
				}

				if (marks) {
					for (int markBit = 0; (markBit < 32) && marks; markBit++) {
						if (marks & 1) {
							LineMarker::FoldPart part = LineMarker::FoldPart::undefined;
							if (vs.ms[margin].ShowsFolding() && highlightDelimiter.IsFoldBlockHighlighted(lineDoc)) {
								if (highlightDelimiter.IsBodyOfFoldBlock(lineDoc)) {
									part = LineMarker::FoldPart::body;
								} else if (highlightDelimiter.IsHeadOfFoldBlock(lineDoc)) {
									if (firstSubLine) {
										part = headWithTail ? LineMarker::FoldPart::headWithTail : LineMarker::FoldPart::head;
									} else {
										if (model.pcs->GetExpanded(lineDoc) || headWithTail) {
											part = LineMarker::FoldPart::body;
										} else {
											part = LineMarker::FoldPart::undefined;
										}
									}
								} else if (highlightDelimiter.IsTailOfFoldBlock(lineDoc)) {
									part = LineMarker::FoldPart::tail;
								}
							}
							vs.markers[markBit].Draw(surface, rcMarker, fontLineNumber, part, vs.ms[margin].style);
						}
						marks >>= 1;
					}
				}

				visibleLine++;
				yposScreen += vs.lineHeight;
			}
		}
	}

	PRectangle rcBlankMargin = rcMargin;
	rcBlankMargin.left = rcSelMargin.right;
	surface->FillRectangle(rcBlankMargin, vs.styles[StyleDefault].back);
}

}

