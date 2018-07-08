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

#include "ground_object.h"
#include "../../assets/texture.h"
#include "../weapon/weapon.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

struct sMilitaryBuildingData {
	float Strength;
	std::string Model3DFileName;
	std::string TextureFileName;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::vector<sMilitaryBuildingData> PresetMilitaryBuildingData{
	{80,	"models/militarybuilding/aa-gun-01.vw3d", "models/gr-01.vw2d"},
	{120,	"models/militarybuilding/aa-gun-02.vw3d", "models/gr-01.vw2d"},
	{80,	"models/militarybuilding/aa-gun-03.vw3d", "models/gr-02.vw2d"},
	{150,	"models/militarybuilding/aa-gun-04.vw3d", "models/gr-01.vw2d"},
	{250,	"models/militarybuilding/aa-gun-05.vw3d", "models/gr-03.vw2d"},
	{50,	"models/militarybuilding/artiler-gun-01.vw3d", "models/gr-03.vw2d"},
	{200,	"models/militarybuilding/artiler-gun-02.vw3d", "models/gr-01.vw2d"}
};
#pragma GCC diagnostic pop


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cMilitaryBuilding::cMilitaryBuilding(int MilitaryBuildingNum)
{
	if ((MilitaryBuildingNum <= 0) || (static_cast<unsigned>(MilitaryBuildingNum) > PresetMilitaryBuildingData.size())) {
		std::cerr << __func__ << "(): "
			  << "Could not init cMilitaryBuilding object with Number " << MilitaryBuildingNum << "\n";
		return;
	}

	ObjectType = eObjectType::PirateBuilding;
	InternalType = MilitaryBuildingNum;

	LoadObjectData(PresetMilitaryBuildingData[MilitaryBuildingNum - 1].Model3DFileName, this);

	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetMilitaryBuildingData[MilitaryBuildingNum - 1].TextureFileName);
	}

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;

	Strength = StrengthStart = PresetMilitaryBuildingData[MilitaryBuildingNum - 1].Strength/GameEnemyArmorPenalty;

	// установка доп. текстуры и других настроек для каждой модели
	switch (MilitaryBuildingNum) {
	case 1:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(0.3f, 4.6f, 2.6f));
		Weapon[1](CreateWeapon(204), sVECTOR3D(-0.3f, 4.6f, 2.6f));
		WeaponFireType = 2;

		TargetHorizBlocks.resize(4);
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 2;
		TargetHorizBlocks[2] = 3;
		TargetHorizBlocks[3] = 4;

		TargetVertBlocks.resize(3);
		TargetVertBlocks[0] = 2;
		TargetVertBlocks[1] = 3;
		TargetVertBlocks[2] = 4;
		TargetVertBlocksMaxAngle = 60.0f;
		break;

	case 2:
		Weapon.emplace_back(CreateWeapon(204), sVECTOR3D(0.0f, 4.6f, 4.5f));
		Weapon.back().Weapon->NextFireTime /= 3.0f;
		WeaponFireType = 2;

		TargetHorizBlocks.resize(4);
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 2;
		TargetHorizBlocks[2] = 3;
		TargetHorizBlocks[3] = 4;

		TargetVertBlocks.resize(3);
		TargetVertBlocks[0] = 2;
		TargetVertBlocks[1] = 3;
		TargetVertBlocks[2] = 4;
		TargetVertBlocksMaxAngle = 60.0f;

		BarrelBlocks.emplace_back(4);
		break;

	case 3:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(205), sVECTOR3D(2.8f, 5.3f, 7.5f));
		Weapon[0].Weapon->NextFireTime /= 2.0f;
		Weapon[1](CreateWeapon(205), sVECTOR3D(-2.8f, 5.3f, 7.5f));
		Weapon[1].Weapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		TargetHorizBlocks.resize(2);
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 2;

		TargetVertBlocks.emplace_back(2);
		TargetVertBlocksMaxAngle = 60.0f;
		break;

	case 4:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(3.6f, 8.5f, 4.0f));
		Weapon[0].Weapon->NextFireTime /= 2.0f;
		Weapon[1](CreateWeapon(204), sVECTOR3D(-3.6f, 8.5f, 4.0f));
		Weapon[1].Weapon->NextFireTime /= 2.0f;
		WeaponFireType = 2;

		TargetHorizBlocks.resize(4);
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 2;
		TargetHorizBlocks[2] = 3;
		TargetHorizBlocks[3] = 4;

		TargetVertBlocks.resize(3);
		TargetVertBlocks[0] = 2;
		TargetVertBlocks[1] = 3;
		TargetVertBlocks[2] = 4;
		TargetVertBlocksMaxAngle = 60.0f;

		BarrelBlocks.resize(2);
		BarrelBlocks[0] = 3;
		BarrelBlocks[1] = 4;
		break;

	case 5:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(1.95f, 6.5f, 3.7f));
		Weapon[0].Weapon->NextFireTime /= 3.0f;
		Weapon[1](CreateWeapon(204), sVECTOR3D(-1.95f, 6.5f, 3.7f));
		Weapon[1].Weapon->NextFireTime /= 3.0f;
		WeaponFireType = 2;

		TargetHorizBlocks.resize(2);
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 2;

		TargetVertBlocks.emplace_back(2);
		TargetVertBlocksMaxAngle = 60.0f;
		break;

	case 6:
		Weapon.emplace_back(CreateWeapon(204), sVECTOR3D(0.0f, 3.0f, 7.0f));
		Weapon.back().Weapon->NextFireTime /= 3.5f;

		TargetHorizBlocks.resize(2);
		TargetHorizBlocks[0] = 2;
		TargetHorizBlocks[1] = 1;

		TargetVertBlocks.emplace_back(1);
		TargetVertBlocksMaxAngle = 80.0f;
		break;

	case 7:
		Weapon.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 5.2f, 10.3f));

		TargetHorizBlocks.resize(4);
		TargetHorizBlocks[0] = 4;
		TargetHorizBlocks[1] = 2;
		TargetHorizBlocks[2] = 3;
		TargetHorizBlocks[3] = 1;

		TargetVertBlocks.resize(3);
		TargetVertBlocks[0] = 3;
		TargetVertBlocks[1] = 2;
		TargetVertBlocks[2] = 1;
		TargetVertBlocksMaxAngle = 60.0f;

		BarrelBlocks.emplace_back(1);
		break;
	}

	// вычисляем данные для нахождения точки стрельбы
	if (!TargetHorizBlocks.empty()) {
		BaseBound = Model3DBlocks[TargetHorizBlocks[0]].Location;
	}

	if (!TargetVertBlocks.empty()) {
		if (!TargetHorizBlocks.empty())
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location - Model3DBlocks[TargetHorizBlocks[0]].Location;
		else
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location;
	}

	for (auto &tmpWeapon : Weapon) {
		if (!TargetVertBlocks.empty())
			tmpWeapon.Bound = tmpWeapon.Location - Model3DBlocks[TargetVertBlocks[0]].Location;
		else if (!TargetHorizBlocks.empty())
			tmpWeapon.Bound = tmpWeapon.Location - Model3DBlocks[TargetHorizBlocks[0]].Location;
		else
			tmpWeapon.Bound = tmpWeapon.Location;
	}
}

} // astromenace namespace
} // viewizard namespace
