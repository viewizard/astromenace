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

// NOTE we use ReleaseProjectile() in weapon code for beam, since we use beam weapon for
//      player and motherships only (rare enough). If enemy will have beam weapon, revise
//      weapon code and/or projectile STL container (std::list is using now).

/*

Note, all enemy's projectiles and missiles without penalty should be about 30% faster
and maneuverable than player's projectiles and missiles. In this way, even x3 penalty
will not kill their speed and maneuverability and we will have nice difficulty matrix.

*/

#include "projectile.h"
#include "../explosion/explosion.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

struct sProjectileData {
	float Radius;
	float DamageHull;
	float DamageSystems;
	// 0-обычный снаряд, 1-уничтожаемый, 2-лучевой, 3- фларес/мина пришельцев, 4-мина (уничтожаемая)
	int ProjectileType;
	float Speed;
	float Age;
	int GraphicFXQuantity;
};

const std::string MissileTrailTextures[]{{"gfx/trail1.tga"},
					 {"gfx/trail2.tga"},
					 {"gfx/trail3.tga"},
					 {"gfx/trail4.tga"},
					 {"gfx/trail5.tga"}};

// снаряды для оружия землян 1-99
const std::vector<sProjectileData> PresetEarthProjectileData{
	// Kinetic
	{0.3f, 5, 0,	0, 50, 4, 1},
	{0.6f, 20, 0,	0, 45, 4, 1},
	{0.6f, 40, 0,	0, 45, 4, 1},
	{0.4f, 10, 0,	0, 50, 4, 1},
	// Ion
	{1.5f, 0, 10,	0, 40, 4, 1},
	{1.5f, 0, 30,	0, 40, 4, 2},
	{2.0f, 0, 50,	0, 40, 4, 2},
	// Plasma
	{1.0f, 30, 5,	0, 40, 4, 1},
	{1.3f, 40, 25,	0, 40, 4, 3},
	{1.3f, 50, 50,	0, 45, 4, 3},
	// Maser
	{0.6f, 50, 0,	2, 0, 2, 1},
	{1.0f, 80, 0,	2, 0, 2, 2},
	// Antimatter
	{1.0f, 80, 10,	0, 70, 3, 1},
	// Laser
	{0.4f, 150, 0,	2, 0, 1, 1},
	// Gauss
	{1.1f, 100, 0,	0, 80, 3, 1},
	// Missiles
	{0.2f, 100, 0,	1, 40, 8, 2},
	{0.2f, 20, 0,	1, 40, 6, 2},
	{0.2f, 400, 0,	1, 35, 7, 2},
	{0.2f, 800, 0,	1, 30, 6, 2},
};

// снаряды для оружия пришельцев 101-199
const std::vector<sProjectileData> PresetAlienProjectileData{
	// как Kinetic1
	{0.3f, 5, 0,	0, 70, 4, 1},
	// с наведением, как Kinetic2
	{0.6f, 10, 0,	0, 50, 4, 2},
	// как Kinetic3
	{0.6f, 20, 0,	0, 60, 4, 1},
	// с наведением, как Kinetic3
	{0.6f, 20, 0,	0, 60, 4, 2},
	// как Kinetic2
	{0.6f, 10, 0,	0, 50, 4, 1},
	// энергетическая мина (1-й тип), только позиционируется по высоте (на ближайшего врага)
	{2.0f, 60, 0,	3, 0, 16, 1},
	// энергетическая мина (2-й тип), позиционируется по высоте (на ближайшего врага) + немного приближается
	{2.0f, 120, 0,	3, 7, 16, 1},
	// как Plasma3
	{1.3f, 50, 50,	0, 55, 4, 3},
	// как Plasma2
	{1.3f, 40, 25,	0, 55, 4, 3},
	// как Laser (для больших кораблей)
	{1.0f, 250, 0,	2, 0, 1, 1},

};

// снаряды для оружия пиратов 201-299
const std::vector<sProjectileData> PresetPirateProjectileData{
	// стрельба турели 1
	{0.3f, 5, 0,	0, 60, 4, 1},
	// стрельба турели 2
	{0.6f, 20, 0,	0, 45, 4, 1},
	// фларес
	{3.0f, 5, 0,	3, 5, 5, 1},

	// как Kinetic1
	{0.3f, 5, 0,	0, 60, 4, 1},
	// как Missile1
	{0.2f, 40, 0,	1, 60, 6, 2},
	// как Missile2
	{0.2f, 20, 0,	1, 60, 6, 2},
	// как Ion2
	{1.5f, 0, 30,	0, 40, 4, 2},
	// как Antimatter
	{1.0f, 80, 10,	0, 70, 3, 1},
	// как Missile3 (торпеда)
	{0.2f, 400, 0,	1, 55, 7, 2},
	// как Missile4 (бомба)
	{0.2f, 1000, 0,	1, 50, 6, 2},
	// как Kinetic2
	{0.6f, 20, 0,	0, 45, 4, 1},
	// как Kinetic3
	{0.6f, 40, 0,	0, 45, 4, 1},
	// как Plasma2
	{1.3f, 40, 25,	0, 40, 4, 3},

	// мина1 (простое наведение по высоте)
	{1.2f, 50, 0,	4, 0, -1, 1},
	// мина2 (наведение по высоте + приближение)
	{1.2f, 100, 0,	4, 7, -1, 1},
	// мина3 (наведение по высоте + стрельба снарядами)
	{1.2f, 150, 0,	4, 0, -1, 1},
	// мина4 (наведение по высоте + стрельба ракетами)
	{1.2f, 200, 0,	4, 0, -1, 1},
};

std::list<std::shared_ptr<cProjectile>> ProjectileList{};

} // unnamed namespace


/*
 * Create cProjectile object.
 */
std::weak_ptr<cProjectile> CreateProjectile(int ProjectileNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	ProjectileList.emplace_front(new cProjectile{ProjectileNum}, [](cProjectile *p) {delete p;});
	return ProjectileList.front();
}

//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllProjectile(float Time)
{
	for (auto iter = ProjectileList.begin(); iter != ProjectileList.end();) {
		if (!iter->get()->Update(Time))
			iter = ProjectileList.erase(iter);
		else
			++iter;
	}
}

//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllProjectiles(bool VertexOnlyPass, unsigned int ShadowMap)
{
	for (auto &tmpProjectile : ProjectileList) {
		tmpProjectile->Draw(VertexOnlyPass, ShadowMap);
	}
}

/*
 * Release particular projectile object.
 */
void ReleaseProjectile(std::weak_ptr<cProjectile> &Object)
{
	auto sharedObject = Object.lock();
	if (!sharedObject)
		return;

	for (auto iter = ProjectileList.begin(); iter != ProjectileList.end();) {
		if (iter->get() == sharedObject.get()) {
			ProjectileList.erase(iter);
			return;
		}
		++iter;
	}
}

//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllProjectiles()
{
	ProjectileList.clear();
}

/*
 * Managed cycle for each projectile.
 */
