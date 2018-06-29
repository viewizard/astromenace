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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef CORE_BASE_H
#define CORE_BASE_H

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE // suppress warnings
#include <windows.h>
#include <shellapi.h>
#endif

#include "SDL2/SDL.h"

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

// suppress warnings about unused variables, when we need to
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

namespace viewizard {

// functor in order to use enumeration as a key (std::map, std::unordered_map, std::unordered_multimap)
struct sEnumHash {
	template <typename T>
	std::size_t operator () (T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

// integral + floating point dual type
// caller should care about types size and numeric limits
template <typename I, typename F>
struct sIF_dual_type {
public:
	// caller should guarantee, that integral value will not exceed floating point value size
	explicit sIF_dual_type(const I _i) :
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
	sIF_dual_type &operator = (const I _i)
	{
		__i = _i;
		__f = static_cast<F>(_i);
		return *this;
	}

	// caller should guarantee, that integral value will not exceed floating point value size
	void operator () (const I _i)
	{
		__i = _i;
		__f = static_cast<F>(_i);
	}

	bool operator == (sIF_dual_type &_dual)
	{
		// since both parts synchronized, we need only one check
		return (__i == _dual.i());
	}

private:
	// don't allow direct access, we should guarantee, that all
	// parts have proper value, only public methods should be used
	I __i;
	F __f;
};

/*
 * Compile-time Bernstein hash (djb2a) calculation.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
constexpr unsigned constexpr_hash_djb2a(const char *str, unsigned h = 0)
{
	return !str[h] ? 5381 : (constexpr_hash_djb2a(str, h + 1) * 33) ^ static_cast<unsigned>(str[h]);
}
#pragma GCC diagnostic pop

// error codes
constexpr int ERR_PARAMETERS{-1};	// function's parameters related issue
constexpr int ERR_FILE_NOT_FOUND{-2};	// file not found
constexpr int ERR_FILE_IO{-3};		// file I/O issue
constexpr int ERR_VFS_BUILD{-4};	// VFS build number issue
constexpr int ERR_EXT_RES{-5};		// issue with external resources
constexpr int ERR_MEM{-6};		// memory allocation related issue
constexpr int ERR_NOT_SUPPORTED{-7};	// file format not supported

} // viewizard namespace

#endif // CORE_BASE_H
