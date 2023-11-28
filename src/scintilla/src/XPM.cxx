// Scintilla source code edit control
/** @file XPM.cxx
 ** Define a class that holds data in the X Pixmap (XPM) format.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cstring>
#include <climits>

#include <stdexcept>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>

#include "ScintillaTypes.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

#include "XPM.h"

using namespace Scintilla;
using namespace Scintilla::Internal;

namespace {

const char *NextField(const char *s) noexcept {
	// In case there are leading spaces in the string
	while (*s == ' ') {
		s++;
	}
	while (*s && *s != ' ') {
		s++;
	}
	while (*s == ' ') {
		s++;
	}
	return s;
}

// Data lines in XPM can be terminated either with NUL or "
size_t MeasureLength(const char *s) noexcept {
	size_t i = 0;
	while (s[i] && (s[i] != '\"'))
		i++;
	return i;
}

unsigned int ValueOfHex(const char ch) noexcept {
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	else if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	else
		return 0;
}

ColourRGBA ColourFromHex(const char *val) noexcept {
	const unsigned int r = ValueOfHex(val[0]) * 16 + ValueOfHex(val[1]);
	const unsigned int g = ValueOfHex(val[2]) * 16 + ValueOfHex(val[3]);
	const unsigned int b = ValueOfHex(val[4]) * 16 + ValueOfHex(val[5]);
	return ColourRGBA(r, g, b);
}

}


ColourRGBA XPM::ColourFromCode(int ch) const noexcept {
	return colourCodeTable[ch];
}

void XPM::FillRun(Surface *surface, int code, int startX, int y, int x) const {
	if ((code != codeTransparent) && (startX != x)) {
		const PRectangle rc = PRectangle::FromInts(startX, y, x, y + 1);
		surface->FillRectangle(rc, ColourFromCode(code));
	}
}

XPM::XPM(const char *textForm) {
	Init(textForm);
}

XPM::XPM(const char *const *linesForm) {
	Init(linesForm);
}

void XPM::Init(const char *textForm) {
	// Test done is two parts to avoid possibility of overstepping the memory
	// if memcmp implemented strangely. Must be 4 bytes at least at destination.
	if ((0 == memcmp(textForm, "/* X", 4)) && (0 == memcmp(textForm, "/* XPM */", 9))) {
		// Build the lines form out of the text form
		std::vector<const char *> linesForm = LinesFormFromTextForm(textForm);
		if (!linesForm.empty()) {
			Init(linesForm.data());
		}
	} else {
		// It is really in line form
		Init(reinterpret_cast<const char * const *>(textForm));
	}
}

void XPM::Init(const char *const *linesForm) {
	height = 1;
	width = 1;
	nColours = 1;
	pixels.clear();
	codeTransparent = ' ';
	if (!linesForm)
		return;

	std::fill(colourCodeTable, std::end(colourCodeTable), ColourRGBA(0, 0, 0));
	const char *line0 = linesForm[0];
	width = atoi(line0);
	line0 = NextField(line0);
	height = atoi(line0);
	pixels.resize(width*height);
	line0 = NextField(line0);
	nColours = atoi(line0);
	line0 = NextField(line0);
	if (atoi(line0) != 1) {
		// Only one char per pixel is supported
		return;
	}

	for (int c=0; c<nColours; c++) {
		const char *colourDef = linesForm[c+1];
		const char code = colourDef[0];
		colourDef += 4;
		ColourRGBA colour(0, 0, 0, 0);
		if (*colourDef == '#') {
			colour = ColourFromHex(colourDef+1);
		} else {
			codeTransparent = code;
		}
		colourCodeTable[static_cast<unsigned char>(code)] = colour;
	}

	for (ptrdiff_t y=0; y<height; y++) {
		const char *lform = linesForm[y+nColours+1];
		const size_t len = MeasureLength(lform);
		for (size_t x = 0; x<len; x++)
			pixels[y * width + x] = lform[x];
	}
}

void XPM::Draw(Surface *surface, const PRectangle &rc) {
	if (pixels.empty()) {
		return;
	}
	// Centre the pixmap
	const int startY = static_cast<int>(rc.top + (rc.Height() - height) / 2);
	const int startX = static_cast<int>(rc.left + (rc.Width() - width) / 2);
	for (int y=0; y<height; y++) {
		int prevCode = 0;
		int xStartRun = 0;
		for (int x=0; x<width; x++) {
			const int code = pixels[y * width + x];
			if (code != prevCode) {
				FillRun(surface, prevCode, startX + xStartRun, startY + y, startX + x);
				xStartRun = x;
				prevCode = code;
			}
		}
		FillRun(surface, prevCode, startX + xStartRun, startY + y, startX + width);
	}
}

ColourRGBA XPM::PixelAt(int x, int y) const noexcept {
	if (pixels.empty() || (x < 0) || (x >= width) || (y < 0) || (y >= height)) {
		// Out of bounds -> transparent black
		return ColourRGBA(0, 0, 0, 0);
	}
	const int code = pixels[y * width + x];
	return ColourFromCode(code);
}

