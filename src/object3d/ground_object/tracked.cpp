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


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cTracked::cTracked(int TrackedNum)
{
	if ((TrackedNum <= 0) || (static_cast<unsigned>(TrackedNum) > PresetTrackedData.size())) {
		std::cerr << __func__ << "(): " << "Could not init cTracked object with Number " << TrackedNum << "\n";
		return;
	}

	ObjectType = eObjectType::PirateVehicle;
	MaxSpeed = 20.0f;
	MaxAcceler = 5.0f;
	MaxSpeedRotate = 20.0f;
	InternalType = TrackedNum;

	// ставим без оптимизации, иначе не увидим гусениц
	NeedCullFaces = false;

	LoadObjectData(PresetTrackedData[TrackedNum - 1].Model3DFileName, *this);

	for (unsigned int i = 0; i < Chunks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetTrackedData[TrackedNum - 1].TextureFileName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToWheelRotarySpeedFactor = PresetTrackedData[TrackedNum - 1].SpeedToWheelRotarySpeedFactor;

	Strength = StrengthStart = PresetTrackedData[TrackedNum - 1].Strength/GameEnemyArmorPenalty;

	// установка доп. текстуры и других настроек для каждой модели
	switch (TrackedNum) {
	case 1:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 5.5f, 9.0f));

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 2;
		WheelChunkNums[1] = 3;
		WheelChunkNums[2] = 4;
		WheelChunkNums[3] = 6;
		WheelChunkNums[4] = 7;
		WheelChunkNums[5] = 8;
		WheelChunkNums[6] = 9;
		WheelChunkNums[7] = 10;
		WheelChunkNums[8] = 11;
		WheelChunkNums[9] = 12;
		WheelChunkNums[10] = 13;
		WheelChunkNums[11] = 14;
		WheelChunkNums[12] = 15;
		WheelChunkNums[13] = 16;
		WheelChunkNums[14] = 17;
		WheelChunkNums[15] = 18;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 1;
		TargetHorizChunkNums[1] = 5;

		TargetVertChunkNums.emplace_back(5);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 2:
		WeaponSlots.resize(2);
		WeaponSlots[0](CreateWeapon(204), sVECTOR3D(0.1f, 6.1f, -0.4f));
		WeaponSlots[1](CreateWeapon(204), sVECTOR3D(-0.1f, 6.1f, -0.4f));
		WeaponFireType = 2;

		WheelChunkNums.resize(10);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 11;
		TargetHorizChunkNums[1] = 12;

		TargetVertChunkNums.emplace_back(12);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 13;
		Texture[13] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 3:
		WeaponSlots.emplace_back(CreateWeapon(213), sVECTOR3D(0.0f, 5.2f, 3.7f));

		WheelChunkNums.resize(14);
		WheelChunkNums[0] = 0;
		WheelChunkNums[1] = 1;
		WheelChunkNums[2] = 2;
		WheelChunkNums[3] = 3;
		WheelChunkNums[4] = 4;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 10;
		WheelChunkNums[8] = 11;
		WheelChunkNums[9] = 12;
		WheelChunkNums[10] = 13;
		WheelChunkNums[11] = 14;
		WheelChunkNums[12] = 15;
		WheelChunkNums[13] = 16;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 8;
		TargetHorizChunkNums[1] = 9;

		TargetVertChunkNums.emplace_back(9);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 17;
		Texture[17] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 4:
		WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 5.3f, 6.5f));

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 11;
		WheelChunkNums[11] = 12;
		WheelChunkNums[12] = 13;
		WheelChunkNums[13] = 14;
		WheelChunkNums[14] = 15;
		WheelChunkNums[15] = 16;

		TargetHorizChunkNums.resize(3);
		TargetHorizChunkNums[0] = 17;
		TargetHorizChunkNums[1] = 18;
		TargetHorizChunkNums[2] = 19;

		TargetVertChunkNums.emplace_back(18);
		TargetVertChunksMaxAngle = 50.0f;

		Chunks[19].Location.z -= 1.0f;

		TrackChunkNum = 20;
		Texture[20] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 5:
		WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 5.8f, 4.5f));

		WheelChunkNums.resize(14);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 10;
		WheelChunkNums[8] = 11;
		WheelChunkNums[9] = 12;
		WheelChunkNums[10] = 13;
		WheelChunkNums[11] = 14;
		WheelChunkNums[12] = 15;
		WheelChunkNums[13] = 16;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 9;
		TargetHorizChunkNums[1] = 8;

		TargetVertChunkNums.emplace_back(8);
		TargetVertChunksMaxAngle = 30.0f;

		TrackChunkNum = 17;
		Texture[17] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 6:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 4.9f, 4.0f));

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 2;
		WheelChunkNums[1] = 3;
		WheelChunkNums[2] = 4;
		WheelChunkNums[3] = 5;
		WheelChunkNums[4] = 6;
		WheelChunkNums[5] = 7;
		WheelChunkNums[6] = 8;
		WheelChunkNums[7] = 9;
		WheelChunkNums[8] = 10;
		WheelChunkNums[9] = 11;
		WheelChunkNums[10] = 12;
		WheelChunkNums[11] = 13;
		WheelChunkNums[12] = 14;
		WheelChunkNums[13] = 15;
		WheelChunkNums[14] = 16;
		WheelChunkNums[15] = 17;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 1;
		TargetHorizChunkNums[1] = 18;

		TargetVertChunkNums.emplace_back(18);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 7:
		WeaponSlots.emplace_back(CreateWeapon(212), sVECTOR3D(0.0f, 7.6f, 5.5f));

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 13;
		WheelChunkNums[11] = 14;
		WheelChunkNums[12] = 15;
		WheelChunkNums[13] = 16;
		WheelChunkNums[14] = 17;
		WheelChunkNums[15] = 18;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 12;
		TargetHorizChunkNums[1] = 11;

		TargetVertChunkNums.emplace_back(11);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 8:
		WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 7.0f, 8.5f));

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 11;
		WheelChunkNums[11] = 12;
		WheelChunkNums[12] = 13;
		WheelChunkNums[13] = 14;
		WheelChunkNums[14] = 15;
		WheelChunkNums[15] = 16;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 17;
		TargetHorizChunkNums[1] = 18;

		TargetVertChunkNums.emplace_back(18);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 9:
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 6.7f, 6.8f));
		WeaponSlots.back().Weapon->NextFireTime /= 2.0f;

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 11;
		WheelChunkNums[11] = 12;
		WheelChunkNums[12] = 13;
		WheelChunkNums[13] = 14;
		WheelChunkNums[14] = 15;
		WheelChunkNums[15] = 16;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 18;
		TargetHorizChunkNums[1] = 0;

		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 10:
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D(0.0f, 6.1f, 0.5f));
		WeaponSlots.back().Weapon->NextFireTime /= 2.0f;

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 11;
		WheelChunkNums[11] = 12;
		WheelChunkNums[12] = 13;
		WheelChunkNums[13] = 14;
		WheelChunkNums[14] = 15;
		WheelChunkNums[15] = 16;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 18;
		TargetHorizChunkNums[1] = 0;

		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 11:
		WeaponSlots.resize(2);
		WeaponSlots[0](CreateWeapon(209), sVECTOR3D(2.2f, 5.4f, 7.0f));
		WeaponSlots[1](CreateWeapon(209), sVECTOR3D(-2.2f, 5.4f, 7.0f));
		WeaponFireType = 3;

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 11;
		WheelChunkNums[11] = 12;
		WheelChunkNums[12] = 13;
		WheelChunkNums[13] = 14;
		WheelChunkNums[14] = 15;
		WheelChunkNums[15] = 16;

		TargetVertChunkNums.emplace_back(0);
		TargetVertChunksMaxAngle = 55.0f;

		TrackChunkNum = 18;
		Texture[18] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 12:
		WeaponSlots.resize(4);
		WeaponSlots[0](CreateWeapon(206), sVECTOR3D(0.55f, 5.0f, 2.0f));
		WeaponSlots[0].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[1](CreateWeapon(206), sVECTOR3D(-0.55f, 5.0f, 2.0f));
		WeaponSlots[1].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[2](CreateWeapon(206), sVECTOR3D(1.65f, 5.0f, 2.0f));
		WeaponSlots[2].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[3](CreateWeapon(206), sVECTOR3D(-1.65f, 5.0f, 2.0f));
		WeaponSlots[3].Weapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		WheelChunkNums.resize(16);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;
		WheelChunkNums[8] = 9;
		WheelChunkNums[9] = 10;
		WheelChunkNums[10] = 11;
		WheelChunkNums[11] = 12;
		WheelChunkNums[12] = 13;
		WheelChunkNums[13] = 14;
		WheelChunkNums[14] = 15;
		WheelChunkNums[15] = 16;


		TargetVertChunkNums.resize(2);
		TargetVertChunkNums[0] = 18;
		TargetVertChunkNums[1] = 0;
		TargetVertChunksMaxAngle = 40.0f;

		TrackChunkNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 13:
		WeaponSlots.resize(2);
		WeaponSlots[0](CreateWeapon(206), sVECTOR3D(1.4f, 5.0f, 0.4f));
		WeaponSlots[0].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[1](CreateWeapon(206), sVECTOR3D(-1.4f, 5.0f, 0.4f));
		WeaponSlots[1].Weapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		WheelChunkNums.resize(14);
		WheelChunkNums[0] = 2;
		WheelChunkNums[1] = 3;
		WheelChunkNums[2] = 4;
		WheelChunkNums[3] = 5;
		WheelChunkNums[4] = 6;
		WheelChunkNums[5] = 7;
		WheelChunkNums[6] = 8;
		WheelChunkNums[7] = 9;
		WheelChunkNums[8] = 10;
		WheelChunkNums[9] = 11;
		WheelChunkNums[10] = 12;
		WheelChunkNums[11] = 13;
		WheelChunkNums[12] = 14;
		WheelChunkNums[13] = 15;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 0;
		TargetHorizChunkNums[1] = 16;

		TargetVertChunkNums.emplace_back(16);
		TargetVertChunksMaxAngle = 50.0f;

		TrackChunkNum = 17;
		Texture[17] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 14:
		WheelChunkNums.resize(8);
		WheelChunkNums[0] = 0;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;
		WheelChunkNums[4] = 5;
		WheelChunkNums[5] = 6;
		WheelChunkNums[6] = 7;
		WheelChunkNums[7] = 8;

		TrackChunkNum = 10;
		Texture[10] = GetPreloadedTextureAsset("models/track.vw2d");
		TrackRotationDirection = -1;
		break;
	}

	// делаем сдвиг поворота колес, чтобы смотрелось естественнее
	for (unsigned i = 0; i < WheelChunkNums.size(); i++) {
		Chunks[WheelChunkNums[i]].Rotation.x = vw_fRandNum(360.0f);
	}

	// вычисляем данные для нахождения точки стрельбы
	if (!TargetHorizChunkNums.empty()) {
		BaseBound = Chunks[TargetHorizChunkNums[0]].Location;
	}

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