void ForEachProjectile(std::function<void (cProjectile &Object, eProjectileCycle &Command)> function)
{
	for (auto iter = ProjectileList.begin(); iter != ProjectileList.end();) {
		eProjectileCycle Command{eProjectileCycle::Continue};
		function(*iter->get(), Command);

		switch (Command) {
		case eProjectileCycle::Continue:
			++iter;
			break;
		case eProjectileCycle::Break:
			return;
		case eProjectileCycle::DeleteObjectAndContinue:
			iter = ProjectileList.erase(iter);
			break;
		case eProjectileCycle::DeleteObjectAndBreak:
			ProjectileList.erase(iter);
			return;
		}
	}
}

/*
 * Managed cycle for each projectile pair.
 */
void ForEachProjectilePair(std::function<void (cProjectile &FirstObject,
					       cProjectile &SecondObject,
					       eProjectilePairCycle &Command)> function)
{
	for (auto iterFirst = ProjectileList.begin(); iterFirst != ProjectileList.end();) {
		eProjectilePairCycle Command{eProjectilePairCycle::Continue};

		for (auto iterSecond = std::next(iterFirst, 1); iterSecond != ProjectileList.end();) {
			Command = eProjectilePairCycle::Continue;
			function(*iterFirst->get(), *iterSecond->get(), Command);

			if ((Command == eProjectilePairCycle::DeleteSecondObjectAndContinue) ||
			    (Command == eProjectilePairCycle::DeleteBothObjectsAndContinue))
				iterSecond = ProjectileList.erase(iterSecond);
			else
				++iterSecond;

			// break second cycle
			if ((Command == eProjectilePairCycle::DeleteFirstObjectAndContinue) ||
			    (Command == eProjectilePairCycle::DeleteBothObjectsAndContinue))
				break;
		}

		if ((Command == eProjectilePairCycle::DeleteFirstObjectAndContinue) ||
		    (Command == eProjectilePairCycle::DeleteBothObjectsAndContinue))
			iterFirst = ProjectileList.erase(iterFirst);
		else
			++iterFirst;
	}
}

/*
 * Get object ptr by reference.
 */
std::weak_ptr<cObject3D> GetProjectilePtr(const cProjectile &Object)
{
	for (auto &tmpProjectile : ProjectileList) {
		if (tmpProjectile.get() == &Object)
			return tmpProjectile;
	}

	return std::weak_ptr<cObject3D>{};
}




// получаем время жизни снаряда
float GetProjectileRange(int Num)
{

	if (Num >= 1 && Num <= 99) {
		float Return = PresetEarthProjectileData[Num-1].Age * PresetEarthProjectileData[Num-1].Speed;
		if (Num == 11) Return = 100.0f;
		if (Num == 12) Return = 110.0f;
		if (Num == 14) Return = 120.0f;
		return Return;
	} else if (Num >= 101 && Num <= 199) {
		if (Num == 110) return 200.0f;
		return PresetAlienProjectileData[Num-101].Age * PresetAlienProjectileData[Num-101].Speed;
	} else if (Num >= 201 && Num <= 299) {
		return PresetPirateProjectileData[Num-201].Age * PresetPirateProjectileData[Num-201].Speed;
	}

	return 0.0f;
}
int GetWeaponHullDamage(int Num)
{
	if (Num >= 1 && Num <= 99) {
		// если система залпового огня
		if (Num == 17) return 10 * (int)PresetEarthProjectileData[Num-1].DamageHull;
		return (int)PresetEarthProjectileData[Num-1].DamageHull;
	} else if (Num >= 101 && Num <= 199) {
		return (int)PresetAlienProjectileData[Num-101].DamageHull;
	} else if (Num >= 201 && Num <= 299) {
		return (int)PresetPirateProjectileData[Num-201].DamageHull;
	}
	return 0;
}
int GetWeaponSystemsDamage(int Num)
{
	if (Num >= 1 && Num <= 99) {
		// если система залпового огня
		if (Num == 17) return 10 * (int)PresetEarthProjectileData[Num-1].DamageSystems;
		return (int)PresetEarthProjectileData[Num-1].DamageSystems;
	} else if (Num >= 101 && Num <= 199) {
		return (int)PresetAlienProjectileData[Num-101].DamageSystems;
	} else if (Num >= 201 && Num <= 299) {
		return (int)PresetPirateProjectileData[Num-201].DamageSystems;
	}
	return 0;
}
float GetProjectileSpeed(int Num)
{
	if (Num >= 1 && Num <= 99) {
		return PresetEarthProjectileData[Num-1].Speed;
	} else if (Num >= 101 && Num <= 199) {
		return PresetAlienProjectileData[Num-101].Speed;
	} else if (Num >= 201 && Num <= 299) {
		return PresetPirateProjectileData[Num-201].Speed;
	}
	return 0.0f;
}






