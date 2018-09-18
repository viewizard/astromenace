/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef CORE_FONT_FONT_H
#define CORE_FONT_FONT_H

#include "../base.h"

namespace viewizard {

struct sRGBCOLOR;

// Font initialization by font name (path to file).
int vw_InitFont(const std::string &FontName);
// Set current font size.
void vw_SetFontSize(int FontSize);
// Generate font characters by list.
int vw_GenerateFontChars(unsigned FontTextureWidth, unsigned FontTextureHeight,
			 const std::unordered_set<char32_t> &CharsSetUTF32);
// Check font character by UTF32 code.
bool vw_CheckFontCharByUTF32(char32_t UTF32);
// Release all font characters and created for this characters textures.
void vw_ReleaseAllFontChars();
// Shutdown font.
void vw_ShutdownFont();

// Get string size with current font size.
int vw_TextWidthUTF32(const std::u32string &Text);
// Draw text with current font. Origin is upper left corner.
int vw_DrawTextUTF32(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		     const sRGBCOLOR &Color, float Transp, const std::u32string &Text);
// Draw 3D text with current font.
int vw_DrawText3DUTF32(float X, float Y, float Z, const std::u32string &Text);

// Get string size with current font size with variadic arguments.
int vw_TextWidth(const char *Text, ...);
// Draw text with current font with variadic arguments.
int vw_DrawText(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		const sRGBCOLOR &Color, float Transp, const char *Text, ...);

} // viewizard namespace

#endif // CORE_FONT_FONT_H
