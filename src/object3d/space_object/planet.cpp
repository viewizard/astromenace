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

// TODO don't call GetPreloadedTextureAsset() all the time, use cached texture instead

#include "space_object.h"
#include "../../config/config.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Constructor.
 */
cPlanet::cPlanet(const int PlanetNum)
{
	ObjectStatus = eObjectStatus::none;
	ObjectType = eObjectType::Planet;

	InternalType = PlanetNum;

	switch (InternalType) {
	case 1:
		NeedCullFaces = false;
		NeedAlphaTest = true;
		LoadObjectData("models/planet/aplanet.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		Texture[1] = GetPreloadedTextureAsset("models/planet/asteroid.tga");
		TextureIllum[1] = 0;
		NormalMap[1] = GetPreloadedTextureAsset("models/normalmap/planet_asteroids_nm.tga");
		Texture[2] = GetPreloadedTextureAsset("models/planet/asteroid.tga");
		TextureIllum[2] = 0;
		NormalMap[2] = GetPreloadedTextureAsset("models/normalmap/planet_asteroids_nm.tga");
		Texture[3] = GetPreloadedTextureAsset("models/planet/asteroid.tga");
		TextureIllum[3] = 0;
		NormalMap[3] = GetPreloadedTextureAsset("models/normalmap/planet_asteroids_nm.tga");
		break;

	case 2:
		PromptDrawDist2 = 100.0f;
		LoadObjectData("models/planet/dplanet.vw3d", *this);
		for (unsigned int i = 0; i < Chunks.size(); i++) {
			Texture[i] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
			TextureIllum[i] = 0;
			NormalMap[i] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		}
		break;

	case 3:
		NeedCullFaces = false;
		LoadObjectData("models/planet/gplanet.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/planet/m_class7.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/m_class7_nm.tga");

		Texture[1] = GetPreloadedTextureAsset("models/planet/clouds.tga");
		TextureIllum[1] = 0;
		Chunks[1].DrawType = eModel3DDrawType::Blend;
		break;

	case 4:
		LoadObjectData("models/planet/moon.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/planet/q_class2.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/q_class2_nm.tga");
		break;

	case 5:
		LoadObjectData("models/planet/planet5.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/planet/a_class4.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/a_class4_nm.tga");
		break;

	case 6:
		LoadObjectData("models/planet/planet6.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/planet/d_class3.tga");
		TextureIllum[0] = 0;
		NormalMap[0] = GetPreloadedTextureAsset("models/normalmap/d_class3_nm.tga");
		break;
	}
}

} // astromenace namespace
} // viewizard namespace
