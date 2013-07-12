/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#ifndef CONFIG_H
#define CONFIG_H

#define GAME_VERSION	 	"1.3.2"
#define GAME_BUILD			130712


// VBO OpenGL extention support switcher (if defined - game will detect and use VBO) Game could crash
// in FreeBSD under VirtualBox if vbo are used. Added just in case, if hardware capabilities detection fail.
// Looks like only virtualized systems could have this issue.
#define vbo

// VAO OpenGL extention support switcher (if defined - game will detect and use VAO), VAO will not work without VBO
// Intel video cards might have a problem with VAO (index buffer issue in intel video drivers)
#define vao

// FBO OpenGL extention support switcher (if defined - game will detect and use FBO)
// Note, this will also take affect on anti aliasing support.
#define fbo

// xinerama support, required libXinerama
#ifdef xinerama_detected_by_cmake // do not comment this line, unless you are sure what are you doing
	#define xinerama
#endif // xinerama_detected_by_cmake

// joystick support, required SDL compilled with "joystick" flag
#define joystick

// separate thread for music during data loading (based on SDL multithread implementation)
// #define multithread

// compression during gamedata.vfs creation, game will take more time for loading in exchange of size reducing (about 30 MB)
// #define compression

// define DATADIR with gamedata.vfs file location if you don't put it together in the same directory with binary file.
// Use cmake with DATADIR flag OR uncomment line below
// #define DATADIR "/usr/share/astromenace"

// game configuration file will be stored with game binary file
// #define portable

// fontconfig support.
// With fontconfig you don't need fonts bundled with game, system TrueType, Type 1 or CFF fonts will be used instead.
// Make sure, that you have at least one installed bold style TrueType, Type 1 or CFF font with en, de and ru languages support.
// RAW_VFS_DATA/FONT/ folder can be removed in case of fontconfig use.
#ifdef fontconfig_detected_by_cmake // do not comment this line, unless you are sure what are you doing
//	#define fontconfig
#endif // fontconfig_detected_by_cmake

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








// Since game use content released under different licenses (GPL v3, OFL v1.1, CC BY-SA v3.0),
// you might need create separate VFS game data file (gamedata_cc.vfs) with CC BY-SA licensed content only.
// If not defined, all content merged into one VFS game data file (gamedata.vfs)
// #define separate_cc_vfs





// You can create standalone AstroMenaceFS2VFS util for gamedata.vfs file creation.
// Use cmake with FS2VFS flag OR
// Use CodeBlocks.cbp project file (you will need Code::Blocks IDE) with "FS2VFS Linux GCC" as Build target OR
// 1) Uncomment "#define vfs_pack_standalone" line
// 2) Make sure, that your compilation list consist only 4 source files:
//			MainFS2VFS.cpp
//			Core/VirtualFileSystem/VFS.cpp
//			Core/VirtualFileSystem/Haff.cpp
//			Core/VirtualFileSystem/RLE.cpp
//    and you have 2 header files - config.h and Core/VirtualFileSystem/VFS.h close to them.
//
// #define vfs_pack_standalone





// more actions and text outputs with debug information for bug hunting
// #define gamedebug

// PNG image format support via libpng.
// #define png

// JPEG image format support via libjpeg.
// #define jpeg

#endif // CONFIG_H
