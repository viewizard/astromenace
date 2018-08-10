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

// TODO switch to color preset in SetWeaponFire()

// TODO switch to enumeration SetWeaponFire()

// TODO codestyle should be fixed

#include "weapon.h"
#include "../projectile/projectile.h"
#include "../../assets/audio.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

std::list<std::shared_ptr<cWeapon>> WeaponList{};

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
	std::string NameVW3D;
	std::string TextureName;
	std::string TextureIllumName;
};

// earth 1-99
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::vector<sWeaponData> PresetEarthWeaponData{
	// Kinetic
	{eGameSFX::WeaponFire_Kinetic1,		true, 25,	1,	1.5f,	3000,	0.4f, sVECTOR3D{0.0f, -0.613f, 2.0f}, sVECTOR3D{0.0f, -0.65f, 1.0f}, "models/earthfighter/weapon5.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{eGameSFX::WeaponFire_Kinetic2,		true, 25,	1,	3,	1500,	0.6f, sVECTOR3D{0.0f, -0.613f, 2.93f}, sVECTOR3D{0.0f, -0.6f, 1.8f}, "models/earthfighter/weapon11.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{eGameSFX::WeaponFire_Kinetic3,		true, 30,	2,	3.5,	1000,	0.7f, sVECTOR3D{0.0f, -0.613f, 3.33f}, sVECTOR3D{0.0f, -0.6f, 2.2f}, "models/earthfighter/weapon13.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	{eGameSFX::WeaponFire_Kinetic4,		true, 30,	2,	5,	7000,	0.3f, sVECTOR3D{0.0f, -0.613f, 2.33f}, sVECTOR3D{0.0f, -0.6f, 1.2f}, "models/earthfighter/weapon8.vw3d", "models/earthfighter/sf-text00.vw2d", "models/earthfighter/sf-illum01.vw2d"},
	// Ion
	{eGameSFX::WeaponFire_Ion1,		true, 10,	1,	7,	1000,	0.7f, sVECTOR3D{0.0f, -0.43f, 2.13f}, sVECTOR3D{0.0f, -0.45f, 1.5f}, "models/earthfighter/weapon1.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	{eGameSFX::WeaponFire_Ion2,		true, 10,	1,	10,	2000,	0.9f, sVECTOR3D{0.0f, -0.53f, 1.86f}, sVECTOR3D{0.0f, -0.5f, 1.8f}, "models/earthfighter/weapon4.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	{eGameSFX::WeaponFire_Ion3,		true, 15,	2,	11.5,	3000,	1.0f, sVECTOR3D{0.0f, -0.63f, 2.26f}, sVECTOR3D{0.0f, -0.65f, 2.3f}, "models/earthfighter/weapon6.vw3d", "models/earthfighter/sf-text04.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	// Plasma
	{eGameSFX::WeaponFire_Plasma1,		true, 10,	2,	20,	2000,	0.6f, sVECTOR3D{0.0f, -0.613f, 2.0f}, sVECTOR3D{0.0f, -0.65f, 1.8f}, "models/earthfighter/weapon3.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{eGameSFX::WeaponFire_Plasma2,		true, 15,	2,	30,	1000,	0.7f, sVECTOR3D{0.0f, -0.8f, 1.86f}, sVECTOR3D{0.0f, -0.8f, 1.8f}, "models/earthfighter/weapon2.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{eGameSFX::WeaponFire_Plasma3,		true, 10,	3,	50,	800,	0.9f, sVECTOR3D{0.0f, -0.613f, 1.2f}, sVECTOR3D{0.0f, -0.7f, 1.6f}, "models/earthfighter/weapon7.vw3d", "models/earthfighter/sf-text06.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Maser
	{eGameSFX::WeaponFire_Maser1,		true, 10,	3,	50,	800,	3.0f, sVECTOR3D{0.0f, -0.55f, 2.1f}, sVECTOR3D{0.0f, -0.55f, 1.4f}, "models/earthfighter/weapon10.vw3d", "models/earthfighter/sf-text07.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	{eGameSFX::WeaponFire_Maser2,		true, 15,	4,	80,	1000,	2.4f, sVECTOR3D{0.0f, -0.55f, 2.5f}, sVECTOR3D{0.0f, -0.55f, 1.8f}, "models/earthfighter/weapon9.vw3d", "models/earthfighter/sf-text07.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Antimatter
	{eGameSFX::WeaponFire_Antimatter,	true, 20,	4,	50,	5000,	0.8f, sVECTOR3D{0.0f, -0.65f, 2.1f}, sVECTOR3D{0.0f, -0.65f, 1.9f}, "models/earthfighter/weapon12.vw3d", "models/earthfighter/sf-text09.vw2d", "models/earthfighter/sf-illum02.vw2d"},
	// Laser
	{eGameSFX::WeaponFire_Laser,		true, 15,	5,	150,	800,	1.2f, sVECTOR3D{0.0f, -0.6f, 2.5f}, sVECTOR3D{0.0f, -0.6f, 2.1f}, "models/earthfighter/weapon14.vw3d", "models/earthfighter/sf-text05.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Gauss
	{eGameSFX::WeaponFire_Gauss,		true, 20,	5,	150,	2000,	0.7f, sVECTOR3D{0.0f, -0.65f, 3.6f}, sVECTOR3D{0.0f, -0.55f, 2.2f}, "models/earthfighter/weapon15.vw3d", "models/earthfighter/sf-text08.vw2d", "models/earthfighter/sf-illum03.vw2d"},
	// Missiles
	{eGameSFX::WeaponFire_SmallMissile,	false, 30,	4,	5,	200,	3.0f, sVECTOR3D{0.0f, -0.8f, 4.7f}, sVECTOR3D{0.0f, -0.9f, 1.5f}, "models/earthfighter/lnch1.vw3d", "models/earthfighter/lnch12.tga", ""},
	{eGameSFX::WeaponFire_NormalMissile,	false, 30,	4,	15,	400,	8.0f, sVECTOR3D{0.2f, -0.95f, 2.6f}, sVECTOR3D{0.0f, -0.6f, 1.0f}, "models/earthfighter/lnch2.vw3d", "models/earthfighter/lnch12.tga", ""},
	{eGameSFX::WeaponFire_Torpedo,		false, 25,	5,	10,	50,	8.0f, sVECTOR3D{0.0f, -0.95f, 4.0f}, sVECTOR3D{0.0f, -0.9f, 1.5f}, "models/earthfighter/lnch3.vw3d", "models/earthfighter/lnch34.tga", ""},
	{eGameSFX::WeaponFire_Bomb,		false, 30,	5,	15,	25,	10.0f, sVECTOR3D{0.0f, -0.95f, 5.0f}, sVECTOR3D{0.0f, -0.9f, 1.8f}, "models/earthfighter/lnch4.vw3d", "models/earthfighter/lnch34.tga", ""}
};
#pragma GCC diagnostic pop

// alien 101-199
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::vector<sWeaponData> PresetAlienWeaponData{
	// like Kinetic1
	{eGameSFX::WeaponFire_Ion2, true, 1.0f,		1,	1,	5000,	0.7f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// homing, like Kinetic2
	{eGameSFX::WeaponFire_Plasma1, false, 1.0f,	1,	1,	2500,	3.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Kinetic3
	{eGameSFX::WeaponFire_Plasma2, true, 1.0f,	1,	1,	2500,	3.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// homing, like Kinetic3
	{eGameSFX::WeaponFire_Plasma2, false, 1.0f,	1,	1,	2500,	2.5f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Kinetic2
	{eGameSFX::WeaponFire_Plasma1, true, 1.0f,	1,	1,	2500,	3.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// emergy mine 1
	{eGameSFX::WeaponFire_Antimatter, false, 1.0f,	1,	1,	50,	5.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// emergy mine 2, with homing
	{eGameSFX::WeaponFire_Antimatter, false, 1.0f,	1,	1,	50,	5.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Plasma3
	{eGameSFX::WeaponFire_Plasma3, true, 10,	3,	50,	800,	0.9f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Plasma2
	{eGameSFX::WeaponFire_Plasma2, true, 15,	2,	25,	1000,	0.8f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Laser (for matherships)
	{eGameSFX::WeaponFire_Laser, true, 15,		5,	150,	800,	1.2f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""}
};
#pragma GCC diagnostic pop

// pirate 201-299
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::vector<sWeaponData> PresetPirateWeaponData{
	// turrent 1
	{eGameSFX::WeaponFire_Kinetic1, false, 10,	1,	1,	3000,	0.7f, sVECTOR3D{0.0f, 1.5f, 1.0f}, sVECTOR3D{0.0f, 1.5f, 1.0f}, "models/turret/turret-01.vw3d", "models/turret/turrets.tga", ""},
	// turrent 2
	{eGameSFX::WeaponFire_Kinetic2, false, 10,	1,	1,	3000,	0.7f, sVECTOR3D{0.0f, 1.0f, 0.0f}, sVECTOR3D{0.0f, 1.0f, 0.0f}, "models/turret/turret-02.vw3d", "models/turret/turrets.tga", ""},

	// flares
	{eGameSFX::WeaponFire_Kinetic1, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},

	// like Kinetic1
	{eGameSFX::WeaponFire_Kinetic1, true, 10,	1,	1.7f,	3000,	0.7f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Missile1
	{eGameSFX::WeaponFire_SmallMissile, false, 80,	4,	5,	200,	5.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Missile2
	{eGameSFX::WeaponFire_NormalMissile, false, 30,	4,	5,	800,	8.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Ion2
	{eGameSFX::WeaponFire_Ion2, true, 10,	1,	10,		2000,	0.9f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Antimatter
	{eGameSFX::WeaponFire_Antimatter, true, 20,	4,	50,	5000,	0.8f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Missile3 (torpedo)
	{eGameSFX::WeaponFire_Torpedo, false, 25,	5,	10,	200,	8.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Missile4 (bomb)
	{eGameSFX::WeaponFire_Bomb, false, 30,	5,	15,		100,	10.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Kinetic2
	{eGameSFX::WeaponFire_Kinetic2, true, 25,	1,	3,	1500,	0.6f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Kinetic3
	{eGameSFX::WeaponFire_Kinetic3, true, 30,	2,	3.5,	500,	0.7f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// like Plasma2
	{eGameSFX::WeaponFire_Plasma2, true, 15,	2,	25,	1000,	0.8f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},

	// mine 1
	{eGameSFX::WeaponFire_Torpedo, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// mine 2
	{eGameSFX::WeaponFire_Torpedo, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// mine 31
	{eGameSFX::WeaponFire_Bomb, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""},
	// mine 4
	{eGameSFX::WeaponFire_Bomb, false, 1.0f,	1,	1,	2500,	4.0f, sVECTOR3D{0.0f, 0.0f, 0.0f}, sVECTOR3D{0.0f, 0.0f, 0.0f}, "", "", ""}
};
#pragma GCC diagnostic pop

} // unnamed namespace


/*
 * Create cWeapon object.
 */
std::weak_ptr<cWeapon> CreateWeapon(const int WeaponNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	WeaponList.emplace_front(new cWeapon{WeaponNum}, [](cWeapon *p) {delete p;});
	return WeaponList.front();
}

/*
 * Update and remove (erase) dead objects.
 */
void UpdateAllWeapon(float Time)
{
	for (auto iter = WeaponList.begin(); iter != WeaponList.end();) {
		if (!iter->get()->Update(Time))
			iter = WeaponList.erase(iter);
		else
			++iter;
	}
}

/*
 * Draw all objects.
 */
void DrawAllWeapons(bool VertexOnlyPass, unsigned int ShadowMap)
{
	for (auto &tmpWeapon : WeaponList) {
		tmpWeapon.get()->Draw(VertexOnlyPass, ShadowMap);
	}
}

/*
 * Release particular weapon object.
 */
void ReleaseWeapon(std::weak_ptr<cWeapon> &Object)
{
	auto sharedObject = Object.lock();
	if (!sharedObject)
		return;

	for (auto iter = WeaponList.begin(); iter != WeaponList.end();) {
		if (iter->get() == sharedObject.get()) {
			WeaponList.erase(iter);
			return;
		}
		++iter;
	}
}

/*
 * Release particular weapon object during update cycle.
 */
void ReleaseWeaponLazy(std::weak_ptr<cWeapon> &Object)
{
	auto sharedObject = Object.lock();
	if (!sharedObject)
		return;

	// make sure, that the DeleteAfterLeaveScene is disabled,
	// in order to prevent possible Lifetime counter reset
	sharedObject->DeleteAfterLeaveScene = eDeleteAfterLeaveScene::disabled;
	sharedObject->Lifetime = 0.0f;
}

/*
 * Release all objects.
 */
void ReleaseAllWeapons()
{
	WeaponList.clear();
}

/*
 * Setup fire gfx.
 */
static void SetWeaponFire(std::shared_ptr<cParticleSystem> &ParticleSystem, const int WeaponNum)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
	ParticleSystem->ParticlesPerSec = 50;
	ParticleSystem->IsSuppressed = true;

	switch (WeaponNum) {
	case 1:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
		break;
	case 2:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 2.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 40.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
		break;
	case 3:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.60f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 1.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 50.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
		break;
	case 4:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
		break;
	case 5:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeEnd = 0.00f;
		ParticleSystem->Speed = 4.00f;
		ParticleSystem->SpeedVar = 1.00f;
		ParticleSystem->Theta = 10.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 6:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.20f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeEnd = 0.20f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 1.00f;
		ParticleSystem->Theta = 20.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 7:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.20f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeEnd = 0.20f;
		ParticleSystem->Speed = 6.00f;
		ParticleSystem->SpeedVar = 1.00f;
		ParticleSystem->Theta = 10.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 8:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.2f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.2f;
		ParticleSystem->Speed = 4.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 0.00f;
		ParticleSystem->Life = 0.30f;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.4f, 0.4f, 0.4f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 9:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.15f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.15f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 0.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.3f, 0.3f, 0.3f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 10:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.30f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 0.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.3f, 0.3f, 0.3f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 11:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.60f;
		ParticleSystem->SizeStart = 0.1f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.00f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.30f;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.3f, 0.2f, 0.3f);
		break;
	case 12:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.60f;
		ParticleSystem->SizeStart = 0.1f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.00f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.30f;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.3f, 0.2f, 0.3f);
		break;
	case 13:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.7f, 1.0f, 0.0f, 0.0f, 0.02f);
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
		ParticleSystem->CreationSize       = sVECTOR3D{0.2f, 0.2f, 0.8f};
		break;
	case 15:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar= 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.5f, 0.0f, 0.0f, 0.02f);
		break;
	case 16:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		break;
	case 17:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		break;
	case 18:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		break;
	case 19:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.4f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 3.00f;
		ParticleSystem->SpeedVar = 3.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.40f;
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong WeaponNum.\n";
		break;
	}
}

/*
 * Constructor.
 */
cWeapon::cWeapon(const int WeaponNum)
{
	ObjectStatus = eObjectStatus::Ally;
	ObjectType = eObjectType::ShipWeapon;

	ShowStrength = false;
	Weight = 500;

	if (WeaponNum <= 0) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 1 && WeaponNum <= 99) && ((unsigned int)WeaponNum > PresetEarthWeaponData.size())) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(1) object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 101 && WeaponNum <= 199) && ((unsigned int)WeaponNum - 100 > PresetAlienWeaponData.size())) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(2) object with Number " << WeaponNum << "\n";
		return;
	} else if ((WeaponNum >= 201 && WeaponNum <= 299) && ((unsigned int)WeaponNum - 200 > PresetPirateWeaponData.size())) {
		std::cerr << __func__ << "(): " << "Couldn't init cWeapon(3) object with Number " << WeaponNum << "\n";
		return;
	}

	InternalType = WeaponNum;

	// earth 1-99
	if (WeaponNum >= 1 && WeaponNum <= 99) {
		ObjectStatus = eObjectStatus::Ally;

		Strength = StrengthStart = PresetEarthWeaponData[WeaponNum - 1].Strength;
		WeaponLevel = PresetEarthWeaponData[WeaponNum - 1].WeaponLevel;
		Ammo = AmmoStart =  PresetEarthWeaponData[WeaponNum - 1].Ammo;
		NextFireTime =  PresetEarthWeaponData[WeaponNum - 1].NextFireTime;
		SFX = PresetEarthWeaponData[WeaponNum - 1].SFX;
		NeedRotateOnTargeting = PresetEarthWeaponData[WeaponNum - 1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetEarthWeaponData[WeaponNum - 1].EnergyUse;
		FireLocation = PresetEarthWeaponData[WeaponNum - 1].FireLocation;

		DestroyedFireLocation = PresetEarthWeaponData[WeaponNum - 1].DestrFireLocation;
		Fire = vw_CreateParticleSystem();
		if (auto sharedFire = Fire.lock()) {
			sharedFire->SetStartLocation(FireLocation);
			sharedFire->Direction = sVECTOR3D{0.0f, 0.0f, 1.0f};
			SetWeaponFire(sharedFire, WeaponNum);
		}

		LoadObjectData(PresetEarthWeaponData[WeaponNum - 1].NameVW3D, *this);

		for (unsigned int i = 0; i < Chunks.size(); i++) {
			Texture[i] = GetPreloadedTextureAsset(PresetEarthWeaponData[WeaponNum - 1].TextureName);
			if (WeaponNum < 16)
				TextureIllum[i] = GetPreloadedTextureAsset(PresetEarthWeaponData[WeaponNum - 1].TextureIllumName);
		}

	}
	// alien 101-199
	else if ((WeaponNum >= 101) &&
		 (WeaponNum <= 199)) {
		int IntWeaponNum = WeaponNum - 100;

		ObjectStatus = eObjectStatus::Enemy;

		Strength = StrengthStart = PresetAlienWeaponData[IntWeaponNum - 1].Strength;
		WeaponLevel = PresetAlienWeaponData[IntWeaponNum - 1].WeaponLevel;
		Ammo = AmmoStart =  PresetAlienWeaponData[IntWeaponNum - 1].Ammo;
		NextFireTime =  PresetAlienWeaponData[IntWeaponNum - 1].NextFireTime;
		SFX = PresetAlienWeaponData[IntWeaponNum - 1].SFX;
		NeedRotateOnTargeting = PresetAlienWeaponData[IntWeaponNum - 1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetAlienWeaponData[IntWeaponNum - 1].EnergyUse;
		FireLocation = PresetAlienWeaponData[IntWeaponNum - 1].FireLocation;

	}
	// pirate 201-299
	else if ((WeaponNum >= 201) &&
		 (WeaponNum <= 299)) {
		int IntWeaponNum = WeaponNum - 200;

		ObjectStatus = eObjectStatus::Enemy;

		Strength = StrengthStart = PresetPirateWeaponData[IntWeaponNum - 1].Strength;
		WeaponLevel = PresetPirateWeaponData[IntWeaponNum - 1].WeaponLevel;
		Ammo = AmmoStart =  PresetPirateWeaponData[IntWeaponNum - 1].Ammo;
		NextFireTime =  PresetPirateWeaponData[IntWeaponNum - 1].NextFireTime;
		SFX = PresetPirateWeaponData[IntWeaponNum - 1].SFX;
		NeedRotateOnTargeting = PresetPirateWeaponData[IntWeaponNum - 1].NeedRotateOnTargeting;

		CurrentEnergyAccumulated = EnergyUse = PresetPirateWeaponData[IntWeaponNum - 1].EnergyUse;
		FireLocation = PresetPirateWeaponData[IntWeaponNum - 1].FireLocation;

		// турели пиратов
		switch (WeaponNum) {
		case 201:
			WeaponTurret = true;
			TargetHorizChunkNum = 0;
			TargetVertChunkNum = 1;
			TargetVertChunkMaxAngle = 89.0f;
			TargetVertChunkMinAngle = 0.0f;

			LoadObjectData(PresetPirateWeaponData[IntWeaponNum - 1].NameVW3D, *this);
			for (unsigned int i = 0; i < Chunks.size(); i++) {
				Texture[i] = GetPreloadedTextureAsset(PresetPirateWeaponData[IntWeaponNum - 1].TextureName);
				TextureIllum[i] = 0;
			}
			break;

		case 202:
			WeaponTurret = true;
			TargetHorizChunkNum = 0;
			TargetVertChunkNum = 1;
			TargetVertChunkMaxAngle = 89.0f;
			TargetVertChunkMinAngle = 20.0f;

			LoadObjectData(PresetPirateWeaponData[IntWeaponNum - 1].NameVW3D, *this);
			for (unsigned int i = 0; i < Chunks.size(); i++) {
				Texture[i] = GetPreloadedTextureAsset(PresetPirateWeaponData[IntWeaponNum - 1].TextureName);
				TextureIllum[i] = 0;
			}
			break;
		}

		if ((WeaponNum == 201) ||
		    (WeaponNum == 202)) {
			if (TargetHorizChunkNum != -1) {
				BaseBound = Chunks[TargetHorizChunkNum].Location;
			}

			if (TargetVertChunkNum != -1) {
				if (TargetHorizChunkNum != -1)
					MiddleBound = Chunks[TargetVertChunkNum].Location -
						      Chunks[TargetHorizChunkNum].Location;
				else
					MiddleBound = Chunks[TargetVertChunkNum].Location;
			}

			if (TargetVertChunkNum != -1)
				WeaponBound = FireLocation - Chunks[TargetVertChunkNum].Location;
			else if (TargetHorizChunkNum != -1)
				WeaponBound = FireLocation - Chunks[TargetHorizChunkNum].Location;
			else
				WeaponBound = FireLocation;
		}
	}
}

/*
 * Destructor.
 */
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
	if (!LaserMaser.expired()) {
		ReleaseProjectile(LaserMaser);
		if (vw_IsSoundAvailable(LaserMaserSoundNum))
			vw_StopSound(LaserMaserSoundNum, 150);
	}
}

/*
 * Update.
 */
bool cWeapon::Update(float Time)
{
	if (auto sharedLaserMaser = LaserMaser.lock()) {
		if (sharedLaserMaser->Lifetime <= 0.0f)
			ReleaseProjectile(LaserMaser);
	}

	if (!cObject3D::Update(Time))
		return false;

	if (LastFireTime + TimeDelta <= Time)
		if (auto sharedFire = Fire.lock())
			sharedFire->IsSuppressed = true;

	if ((InternalType >= 1) &&
	    (InternalType <= 99) &&
	    (Strength < StrengthStart) &&
	    DestroyedFire.expired()) {
		DestroyedFire = vw_CreateParticleSystem();
		if (auto sharedDestroyedFire = DestroyedFire.lock()) {
			sharedDestroyedFire->ColorStart.r = 1.00f;
			sharedDestroyedFire->ColorStart.g = 0.70f;
			sharedDestroyedFire->ColorStart.b = 0.30f;
			sharedDestroyedFire->ColorEnd.r = 1.00f;
			sharedDestroyedFire->ColorEnd.g = 0.00f;
			sharedDestroyedFire->ColorEnd.b = 0.00f;
			sharedDestroyedFire->AlphaStart = 1.00f;
			sharedDestroyedFire->AlphaEnd = 0.10f;
			sharedDestroyedFire->SizeStart = 0.20f;
			sharedDestroyedFire->SizeVar= 0.10f;
			sharedDestroyedFire->SizeEnd = 0.10f;
			sharedDestroyedFire->Speed = 8.00f;
			sharedDestroyedFire->SpeedVar = 2.00f;
			sharedDestroyedFire->Theta = 5.00f;
			sharedDestroyedFire->Life = 0.50f * Length / 3.0f;
			sharedDestroyedFire->ParticlesPerSec = 70;
			sharedDestroyedFire->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
			sharedDestroyedFire->CreationType = eParticleCreationType::Cube;
			sharedDestroyedFire->CreationSize(Width / 2.0f, Width / 2.0f, 0.1f);
			sharedDestroyedFire->Direction(0.0f, 0.0f, -1.0f);
			sharedDestroyedFire->SetStartLocation(DestroyedFireLocation);
		}
	}

	if ((InternalType >= 1) &&
	    (InternalType <= 99) &&
	    (Strength <= 0.0f) &&
	    DestroyedSmoke.expired()) {
		DestroyedSmoke = vw_CreateParticleSystem();
		if (auto sharedDestroyedSmoke = DestroyedSmoke.lock()) {
			sharedDestroyedSmoke->ColorStart.r = 1.00f;
			sharedDestroyedSmoke->ColorStart.g = 1.00f;
			sharedDestroyedSmoke->ColorStart.b = 1.00f;
			sharedDestroyedSmoke->ColorEnd.r = 1.00f;
			sharedDestroyedSmoke->ColorEnd.g = 1.00f;
			sharedDestroyedSmoke->ColorEnd.b = 1.00f;
			sharedDestroyedSmoke->AlphaStart = 0.20f;
			sharedDestroyedSmoke->AlphaEnd = 0.00f;
			sharedDestroyedSmoke->SizeStart = 0.25f;
			sharedDestroyedSmoke->SizeVar = 0.10f;
			sharedDestroyedSmoke->SizeEnd = 0.00f;
			sharedDestroyedSmoke->Speed = 0.00f;
			sharedDestroyedSmoke->SpeedVar = 0.00f;
			sharedDestroyedSmoke->Theta = 35.00f;
			sharedDestroyedSmoke->Life = 2.00f * Length / 3.0f;
			sharedDestroyedSmoke->ParticlesPerSec = 300;
			sharedDestroyedSmoke->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
			sharedDestroyedSmoke->CreationType = eParticleCreationType::Point;
			sharedDestroyedSmoke->CreationSize(Width / 2.5f, Width / 2.5f, 0.1f);
			sharedDestroyedSmoke->Direction(0.0f, 0.0f, -1.0f);
			sharedDestroyedSmoke->SetStartLocation(DestroyedFireLocation);
		}
	}

	if (InternalType == 17 && SwarmNum > 0)
		if (LastFireTime + 0.15f < Time)
			if (Ammo > 0 || !GameUnlimitedAmmo) {
				LastFireTime = Time;

				if (!GameUnlimitedAmmo)
					Ammo -= 1;

				float CurrentPenalty{1.0f};
				if (ObjectStatus == eObjectStatus::Enemy)
					CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty);

				switch (SwarmNum) {
				case 9:
					FireLocation(-0.5f, -0.5f, 2.6f);
					break;
				case 8:
					FireLocation(0.5f, -0.8f, 2.6f);
					break;
				case 7:
					FireLocation(-0.5f, -0.8f, 2.6f);
					break;
				case 6:
					FireLocation(0.5f, -0.5f, 2.6f);
					break;
				case 5:
					FireLocation(-0.2f, -0.95f, 2.6f);
					break;
				case 4:
					FireLocation(0.2f, -0.65f, 2.6f);
					break;
				case 3:
					FireLocation(-0.2f, -0.3f, 2.6f);
					break;
				case 2:
					FireLocation(0.2f, -0.65f, 2.6f);
					break;
				case 1:
					FireLocation(0.2f, -0.3f, 2.6f);
					break;
				}
				vw_Matrix33CalcPoint(FireLocation, CurrentRotationMat);

				std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(InternalType);
				if (auto sharedProjectile = tmpProjectile.lock()) {
					sharedProjectile->SetLocation(Location + FireLocation);
					sharedProjectile->SetRotation(Rotation);
					for (auto &tmpGFX : sharedProjectile->GraphicFX) {
						if (auto sharedGFX = tmpGFX.lock()) {
							if (auto sharedFire = Fire.lock())
								sharedGFX->Direction = sharedFire->Direction ^ -1;

							if (CurrentPenalty == 2)
								sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.33f);
							else if (CurrentPenalty == 3)
								sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.5f);
							sharedGFX->Speed = sharedGFX->Speed / CurrentPenalty;
							sharedGFX->Life = sharedGFX->Life * CurrentPenalty;
							sharedGFX->MagnetFactor = sharedGFX->MagnetFactor / (CurrentPenalty * CurrentPenalty);
						}
					}
					sharedProjectile->ObjectStatus = ObjectStatus;
					sharedProjectile->SpeedStart = sharedProjectile->SpeedEnd = sharedProjectile->SpeedStart / CurrentPenalty;
					sharedProjectile->Age = sharedProjectile->Lifetime = sharedProjectile->Age * CurrentPenalty;
					sharedProjectile->DamageHull = sharedProjectile->DamageHull / CurrentPenalty;
					sharedProjectile->DamageSystems = sharedProjectile->DamageSystems / CurrentPenalty;

					if (SFX != eGameSFX::none) {
						float fVol = 1.0f;
						PlayGameSFX(SFX, fVol, sharedProjectile->Location);
					}
				}
				SwarmNum--;
			}

	if (InternalType == 203 && SwarmNum > 0)
		if (LastFireTime + 0.4f < Time)
			if (Ammo > 0 || !GameUnlimitedAmmo) {
				LastFireTime = Time;

				if (!GameUnlimitedAmmo)
					Ammo -= 1;

				float CurrentPenalty = 1.0f; // FIXME why we need if-else below if we use 1.0f only?

				std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(InternalType);
				if (auto sharedProjectile = tmpProjectile.lock()) {
					sharedProjectile->SetLocation(Location + FireLocation);
					sharedProjectile->SetRotation(Rotation + sVECTOR3D{vw_fRand0() * 30.0f,
											   0.0f,
											   vw_fRand0() * 30.0f});

					for (auto &tmpGFX : sharedProjectile->GraphicFX) {
						if (auto sharedGFX = tmpGFX.lock()) {
							sharedGFX->Direction = Orientation ^ -1;

							if (CurrentPenalty == 2)
								sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.33f);
							else if (CurrentPenalty == 3)
								sharedGFX->ParticlesPerSec -= (int)(sharedGFX->ParticlesPerSec * 0.5f);
							sharedGFX->Speed = sharedGFX->Speed / CurrentPenalty;
							sharedGFX->Life = sharedGFX->Life * CurrentPenalty;
							sharedGFX->MagnetFactor = sharedGFX->MagnetFactor / (CurrentPenalty * CurrentPenalty);
						}
					}
					sharedProjectile->ObjectStatus = ObjectStatus;
					sharedProjectile->SpeedStart = sharedProjectile->SpeedEnd = sharedProjectile->SpeedStart / CurrentPenalty;
					sharedProjectile->Age = sharedProjectile->Lifetime = sharedProjectile->Age * CurrentPenalty;
					sharedProjectile->DamageHull = sharedProjectile->DamageHull / CurrentPenalty;
					sharedProjectile->DamageSystems = sharedProjectile->DamageSystems / CurrentPenalty;

					if (SFX != eGameSFX::none)
						PlayGameSFX(SFX, 1.0f, sharedProjectile->Location);
				}
				SwarmNum--;
			}

	if (WeaponTurret) {
		sVECTOR3D NeedAngle(TargetVertChunkNeedAngle,TargetHorizChunkNeedAngle,0);
		GetTurretOnTargetOrientateion(ObjectStatus, Location+FireLocation, Rotation,
					      CurrentRotationMat, NeedAngle, InternalType);
		TargetHorizChunkNeedAngle = NeedAngle.y;
		TargetVertChunkNeedAngle = NeedAngle.x;

		if (TargetHorizChunkNum != -1)
			if (TargetHorizChunkNeedAngle != TargetHorizChunkCurrentAngle) {
				if (fabsf(TargetHorizChunkNeedAngle - TargetHorizChunkCurrentAngle) > 180.0f) {
					if (TargetHorizChunkCurrentAngle - TargetHorizChunkNeedAngle > 180.0f)
						TargetHorizChunkCurrentAngle -= 360.0f;
					if (TargetHorizChunkNeedAngle - TargetHorizChunkCurrentAngle > 180.0f)
						TargetHorizChunkCurrentAngle += 360.0f;
				}
				float NeedRotate = TargetHorizChunkCurrentAngle;

				if (TargetHorizChunkNeedAngle > TargetHorizChunkCurrentAngle) {
					NeedRotate += 80.0f * TimeDelta / GameEnemyTargetingSpeedPenalty;
					if (NeedRotate > TargetHorizChunkNeedAngle)
						NeedRotate = TargetHorizChunkNeedAngle;
				} else {
					NeedRotate -= 80.0f * TimeDelta / GameEnemyTargetingSpeedPenalty;
					if (NeedRotate < TargetHorizChunkNeedAngle)
						NeedRotate = TargetHorizChunkNeedAngle;
				}

				TargetHorizChunkCurrentAngle = NeedRotate;

				for (auto &tmpChunk : Chunks) {
					sVECTOR3D tmp = tmpChunk.Location - Chunks[TargetHorizChunkNum].Location;

					vw_RotatePointInv(tmp, tmpChunk.Rotation ^ (-1.0f));

					tmpChunk.Rotation.y = -NeedRotate;

					vw_RotatePoint(tmp, tmpChunk.Rotation);

					tmpChunk.Location = tmp + Chunks[TargetHorizChunkNum].Location;
				}
			}

		if (TargetVertChunkNum != -1)
			if (TargetVertChunkNeedAngle != TargetVertChunkCurrentAngle) {
				float NeedRotate = TargetVertChunkCurrentAngle;
				if (TargetVertChunkNeedAngle > TargetVertChunkCurrentAngle) {
					NeedRotate += 80.0f * TimeDelta / GameEnemyTargetingSpeedPenalty;
					if (NeedRotate > TargetVertChunkNeedAngle)
						NeedRotate = TargetVertChunkNeedAngle;
					if (NeedRotate > TargetVertChunkMaxAngle)
						NeedRotate = TargetVertChunkMaxAngle;
				} else {
					NeedRotate -= 80.0f * TimeDelta / GameEnemyTargetingSpeedPenalty;
					if (NeedRotate < TargetVertChunkNeedAngle)
						NeedRotate = TargetVertChunkNeedAngle;
					if (NeedRotate < TargetVertChunkMinAngle)
						NeedRotate = TargetVertChunkMinAngle;
				}

				TargetVertChunkCurrentAngle = NeedRotate;

				sVECTOR3D tmp = Chunks[TargetVertChunkNum].Location - Chunks[TargetVertChunkNum].Location; // FIXME (0,0,0)???

				vw_RotatePointInv(tmp, Chunks[TargetVertChunkNum].Rotation ^ (-1.0f));

				Chunks[TargetVertChunkNum].Rotation.x = -NeedRotate;

				vw_RotatePoint(tmp, Chunks[TargetVertChunkNum].Rotation);

				Chunks[TargetVertChunkNum].Location = tmp + Chunks[TargetVertChunkNum].Location;
			}

		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizChunkNum != -1)
			RotationMiddle = Chunks[TargetHorizChunkNum].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		sVECTOR3D RotationWeapon = Rotation;
		if (TargetVertChunkNum != -1)
			RotationWeapon = Chunks[TargetVertChunkNum].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

		if ((TargetHorizChunkNum == -1) && (TargetVertChunkNum == -1))
			RotationWeapon = sVECTOR3D{TargetVertChunkNeedAngle,
						   TargetHorizChunkNeedAngle,
						   0.0f} +
					 Rotation;

		Orientation = sVECTOR3D{0.0f, 0.0f, 1.0f};
		vw_RotatePoint(Orientation, RotationWeapon);
	}

	return true;
}

/*
 * Set rotation.
 */
void cWeapon::SetRotation(const sVECTOR3D &NewRotation)
{
	cObject3D::SetRotation(NewRotation);

	if (!WeaponTurret) {
		vw_Matrix33CalcPoint(FireLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(FireLocation, CurrentRotationMat);

		if (auto sharedFire = Fire.lock()) {
			sharedFire->MoveSystem(Location + FireLocation);
			sharedFire->SetStartLocation(Location + FireLocation);
			sharedFire->RotateSystemByAngle(Rotation);
		}
	} else {
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizChunkNum != -1)
			RotationMiddle = Chunks[TargetHorizChunkNum].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		sVECTOR3D RotationWeapon = Rotation;
		if (TargetVertChunkNum != -1)
			RotationWeapon = Chunks[TargetVertChunkNum].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

		if ((TargetHorizChunkNum == -1) && (TargetVertChunkNum == -1))
			RotationWeapon = sVECTOR3D{TargetVertChunkNeedAngle,
						   TargetHorizChunkNeedAngle,
						   0.0f} +
					 Rotation;

		Orientation = sVECTOR3D{0.0f, 0.0f, 1.0f};
		vw_RotatePoint(Orientation, RotationWeapon);
	}

	if (auto sharedDestroyedFire = DestroyedFire.lock()) {
		vw_Matrix33CalcPoint(DestroyedFireLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(DestroyedFireLocation, CurrentRotationMat);
		sharedDestroyedFire->MoveSystem(Location + DestroyedFireLocation);
		sharedDestroyedFire->SetStartLocation(Location + DestroyedFireLocation);
		sharedDestroyedFire->RotateSystemByAngle(Rotation);
	}
	if (auto sharedDestroyedSmoke = DestroyedSmoke.lock()) {
		sharedDestroyedSmoke->MoveSystemLocation(Location + DestroyedFireLocation);
		sharedDestroyedSmoke->RotateSystemByAngle(Rotation);
	}
	if (auto sharedLaserMaser = LaserMaser.lock()) {
		vw_Matrix33CalcPoint(sharedLaserMaser->ProjectileCenter, OldInvRotationMat);
		vw_Matrix33CalcPoint(sharedLaserMaser->ProjectileCenter, CurrentRotationMat);
		sharedLaserMaser->SetLocation(Location + FireLocation + sharedLaserMaser->ProjectileCenter);
		sharedLaserMaser->SetRotation(NewRotation);
	}
}

/*
 * Set location.
 */
void cWeapon::SetLocation(const sVECTOR3D &NewLocation)
{
	cObject3D::SetLocation(NewLocation);

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

	if (auto sharedLaserMaser = LaserMaser.lock())
		sharedLaserMaser->SetLocation(Location + FireLocation + sharedLaserMaser->ProjectileCenter);

	if (vw_IsSoundAvailable(LaserMaserSoundNum))
		vw_SetSoundLocation(LaserMaserSoundNum, Location.x, Location.y, Location.z);
}

/*
 * Fire.
 */
bool cWeapon::WeaponFire(float Time)
{
	if (InternalType == 0)
		return false;

	float CurrentPenalty{1.0f};
	if (ObjectStatus == eObjectStatus::Enemy)
		CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty);

	if (InternalType == 203) // flares
		CurrentPenalty = 1.0f;

	if (Time < LastFireTime + NextFireTime * CurrentPenalty)
		return false;
	LastFireTime = Time;

	if ((InternalType >= 1) &&
	    (InternalType <= 99)) {
		bool Misfire{false};
		if (Strength < StrengthStart)
			if (Strength/StrengthStart > vw_fRand())
				Misfire = true;

		if ((Strength <= 0.0f) ||
		    (Ammo == 0) ||
		    Misfire) {
			float fVol = 1.0f;

			switch (InternalType) {
			case 1: // Kinetic1
			case 2: // Kinetic2
			case 3: // Kinetic3
			case 4: // Kinetic4
				PlayGameSFX(eGameSFX::WeaponMalfunction_Kinetic, fVol, Location);
				break;

			case 5: // Ion1
			case 6: // Ion2
			case 7: // Ion3
			case 8: // Plasma1
			case 9: // Plasma2
			case 10: // Plasma3
				PlayGameSFX(eGameSFX::WeaponMalfunction_Particle, fVol, Location);
				break;

			case 11: // Maser1
			case 12: // Maser2
			case 14: // Laser
				PlayGameSFX(eGameSFX::WeaponMalfunction_Beam, fVol, Location);
				break;

			case 13: // Antimatter

			case 15: // Gauss
				PlayGameSFX(eGameSFX::WeaponMalfunction_Energy, fVol, Location);
				break;

			case 16: // missiles
			case 17: // swarm
			case 18: // torpedo
			case 19: // bomb
				PlayGameSFX(eGameSFX::WeaponMalfunction_Launcher, fVol, Location);
				break;
			}

			return false;
		}

		if (ObjectStatus == eObjectStatus::Player) {
			if (CurrentEnergyAccumulated < EnergyUse)
				return false;
			else
				CurrentEnergyAccumulated = 0.0f;
		}

		if (!GameUnlimitedAmmo)
			Ammo -= 1;

		if (InternalType < 16) {
			if (auto sharedFire = Fire.lock())
				sharedFire->IsSuppressed = false;
		}

	}

	sVECTOR3D RotationWeapon = Rotation;
	if (WeaponTurret) {
		sVECTOR3D RotationBase = Rotation;
		sVECTOR3D BaseBoundTMP = BaseBound;
		vw_RotatePoint(BaseBoundTMP, RotationBase);

		sVECTOR3D RotationMiddle = Rotation;
		sVECTOR3D MiddleBoundTMP = MiddleBound;
		if (TargetHorizChunkNum != -1)
			RotationMiddle = Chunks[TargetHorizChunkNum].Rotation + Rotation;
		vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

		if (TargetVertChunkNum != -1)
			RotationWeapon = Chunks[TargetVertChunkNum].Rotation + Rotation;

		sVECTOR3D WeaponBoundTMP = WeaponBound;
		vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

		FireLocation = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

		if (TargetHorizChunkNum == -1 && TargetVertChunkNum == -1) {
			RotationWeapon = sVECTOR3D{TargetVertChunkNeedAngle,
						   TargetHorizChunkNeedAngle,
						   0.0f} +
					 Rotation;
		}
	}

	std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(InternalType);
	if (auto sharedProjectile = tmpProjectile.lock()) {
		if (sharedProjectile->ProjectileType == 2) {
			if (!LaserMaser.expired())
				ReleaseProjectile(LaserMaser);

			LaserMaser = tmpProjectile;
			vw_Matrix33CalcPoint(sharedProjectile->ProjectileCenter, CurrentRotationMat);
			sharedProjectile->SetLocation(Location + FireLocation + sharedProjectile->ProjectileCenter);
		} else {
			if (sharedProjectile->ProjectileType == 0) {
				sVECTOR3D AddPos(0.0f, 0.0f, 4.0f);
				if (WeaponTurret)
					AddPos = sVECTOR3D{0.0f, 0.0f, 2.0f};

				vw_RotatePoint(AddPos, RotationWeapon);
				sharedProjectile->SetLocation(Location+FireLocation + AddPos);
			} else
				sharedProjectile->SetLocation(Location+FireLocation);
		}

		if ((sharedProjectile->ProjectileType == 3) ||
		    (sharedProjectile->ProjectileType == 4)) {
			sharedProjectile->SetRotation(RotationWeapon);
			for (auto &tmpGFX : sharedProjectile->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock())
					sharedGFX->Direction = Orientation;
			}
			sharedProjectile->ObjectStatus = ObjectStatus;
			sharedProjectile->SpeedStart = sharedProjectile->SpeedEnd = sharedProjectile->SpeedStart / CurrentPenalty;
			sharedProjectile->Age = sharedProjectile->Lifetime = sharedProjectile->Age;
			sharedProjectile->DamageHull = sharedProjectile->DamageHull / CurrentPenalty;
			sharedProjectile->DamageSystems = sharedProjectile->DamageSystems / CurrentPenalty;

			// FIXME should be revised, we use -3 in order to 'convert' to proper projectile type
			sharedProjectile->ProjectileType = sharedProjectile->ProjectileType - 3;
		} else {
			sharedProjectile->SetRotation(RotationWeapon);
			for (auto &tmpGFX : sharedProjectile->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = Orientation;
					// учитываем пенальти для визуальных эффектов
					sharedGFX->ParticlesPerSec = (int)(sharedGFX->ParticlesPerSec / CurrentPenalty);

					sharedGFX->Speed = sharedGFX->Speed / CurrentPenalty;
					sharedGFX->Life = sharedGFX->Life * CurrentPenalty;
					sharedGFX->MagnetFactor = sharedGFX->MagnetFactor / (CurrentPenalty * CurrentPenalty);
				}
			}
			sharedProjectile->ObjectStatus = ObjectStatus;
			sharedProjectile->SpeedStart = sharedProjectile->SpeedEnd = sharedProjectile->SpeedStart / CurrentPenalty;
			sharedProjectile->Age = sharedProjectile->Lifetime = sharedProjectile->Age * CurrentPenalty;
			sharedProjectile->DamageHull = sharedProjectile->DamageHull / CurrentPenalty;
			sharedProjectile->DamageSystems = sharedProjectile->DamageSystems / CurrentPenalty;
		}
	}

	if (SFX != eGameSFX::none) {
		float fVol = 1.0f;
		LaserMaserSoundNum = PlayGameSFX(SFX, fVol, Location + FireLocation);
		if (LaserMaser.expired())
			LaserMaserSoundNum = 0;
	}

	if (InternalType == 17) // 9 more swarm missiles
		SwarmNum = 9;

	if (InternalType == 203) // 4 more flares
		SwarmNum = 4;

	return true;
}

} // astromenace namespace
} // viewizard namespace