//-----------------------------------------------------------------------------
// Создание графического эффекта
//-----------------------------------------------------------------------------
static void SetProjectileGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXNum)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");

	switch(GFXNum) {
	case 1:	// Kinetic
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 0.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.15f;
		ParticleSystem->ParticlesPerSec = 300;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
		break;
	case 2:	// Kinetic
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.15f);
		break;
	case 3:	// Kinetic
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.60f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.1f);
		break;
	case 4:	// Kinetic
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->SpeedVar   = 0.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 300;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.17f);
		break;
	case 5:	// Ion
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.20f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 1.00f;
		ParticleSystem->ParticlesPerSec = 60;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{2.0f, 2.0f, 0.5f};
		ParticleSystem->DeadZone = 1.5f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 25.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.5f, 0.35f, 0.0f, 0.1f);
		break;
	case 6:	// Ion
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.30f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 30;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -20.0f;
		break;
	case 7:	// Plasma
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.30f;
		ParticleSystem->Speed      = 6.40f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->NeedStop = true;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.15f);
		break;
	case 8:	// Plasma
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.80f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{1.3f, 1.3f, 0.2f};
		ParticleSystem->DeadZone = 1.2f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 2.0f;
		break;
	case 9:	// Plasma
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.80f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{1.3f, 0.2f, 1.3f};
		ParticleSystem->DeadZone = 1.2f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 2.0f;
		break;
	case 10:	// Plasma
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.80f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 4.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 30;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.10f);
		break;
	case 11:	// Plasma
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.60f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 150;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.08f);
		break;
	case 12:	// Ion
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 200;
		break;


	case 13:	// Missile
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.35f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.1f);
		break;
	case 14:	// Torpedo
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.35f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 1.0f, 0.15f, 0.0f, 0.075f);
		break;
	case 15:	// Nuke
		ParticleSystem->ColorStart.r = 0.30f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.15f, 0.35f, 1.0f, 0.0f, 0.05f);
		break;
	case 16:	// Swarm
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.25f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
		break;



	case 17:	// maser
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 1.00f;
		ParticleSystem->ParticlesPerSec = 600;
		ParticleSystem->CreationType = eParticleCreationType::Tube;
		ParticleSystem->CreationSize = sVECTOR3D{0.4f, 0.4f, 0.2f};
		ParticleSystem->AlphaShowHide = true;
		break;
	case 18:	// maser2
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 1.00f;
		ParticleSystem->ParticlesPerSec = 400;
		ParticleSystem->CreationType = eParticleCreationType::Tube;
		ParticleSystem->CreationSize = sVECTOR3D{0.8f, 0.8f, 0.2f};
		ParticleSystem->AlphaShowHide = true;
		break;
	case 19:	// Antimatter
		ParticleSystem->ColorStart.r = 0.50f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 0.50f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 1.00f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{0.8f, 0.1f, 0.8f};
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -20.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.5f, 1.0f, 0.0f, 0.0f, 0.05f);
		break;
	case 20:	// Laser
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.20f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 3000;
		ParticleSystem->CreationType = eParticleCreationType::Tube;
		ParticleSystem->CreationSize = sVECTOR3D{0.2f, 0.2f, 0.1f};
		break;
	case 21:	// Antimatter
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.20f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 0.30f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 150;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{1.0f, 1.0f, 0.1f};
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -40.0f;
		break;



	case 22: // Оружие пришельцев 1
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 0.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.15f;
		ParticleSystem->ParticlesPerSec = 300;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.85f, 1.0f, 0.0f, 0.2f);
		break;
	case 23:	// Оружие пришельцев 2
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.85f, 1.0f, 0.0f, 0.1f);
		break;
	case 24:	// Оружие пришельцев 2
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.60f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.85f, 1.0f, 0.0f, 0.1f);
		break;

	case 25:	// фларес
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 1.00f;
		ParticleSystem->SizeVar    = 0.50f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{4.0f, 4.0f, 4.0f};
		ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare.tga");
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.5f, 0.25f, 0.05f, 0.0f, 0.05f);
		break;
	case 26:	// стрельба 1-го пирата Kinetic
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 0.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.15f;
		ParticleSystem->ParticlesPerSec = 300;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.2f);
		break;
	case 27:	// стрельба 2-го пирата Missile
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.35f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
		break;


	case 28:	// 2-я мина
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 180.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D{0.0f, -1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
		break;
	case 29:	// 3-я мина
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 60.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D{0.0f, -1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.15f);
		break;
	case 30:	// 4-я мина
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 180.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 150;
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{1.5f, 0.1f, 1.5f};
		ParticleSystem->Direction = sVECTOR3D{0.0f, -1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.1f);
		break;
	case 31:	// Swarm для пиратов
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.25f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.2f);
		break;
	case 32:	// Torpedo пиратов
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.35f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 1.0f, 0.15f, 0.0f, 0.075f);
		break;
	case 33:	// Nuke пиратов
		ParticleSystem->ColorStart.r = 0.30f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 250;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.15f, 0.35f, 1.0f, 0.0f, 0.05f);
		break;
	case 34:	// Kinetic2 пиратов
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.15f);
		break;
	case 35:	// Kinetic3 пиратов
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.60f;
		ParticleSystem->SizeVar    = 0.00f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
		break;

	case 36:	// Оружие пришельцев, мина 1 тип
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.01f;
		ParticleSystem->SizeVar    = 0.02f;
		ParticleSystem->SizeEnd    = 0.40f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{2.0, 2.0f, 2.0f};
		ParticleSystem->DeadZone = 1.9f;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 25.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.75f, 1.0f, 0.0f, 0.1f);
		break;

	case 37:	// Оружие пришельцев, мина 2 тип
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.20f;
		ParticleSystem->ColorStart.b = 0.20f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.20f;
		ParticleSystem->ColorEnd.b = 0.20f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.3f;
		ParticleSystem->SizeVar    = 0.02f;
		ParticleSystem->SizeEnd    = 0.1f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{2.0, 0.2f, 2.0f};
		ParticleSystem->DeadZone = 1.9f;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 2.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.2f, 0.2f, 0.0f, 0.1f);
		break;
	case 38:	// Оружие пришельцев, Laser
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.30f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.40f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 9000;
		ParticleSystem->CreationType = eParticleCreationType::Tube;
		ParticleSystem->CreationSize = sVECTOR3D{0.2f, 0.2f, 0.1f};
		ParticleSystem->AlphaShowHide = true;
		break;

	case 101:	// шлейф ракеты землян и пиратов
		ParticleSystem->Texture = GetPreloadedTextureAsset(MissileTrailTextures[vw_iRandNum(4)]);
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.06f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.20f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 2.50f;
		ParticleSystem->Speed      = 0.20f;
		ParticleSystem->SpeedVar   = 0.30f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 5.00f;
		ParticleSystem->LifeVar    = 5.00f;
		ParticleSystem->ParticlesPerSec = 150;
		ParticleSystem->TextureBlend = true;
		break;

	case 102:	// шлейф ракет пришельцев
		ParticleSystem->Texture = GetPreloadedTextureAsset(MissileTrailTextures[vw_iRandNum(4)]);
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.08f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.20f;
		ParticleSystem->SizeVar    = 0.10f;
		ParticleSystem->SizeEnd    = 2.00f;
		ParticleSystem->Speed      = 0.20f;
		ParticleSystem->SpeedVar   = 0.30f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 5.00f;
		ParticleSystem->LifeVar    = 5.00f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->TextureBlend = true;
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong GFXNum.\n";
		break;
	}
}





