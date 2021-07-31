// Lexilla lexer library
/** @file TestLexers.cxx
 ** Test lexers through Lexilla.
 **/
 // Copyright 2019 by Neil Hodgson <neilh@scintilla.org>
 // The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <optional>

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "ILexer.h"

#include "Lexilla.h"
#include "LexillaAccess.h"

#include "TestDocument.h"

namespace {

std::string ReadFile(std::filesystem::path path) {
	std::ifstream ifs(path, std::ios::binary);
	std::string content((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	return content;
}

std::string MarkedDocument(const Scintilla::IDocument *pdoc) {
	std::ostringstream os(std::ios::binary);
	char prevStyle = -1;
	for (Sci_Position pos = 0; pos < pdoc->Length(); pos++) {
		const char styleNow = pdoc->StyleAt(pos);
		if (styleNow != prevStyle) {
			os << "{" << static_cast<unsigned int>(styleNow) << "}";
			prevStyle = styleNow;
		}
		char ch = '\0';
		pdoc->GetCharRange(&ch, pos, 1);
		os << ch;
	}
	return os.str();
}

void PrintLevel(std::ostringstream &os, int level) {
	const int levelNow = level & 0xFFF;
	const int levelNext = level >> 16;
	const int levelFlags = (level >> 12) & 0xF;
	char foldSymbol = ' ';
	if (level & 0x2000)
		foldSymbol = '+';
	else if (levelNow > 0x400)
		foldSymbol = '|';
	os << std::hex << " " << levelFlags << " "
		<< std::setw(3) << levelNow << " "
		<< std::setw(3) << levelNext << " "
		<< foldSymbol << " ";
}

std::string FoldedDocument(const Scintilla::IDocument *pdoc) {
	std::ostringstream os(std::ios::binary);
	Sci_Position linePrev = -1;
	char ch = '\0';
	for (Sci_Position pos = 0; pos < pdoc->Length(); pos++) {
		const Sci_Position lineNow = pdoc->LineFromPosition(pos);
		if (linePrev < lineNow) {
			PrintLevel(os, pdoc->GetLevel(lineNow));
			linePrev = lineNow;
		}
		pdoc->GetCharRange(&ch, pos, 1);
		os << ch;
	}
	if (ch == '\n') {
		// Extra empty line
		PrintLevel(os, pdoc->GetLevel(linePrev + 1));
	}
	return os.str();
}

class PropertyMap {
public:
	using PropMap = std::map<std::string, std::string>;
	PropMap properties;

	void ReadFromFile(std::filesystem::path path) {
		std::ifstream ifs(path);
		std::string line;
		std::string logicalLine;
		while (std::getline(ifs, line)) {
			if (line.ends_with("\r")) {
				// Accidentally have \r\n line ends on Unix system
				line.pop_back();
			}
			logicalLine += line;
			if (logicalLine.ends_with("\\")) {
				logicalLine.pop_back();
			} else {
				const size_t positionEquals = logicalLine.find("=");
				if (positionEquals != std::string::npos) {
					const std::string key = logicalLine.substr(0, positionEquals);
					const std::string value = logicalLine.substr(positionEquals + 1);
					properties[key] = value;
				}
				logicalLine.clear();
			}
		}
	}

	std::optional<std::string> GetProperty(std::string_view key) const {
		const PropMap::const_iterator prop = properties.find(std::string(key));
		if (prop == properties.end())
			return std::nullopt;
		else
			return prop->second;
	}

	std::optional<int> GetPropertyValue(std::string_view key) const {
		std::optional<std::string> value = GetProperty(key);
		try {
			if (value)
				return std::stoi(value->c_str());
		}
		catch (std::invalid_argument &) {
			// Just return empty
		}
		return {};
	}

};

int Substitute(std::string &s, const std::string &sFind, const std::string &sReplace) {
	int c = 0;
	const size_t lenFind = sFind.size();
	const size_t lenReplace = sReplace.size();
	size_t posFound = s.find(sFind);
	while (posFound != std::string::npos) {
		s.replace(posFound, lenFind, sReplace);
		posFound = s.find(sFind, posFound + lenReplace);
		c++;
	}
	return c;
}

const std::string BOM = "\xEF\xBB\xBF";

void TestCRLF(std::filesystem::path path, const std::string s, Scintilla::ILexer5 *plex) {
	// Convert all line ends to \r\n to check if styles change between \r and \n which makes
	// it difficult to test on different platforms when files may have line ends changed.
	std::string text = s;
	Substitute(text, "\r\n", "\n");
	Substitute(text, "\n", "\r\n");
	TestDocument doc;
	doc.Set(text);
	Scintilla::IDocument *pdoc = &doc;
	plex->Lex(0, pdoc->Length(), 0, pdoc);
	int prevStyle = -1;
	Sci_Position line = 1;
	for (Sci_Position pos = 0; pos < pdoc->Length(); pos++) {
		const int styleNow = pdoc->StyleAt(pos);
		char ch = '\0';
		pdoc->GetCharRange(&ch, pos, 1);
		if (ch == '\n') {
			if (styleNow != prevStyle) {
				std::cout << path.string() << ":" << line << ":" <<
					" different styles between \\r and \\n at " <<
					pos << ": " << prevStyle << ", " << styleNow << "\n";
			}
			line++;
		}
		prevStyle = styleNow;
	}
	plex->Release();
}

bool TestFile(const std::filesystem::path &path, const PropertyMap &propertyMap) {
	// Find and create correct lexer
	std::string language;
	Scintilla::ILexer5 *plex = nullptr;
	for (auto const &[key, val] : propertyMap.properties) {
		if (key.starts_with("lexer.*")) {
			language = val;
			plex = Lexilla::MakeLexer(language);
			break;
		}
	}
	if (!plex) {
		std::cout << "\n" << path.string() << ":1: has no lexer\n\n";
		return false;
	}

	// Set parameters of lexer
	const std::string keywords = "keywords";
	for (auto const &[key, val] : propertyMap.properties) {
		if (key.starts_with("#")) {
			// Ignore comments
		} else if (key.starts_with("lexer.*")) {
			// Ignore
		} else if (key.starts_with("keywords")) {
			// Get character after keywords
			std::string afterKeywords = key.substr(keywords.length(), 1);
			char characterAfterKeywords = afterKeywords.empty() ? '1' : afterKeywords[0];
			if (characterAfterKeywords < '1' || characterAfterKeywords > '9')
				characterAfterKeywords = '1';
			const int wordSet = characterAfterKeywords - '1';
			plex->WordListSet(wordSet, val.c_str());
		} else {
			plex->PropertySet(key.c_str(), val.c_str());
		}
	}
	std::string text = ReadFile(path);
	if (text.starts_with(BOM)) {
		text.erase(0, BOM.length());
	}

	std::filesystem::path pathStyled = path;
	pathStyled += ".styled";
	const std::string styledText = ReadFile(pathStyled);

	std::filesystem::path pathFolded = path;
	pathFolded += ".folded";
	const std::string foldedText = ReadFile(pathFolded);

	const int repeatLex = propertyMap.GetPropertyValue("testlexers.repeat.lex").value_or(1);
	const int repeatFold = propertyMap.GetPropertyValue("testlexers.repeat.fold").value_or(1);

	TestDocument doc;
	doc.Set(text);
	Scintilla::IDocument *pdoc = &doc;
	for (int i = 0; i < repeatLex; i++) {
		plex->Lex(0, pdoc->Length(), 0, pdoc);
	}
	for (int i = 0; i < repeatFold; i++) {
		plex->Fold(0, pdoc->Length(), 0, pdoc);
	}

	bool success = true;

	const std::string styledTextNew = MarkedDocument(pdoc);
	if (styledTextNew != styledText) {
		success = false;
		std::cout << "\n" << path.string() << ":1: is different\n\n";
		std::filesystem::path pathNew = path;
		pathNew += ".styled.new";
		std::ofstream ofs(pathNew, std::ios::binary);
		ofs << styledTextNew;
	}

	const std::string foldedTextNew = FoldedDocument(pdoc);
	if (foldedTextNew != foldedText) {
		success = false;
		std::cout << "\n" << path.string() << ":1: has different folds\n\n";
		std::filesystem::path pathNew = path;
		pathNew += ".folded.new";
		std::ofstream ofs(pathNew, std::ios::binary);
		ofs << foldedTextNew;
	}

	const std::optional<int> perLineDisable = propertyMap.GetPropertyValue("testlexers.per.line.disable");
	const bool disablePerLineTests = perLineDisable.value_or(false);

	if (success && !disablePerLineTests) {
		// Test line by line lexing/folding
		doc.Set(text);
		const Sci_Position lines = doc.LineFromPosition(doc.Length());
		Sci_Position startLine = 0;
		for (Sci_Position line = 0; line <= lines; line++) {
			const Sci_Position endLine = doc.LineStart(line+1);
			int styleStart = 0;
			if (startLine > 0)
				styleStart = doc.StyleAt(startLine - 1);
			plex->Lex(startLine, endLine - startLine, styleStart, pdoc);
			plex->Fold(startLine, endLine - startLine, styleStart, pdoc);
			startLine = endLine;
		}

		const std::string styledTextNewPerLine = MarkedDocument(pdoc);
		if (styledTextNewPerLine != styledText) {
			success = false;
			std::cout << "\n" << path.string() << ":1: per-line is different\n\n";
			std::filesystem::path pathNew = path;
			pathNew += ".styled.new";
			std::ofstream ofs(pathNew, std::ios::binary);
			ofs << styledTextNewPerLine;
		}

		const std::string foldedTextNewPerLine = FoldedDocument(pdoc);
		if (foldedTextNewPerLine != foldedText) {
			success = false;
			std::cout << "\n" << path.string() << ":1: per-line has different folds\n\n";
			std::filesystem::path pathNew = path;
			pathNew += ".folded.new";
			std::ofstream ofs(pathNew, std::ios::binary);
			ofs << foldedTextNewPerLine;
		}
	}

	plex->Release();

	TestCRLF(path, text, Lexilla::MakeLexer(language));

	return success;
}

bool TestDirectory(std::filesystem::path directory, std::filesystem::path basePath) {
	PropertyMap properties;
	properties.ReadFromFile(directory / "SciTE.properties");
	bool success = true;
	for (auto &p : std::filesystem::directory_iterator(directory)) {
		if (!p.is_directory()) {
			const std::string extension = p.path().extension().string();
			if (extension != ".properties" && extension != ".styled" && extension != ".new" &&
				extension != ".folded") {
				const std::filesystem::path relativePath = p.path().lexically_relative(basePath);
				std::cout << "Lexing " << relativePath.string() << '\n';
				if (!TestFile(p, properties)) {
					success = false;
				}
			}
		}
	}
	return success;
}

bool AccessLexilla(std::filesystem::path basePath) {
	if (!std::filesystem::exists(basePath)) {
		std::cout << "No examples at " << basePath.string() << "\n";
		return false;
	}

	bool success = true;
	for (auto &p : std::filesystem::recursive_directory_iterator(basePath)) {
		if (p.is_directory()) {
			//std::cout << p.path().string() << '\n';
			if (!TestDirectory(p, basePath)) {
				success = false;
			}
		}
	}
	return success;
}

std::filesystem::path FindLexillaDirectory(std::filesystem::path startDirectory) {
	// Search up from startDirectory for a directory named "lexilla" or containing a "bin" subdirectory 
	std::filesystem::path directory = startDirectory;
	while (!directory.empty()) {
		//std::cout << "Searching " << directory.string() << "\n";
		const std::filesystem::path parent = directory.parent_path();
		const std::filesystem::path localLexilla = directory / "lexilla";
		const std::filesystem::directory_entry entry(localLexilla);
		if (entry.is_directory()) {
			std::cout << "Found Lexilla at " << entry.path().string() << "\n";
			return localLexilla;
		}
		const std::filesystem::path localBin = directory / "bin";
		const std::filesystem::directory_entry entryBin(localBin);
		if (entryBin.is_directory()) {
			std::cout << "Found Lexilla at " << directory.string() << "\n";
			return directory;
		}
		if (parent == directory) {
			std::cout << "Reached root at " << directory.string() << "\n";
			return std::filesystem::path();
		}
		directory = parent;
	}
	return std::filesystem::path();
}

}



int main() {
	bool success = false;
	// TODO: Allow specifying the base directory through a command line argument
	const std::filesystem::path baseDirectory = FindLexillaDirectory(std::filesystem::current_path());
	if (!baseDirectory.empty()) {
		const std::filesystem::path examplesDirectory = baseDirectory / "test" / "examples";
#ifdef LEXILLA_STATIC
		success = AccessLexilla(examplesDirectory);
#else
		const std::filesystem::path sharedLibrary = baseDirectory / "bin" / LEXILLA_LIB;
		if (Lexilla::Load(sharedLibrary.string())) {
			success = AccessLexilla(examplesDirectory);
		} else {
			std::cout << "Failed to load " << sharedLibrary << "\n";
		}
#endif
	}
	return success ? 0 : 1;
}
