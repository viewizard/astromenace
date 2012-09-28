/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Defines.h

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


#ifndef DEFINES_H
#define DEFINES_H


// required libXinerama
#define xinerama

// required SDL compilled with "joystick" flag
#define joystick

// separate thread for music during data loading (based on SDL multithread implementation)
#define multithread

// enable compression during gamedata.vfs creation, game will take more time for loading in exchange of size reducing (about 30 MB)
// #define compression

// define DATADIR with gamedata.vfs file location if you don't put it together in the same directory with binary file
// #define DATADIR "/usr/share/AstroMenace"





#define GAME_VERSION_ID	 	1.3f
#define GAME_VERSION_BUILD 	120928


#endif // DEFINES_H
