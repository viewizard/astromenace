/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: config.h

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#ifndef CONFIG_H
#define CONFIG_H

#define GAME_VERSION	 	1.3
#define GAME_BUILD			121010


// VBO OpenGL extention support switcher (if defined - game will detect and use VBO) Game could crash
// in FreeBSD under VirtualBox if vbo are used. Added just in case, if hardware capabilities detection fail.
// Looks like only virtualized systems could have this issue.
#define vbo

// VAO OpenGL extention support switcher (if defined - game will detect and use VAO), VAO will not work without VBO
// Intel video cards might have a problem with VAO (index buffer issue in intel video drivers)
#define vao

// xinerama support, required libXinerama
#define xinerama

// joystick support, required SDL compilled with "joystick" flag
#define joystick

// separate thread for music during data loading (based on SDL multithread implementation)
#define multithread

// compression during gamedata.vfs creation, game will take more time for loading in exchange of size reducing (about 30 MB)
// #define compression

// define DATADIR with gamedata.vfs file location if you don't put it together in the same directory with binary file
// #define DATADIR "/usr/share/AstroMenace"



// more actions and text outputs with debug information for bug hunting
#define gamedebug

// linux libSDL issue (at least <1.2.15), can't create game window if more than 4x MSAA
#ifdef __unix
#define forced4xmsaa
#endif

// compile with build-in tynixml lib (version 2.6.2)
#define buildin_tynixml

// PNG image format support via libpng. Don't use png images right now, disabled atm
// #define png

#endif // CONFIG_H
