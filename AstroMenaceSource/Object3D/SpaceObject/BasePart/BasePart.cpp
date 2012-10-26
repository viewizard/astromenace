/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: BasePart.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


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
			LoadObjectData("DATA/MODELS/SPACEBASE/1/1.VW3D", this, 0, -1.0f);
			goto case1next;
		case 12:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/2.VW3D", this, 0, -1.0f);
			goto case1next;
		case 13:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/3.VW3D", this, 0, -1.0f);
			goto case1next;
		case 14:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/4.VW3D", this, 0, -1.0f);
			goto case1next;
		case 15:
			LoadObjectData("DATA/MODELS/SPACEBASE/1/5.VW3D", this, 0, -1.0f);

case1next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/aster.jpg");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[1] = 0;
			Texture[2] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.jpg");
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
			LoadObjectData("DATA/MODELS/SPACEBASE/2/1.VW3D", this, 0, -1.0f);
			goto case2next;
		case 22:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/2.VW3D", this, 0, -1.0f);
			goto case2next;
		case 23:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/3.VW3D", this, 0, -1.0f);
			goto case2next;
		case 24:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/4.VW3D", this, 0, -1.0f);
			goto case2next;
		case 25:
			LoadObjectData("DATA/MODELS/SPACEBASE/2/5.VW3D", this, 0, -1.0f);

case2next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/aster.jpg");
			TextureIllum[1] = 0;
			Texture[2] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.jpg");
			TextureIllum[2] = 0;
			break;

		case 3:
			// геометрия
			LoadObjectData("DATA/MODELS/SPACEBASE/3/1.VW3D", this, 0, -1.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.jpg");
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
			LoadObjectData("DATA/MODELS/SPACEBASE/6/1.VW3D", this, 0, -1.0f);
			goto case6next;
		case 62:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/2.VW3D", this, 0, -1.0f);
			goto case6next;
		case 63:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/3.VW3D", this, 0, -1.0f);
			goto case6next;
		case 64:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/4.VW3D", this, 0, -1.0f);
			goto case6next;
		case 65:
			LoadObjectData("DATA/MODELS/SPACEBASE/6/5.VW3D", this, 0, -1.0f);

case6next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/aster.jpg");
			TextureIllum[1] = 0;
			break;

		case 7:
			// геометрия
			LoadObjectData("DATA/MODELS/SPACEBASE/7/1.VW3D", this, 0, -1.0f);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			break;

		case 81:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/1.VW3D", this, 0, -1.0f);
			goto case8next;
		case 82:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/2.VW3D", this, 0, -1.0f);
			goto case8next;
		case 83:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/3.VW3D", this, 0, -1.0f);
			goto case8next;
		case 84:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/4.VW3D", this, 0, -1.0f);
			goto case8next;
		case 85:
			LoadObjectData("DATA/MODELS/SPACEBASE/8/5.VW3D", this, 0, -1.0f);

case8next:

			Texture[0] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/allalpha.tga");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/metal.jpg");
			TextureIllum[1] = 0;
			Texture[2] = vw_FindTextureByName("DATA/MODELS/SPACEBASE/aster.jpg");
			TextureIllum[2] = 0;
			break;

	}



	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();

}



