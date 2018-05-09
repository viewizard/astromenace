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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

/*
All menu elements should be pre generated.
Exceptions: profiles pilots name, players names in top scores.
Check game log for lines:
```
Texture created from memory: fontsize_10_character_1103
Font character was created for size: 10,  char: 'я',  code: 0x44F
```
and correct GenerateFonts() in order to avoid separate textures
usage for fonts in menu elements.
*/

#include "../core/core.h"
#include "../build_config.h"

namespace {

constexpr unsigned MainFontSize{16};

} // unnamed namespace


/*
 * Initialize font.
 */
void InitFont(const std::string &FontName)
{
	vw_InitFont(FontName);
	vw_SetFontSize(MainFontSize);
}

/*
 * Reset font size to main font size.
 */
void ResetFontSize()
{
	vw_SetFontSize(MainFontSize);
}

/*
 * Generate in-game font for particular size.
 */
static void GenerateFont(unsigned FontTextureWidth, unsigned FontTextureHeight,
			 unsigned FontSize, std::string &Symbols)
{
	std::unordered_set<char32_t> tmpCharsSet{};
	if (FontSize == MainFontSize)
		tmpCharsSet = vw_FindCharsSetForLanguage();

	std::u32string tmpSymbolsUTF32 = ConvertUTF8.from_bytes(Symbols);
	for (const auto &UTF32 : tmpSymbolsUTF32) {
		tmpCharsSet.insert(UTF32);
	}
	vw_SetFontSize(FontSize);
	vw_GenerateFontChars(FontTextureWidth, FontTextureHeight, tmpCharsSet);
	vw_SetFontSize(MainFontSize);
}

/*
 * Generate all in-game fonts for all sizes.
 */
void GenerateFonts()
{
	// MainFontSize
	// add all languages' names
	std::string tmpSymbols{};
	for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
		tmpSymbols += vw_GetText("English", i);
	}
	GenerateFont(256, 256, MainFontSize, tmpSymbols);

	// 10
	std::string tmpSymbols10{"Copyright © 2007-2018, Viewizard"};
	tmpSymbols10 += vw_GetText("Version");
	tmpSymbols10 += GAME_VERSION;
	GenerateFont(128, 128, 10, tmpSymbols10);

	// 24
	std::string tmpSymbols24{};
	tmpSymbols24 += vw_GetText("System Stock");
	tmpSymbols24 += vw_GetText("Installed Systems");
	tmpSymbols24 += vw_GetText("Weapon Stock");
	tmpSymbols24 += vw_GetText("Installed Weapons");
	tmpSymbols24 += vw_GetText("Ship Stock");
	tmpSymbols24 += vw_GetText("Player Ship");
	tmpSymbols24 += vw_GetText("English");
	tmpSymbols24 += vw_GetText("Missile Detected");
	tmpSymbols24 += vw_GetText("Collision Course Detected");
	GenerateFont(256, 256, 24, tmpSymbols24);

	// 20
	std::string tmpSymbols20{"0123456789x."};
	tmpSymbols20 += vw_GetText("Money");
	tmpSymbols20 += vw_GetText("Game Speed:");
	GenerateFont(128, 128, 20, tmpSymbols20);
}
