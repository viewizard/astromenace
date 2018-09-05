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

#include "../base.h"
#include <random> // this one should not being used in other parts of code

namespace viewizard {

namespace {

std::random_device rd;
std::default_random_engine gen(rd());

} // unnamed namespace

/*
 * Generate random float in range [0.0f, 1.0f).
 */
float vw_fRand()
{
	return std::generate_canonical<float, 10>(gen);
}

/*
 * Generate random float in range [0.0f, Max) or (Max, 0.0f] in case Max is negative.
 */
float vw_fRandNum(float Max)
{
	return vw_fRand() * Max;
}

/*
 * Generate random unsigned integer in range [0, Max].
 */
unsigned vw_uRandNum(unsigned Max)
{
	return static_cast<unsigned>(std::floor(vw_fRand() * static_cast<float>(Max + 1)));
}

} // viewizard namespace
