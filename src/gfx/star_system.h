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

#ifndef GFX_STARSYSTEM_H
#define GFX_STARSYSTEM_H

#include "../enum.h"

// Star system initialization.
void StarSystemInit(int Num, sVECTOR3D SetBaseRotation);
// Star system initialization by game's part type (menu/game).
void StarSystemInitByType(eDrawType DrawType);
// Setup first and second layers transparency.
void StarSystemLayer1Transp(float Start, float End);
// Setup third layer transparency.
void StarSystemLayer3Transp(float Start, float End);
// Draw star system.
void StarSystemDraw(eDrawType DrawType);
// Draw third layer of space dust.
void StarSystemDrawThirdLayer(eDrawType DrawType);
// Release star system.
void StarSystemRelease();

#endif // GFX_STARSYSTEM_H
