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
	float SpeedToRotate;
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

	LoadObjectData(PresetWheeledData[WheeledNum - 1].Model3DFileName, this);

	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetWheeledData[WheeledNum - 1].TextureFileName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToRotate = PresetWheeledData[WheeledNum - 1].SpeedToRotate;

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

		WheelObjects.resize(4);
		WheelObjects[0] = 0;
		WheelObjects[1] = 2;
		WheelObjects[2] = 3;
		WheelObjects[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 3;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 30.0f;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 5;
		TargetVertBlocksMaxAngle = 30.0f;
		break;

	case 2:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(206), sVECTOR3D(1.3f, 3.5f, -1.5f));
		Weapon[1](CreateWeapon(206), sVECTOR3D(-1.3f, 3.5f, -1.5f));
		WeaponFireType = 3;
		DoNotCalculateRotation = true;

		WheelObjects.resize(4);
		WheelObjects[0] = 1;
		WheelObjects[1] = 2;
		WheelObjects[2] = 3;
		WheelObjects[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 20.0f;
		break;

	case 3:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(0.8f, 4.2f, -1.0f));
		Weapon[1](CreateWeapon(204), sVECTOR3D(-0.8f, 4.2f, -1.0f));
		WeaponFireType = 2;

		WheelObjects.resize(4);
		WheelObjects[0] = 0;
		WheelObjects[1] = 1;
		WheelObjects[2] = 2;
		WheelObjects[3] = 5;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 0;
		WheelRotateObjectsNum[1] = 1;
		MaxWheelRotateAngle = 20.0f;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 3;
		TargetHorizBlocks[1] = 6;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 6;
		TargetVertBlocksMaxAngle = 60.0f;
		break;

	case 4:
		Weapon.emplace_back(CreateWeapon(207), sVECTOR3D(0.0f, 4.5f, -4.0f));

		WheelObjects.resize(4);
		WheelObjects[0] = 0;
		WheelObjects[1] = 1;
		WheelObjects[2] = 3;
		WheelObjects[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 0;
		WheelRotateObjectsNum[1] = 3;
		MaxWheelRotateAngle = 20.0f;
		break;

	case 5:
		Weapon.emplace_back(CreateWeapon(208), sVECTOR3D(0.0f, 4.5f, -4.0f));

		WheelObjects.resize(4);
		WheelObjects[0] = 1;
		WheelObjects[1] = 2;
		WheelObjects[2] = 3;
		WheelObjects[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 2;
		MaxWheelRotateAngle = 30.0f;
		break;

	case 6:
		Weapon.emplace_back(CreateWeapon(204), sVECTOR3D(0.0f, 6.0f, -3.0f));

		WheelObjects.resize(4);
		WheelObjects[0] = 1;
		WheelObjects[1] = 2;
		WheelObjects[2] = 3;
		WheelObjects[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 30.0f;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 5;
		TargetHorizBlocks[1] = 6;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 6;
		TargetVertBlocksMaxAngle = 40.0f;
		break;

	case 7:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(204), sVECTOR3D(0.1f, 5.0f, -1.0f));
		Weapon[1](CreateWeapon(204), sVECTOR3D(-0.1f, 5.0f, -1.0f));
		WeaponFireType = 2;

		WheelObjects.resize(4);
		WheelObjects[0] = 1;
		WheelObjects[1] = 2;
		WheelObjects[2] = 3;
		WheelObjects[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 20.0f;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 5;
		TargetHorizBlocks[1] = 6;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 6;
		TargetVertBlocksMaxAngle = 40.0f;
		break;

	case 8:
		Weapon.resize(2);
		Weapon[0](CreateWeapon(210), sVECTOR3D(1.5f, 5.2f, 7.0f));
		Weapon[1](CreateWeapon(210), sVECTOR3D(-1.5f, 5.2f, 7.0f));
		WeaponFireType = 3;

		WheelObjects.resize(8);
		WheelObjects[0] = 1;
		WheelObjects[1] = 3;
		WheelObjects[2] = 4;
		WheelObjects[3] = 5;
		WheelObjects[4] = 6;
		WheelObjects[5] = 7;
		WheelObjects[6] = 8;
		WheelObjects[7] = 9;

		WheelRotateQuantity = 4;
		WheelRotateObjectsNum = new int[4];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 3;
		WheelRotateObjectsNum[2] = 6;
		WheelRotateObjectsNum[3] = 7;
		MaxWheelRotateAngle = 20.0f;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 2;
		TargetVertBlocksMaxAngle = 90.0f;
		break;
	}

	// установка остальных параметров девиации
	DeviationOn = false;
	DeviationObjQuantity = WheelObjects.size();
	Deviation = new sVECTOR3D[DeviationObjQuantity];
	NeedDeviation = new float[DeviationObjQuantity];
	CurentDeviation = new float[DeviationObjQuantity];
	CurentDeviationSum = new float[DeviationObjQuantity];
	DeviationObjNum = new int[DeviationObjQuantity];

	for (unsigned i = 0; i < DeviationObjQuantity; i++) {
		Deviation[i] = sVECTOR3D(0.0f, 1.0f, 0.0f);
		NeedDeviation[i] = vw_fRand0() * 0.1f;
		CurentDeviation[i] = CurentDeviationSum[i] = 0.0f;
		DeviationObjNum[i] = WheelObjects[i];
	}

	// делаем сдвиг поворота колес, чтобы смотрелось естественнее
	for (unsigned i = 0; i < WheelObjects.size(); i++) {
		Model3DBlocks[WheelObjects[i]].Rotation.x = vw_fRandNum(360.0f);
	}

	// вычисляем данные для нахождения точки стрельбы
	if (TargetHorizBlocks != nullptr)
		BaseBound = Model3DBlocks[TargetHorizBlocks[0]].Location;

	if (TargetVertBlocks != nullptr) {
		if (TargetHorizBlocks != nullptr) {
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location - Model3DBlocks[TargetHorizBlocks[0]].Location;
		} else {
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location;
		}
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