//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cProjectile::cProjectile(int ProjectileNum)
{
	ObjectStatus = eObjectStatus::Ally;
	ObjectType = eObjectType::Projectile;

	// проверяем на столкновения только по радиусу
	Radius = 0.0f;

	Ambient[0] = Ambient[1] = Ambient[2] = Ambient[3] = 0.35f;

	if (ProjectileNum <= 0) {
		std::cerr << __func__ << "(): " << "Could not init cProjectile object with Number " << ProjectileNum << "\n";
		return;
	} else if ((ProjectileNum >= 1 && ProjectileNum <= 99) && ((unsigned int)ProjectileNum > PresetEarthProjectileData.size())) {
		std::cerr << __func__ << "(): " << "Could not init cProjectile(1) object with Number " << ProjectileNum << "\n";
		return;
	} else if ((ProjectileNum >= 101 && ProjectileNum <= 199) && ((unsigned int)ProjectileNum-100 > PresetAlienProjectileData.size())) {
		std::cerr << __func__ << "(): " << "Could not init cProjectile(2) object with Number " << ProjectileNum << "\n";
		return;
	} else if ((ProjectileNum >= 201 && ProjectileNum <= 299) && ((unsigned int)ProjectileNum-200 > PresetPirateProjectileData.size())) {
		std::cerr << __func__ << "(): " << "Could not init cProjectile(3) object with Number " << ProjectileNum << "\n";
		return;
	}

	Num = ProjectileNum;

	// внутренний номер, номер в таблицах
	int IntNum = ProjectileNum;
	int GraphicFXQuantity{0};


	if (Num >= 1 && Num <= 99) {
		Radius = PresetEarthProjectileData[IntNum-1].Radius;
		DamageHull = PresetEarthProjectileData[IntNum-1].DamageHull;
		DamageSystems = PresetEarthProjectileData[IntNum-1].DamageSystems;
		ProjectileType = PresetEarthProjectileData[IntNum-1].ProjectileType;
		SpeedStart = SpeedEnd = Speed = PresetEarthProjectileData[IntNum-1].Speed;
		Age = Lifetime = PresetEarthProjectileData[IntNum-1].Age;
		GraphicFXQuantity = PresetEarthProjectileData[IntNum-1].GraphicFXQuantity;
	} else if (Num >= 101 && Num <= 199) {
		IntNum = ProjectileNum - 100;
		Radius = PresetAlienProjectileData[IntNum-1].Radius;
		DamageHull = PresetAlienProjectileData[IntNum-1].DamageHull;
		DamageSystems = PresetAlienProjectileData[IntNum-1].DamageSystems;
		ProjectileType = PresetAlienProjectileData[IntNum-1].ProjectileType;
		SpeedStart = SpeedEnd = Speed = PresetAlienProjectileData[IntNum-1].Speed;
		Age = Lifetime = PresetAlienProjectileData[IntNum-1].Age;
		GraphicFXQuantity = PresetAlienProjectileData[IntNum-1].GraphicFXQuantity;
	} else if (Num >= 201 && Num <= 299) {
		IntNum = ProjectileNum - 200;
		// радиус для мин потом изменим при инициализации геометрии
		Radius = PresetPirateProjectileData[IntNum-1].Radius;
		DamageHull = PresetPirateProjectileData[IntNum-1].DamageHull;
		DamageSystems = PresetPirateProjectileData[IntNum-1].DamageSystems;
		ProjectileType = PresetPirateProjectileData[IntNum-1].ProjectileType;
		SpeedStart = SpeedEnd = Speed = PresetPirateProjectileData[IntNum-1].Speed;
		Age = Lifetime = PresetPirateProjectileData[IntNum-1].Age;
		GraphicFXQuantity = PresetPirateProjectileData[IntNum-1].GraphicFXQuantity;
	}



	// начальные установки
	GraphicFXLocation.resize(GraphicFXQuantity, sVECTOR3D{0.0f, 0.0f, 0.0f});
	GraphicFX.resize(GraphicFXQuantity);

	if (ProjectileType == 1) {
		Strength = StrengthStart = 1.0f;
	}


	switch (ProjectileNum) {
	// снаряды землян

	// Kinetic
	case 1:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 1);
		break;
	case 2:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 2);
		break;
	case 3:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 3);
		break;
	case 4:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 4);
		break;
	// Ion
	case 5:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 5);
		break;
	case 6:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 6);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetProjectileGFX(sharedGFX, 5);
			sharedGFX->CreationSize = sVECTOR3D{2.5f, 2.5f, 0.5f};
			sharedGFX->DeadZone = 1.9f;
		}
		break;
	case 7:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 12);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 5);
		break;
	// Plasma
	case 8:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 7);
		break;
	case 9:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 10);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 8);
		GraphicFX[2] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[2].lock())
			SetProjectileGFX(sharedGFX, 9);
		break;
	case 10:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 8);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 9);
		GraphicFX[2] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[2].lock())
			SetProjectileGFX(sharedGFX, 11);
		break;
	// Maser
	case 11:
		ProjectileCenter = sVECTOR3D{0.0f, 0.0f, 50.0f};
		NeedStopPartic = true;
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetProjectileGFX(sharedGFX, 17);
			sharedGFX->CreationSize = sVECTOR3D{0.8f, 0.8f, 100.0f};

			float MinX = -sharedGFX->CreationSize.x/2;
			float MaxX = sharedGFX->CreationSize.x/2;
			float MinY = -sharedGFX->CreationSize.y/2;
			float MaxY = sharedGFX->CreationSize.y/2;
			float MinZ = -sharedGFX->CreationSize.z/2;
			float MaxZ = sharedGFX->CreationSize.z/2;
			// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
			OBB.Box[0] = AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
			OBB.Box[1] = AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
			OBB.Box[2] = AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
			OBB.Box[3] = AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
			OBB.Box[4] = AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
			OBB.Box[5] = AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
			OBB.Box[6] = AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
			OBB.Box[7] = AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
			OBB.Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Width = fabsf(MaxX - MinX);
			Height = fabsf(MaxY - MinY);
			Length = fabsf(MaxZ - MinZ);
		}
		break;
	case 12:
		ProjectileCenter = sVECTOR3D{0.0f, 0.0f, 55.0f};
		NeedStopPartic = true;
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetProjectileGFX(sharedGFX, 18);
			sharedGFX->CreationSize = sVECTOR3D{1.6f, 1.6f, 110.0f};
		}
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetProjectileGFX(sharedGFX, 17);
			sharedGFX->CreationSize = sVECTOR3D{0.8f, 0.8f, 110.0f};

			float MinX = -sharedGFX->CreationSize.x/2;
			float MaxX = sharedGFX->CreationSize.x/2;
			float MinY = -sharedGFX->CreationSize.y/2;
			float MaxY = sharedGFX->CreationSize.y/2;
			float MinZ = -sharedGFX->CreationSize.z/2;
			float MaxZ = sharedGFX->CreationSize.z/2;
			// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
			OBB.Box[0] = AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
			OBB.Box[1] = AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
			OBB.Box[2] = AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
			OBB.Box[3] = AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
			OBB.Box[4] = AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
			OBB.Box[5] = AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
			OBB.Box[6] = AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
			OBB.Box[7] = AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
			OBB.Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Width = fabsf(MaxX-MinX);
			Height = fabsf(MaxY-MinY);
			Length = fabsf(MaxZ-MinZ);
		}
		break;
	// Antimatter
	case 13:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 19);
		break;
	// Laser
	case 14:
		ProjectileCenter = sVECTOR3D{0.0f, 0.0f, 60.0f};
		NeedStopPartic = true;
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetProjectileGFX(sharedGFX, 20);
			sharedGFX->CreationSize = sVECTOR3D{0.4f, 0.4f, 120.0f};

			float MinX = -sharedGFX->CreationSize.x/2;
			float MaxX = sharedGFX->CreationSize.x/2;
			float MinY = -sharedGFX->CreationSize.y/2;
			float MaxY = sharedGFX->CreationSize.y/2;
			float MinZ = -sharedGFX->CreationSize.z/2;
			float MaxZ = sharedGFX->CreationSize.z/2;
			// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
			OBB.Box[0] = AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
			OBB.Box[1] = AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
			OBB.Box[2] = AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
			OBB.Box[3] = AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
			OBB.Box[4] = AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
			OBB.Box[5] = AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
			OBB.Box[6] = AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
			OBB.Box[7] = AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
			OBB.Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Width = fabsf(MaxX-MinX);
			Height = fabsf(MaxY-MinY);
			Length = fabsf(MaxZ-MinZ);
		}
		break;
	// Gauss
	case 15:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 21);
		break;
	// ракета
	case 16:
		LoadObjectData("models/earthfighter/missile.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 13);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// рой
	case 17:
		LoadObjectData("models/earthfighter/swarm.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 16);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetProjectileGFX(sharedGFX, 101);
			sharedGFX->Life = 1.00f; // у роя слишком много ракет, если делать долгий шлейф - может просесть фпс
		}
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// торпеда
	case 18:
		LoadObjectData("models/earthfighter/torpedo.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 14);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// бомба
	case 19:
		LoadObjectData("models/earthfighter/nuke.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 15);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;






	// снаряды пришельцев



	// как Kinetic1
	case 101:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 22);
		break;
	// с наведением, как Kinetic2
	case 102:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 23);
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 102);
		break;
	// как Kinetic3
	case 103:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 24);
		break;
	// с наведением, как Kinetic3
	case 104:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 24);
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 102);
		break;
	// как Kinetic2
	case 105:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 23);
		break;
	// энергетическая мина (1-й тип)
	case 106:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 36);
		break;
	// энергетическая мина (2-й тип)
	case 107:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 37);
		break;
	// как Plasma3
	case 108:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 8);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 9);
		GraphicFX[2] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[2].lock())
			SetProjectileGFX(sharedGFX, 11);
		break;
	// как Plasma2
	case 109:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 10);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 8);
		GraphicFX[2] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[2].lock())
			SetProjectileGFX(sharedGFX, 9);
		break;
	// как Laser
	case 110:
		ProjectileCenter = sVECTOR3D{0.0f, 0.0f, 55.0f};
		NeedStopPartic = true;
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetProjectileGFX(sharedGFX, 38);
			sharedGFX->CreationSize = sVECTOR3D{2.0f, 2.0f, 110.0f};

			float MinX = -sharedGFX->CreationSize.x/2;
			float MaxX = sharedGFX->CreationSize.x/2;
			float MinY = -sharedGFX->CreationSize.y/2;
			float MaxY = sharedGFX->CreationSize.y/2;
			float MinZ = -sharedGFX->CreationSize.z/2;
			float MaxZ = sharedGFX->CreationSize.z/2;
			// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
			OBB.Box[0] = AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
			OBB.Box[1] = AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
			OBB.Box[2] = AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
			OBB.Box[3] = AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
			OBB.Box[4] = AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
			OBB.Box[5] = AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
			OBB.Box[6] = AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
			OBB.Box[7] = AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
			OBB.Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Width = fabsf(MaxX-MinX);
			Height = fabsf(MaxY-MinY);
			Length = fabsf(MaxZ-MinZ);
		}
		break;




	// снаряды пиратов


	// стрельба турели 1
	case 201:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 26);
		break;
	// стрельба турели 2
	case 202:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 34);
		break;
	// фларес
	case 203:
		// смотрит вверх
		Orientation = sVECTOR3D{0.0f, 0.5f, 0.5f};
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 25);
		break;
	// как Kinetic1
	case 204:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 26);
		break;
	// как Missile1
	case 205:
		LoadObjectData("models/earthfighter/missile.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 27);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// как Missile2
	case 206:
		LoadObjectData("models/earthfighter/swarm.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 31);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// как Ion2
	case 207:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 6);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetProjectileGFX(sharedGFX, 5);
			sharedGFX->CreationSize = sVECTOR3D{2.5f, 2.5f, 0.5f};
			sharedGFX->DeadZone = 1.9f;
		}
		break;
	// как Antimatter
	case 208:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 19);
		break;
	// как торпеда
	case 209:
		LoadObjectData("models/earthfighter/torpedo.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 32);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// как бомба
	case 210:
		LoadObjectData("models/earthfighter/nuke.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/earthfighter/rockets.tga");
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 33);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, 0.0f, -Length / 2.0f};
		NeedStopPartic = true;
		// шлейф
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 101);
		GraphicFXLocation[1] = GraphicFXLocation[0];
		break;
	// как Kinetic2
	case 211:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 34);
		break;
	// как Kinetic3
	case 212:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 35);
		break;
	// как Plasma2
	case 213:
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 10);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock())
			SetProjectileGFX(sharedGFX, 8);
		GraphicFX[2] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[2].lock())
			SetProjectileGFX(sharedGFX, 9);
		break;



	// мина1 (простое наведение по высоте)
	case 214:
		MineIData = 0.0f;
		Strength = StrengthStart = 10.0f;
		LoadObjectData("models/mine/mine-01.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/mine/mine1.tga");
		TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine1i.tga");
		break;
	// мина2 (наведение по высоте + приближение)
	case 215:
		MineIData = 0.0f;
		Strength = StrengthStart = 15.0f;
		LoadObjectData("models/mine/mine-02.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/mine/mine2.tga");
		TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine2i.tga");

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 28);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, -0.8f, 0.0f};
		NeedStopPartic = true;
		break;
	// мина3 (наведение по высоте + стрельба снарядами)
	case 216:
		MineIData = 0.0f;
		Strength = StrengthStart = 20.0f;
		MineReloadTime = MineNextFireTime = 1.0f;
		LoadObjectData("models/mine/mine-03.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/mine/mine3.tga");
		TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine3i.tga");

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 29);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, -2.5f, 0.0f};
		NeedStopPartic = true;
		break;
	// мина4 (наведение по высоте + стрельба ракетами)
	case 217:
		MineIData = 0.0f;
		Strength = StrengthStart = 40.0f;
		MineReloadTime = MineNextFireTime = 3.0f;
		LoadObjectData("models/mine/mine-04.vw3d", *this);
		Texture[0] = GetPreloadedTextureAsset("models/mine/mine4.tga");
		TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine4i.tga");

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock())
			SetProjectileGFX(sharedGFX, 30);
		GraphicFXLocation[0] = sVECTOR3D{0.0f, -0.7f, 0.0f};
		NeedStopPartic = true;
		break;





	default:
		std::cerr << __func__ << "(): " << "wrong ProjectileNum.\n";
	}




}

