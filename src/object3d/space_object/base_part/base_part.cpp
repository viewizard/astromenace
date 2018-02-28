/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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


/// подключаем нужные файлы
#include "base_part.h"




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cBasePart::Create(int BasePartNum)
{
	ObjectType = 13;
	ObjectStatus = 1; // чужой
	NeedAlphaTest = true;

	// задали первичный, нужно случайным образом найти
	if (BasePartNum == 1 || BasePartNum == 2 || BasePartNum == 6 || BasePartNum == 8) {
		int Rand = 1 + vw_iRandNum(4);
		if (Rand < 1) Rand = 1;
		if (Rand > 5) Rand = 5;
		BasePartNum = BasePartNum*10 + Rand;
	}
	ObjectCreationType = BasePartNum;

	// перебираем и ставим нужные данные
	switch (BasePartNum) {
	case 11:
		LoadObjectData("models/spacebase/1/1.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case1next;
	case 12:
		LoadObjectData("models/spacebase/1/2.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case1next;
	case 13:
		LoadObjectData("models/spacebase/1/3.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case1next;
	case 14:
		LoadObjectData("models/spacebase/1/4.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case1next;
	case 15:
		LoadObjectData("models/spacebase/1/5.vw3d", this, 0, -1.0f, Setup.UseGLSL);

case1next:

		Texture[0] = vw_FindTextureByName("models/planet/d_class3.tga");
		TextureIllum[0] = nullptr;
		NormalMap[0] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		Texture[1] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[1] = nullptr;
		Texture[2] = vw_FindTextureByName("models/spacebase/metal.tga");
		TextureIllum[2] = nullptr;

		GFXQuantity = 8;
		GFXLocation = new sVECTOR3D[GFXQuantity];
		GFX = new cParticleSystem*[GFXQuantity];
		for (int i = 0; i < GFXQuantity; i++) GFX[i] = nullptr;

		GFX[0] = new cParticleSystem;
		GFXLocation[0] = sVECTOR3D(14.3f, -4.0f, -14.3f);
		SetSpaceObjectGFX(GFX[0], 1);
		GFX[1] = new cParticleSystem;
		GFXLocation[1] = sVECTOR3D(14.3f, -4.0f, 14.3f);
		SetSpaceObjectGFX(GFX[1], 1);
		GFX[2] = new cParticleSystem;
		GFXLocation[2] = sVECTOR3D(-14.3f, -4.0f, -14.3f);
		SetSpaceObjectGFX(GFX[2], 1);
		GFX[3] = new cParticleSystem;
		GFXLocation[3] = sVECTOR3D(-14.3f, -4.0f, 14.3f);
		SetSpaceObjectGFX(GFX[3], 1);
		//----
		GFX[4] = new cParticleSystem;
		GFXLocation[4] = sVECTOR3D(14.3f, 3.2f, -14.3f);
		SetSpaceObjectGFX(GFX[4], 2);
		GFX[5] = new cParticleSystem;
		GFXLocation[5] = sVECTOR3D(14.3f, 3.2f, 14.3f);
		SetSpaceObjectGFX(GFX[5], 2);
		GFX[6] = new cParticleSystem;
		GFXLocation[6] = sVECTOR3D(-14.3f, 3.2f, -14.3f);
		SetSpaceObjectGFX(GFX[6], 2);
		GFX[7] = new cParticleSystem;
		GFXLocation[7] = sVECTOR3D(-14.3f, 3.2f, 14.3f);
		SetSpaceObjectGFX(GFX[7], 2);
		break;

	case 21:
		LoadObjectData("models/spacebase/2/1.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case2next;
	case 22:
		LoadObjectData("models/spacebase/2/2.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case2next;
	case 23:
		LoadObjectData("models/spacebase/2/3.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case2next;
	case 24:
		LoadObjectData("models/spacebase/2/4.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case2next;
	case 25:
		LoadObjectData("models/spacebase/2/5.vw3d", this, 0, -1.0f, Setup.UseGLSL);

case2next:

		Texture[0] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[0] = nullptr;
		Texture[1] = vw_FindTextureByName("models/planet/d_class3.tga");
		TextureIllum[1] = nullptr;
		NormalMap[1] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		Texture[2] = vw_FindTextureByName("models/spacebase/metal.tga");
		TextureIllum[2] = nullptr;
		break;

	case 3:
		// геометрия
		LoadObjectData("models/spacebase/3/1.vw3d", this, 0, -1.0f);
		Texture[0] = vw_FindTextureByName("models/spacebase/metal.tga");
		TextureIllum[0] = nullptr;
		Texture[1] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[1] = nullptr;

		GFXQuantity = 2;
		GFXLocation = new sVECTOR3D[GFXQuantity];
		GFX = new cParticleSystem*[GFXQuantity];
		for (int i = 0; i < GFXQuantity; i++) GFX[i] = nullptr;

		GFX[0] = new cParticleSystem;
		GFXLocation[0] = sVECTOR3D(0.0f, -5.0f, 13.7f);
		SetSpaceObjectGFX(GFX[0], 1);
		//----
		GFX[1] = new cParticleSystem;
		GFXLocation[1] = sVECTOR3D(0.0f, 5.0f, 13.7f);
		SetSpaceObjectGFX(GFX[1], 2);
		break;

	case 4:
		// геометрия
		LoadObjectData("models/spacebase/4/1.vw3d", this, 0, -1.0f);
		Texture[0] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[0] = nullptr;
		break;

	case 5:
		// геометрия
		LoadObjectData("models/spacebase/5/1.vw3d", this, 0, -1.0f);
		Texture[0] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[0] = nullptr;
		break;

	case 61:
		LoadObjectData("models/spacebase/6/1.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case6next;
	case 62:
		LoadObjectData("models/spacebase/6/2.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case6next;
	case 63:
		LoadObjectData("models/spacebase/6/3.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case6next;
	case 64:
		LoadObjectData("models/spacebase/6/4.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case6next;
	case 65:
		LoadObjectData("models/spacebase/6/5.vw3d", this, 0, -1.0f, Setup.UseGLSL);

case6next:

		Texture[0] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[0] = nullptr;
		Texture[1] = vw_FindTextureByName("models/planet/d_class3.tga");
		TextureIllum[1] = nullptr;
		NormalMap[1] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		break;

	case 7:
		// геометрия
		LoadObjectData("models/spacebase/7/1.vw3d", this, 0, -1.0f);
		Texture[0] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[0] = nullptr;
		break;

	case 81:
		LoadObjectData("models/spacebase/8/1.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case8next;
	case 82:
		LoadObjectData("models/spacebase/8/2.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case8next;
	case 83:
		LoadObjectData("models/spacebase/8/3.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case8next;
	case 84:
		LoadObjectData("models/spacebase/8/4.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		goto case8next;
	case 85:
		LoadObjectData("models/spacebase/8/5.vw3d", this, 0, -1.0f, Setup.UseGLSL);

case8next:

		Texture[0] = vw_FindTextureByName("models/spacebase/allalpha.tga");
		TextureIllum[0] = nullptr;
		Texture[1] = vw_FindTextureByName("models/spacebase/metal.tga");
		TextureIllum[1] = nullptr;
		Texture[2] = vw_FindTextureByName("models/planet/d_class3.tga");
		TextureIllum[2] = nullptr;
		NormalMap[2] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		break;

	}



	// находим все данные по геометрии
	::cObject3D::InitByDrawObjectList();

}



