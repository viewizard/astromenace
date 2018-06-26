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

#include "big_asteroid.h"
#include "../../../config/config.h"
#include "../../../assets/texture.h"


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cBigAsteroid::Create(int AsteroidNum)
{
	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::BigAsteroid;

	// задали первичный, нужно случайным образом найти
	if (AsteroidNum == 1 || AsteroidNum == 2) {
		int Rand = 1 + vw_iRandNum(4);
		if (Rand < 1) Rand = 1;
		if (Rand > 5) Rand = 5;
		AsteroidNum = AsteroidNum*10 + Rand;
	}
	InternalType = AsteroidNum;


	// перебираем и ставим нужные данные
	switch (AsteroidNum) {
	case 11:
		LoadObjectData("models/space/bigasteroid-01.vw3d", this);
		goto case9next;
	case 12:
		LoadObjectData("models/space/bigasteroid-02.vw3d", this);
		goto case9next;
	case 13:
		LoadObjectData("models/space/bigasteroid-03.vw3d", this);
		goto case9next;
	case 14:
		LoadObjectData("models/space/bigasteroid-04.vw3d", this);
		goto case9next;
	case 15:
		LoadObjectData("models/space/bigasteroid-05.vw3d", this);

case9next:

		//Speed = 25.0f;
		RotationSpeed.x = 10.0f + 10.0f*vw_Randf0;
		RotationSpeed.y = 2.0f + 20.0f*vw_Randf0;

		Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		break;


	// те же астероиды, но они летят сверху...
	case 21:
		LoadObjectData("models/space/bigasteroid-01.vw3d", this);
		goto case10next;
	case 22:
		LoadObjectData("models/space/bigasteroid-02.vw3d", this);
		goto case10next;
	case 23:
		LoadObjectData("models/space/bigasteroid-03.vw3d", this);
		goto case10next;
	case 24:
		LoadObjectData("models/space/bigasteroid-04.vw3d", this);
		goto case10next;
	case 25:
		LoadObjectData("models/space/bigasteroid-05.vw3d", this);

case10next:

		//Speed = 25.0f;
		RotationSpeed.x = 10.0f + 10.0f*vw_Randf0;
		RotationSpeed.y = 2.0f + 20.0f*vw_Randf0;

		Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		break;
	}
}
