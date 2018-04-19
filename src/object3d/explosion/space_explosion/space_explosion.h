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

#ifndef SPACEEXPLOSION_H
#define SPACEEXPLOSION_H

#include "../explosion.h"


/*
 * All space ships explosions.
 */
class cSpaceExplosion : public cExplosion
{
public:
	cSpaceExplosion(cObject3D *Object, int ExplType, const sVECTOR3D &ExplLocation,
			float Speed, int ObjectPieceNum, bool NeedExplosionSFX = true);
	virtual ~cSpaceExplosion() {};
};


#endif // SPACEEXPLOSION_H
