/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


    AstroMenace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AstroMenace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

#ifndef CORE_TEXT_TEXT_H
#define CORE_TEXT_TEXT_H

#include "../base.h"

namespace viewizard {

// Initialization. Load file with translation in .csv format (supported by LibreOffice Calc).
int vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine);
// Set default language.
void vw_SetTextLanguage(unsigned int Language);
// Get UTF8 text for particular language.
const std::string &vw_GetText(const std::string &ItemID,
                              unsigned int Language = 999999 /*should be greater than loaded languages number*/);
// Get UTF32 text for particular language.
const std::u32string &vw_GetTextUTF32(const std::string &ItemID,
                                      unsigned int Language = 999999 /*should be greater than loaded languages number*/);
// Find all used in text characters for current language.
std::unordered_set<char32_t> &vw_FindCharsSetForLanguage();
// Release data.
void vw_ReleaseText();
// Get available languages count.
unsigned int vw_GetLanguageListCount();

} // viewizard namespace

#endif // CORE_TEXT_TEXT_H
