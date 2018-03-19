/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "weapon.h"



// название типа оружия землян
const char *GetWeaponGroupTitle(int Num)
{
	switch (Num) {
	case 1:
	case 2:
	case 3:
	case 4:
		return "4_Kinetic";
	case 5:
	case 6:
	case 7:
		return "4_Ion";
	case 8:
	case 9:
	case 10:
		return "4_Plasma";
	case 11:
	case 12:
		return "4_Maser";
	case 13:
		return "4_Antimatter";
	case 14:
		return "4_Laser";
	case 15:
		return "4_Gauss";

	case 16:
	case 17:
	case 18:
	case 19:
		return "4_Propelled";

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}
	return nullptr;
}








struct sWeaponData {
	int		SoundNum;
	bool	NeedRotateOnTargeting;
	float	Strength;
	int		WeaponLevel;
	float	EnergyUse;
	int		Ammo;
	float	NextFireTime;
	int		ObjectNum;
	sVECTOR3D	FireLocation;
	sVECTOR3D	DestrFireLocation;
	const char	*NameVW3D;
	const char	*TextureName;
	const char	*TextureIllumName;
};


// оружие землян 1-99
static sWeaponData PresetEarthWeaponData[] = {
	// Kinetic
	{9,  true, 25,	1,	1.5f,	3000,	0.4f, 4, sVECTOR3D(0.0f, -0.613f, 2.0f), sVECTOR3D(0.0f, -0.65f, 1.0f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{10, true, 25,	1,	3,		1500,	0.6f, 10, sVECTOR3D(0.0f, -0.613f, 2.93f), sVECTOR3D(0.0f, -0.6f, 1.8f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{11, true, 30,	2,	3.5,	1000,	0.7f, 12, sVECTOR3D(0.0f, -0.613f, 3.33f), sVECTOR3D(0.0f, -0.6f, 2.2f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{12, true, 30,	2,	5,		7000,	0.3f, 7, sVECTOR3D(0.0f, -0.613f, 2.33f), sVECTOR3D(0.0f, -0.6f, 1.2f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	// Ion
	{13, true, 10,	1,	7,		1000,	0.7f, 0, sVECTOR3D(0.0f, -0.43f, 2.13f), sVECTOR3D(0.0f, -0.45f, 1.5f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	{14, true, 10,	1,	10,		2000,	0.9f, 3, sVECTOR3D(0.0f, -0.53f, 1.86f), sVECTOR3D(0.0f, -0.5f, 1.8f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	{15, true, 15,	2,	11.5,	3000,	1.0f, 5, sVECTOR3D(0.0f, -0.63f, 2.26f), sVECTOR3D(0.0f, -0.65f, 2.3f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	// Plasma
	{16, true, 10,	2,	20,		2000,	0.6f, 2, sVECTOR3D(0.0f, -0.613f, 2.0f), sVECTOR3D(0.0f, -0.65f, 1.8f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{17, true, 15,	2,	30,		1000,	0.7f, 1, sVECTOR3D(0.0f, -0.8f, 1.86f), sVECTOR3D(0.0f, -0.8f, 1.8f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{18, true, 10,	3,	50,		800,	0.9f, 6, sVECTOR3D(0.0f, -0.613f, 1.2f), sVECTOR3D(0.0f, -0.7f, 1.6f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Maser
	{19, true, 10,	3,	50,		800,	3.0f, 9, sVECTOR3D(0.0f, -0.55f, 2.1f), sVECTOR3D(0.0f, -0.55f, 1.4f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text07.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{20, true, 15,	4,	80,		1000,	2.4f, 8, sVECTOR3D(0.0f, -0.55f, 2.5f), sVECTOR3D(0.0f, -0.55f, 1.8f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text07.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Antimatter
	{21, true, 20,	4,	50,		5000,	0.8f, 11, sVECTOR3D(0.0f, -0.65f, 2.1f), sVECTOR3D(0.0f, -0.65f, 1.9f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text09.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	// Laser
	{22, true, 15,	5,	150,	800,	1.2f, 13, sVECTOR3D(0.0f, -0.6f, 2.5f), sVECTOR3D(0.0f, -0.6f, 2.1f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text05.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Gauss
	{23, true, 20,	5,	150,	2000,	0.7f, 14, sVECTOR3D(0.0f, -0.65f, 3.6f), sVECTOR3D(0.0f, -0.55f, 2.2f), "models/earthfighter/weapons.vw3d", "models/earthfighter/sf-text08.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Missiles
	{24, false, 30,	4,	5,		200,	3.0f, 0, sVECTOR3D(0.0f, -0.8f, 4.7f), sVECTOR3D(0.0f, -0.9f, 1.5f), "models/earthfighter/lnch1.vw3d", "models/earthfighter/lnch12.tga", ""},
	{25, false, 30,	4,	15,		400,	8.0f, 0, sVECTOR3D(0.2f, -0.95f, 2.6f), sVECTOR3D(0.0f, -0.6f, 1.0f), "models/earthfighter/lnch2.vw3d", "models/earthfighter/lnch12.tga", ""},
	{26, false, 25,	5,	10,		50,		8.0f, 0, sVECTOR3D(0.0f, -0.95f, 4.0f), sVECTOR3D(0.0f, -0.9f, 1.5f), "models/earthfighter/lnch3.vw3d", "models/earthfighter/lnch34.tga", ""},
	{27, false, 30,	5,	15,		25,		10.0f, 0, sVECTOR3D(0.0f, -0.95f, 5.0f), sVECTOR3D(0.0f, -0.9f, 1.8f), "models/earthfighter/lnch4.vw3d", "models/earthfighter/lnch34.tga", ""},
};
#define PresetEarthWeaponDataCount sizeof(PresetEarthWeaponData)/sizeof(PresetEarthWeaponData[0])




// оружие пришельцев 101-199
static sWeaponData PresetAlienWeaponData[] = {
	// оружие пришельцев (как Kinetic1)
	{14, true, 1.0f,	1,	1,	5000,	0.7f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (с наведением, как Kinetic2)
	{16, false, 1.0f,	1,	1,	2500,	3.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Kinetic3)
	{17, true, 1.0f,	1,	1,	2500,	3.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (с наведением, как Kinetic3)
	{17, false, 1.0f,	1,	1,	2500,	2.5f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Kinetic2)
	{16, true, 1.0f,	1,	1,	2500,	3.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев для выброса мин (1 типа)
	{21, false, 1.0f,	1,	1,	50,		5.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев для выброса мин (2 типа)
	{21, false, 1.0f,	1,	1,	50,		5.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Plasma3)
	{18, true, 10,	3,	50,		800,	0.9f, 6, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Plasma2)
	{17, true, 15,	2,	25,		1000,	0.8f, 1, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Laser) для больших кораблей
	{22, true, 15,	5,	150,	800,	1.2f, 13, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},



};
#define PresetAlienWeaponDataCount sizeof(PresetAlienWeaponData)/sizeof(PresetAlienWeaponData[0])





// оружие пиратов 201-299
static sWeaponData PresetPirateWeaponData[] = {
	// турель для кораблей пиратов
	{9, false, 10,		1,	1,	3000,	0.7f, 4, sVECTOR3D(0.0f, 1.5f, 1.0f), sVECTOR3D(0.0f, 1.5f, 1.0f), "models/turret/turret-01.vw3d", "models/turret/turrets.tga", ""},
	// турель для кораблей пиратов
	{10, false, 10,		1,	1,	3000,	0.7f, 4, sVECTOR3D(0.0f, 1.0f, 0.0f), sVECTOR3D(0.0f, 1.0f, 0.0f), "models/turret/turret-02.vw3d", "models/turret/turrets.tga", ""},

	// оружие пиратов - отстрел фларес для больших кораблей
	{9, false, 1.0f,	1,	1,	2500,	4.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},

	// как Kinetic1
	{9, true, 10,		1,	1.7f,	3000,	0.7f, 4, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile1
	{24, false, 80,	4,	5,		200,	5.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile2
	{25, false, 30,	4,	5,		800,	8.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Ion2
	{14, true, 10,	1,	10,		2000,	0.9f, 3, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Antimatter
	{21, true, 20,	4,	50,		5000,	0.8f, 11, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile3 (торпеда)
	{26, false, 25,	5,	10,		200,	8.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile4 (бомба)
	{27, false, 30,	5,	15,		100,	10.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Kinetic2
	{10, true, 25,	1,	3,		1500,	0.6f, 10, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Kinetic3
	{11, true, 30,	2,	3.5,	500,	0.7f, 12, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Plasma2
	{17, true, 15,	2,	25,		1000,	0.8f, 1, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},

	// мина1 (простое наведение по высоте)
	{26, false, 1.0f,	1,	1,	2500,	4.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// мина2 (наведение по высоте + приближение)
	{26, false, 1.0f,	1,	1,	2500,	4.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// мина3 (наведение по высоте + стрельба снарядами)
	{27, false, 1.0f,	1,	1,	2500,	4.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// мина4 (наведение по высоте + стрельба ракетами)
	{27, false, 1.0f,	1,	1,	2500,	4.0f, 0, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},


};
#define PresetPirateWeaponDataCount sizeof(PresetPirateWeaponData)/sizeof(PresetPirateWeaponData[0])

























//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cWeapon::cWeapon(void)
{
	// друг
	ObjectStatus = 2;

	ObjectType = 9;

	// никогда не показываем линию
	ShowStrength = false;

	// вес
	Weight = 500;

	AttachWeapon(this);
}



//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cWeapon::Create(int WeaponNum)
{
	if (WeaponNum <= 0) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 1 && WeaponNum <= 99) && ((unsigned int)WeaponNum > PresetEarthWeaponDataCount)) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(1) object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 101 && WeaponNum <= 199) && ((unsigned int)WeaponNum-100 > PresetAlienWeaponDataCount)) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(2) object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 201 && WeaponNum <= 299) && ((unsigned int)WeaponNum-200 > PresetPirateWeaponDataCount)) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(3) object with Number " << WeaponNum << "\n";
		return;
	}

	ObjectCreationType = WeaponNum;




	// 1-99 - это оружие землян
	if (WeaponNum >= 1 && WeaponNum <= 99) {
		// союзник
		ObjectStatus = 2;

		Strength = StrengthStart = PresetEarthWeaponData[WeaponNum-1].Strength;
		WeaponLevel = PresetEarthWeaponData[WeaponNum-1].WeaponLevel;
		Ammo = AmmoStart =  PresetEarthWeaponData[WeaponNum-1].Ammo;
		NextFireTime =  PresetEarthWeaponData[WeaponNum-1].NextFireTime;
		SoundNum = PresetEarthWeaponData[WeaponNum-1].SoundNum;
		NeedRotateOnTargeting = PresetEarthWeaponData[WeaponNum-1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetEarthWeaponData[WeaponNum-1].EnergyUse;
		FireLocation = PresetEarthWeaponData[WeaponNum-1].FireLocation;

		DestroyedFireLocation = PresetEarthWeaponData[WeaponNum-1].DestrFireLocation;
		Fire = new cParticleSystem;
		Fire->SetStartLocation(FireLocation);
		Fire->Direction = sVECTOR3D(0.0f, 0.0f, 1.0f);
		SetWeaponFire(Fire, WeaponNum);

		LoadObjectData(PresetEarthWeaponData[WeaponNum-1].NameVW3D, this, PresetEarthWeaponData[WeaponNum-1].ObjectNum+1, 2.0f);

		for (int i=0; i<DrawObjectQuantity; i++) {
			Texture[i] = vw_FindTextureByName(PresetEarthWeaponData[WeaponNum-1].TextureName);
			if (WeaponNum < 16)
				TextureIllum[i] = vw_FindTextureByName(PresetEarthWeaponData[WeaponNum-1].TextureIllumName);
		}

		// находим все данные по геометрии
		::cObject3D::InitByDrawObjectList();

	}
	// 101-199 - это оружие пришельцев
	else if (WeaponNum >= 101 && WeaponNum <= 199) {
		// внутренний номер
		int IntWeaponNum = WeaponNum - 100;

		// враг
		ObjectStatus = 1;

		Strength = StrengthStart = PresetAlienWeaponData[IntWeaponNum-1].Strength;
		WeaponLevel = PresetAlienWeaponData[IntWeaponNum-1].WeaponLevel;
		Ammo = AmmoStart =  PresetAlienWeaponData[IntWeaponNum-1].Ammo;
		NextFireTime =  PresetAlienWeaponData[IntWeaponNum-1].NextFireTime;
		SoundNum = PresetAlienWeaponData[IntWeaponNum-1].SoundNum;
		NeedRotateOnTargeting = PresetAlienWeaponData[IntWeaponNum-1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetAlienWeaponData[IntWeaponNum-1].EnergyUse;
		FireLocation = PresetAlienWeaponData[IntWeaponNum-1].FireLocation;

	}
	// 201-299 - это оружие пиратов
	else if (WeaponNum >= 201 && WeaponNum <= 299) {
		// внутренний номер
		int IntWeaponNum = WeaponNum - 200;

		// враг
		ObjectStatus = 1;

		Strength = StrengthStart = PresetPirateWeaponData[IntWeaponNum-1].Strength;
		WeaponLevel = PresetPirateWeaponData[IntWeaponNum-1].WeaponLevel;
		Ammo = AmmoStart =  PresetPirateWeaponData[IntWeaponNum-1].Ammo;
		NextFireTime =  PresetPirateWeaponData[IntWeaponNum-1].NextFireTime;
		SoundNum = PresetPirateWeaponData[IntWeaponNum-1].SoundNum;
		NeedRotateOnTargeting = PresetPirateWeaponData[IntWeaponNum-1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetPirateWeaponData[IntWeaponNum-1].EnergyUse;
		FireLocation = PresetPirateWeaponData[IntWeaponNum-1].FireLocation;

		// турели пиратов
		switch (WeaponNum) {
		case 201: {
			WeaponTurret = true;
			TargetHorizObject = 0;
			TargetVertObject = 1;
			TargetVertObjectMaxAngle = 89.0f;
			TargetVertObjectMinAngle = 0.0f;

			LoadObjectData(PresetPirateWeaponData[IntWeaponNum-1].NameVW3D, this, 0, 2.0f);
			for (int i=0; i<DrawObjectQuantity; i++) {
				Texture[i] = vw_FindTextureByName(PresetPirateWeaponData[IntWeaponNum-1].TextureName);
				TextureIllum[i] = nullptr;
			}

			// находим все данные по геометрии
			::cObject3D::InitByDrawObjectList();
		}
		break;


		case 202: {
			WeaponTurret = true;
			TargetHorizObject = 0;
			TargetVertObject = 1;
			TargetVertObjectMaxAngle = 89.0f;
			TargetVertObjectMinAngle = 20.0f;

			LoadObjectData(PresetPirateWeaponData[IntWeaponNum-1].NameVW3D, this, 0, 2.0f);
			for (int i=0; i<DrawObjectQuantity; i++) {
				Texture[i] = vw_FindTextureByName(PresetPirateWeaponData[IntWeaponNum-1].TextureName);
				TextureIllum[i] = nullptr;
			}

			// находим все данные по геометрии
			::cObject3D::InitByDrawObjectList();
		}
		break;
		}

		if (WeaponNum == 201 || WeaponNum == 202) {
			// вычисляем данные для нахождения точки стрельбы
			if (TargetHorizObject != -1) {
				BaseBound = DrawObjectList[TargetHorizObject].Location;
			}

			if (TargetVertObject != -1) {
				if (TargetHorizObject != -1) {
					MiddleBound = DrawObjectList[TargetVertObject].Location - DrawObjectList[TargetHorizObject].Location;
				} else {
					MiddleBound = DrawObjectList[TargetVertObject].Location;
				}
			}


			if (TargetVertObject != -1) {
				WeaponBound = FireLocation - DrawObjectList[TargetVertObject].Location;
			} else if (TargetHorizObject != -1) {
				WeaponBound = FireLocation - DrawObjectList[TargetHorizObject].Location;
			} else {
				WeaponBound = FireLocation;
			}

		}



	}


}










//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cWeapon::~cWeapon(void)
{
	if (Fire != nullptr) {
		Fire->IsSuppressed = true;
		Fire->DestroyIfNoParticles = true;
		Fire = nullptr;
	}
	if (DestroyedFire != nullptr) {
		DestroyedFire->IsSuppressed = true;
		DestroyedFire->DestroyIfNoParticles = true;
		DestroyedFire = nullptr;
	}
	if (DestroyedSmoke != nullptr) {
		DestroyedSmoke->IsSuppressed = true;
		DestroyedSmoke->DestroyIfNoParticles = true;
		DestroyedSmoke = nullptr;
	}
	// если лучевое оружие
	if (LaserMaser != nullptr) {
		delete LaserMaser;
		LaserMaser = nullptr;
		// убираем звук выстрела
		if (vw_IsSoundAvailable(LaserMaserSoundNum))
			vw_StopSound(LaserMaserSoundNum, 150);
	}

	DetachWeapon(this);
}









//-----------------------------------------------------------------------------
// выполнение действий
//-----------------------------------------------------------------------------
bool cWeapon::Update(float Time)
{

	// если лучевое оружие
	if (LaserMaser != nullptr) {
		if (LaserMaser->Lifetime <= 0.0f) {
			delete LaserMaser;
			LaserMaser = nullptr;
		}
	}


	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!::cObject3D::Update(Time)) {
		return false;
	}



	// если запущено - выключаем прорисовку эффекта выстрела (вспышка возле ствола)
	if ((Fire != nullptr) &&
	    !Fire->IsSuppressed &&
	    (LastFireTime + TimeDelta <= Time))
		Fire->IsSuppressed = true;





	// для землян, создание эффекта повреждения оружия
	// если оружие только повредили...
	if ((ObjectCreationType >= 1) &&
	    (ObjectCreationType <= 99) &&
	    (Strength < StrengthStart) &&
	    (DestroyedFire == nullptr)) {
		// горение
		DestroyedFire = new cParticleSystem;
		DestroyedFire->ColorStart.r = 1.00f;
		DestroyedFire->ColorStart.g = 0.70f;
		DestroyedFire->ColorStart.b = 0.30f;
		DestroyedFire->ColorEnd.r = 1.00f;
		DestroyedFire->ColorEnd.g = 0.00f;
		DestroyedFire->ColorEnd.b = 0.00f;
		DestroyedFire->AlphaStart = 1.00f;
		DestroyedFire->AlphaEnd   = 0.10f;
		DestroyedFire->SizeStart  = 0.20f;
		DestroyedFire->SizeVar    = 0.10f;
		DestroyedFire->SizeEnd    = 0.10f;
		DestroyedFire->Speed      = 8.00f;
		DestroyedFire->SpeedVar   = 2.00f;
		DestroyedFire->Theta      = 5.00f;
		DestroyedFire->Life       = 0.50f*Length/3.0f;
		DestroyedFire->ParticlesPerSec = 70;
		DestroyedFire->Texture[0] = vw_FindTextureByName("gfx/flare1.tga");
		DestroyedFire->CreationType = 1;
		DestroyedFire->CreationSize = sVECTOR3D(Width/2.0f,Width/2.0f,0.1f);
		DestroyedFire->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);
		DestroyedFire->SetStartLocation(DestroyedFireLocation);

		// небольшая поправка для ракетных систем

		if (ObjectCreationType == 16 || ObjectCreationType == 17) {
			DestroyedFire->DeadZone = Width/2.0f - 0.1f;
		} else if (ObjectCreationType == 18) {
			DestroyedFire->CreationSize = sVECTOR3D(Width/3.5f,Width/3.5f,0.1f);
			DestroyedFire->DeadZone = Width/3.5f - 0.1f;
		} else if (ObjectCreationType == 19) {
			DestroyedFire->CreationSize = sVECTOR3D(Width/3.0f,Width/3.0f,0.1f);
			DestroyedFire->DeadZone = Width/3.0f - 0.1f;
		}
	}


	// для землян, создание эффекта уничтоженного оружия
	// проверяем тут был ли выход из строя
	if ((ObjectCreationType >= 1) &&
	    (ObjectCreationType <= 99) &&
	    (Strength <= 0.0f) &&
	    (DestroyedSmoke == nullptr)) {
		// дым
		DestroyedSmoke = new cParticleSystem;
		DestroyedSmoke->ColorStart.r = 1.00f;
		DestroyedSmoke->ColorStart.g = 1.00f;
		DestroyedSmoke->ColorStart.b = 1.00f;
		DestroyedSmoke->ColorEnd.r = 1.00f;
		DestroyedSmoke->ColorEnd.g = 1.00f;
		DestroyedSmoke->ColorEnd.b = 1.00f;
		DestroyedSmoke->AlphaStart = 0.20f;
		DestroyedSmoke->AlphaEnd   = 0.00f;
		DestroyedSmoke->SizeStart  = 0.25f;
		DestroyedSmoke->SizeVar    = 0.10f;
		DestroyedSmoke->SizeEnd    = 0.00f;
		DestroyedSmoke->Speed      = 0.00f;
		DestroyedSmoke->SpeedVar   = 0.00f;
		DestroyedSmoke->Theta      = 35.00f;
		DestroyedSmoke->Life       = 2.00f*Length/3.0f;
		DestroyedSmoke->ParticlesPerSec = 300;
		DestroyedSmoke->Texture[0] = vw_FindTextureByName("gfx/flare1.tga");
		DestroyedSmoke->CreationType = 0;
		DestroyedSmoke->CreationSize = sVECTOR3D(Width/2.5f,Width/2.5f,0.1f);
		DestroyedSmoke->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);
		DestroyedSmoke->SetStartLocation(DestroyedFireLocation);
	}







	// если это swamp - запуск других ракет
	if (ObjectCreationType == 17 && SwampNum > 0)
		if (LastFireTime + 0.15f < Time)
			if (Ammo > 0 || GameLimitedAmmo != 1) {
				LastFireTime = Time;

				// ум. кол-во боеприпасов, если нужно
				if (GameLimitedAmmo == 0)	Ammo -= 1;

				// общий - пенальти, если не игрок
				float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
				// если игрок - ничего не надо...
				if (ObjectStatus >= 2) CurrentPenalty = 1.0f;

				switch (SwampNum) {
				case 9:
					FireLocation = sVECTOR3D(-0.5f, -0.5f, 2.6f);
					break;
				case 8:
					FireLocation = sVECTOR3D(0.5f, -0.8f, 2.6f);
					break;
				case 7:
					FireLocation = sVECTOR3D(-0.5f, -0.8f, 2.6f);
					break;
				case 6:
					FireLocation = sVECTOR3D(0.5f, -0.5f, 2.6f);
					break;
				case 5:
					FireLocation = sVECTOR3D(-0.2f, -0.95f, 2.6f);
					break;
				case 4:
					FireLocation = sVECTOR3D(0.2f, -0.65f, 2.6f);
					break;
				case 3:
					FireLocation = sVECTOR3D(-0.2f, -0.3f, 2.6f);
					break;
				case 2:
					FireLocation = sVECTOR3D(0.2f, -0.65f, 2.6f);
					break;
				case 1:
					FireLocation = sVECTOR3D(0.2f, -0.3f, 2.6f);
					break;
				}
				vw_Matrix33CalcPoint(FireLocation, CurrentRotationMat);

				// создаем снаряд
				cProjectile *Projectile = new cProjectile;
				Projectile->Create(ObjectCreationType);
				Projectile->SetLocation(Location+FireLocation);
				Projectile->SetRotation(Rotation);
				for (int i = 0; i < Projectile->GraphicFXQuantity; i++) {
					Projectile->GraphicFX[i]->Direction = Fire->Direction^-1;
					// учитываем пенальти для визуальных эффектов
					if (CurrentPenalty == 2)
						Projectile->GraphicFX[i]->ParticlesPerSec -= (int)(Projectile->GraphicFX[i]->ParticlesPerSec*0.33f);
					if (CurrentPenalty == 3)
						Projectile->GraphicFX[i]->ParticlesPerSec -= (int)(Projectile->GraphicFX[i]->ParticlesPerSec*0.5f);
					Projectile->GraphicFX[i]->Speed = Projectile->GraphicFX[i]->Speed/CurrentPenalty;
					Projectile->GraphicFX[i]->Life = Projectile->GraphicFX[i]->Life*CurrentPenalty;
					Projectile->GraphicFX[i]->MagnetFactor = Projectile->GraphicFX[i]->MagnetFactor/(CurrentPenalty*CurrentPenalty);
				}
				Projectile->ObjectStatus = ObjectStatus;
				// учитываем пенальти для снаряда
				Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
				Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
				Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
				Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;


				// звук...
				if (SoundNum != 0) {
					float fVol = 1.0f;
					Audio_PlaySound3D(SoundNum, fVol, Projectile->Location);
				}


				SwampNum--;
			}





	// если это фларес - выпускаем остальные
	// SwampNum в этом случае используем с другой целью
	if (ObjectCreationType == 203 && SwampNum > 0)
		if (LastFireTime + 0.4f < Time)
			if (Ammo > 0 || GameLimitedAmmo != 1) {
				LastFireTime = Time;

				// ум. кол-во боеприпасов, если нужно
				if (GameLimitedAmmo == 0)	Ammo -= 1;

				// общий - пенальти, если не игрок
				float CurrentPenalty = 1.0f;


				// создаем снаряд
				cProjectile *Projectile = new cProjectile;
				Projectile->Create(ObjectCreationType);
				Projectile->SetLocation(Location + FireLocation);
				Projectile->SetRotation(Rotation + sVECTOR3D(vw_Randf0 * 30.0f, 0.0f, vw_Randf0 * 30.0f));

				for (int i = 0; i < Projectile->GraphicFXQuantity; i++) {
					Projectile->GraphicFX[i]->Direction = Orientation^-1;
					// учитываем пенальти для визуальных эффектов
					if (CurrentPenalty == 2)
						Projectile->GraphicFX[i]->ParticlesPerSec -= (int)(Projectile->GraphicFX[i]->ParticlesPerSec*0.33f);
					if (CurrentPenalty == 3)
						Projectile->GraphicFX[i]->ParticlesPerSec -= (int)(Projectile->GraphicFX[i]->ParticlesPerSec*0.5f);
					Projectile->GraphicFX[i]->Speed = Projectile->GraphicFX[i]->Speed/CurrentPenalty;
					Projectile->GraphicFX[i]->Life = Projectile->GraphicFX[i]->Life*CurrentPenalty;
					Projectile->GraphicFX[i]->MagnetFactor = Projectile->GraphicFX[i]->MagnetFactor/(CurrentPenalty*CurrentPenalty);
				}
				Projectile->ObjectStatus = ObjectStatus;
				// учитываем пенальти для снаряда
				Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
				Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
				Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
				Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;


				// звук...
				if (SoundNum != 0) {
					float fVol = 1.0f;
					Audio_PlaySound3D(SoundNum, fVol, Projectile->Location);
				}

				SwampNum--;
			}








	// для турелей
	if (WeaponTurret) {

		// если нужно наводить на цель
		//if (Fire != 0)
		{
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// наводим на цель, если есть возможность
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			// находимся в начальном состоянии поворота ствола
			sVECTOR3D NeedAngle(TargetVertObjectNeedAngle,TargetHorizObjectNeedAngle,0);
			GetTurretOnTargetOrientateion(ObjectStatus, Location+FireLocation, Rotation,
						      CurrentRotationMat,	&NeedAngle, ObjectCreationType);

			// наводим на цель
			TargetHorizObjectNeedAngle = NeedAngle.y;
			TargetVertObjectNeedAngle = NeedAngle.x;
		}
		/*else
		{
			// устанавливаем в начальное положение
			TargetHorizObjectNeedAngle = 0.0f;
			TargetVertObjectNeedAngle = 0.0f;
		}*/






		// собственно повороты элементов модели на углы

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот башни по горизонтале
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (TargetHorizObject != -1)
			if (TargetHorizObjectNeedAngle != TargetHorizObjectCurrentAngle) {
				if (fabsf(TargetHorizObjectNeedAngle-TargetHorizObjectCurrentAngle) > 180.0f) {
					if (TargetHorizObjectCurrentAngle - TargetHorizObjectNeedAngle > 180.0f) TargetHorizObjectCurrentAngle -= 360.0f;
					if (TargetHorizObjectNeedAngle - TargetHorizObjectCurrentAngle > 180.0f) TargetHorizObjectCurrentAngle += 360.0f;
				}

				// находим угол, на который нужно повернуть
				float NeedRotate = TargetHorizObjectCurrentAngle;

				if (TargetHorizObjectNeedAngle>TargetHorizObjectCurrentAngle) {
					NeedRotate += 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
					if (NeedRotate > TargetHorizObjectNeedAngle) NeedRotate = TargetHorizObjectNeedAngle;
				} else {
					NeedRotate -= 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
					if (NeedRotate < TargetHorizObjectNeedAngle) NeedRotate = TargetHorizObjectNeedAngle;
				}

				// устанавливаем текущий поворот
				TargetHorizObjectCurrentAngle = NeedRotate;

				// поворачиваем все объекты
				for (int i=0; i<DrawObjectQuantity; i++) {
					sVECTOR3D tmp = DrawObjectList[i].Location-DrawObjectList[TargetHorizObject].Location;

					vw_RotatePointInv(tmp, DrawObjectList[i].Rotation^(-1.0f));

					DrawObjectList[i].Rotation.y = -NeedRotate;

					vw_RotatePoint(tmp, DrawObjectList[i].Rotation);

					DrawObjectList[i].Location = tmp+DrawObjectList[TargetHorizObject].Location;
				}
			}






		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот стволов по вертикали
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (TargetVertObject != -1)
			if (TargetVertObjectNeedAngle != TargetVertObjectCurrentAngle) {

				// находим угол, на который нужно повернуть
				float NeedRotate = TargetVertObjectCurrentAngle;
				if (TargetVertObjectNeedAngle>TargetVertObjectCurrentAngle) {
					NeedRotate += 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
					if (NeedRotate > TargetVertObjectNeedAngle) NeedRotate = TargetVertObjectNeedAngle;
					if (NeedRotate > TargetVertObjectMaxAngle) NeedRotate = TargetVertObjectMaxAngle;
				} else {
					NeedRotate -= 80.0f*TimeDelta/GameNPCTargetingSpeedPenalty;
					if (NeedRotate < TargetVertObjectNeedAngle) NeedRotate = TargetVertObjectNeedAngle;
					if (NeedRotate < TargetVertObjectMinAngle) NeedRotate = TargetVertObjectMinAngle;
				}

				// устанавливаем текущий поворот
				TargetVertObjectCurrentAngle = NeedRotate;

				// поворачиваем все объекты
				sVECTOR3D tmp = DrawObjectList[TargetVertObject].Location-DrawObjectList[TargetVertObject].Location;

				vw_RotatePointInv(tmp, DrawObjectList[TargetVertObject].Rotation^(-1.0f));

				DrawObjectList[TargetVertObject].Rotation.x = -NeedRotate;

				vw_RotatePoint(tmp, DrawObjectList[TargetVertObject].Rotation);

				DrawObjectList[TargetVertObject].Location = tmp+DrawObjectList[TargetVertObject].Location;
			}


		// турель
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizObject != -1) {
			RotationMiddle = DrawObjectList[TargetHorizObject].Rotation+Rotation;
		}
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		sVECTOR3D RotationWeapon = Rotation;
		if (TargetVertObject != -1) {
			RotationWeapon = DrawObjectList[TargetVertObject].Rotation+Rotation;
		}

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;


		// особый случай, испускаем без вращающихся частей (антиматерия, ион)
		if (TargetHorizObject == -1 && TargetVertObject == -1) {
			RotationWeapon = sVECTOR3D(TargetVertObjectNeedAngle, TargetHorizObjectNeedAngle, 0.0f)+Rotation;
		}

		Orientation = sVECTOR3D(0.0f, 0.0f, 1.0f);
		vw_RotatePoint(Orientation, RotationWeapon);
	}




	// объект в порядке - удалять не нужно
	return true;

}













//-----------------------------------------------------------------------------
// Установка положения
//-----------------------------------------------------------------------------
void cWeapon::SetRotation(sVECTOR3D NewRotation)
{

	// вызываем родительскую функцию
	::cObject3D::SetRotation(NewRotation);


	if (!WeaponTurret) {
		// положение точки выстрела
		vw_Matrix33CalcPoint(FireLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(FireLocation, CurrentRotationMat);

		if (Fire != nullptr) {
			Fire->MoveSystem(Location + FireLocation);
			Fire->SetStartLocation(Location + FireLocation);
			Fire->RotateSystemByAngle(Rotation);
		}
	} else {
		// турель
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizObject != -1)
			RotationMiddle = DrawObjectList[TargetHorizObject].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		sVECTOR3D RotationWeapon = Rotation;
		if (TargetVertObject != -1)
			RotationWeapon = DrawObjectList[TargetVertObject].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

		// особый случай, испускаем без вращающихся частей (антиматерия, ион)
		if (TargetHorizObject == -1 && TargetVertObject == -1)
			RotationWeapon = sVECTOR3D(TargetVertObjectNeedAngle, TargetHorizObjectNeedAngle, 0.0f)+Rotation;

		Orientation = sVECTOR3D(0.0f, 0.0f, 1.0f);
		vw_RotatePoint(Orientation, RotationWeapon);
	}




	if (DestroyedFire != nullptr) {
		vw_Matrix33CalcPoint(DestroyedFireLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(DestroyedFireLocation, CurrentRotationMat);
		DestroyedFire->MoveSystem(Location + DestroyedFireLocation);
		DestroyedFire->SetStartLocation(Location + DestroyedFireLocation);
		DestroyedFire->RotateSystemByAngle(Rotation);
	}
	// тут DestroyedFireLocation не считаем, т.к. все равно всегда создаем DestroyedFire
	if (DestroyedSmoke != nullptr) {
		DestroyedSmoke->MoveSystemLocation(Location + DestroyedFireLocation);
		DestroyedSmoke->RotateSystemByAngle(Rotation);
	}
	// если лучевое оружие
	if (LaserMaser != nullptr) {
		vw_Matrix33CalcPoint(LaserMaser->ProjectileCenter, OldInvRotationMat);
		vw_Matrix33CalcPoint(LaserMaser->ProjectileCenter, CurrentRotationMat);
		LaserMaser->SetLocation(Location+FireLocation+LaserMaser->ProjectileCenter);
		LaserMaser->SetRotation(NewRotation);
	}
}







//-----------------------------------------------------------------------------
// Установка положения
//-----------------------------------------------------------------------------
void cWeapon::SetLocation(sVECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::cObject3D::SetLocation(NewLocation);

	// положение утечки
	if (Fire != nullptr) {
		Fire->MoveSystem(NewLocation + FireLocation);
		Fire->SetStartLocation(NewLocation + FireLocation);
	}
	if (DestroyedFire != nullptr) {
		DestroyedFire->MoveSystem(NewLocation + DestroyedFireLocation);
		DestroyedFire->SetStartLocation(NewLocation + DestroyedFireLocation);
	}
	if (DestroyedSmoke != nullptr)
		DestroyedSmoke->MoveSystemLocation(NewLocation + DestroyedFireLocation);

	// если лучевое оружие
	if (LaserMaser != nullptr)
		LaserMaser->SetLocation(Location+FireLocation+LaserMaser->ProjectileCenter);



	// звук... для мазеров-лазеров нужно учитывать перемещение
	if (vw_IsSoundAvailable(LaserMaserSoundNum))  // уже играем, нужно изменить данные
		vw_SetSoundLocation(LaserMaserSoundNum, Location.x, Location.y, Location.z);

}







//-----------------------------------------------------------------------------
// Выстрел
//-----------------------------------------------------------------------------
bool cWeapon::WeaponFire(float Time)
{

	// если оружие не установлено - нам тут делать нечего
	if (ObjectCreationType == 0) return false;


	// общий - пенальти, если не игрок
	float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
	// если игрок или свои - ничего не надо...
	if (ObjectStatus >= 2) CurrentPenalty = 1.0f;


	// если фларес - тоже ничего не надо
	if (ObjectCreationType == 203) CurrentPenalty = 1.0f;


	// проверяем по времени, можем стрелять или нет
	// делаем ее тут, чтобы "правильно" озвучивать проблемы стрельбы
	if (Time < LastFireTime + NextFireTime*CurrentPenalty) return false;
	// запоминаем время стрельбы
	LastFireTime = Time;



	// для оружия землян, делаем небольшие проверки...
	if (ObjectCreationType >= 1 && ObjectCreationType <= 99) {

		// для поврежденного оружия проверяем... может быть осечка в стрельбе
		bool Misfire = false;
		if (Strength < StrengthStart)
			if (Strength/StrengthStart > vw_Randf1) {
				Misfire = true;
			}


		// если оружие уничтожено или уже нечем стрелять - нам тут делать нечего
		if (Strength <= 0.0f || Ammo == 0 || Misfire) {
			// перед тем как уйти, ставим проигрывание звука "проблема с оружием"

			float fVol = 1.0f;

			switch (ObjectCreationType) {
			// Kinetic
			case 1:
			case 2:
			case 3:
			case 4:
				Audio_PlaySound3D(1, fVol, Location);
				break;

			// Ion
			case 5:
			case 6:
			case 7:
			// Plasma
			case 8:
			case 9:
			case 10:
				Audio_PlaySound3D(2, fVol, Location);
				break;

			// Maser
			case 11:
			case 12:
			// Laser
			case 14:
				Audio_PlaySound3D(3, fVol, Location);

				break;
			// Antimatter
			case 13:
			// Gauss
			case 15:
				Audio_PlaySound3D(4, fVol, Location);
				break;

			// ракета
			case 16:
			// рой
			case 17:
			// торпеда
			case 18:
			// бомба
			case 19:
				Audio_PlaySound3D(5, fVol, Location);
				break;
			}

			return false;
		}




		// только для игрока! проверяем заряд энергии для выстрела
		if (ObjectStatus == 3) {
			if (CurrentEnergyAccumulated < EnergyUse) return false;
			else CurrentEnergyAccumulated = 0.0f;
		}

		// ум. кол-во боеприпасов, если нужно
		if (GameLimitedAmmo == 0) Ammo -= 1;


		// делаем вспышку возле ствола для всего оружия землят (только землян), если это не ракетная установка
		if (ObjectCreationType < 16)
			Fire->IsSuppressed = false;

	}











	sVECTOR3D RotationWeapon = Rotation;
	if (WeaponTurret) {
		// турель
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizObject != -1) {
			RotationMiddle = DrawObjectList[TargetHorizObject].Rotation + Rotation;
		}
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		if (TargetVertObject != -1) {
			RotationWeapon = DrawObjectList[TargetVertObject].Rotation + Rotation;
		}

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;


		// особый случай, испускаем без вращающихся частей (антиматерия, ион)
		if (TargetHorizObject == -1 && TargetVertObject == -1) {
			RotationWeapon = sVECTOR3D(TargetVertObjectNeedAngle, TargetHorizObjectNeedAngle, 0.0f)+Rotation;
		}

	}









	// создаем снаряд
	cProjectile *Projectile = new cProjectile;
	Projectile->Create(ObjectCreationType);


	// если лучевое оружие, немного все делаем по другому
	// или это испускатель мин
	if (Projectile->ProjectileType == 2) {
		if (LaserMaser != nullptr) {
			delete LaserMaser;
			LaserMaser = nullptr;
		}
		LaserMaser = Projectile;
		vw_Matrix33CalcPoint(LaserMaser->ProjectileCenter, CurrentRotationMat);
		LaserMaser->SetLocation(Location+FireLocation+LaserMaser->ProjectileCenter);
	} else { // если это снаряд, его нужно немного вынести, так лучше смотрится
		if (Projectile->ProjectileType == 0) {
			sVECTOR3D ADDPOS(0,0,4.0f);
			if (WeaponTurret) ADDPOS = sVECTOR3D(0,0,2.0f);

			vw_RotatePoint(ADDPOS, RotationWeapon);

			Projectile->SetLocation(Location+FireLocation+ADDPOS);
		} else // для ракет и мин все без изменения
			Projectile->SetLocation(Location+FireLocation);
	}






	// если это мина, то нужно делать немного по другому
	if (Projectile->ProjectileType == 3 || Projectile->ProjectileType == 4) {
		Projectile->SetRotation(RotationWeapon);
		for (int i=0; i<Projectile->GraphicFXQuantity; i++) {
			Projectile->GraphicFX[i]->Direction = Orientation;
			// учитываем пенальти для визуальных эффектов
			Projectile->GraphicFX[i]->ParticlesPerSec = (int)(Projectile->GraphicFX[i]->ParticlesPerSec);

			Projectile->GraphicFX[i]->Speed = Projectile->GraphicFX[i]->Speed;
			Projectile->GraphicFX[i]->Life = Projectile->GraphicFX[i]->Life;
			Projectile->GraphicFX[i]->MagnetFactor = Projectile->GraphicFX[i]->MagnetFactor;
		}
		Projectile->ObjectStatus = ObjectStatus;
		// учитываем пенальти для снаряда
		Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
		Projectile->Age = Projectile->Lifetime = Projectile->Age;
		Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
		Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;

		// приводим к типу снаряда (0-обычный или 1-уничтожаемый)
		Projectile->ProjectileType = Projectile->ProjectileType - 3;
	} else {
		Projectile->SetRotation(RotationWeapon);
		for (int i=0; i<Projectile->GraphicFXQuantity; i++) {
			Projectile->GraphicFX[i]->Direction = Orientation;//Fire->Direction^-1;
			// учитываем пенальти для визуальных эффектов
			Projectile->GraphicFX[i]->ParticlesPerSec = (int)(Projectile->GraphicFX[i]->ParticlesPerSec/CurrentPenalty);

			Projectile->GraphicFX[i]->Speed = Projectile->GraphicFX[i]->Speed/CurrentPenalty;
			Projectile->GraphicFX[i]->Life = Projectile->GraphicFX[i]->Life*CurrentPenalty;
			Projectile->GraphicFX[i]->MagnetFactor = Projectile->GraphicFX[i]->MagnetFactor/(CurrentPenalty*CurrentPenalty);
		}
		Projectile->ObjectStatus = ObjectStatus;
		// учитываем пенальти для снаряда
		Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
		Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
		Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
		Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;
	}






	// звук...
	if (SoundNum != 0) {
		float fVol = 1.0f;
		LaserMaserSoundNum = Audio_PlaySound3D(SoundNum, fVol, Location+FireLocation);
		// если не надо сохранять
		if (LaserMaser == nullptr)
			LaserMaserSoundNum = 0;
	}






	// если это груповой выстрел:

	// нужно создать еще 9 ракет
	if (ObjectCreationType == 17)
		SwampNum = 9;

	// нужно создать еще 4 фларес
	if (ObjectCreationType == 203)
		SwampNum = 4;






	// выстрел был
	return true;
}


