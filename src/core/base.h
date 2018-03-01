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

// TODO should not be connected to core in this way

#ifndef Base_H
#define Base_H

#include "../config.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE // suppress warnings
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define __glext_h_	// Don't let gl.h include glext.h
#include <OpenGL/gl.h>	// Header File For The OpenGL Library
#include <OpenGL/glu.h>	// Header File For The GLu Library
#undef __glext_h_
#else
#define __glext_h_	// Don't let gl.h include glext.h
#include <GL/gl.h>	// Header File For The OpenGL Library
#include <GL/glu.h>	// Header File For The GLu Library
#undef __glext_h_
#endif

#include "graphics/glext.h"
#include "SDL2/SDL.h"

// suppress warnings about unused variables, when we need to
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#if defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif /* MAX_PATH */

#endif /* unix */


// STL
// only includes that really should being used in different parts of code
#include <unordered_map>
#include <forward_list>
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <algorithm>


// error codes
#define ERR_PARAMETERS		-1	// function's parameters related issue
#define ERR_FILE_NOT_FOUND	-2	// file not found
#define ERR_FILE_IO		-3	// file I/O issue
#define ERR_VFS_BUILD		-4	// VFS build number issue
#define ERR_EXT_RES		-5	// issue with external resources
#define ERR_MEM			-6	// memory allocation related issue


#endif // Base_H
