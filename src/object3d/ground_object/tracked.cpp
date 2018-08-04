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

struct sTrackedData {
	float Strength;
	float SpeedToWheelRotarySpeedFactor;
	std::string Model3DFileName;
	std::string TextureFileName;
};

const std::vector<sTrackedData> PresetTrackedData{
	{250,	60.0f,	"models/tracked/tank-01.vw3d", "models/gr-01.vw2d"},
	{200,	45.0f,	"models/tracked/tank-03.vw3d", "models/gr-01.vw2d"},
	{300,	45.0f,	"models/tracked/tank-05.vw3d", "models/gr-06.vw2d"},
	{300,	55.0f,	"models/tracked/tank-06.vw3d", "models/gr-03.vw2d"},
	{400,	60.0f,	"models/tracked/tank-07.vw3d", "models/gr-06.vw2d"},
	{250,	70.0f,	"models/tracked/tank-08.vw3d", "models/gr-01.vw2d"},
	{400,	60.0f,	"models/tracked/tank-09.vw3d", "models/gr-01.vw2d"},
	{300,	60.0f,	"models/tracked/tank-10.vw3d", "models/gr-03.vw2d"},
	{350,	60.0f,	"models/tracked/tank-11.vw3d", "models/gr-03.vw2d"},
	{300,	60.0f,	"models/tracked/apc-01.vw3d", "models/gr-03.vw2d"},
	{400,	60.0f,	"models/tracked/apc-03.vw3d", "models/gr-03.vw2d"},
	{250,	60.0f,	"models/tracked/apc-aa-01.vw3d", "models/gr-03.vw2d"},
	{200,	42.0f,	"models/tracked/apc-aa-02.vw3d", "models/gr-02.vw2d"},
	{50,	50.0f,	"models/tracked/engineering-01.vw3d", "models/gr-03.vw2d"}
};


/*
 * Constructor.
 */
