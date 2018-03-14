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

#ifndef CONFIG_H
#define CONFIG_H

#define GAME_VERSION		"1.9.9 git"
#define GAME_BUILD		180315



// joystick support, required SDL compilled with "joystick" flag
#define joystick

// define DATADIR with gamedata.vfs file location if you don't put it together in the same directory with binary file.
// Use cmake with DATADIR flag OR uncomment line below
// #define DATADIR "/usr/share/astromenace"

// game configuration file will be stored with game binary file
// #define portable

// fontconfig support.
// With fontconfig you don't need fonts bundled with game, system TrueType, Type 1 or CFF fonts will be used instead.
// Make sure, that you have at least one installed bold style TrueType, Type 1 or CFF font with en, de and ru languages support.
// ./gamedata/font/ folder can be removed in case of fontconfig use.
//#ifdef fontconfig_detected_by_cmake // do not comment this line, unless you are sure what are you doing
//	#define fontconfig
//#endif // fontconfig_detected_by_cmake

// Default font family for game text rendering in case of fontconfig use.
// Please note, font family name could be vary in different distros.
// Recomended fonts as dafault game font (were tested for correct ingame text rendering):
// 1) Linux Libertine/Linux Biolinum
#define default_font_family "Linux Biolinum O"
//#define default_font_family "Linux Libertine O"
// 2) Liberation Fonts
//#define default_font_family "Liberation Mono"
//#define default_font_family "Liberation Sans"
//#define default_font_family "Liberation Serif"
// 3) GNU FreeFont
//#define default_font_family "FreeMono"
//#define default_font_family "FreeSans"
//#define default_font_family "FreeSerif"




// more actions and text outputs with debug information for bug hunting
// #define gamedebug

#endif /* CONFIG_H */
