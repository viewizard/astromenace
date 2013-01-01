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
#include "Planet.h"





//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void CPlanet::Create(int PlanetNum)
{
	ObjectType = 14;
	ObjectStatus = 1; // чужой

	ObjectCreationType = PlanetNum;


	// перебираем и ставим нужные данные
	switch (PlanetNum)
	{

		// планета с астероидным кольцом
		case 1:
			NeedCullFaces = false;
			NeedAlphaTest = true;
			LoadObjectData("DATA/MODELS/PLANET/aplanet.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			Texture[1] = vw_FindTextureByName("DATA/MODELS/PLANET/asteroid.tga");
			TextureIllum[1] = 0;
			NormalMap[1] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/planet_asteroids_nm.tga");
			Texture[2] = vw_FindTextureByName("DATA/MODELS/PLANET/asteroid.tga");
			TextureIllum[2] = 0;
			NormalMap[2] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/planet_asteroids_nm.tga");
			Texture[3] = vw_FindTextureByName("DATA/MODELS/PLANET/asteroid.tga");
			TextureIllum[3] = 0;
			NormalMap[3] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/planet_asteroids_nm.tga");
			break;
		// полу-разрушенная планета
		case 2:
			PromptDrawDist2 = 100.0f; // только для этой планеты ставим такое !!!
			LoadObjectData("DATA/MODELS/PLANET/dplanet.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			for (int i=0; i<DrawObjectQuantity; i++)
			{
				Texture[i] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
				TextureIllum[i] = 0;
				NormalMap[i] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			}
			break;
		// планета с атмосферой
		case 3:
			NeedCullFaces = false;
			LoadObjectData("DATA/MODELS/PLANET/gplanet.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/m_class7.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/m_class7_nm.tga");

			Texture[1] = vw_FindTextureByName("DATA/MODELS/PLANET/clouds.tga");
			TextureIllum[1] = 0;
			DrawObjectList[1].DrawType = 1; // рисуем прозрачным
			break;
		// "красная" планета
		case 4:
			LoadObjectData("DATA/MODELS/PLANET/moon.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/q_class2.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/q_class2_nm.tga");
			break;
		// планета пришельцев, лава
		case 5:
			LoadObjectData("DATA/MODELS/PLANET/planet5.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/a_class4.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/a_class4_nm.tga");
			break;
		// планета пришельцев
		case 6:
			LoadObjectData("DATA/MODELS/PLANET/planet6.VW3D", this, 0, -1.0f, Setup.UseGLSL);
			Texture[0] = vw_FindTextureByName("DATA/MODELS/PLANET/d_class3.tga");
			TextureIllum[0] = 0;
			NormalMap[0] = vw_FindTextureByName("DATA/MODELS/NORMALMAP/d_class3_nm.tga");
			break;
	}


	// находим все данные по геометрии
	::CObject3D::InitByDrawObjectList();

}



