/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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