//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cProjectile::~cProjectile()
{
	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		/* this GFX is not in use */
		auto sharedGFX = GraphicFX[i].lock();
		if (!sharedGFX)
			continue;

		/* should destroy GFX right now */
		if (GraphicFXDestroyType) {
			vw_ReleaseParticleSystem(GraphicFX[i]);
			continue;
		}

		/* time to suppress GFX and destroy it at the end */
		sharedGFX->IsSuppressed = true;
		sharedGFX->DestroyIfNoParticles = true;

		/* change speed only */
		if (!NeedStopPartic) {
			sharedGFX->ChangeSpeed(Orientation ^ Speed);
			continue;
		}

		sharedGFX->StopAllParticles();

		// только для ракет землян и пиратов делаем изменение шлейфа при взрыве (только шлейф!, он всегда 2-й эффект)
		if (i != 1)
			continue;

		// для разных типов ракет делаем разную "ударную волну"
		float effective_dist2 = 300;
		switch(Num) {
		case 16:
		case 205:
			effective_dist2 = 500.0f;
			break;
		case 18:
		case 209:
			effective_dist2 = 1000.0f;
			break;
		case 19:
		case 210:
			effective_dist2 = 2000.0f;
			break;
		}

		switch(Num) {
		case 16:
		case 17:
		case 18:
		case 19:
		case 205:
		case 206:
		case 209:
		case 210:
			// on missile impact and explosion we should calculate "shock wave", that
			// should scatter missile's trails and make scene looks more "realistic",
			// since particles data are private, we call ForEachParticle() with lambda
			sharedGFX->ForEachParticle([this, effective_dist2](sVECTOR3D &pLocation,
									   sVECTOR3D &pVelocity,
									   bool &pNeedStop){
				sVECTOR3D Dist2 = pLocation - this->Location;
				float fDist2 = Dist2.x * Dist2.x + Dist2.y * Dist2.y + Dist2.z * Dist2.z;
				if (fDist2 < 1.0f)
					fDist2 = 3.0f;

				if (fDist2 < effective_dist2) {
					pVelocity = sVECTOR3D{Dist2.x + 10.0f * vw_fRand0(),
							      Dist2.y + 10.0f * vw_fRand0(),
							      Dist2.z + 10.0f * vw_fRand0()};
					pVelocity.Normalize();
					pVelocity = pVelocity ^ (effective_dist2 / fDist2);
					pNeedStop = true;
				}
			});
			break;
		}
	}
}











