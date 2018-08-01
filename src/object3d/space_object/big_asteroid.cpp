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

// FIXME split big asteroid code to big asteroid + 'planetoid' (backgroind big asteroid)

#include "space_object.h"
#include "../../config/config.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * cBigAsteroid.
 */
cBigAsteroid::cBigAsteroid(const int AsteroidNum)
{
	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::BigAsteroid;

	// we could provide an Asteroid type (1 or 2) and pick random asteroid, or asteroid number:
	// 11-15 - planetoids (backgroind big asteroids)
	// 21-25 - big asteroids
	if ((AsteroidNum == 1) ||
	    (AsteroidNum == 2))
		InternalType = AsteroidNum * 10 + 1 + vw_iRandNum(4);
	else
		InternalType = AsteroidNum;

	switch (InternalType) {
	case 11:
	case 21:
		LoadObjectData("models/space/bigasteroid-01.vw3d", *this);
		break;
	case 12:
	case 22:
		LoadObjectData("models/space/bigasteroid-02.vw3d", *this);
		break;
	case 13:
	case 23:
		LoadObjectData("models/space/bigasteroid-03.vw3d", *this);
		break;
	case 14:
	case 24:
		LoadObjectData("models/space/bigasteroid-04.vw3d", *this);
		break;
	case 15:
	case 25:
		LoadObjectData("models/space/bigasteroid-05.vw3d", *this);
		break;
	}

	RotationSpeed.x = 10.0f + 10.0f * vw_fRand0();
	RotationSpeed.y = 2.0f + 20.0f * vw_fRand0();

	Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
	TextureIllum[0] = 0;
	NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
}

} // astromenace namespace
} // viewizard namespace
