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

#ifndef Base_H
#define Base_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE // suppress warnings
#include <windows.h>
#include <shellapi.h>
#endif

#include "SDL2/SDL.h"

// suppress warnings about unused variables, when we need to
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

enum class eRGBCOLOR {
	white,
	black,
	red,
	green,
	blue,
	yellow,
	orange
};

struct sRGBCOLOR {
	float r{0.0f};
	float g{0.0f};
	float b{0.0f};

	sRGBCOLOR() = default;
	sRGBCOLOR(float _r, float _g, float _b) :
		r{_r},
		g{_g},
		b{_b}
	{}
	sRGBCOLOR(eRGBCOLOR color)
	{
		switch (color) {
		case eRGBCOLOR::white:
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
			break;
		case eRGBCOLOR::black:
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::red:
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::green:
			r = 0.0f;
			g = 1.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::blue:
			r = 0.0f;
			g = 0.0f;
			b = 1.0f;
			break;
		case eRGBCOLOR::yellow:
			r = 1.0f;
			g = 1.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::orange:
			r = 1.0f;
			g = 0.5f;
			b = 0.0f;
			break;
		}
	}
};

// STL (only includes what's really used)
#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <list>
#include <memory>
#include <map>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <iostream>
#include <ctime>
#include <functional>
#include <cmath>
#include <cstdlib>

// don't setup NDEBUG in sources, use definition in build system instead
#include <cassert>

// error codes
#define ERR_PARAMETERS		-1	// function's parameters related issue
#define ERR_FILE_NOT_FOUND	-2	// file not found
#define ERR_FILE_IO		-3	// file I/O issue
#define ERR_VFS_BUILD		-4	// VFS build number issue
#define ERR_EXT_RES		-5	// issue with external resources
#define ERR_MEM			-6	// memory allocation related issue
#define ERR_NOT_SUPPORTED	-7	// file format not supported

// integral + floating point complex type
// caller should care about types size and numeric limits
template <typename I, typename F>
struct sIF_complex_type {
public:
	// caller should guarantee, that integral value will not exceed floating point value size
	sIF_complex_type(const I _i) :
		__i{_i},
		__f{static_cast<F>(_i)}
	{
		static_assert(std::is_integral<I>::value, "First variable's type should be integral.");
		static_assert(std::is_floating_point<F>::value, "Second variable's type should be floating-point.");
	}

	unsigned i()
	{
		return __i;
	}

	float f()
	{
		return __f;
	}

	// caller should guarantee, that integral value will not exceed floating point value size
	void operator () (const I _i)
	{
		__i = _i;
		__f = static_cast<F>(_i);
	}

	bool operator == (sIF_complex_type &_complex)
	{
		// since both parts synchronized, we need only one check
		return (__i == _complex.i());
	}

private:
	// don't allow direct access, we should guarantee, that all
	// parts have proper value, only public methods should be used
	I __i;
	F __f;
};

#endif // Base_H