//-----------------------------------------------------------------------------
// Установка положения
//-----------------------------------------------------------------------------
void cProjectile::SetRotation(const sVECTOR3D &NewRotation)
{

	// вызываем родительскую функцию
	cObject3D::SetRotation(NewRotation);


	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		if (auto sharedGFX = GraphicFX[i].lock()) {
			vw_Matrix33CalcPoint(GraphicFXLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(GraphicFXLocation[i], CurrentRotationMat);
			// если лучевое оружие, нужно вращать все, и частицы тоже
			if (ProjectileType == 2)
				sharedGFX->RotateSystemAndParticlesByAngle(Rotation);
			else
				sharedGFX->RotateSystemByAngle(Rotation);
		}
	}

}







//-----------------------------------------------------------------------------
// Установка положения
//-----------------------------------------------------------------------------
void cProjectile::SetLocation(const sVECTOR3D &NewLocation)
{
	// вызываем родительскую функцию
	cObject3D::SetLocation(NewLocation);

	if (GraphicFX.empty())
		return;

	switch (Num) {

	// снаряды землян

	// Kinetic
	case 1:
	case 2:
	case 3:
	case 4:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// Ion
	case 5:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		break;
	case 6:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		break;
	case 7:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		break;
	// Plasma
	case 8:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		break;
	case 9:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		if (auto sharedGFX = GraphicFX[2].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[2] + Location);
		break;
	case 10:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		if (auto sharedGFX = GraphicFX[2].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[2] + Location);
		break;
	// Maser
	case 11:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		break;
	case 12:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		if (auto sharedGFX = GraphicFX[1].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[1] + Location);
		}
		break;
	// Antimatter
	case 13:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		break;
	// Laser
	case 14:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		break;
	// Gauss
	case 15:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		break;
	// Missiles
	case 16:
	case 17:
	case 18:
	case 19:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[1] + Location);
		break;

	// снаряды пришельцев

	// как Kinetic1
	case 101:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// с наведением, как Kinetic2
	case 102:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[1] + Location);
		break;
	case 103:
		// как Kinetic3
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// с наведением, как Kinetic3
	case 104:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[1] + Location);
		break;
	// как Kinetic2
	case 105:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// энергетическая мина (1-й тип)
	case 106:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// энергетическая мина (2-й тип)
	case 107:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// как Plasma3
	case 108:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		if (auto sharedGFX = GraphicFX[2].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[2] + Location);
		break;
	// как Plasma2
	case 109:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		if (auto sharedGFX = GraphicFX[2].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[2] + Location);
		break;
	// как Laser
	case 110:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		break;

	// снаряды пиратов

	// стрельба турели 1
	case 201:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// стрельба турели 2
	case 202:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// фларес
	case 203:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// как Kinetic1
	case 204:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	case 205: // как Missile1
	case 206: // как Missile2
	case 209: // как Missile3
	case 210: // как Missile4
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[1] + Location);
		break;
	// как Ion2
	case 207:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		break;
	// Antimatter
	case 208:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		break;
	// как Kinetic2
	case 211:
	// как Kinetic3
	case 212:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystemLocation(GraphicFXLocation[0] + Location);
		break;
	// как Plasma2
	case 213:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[1] + Location);
		if (auto sharedGFX = GraphicFX[2].lock())
			sharedGFX->MoveSystem(GraphicFXLocation[2] + Location);
		break;

	// снаряды без оружия

	// мины
	case 214:
		break;
	case 215:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		break;
	case 216:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		break;
	case 217:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->MoveSystem(GraphicFXLocation[0] + Location);
			sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
		}
		break;
	}
}








