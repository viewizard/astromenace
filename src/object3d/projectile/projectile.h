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

// TODO split to projectile/swarm/beam

// TODO split to earth/pirate/alien

// TODO switch to enumeration for projectile type (ProjectileType)
//      split flare and alien's mine

#ifndef OBJECT3D_PROJECTILE_PROJECTILE_H
#define OBJECT3D_PROJECTILE_PROJECTILE_H

#include "../object3d.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eProjectileCycle {
	Continue,
	Break,
	DeleteObjectAndContinue,
	DeleteObjectAndBreak
};

enum class eProjectilePairCycle {
	Continue,
	DeleteFirstObjectAndContinue,
	DeleteSecondObjectAndContinue,
	DeleteBothObjectsAndContinue
};

class cProjectile final : public cObject3D {
	friend std::weak_ptr<cProjectile> CreateProjectile(const int ProjectileNum);

private:
	// Don't allow direct new/delete usage in code, only CreateProjectile()
	// allowed for cProjectile creation and release setup (deleter must be provided).
	explicit cProjectile(const int ProjectileNum);
	~cProjectile();

public:
	virtual bool Update(float Time) override;
	virtual void SetRotation(const sVECTOR3D &NewRotation) override;
	virtual void SetLocation(const sVECTOR3D &NewLocation) override;

	int Num{0}; // internal id (same as creation number ProjectileNum)

	float DamageHull{0.0f};
	float DamageSystems{0.0f};

	// 0 - projectile
	// 1 - projectile with 3d model
	// 2 - beam
	// 3 - flares / energy mine (aliens)
	// 4 - mine with 3d model
	int ProjectileType{0};

	// target for himing missile/mine
	std::weak_ptr<cObject3D> Target{};

	// projectile center for beam, since we need correct it with weapon rotation
	sVECTOR3D ProjectileCenter{0.0f, 0.0f, 0.0f};

	float Speed{0.0f};
	float SpeedStart{0.0f};
	float SpeedEnd{0.0f};

	float Age{0.0f};

	bool NeedDeadSound{true};

	// texture illumination in mine 3d model
	float MineIData{0.0f};

	float MineReloadTime{1.0f};
	float MineNextFireTime{1.0f};

	// gfx
	bool GraphicFXDestroyType{false};
	std::vector<sVECTOR3D> GraphicFXLocation{};
	std::vector<std::weak_ptr<cParticleSystem>> GraphicFX{};
	bool NeedStopPartic{false};
};


// Create cProjectile object.
std::weak_ptr<cProjectile> CreateProjectile(const int ProjectileNum);
// Update and remove (erase) dead objects.
void UpdateAllProjectile(float Time);
// Draw all objects.
void DrawAllProjectiles(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular projectile object.
void ReleaseProjectile(std::weak_ptr<cProjectile> &Object);
// Release all objects.
void ReleaseAllProjectiles();
// Cycle for each projectile.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachProjectile(std::function<void (cProjectile &Object)> function);
// Managed cycle for each projectile.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachProjectile(std::function<void (cProjectile &Object, eProjectileCycle &Command)> function);
// Managed cycle for each projectile pair.
// Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
void ForEachProjectilePair(std::function<void (cProjectile &FirstObject,
					       cProjectile &SecondObject,
					       eProjectilePairCycle &Command)> function);
// Get object ptr by reference.
std::weak_ptr<cObject3D> GetProjectilePtr(const cProjectile &Object);

// Get projectile fly range.
float GetProjectileRange(int Num);
// Get projectile hull (main) damage.
int GetProjectileHullDamage(int Num);
// Get projectile system (internal) damage.
int GetProjectileSystemsDamage(int Num);
// Get projectile speed.
float GetProjectileSpeed(int Num);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_PROJECTILE_PROJECTILE_H
