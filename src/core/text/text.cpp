/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../math/math.h"
#include "../vfs/vfs.h"
#include "../font/font.h"
#include "text.h"

namespace {

// Map of Maps with all sorted language data in utf8.
std::unordered_map<unsigned int, std::unordered_map<std::string, std::string>> TextTable;
// Map of Maps with all sorted language data in utf32.
std::unordered_map<unsigned int, std::unordered_map<std::string, std::u32string>> TextTableUTF32;
// Current default language.
unsigned int CurrentLanguage{0};
// Error text for UTF32, if we can't use TextTableUTF32 by some reason
const std::u32string TextTableUTF32Error{vw_UTF8toUTF32("vw_GetTextUTF32() Error")};

} // unnamed namespace


/*
 * Set default language.
 */
void vw_SetTextLanguage(unsigned int Language)
{
	CurrentLanguage = Language;
}

/*
 * Get available languages count.
 */
unsigned int vw_GetLanguageListCount() {
	if (TextTable.empty() || (TextTable.size() < 1))
		return 0;

	return TextTable.size() - 1 /*first column contain index, not data, don't count it*/;
}

/*
 * Release data.
 */
void vw_ReleaseText()
{
	TextTable.clear();
	TextTableUTF32.clear();
	CurrentLanguage = 0;
}

/*
 * Create text table with utf32 data (convert TextTable data for utf8 to utf32).
 */
static void CreateTextTableUTF32()
{
	for (unsigned int i = 0; i < TextTable.size(); i++) {
		for (auto tmpData : TextTable[i]) {
			TextTableUTF32[i][tmpData.first] = vw_UTF8toUTF32(tmpData.second.c_str());
		}
	}
	// unconditional rehash, at this line we have not rehashed map
	TextTableUTF32.rehash(0);
	for (unsigned int i = 0; i < TextTableUTF32.size(); i++) {
		TextTableUTF32[i].rehash(0);
	}
}

/*
 * Initialization. Load file with translation in .csv format (supported by LibreOffice Calc).
 */
int vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine)
{
	vw_ReleaseText();

	// open and don't call vw_fclose(), use tmpFile->Data directly
	std::unique_ptr<sFILE> tmpFile = vw_fopen(FileName);
	if (!tmpFile)
		return ERR_FILE_NOT_FOUND;

	// plain .csv file format parser
	// parse data by each byte, in order to use string as RowCode - build
	// RowCode first, only after that, initialize new TextTable row
	bool NeedBuildCurrentRowCode = true;
	std::string CurrentRowCode;
	unsigned int CurrentColumnNumber{0};
	for (unsigned int i = 0; i < tmpFile->Size; i++) {
		// parse each row
		for (; (tmpFile->Data[i] != SymbolEndOfLine) && (i < tmpFile->Size); i++) {
			// parse each row for blocks, separated by 1.SymbolSeparator, 2.SymbolEndOfLine, 3.EOF
			for (; (tmpFile->Data[i] != SymbolSeparator) && (tmpFile->Data[i] != SymbolEndOfLine) && (i < tmpFile->Size); i++) {
				if (NeedBuildCurrentRowCode)
					CurrentRowCode += tmpFile->Data[i];
				else
					TextTable[CurrentColumnNumber][CurrentRowCode] += tmpFile->Data[i];
			}
			// RowCode built, next blocks in this row contain data
			NeedBuildCurrentRowCode = false;
			CurrentColumnNumber++;
			// we found SymbolEndOfLine in previous cycle, in order to prevent "i" changes, break cycle
			if (tmpFile->Data[i] == SymbolEndOfLine)
				break;
		}
		// move to next row
		CurrentColumnNumber = 0;
		NeedBuildCurrentRowCode = true;
		CurrentRowCode.clear();
	}
	// unconditional rehash, at this line we have not rehashed map
	TextTable.rehash(0);
	for (unsigned int i = 0; i < TextTable.size(); i++) {
		TextTable[i].rehash(0);
	}

	CreateTextTableUTF32();

	std::cout << "Parsed .csv file " << FileName << "\n";
	std::cout << "Detected " << vw_GetLanguageListCount() << " languages:";
	for (unsigned int i = 1; i < TextTable.size(); i++) {
		std::cout << " " << vw_GetText("0_code", i);
	}
	std::cout << "\n\n";
	return 0;
}

/*
 * Get UTF8 text for particular language.
 */
const char *vw_GetText(const char *ItemID, unsigned int Language)
{
	if (!ItemID || TextTable.empty())
		return nullptr;

	if (Language > vw_GetLanguageListCount())
		Language = CurrentLanguage;

	return TextTable[Language][ItemID].c_str();
}

/*
 * Get UTF32 text for particular language.
 */
const std::u32string &vw_GetTextUTF32(const char *ItemID, unsigned int Language)
{
	if (!ItemID || TextTableUTF32.empty())
		return TextTableUTF32Error;

	if (Language > vw_GetLanguageListCount())
		Language = CurrentLanguage;

	return TextTableUTF32[Language][ItemID];
}


/*
 * Detect what characters was not generated (need for testing purposes).
 */
int vw_CheckFontCharsInText()
{
	if (TextTableUTF32.empty())
		return ERR_PARAMETERS;

	std::cout << "Font characters detection start.\n";

	// note, i = 1 - first column contain index, not data, start from second
	for (unsigned int i = 0; i < TextTableUTF32.size(); i++) {	// cycle by all columns
		for (auto tmpDataUTF32 : TextTableUTF32[i]) {		// cycle by all words
			for (auto UTF32 : tmpDataUTF32.first) {		// cycle by all chars
				if (!vw_CheckFontCharByUTF32(UTF32))
					std::cout << "!!! FontChar was not created, Unicode: " << UTF32 << "\n";
			}
		}
	}

	std::cout << "Font characters detection end.\n\n";
	return 0;
}
