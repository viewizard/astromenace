/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Planet.cpp

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
#include "Planet.h"





//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CPlanet::Create(int PlanetNum)
{
	ObjectType = 14;
	ObjectStatus = 1; // чужой

	ObjectCreationType = PlanetNum;

	// устанавливаем кому надо шейдеры, а кому нет
	int ShaderType = 1;


	// перебираем и ставим нужные данные
	switch (PlanetNum)
	{

		// планета с астероидным кольцом
		case 1:
			ShaderType = -1;
			NeedCullFaces = false;
			NeedAlphaTest = true;
			LoadObjectData("DATA/MODELS/PLANET/aplanet.VW3D", this, 0);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/planet3.jpg");
			TextureIllum[0] = 0;
			Texture[1] = vw_FindTextureByName("DATA/MODELS/PLANET/asteroid.tga");
			TextureIllum[1] = 0;
			Texture[2] = vw_FindTextureByName("DATA/MODELS/PLANET/asteroid.tga");
			TextureIllum[2] = 0;
			Texture[3] = vw_FindTextureByName("DATA/MODELS/PLANET/asteroid.tga");
			TextureIllum[3] = 0;
			break;
		// полу разрушенная планета
		case 2:
			ShaderType = -1;
			PromptDrawDist2 = 100.0f; // только для этой планеты ставим такое !!!
			LoadObjectData("DATA/MODELS/PLANET/dplanet.VW3D", this, 0);
			for (int i=0; i<DrawObjectQuantity; i++)
			{
				Texture[i] = vw_FindTextureByName("DATA/MODELS/PLANET/planet3.jpg");
				TextureIllum[i] = 0;
			}
			break;
		// планета с атмосферой
		case 3:
			ShaderType = -1;
			NeedCullFaces = false;
			LoadObjectData("DATA/MODELS/PLANET/gplanet.VW3D", this, 0);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/planet1.jpg");
			TextureIllum[0] = 0;

			Texture[1] = vw_FindTextureByName("DATA/MODELS/PLANET/clouds.tga");
			TextureIllum[1] = 0;
			DrawObjectList[1].DrawType = 1; // рисуем прозрачным
			break;
		// луна
		case 4:
			ShaderType = -1;
			LoadObjectData("DATA/MODELS/PLANET/moon.VW3D", this, 0);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/moon.tga");
			TextureIllum[0] = 0;
			break;
		// планета пришельцев, с подсветкой
		case 5:
			LoadObjectData("DATA/MODELS/PLANET/planet5.VW3D", this, 0);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/p5base.jpg");
			TextureIllum[0] = vw_FindTextureByName("DATA/MODELS/PLANET/p5i.tga");
			break;
		// планета пришельцев, _без_ подсветки
		case 6:
			ShaderType = -1;
			LoadObjectData("DATA/MODELS/PLANET/planet6.VW3D", this, 0);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/planet3.jpg");
			TextureIllum[0] = 0;
			break;
	}


	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();


	if (ShaderType == -1)
		for (int j=0; j<DrawObjectQuantity; j++)
		{
			// это слишком большие объекты + практически не освещаются и плоские, т.е. попиксельное освещение не нужно
			DrawObjectList[j].ShaderType = -1;
		}
}



