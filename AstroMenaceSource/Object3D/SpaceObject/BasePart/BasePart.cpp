/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


/// подключаем нужные файлы
#include "BasePart.h"




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CBasePart::Create(int BasePartNum)
{
	ObjectType = 13;
	ObjectStatus = 1; // чужой
	NeedAlphaTest = true;

	// задали первичный, нужно случайным образом найти
	if (BasePartNum == 1 || BasePartNum == 2 || BasePartNum == 6 || BasePartNum == 8)
	{
		int Rand = 1 + vw_iRandNum(4);
		if (Rand < 1) Rand = 1;
		if (Rand > 5) Rand = 5;
		BasePartNum = BasePartNum*10 + Rand;
	}
	ObjectCreationType = BasePartNum;

	// перебираем и ставим нужные данные
	switch (BasePartNum)
	{
		case 11:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/1.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case1next;
		case 12:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/2.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case1next;
		case 13:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/3.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case1next;
		case 14:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/4.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case1next;
		case 15:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/5.VW3D", this, 0, -1.0f, Setup.UseGLSL);

case1next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[1] = 0;
			Texture[2] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.tga");
			TextureIllum[2] = 0;

			GFXQuantity = 8;
			GFXLocation = new VECTOR3D[GFXQuantity];
			GFX = new eParticleSystem*[GFXQuantity];
			for (int i=0; i<GFXQuantity; i++) GFX[i] = 0;

			GFX[0] = new eParticleSystem;
			GFXLocation[0] = VECTOR3D(14.3f, -4.0f, -14.3f);
			SetSpaceObjectGFX(GFX[0], 1);
			GFX[1] = new eParticleSystem;
			GFXLocation[1] = VECTOR3D(14.3f, -4.0f, 14.3f);
			SetSpaceObjectGFX(GFX[1], 1);
			GFX[2] = new eParticleSystem;
			GFXLocation[2] = VECTOR3D(-14.3f, -4.0f, -14.3f);
			SetSpaceObjectGFX(GFX[2], 1);
			GFX[3] = new eParticleSystem;
			GFXLocation[3] = VECTOR3D(-14.3f, -4.0f, 14.3f);
			SetSpaceObjectGFX(GFX[3], 1);
			//----
			GFX[4] = new eParticleSystem;
			GFXLocation[4] = VECTOR3D(14.3f, 3.2f, -14.3f);
			SetSpaceObjectGFX(GFX[4], 2);
			GFX[5] = new eParticleSystem;
			GFXLocation[5] = VECTOR3D(14.3f, 3.2f, 14.3f);
			SetSpaceObjectGFX(GFX[5], 2);
			GFX[6] = new eParticleSystem;
			GFXLocation[6] = VECTOR3D(-14.3f, 3.2f, -14.3f);
			SetSpaceObjectGFX(GFX[6], 2);
			GFX[7] = new eParticleSystem;
			GFXLocation[7] = VECTOR3D(-14.3f, 3.2f, 14.3f);
			SetSpaceObjectGFX(GFX[7], 2);
			break;

		case 21:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/1.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case2next;
		case 22:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/2.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case2next;
		case 23:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/3.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case2next;
		case 24:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/4.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case2next;
		case 25:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/5.VW3D", this, 0, -1.0f, Setup.UseGLSL);

case2next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[1] = 0;
			NormalMap[1] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			Texture[2] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.tga");
			TextureIllum[2] = 0;
			break;

		case 3:
			// геометрия
			LoadObjectData("DATA/MODELS/SPACEBASE/3/1.VW3D", this, 0, -1.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[1] = 0;

			GFXQuantity = 2;
			GFXLocation = new VECTOR3D[GFXQuantity];
			GFX = new eParticleSystem*[GFXQuantity];
			for (int i=0; i<GFXQuantity; i++) GFX[i] = 0;

			GFX[0] = new eParticleSystem;
			GFXLocation[0] = VECTOR3D(0.0f, -5.0f, 13.7f);
			SetSpaceObjectGFX(GFX[0], 1);
			//----
			GFX[1] = new eParticleSystem;
			GFXLocation[1] = VECTOR3D(0.0f, 5.0f, 13.7f);
			SetSpaceObjectGFX(GFX[1], 2);
			break;

		case 4:
			// геометрия
			LoadObjectData("DATA/MODELS/SPACEBASE/4/1.VW3D", this, 0, -1.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			break;

		case 5:
			// геометрия
			LoadObjectData("DATA/MODELS/SPACEBASE/5/1.VW3D", this, 0, -1.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			break;

		case 61:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/1.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case6next;
		case 62:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/2.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case6next;
		case 63:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/3.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case6next;
		case 64:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/4.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case6next;
		case 65:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/5.VW3D", this, 0, -1.0f, Setup.UseGLSL);

case6next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[1] = 0;
			NormalMap[1] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			break;

		case 7:
			// геометрия
			LoadObjectData("DATA/MODELS/SPACEBASE/7/1.VW3D", this, 0, -1.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			break;

		case 81:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/1.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case8next;
		case 82:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/2.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case8next;
		case 83:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/3.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case8next;
		case 84:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/4.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case8next;
		case 85:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/5.VW3D", this, 0, -1.0f, Setup.UseGLSL);

case8next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.tga");
			TextureIllum[1] = 0;
			Texture[2] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[2] = 0;
			NormalMap[2] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			break;

	}



	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();

}