//-----------------------------------------------------------------------------
// выполнение действий
//-----------------------------------------------------------------------------
bool cProjectile::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!cObject3D::Update(Time)) {
		// если лучевое оружие, никогда не уничтожаем его!!!
		// это сделаем в пушке...
		if (ProjectileType == 2)
			return true;
		// передаем на самоуничтожение... там все сделаем правильно
		CreateBulletExplosion(nullptr, this, -Num, Location, Speed);
		return false;
	}

	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f)
		return true;



	Speed = SpeedStart*(Lifetime/Age) + SpeedEnd*((Age-Lifetime)/Age);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Общее перемещение снаряда...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	sVECTOR3D Velocity = Orientation^(Speed*TimeDelta);
	SetLocation(Location+Velocity);




	float CurrentPenalty{1.0f};
	if (ObjectStatus == eObjectStatus::Enemy)
		CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty);



	float RotationSpeed;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Если что-то надо делать со снарядом (наводить к примеру)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch (Num) {
	// Plasma
	case 9:
	case 109:
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->RotateSystemAndParticlesByAngle(sVECTOR3D{sharedGFX->Angle.x - 360.0f * TimeDelta,
									     sharedGFX->Angle.y,
									     sharedGFX->Angle.z});
		if (auto sharedGFX = GraphicFX[2].lock())
			sharedGFX->RotateSystemAndParticlesByAngle(sVECTOR3D{sharedGFX->Angle.x - 360.0f * TimeDelta,
									     sharedGFX->Angle.y,
									     sharedGFX->Angle.z});
		break;
	case 10:
	case 108:
	case 213:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->RotateSystemAndParticlesByAngle(sVECTOR3D{sharedGFX->Angle.x - 360.0f * TimeDelta,
									     sharedGFX->Angle.y,
									     sharedGFX->Angle.z});
		if (auto sharedGFX = GraphicFX[1].lock())
			sharedGFX->RotateSystemAndParticlesByAngle(sVECTOR3D{sharedGFX->Angle.x - 360.0f * TimeDelta,
									     sharedGFX->Angle.y,
									     sharedGFX->Angle.z});
		break;

	// Maser
	case 11:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->RotateParticlesByAngle(sVECTOR3D{0.0f, 0.0f, 360.0f * TimeDelta * 3.0f});
			if (Lifetime <= sharedGFX->Life / 1.5f)
				sharedGFX->IsSuppressed = true;
		}
		break;
	case 12:
		if (auto sharedGFX = GraphicFX[0].lock()) {
			sharedGFX->RotateParticlesByAngle(sVECTOR3D{0.0f, 0.0f, 360.0f * TimeDelta * 4.0f});
			if (Lifetime <= sharedGFX->Life / 1.5f)
				sharedGFX->IsSuppressed = true;
		}
		if (auto sharedGFX = GraphicFX[1].lock()) {
			sharedGFX->RotateParticlesByAngle(sVECTOR3D{0.0f, 0.0f, -360.0f * TimeDelta * 2.0f});
			if (Lifetime <= sharedGFX->Life / 1.5f)
				sharedGFX->IsSuppressed = true;
		}
		break;

	// Antimatter
	case 13:
	case 208:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->RotateSystemAndParticlesByAngle(sVECTOR3D{sharedGFX->Angle.x,
									     sharedGFX->Angle.y - 360.0f * TimeDelta * 2.0f,
									     sharedGFX->Angle.z});
		break;

	// Laser
	case 14:
	case 110:
		if (auto sharedGFX = GraphicFX[0].lock())
			if (Lifetime <= sharedGFX->Life / 1.5f)
				sharedGFX->IsSuppressed = true;
		break;

	// Gauss
	case 15:
		if (auto sharedGFX = GraphicFX[0].lock())
			sharedGFX->RotateParticlesByAngle(sVECTOR3D{0.0f, 0.0f, 360.0f * TimeDelta * 6.0f});
		break;




	// ракеты



	// ракеты землян
	case 16:
		RotationSpeed = 50.0f;
		goto missile;
	case 17:
		RotationSpeed = 60.0f;
		goto missile;
	case 18:
		RotationSpeed = 35.0f;
		goto missile;
	case 19:
		RotationSpeed = 20.0f;
		goto missile;
	// ракеты пришельцев
	case 102:
		RotationSpeed = 50.0f;
		goto missile;
	case 104:
		RotationSpeed = 60.0f;
		goto missile;
	// ракеты пиратов
	case 205:
		RotationSpeed = 50.0f;
		goto missile;
	case 206:
		RotationSpeed = 60.0f;
		goto missile;
	case 209:
		RotationSpeed = 50.0f;
		goto missile;
	case 210:
		RotationSpeed = 30.0f;
		{
missile:
			RotationSpeed /= CurrentPenalty;

			sVECTOR3D NeedAngle = Rotation;

			float EffectiveRange = 1000000.0f;
			// только для ракет игрока учитываем максимальную дальность полета
			if (ObjectStatus == eObjectStatus::Player)
				EffectiveRange = Lifetime * Speed;

			// устанавливаем в Target на что наведен этот снаряд, если еще ничего не выбрано
			if (Target.expired()) {
				Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, NeedAngle, EffectiveRange);
			} else { // если уже что-то выбрали
				// 1. надо проверить, есть ли еще вообще этот объект и где по отношению ракеты он находится, если он есть и впереди - летим к нему
				if (GetMissileTargetStatus(Target, Location, CurrentRotationMat)) {
					// получаем углы, возвращает false - значит цель уже проскочили, и надо навестись на новую
					if (!GetMissileOnTargetOrientateion(Location, Rotation, CurrentRotationMat, Target, NeedAngle))
						Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, NeedAngle, EffectiveRange);
				}
				// 2. если объекта нет (уже взорвали) или мы его уже проскочили, надо наводить на другой
				else {
					Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, NeedAngle, EffectiveRange);
				}
			}



			// учитываем скорость поворота по вертикали
			if (Rotation.y < NeedAngle.y) {
				float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
				if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;

			}
			if (Rotation.y > NeedAngle.y) {
				float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
				if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;
			}

			if (Rotation.x < NeedAngle.x) {
				float NeedAngle_x = Rotation.x+RotationSpeed*TimeDelta;
				if (NeedAngle_x > NeedAngle.x) NeedAngle_x = NeedAngle.x;
				NeedAngle.x = NeedAngle_x;

			}
			if (Rotation.x > NeedAngle.x) {
				float NeedAngle_x = Rotation.x-RotationSpeed*TimeDelta;
				if (NeedAngle_x < NeedAngle.x) NeedAngle_x = NeedAngle.x;
				NeedAngle.x = NeedAngle_x;
			}


			sVECTOR3D TTT = NeedAngle-Rotation;
			SetRotation(TTT);

		}
		break;



	// мины



	// мина пиратов (1-й тип)
	case 214:
		MineIData += TimeDelta;
		if (MineIData >= 0.1f) {
			if (!TextureIllum[0])
				TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine1i.tga");
			else
				TextureIllum[0] = 0;
			MineIData = 0.0f;
		}
		Chunks[0].Rotation.y += 90.0f * TimeDelta;
		while (Chunks[0].Rotation.y > 360.0f) {
			Chunks[0].Rotation.y -= 360.0f;
		}
		// wip, disabled for now
	break;
	// мина пришельцев, энергетическая (1-й тип)
	case 106: {
		// получаем положение ближайшего врага
		std::weak_ptr<cObject3D> tmpTarget = GetCloserTargetPosition(ObjectStatus, Location);

		// !!! не учитываем положение плоскости
		auto sharedTarget = tmpTarget.lock();
		if (sharedTarget) {
			float MineSpeed = 5.0f;
			float SpeedTmp = MineSpeed*TimeDelta;
			if (SpeedTmp > fabs(Location.y - sharedTarget->Location.y))
				SpeedTmp = fabs(Location.y - sharedTarget->Location.y);

			if (SpeedTmp != 0.0f) {
				// находим направление (если нужно вниз, меняем знак)
				if (Location.y > sharedTarget->Location.y) SpeedTmp *= -1.0f;

				sVECTOR3D VelocityUp = sVECTOR3D{0.0f, SpeedTmp, 0.0f};
				SetLocation(Location+VelocityUp);
			}
		}

		if (!GraphicFX.empty())
			if (auto sharedGFX = GraphicFX[0].lock())
				sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);
	}
	break;




	// мина пиратов (2-й тип)
	case 215:
		MineIData += TimeDelta;
		if (MineIData >= 0.1f) {
			if (!TextureIllum[0])
				TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine2i.tga");
			else
				TextureIllum[0] = 0;
			MineIData = 0.0f;
		}
		Chunks[0].Rotation.y += 90.0f * TimeDelta;
		while (Chunks[0].Rotation.y > 360.0f) {
			Chunks[0].Rotation.y -= 360.0f;
		}
		// wip, disabled for now
	break;
	// мина пришельцев, энергетическая (2-й тип)
	case 107:
		RotationSpeed = 180.0f;
		{
			sVECTOR3D NeedAngle = Rotation;
			// устанавливаем в Target на что наведен этот снаряд
			Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, NeedAngle, 1000000);


			// учитываем скорость поворота по вертикали
			if (Rotation.y < NeedAngle.y) {
				float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
				if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;

			}
			if (Rotation.y > NeedAngle.y) {
				float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
				if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;
			}

			NeedAngle.z = Rotation.z;
			NeedAngle.x = Rotation.x;

			sVECTOR3D TTT = NeedAngle-Rotation;
			SetRotation(TTT);


			// если есть цель, поднимаемся на ее уровень
			auto sharedTarget = Target.lock();
			if (sharedTarget) {
				float MineSpeed = 5.0f;

				// получаем положение ближайшего врага
				sVECTOR3D NeedPoint = sharedTarget->Location;

				// !!! не учитываем положение плоскости

				float SpeedTmp = MineSpeed*TimeDelta;
				if (SpeedTmp > fabs(Location.y-NeedPoint.y))
					SpeedTmp = fabs(Location.y-NeedPoint.y);

				if (SpeedTmp != 0.0f) {
					// находим направление (если нужно вниз, меняем знак)
					if (Location.y > NeedPoint.y) SpeedTmp *= -1.0f;

					sVECTOR3D VelocityUp = sVECTOR3D{0.0f, SpeedTmp, 0.0f};
					SetLocation(Location+VelocityUp);
				}
			}

			if (!GraphicFX.empty())
				if (auto sharedGFX = GraphicFX[0].lock())
					sharedGFX->SetStartLocation(GraphicFXLocation[0] + Location);

			// сбрасываем установку, чтобы не было голосового предупреждения
			Target.reset();
		}
		break;



	case 216:
		MineIData += TimeDelta;
		if (MineIData >= 0.1f) {
			if (!TextureIllum[0])
				TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine3i.tga");
			else
				TextureIllum[0] = 0;
			MineIData = 0.0f;
		}
		Chunks[0].Rotation.y += 120.0f * TimeDelta;
		while (Chunks[0].Rotation.y > 360.0f) {
			Chunks[0].Rotation.y -= 360.0f;
		}

		RotationSpeed = 180.0f;
		{
			sVECTOR3D NeedAngle = Rotation;
			// устанавливаем в Target на что наведен этот снаряд
			Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, NeedAngle, 1000000);


			// учитываем скорость поворота по вертикали
			if (Rotation.y < NeedAngle.y) {
				float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
				if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;

			}
			if (Rotation.y > NeedAngle.y) {
				float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
				if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;
			}

			NeedAngle.z = Rotation.z;
			NeedAngle.x = Rotation.x;

			sVECTOR3D TTT = NeedAngle-Rotation;
			SetRotation(TTT);


			// если есть цель, поднимаемся на ее уровень
			auto sharedTarget = Target.lock();
			if (sharedTarget) {
				float MineSpeed = 5.0f;

				// получаем положение ближайшего врага
				sVECTOR3D NeedPoint = sharedTarget->Location;

				// !!! не учитываем положение плоскости

				float SpeedTmp = MineSpeed*TimeDelta;
				if (SpeedTmp > fabs(Location.y-2.0f-NeedPoint.y)) SpeedTmp = fabs(Location.y-2.0f-NeedPoint.y);

				if (SpeedTmp != 0.0f) {
					// находим направление (если нужно вниз, меняем знак)
					if (Location.y-2.0f > NeedPoint.y) SpeedTmp *= -1.0f;

					sVECTOR3D VelocityUp = sVECTOR3D{0.0f, SpeedTmp, 0.0f};
					SetLocation(Location+VelocityUp);
				}

				// стрельба
				MineNextFireTime -= TimeDelta;
				if (MineNextFireTime <= 0.0f) {
					// создаем снаряд
					cProjectile *Projectile = new cProjectile{204};
					Projectile->SetLocation(Location + sVECTOR3D{0.0f, -2.0f, 0.0f});

					Projectile->SetRotation(Rotation);
					for (auto &tmpGFX : Projectile->GraphicFX) {
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


					MineNextFireTime = MineReloadTime;
				}

			}


			// сбрасываем установку, чтобы не было голосового предупреждения
			Target.reset();
		}
		break;


	case 217:
		MineIData += TimeDelta;
		if (MineIData >= 0.1f) {
			if (!TextureIllum[0])
				TextureIllum[0] = GetPreloadedTextureAsset("models/mine/mine4i.tga");
			else
				TextureIllum[0] = 0;
			MineIData = 0.0f;
		}
		Chunks[0].Rotation.y += 90.0f * TimeDelta;
		while (Chunks[0].Rotation.y > 360.0f) {
			Chunks[0].Rotation.y -= 360.0f;
		}

		RotationSpeed = 180.0f;
		{
			sVECTOR3D NeedAngle = Rotation;
			// устанавливаем в Target на что наведен этот снаряд
			Target = GetMissileOnTargetOrientateion(ObjectStatus, Location, Rotation, CurrentRotationMat, NeedAngle, 1000000);


			// учитываем скорость поворота по вертикали
			if (Rotation.y < NeedAngle.y) {
				float NeedAngle_y = Rotation.y+RotationSpeed*TimeDelta;
				if (NeedAngle_y > NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;

			}
			if (Rotation.y > NeedAngle.y) {
				float NeedAngle_y = Rotation.y-RotationSpeed*TimeDelta;
				if (NeedAngle_y < NeedAngle.y) NeedAngle_y = NeedAngle.y;
				NeedAngle.y = NeedAngle_y;
			}

			NeedAngle.z = Rotation.z;
			NeedAngle.x = Rotation.x;

			sVECTOR3D TTT = NeedAngle-Rotation;
			SetRotation(TTT);


			// если есть цель, поднимаемся на ее уровень
			auto sharedTarget = Target.lock();
			if (sharedTarget) {
				float MineSpeed = 5.0f;

				// получаем положение ближайшего врага
				sVECTOR3D NeedPoint = sharedTarget->Location;

				// !!! не учитываем положение плоскости

				float SpeedTmp = MineSpeed*TimeDelta;
				if (SpeedTmp > fabs(Location.y+2.0f-NeedPoint.y)) SpeedTmp = fabs(Location.y+2.0f-NeedPoint.y);

				if (SpeedTmp != 0.0f) {
					// находим направление (если нужно вниз, меняем знак)
					if (Location.y+2.0f > NeedPoint.y) SpeedTmp *= -1.0f;

					sVECTOR3D VelocityUp = sVECTOR3D{0.0f, SpeedTmp, 0.0f};
					SetLocation(Location+VelocityUp);
				}

				// стрельба
				MineNextFireTime -= TimeDelta;
				if (MineNextFireTime <= 0.0f) {
					// создаем снаряд
					cProjectile *Projectile = new cProjectile{206};
					Projectile->SetLocation(Location + sVECTOR3D{0.0f, 0.0f, 0.0f});

					Projectile->SetRotation(Rotation);
					for (auto &tmpGFX : Projectile->GraphicFX) {
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


					MineNextFireTime = MineReloadTime;
				}

			}


			// сбрасываем установку, чтобы не было голосового предупреждения
			Target.reset();
		}
		break;
	}




	// объект в порядке - удалять не нужно
	return true;
}

} // astromenace namespace
} // viewizard namespace
