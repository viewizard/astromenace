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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "base_part.h"
#include "../../../config/config.h"
#include "../../../assets/texture.h"



//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cBasePart::Create(int BasePartNum)
{
	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::BasePart;
	NeedAlphaTest = true;

	// задали первичный, нужно случайным образом найти
	if (BasePartNum == 1 || BasePartNum == 2 || BasePartNum == 6 || BasePartNum == 8) {
		int Rand = 1 + vw_iRandNum(4);
		if (Rand < 1) Rand = 1;
		if (Rand > 5) Rand = 5;
		BasePartNum = BasePartNum*10 + Rand;
	}
	InternalType = BasePartNum;

	// перебираем и ставим нужные данные
	switch (BasePartNum) {
	case 11:
		LoadObjectData("models/spacebase/1/1.vw3d", this);
		goto case1next;
	case 12:
		LoadObjectData("models/spacebase/1/2.vw3d", this);
		goto case1next;
	case 13:
		LoadObjectData("models/spacebase/1/3.vw3d", this);
		goto case1next;
	case 14:
		LoadObjectData("models/spacebase/1/4.vw3d", this);
		goto case1next;
	case 15:
		LoadObjectData("models/spacebase/1/5.vw3d", this);

case1next:

		Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		Texture[1] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[1] = 0;
		Texture[2] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[2] = 0;

		GraphicFXLocation.resize(8);
		GraphicFX.resize(8);

		GraphicFX[0] = vw_CreateParticleSystem();
		GraphicFXLocation[0] = sVECTOR3D(14.3f, -4.0f, -14.3f);
		if (auto sharedGFX = GraphicFX[0].lock())
			SetSpaceObjectGFX(sharedGFX, 1);
		GraphicFX[1] = vw_CreateParticleSystem();
		GraphicFXLocation[1] = sVECTOR3D(14.3f, -4.0f, 14.3f);
		if (auto sharedGFX = GraphicFX[1].lock())
			SetSpaceObjectGFX(sharedGFX, 1);
		GraphicFX[2] = vw_CreateParticleSystem();
		GraphicFXLocation[2] = sVECTOR3D(-14.3f, -4.0f, -14.3f);
		if (auto sharedGFX = GraphicFX[2].lock())
			SetSpaceObjectGFX(sharedGFX, 1);
		GraphicFX[3] = vw_CreateParticleSystem();
		GraphicFXLocation[3] = sVECTOR3D(-14.3f, -4.0f, 14.3f);
		if (auto sharedGFX = GraphicFX[3].lock())
			SetSpaceObjectGFX(sharedGFX, 1);
		//----
		GraphicFX[4] = vw_CreateParticleSystem();
		GraphicFXLocation[4] = sVECTOR3D(14.3f, 3.2f, -14.3f);
		if (auto sharedGFX = GraphicFX[4].lock())
			SetSpaceObjectGFX(sharedGFX, 2);
		GraphicFX[5] = vw_CreateParticleSystem();
		GraphicFXLocation[5] = sVECTOR3D(14.3f, 3.2f, 14.3f);
		if (auto sharedGFX = GraphicFX[5].lock())
			SetSpaceObjectGFX(sharedGFX, 2);
		GraphicFX[6] = vw_CreateParticleSystem();
		GraphicFXLocation[6] = sVECTOR3D(-14.3f, 3.2f, -14.3f);
		if (auto sharedGFX = GraphicFX[6].lock())
			SetSpaceObjectGFX(sharedGFX, 2);
		GraphicFX[7] = vw_CreateParticleSystem();
		GraphicFXLocation[7] = sVECTOR3D(-14.3f, 3.2f, 14.3f);
		if (auto sharedGFX = GraphicFX[7].lock())
			SetSpaceObjectGFX(sharedGFX, 2);
		break;

	case 21:
		LoadObjectData("models/spacebase/2/1.vw3d", this);
		goto case2next;
	case 22:
		LoadObjectData("models/spacebase/2/2.vw3d", this);
		goto case2next;
	case 23:
		LoadObjectData("models/spacebase/2/3.vw3d", this);
		goto case2next;
	case 24:
		LoadObjectData("models/spacebase/2/4.vw3d", this);
		goto case2next;
	case 25:
		LoadObjectData("models/spacebase/2/5.vw3d", this);

case2next:

		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[1] = 0;
		NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		Texture[2] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[2] = 0;
		break;

	case 3:
		// геометрия
		LoadObjectData("models/spacebase/3/1.vw3d", this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[1] = 0;

		GraphicFXLocation.resize(2);
		GraphicFX.resize(2);

		GraphicFX[0] = vw_CreateParticleSystem();
		GraphicFXLocation[0] = sVECTOR3D(0.0f, -5.0f, 13.7f);
		if (auto sharedGFX = GraphicFX[0].lock())
			SetSpaceObjectGFX(sharedGFX, 1);
		//----
		GraphicFX[1] = vw_CreateParticleSystem();
		GraphicFXLocation[1] = sVECTOR3D(0.0f, 5.0f, 13.7f);
		if (auto sharedGFX = GraphicFX[1].lock())
			SetSpaceObjectGFX(sharedGFX, 2);
		break;

	case 4:
		// геометрия
		LoadObjectData("models/spacebase/4/1.vw3d", this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		break;

	case 5:
		// геометрия
		LoadObjectData("models/spacebase/5/1.vw3d", this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		break;

	case 61:
		LoadObjectData("models/spacebase/6/1.vw3d", this);
		goto case6next;
	case 62:
		LoadObjectData("models/spacebase/6/2.vw3d", this);
		goto case6next;
	case 63:
		LoadObjectData("models/spacebase/6/3.vw3d", this);
		goto case6next;
	case 64:
		LoadObjectData("models/spacebase/6/4.vw3d", this);
		goto case6next;
	case 65:
		LoadObjectData("models/spacebase/6/5.vw3d", this);

case6next:

		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[1] = 0;
		NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		break;

	case 7:
		// геометрия
		LoadObjectData("models/spacebase/7/1.vw3d", this);
		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		break;

	case 81:
		LoadObjectData("models/spacebase/8/1.vw3d", this);
		goto case8next;
	case 82:
		LoadObjectData("models/spacebase/8/2.vw3d", this);
		goto case8next;
	case 83:
		LoadObjectData("models/spacebase/8/3.vw3d", this);
		goto case8next;
	case 84:
		LoadObjectData("models/spacebase/8/4.vw3d", this);
		goto case8next;
	case 85:
		LoadObjectData("models/spacebase/8/5.vw3d", this);

case8next:

		Texture[0] = GetPreloadedTextureAsset("models/spacebase/allalpha.tga");
		TextureIllum[0] = 0;
		Texture[1] = GetPreloadedTextureAsset("models/spacebase/metal.tga");
		TextureIllum[1] = 0;
		Texture[2] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[2] = 0;
		NormalMap[2] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		break;

	}
}
