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
	float SpeedToRotate;
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

	LoadObjectData(PresetTrackedData[TrackedNum - 1].Model3DFileName, this);

	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetTrackedData[TrackedNum - 1].TextureFileName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToRotate = PresetTrackedData[TrackedNum - 1].SpeedToRotate;

	Strength = StrengthStart = PresetTrackedData[TrackedNum - 1].Strength/GameEnemyArmorPenalty;

	// установка доп. текстуры и других настроек для каждой модели
	switch (TrackedNum) {
	case 1:
		Weapon.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 5.5f, 9.0f));

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 2;
		WheelObjectsNum[1] = 3;
		WheelObjectsNum[2] = 4;
		WheelObjectsNum[3] = 6;
		WheelObjectsNum[4] = 7;
		WheelObjectsNum[5] = 8;
		WheelObjectsNum[6] = 9;
		WheelObjectsNum[7] = 10;
		WheelObjectsNum[8] = 11;
		WheelObjectsNum[9] = 12;
		WheelObjectsNum[10] = 13;
		WheelObjectsNum[11] = 14;
		WheelObjectsNum[12] = 15;
		WheelObjectsNum[13] = 16;
		WheelObjectsNum[14] = 17;
		WheelObjectsNum[15] = 18;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 5;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 5;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 2:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(0.1f, 6.1f, -0.4f));
		Weapon[1](CreateWeapon(204), sVECTOR3D(-0.1f, 6.1f, -0.4f));
		WeaponFireType = 2;

		WheelQuantity = 10;
		WheelObjectsNum = new int[10];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 11;
		TargetHorizBlocks[1] = 12;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 12;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 13;
		Texture[13] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 3:
		Weapon.emplace_back(CreateWeapon(213), sVECTOR3D(0.0f, 5.2f, 3.7f));

		WheelQuantity = 14;
		WheelObjectsNum = new int[14];
		WheelObjectsNum[0] = 0;
		WheelObjectsNum[1] = 1;
		WheelObjectsNum[2] = 2;
		WheelObjectsNum[3] = 3;
		WheelObjectsNum[4] = 4;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 10;
		WheelObjectsNum[8] = 11;
		WheelObjectsNum[9] = 12;
		WheelObjectsNum[10] = 13;
		WheelObjectsNum[11] = 14;
		WheelObjectsNum[12] = 15;
		WheelObjectsNum[13] = 16;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 8;
		TargetHorizBlocks[1] = 9;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 9;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 17;
		Texture[17] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 4:
		Weapon.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 5.3f, 6.5f));

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 11;
		WheelObjectsNum[11] = 12;
		WheelObjectsNum[12] = 13;
		WheelObjectsNum[13] = 14;
		WheelObjectsNum[14] = 15;
		WheelObjectsNum[15] = 16;

		TargetHorizBlocksQuantity = 3;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 17;
		TargetHorizBlocks[1] = 18;
		TargetHorizBlocks[2] = 19;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 18;
		//TargetVertBlocks[1] = 19; // пулемет
		TargetVertBlocksMaxAngle = 50.0f;

		Model3DBlocks[19].Location.z -= 1.0f;

		TrackObjectNum = 20;
		Texture[20] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 5:
		Weapon.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 5.8f, 4.5f));

		WheelQuantity = 14;
		WheelObjectsNum = new int[14];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 10;
		WheelObjectsNum[8] = 11;
		WheelObjectsNum[9] = 12;
		WheelObjectsNum[10] = 13;
		WheelObjectsNum[11] = 14;
		WheelObjectsNum[12] = 15;
		WheelObjectsNum[13] = 16;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 9;
		TargetHorizBlocks[1] = 8;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 8;
		TargetVertBlocksMaxAngle = 30.0f;

		TrackObjectNum = 17;
		Texture[17] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 6:
		Weapon.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 4.9f, 4.0f));

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 2;
		WheelObjectsNum[1] = 3;
		WheelObjectsNum[2] = 4;
		WheelObjectsNum[3] = 5;
		WheelObjectsNum[4] = 6;
		WheelObjectsNum[5] = 7;
		WheelObjectsNum[6] = 8;
		WheelObjectsNum[7] = 9;
		WheelObjectsNum[8] = 10;
		WheelObjectsNum[9] = 11;
		WheelObjectsNum[10] = 12;
		WheelObjectsNum[11] = 13;
		WheelObjectsNum[12] = 14;
		WheelObjectsNum[13] = 15;
		WheelObjectsNum[14] = 16;
		WheelObjectsNum[15] = 17;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 1;
		TargetHorizBlocks[1] = 18;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 18;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 7:
		Weapon.emplace_back(CreateWeapon(212), sVECTOR3D(0.0f, 7.6f, 5.5f));

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 13;
		WheelObjectsNum[11] = 14;
		WheelObjectsNum[12] = 15;
		WheelObjectsNum[13] = 16;
		WheelObjectsNum[14] = 17;
		WheelObjectsNum[15] = 18;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 12;
		TargetHorizBlocks[1] = 11;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 11;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 8:
		Weapon.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 7.0f, 8.5f));

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 11;
		WheelObjectsNum[11] = 12;
		WheelObjectsNum[12] = 13;
		WheelObjectsNum[13] = 14;
		WheelObjectsNum[14] = 15;
		WheelObjectsNum[15] = 16;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 17;
		TargetHorizBlocks[1] = 18;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 18;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 9:
		Weapon.emplace_back(CreateWeapon(211), sVECTOR3D(0.0f, 6.7f, 6.8f));
		Weapon.back().Weapon->NextFireTime /= 2.0f;

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 11;
		WheelObjectsNum[11] = 12;
		WheelObjectsNum[12] = 13;
		WheelObjectsNum[13] = 14;
		WheelObjectsNum[14] = 15;
		WheelObjectsNum[15] = 16;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 18;
		TargetHorizBlocks[1] = 0;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 0;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 10:
		Weapon.emplace_back(CreateWeapon(204), sVECTOR3D(0.0f, 6.1f, 0.5f));
		Weapon.back().Weapon->NextFireTime /= 2.0f;

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 11;
		WheelObjectsNum[11] = 12;
		WheelObjectsNum[12] = 13;
		WheelObjectsNum[13] = 14;
		WheelObjectsNum[14] = 15;
		WheelObjectsNum[15] = 16;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 18;
		TargetHorizBlocks[1] = 0;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 0;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 11:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(209), sVECTOR3D(2.2f, 5.4f, 7.0f));
		Weapon[1](CreateWeapon(209), sVECTOR3D(-2.2f, 5.4f, 7.0f));
		WeaponFireType = 3;

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 11;
		WheelObjectsNum[11] = 12;
		WheelObjectsNum[12] = 13;
		WheelObjectsNum[13] = 14;
		WheelObjectsNum[14] = 15;
		WheelObjectsNum[15] = 16;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 0;
		TargetVertBlocksMaxAngle = 55.0f;

		TrackObjectNum = 18;
		Texture[18] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 12:
		Weapon.resize(4);
		Weapon[0](CreateWeapon(206), sVECTOR3D(0.55f, 5.0f, 2.0f));
		Weapon[0].Weapon->NextFireTime /= 2.0f;
		Weapon[1](CreateWeapon(206), sVECTOR3D(-0.55f, 5.0f, 2.0f));
		Weapon[1].Weapon->NextFireTime /= 2.0f;
		Weapon[2](CreateWeapon(206), sVECTOR3D(1.65f, 5.0f, 2.0f));
		Weapon[2].Weapon->NextFireTime /= 2.0f;
		Weapon[3](CreateWeapon(206), sVECTOR3D(-1.65f, 5.0f, 2.0f));
		Weapon[3].Weapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		WheelQuantity = 16;
		WheelObjectsNum = new int[16];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;
		WheelObjectsNum[8] = 9;
		WheelObjectsNum[9] = 10;
		WheelObjectsNum[10] = 11;
		WheelObjectsNum[11] = 12;
		WheelObjectsNum[12] = 13;
		WheelObjectsNum[13] = 14;
		WheelObjectsNum[14] = 15;
		WheelObjectsNum[15] = 16;


		TargetVertBlocksQuantity = 2;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 18;
		TargetVertBlocks[1] = 0;
		TargetVertBlocksMaxAngle = 40.0f;

		TrackObjectNum = 19;
		Texture[19] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 13:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(206), sVECTOR3D(1.4f, 5.0f, 0.4f));
		Weapon[0].Weapon->NextFireTime /= 2.0f;
		Weapon[1](CreateWeapon(206), sVECTOR3D(-1.4f, 5.0f, 0.4f));
		Weapon[1].Weapon->NextFireTime /= 2.0f;
		WeaponFireType = 3;

		WheelQuantity = 14;
		WheelObjectsNum = new int[14];
		WheelObjectsNum[0] = 2;
		WheelObjectsNum[1] = 3;
		WheelObjectsNum[2] = 4;
		WheelObjectsNum[3] = 5;
		WheelObjectsNum[4] = 6;
		WheelObjectsNum[5] = 7;
		WheelObjectsNum[6] = 8;
		WheelObjectsNum[7] = 9;
		WheelObjectsNum[8] = 10;
		WheelObjectsNum[9] = 11;
		WheelObjectsNum[10] = 12;
		WheelObjectsNum[11] = 13;
		WheelObjectsNum[12] = 14;
		WheelObjectsNum[13] = 15;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 0;
		TargetHorizBlocks[1] = 16;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 16;
		TargetVertBlocksMaxAngle = 50.0f;

		TrackObjectNum = 17;
		Texture[17] = GetPreloadedTextureAsset("models/track.vw2d");
		break;

	case 14:
		WheelQuantity = 8;
		WheelObjectsNum = new int[8];
		WheelObjectsNum[0] = 0;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;
		WheelObjectsNum[4] = 5;
		WheelObjectsNum[5] = 6;
		WheelObjectsNum[6] = 7;
		WheelObjectsNum[7] = 8;

		TrackObjectNum = 10;
		Texture[10] = GetPreloadedTextureAsset("models/track.vw2d");
		TrackRotationDirection = -1;
		break;
	}

	// установка остальных параметров девиации
	DeviationOn = false;
	DeviationObjQuantity = WheelQuantity;
	Deviation = new sVECTOR3D[DeviationObjQuantity];
	NeedDeviation = new float[DeviationObjQuantity];
	CurentDeviation = new float[DeviationObjQuantity];
	CurentDeviationSum = new float[DeviationObjQuantity];
	DeviationObjNum = new int[DeviationObjQuantity];

	for (int i = 0; i < DeviationObjQuantity; i++) {
		Deviation[i] = sVECTOR3D(0.0f, 1.0f, 0.0f);
		NeedDeviation[i] = vw_fRand0() * 0.1f;
		CurentDeviation[i] = CurentDeviationSum[i] = 0.0f;
		DeviationObjNum[i] = WheelObjectsNum[i];
	}

	// делаем сдвиг поворота колес, чтобы смотрелось естественнее
	for (int i = 0; i < WheelQuantity; i++)
		Model3DBlocks[WheelObjectsNum[i]].Rotation.x = vw_fRandNum(360.0f);

	// вычисляем данные для нахождения точки стрельбы
	if (TargetHorizBlocks != nullptr) {
		BaseBound = Model3DBlocks[TargetHorizBlocks[0]].Location;
	}

	if (TargetVertBlocks != nullptr) {
		if (TargetHorizBlocks != nullptr)
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location - Model3DBlocks[TargetHorizBlocks[0]].Location;
		else
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location;
	}

	for (auto &tmpWeapon : Weapon) {
		if (TargetVertBlocks != nullptr)
			tmpWeapon.Bound = tmpWeapon.Location - Model3DBlocks[TargetVertBlocks[0]].Location;
		else if (TargetHorizBlocks != nullptr)
			tmpWeapon.Bound = tmpWeapon.Location - Model3DBlocks[TargetHorizBlocks[0]].Location;
		else
			tmpWeapon.Bound = tmpWeapon.Location;
	}
}

} // astromenace namespace
} // viewizard namespace
