/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Base.h

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

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

#include "../config.h"



#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _CRT_SECURE_NO_DEPRECATE // выключаем задалбывание с безопасными функциями
	#include <windows.h>
	#include <shellapi.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
	#define __glext_h_  // Don't let gl.h include glext.h
	#include <OpenGL/gl.h>	// Header File For The OpenGL Library
	#include <OpenGL/glu.h>	// Header File For The GLu Library
	#undef __glext_h_
#else
	#define __glext_h_  // Don't let gl.h include glext.h
	#include <GL/gl.h>	// Header File For The OpenGL Library
	#include <GL/glu.h>	// Header File For The GLu Library
	#undef __glext_h_
#endif

#include <stdlib.h> // rand, ...
#include <math.h> // math for core Math


#include "RendererInterface/GLext.h"
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_syswm.h"
#include "SDL/SDL_endian.h" // для VFS, чтобы правильно считывать таблицу файлов + хранение данных игры



// если не используем переменные, и их нельзя убрать (используем прототип функции, к примеру)
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif



#if defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

// декларируем типы данных, которых может не быть
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