std::vector<const char *> XPM::LinesFormFromTextForm(const char *textForm) {
	// Build the lines form out of the text form
	std::vector<const char *> linesForm;
	int countQuotes = 0;
	int strings=1;
	int j=0;
	for (; countQuotes < (2*strings) && textForm[j] != '\0'; j++) {
		if (textForm[j] == '\"') {
			if (countQuotes == 0) {
				// First field: width, height, number of colours, chars per pixel
				const char *line0 = textForm + j + 1;
				// Skip width
				line0 = NextField(line0);
				// Add 1 line for each pixel of height
				strings += atoi(line0);
				line0 = NextField(line0);
				// Add 1 line for each colour
				strings += atoi(line0);
			}
			if (countQuotes / 2 >= strings) {
				break;	// Bad height or number of colours!
			}
			if ((countQuotes & 1) == 0) {
				linesForm.push_back(textForm + j + 1);
			}
			countQuotes++;
		}
	}
	if (textForm[j] == '\0' || countQuotes / 2 > strings) {
		// Malformed XPM! Height + number of colours too high or too low
		linesForm.clear();
	}
	return linesForm;
}

RGBAImage::RGBAImage(int width_, int height_, float scale_, const unsigned char *pixels_) :
	height(height_), width(width_), scale(scale_) {
	if (pixels_) {
		pixelBytes.assign(pixels_, pixels_ + CountBytes());
	} else {
		pixelBytes.resize(CountBytes());
	}
}

RGBAImage::RGBAImage(const XPM &xpm) {
	height = xpm.GetHeight();
	width = xpm.GetWidth();
	scale = 1;
	pixelBytes.resize(CountBytes());
	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++) {
			SetPixel(x, y, xpm.PixelAt(x, y));
		}
	}
}

float RGBAImage::GetScaledHeight() const noexcept {
	return static_cast<float>(height) / scale;
}

float RGBAImage::GetScaledWidth() const noexcept {
	return static_cast<float>(width) / scale;
}

int RGBAImage::CountBytes() const noexcept {
	return width * height * 4;
}

const unsigned char *RGBAImage::Pixels() const noexcept {
	return pixelBytes.data();
}

void RGBAImage::SetPixel(int x, int y, ColourRGBA colour) noexcept {
	unsigned char *pixel = pixelBytes.data() + (y * width + x) * 4;
	// RGBA
	pixel[0] = colour.GetRed();
	pixel[1] = colour.GetGreen();
	pixel[2] = colour.GetBlue();
	pixel[3] = colour.GetAlpha();
}

namespace {

constexpr unsigned char AlphaMultiplied(unsigned char value, unsigned char alpha) noexcept {
	return (value * alpha / UCHAR_MAX) & 0xffU;
}

}

// Transform a block of pixels from RGBA to BGRA with premultiplied alpha.
// Used for DrawRGBAImage on some platforms.
void RGBAImage::BGRAFromRGBA(unsigned char *pixelsBGRA, const unsigned char *pixelsRGBA, size_t count) noexcept {
	for (size_t i = 0; i < count; i++) {
		const unsigned char alpha = pixelsRGBA[3];
		// Input is RGBA, output is BGRA with premultiplied alpha
		pixelsBGRA[2] = AlphaMultiplied(pixelsRGBA[0], alpha);
		pixelsBGRA[1] = AlphaMultiplied(pixelsRGBA[1], alpha);
		pixelsBGRA[0] = AlphaMultiplied(pixelsRGBA[2], alpha);
		pixelsBGRA[3] = alpha;
		pixelsRGBA += bytesPerPixel;
		pixelsBGRA += bytesPerPixel;
	}
}

RGBAImageSet::RGBAImageSet() : height(-1), width(-1) {
}

/// Remove all images.
void RGBAImageSet::Clear() noexcept {
	images.clear();
	height = -1;
	width = -1;
}

/// Add an image.
void RGBAImageSet::AddImage(int ident, std::unique_ptr<RGBAImage> image) {
	images[ident] = std::move(image);
	height = -1;
	width = -1;
}

/// Get image by id.
RGBAImage *RGBAImageSet::Get(int ident) {
	ImageMap::iterator it = images.find(ident);
	if (it != images.end()) {
		return it->second.get();
	}
	return nullptr;
}

/// Give the largest height of the set.
int RGBAImageSet::GetHeight() const noexcept {
	if (height < 0) {
		for (const std::pair<const int, std::unique_ptr<RGBAImage>> &image : images) {
			if (height < image.second->GetHeight()) {
				height = image.second->GetHeight();
			}
		}
	}
	return (height > 0) ? height : 0;
}

/// Give the largest width of the set.
int RGBAImageSet::GetWidth() const noexcept {
	if (width < 0) {
		for (const std::pair<const int, std::unique_ptr<RGBAImage>> &image : images) {
			if (width < image.second->GetWidth()) {
				width = image.second->GetWidth();
			}
		}
	}
	return (width > 0) ? width : 0;
}