cTracked::cTracked(const int TrackedNum)
{
	if ((TrackedNum <= 0) ||
	    (static_cast<unsigned>(TrackedNum) > PresetTrackedData.size())) {
		std::cerr << __func__ << "(): "
			  << "Could not init cTracked object with Number "
			  << TrackedNum << "\n";
		return;
	}

	ObjectType = eObjectType::PirateVehicle;
	MaxSpeed = 20.0f;
	MaxAcceler = 5.0f;
	MaxSpeedRotate = 20.0f;
	InternalType = TrackedNum;

	NeedCullFaces = false;

	LoadObjectData(PresetTrackedData[TrackedNum - 1].Model3DFileName, *this);

	for (unsigned int i = 0; i < Chunks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetTrackedData[TrackedNum - 1].TextureFileName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToWheelRotarySpeedFactor = PresetTrackedData[TrackedNum - 1].SpeedToWheelRotarySpeedFactor;

	Strength = StrengthStart = PresetTrackedData[TrackedNum - 1].Strength / GameEnemyArmorPenalty;

	switch (TrackedNum) {
	case 1:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{0.0f, 5.5f, 9.0f});

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);
		WheelChunkNums.emplace_back(17);
		WheelChunkNums.emplace_back(18);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(5);

		TargetVertChunkNums.emplace_back(5);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		break;

	case 2:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.1f, 6.1f, -0.4f});
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-0.1f, 6.1f, -0.4f});
		WeaponFireType = 2;

		WheelChunkNums.reserve(10);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(11);
		TargetHorizChunkNums.emplace_back(12);

		TargetVertChunkNums.emplace_back(12);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 13;
		break;

	case 3:
		WeaponSlots.emplace_back(CreateWeapon(213), sVECTOR3D{0.0f, 5.2f, 3.7f});

		WheelChunkNums.reserve(14);
		WheelChunkNums.emplace_back(0);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(8);
		TargetHorizChunkNums.emplace_back(9);

		TargetVertChunkNums.emplace_back(9);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 17;
		break;

	case 4:
		WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D{0.0f, 5.3f, 6.5f});

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetHorizChunkNums.reserve(3);
		TargetHorizChunkNums.emplace_back(17);
		TargetHorizChunkNums.emplace_back(18);
		TargetHorizChunkNums.emplace_back(19);

		TargetVertChunkNums.emplace_back(18);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 20;
		break;

	case 5:
		WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D{0.0f, 5.8f, 4.5f});

		WheelChunkNums.reserve(14);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(9);
		TargetHorizChunkNums.emplace_back(8);

		TargetVertChunkNums.emplace_back(8);
		TargetVertChunksMaxAngle = 30.0f;

		TrackChunkNum = 17;
		break;

	case 6:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{0.0f, 4.9f, 4.0f});

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);
		WheelChunkNums.emplace_back(17);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(1);
		TargetHorizChunkNums.emplace_back(18);

		TargetVertChunkNums.emplace_back(18);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		break;

	case 7:
		WeaponSlots.emplace_back(CreateWeapon(212), sVECTOR3D{0.0f, 7.6f, 5.5f});

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);
		WheelChunkNums.emplace_back(17);
		WheelChunkNums.emplace_back(18);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(12);
		TargetHorizChunkNums.emplace_back(11);

		TargetVertChunkNums.emplace_back(11);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		break;

	case 8:
		WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D{0.0f, 7.0f, 8.5f});

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(17);
		TargetHorizChunkNums.emplace_back(18);

		TargetVertChunkNums.emplace_back(18);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		break;

	case 9:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{0.0f, 6.7f, 6.8f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(18);
		TargetHorizChunkNums.emplace_back(0);

		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		break;

	case 10:
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.0f, 6.1f, 0.5f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(18);
		TargetHorizChunkNums.emplace_back(0);

		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		break;

	case 11:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{2.2f, 5.4f, 7.0f});
		WeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{-2.2f, 5.4f, 7.0f});
		WeaponFireType = 3;

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 55.0f;

		TrackChunkNum = 18;
		break;

	case 12:
		WeaponSlots.reserve(4);
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{0.55f, 5.0f, 2.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-0.55f, 5.0f, 2.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{1.65f, 5.0f, 2.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-1.65f, 5.0f, 2.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		WheelChunkNums.reserve(16);
		WheelChunkNums.emplace_back(1);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);
		WheelChunkNums.emplace_back(16);

		TargetVertChunkNums.reserve(2);
		TargetVertChunkNums.emplace_back(18);
		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 40.0f;

		TrackChunkNum = 19;
		break;

	case 13:
		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{1.4f, 5.0f, 0.4f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-1.4f, 5.0f, 0.4f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		WheelChunkNums.reserve(14);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);
		WheelChunkNums.emplace_back(9);
		WheelChunkNums.emplace_back(10);
		WheelChunkNums.emplace_back(11);
		WheelChunkNums.emplace_back(12);
		WheelChunkNums.emplace_back(13);
		WheelChunkNums.emplace_back(14);
		WheelChunkNums.emplace_back(15);

		TargetHorizChunkNums.reserve(2);
		TargetHorizChunkNums.emplace_back(0);
		TargetHorizChunkNums.emplace_back(16);

		TargetVertChunkNums.emplace_back(16);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 17;
		break;

	case 14:
		WheelChunkNums.reserve(8);
		WheelChunkNums.emplace_back(0);
		WheelChunkNums.emplace_back(2);
		WheelChunkNums.emplace_back(3);
		WheelChunkNums.emplace_back(4);
		WheelChunkNums.emplace_back(5);
		WheelChunkNums.emplace_back(6);
		WheelChunkNums.emplace_back(7);
		WheelChunkNums.emplace_back(8);

		TrackChunkNum = 10;
		TrackRotationDirection = -1;
		break;
	}

	assert(TrackChunkNum >= 0);
	assert(TrackChunkNum < static_cast<int>(Texture.size()));
	Texture[TrackChunkNum] = GetPreloadedTextureAsset("models/track.vw2d");

	// randomize wheels initial rotation for better look
	for (const auto &WheelChunkNum : WheelChunkNums) {
		Chunks[WheelChunkNum].Rotation.x = vw_fRandNum(360.0f);
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
