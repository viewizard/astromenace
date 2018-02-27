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

#ifndef FONT_H
#define FONT_H

#include "../core.h"
#include <ft2build.h>
#include FT_FREETYPE_H

// Font initialization by font name (path to file).
int vw_InitFont(const std::string &FontName);
// Set current font size.
void vw_SetFontSize(int FontSize);
// Set font offset.
void vw_SetFontOffsetY(int NewOffsetY);
// Generate font characters by list.
void vw_GenerateFontChars(int FontTextureWidth, int FontTextureHeight, const char *CharsList);
// Check font character by UTF32 code.
bool vw_CheckFontCharByUTF32(unsigned UTF32);
// Release all font characters and created for this characters textures.
void vw_ReleaseAllFontChars();
// Shutdown font.
void vw_ShutdownFont();

// Get string size with current font size.
int vw_FontSize(const char *Text, ...);
// Draw text with current font.
void vw_DrawFont(int X, int Y, float StrictWidth, float ExpandWidth, float FontScale,
		 float R, float G, float B, float Transp, const char *Text, ...);
// Draw 3D text with current font.
void vw_DrawFont3D(float X, float Y, float Z, const char *Text, ...);

#endif // FONT_H
