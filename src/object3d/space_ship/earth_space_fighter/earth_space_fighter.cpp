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

#include "earth_space_fighter.h"



//-----------------------------------------------------------------------------
// Получаем возможный поворот орудия в данном слоте
//-----------------------------------------------------------------------------
void GetShipWeaponSlotAngle(int ShipNum, int SlotNum, float *Min, float *Max)
{
	*Min = 0.0f;
	*Max = 0.0f;

	switch (ShipNum) {
	case 1:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 5.0f;
			break;
		case 1:
			*Min = -5.0f;
			*Max = 45.0f;
			break;
		case 2:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 3:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		case 4:
			*Min = -30.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 2:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 5.0f;
			break;
		case 1:
			*Min = -5.0f;
			*Max = 45.0f;
			break;
		case 2:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 3:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 3:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 45.0f;
			break;
		case 1:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		case 2:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		}
		break;
	case 4:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		case 2:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 3:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		case 4:
			*Min = -30.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 5:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 5.0f;
			break;
		case 1:
			*Min = -5.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 6:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 5.0f;
			break;
		case 1:
			*Min = -5.0f;
			*Max = 45.0f;
			break;
		case 2:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 3:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		case 4:
			*Min = -30.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 7:
		switch (SlotNum) {
		case 0:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -45.0f;
			*Max = 25.0f;
			break;
		case 2:
			*Min = -25.0f;
			*Max = 45.0f;
			break;
		case 3:
			*Min = -45.0f;
			*Max = 15.0f;
			break;
		case 4:
			*Min = -15.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 8:
		switch (SlotNum) {
		case 0:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		}
		break;
	case 9:
		switch (SlotNum) {
		case 0:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		}
		break;
	case 10:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		case 2:
			*Min = -20.0f;
			*Max = 5.0f;
			break;
		case 3:
			*Min = -5.0f;
			*Max = 20.0f;
			break;
		case 4:
			*Min = -30.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 11:
		switch (SlotNum) {
		case 0:
			*Min = -30.0f;
			*Max = 0.0f;
			break;
		case 1:
			*Min = -0.0f;
			*Max = 30.0f;
			break;
		case 2:
			*Min = -30.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 12:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 25.0f;
			break;
		case 1:
			*Min = -25.0f;
			*Max = 45.0f;
			break;
		case 2:
			*Min = -30.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 13:
		switch (SlotNum) {
		case 0:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -45.0f;
			*Max = 20.0f;
			break;
		case 2:
			*Min = -20.0f;
			*Max = 45.0f;
			break;
		case 3:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 4:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 14:
		switch (SlotNum) {
		case 0:
			*Min = -30.0f;
			*Max = 5.0f;
			break;
		case 1:
			*Min = -5.0f;
			*Max = 30.0f;
			break;
		case 2:
			*Min = -35.0f;
			*Max = 5.0f;
			break;
		case 3:
			*Min = -5.0f;
			*Max = 35.0f;
			break;
		case 4:
			*Min = -15.0f;
			*Max = 15.0f;
			break;
		}
		break;
	case 15:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 45.0f;
			break;
		case 1:
			*Min = -45.0f;
			*Max = 20.0f;
			break;
		case 2:
			*Min = -20.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 16:
		switch (SlotNum) {
		case 0:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		case 1:
			*Min = -30.0f;
			*Max = 0.0f;
			break;
		case 2:
			*Min = -0.0f;
			*Max = 30.0f;
			break;
		}
		break;
	case 17:
		switch (SlotNum) {
		case 0:
			*Min = -30.0f;
			*Max = 5.0f;
			break;
		case 1:
			*Min = -5.0f;
			*Max = 30.0f;
			break;
		case 2:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 3:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		case 4:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		}
		break;
	case 18:
		switch (SlotNum) {
		case 0:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		case 1:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		case 2:
			*Min = -25.0f;
			*Max = 5.0f;
			break;
		case 3:
			*Min = -5.0f;
			*Max = 25.0f;
			break;
		}
		break;
	case 19:
		switch (SlotNum) {
		case 0:
			*Min = -45.0f;
			*Max = 45.0f;
			break;
		case 1:
			*Min = -45.0f;
			*Max = 10.0f;
			break;
		case 2:
			*Min = -10.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 20:
		switch (SlotNum) {
		case 0:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		case 1:
			*Min = -10.0f;
			*Max = 10.0f;
			break;
		case 2:
			*Min = -45.0f;
			*Max = 20.0f;
			break;
		case 3:
			*Min = -20.0f;
			*Max = 45.0f;
			break;
		}
		break;
	case 21:
		switch (SlotNum) {
		case 0:
			*Min = -25.0f;
			*Max = 0.0f;
			break;
		case 1:
			*Min = -0.0f;
			*Max = 25.0f;
			break;
		case 2:
			*Min = -20.0f;
			*Max = 20.0f;
			break;
		}
		break;
	case 22:
		switch (SlotNum) {
		case 0:
			*Min = -25.0f;
			*Max = 25.0f;
			break;
		}
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong ShipNum.\n";
		break;
	}
}











struct sEarthSpaceFighterData {
	float Weight;
	float Strength;
	unsigned int WeaponQuantity;
	unsigned int EngineQuantity;
	const char *Name;
};

const sEarthSpaceFighterData PresetEarthSpaceFighterData[] = {
	{5000,	30,	5,	2,	"models/earthfighter/sf-01.vw3d"},
	{8000,	40,	4,	4,	"models/earthfighter/sf-02.vw3d"},
	{10000,	60,	3,	4,	"models/earthfighter/sf-03.vw3d"},
	{6000,	35,	5,	2,	"models/earthfighter/sf-04.vw3d"},
	{7000,	45,	2,	4,	"models/earthfighter/sf-05.vw3d"},
	{6000,	50,	5,	2,	"models/earthfighter/sf-06.vw3d"},
	{10000,	60,	5,	3,	"models/earthfighter/sf-07.vw3d"},
	{3000,	25,	2,	4,	"models/earthfighter/sf-08.vw3d"},
	{3000,	55,	2,	2,	"models/earthfighter/sf-09.vw3d"},
	{6500,	55,	5,	2,	"models/earthfighter/sf-10.vw3d"},
	{3000,	60,	3,	2,	"models/earthfighter/sf-11.vw3d"},
	{6000,	70,	3,	2,	"models/earthfighter/sf-12.vw3d"},
	{15000,	80,	5,	4,	"models/earthfighter/sf-13.vw3d"},
	{7000,	65,	5,	2,	"models/earthfighter/sf-14.vw3d"},
	{6000,	70,	3,	2,	"models/earthfighter/sf-15.vw3d"},
	{13000,	90,	3,	4,	"models/earthfighter/sf-16.vw3d"},
	{6500,	75,	5,	2,	"models/earthfighter/sf-17.vw3d"},
	{17000,	110,	4,	4,	"models/earthfighter/sf-18.vw3d"},
	{8000,	55,	3,	4,	"models/earthfighter/sf-19.vw3d"},
	{7000,	80,	4,	4,	"models/earthfighter/sf-20.vw3d"},
	{4000,	80,	3,	2,	"models/earthfighter/sf-21.vw3d"},
	{1500,	100,	1,	1,	"models/earthfighter/sf-22.vw3d"}
};
#define PresetEarthSpaceFighterDataCount sizeof(PresetEarthSpaceFighterData)/sizeof(PresetEarthSpaceFighterData[0])

float GetShipArmor(int SpaceShipNum)
{
	return PresetEarthSpaceFighterData[SpaceShipNum-1].Strength;
}





//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cEarthSpaceFighter::cEarthSpaceFighter(int SpaceShipNum)
{
	if ((SpaceShipNum <= 0) || ((unsigned int)SpaceShipNum > PresetEarthSpaceFighterDataCount)) {
		std::cerr << __func__ << "(): "
			  << "Could not init cEarthSpaceFighter object with Number " << SpaceShipNum << "\n";
		return;
	}

	ObjectStatus = eObjectStatus::Ally;
	ObjectType = eObjectType::EarthFighter;
	InternalType = SpaceShipNum;
	PromptDrawDist2 = 100.0f;

	// ставим нужные данные
	Weight = PresetEarthSpaceFighterData[SpaceShipNum-1].Weight;
	Strength = StrengthStart = PresetEarthSpaceFighterData[SpaceShipNum-1].Strength; // GameEnemyArmorPenalty for enemies only
	WeaponQuantity = PresetEarthSpaceFighterData[SpaceShipNum-1].WeaponQuantity;
	unsigned int EngineQuantity = PresetEarthSpaceFighterData[SpaceShipNum - 1].EngineQuantity;
	LoadObjectData(PresetEarthSpaceFighterData[SpaceShipNum-1].Name, this);


	// начальные установки для оружия
	WeaponSetFire = new bool[WeaponQuantity];
	WeaponLocation = new sVECTOR3D[WeaponQuantity];
	WeaponType = new int[WeaponQuantity];
	Weapon = new cWeapon*[WeaponQuantity];
	WeaponYAngle = new float[WeaponQuantity];
	for (int i=0; i<WeaponQuantity; i++) {
		WeaponSetFire[i] = false;
		WeaponType[i] = 1;
		Weapon[i] = nullptr;
		WeaponYAngle[i] = 0.0f;
	}


	// начальные установки для двигателей

	EnginesLocation.resize(EngineQuantity);
	Engines.resize(EngineQuantity);

	EnginesLeftLocation.resize(1);
	EnginesLeft.resize(1);

	EnginesRightLocation.resize(1);
	EnginesRight.resize(1);




	// перебираем и ставим нужные данные
	switch (SpaceShipNum) {
	case 1:
		WeaponType[0] = 1;
		WeaponLocation[0] = sVECTOR3D(4.4f, -0.1f, -6.0f);
		WeaponType[1] = 1;
		WeaponLocation[1] = sVECTOR3D(-4.4f, -0.1f, -6.0f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(1.9f, -1.25f, 6.5f);
		WeaponType[3] = 1;
		WeaponLocation[3] = sVECTOR3D(-1.9f, -1.25f, 6.5f);
		WeaponType[4] = 4;
		WeaponLocation[4] = sVECTOR3D(0.0f, -1.20f, 1.0f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(3.0f, -1.2f, -4.7f);
		EnginesLocation[1] = sVECTOR3D(-3.0f, -1.2f, -4.7f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, -0.7f, 3.7f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, -0.7f, 3.7f);
		break;
	case 2:
		WeaponType[0] = 1;
		WeaponLocation[0] = sVECTOR3D(3.45f, 1.03f, -8.5f);
		WeaponType[1] = 1;
		WeaponLocation[1] = sVECTOR3D(-3.45f, 1.03f, -8.5f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(1.7f, -1.03f, 3.9f);
		WeaponType[3] = 1;
		WeaponLocation[3] = sVECTOR3D(-1.7f, -1.03f, 3.9f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(4.9f, -1.1f, -8.9f);
		EnginesLocation[1] = sVECTOR3D(-4.9f, -1.1f, -8.9f);
		EnginesLocation[2] = sVECTOR3D(0.75f, 0.7f, -9.6f);
		EnginesLocation[3] = sVECTOR3D(-0.75f, 0.7f, -9.6f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, -0.7f, 5.7f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, -0.7f, 5.7f);
		break;
	case 3:
		WeaponType[0] = 5;
		WeaponLocation[0] = sVECTOR3D(0.0f, -1.75f, 3.5f);
		WeaponType[1] = 1;
		WeaponLocation[1] = sVECTOR3D(7.15f, -0.38f, -4.4f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(-7.15f, -0.38f, -4.4f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(4.4f, -3.0f, -9.7f);
		EnginesLocation[1] = sVECTOR3D(-4.4f, -3.0f, -9.7f);
		EnginesLocation[2] = sVECTOR3D(4.4f, 3.0f, -9.7f);
		EnginesLocation[3] = sVECTOR3D(-4.4f, 3.0f, -9.7f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(0.8f, -1.6f, 8.7f);
		EnginesRightLocation[0] = sVECTOR3D(-0.8f, -1.6f, 8.7f);
		break;
	case 4:
		WeaponType[0] = 1;
		WeaponLocation[0] = sVECTOR3D(4.10f, 1.4f, -2.5f);
		WeaponType[1] = 1;
		WeaponLocation[1] = sVECTOR3D(-4.10f, 1.4f, -2.5f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(5.10f, -1.2f, -2.5f);
		WeaponType[3] = 1;
		WeaponLocation[3] = sVECTOR3D(-5.10f, -1.2f, -2.5f);
		WeaponType[4] = 4;
		WeaponLocation[4] = sVECTOR3D(0.0f, -1.5f, 5.3f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.8f, 0.1f, -9.6f);
		EnginesLocation[1] = sVECTOR3D(-0.8f, 0.1f, -9.6f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.6f, -0.9f, 5.7f);
		EnginesRightLocation[0] = sVECTOR3D(-1.6f, -0.9f, 5.7f);
		break;
	case 5:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(4.0f, -0.5f, 1.0f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-4.0f, -0.5f, 1.0f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(6.72f, -0.28f, -7.98f);
		EnginesLocation[1] = sVECTOR3D(-6.72f, -0.28f, -7.98f);
		EnginesLocation[2] = sVECTOR3D(6.72f, -1.96f, -7.28f);
		EnginesLocation[3] = sVECTOR3D(-6.72f, -1.96f, -7.28f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.4f, 0.0f, 3.92f);
		EnginesRightLocation[0] = sVECTOR3D(-1.4f, 0.0f, 3.92f);
		break;
	case 6:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(2.80f, 0.25f, -7.0f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-2.80f, 0.25f, -7.0f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(5.20f, 0.6f, -6.0f);
		WeaponType[3] = 1;
		WeaponLocation[3] = sVECTOR3D(-5.20f, 0.6f, -6.0f);
		WeaponType[4] = 4;
		WeaponLocation[4] = sVECTOR3D(0.0f, -1.3f, 2.6f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.6f, 0.45f, -9.9f);
		EnginesLocation[1] = sVECTOR3D(-0.6f, 0.45f, -9.9f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.6f, -0.9f, 4.7f);
		EnginesRightLocation[0] = sVECTOR3D(-1.6f, -0.9f, 4.7f);
		break;
	case 7:
		WeaponType[0] = 5;
		WeaponLocation[0] = sVECTOR3D(0.0f, -0.2f, 0.0f);
		WeaponType[1] = 1;
		WeaponLocation[1] = sVECTOR3D(4.3f, -1.25f, -1.4f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(-4.3f, -1.25f, -1.4f);
		WeaponType[3] = 1;
		WeaponLocation[3] = sVECTOR3D(8.2f, -0.75f, -3.6f);
		WeaponType[4] = 1;
		WeaponLocation[4] = sVECTOR3D(-8.2f, -0.75f, -3.6f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(3.75f, -0.5f, -10.9f);
		EnginesLocation[1] = sVECTOR3D(0.0f, 0.4f, -8.3f);
		EnginesLocation[2] = sVECTOR3D(-3.75f, -0.5f, -10.9f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.4f, 0.1f, 1.1f);
		EnginesRightLocation[0] = sVECTOR3D(-1.4f, 0.1f, 1.1f);
		break;
	case 8:
		WeaponType[0] = 1;
		WeaponLocation[0] = sVECTOR3D(1.0f, -0.3f, 0.4f);
		WeaponType[1] = 1;
		WeaponLocation[1] = sVECTOR3D(-1.0f, -0.3f, 0.4f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(3.5f, 1.4f, -7.4f);
		EnginesLocation[1] = sVECTOR3D(-3.5f, 1.4f, -7.4f);
		EnginesLocation[2] = sVECTOR3D(3.65f, 0.5f, -7.9f);
		EnginesLocation[3] = sVECTOR3D(-3.65f, 0.5f, -7.9f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, 0.0f, 2.4f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, 0.0f, 2.4f);
		break;
	case 9:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(1.2f, 0.2f, 1.6f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-1.2f, 0.2f, 1.6f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(3.30f, -1.20f, -0.7f);
		EnginesLocation[1] = sVECTOR3D(-3.30f, -1.20f, -0.7f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, 0.2f, 2.2f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, 0.2f, 2.2f);
		break;
	case 10:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(5.05f, -1.8f, -1.7f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-5.05f, -1.8f, -1.7f);
		WeaponType[2] = 2;
		WeaponLocation[2] = sVECTOR3D(2.55f, -0.7f, -0.7f);
		WeaponType[3] = 2;
		WeaponLocation[3] = sVECTOR3D(-2.55f, -0.7f, -0.7f);
		WeaponType[4] = 4;
		WeaponLocation[4] = sVECTOR3D(0.0f, -0.9f, 6.1f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.8f, 0.55f, -9.9f);
		EnginesLocation[1] = sVECTOR3D(-0.8f, 0.55f, -9.9f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.6f, -0.4f, 5.3f);
		EnginesRightLocation[0] = sVECTOR3D(-1.6f, -0.4f, 5.3f);
		break;
	case 11:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(2.0f, -0.55f, -2.2f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-2.0f, -0.55f, -2.2f);
		WeaponType[2] = 1;
		WeaponLocation[2] = sVECTOR3D(0.0f, -0.7f, 2.2f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(4.35f, -0.35f, -4.4f);
		EnginesLocation[1] = sVECTOR3D(-4.35f, -0.35f, -4.4f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.5f, -0.6f, 2.0f);
		EnginesRightLocation[0] = sVECTOR3D(-1.5f, -0.6f, 2.0f);
		break;
	case 12:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(2.25f, -0.85f, 3.3f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-2.25f, -0.85f, 3.3f);
		WeaponType[2] = 4;
		WeaponLocation[2] = sVECTOR3D(0.0f, -0.35f, 2.8f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(4.8f, -1.2f, -2.8f);
		EnginesLocation[1] = sVECTOR3D(-4.8f, -1.2f, -2.8f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.2f, -0.2f, 6.3f);
		EnginesRightLocation[0] = sVECTOR3D(-1.2f, -0.2f, 6.3f);
		break;
	case 13:
		WeaponType[0] = 5;
		WeaponLocation[0] = sVECTOR3D(0.0f, -2.15f, -4.4f);
		WeaponType[1] = 4;
		WeaponLocation[1] = sVECTOR3D(7.2f, 1.85f, -5.6f);
		WeaponType[2] = 4;
		WeaponLocation[2] = sVECTOR3D(-7.2f, 1.85f, -5.6f);
		WeaponType[3] = 1;
		WeaponLocation[3] = sVECTOR3D(3.05f, -1.95f, -0.4f);
		WeaponType[4] = 1;
		WeaponLocation[4] = sVECTOR3D(-3.05f, -1.95f, -0.4f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(2.2f, -1.35f, -9.8f);
		EnginesLocation[1] = sVECTOR3D(-2.2f, -1.35f, -9.8f);
		EnginesLocation[2] = sVECTOR3D(1.1f, -1.0f, -9.8f);
		EnginesLocation[3] = sVECTOR3D(-1.1f, -1.0f, -9.8f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.4f, -0.1f, 0.5f);
		EnginesRightLocation[0] = sVECTOR3D(-1.4f, -0.1f, 0.5f);
		break;
	case 14:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(1.70f, -1.1f, 5.5f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-1.70f, -1.1f, 5.5f);
		WeaponType[2] = 2;
		WeaponLocation[2] = sVECTOR3D(4.1f, 0.2f, -6.3f);
		WeaponType[3] = 2;
		WeaponLocation[3] = sVECTOR3D(-4.1f, 0.2f, -6.3f);
		WeaponType[4] = 4;
		WeaponLocation[4] = sVECTOR3D(0.0f, -0.9f, 3.3f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.85f, 0.1f, -8.1f);
		EnginesLocation[1] = sVECTOR3D(-0.85f, 0.1f, -8.1f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.3f, -0.7f, 6.3f);
		EnginesRightLocation[0] = sVECTOR3D(-1.3f, -0.7f, 6.3f);
		break;
	case 15:
		WeaponType[0] = 4;
		WeaponLocation[0] = sVECTOR3D(0.0f, -0.7f, 1.3f);
		WeaponType[1] = 3;
		WeaponLocation[1] = sVECTOR3D(7.1f, -0.2f, -5.8f);
		WeaponType[2] = 3;
		WeaponLocation[2] = sVECTOR3D(-7.1f, -0.2f, -5.8f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(4.75f, -0.5f, -7.2f);
		EnginesLocation[1] = sVECTOR3D(-4.75f, -0.5f, -7.2f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, -0.6f, 6.25f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, -0.6f, 6.25f);
		break;
	case 16:
		WeaponType[0] = 3;
		WeaponLocation[0] = sVECTOR3D(0.0f, -1.2f, 7.7f);
		WeaponType[1] = 4;
		WeaponLocation[1] = sVECTOR3D(2.0f, -1.05f, 0.4f);
		WeaponType[2] = 4;
		WeaponLocation[2] = sVECTOR3D(-2.0f, -1.05f, 0.4f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(3.8f, -0.75f, -11.4f);
		EnginesLocation[1] = sVECTOR3D(-3.8f, -0.75f, -11.4f);
		EnginesLocation[2] = sVECTOR3D(3.5f, 0.85f, -10.85f);
		EnginesLocation[3] = sVECTOR3D(-3.5f, 0.85f, -10.85f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.6f, -0.6f, 6.25f);
		EnginesRightLocation[0] = sVECTOR3D(-1.6f, -0.6f, 6.25f);
		break;
	case 17:
		WeaponType[0] = 3;
		WeaponLocation[0] = sVECTOR3D(3.10f, 0.2f, -4.6f);
		WeaponType[1] = 3;
		WeaponLocation[1] = sVECTOR3D(-3.10f, 0.2f, -4.6f);
		WeaponType[2] = 2;
		WeaponLocation[2] = sVECTOR3D(5.85f, -0.5f, -5.6f);
		WeaponType[3] = 2;
		WeaponLocation[3] = sVECTOR3D(-5.85f, -0.5f, -5.6f);
		WeaponType[4] = 4;
		WeaponLocation[4] = sVECTOR3D(0.0f, -0.5f, -0.5f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.65f, 0.2f, -8.2f);
		EnginesLocation[1] = sVECTOR3D(-0.65f, 0.2f, -8.2f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.4f, -0.5f, 6.25f);
		EnginesRightLocation[0] = sVECTOR3D(-1.4f, -0.5f, 6.25f);
		break;
	case 18:
		WeaponType[0] = 5;
		WeaponLocation[0] = sVECTOR3D(1.7f, -2.5f, -4.4f);
		WeaponType[1] = 5;
		WeaponLocation[1] = sVECTOR3D(-1.7f, -2.5f, -4.4f);
		WeaponType[2] = 3;
		WeaponLocation[2] = sVECTOR3D(5.1f, -1.0f, -5.4f);
		WeaponType[3] = 3;
		WeaponLocation[3] = sVECTOR3D(-5.1f, -1.0f, -5.4f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(7.5f, 0.15f, -9.1f);
		EnginesLocation[1] = sVECTOR3D(-7.5f, 0.15f, -9.1f);
		EnginesLocation[2] = sVECTOR3D(7.7f, -1.15f, -9.6f);
		EnginesLocation[3] = sVECTOR3D(-7.7f, -1.15f, -9.6f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.9f, -1.6f, 6.25f);
		EnginesRightLocation[0] = sVECTOR3D(-1.9f, -1.6f, 6.25f);
		break;
	case 19:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(0.0f, -0.75f, 0.0f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(4.7f, -0.4f, -5.0f);
		WeaponType[2] = 2;
		WeaponLocation[2] = sVECTOR3D(-4.7f, -0.4f, -5.0f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(6.3f, 0.60f, -8.1f);
		EnginesLocation[1] = sVECTOR3D(-6.3f, 0.60f, -8.1f);
		EnginesLocation[2] = sVECTOR3D(6.7f, -0.60f, -8.8f);
		EnginesLocation[3] = sVECTOR3D(-6.7f, -0.60f, -8.8f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.45f, 0.05f, 5.0f);
		EnginesRightLocation[0] = sVECTOR3D(-1.45f, 0.05f, 5.0f);
		break;
	case 20:
		WeaponType[0] = 3;
		WeaponLocation[0] = sVECTOR3D(0.80f, -0.8f, -2.3f);
		WeaponType[1] = 3;
		WeaponLocation[1] = sVECTOR3D(-0.80f, -0.8f, -2.3f);
		WeaponType[2] = 4;
		WeaponLocation[2] = sVECTOR3D(3.9f, -1.25f, -3.2f);
		WeaponType[3] = 4;
		WeaponLocation[3] = sVECTOR3D(-3.9f, -1.25f, -3.2f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(3.45f, -0.3f, -7.3f);
		EnginesLocation[1] = sVECTOR3D(-3.45f, -0.3f, -7.3f);
		EnginesLocation[2] = sVECTOR3D(4.55f, -0.5f, -7.0f);
		EnginesLocation[3] = sVECTOR3D(-4.55f, -0.5f, -7.0f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, -0.4f, 1.45f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, -0.4f, 1.45f);
		break;
	case 21:
		WeaponType[0] = 2;
		WeaponLocation[0] = sVECTOR3D(2.05f, -0.22f, -1.2f);
		WeaponType[1] = 2;
		WeaponLocation[1] = sVECTOR3D(-2.05f, -0.22f, -1.2f);
		WeaponType[2] = 2;
		WeaponLocation[2] = sVECTOR3D(0.0f, -0.5f, 0.5f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.7f, 0.0f, -4.4f);
		EnginesLocation[1] = sVECTOR3D(-0.7f, 0.0f, -4.4f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.0f, -0.4f, 3.0f);
		EnginesRightLocation[0] = sVECTOR3D(-1.0f, -0.4f, 3.0f);
		break;
	case 22:
		WeaponType[0] = 3;
		WeaponLocation[0] = sVECTOR3D(0.0f, 0.05f, 3.6f);
		// двигатели, маршевые
		EnginesLocation[0] = sVECTOR3D(0.0f, 0.3f, -5.2f);
		// маневровые
		EnginesLeftLocation[0] = sVECTOR3D(1.9f, -0.3f, 2.6f);
		EnginesRightLocation[0] = sVECTOR3D(-1.9f, -0.3f, 2.6f);
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong SpaceShipNum.\n";
		return;
	}



	SetEarthSpaceFighterEngine(this, 1);
	/*	MaxSpeed = 4.5f;
		MaxAcceler = 4.5f;
		MaxSpeedRotate = 4.5f;*/

	for (unsigned int i = 0; i < EngineQuantity; i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			// находим кол-во внутренних источников света
			if (!sharedEngine->Light.expired())
				InternalLights++;
		}
	}

	// делаем предварительную уснановку брони-текстур
	SetEarthSpaceFighterArmour(this, 0);
}
