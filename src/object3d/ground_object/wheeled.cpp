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

struct sWheeledData {
	float Strength;
	float SpeedToWheelRotarySpeedFactor;
	std::string Model3DFileName;
	std::string TextureFileName;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::vector<sWheeledData> PresetWheeledData{
	{120,	50.0f,	"models/wheeled/jeep-01.vw3d", "models/gr-02.vw2d"},
	{70,	50.0f,	"models/wheeled/jeep-02.vw3d", "models/gr-02.vw2d"},
	{100,	50.0f,	"models/wheeled/jeep-03.vw3d", "models/gr-02.vw2d"},
	{100,	50.0f,	"models/wheeled/jeep-04.vw3d", "models/gr-07.vw2d"},
	{150,	50.0f,	"models/wheeled/jeep-05.vw3d", "models/gr-03.vw2d"},
	{250,	30.0f,	"models/wheeled/apc-02.vw3d", "models/gr-03.vw2d"},
	{200,	30.0f,	"models/wheeled/apc-04.vw3d", "models/gr-01.vw2d"},
	{400,	34.0f,	"models/wheeled/r-launcher-01.vw3d", "models/gr-02.vw2d"}
};
#pragma GCC diagnostic pop


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cWheeled::cWheeled(int WheeledNum)
{
	if ((WheeledNum <= 0) || (static_cast<unsigned>(WheeledNum) > PresetWheeledData.size())) {
		std::cerr << __func__ << "(): " << "Could not init cWheeled object with Number " << WheeledNum << "\n";
		return;
	}

	ObjectType = eObjectType::PirateVehicle;
	InternalType = WheeledNum;
	MaxSpeed = 20.0f;
	MaxAcceler = 5.0f;
	MaxSpeedRotate = 20.0f;

	LoadObjectData(PresetWheeledData[WheeledNum - 1].Model3DFileName, *this);

	for (unsigned int i = 0; i < Chunks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetWheeledData[WheeledNum - 1].TextureFileName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToWheelRotarySpeedFactor = PresetWheeledData[WheeledNum - 1].SpeedToWheelRotarySpeedFactor;

	Strength = StrengthStart = PresetWheeledData[WheeledNum - 1].Strength/GameEnemyArmorPenalty;

	// установка доп. текстуры и других настроек для каждой модели
	switch (WheeledNum) {
	case 1:
		Weapon.resize(4);
		Weapon[0](CreateWeapon(206), sVECTOR3D(0.3f, 4.5f, -1.5f));
		Weapon[1](CreateWeapon(206), sVECTOR3D(-0.3f, 4.5f, -1.5f));
		Weapon[2](CreateWeapon(206), sVECTOR3D(0.9f, 4.5f, -1.5f));
		Weapon[3](CreateWeapon(206), sVECTOR3D(-0.9f, 4.5f, -1.5f));
		WeaponFireType = 3;

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 0;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 3;
		SteerableWheelChunkNums[1] = 4;
		MaxSteerableWheelAngle = 30.0f;

		TargetVertChunkNums.emplace_back(5);
		TargetVertChunksMaxAngle = 30.0f;
		break;

	case 2:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(206), sVECTOR3D(1.3f, 3.5f, -1.5f));
		Weapon[1](CreateWeapon(206), sVECTOR3D(-1.3f, 3.5f, -1.5f));
		WeaponFireType = 3;
		DoNotCalculateRotation = true;

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 1;
		SteerableWheelChunkNums[1] = 4;
		MaxSteerableWheelAngle = 20.0f;
		break;

	case 3:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(0.8f, 4.2f, -1.0f));
		Weapon[1](CreateWeapon(204), sVECTOR3D(-0.8f, 4.2f, -1.0f));
		WeaponFireType = 2;

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 0;
		WheelChunkNums[1] = 1;
		WheelChunkNums[2] = 2;
		WheelChunkNums[3] = 5;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 0;
		SteerableWheelChunkNums[1] = 1;
		MaxSteerableWheelAngle = 20.0f;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 3;
		TargetHorizChunkNums[1] = 6;

		TargetVertChunkNums.emplace_back(6);
		TargetVertChunksMaxAngle = 60.0f;
		break;

	case 4:
		Weapon.emplace_back(CreateWeapon(207), sVECTOR3D(0.0f, 4.5f, -4.0f));

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 0;
		WheelChunkNums[1] = 1;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 0;
		SteerableWheelChunkNums[1] = 3;
		MaxSteerableWheelAngle = 20.0f;
		break;

	case 5:
		Weapon.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 4.5f, -4.0f));

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 1;
		SteerableWheelChunkNums[1] = 2;
		MaxSteerableWheelAngle = 30.0f;
		break;

	case 6:
		Weapon.emplace_back(CreateWeapon(204), sVECTOR3D(0.0f, 6.0f, -3.0f));

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 1;
		SteerableWheelChunkNums[1] = 4;
		MaxSteerableWheelAngle = 30.0f;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 5;
		TargetHorizChunkNums[1] = 6;

		TargetVertChunkNums.emplace_back(6);
		TargetVertChunksMaxAngle = 40.0f;
		break;

	case 7:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(0.1f, 5.0f, -1.0f));
		Weapon[1](CreateWeapon(204), sVECTOR3D(-0.1f, 5.0f, -1.0f));
		WeaponFireType = 2;

		WheelChunkNums.resize(4);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 2;
		WheelChunkNums[2] = 3;
		WheelChunkNums[3] = 4;

		SteerableWheelChunkNums.resize(2);
		SteerableWheelChunkNums[0] = 1;
		SteerableWheelChunkNums[1] = 4;
		MaxSteerableWheelAngle = 20.0f;

		TargetHorizChunkNums.resize(2);
		TargetHorizChunkNums[0] = 5;
		TargetHorizChunkNums[1] = 6;

		TargetVertChunkNums.emplace_back(6);
		TargetVertChunksMaxAngle = 40.0f;
		break;

	case 8:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(210), sVECTOR3D(1.5f, 5.2f, 7.0f));
		Weapon[1](CreateWeapon(210), sVECTOR3D(-1.5f, 5.2f, 7.0f));
		WeaponFireType = 3;

		WheelChunkNums.resize(8);
		WheelChunkNums[0] = 1;
		WheelChunkNums[1] = 3;
		WheelChunkNums[2] = 4;
		WheelChunkNums[3] = 5;
		WheelChunkNums[4] = 6;
		WheelChunkNums[5] = 7;
		WheelChunkNums[6] = 8;
		WheelChunkNums[7] = 9;

		SteerableWheelChunkNums.resize(4);
		SteerableWheelChunkNums[0] = 1;
		SteerableWheelChunkNums[1] = 3;
		SteerableWheelChunkNums[2] = 6;
		SteerableWheelChunkNums[3] = 7;
		MaxSteerableWheelAngle = 20.0f;

		TargetVertChunkNums.emplace_back(2);
		TargetVertChunksMaxAngle = 90.0f;
		break;
	}

	// делаем сдвиг поворота колес, чтобы смотрелось естественнее
	for (unsigned i = 0; i < WheelChunkNums.size(); i++) {
		Chunks[WheelChunkNums[i]].Rotation.x = vw_fRandNum(360.0f);
	}

	// вычисляем данные для нахождения точки стрельбы
	if (!TargetHorizChunkNums.empty())
		BaseBound = Chunks[TargetHorizChunkNums[0]].Location;

	if (!TargetVertChunkNums.empty()) {
		if (!TargetHorizChunkNums.empty())
			MiddleBound = Chunks[TargetVertChunkNums[0]].Location - Chunks[TargetHorizChunkNums[0]].Location;
		else
			MiddleBound = Chunks[TargetVertChunkNums[0]].Location;
	}

	for (auto &tmpWeapon : Weapon) {
		if (!TargetVertChunkNums.empty())
			tmpWeapon.Bound = tmpWeapon.Location - Chunks[TargetVertChunkNums[0]].Location;
		else if (!TargetHorizChunkNums.empty())
			tmpWeapon.Bound = tmpWeapon.Location - Chunks[TargetHorizChunkNums[0]].Location;
		else
			tmpWeapon.Bound = tmpWeapon.Location;
	}
}

} // astromenace namespace
} // viewizard namespace
