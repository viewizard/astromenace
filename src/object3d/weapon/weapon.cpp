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

#include "weapon.h"
#include "../../assets/audio.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// Указатели на начальный и конечный объект в списке
cWeapon *StartWeapon = nullptr;
cWeapon *EndWeapon = nullptr;

} // unnamed namespace

struct sWeaponData {
	eGameSFX SFX;
	bool NeedRotateOnTargeting;
	float Strength;
	int WeaponLevel;
	float EnergyUse;
	int Ammo;
	float NextFireTime;
	sVECTOR3D FireLocation;
	sVECTOR3D DestrFireLocation;
	const char *NameVW3D;
	const char *TextureName;
	const char *TextureIllumName;
};

// оружие землян 1-99
const std::vector<sWeaponData> PresetEarthWeaponData{
	// Kinetic
	{eGameSFX::WeaponFire_Kinetic1,		true, 25,	1,	1.5f,	3000,	0.4f, sVECTOR3D(0.0f, -0.613f, 2.0f), sVECTOR3D(0.0f, -0.65f, 1.0f), "models/earthfighter/weapon5.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{eGameSFX::WeaponFire_Kinetic2,		true, 25,	1,	3,	1500,	0.6f, sVECTOR3D(0.0f, -0.613f, 2.93f), sVECTOR3D(0.0f, -0.6f, 1.8f), "models/earthfighter/weapon11.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{eGameSFX::WeaponFire_Kinetic3,		true, 30,	2,	3.5,	1000,	0.7f, sVECTOR3D(0.0f, -0.613f, 3.33f), sVECTOR3D(0.0f, -0.6f, 2.2f), "models/earthfighter/weapon13.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{eGameSFX::WeaponFire_Kinetic4,		true, 30,	2,	5,	7000,	0.3f, sVECTOR3D(0.0f, -0.613f, 2.33f), sVECTOR3D(0.0f, -0.6f, 1.2f), "models/earthfighter/weapon8.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	// Ion
	{eGameSFX::WeaponFire_Ion1,		true, 10,	1,	7,	1000,	0.7f, sVECTOR3D(0.0f, -0.43f, 2.13f), sVECTOR3D(0.0f, -0.45f, 1.5f), "models/earthfighter/weapon1.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	{eGameSFX::WeaponFire_Ion2,		true, 10,	1,	10,	2000,	0.9f, sVECTOR3D(0.0f, -0.53f, 1.86f), sVECTOR3D(0.0f, -0.5f, 1.8f), "models/earthfighter/weapon4.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	{eGameSFX::WeaponFire_Ion3,		true, 15,	2,	11.5,	3000,	1.0f, sVECTOR3D(0.0f, -0.63f, 2.26f), sVECTOR3D(0.0f, -0.65f, 2.3f), "models/earthfighter/weapon6.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	// Plasma
	{eGameSFX::WeaponFire_Plasma1,		true, 10,	2,	20,	2000,	0.6f, sVECTOR3D(0.0f, -0.613f, 2.0f), sVECTOR3D(0.0f, -0.65f, 1.8f), "models/earthfighter/weapon3.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{eGameSFX::WeaponFire_Plasma2,		true, 15,	2,	30,	1000,	0.7f, sVECTOR3D(0.0f, -0.8f, 1.86f), sVECTOR3D(0.0f, -0.8f, 1.8f), "models/earthfighter/weapon2.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{eGameSFX::WeaponFire_Plasma3,		true, 10,	3,	50,	800,	0.9f, sVECTOR3D(0.0f, -0.613f, 1.2f), sVECTOR3D(0.0f, -0.7f, 1.6f), "models/earthfighter/weapon7.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Maser
	{eGameSFX::WeaponFire_Maser1,		true, 10,	3,	50,	800,	3.0f, sVECTOR3D(0.0f, -0.55f, 2.1f), sVECTOR3D(0.0f, -0.55f, 1.4f), "models/earthfighter/weapon10.vw3d", "models/earthfighter/sf-text07.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{eGameSFX::WeaponFire_Maser2,		true, 15,	4,	80,	1000,	2.4f, sVECTOR3D(0.0f, -0.55f, 2.5f), sVECTOR3D(0.0f, -0.55f, 1.8f), "models/earthfighter/weapon9.vw3d", "models/earthfighter/sf-text07.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Antimatter
	{eGameSFX::WeaponFire_Antimatter,	true, 20,	4,	50,	5000,	0.8f, sVECTOR3D(0.0f, -0.65f, 2.1f), sVECTOR3D(0.0f, -0.65f, 1.9f), "models/earthfighter/weapon12.vw3d", "models/earthfighter/sf-text09.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	// Laser
	{eGameSFX::WeaponFire_Laser,		true, 15,	5,	150,	800,	1.2f, sVECTOR3D(0.0f, -0.6f, 2.5f), sVECTOR3D(0.0f, -0.6f, 2.1f), "models/earthfighter/weapon14.vw3d", "models/earthfighter/sf-text05.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Gauss
	{eGameSFX::WeaponFire_Gauss,		true, 20,	5,	150,	2000,	0.7f, sVECTOR3D(0.0f, -0.65f, 3.6f), sVECTOR3D(0.0f, -0.55f, 2.2f), "models/earthfighter/weapon15.vw3d", "models/earthfighter/sf-text08.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Missiles
	{eGameSFX::WeaponFire_SmallMissile,	false, 30,	4,	5,	200,	3.0f, sVECTOR3D(0.0f, -0.8f, 4.7f), sVECTOR3D(0.0f, -0.9f, 1.5f), "models/earthfighter/lnch1.vw3d", "models/earthfighter/lnch12.tga", ""},
	{eGameSFX::WeaponFire_NormalMissile,	false, 30,	4,	15,	400,	8.0f, sVECTOR3D(0.2f, -0.95f, 2.6f), sVECTOR3D(0.0f, -0.6f, 1.0f), "models/earthfighter/lnch2.vw3d", "models/earthfighter/lnch12.tga", ""},
	{eGameSFX::WeaponFire_Torpedo,		false, 25,	5,	10,	50,	8.0f, sVECTOR3D(0.0f, -0.95f, 4.0f), sVECTOR3D(0.0f, -0.9f, 1.5f), "models/earthfighter/lnch3.vw3d", "models/earthfighter/lnch34.tga", ""},
	{eGameSFX::WeaponFire_Bomb,		false, 30,	5,	15,	25,	10.0f, sVECTOR3D(0.0f, -0.95f, 5.0f), sVECTOR3D(0.0f, -0.9f, 1.8f), "models/earthfighter/lnch4.vw3d", "models/earthfighter/lnch34.tga", ""},
};

// оружие пришельцев 101-199
static std::vector<sWeaponData> PresetAlienWeaponData{
	// оружие пришельцев (как Kinetic1)
	{eGameSFX::WeaponFire_Ion2, true, 1.0f,		1,	1,	5000,	0.7f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (с наведением, как Kinetic2)
	{eGameSFX::WeaponFire_Plasma1, false, 1.0f,	1,	1,	2500,	3.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Kinetic3)
	{eGameSFX::WeaponFire_Plasma2, true, 1.0f,	1,	1,	2500,	3.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (с наведением, как Kinetic3)
	{eGameSFX::WeaponFire_Plasma2, false, 1.0f,	1,	1,	2500,	2.5f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Kinetic2)
	{eGameSFX::WeaponFire_Plasma1, true, 1.0f,	1,	1,	2500,	3.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев для выброса мин (1 типа)
	{eGameSFX::WeaponFire_Antimatter, false, 1.0f,	1,	1,	50,	5.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев для выброса мин (2 типа)
	{eGameSFX::WeaponFire_Antimatter, false, 1.0f,	1,	1,	50,	5.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Plasma3)
	{eGameSFX::WeaponFire_Plasma3, true, 10,	3,	50,	800,	0.9f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Plasma2)
	{eGameSFX::WeaponFire_Plasma2, true, 15,	2,	25,	1000,	0.8f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// оружие пришельцев (как Laser) для больших кораблей
	{eGameSFX::WeaponFire_Laser, true, 15,		5,	150,	800,	1.2f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
};

// оружие пиратов 201-299
static std::vector<sWeaponData> PresetPirateWeaponData{
	// турель для кораблей пиратов
	{eGameSFX::WeaponFire_Kinetic1, false, 10,	1,	1,	3000,	0.7f, sVECTOR3D(0.0f, 1.5f, 1.0f), sVECTOR3D(0.0f, 1.5f, 1.0f), "models/turret/turret-01.vw3d", "models/turret/turrets.tga", ""},
	// турель для кораблей пиратов
	{eGameSFX::WeaponFire_Kinetic2, false, 10,	1,	1,	3000,	0.7f, sVECTOR3D(0.0f, 1.0f, 0.0f), sVECTOR3D(0.0f, 1.0f, 0.0f), "models/turret/turret-02.vw3d", "models/turret/turrets.tga", ""},

	// оружие пиратов - отстрел фларес для больших кораблей
	{eGameSFX::WeaponFire_Kinetic1, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},

	// как Kinetic1
	{eGameSFX::WeaponFire_Kinetic1, true, 10,	1,	1.7f,	3000,	0.7f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile1
	{eGameSFX::WeaponFire_SmallMissile, false, 80,	4,	5,	200,	5.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile2
	{eGameSFX::WeaponFire_NormalMissile, false, 30,	4,	5,	800,	8.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Ion2
	{eGameSFX::WeaponFire_Ion2, true, 10,	1,	10,		2000,	0.9f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Antimatter
	{eGameSFX::WeaponFire_Antimatter, true, 20,	4,	50,	5000,	0.8f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile3 (торпеда)
	{eGameSFX::WeaponFire_Torpedo, false, 25,	5,	10,	200,	8.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Missile4 (бомба)
	{eGameSFX::WeaponFire_Bomb, false, 30,	5,	15,		100,	10.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Kinetic2
	{eGameSFX::WeaponFire_Kinetic2, true, 25,	1,	3,	1500,	0.6f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Kinetic3
	{eGameSFX::WeaponFire_Kinetic3, true, 30,	2,	3.5,	500,	0.7f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// как Plasma2
	{eGameSFX::WeaponFire_Plasma2, true, 15,	2,	25,	1000,	0.8f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},

	// мина1 (простое наведение по высоте)
	{eGameSFX::WeaponFire_Torpedo, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// мина2 (наведение по высоте + приближение)
	{eGameSFX::WeaponFire_Torpedo, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// мина3 (наведение по высоте + стрельба снарядами)
	{eGameSFX::WeaponFire_Bomb, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
	// мина4 (наведение по высоте + стрельба ракетами)
	{eGameSFX::WeaponFire_Bomb, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D(0.0f, 0.0f, 0.0f), sVECTOR3D(0.0f, 0.0f, 0.0f), "none", "none", "none"},
};


//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
static void AttachWeapon(cWeapon *Weapon)
{
	if (Weapon == nullptr)
		return;

	// первый в списке...
	if (EndWeapon == nullptr) {
		Weapon->Prev = nullptr;
		Weapon->Next = nullptr;
		StartWeapon = Weapon;
		EndWeapon = Weapon;
	} else { // продолжаем заполнение...
		Weapon->Prev = EndWeapon;
		Weapon->Next = nullptr;
		EndWeapon->Next = Weapon;
		EndWeapon = Weapon;
	}
}

//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
static void DetachWeapon(cWeapon *Weapon)
{
	if (Weapon == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartWeapon == Weapon) StartWeapon = Weapon->Next;
	if (EndWeapon == Weapon) EndWeapon = Weapon->Prev;


	if (Weapon->Next != nullptr)
		Weapon->Next->Prev = Weapon->Prev;
	else if (Weapon->Prev != nullptr)
		Weapon->Prev->Next = nullptr;
	if (Weapon->Prev != nullptr)
		Weapon->Prev->Next = Weapon->Next;
	else if (Weapon->Next != nullptr)
		Weapon->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllWeapon(float Time)
{
	cWeapon *tmp = StartWeapon;
	while (tmp != nullptr) {
		cWeapon *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllWeapons(bool VertexOnlyPass, unsigned int ShadowMap)
{
	cWeapon *tmp = StartWeapon;
	while (tmp != nullptr) {
		cWeapon *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllWeapons()
{
	cWeapon *tmp = StartWeapon;
	while (tmp != nullptr) {
		cWeapon *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Установка нужных данных для вспышки возле ствола
//-----------------------------------------------------------------------------
static void SetWeaponFire(std::shared_ptr<cParticleSystem> &ParticleSystem, int WeaponNum)
{
	switch (WeaponNum) {
	case 1:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
		break;
	case 2:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 40.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
		break;
	case 3:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.60f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 50.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
		break;
	case 4:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
		break;
	case 5:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 4.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 10.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 6:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.20f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 20.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 7:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.20f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 6.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 10.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 8:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.2f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.2f;
		ParticleSystem->Speed      = 4.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->CreationType       = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize       = sVECTOR3D(0.4f,0.4f,0.4f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 9:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.15f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.15f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->CreationType       = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.3f,0.3f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 10:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.30f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->CreationType       = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.3f,0.3f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;



	case 11:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.60f;
		ParticleSystem->SizeStart  = 0.1f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType       = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.2f,0.3f);
		break;
	case 12:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.60f;
		ParticleSystem->SizeStart  = 0.1f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType       = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.2f,0.3f);
		break;
	case 13:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.7f, 1.0f, 0.0f, 0.0f, 0.02f);
		break;
	case 14:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.10f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->CreationType       = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize       = sVECTOR3D(0.2f,0.2f,0.8f);
		break;
	case 15:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.0f, 0.0f, 0.02f);
		break;
	case 16:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;
	case 17:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;
	case 18:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;
	case 19:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong WeaponNum.\n";
		break;
	}

	ParticleSystem->IsSuppressed = true;
	ParticleSystem->ParticlesPerSec = 50;
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");

}

//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cWeapon::cWeapon(int WeaponNum)
{
	ObjectStatus = eObjectStatus::Ally;
	ObjectType = eObjectType::ShipWeapon;

	// никогда не показываем линию
	ShowStrength = false;

	// вес
	Weight = 500;

	AttachWeapon(this);

	if (WeaponNum <= 0) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 1 && WeaponNum <= 99) && ((unsigned int)WeaponNum > PresetEarthWeaponData.size())) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(1) object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 101 && WeaponNum <= 199) && ((unsigned int)WeaponNum-100 > PresetAlienWeaponData.size())) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(2) object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 201 && WeaponNum <= 299) && ((unsigned int)WeaponNum-200 > PresetPirateWeaponData.size())) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(3) object with Number " << WeaponNum << "\n";
		return;
	}

	InternalType = WeaponNum;


	// 1-99 - это оружие землян
	if (WeaponNum >= 1 && WeaponNum <= 99) {
		ObjectStatus = eObjectStatus::Ally;

		Strength = StrengthStart = PresetEarthWeaponData[WeaponNum-1].Strength;
		WeaponLevel = PresetEarthWeaponData[WeaponNum-1].WeaponLevel;
		Ammo = AmmoStart =  PresetEarthWeaponData[WeaponNum-1].Ammo;
		NextFireTime =  PresetEarthWeaponData[WeaponNum-1].NextFireTime;
		SFX = PresetEarthWeaponData[WeaponNum-1].SFX;
		NeedRotateOnTargeting = PresetEarthWeaponData[WeaponNum-1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetEarthWeaponData[WeaponNum-1].EnergyUse;
		FireLocation = PresetEarthWeaponData[WeaponNum-1].FireLocation;

		DestroyedFireLocation = PresetEarthWeaponData[WeaponNum-1].DestrFireLocation;
		Fire = vw_CreateParticleSystem();
		if (auto sharedFire = Fire.lock()) {
			sharedFire->SetStartLocation(FireLocation);
			sharedFire->Direction = sVECTOR3D(0.0f, 0.0f, 1.0f);
			SetWeaponFire(sharedFire, WeaponNum);
		}

		LoadObjectData(PresetEarthWeaponData[WeaponNum-1].NameVW3D, this);

		for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
			Texture[i] = GetPreloadedTextureAsset(PresetEarthWeaponData[WeaponNum-1].TextureName);
			if (WeaponNum < 16)
				TextureIllum[i] = GetPreloadedTextureAsset(PresetEarthWeaponData[WeaponNum-1].TextureIllumName);
		}

	}
	// 101-199 - это оружие пришельцев
	else if (WeaponNum >= 101 && WeaponNum <= 199) {
		// внутренний номер
		int IntWeaponNum = WeaponNum - 100;

		ObjectStatus = eObjectStatus::Enemy;

		Strength = StrengthStart = PresetAlienWeaponData[IntWeaponNum-1].Strength;
		WeaponLevel = PresetAlienWeaponData[IntWeaponNum-1].WeaponLevel;
		Ammo = AmmoStart =  PresetAlienWeaponData[IntWeaponNum-1].Ammo;
		NextFireTime =  PresetAlienWeaponData[IntWeaponNum-1].NextFireTime;
		SFX = PresetAlienWeaponData[IntWeaponNum-1].SFX;
		NeedRotateOnTargeting = PresetAlienWeaponData[IntWeaponNum-1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetAlienWeaponData[IntWeaponNum-1].EnergyUse;
		FireLocation = PresetAlienWeaponData[IntWeaponNum-1].FireLocation;

	}
	// 201-299 - это оружие пиратов
	else if (WeaponNum >= 201 && WeaponNum <= 299) {
		// внутренний номер
		int IntWeaponNum = WeaponNum - 200;

		ObjectStatus = eObjectStatus::Enemy;

		Strength = StrengthStart = PresetPirateWeaponData[IntWeaponNum-1].Strength;
		WeaponLevel = PresetPirateWeaponData[IntWeaponNum-1].WeaponLevel;
		Ammo = AmmoStart =  PresetPirateWeaponData[IntWeaponNum-1].Ammo;
		NextFireTime =  PresetPirateWeaponData[IntWeaponNum-1].NextFireTime;
		SFX = PresetPirateWeaponData[IntWeaponNum-1].SFX;
		NeedRotateOnTargeting = PresetPirateWeaponData[IntWeaponNum-1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetPirateWeaponData[IntWeaponNum-1].EnergyUse;
		FireLocation = PresetPirateWeaponData[IntWeaponNum-1].FireLocation;

		// турели пиратов
		switch (WeaponNum) {
		case 201: {
			WeaponTurret = true;
			TargetHorizBlocks = 0;
			TargetVertBlocks = 1;
			TargetVertBlocksMaxAngle = 89.0f;
			TargetVertBlocksMinAngle = 0.0f;

			LoadObjectData(PresetPirateWeaponData[IntWeaponNum-1].NameVW3D, this);
			for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
				Texture[i] = GetPreloadedTextureAsset(PresetPirateWeaponData[IntWeaponNum-1].TextureName);
				TextureIllum[i] = 0;
			}
		}
		break;

		case 202: {
			WeaponTurret = true;
			TargetHorizBlocks = 0;
			TargetVertBlocks = 1;
			TargetVertBlocksMaxAngle = 89.0f;
			TargetVertBlocksMinAngle = 20.0f;

			LoadObjectData(PresetPirateWeaponData[IntWeaponNum-1].NameVW3D, this);
			for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
				Texture[i] = GetPreloadedTextureAsset(PresetPirateWeaponData[IntWeaponNum-1].TextureName);
				TextureIllum[i] = 0;
			}
		}
		break;
		}

		if (WeaponNum == 201 || WeaponNum == 202) {
			// вычисляем данные для нахождения точки стрельбы
			if (TargetHorizBlocks != -1) {
				BaseBound = Model3DBlocks[TargetHorizBlocks].Location;
			}

			if (TargetVertBlocks != -1) {
				if (TargetHorizBlocks != -1)
					MiddleBound = Model3DBlocks[TargetVertBlocks].Location -
						      Model3DBlocks[TargetHorizBlocks].Location;
				else
					MiddleBound = Model3DBlocks[TargetVertBlocks].Location;
			}

			if (TargetVertBlocks != -1)
				WeaponBound = FireLocation - Model3DBlocks[TargetVertBlocks].Location;
			else if (TargetHorizBlocks != -1)
				WeaponBound = FireLocation - Model3DBlocks[TargetHorizBlocks].Location;
			else
				WeaponBound = FireLocation;
		}
	}
}


//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cWeapon::~cWeapon()
{
	if (auto sharedFire = Fire.lock()) {
		sharedFire->IsSuppressed = true;
		sharedFire->DestroyIfNoParticles = true;
	}
	if (auto sharedDestroyedFire = DestroyedFire.lock()) {
		sharedDestroyedFire->IsSuppressed = true;
		sharedDestroyedFire->DestroyIfNoParticles = true;
	}
	if (auto sharedDestroyedSmoke = DestroyedSmoke.lock()) {
		sharedDestroyedSmoke->IsSuppressed = true;
		sharedDestroyedSmoke->DestroyIfNoParticles = true;
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
	if (!cObject3D::Update(Time)) {
		return false;
	}

	// если запущено - выключаем прорисовку эффекта выстрела (вспышка возле ствола)
	if (LastFireTime + TimeDelta <= Time)
		if (auto sharedFire = Fire.lock())
			sharedFire->IsSuppressed = true;

	// для землян, создание эффекта повреждения оружия
	// если оружие только повредили...
	if ((InternalType >= 1) &&
	    (InternalType <= 99) &&
	    (Strength < StrengthStart) &&
	    DestroyedFire.expired()) {
		// горение
		DestroyedFire = vw_CreateParticleSystem();
		if (auto sharedDestroyedFire = DestroyedFire.lock()) {
			sharedDestroyedFire->ColorStart.r = 1.00f;
			sharedDestroyedFire->ColorStart.g = 0.70f;
			sharedDestroyedFire->ColorStart.b = 0.30f;
			sharedDestroyedFire->ColorEnd.r = 1.00f;
			sharedDestroyedFire->ColorEnd.g = 0.00f;
			sharedDestroyedFire->ColorEnd.b = 0.00f;
			sharedDestroyedFire->AlphaStart = 1.00f;
			sharedDestroyedFire->AlphaEnd   = 0.10f;
			sharedDestroyedFire->SizeStart  = 0.20f;
			sharedDestroyedFire->SizeVar    = 0.10f;
			sharedDestroyedFire->SizeEnd    = 0.10f;
			sharedDestroyedFire->Speed      = 8.00f;
			sharedDestroyedFire->SpeedVar   = 2.00f;
			sharedDestroyedFire->Theta      = 5.00f;
			sharedDestroyedFire->Life       = 0.50f*Length/3.0f;
			sharedDestroyedFire->ParticlesPerSec = 70;
			sharedDestroyedFire->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
			sharedDestroyedFire->CreationType = eParticleCreationType::Cube;
			sharedDestroyedFire->CreationSize = sVECTOR3D(Width/2.0f,Width/2.0f,0.1f);
			sharedDestroyedFire->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);
			sharedDestroyedFire->SetStartLocation(DestroyedFireLocation);
		}
	}

	// для землян, создание эффекта уничтоженного оружия
	// проверяем тут был ли выход из строя
	if ((InternalType >= 1) &&
	    (InternalType <= 99) &&
	    (Strength <= 0.0f) &&
	    DestroyedSmoke.expired()) {
		// дым
		DestroyedSmoke = vw_CreateParticleSystem();
		if (auto sharedDestroyedSmoke = DestroyedSmoke.lock()) {
			sharedDestroyedSmoke->ColorStart.r = 1.00f;
			sharedDestroyedSmoke->ColorStart.g = 1.00f;
			sharedDestroyedSmoke->ColorStart.b = 1.00f;
			sharedDestroyedSmoke->ColorEnd.r = 1.00f;
			sharedDestroyedSmoke->ColorEnd.g = 1.00f;
			sharedDestroyedSmoke->ColorEnd.b = 1.00f;
			sharedDestroyedSmoke->AlphaStart = 0.20f;
			sharedDestroyedSmoke->AlphaEnd   = 0.00f;
			sharedDestroyedSmoke->SizeStart  = 0.25f;
			sharedDestroyedSmoke->SizeVar    = 0.10f;
			sharedDestroyedSmoke->SizeEnd    = 0.00f;
			sharedDestroyedSmoke->Speed      = 0.00f;
			sharedDestroyedSmoke->SpeedVar   = 0.00f;
			sharedDestroyedSmoke->Theta      = 35.00f;
			sharedDestroyedSmoke->Life       = 2.00f*Length/3.0f;
			sharedDestroyedSmoke->ParticlesPerSec = 300;
			sharedDestroyedSmoke->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
			sharedDestroyedSmoke->CreationType = eParticleCreationType::Point;
			sharedDestroyedSmoke->CreationSize = sVECTOR3D(Width/2.5f,Width/2.5f,0.1f);
			sharedDestroyedSmoke->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);
			sharedDestroyedSmoke->SetStartLocation(DestroyedFireLocation);
		}
	}

	// если это swamp - запуск других ракет
	if (InternalType == 17 && SwampNum > 0)
		if (LastFireTime + 0.15f < Time)
			if (Ammo > 0 || !GameUnlimitedAmmo) {
				LastFireTime = Time;

				// ум. кол-во боеприпасов, если нужно
				if (!GameUnlimitedAmmo)
					Ammo -= 1;

				float CurrentPenalty{1.0f};
				if (ObjectStatus == eObjectStatus::Enemy)
					CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty);

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
				Projectile->Create(InternalType);
				Projectile->SetLocation(Location+FireLocation);
				Projectile->SetRotation(Rotation);
				for (auto tmpGFX : Projectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						if (auto sharedFire = Fire.lock())
							sharedGFX->Direction = sharedFire->Direction ^ -1;
						// учитываем пенальти для визуальных эффектов
						if (CurrentPenalty == 2)
							sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.33f);
						if (CurrentPenalty == 3)
							sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.5f);
						sharedGFX->Speed = sharedGFX->Speed / CurrentPenalty;
						sharedGFX->Life = sharedGFX->Life * CurrentPenalty;
						sharedGFX->MagnetFactor = sharedGFX->MagnetFactor / (CurrentPenalty * CurrentPenalty);
					}
				}
				Projectile->ObjectStatus = ObjectStatus;
				// учитываем пенальти для снаряда
				Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
				Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
				Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
				Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;

				// звук...
				if (SFX != eGameSFX::none) {
					float fVol = 1.0f;
					PlayGameSFX(SFX, fVol, Projectile->Location);
				}

				SwampNum--;
			}

	// если это фларес - выпускаем остальные
	// SwampNum в этом случае используем с другой целью
	if (InternalType == 203 && SwampNum > 0)
		if (LastFireTime + 0.4f < Time)
			if (Ammo > 0 || !GameUnlimitedAmmo) {
				LastFireTime = Time;

				// ум. кол-во боеприпасов, если нужно
				if (!GameUnlimitedAmmo)
					Ammo -= 1;

				// общий - пенальти, если не игрок
				float CurrentPenalty = 1.0f;


				// создаем снаряд
				cProjectile *Projectile = new cProjectile;
				Projectile->Create(InternalType);
				Projectile->SetLocation(Location + FireLocation);
				Projectile->SetRotation(Rotation + sVECTOR3D(vw_fRand0() * 30.0f, 0.0f, vw_fRand0() * 30.0f));

				for (auto tmpGFX : Projectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = Orientation ^ -1;
						// учитываем пенальти для визуальных эффектов
						if (CurrentPenalty == 2)
							sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.33f);
						if (CurrentPenalty == 3)
							sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.5f);
						sharedGFX->Speed = sharedGFX->Speed / CurrentPenalty;
						sharedGFX->Life = sharedGFX->Life * CurrentPenalty;
						sharedGFX->MagnetFactor = sharedGFX->MagnetFactor / (CurrentPenalty * CurrentPenalty);
					}
				}
				Projectile->ObjectStatus = ObjectStatus;
				// учитываем пенальти для снаряда
				Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
				Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
				Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
				Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;


				// звук...
				if (SFX != eGameSFX::none) {
					float fVol = 1.0f;
					PlayGameSFX(SFX, fVol, Projectile->Location);
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
			sVECTOR3D NeedAngle(TargetVertBlocksNeedAngle,TargetHorizBlocksNeedAngle,0);
			GetTurretOnTargetOrientateion(ObjectStatus, Location+FireLocation, Rotation,
						      CurrentRotationMat, &NeedAngle, InternalType);

			// наводим на цель
			TargetHorizBlocksNeedAngle = NeedAngle.y;
			TargetVertBlocksNeedAngle = NeedAngle.x;
		}
		/*else
		{
			// устанавливаем в начальное положение
			TargetHorizBlocksNeedAngle = 0.0f;
			TargetVertBlocksNeedAngle = 0.0f;
		}*/

		// собственно повороты элементов модели на углы

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот башни по горизонтале
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (TargetHorizBlocks != -1)
			if (TargetHorizBlocksNeedAngle != TargetHorizBlocksCurrentAngle) {
				if (fabsf(TargetHorizBlocksNeedAngle-TargetHorizBlocksCurrentAngle) > 180.0f) {
					if (TargetHorizBlocksCurrentAngle - TargetHorizBlocksNeedAngle > 180.0f) TargetHorizBlocksCurrentAngle -= 360.0f;
					if (TargetHorizBlocksNeedAngle - TargetHorizBlocksCurrentAngle > 180.0f) TargetHorizBlocksCurrentAngle += 360.0f;
				}

				// находим угол, на который нужно повернуть
				float NeedRotate = TargetHorizBlocksCurrentAngle;

				if (TargetHorizBlocksNeedAngle>TargetHorizBlocksCurrentAngle) {
					NeedRotate += 80.0f*TimeDelta/GameEnemyTargetingSpeedPenalty;
					if (NeedRotate > TargetHorizBlocksNeedAngle) NeedRotate = TargetHorizBlocksNeedAngle;
				} else {
					NeedRotate -= 80.0f*TimeDelta/GameEnemyTargetingSpeedPenalty;
					if (NeedRotate < TargetHorizBlocksNeedAngle) NeedRotate = TargetHorizBlocksNeedAngle;
				}

				// устанавливаем текущий поворот
				TargetHorizBlocksCurrentAngle = NeedRotate;

				// поворачиваем все объекты
				for (auto &tmpModel3DBlock : Model3DBlocks) {
					sVECTOR3D tmp = tmpModel3DBlock.Location - Model3DBlocks[TargetHorizBlocks].Location;

					vw_RotatePointInv(tmp, tmpModel3DBlock.Rotation ^ (-1.0f));

					tmpModel3DBlock.Rotation.y = -NeedRotate;

					vw_RotatePoint(tmp, tmpModel3DBlock.Rotation);

					tmpModel3DBlock.Location = tmp + Model3DBlocks[TargetHorizBlocks].Location;
				}
			}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот стволов по вертикали
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (TargetVertBlocks != -1)
			if (TargetVertBlocksNeedAngle != TargetVertBlocksCurrentAngle) {

				// находим угол, на который нужно повернуть
				float NeedRotate = TargetVertBlocksCurrentAngle;
				if (TargetVertBlocksNeedAngle>TargetVertBlocksCurrentAngle) {
					NeedRotate += 80.0f*TimeDelta/GameEnemyTargetingSpeedPenalty;
					if (NeedRotate > TargetVertBlocksNeedAngle) NeedRotate = TargetVertBlocksNeedAngle;
					if (NeedRotate > TargetVertBlocksMaxAngle) NeedRotate = TargetVertBlocksMaxAngle;
				} else {
					NeedRotate -= 80.0f*TimeDelta/GameEnemyTargetingSpeedPenalty;
					if (NeedRotate < TargetVertBlocksNeedAngle) NeedRotate = TargetVertBlocksNeedAngle;
					if (NeedRotate < TargetVertBlocksMinAngle) NeedRotate = TargetVertBlocksMinAngle;
				}

				// устанавливаем текущий поворот
				TargetVertBlocksCurrentAngle = NeedRotate;

				// поворачиваем все объекты
				sVECTOR3D tmp = Model3DBlocks[TargetVertBlocks].Location - Model3DBlocks[TargetVertBlocks].Location;

				vw_RotatePointInv(tmp, Model3DBlocks[TargetVertBlocks].Rotation ^ (-1.0f));

				Model3DBlocks[TargetVertBlocks].Rotation.x = -NeedRotate;

				vw_RotatePoint(tmp, Model3DBlocks[TargetVertBlocks].Rotation);

				Model3DBlocks[TargetVertBlocks].Location = tmp + Model3DBlocks[TargetVertBlocks].Location;
			}

		// турель
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizBlocks != -1)
			RotationMiddle = Model3DBlocks[TargetHorizBlocks].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		sVECTOR3D RotationWeapon = Rotation;
		if (TargetVertBlocks != -1)
			RotationWeapon = Model3DBlocks[TargetVertBlocks].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;


		// особый случай, испускаем без вращающихся частей (антиматерия, ион)
		if ((TargetHorizBlocks == -1) && (TargetVertBlocks == -1))
			RotationWeapon = sVECTOR3D(TargetVertBlocksNeedAngle, TargetHorizBlocksNeedAngle, 0.0f)+Rotation;

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
	cObject3D::SetRotation(NewRotation);

	if (!WeaponTurret) {
		// положение точки выстрела
		vw_Matrix33CalcPoint(FireLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(FireLocation, CurrentRotationMat);

		if (auto sharedFire = Fire.lock()) {
			sharedFire->MoveSystem(Location + FireLocation);
			sharedFire->SetStartLocation(Location + FireLocation);
			sharedFire->RotateSystemByAngle(Rotation);
		}
	} else {
		// турель
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizBlocks != -1)
			RotationMiddle = Model3DBlocks[TargetHorizBlocks].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		sVECTOR3D RotationWeapon = Rotation;
		if (TargetVertBlocks != -1)
			RotationWeapon = Model3DBlocks[TargetVertBlocks].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

		// особый случай, испускаем без вращающихся частей (антиматерия, ион)
		if ((TargetHorizBlocks == -1) && (TargetVertBlocks == -1))
			RotationWeapon = sVECTOR3D(TargetVertBlocksNeedAngle, TargetHorizBlocksNeedAngle, 0.0f)+Rotation;

		Orientation = sVECTOR3D(0.0f, 0.0f, 1.0f);
		vw_RotatePoint(Orientation, RotationWeapon);
	}


	if (auto sharedDestroyedFire = DestroyedFire.lock()) {
		vw_Matrix33CalcPoint(DestroyedFireLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(DestroyedFireLocation, CurrentRotationMat);
		sharedDestroyedFire->MoveSystem(Location + DestroyedFireLocation);
		sharedDestroyedFire->SetStartLocation(Location + DestroyedFireLocation);
		sharedDestroyedFire->RotateSystemByAngle(Rotation);
	}
	// тут DestroyedFireLocation не считаем, т.к. все равно всегда создаем DestroyedFire
	if (auto sharedDestroyedSmoke = DestroyedSmoke.lock()) {
		sharedDestroyedSmoke->MoveSystemLocation(Location + DestroyedFireLocation);
		sharedDestroyedSmoke->RotateSystemByAngle(Rotation);
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
	cObject3D::SetLocation(NewLocation);

	// положение утечки
	if (auto sharedFire = Fire.lock()) {
		sharedFire->MoveSystem(NewLocation + FireLocation);
		sharedFire->SetStartLocation(NewLocation + FireLocation);
	}
	if (auto sharedDestroyedFire = DestroyedFire.lock()) {
		sharedDestroyedFire->MoveSystem(NewLocation + DestroyedFireLocation);
		sharedDestroyedFire->SetStartLocation(NewLocation + DestroyedFireLocation);
	}
	if (auto sharedDestroyedSmoke = DestroyedSmoke.lock())
		sharedDestroyedSmoke->MoveSystemLocation(NewLocation + DestroyedFireLocation);

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
	if (InternalType == 0)
		return false;


	float CurrentPenalty{1.0f};
	if (ObjectStatus == eObjectStatus::Enemy)
		CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty);


	// если фларес - тоже ничего не надо
	if (InternalType == 203) CurrentPenalty = 1.0f;


	// проверяем по времени, можем стрелять или нет
	// делаем ее тут, чтобы "правильно" озвучивать проблемы стрельбы
	if (Time < LastFireTime + NextFireTime*CurrentPenalty) return false;
	// запоминаем время стрельбы
	LastFireTime = Time;


	// для оружия землян, делаем небольшие проверки...
	if (InternalType >= 1 && InternalType <= 99) {

		// для поврежденного оружия проверяем... может быть осечка в стрельбе
		bool Misfire = false;
		if (Strength < StrengthStart)
			if (Strength/StrengthStart > vw_fRand()) {
				Misfire = true;
			}


		// если оружие уничтожено или уже нечем стрелять - нам тут делать нечего
		if (Strength <= 0.0f || Ammo == 0 || Misfire) {
			// перед тем как уйти, ставим проигрывание звука "проблема с оружием"

			float fVol = 1.0f;

			switch (InternalType) {
			// Kinetic
			case 1:
			case 2:
			case 3:
			case 4:
				PlayGameSFX(eGameSFX::WeaponMalfunction_Kinetic, fVol, Location);
				break;

			// Ion
			case 5:
			case 6:
			case 7:
			// Plasma
			case 8:
			case 9:
			case 10:
				PlayGameSFX(eGameSFX::WeaponMalfunction_Particle, fVol, Location);
				break;

			// Maser
			case 11:
			case 12:
			// Laser
			case 14:
				PlayGameSFX(eGameSFX::WeaponMalfunction_Beam, fVol, Location);

				break;
			// Antimatter
			case 13:
			// Gauss
			case 15:
				PlayGameSFX(eGameSFX::WeaponMalfunction_Energy, fVol, Location);
				break;

			// ракета
			case 16:
			// рой
			case 17:
			// торпеда
			case 18:
			// бомба
			case 19:
				PlayGameSFX(eGameSFX::WeaponMalfunction_Launcher, fVol, Location);
				break;
			}

			return false;
		}


		// только для игрока! проверяем заряд энергии для выстрела
		if (ObjectStatus == eObjectStatus::Player) {
			if (CurrentEnergyAccumulated < EnergyUse)
				return false;
			else
				CurrentEnergyAccumulated = 0.0f;
		}

		// ум. кол-во боеприпасов, если нужно
		if (!GameUnlimitedAmmo)
			Ammo -= 1;


		// делаем вспышку возле ствола для всего оружия землят (только землян), если это не ракетная установка
		if (InternalType < 16)
			if (auto sharedFire = Fire.lock())
				sharedFire->IsSuppressed = false;

	}


	sVECTOR3D RotationWeapon = Rotation;
	if (WeaponTurret) {
		// турель
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizBlocks != -1)
			RotationMiddle = Model3DBlocks[TargetHorizBlocks].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		if (TargetVertBlocks != -1)
			RotationWeapon = Model3DBlocks[TargetVertBlocks].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;


		// особый случай, испускаем без вращающихся частей (антиматерия, ион)
		if (TargetHorizBlocks == -1 && TargetVertBlocks == -1) {
			RotationWeapon = sVECTOR3D(TargetVertBlocksNeedAngle, TargetHorizBlocksNeedAngle, 0.0f)+Rotation;
		}

	}


	// создаем снаряд
	cProjectile *Projectile = new cProjectile;
	Projectile->Create(InternalType);


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
		for (auto tmpGFX : Projectile->GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock())
				sharedGFX->Direction = Orientation;
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
		for (auto tmpGFX : Projectile->GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				sharedGFX->Direction = Orientation;
				// учитываем пенальти для визуальных эффектов
				sharedGFX->ParticlesPerSec = (int)(sharedGFX->ParticlesPerSec / CurrentPenalty);

				sharedGFX->Speed = sharedGFX->Speed / CurrentPenalty;
				sharedGFX->Life = sharedGFX->Life * CurrentPenalty;
				sharedGFX->MagnetFactor = sharedGFX->MagnetFactor / (CurrentPenalty * CurrentPenalty);
			}
		}
		Projectile->ObjectStatus = ObjectStatus;
		// учитываем пенальти для снаряда
		Projectile->SpeedStart = Projectile->SpeedEnd = Projectile->SpeedStart/CurrentPenalty;
		Projectile->Age = Projectile->Lifetime = Projectile->Age*CurrentPenalty;
		Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
		Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;
	}


	// звук...
	if (SFX != eGameSFX::none) {
		float fVol = 1.0f;
		LaserMaserSoundNum = PlayGameSFX(SFX, fVol, Location+FireLocation);
		// если не надо сохранять
		if (LaserMaser == nullptr)
			LaserMaserSoundNum = 0;
	}


	// если это груповой выстрел:

	// нужно создать еще 9 ракет
	if (InternalType == 17)
		SwampNum = 9;

	// нужно создать еще 4 фларес
	if (InternalType == 203)
		SwampNum = 4;


	// выстрел был
	return true;
}

} // astromenace namespace
} // viewizard namespace
