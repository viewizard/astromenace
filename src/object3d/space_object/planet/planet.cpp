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
#include "planet.h"





//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cPlanet::Create(int PlanetNum)
{
	ObjectType = 14;
	ObjectStatus = 1; // чужой

	ObjectCreationType = PlanetNum;


	// перебираем и ставим нужные данные
	switch (PlanetNum) {

	// планета с астероидным кольцом
	case 1:
		NeedCullFaces = false;
		NeedAlphaTest = true;
		LoadObjectData("models/planet/aplanet.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		Texture[0] = vw_FindTextureByName("models/planet/d_class3.tga");
		TextureIllum[0] = nullptr;
		NormalMap[0] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		Texture[1] = vw_FindTextureByName("models/planet/asteroid.tga");
		TextureIllum[1] = nullptr;
		NormalMap[1] = vw_FindTextureByName("models/normalmap/planet_asteroids_nm.tga");
		Texture[2] = vw_FindTextureByName("models/planet/asteroid.tga");
		TextureIllum[2] = nullptr;
		NormalMap[2] = vw_FindTextureByName("models/normalmap/planet_asteroids_nm.tga");
		Texture[3] = vw_FindTextureByName("models/planet/asteroid.tga");
		TextureIllum[3] = nullptr;
		NormalMap[3] = vw_FindTextureByName("models/normalmap/planet_asteroids_nm.tga");
		break;
	// полу-разрушенная планета
	case 2:
		PromptDrawDist2 = 100.0f; // только для этой планеты ставим такое !!!
		LoadObjectData("models/planet/dplanet.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		for (int i=0; i<DrawObjectQuantity; i++) {
			Texture[i] = vw_FindTextureByName("models/planet/d_class3.tga");
			TextureIllum[i] = nullptr;
			NormalMap[i] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		}
		break;
	// планета с атмосферой
	case 3:
		NeedCullFaces = false;
		LoadObjectData("models/planet/gplanet.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		Texture[0] = vw_FindTextureByName("models/planet/m_class7.tga");
		TextureIllum[0] = nullptr;
		NormalMap[0] = vw_FindTextureByName("models/normalmap/m_class7_nm.tga");

		Texture[1] = vw_FindTextureByName("models/planet/clouds.tga");
		TextureIllum[1] = nullptr;
		DrawObjectList[1].DrawType = 1; // рисуем прозрачным
		break;
	// "красная" планета
	case 4:
		LoadObjectData("models/planet/moon.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		Texture[0] = vw_FindTextureByName("models/planet/q_class2.tga");
		TextureIllum[0] = nullptr;
		NormalMap[0] = vw_FindTextureByName("models/normalmap/q_class2_nm.tga");
		break;
	// планета пришельцев, лава
	case 5:
		LoadObjectData("models/planet/planet5.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		Texture[0] = vw_FindTextureByName("models/planet/a_class4.tga");
		TextureIllum[0] = nullptr;
		NormalMap[0] = vw_FindTextureByName("models/normalmap/a_class4_nm.tga");
		break;
	// планета пришельцев
	case 6:
		LoadObjectData("models/planet/planet6.vw3d", this, 0, -1.0f, Setup.UseGLSL);
		Texture[0] = vw_FindTextureByName("models/planet/d_class3.tga");
		TextureIllum[0] = nullptr;
		NormalMap[0] = vw_FindTextureByName("models/normalmap/d_class3_nm.tga");
		break;
	}


	// находим все данные по геометрии
	::cObject3D::InitByDrawObjectList();

}



