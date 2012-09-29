/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Base.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#ifndef Base_H
#define Base_H

#include "../Defines.h"



#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _CRT_SECURE_NO_DEPRECATE // выключаем задалбывание с безопасными функциями
	#include <windows.h>
	#include <shellapi.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
	#include <OpenGL/gl.h>	// Header File For The OpenGL32 Library
	#include <OpenGL/glu.h>	// Header File For The GLu32 Library
#else
	#define __glext_h_  // Don't let gl.h include glext.h
	#include <GL/gl.h>	// Header File For The OpenGL32 Library
	#include <GL/glu.h>	// Header File For The GLu32 Library
	#undef __glext_h_
#endif

#include <stdlib.h> // rand, ...
#include <math.h> // math for core Math


#include "RendererInterface/GLext.h"
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_syswm.h"
#include "SDL/SDL_endian.h" // для VFS, чтобы правильно считывать таблицу файлов + хранение данных игры




inline void itoa10(int val, char* res)
{
    // http://www.cplusplus.com/ref/cstdio/sprintf.html
    // если нужны не 10-ки
    sprintf(res,"%i",val);
}



#ifdef __unix

// декларируем типы данных, которых можен не быть
#ifndef BYTE
	#define BYTE Uint8
#endif
#ifndef WORD
	#define WORD Uint16
#endif
#ifndef DWORD
	#define DWORD Uint32
#endif
#ifndef BOOL
	#define BOOL bool
#endif

	struct RECT
	{
		int left;
		int top;
		int right;
		int bottom;
	};
	inline void SetRect(RECT *rect, int left, int top, int right, int bottom)
	{
		rect->right = right;
		rect->top = top;
		rect->left = left;
		rect->bottom =bottom;
	}


	#include <valarray> // memcpy
	#include <stdarg.h> // va_start

	#include <sys/stat.h> // mkdir
	#include <sys/types.h> // mkdir


#ifndef MAX_PATH
	#define MAX_PATH 1024
#endif // MAX_PATH


#endif // unix





#endif // Base_H
