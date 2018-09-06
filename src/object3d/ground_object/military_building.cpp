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
	float Armor;
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


/*
 * Constructor.
 */
cMilitaryBuilding::cMilitaryBuilding(const int MilitaryBuildingNum)
{
	if ((MilitaryBuildingNum <= 0) ||
	    (static_cast<unsigned>(MilitaryBuildingNum) > PresetMilitaryBuildingData.size())) {
		std::cerr << __func__ << "(): "
			  << "Could not init cMilitaryBuilding object with Number "
			  << MilitaryBuildingNum << "\n";
		return;
	}

	ObjectType = eObjectType::PirateBuilding;
	InternalType = MilitaryBuildingNum;

	LoadObjectData(PresetMilitaryBuildingData[MilitaryBuildingNum - 1].Model3DFileName, *this);

	for (unsigned int i = 0; i < Chunks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetMilitaryBuildingData[MilitaryBuildingNum - 1].TextureFileName);
	}

	ArmorCurrentStatus = StrengthStart = PresetMilitaryBuildingData[MilitaryBuildingNum - 1].Armor / GameEnemyArmorPenalty;

	switch (MilitaryBuildingNum) {
	case 1:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.3f, 4.6f, 2.6f});
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-0.3f, 4.6f, 2.6f});
		WeaponFireType = 2;

		TargetHorizChunkNums.reserve(4);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(2);
		TargetHorizChunkNums.emplace_back(3);
		TargetHorizChunkNums.emplace_back(4);

		TargetVertChunkNums.reserve(3);
		TargetVertChunkNums.emplace_back(2);
		TargetVertChunkNums.emplace_back(3);
		TargetVertChunkNums.emplace_back(4);
		TargetVertChunksMaxAngle = 60.0f;
		break;

	case 2:
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.0f, 4.6f, 4.5f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 3.0f;
		WeaponFireType = 2;

		TargetHorizChunkNums.reserve(4);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(2);
		TargetHorizChunkNums.emplace_back(3);
		TargetHorizChunkNums.emplace_back(4);

		TargetVertChunkNums.reserve(3);
		TargetVertChunkNums.emplace_back(2);
		TargetVertChunkNums.emplace_back(3);
		TargetVertChunkNums.emplace_back(4);
		TargetVertChunksMaxAngle = 60.0f;

		BarrelChunkNums.emplace_back(4);
		break;

	case 3:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{2.8f, 5.3f, 7.5f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-2.8f, 5.3f, 7.5f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(2);

		TargetVertChunkNums.emplace_back(2);
		TargetVertChunksMaxAngle = 60.0f;
		break;

	case 4:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{3.6f, 8.5f, 4.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-3.6f, 8.5f, 4.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponFireType = 2;

		TargetHorizChunkNums.reserve(4);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(2);
		TargetHorizChunkNums.emplace_back(3);
		TargetHorizChunkNums.emplace_back(4);

		TargetVertChunkNums.reserve(3);
		TargetVertChunkNums.emplace_back(2);
		TargetVertChunkNums.emplace_back(3);
		TargetVertChunkNums.emplace_back(4);
		TargetVertChunksMaxAngle = 60.0f;

		BarrelChunkNums.reserve(2);
		BarrelChunkNums.emplace_back(3);
		BarrelChunkNums.emplace_back(4);
		break;

	case 5:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{1.95f, 6.5f, 3.7f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 3.0f;
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-1.95f, 6.5f, 3.7f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 3.0f;
		WeaponFireType = 2;

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(2);

		TargetVertChunkNums.emplace_back(2);
		TargetVertChunksMaxAngle = 60.0f;
		break;

	case 6:
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.0f, 3.0f, 7.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 3.5f;

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(2);
		TargetHorizChunkNums.emplace_back(1);

		TargetVertChunkNums.emplace_back(1);
		TargetVertChunksMaxAngle = 80.0f;
		break;

	case 7:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{0.0f, 5.2f, 10.3f});

		TargetHorizChunkNums.reserve(4);
		TargetHorizChunkNums.emplace_back(4);
		TargetHorizChunkNums.emplace_back(2);
		TargetHorizChunkNums.emplace_back(3);
		TargetHorizChunkNums.emplace_back(1);

		TargetVertChunkNums.reserve(3);
		TargetVertChunkNums.emplace_back(3);
		TargetVertChunkNums.emplace_back(2);
		TargetVertChunkNums.emplace_back(1);
		TargetVertChunksMaxAngle = 60.0f;

		BarrelChunkNums.emplace_back(1);
		break;
	}

	if (!TargetHorizChunkNums.empty())
		BaseBound = Chunks[TargetHorizChunkNums[0]].Location;

	if (!TargetVertChunkNums.empty()) {
		if (!TargetHorizChunkNums.empty())
			MiddleBound = Chunks[TargetVertChunkNums[0]].Location - Chunks[TargetHorizChunkNums[0]].Location;
		else
			MiddleBound = Chunks[TargetVertChunkNums[0]].Location;
	}

	for (auto &tmpWeaponSlot : WeaponSlots) {
		if (!TargetVertChunkNums.empty())
			tmpWeaponSlot.Bound = tmpWeaponSlot.Location - Chunks[TargetVertChunkNums[0]].Location;
		else if (!TargetHorizChunkNums.empty())
			tmpWeaponSlot.Bound = tmpWeaponSlot.Location - Chunks[TargetHorizChunkNums[0]].Location;
		else
			tmpWeaponSlot.Bound = tmpWeaponSlot.Location;
	}
}

} // astromenace namespace
} // viewizard namespace
