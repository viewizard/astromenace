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

#define GAME_VERSION	 	"1.3.2 svn"
#define GAME_BUILD			130522


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
#define xinerama

// joystick support, required SDL compilled with "joystick" flag
#define joystick

// separate thread for music during data loading (based on SDL multithread implementation)
// #define multithread

// compression during gamedata.vfs creation, game will take more time for loading in exchange of size reducing (about 30 MB)
// #define compression

// define DATADIR with gamedata.vfs file location if you don't put it together in the same directory with binary file
// #define DATADIR "/usr/share/astromenace"

// game configuration file will be stored with game binary file
// #define portable





// Since game use content released under different licenses (GPL v3, OFL v1.1, CC BY-SA v3.0),
// you might need create separate VFS game data file (gamedata_cc.vfs) with CC BY-SA licensed content only.
// If not defined, all content merged into one VFS game data file (gamedata.vfs)
// #define separate_cc_vfs





// You can create standalone AstroMenaceFS2VFS util for gamedata.vfs file creation.
// Use CMakeLists.txt for your cmake with FS2VFS flag OR
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
