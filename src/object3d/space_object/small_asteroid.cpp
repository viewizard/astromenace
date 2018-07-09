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

// TODO translate comments

#include "space_object.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cSmallAsteroid::cSmallAsteroid()
{
	ObjectType = eObjectType::SmallAsteroid;

	Strength = StrengthStart = 10.0f / GameEnemyArmorPenalty;

	std::string Model3DFileName{"models/space/asteroid-01" + std::to_string(vw_iRandNum(19)) + ".vw3d"};
	LoadObjectData(Model3DFileName, *this);

	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset("models/space/asteroid-01.tga");
	}

	RotationSpeed.x = 100.0f + 50.0f * vw_fRand0();
	RotationSpeed.y = 20.0f + 20.0f * vw_fRand0();

	// небольшая поправка... в зависимости от размеров объекта
	Strength = StrengthStart = Radius * StrengthStart;

	// ставим правильный радиус
	Radius = Width / 2.0f;
}

} // astromenace namespace
} // viewizard namespace
