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

// TODO add UTF32 support

#include "text.h"

namespace {

// Map of Maps with all sorted language data in utf8.
std::unordered_map<unsigned int, std::unordered_map<std::string, std::string>> TextTable;
// Current default language.
unsigned int CurrentLanguage = 0;

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
	CurrentLanguage = 0;
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
	unsigned int CurrentColumnCode{0};
	for (unsigned int i = 0; i < tmpFile->Size; i++) {
		// parse each row
		for (; (tmpFile->Data[i] != SymbolEndOfLine) && (i < tmpFile->Size); i++) {
			// parse each row for blocks, separated by 1.SymbolSeparator, 2.SymbolEndOfLine, 3.EOF
			for (; (tmpFile->Data[i] != SymbolSeparator) && (tmpFile->Data[i] != SymbolEndOfLine) && (i < tmpFile->Size); i++) {
				if (NeedBuildCurrentRowCode)
					CurrentRowCode += tmpFile->Data[i];
				else
					TextTable[CurrentColumnCode][CurrentRowCode] += tmpFile->Data[i];
			}
			// RowCode built, next blocks in this row contain data
			NeedBuildCurrentRowCode = false;
			CurrentColumnCode++;
			TextTable[CurrentColumnCode][CurrentRowCode].clear(); // initialize string before "+=" usage
			// we found SymbolEndOfLine in previous cycle, in order to prevent "i" changes, break cycle
			if (tmpFile->Data[i] == SymbolEndOfLine)
				break;
		}
		// move to next row
		CurrentColumnCode = 0;
		NeedBuildCurrentRowCode = true;
		CurrentRowCode.clear();
	}

	return 0;
}

/*
 * Get text for particular language.
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
 * Detect what characters was not generated (need for testing purposes).
 */
int vw_CheckFontCharsInText()
{
	if (TextTable.empty())
		return ERR_PARAMETERS;

	printf("Font characters detection start.\n");

	// note, i = 1 - first column contain index, not data, start from second
	for (unsigned int i = 1; i < TextTable[i].size(); i++) {
		for (auto tmpData : TextTable[i]) {
			std::u32string tmpDataUTF32 = vw_UTF8toUTF32(tmpData.second);
			for (auto UTF32 : tmpDataUTF32) {
				if (!vw_CheckFontCharByUTF32(UTF32))
					printf("!!! FontChar was not created, Unicode: (0x%04X)\n", UTF32);
			}
		}
	}

	printf("Font characters detection end.\n\n");
	return 0;
}
