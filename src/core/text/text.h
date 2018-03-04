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

#ifndef TEXT_H
#define TEXT_H

#include "../base.h"

// Initialization. Load file with translation in .csv format (supported by LibreOffice Calc).
int vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine);
// Set default language.
void vw_SetTextLanguage(unsigned int Language);
// Get UTF8 text for particular language.
const char *vw_GetText(const char *ItemID, unsigned int Language = 999999 /*should be greater than loaded languages number*/);
// Get UTF32 text for particular language.
const std::u32string &vw_GetTextUTF32(const char *ItemID, unsigned int Language = 999999 /*should be greater than loaded languages number*/);
// Detect what characters was not generated (need for testing purposes).
int vw_CheckFontCharsInText();
// Release data.
void vw_ReleaseText();
// Get available languages count.
unsigned int vw_GetLanguageListCount();

#endif // TEXT_H
