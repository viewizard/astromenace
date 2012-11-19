/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: BigAsteroid.cpp

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
#include "BigAsteroid.h"




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CBigAsteroid::Create(int AsteroidNum)
{
	ObjectStatus = 1; // чужой
	ObjectType = 15;

	// задали первичный, нужно случайным образом найти
	if (AsteroidNum == 1 || AsteroidNum == 2)
	{
		int Rand = 1 + vw_iRandNum(4);
		if (Rand < 1) Rand = 1;
		if (Rand > 5) Rand = 5;
		AsteroidNum = AsteroidNum*10 + Rand;
	}
	ObjectCreationType = AsteroidNum;


	// перебираем и ставим нужные данные
	switch (AsteroidNum)
	{
		case 11:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-01.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case9next;
		case 12:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-02.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case9next;
		case 13:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-03.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case9next;
		case 14:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-04.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case9next;
		case 15:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-05.VW3D", this, 0, -1.0f, Setup.UseGLSL);

case9next:

			//Speed = 25.0f;
			RotationSpeed.x = 10.0f + 10.0f*vw_Randf0;
			RotationSpeed.y = 2.0f + 20.0f*vw_Randf0;

			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			break;


		// те же астероиды, но они летят сверху...
		case 21:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-01.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case10next;
		case 22:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-02.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case10next;
		case 23:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-03.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case10next;
		case 24:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-04.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			goto case10next;
		case 25:
			LoadObjectData("DATA/MODELS/SPACE/bigasteroid-05.VW3D", this, 0, -1.0f, Setup.UseGLSL);

case10next:

			//Speed = 25.0f;
			RotationSpeed.x = 10.0f + 10.0f*vw_Randf0;
			RotationSpeed.y = 2.0f + 20.0f*vw_Randf0;

			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			break;
	}



	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();
}



