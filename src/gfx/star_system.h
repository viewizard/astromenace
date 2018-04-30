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

#ifndef STARSYSTEM_H
#define STARSYSTEM_H

// FIXME should be fixed, use function-based interface, instead of direct variables access
extern float StarsTileStartTransparentLayer1;
extern float StarsTileEndTransparentLayer1;
extern float StarsTileStartTransparentLayer2;
extern float StarsTileEndTransparentLayer2;


void StarSystemInit(int Num, sVECTOR3D SetBaseRotation);
void StarSystemResetTime(float Time);
void StarSystemDraw(int DrawType); // 1- меню, 2- игра
void StarSystemDrawSecondLayer(int DrawType);
void StarSystemRelease();

#endif // STARSYSTEM_H
