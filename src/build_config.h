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

#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

#define GAME_VERSION		"1.3.9 git"
#define GAME_VFS_BUILD		180505



// joystick support, required SDL compilled with "joystick" flag
#define joystick

// define DATADIR with gamedata.vfs file location if you don't put it together in the same directory with binary file.
// Use cmake with DATADIR flag OR uncomment line below
// #define DATADIR "/usr/share/astromenace"

// game configuration file will be stored with game binary file
// #define portable

#endif // BUILD_CONFIG_H
