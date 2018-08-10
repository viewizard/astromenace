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

// TODO split to internal/external->turret

// TODO split to earth/pirate/alien

// TODO switch to enumeration for weapon type

#ifndef OBJECT3D_WEAPON_WEAPON_H
#define OBJECT3D_WEAPON_WEAPON_H

#include "../object3d.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eGameSFX;
class cProjectile;

class cWeapon final : public cObject3D {
	friend std::weak_ptr<cWeapon> CreateWeapon(const int WeaponNum);

private:
	// Don't allow direct new/delete usage in code, only CreateWeapon()
	// allowed for cWeapon creation and release setup (deleter must be provided).
	explicit cWeapon(const int WeaponNum);
	~cWeapon();

public:
	virtual bool Update(float Time) override;
	virtual bool WeaponFire(float Time);
	virtual void SetRotation(const sVECTOR3D &NewRotation) override;
	virtual void SetLocation(const sVECTOR3D &NewLocation) override;

	int WeaponLevel{1}; // by power required (by slot)

	int Ammo{100};
	int AmmoStart{100};

	float NextFireTime{0.3f}; // fire rate
	float LastFireTime{-10.0f};

	float EnergyUse{1.0f}; // energy consumption
	float CurrentEnergyAccumulated{0.0f}; // current capacitor status

	int SwarmNum{0}; // need this for swarm missiles and flares

	bool NeedRotateOnTargeting{true};

	std::weak_ptr<cProjectile> LaserMaser{}; // beam
	unsigned int LaserMaserSoundNum{0};
	bool WeaponTurret{false}; // this object is turret

	sVECTOR3D BaseBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D MiddleBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D WeaponBound{0.0f, 0.0f, 0.0f};

	int TargetHorizChunkNum{-1};
	float TargetHorizChunkCurrentAngle{0.0f};
	float TargetHorizChunkNeedAngle{0.0f};
	int TargetVertChunkNum{-1};
	float TargetVertChunkMaxAngle{80.0f};
	float TargetVertChunkMinAngle{0.0f};
	float TargetVertChunkCurrentAngle{0.0f};
	float TargetVertChunkNeedAngle{0.0f};

	std::weak_ptr<cParticleSystem> Fire{};
	sVECTOR3D FireLocation{0.0f, 0.0f, 0.0f};
	eGameSFX SFX{static_cast<eGameSFX>(0)}; // initialized to 0, eGameSFX::none

	std::weak_ptr<cParticleSystem> DestroyedFire{};
	std::weak_ptr<cParticleSystem> DestroyedSmoke{};
	sVECTOR3D DestroyedFireLocation{0.0f, 0.0f, 0.0f};
};


// Create cWeapon object.
std::weak_ptr<cWeapon> CreateWeapon(const int WeaponNum);
// Update and remove (erase) dead objects.
void UpdateAllWeapon(float Time);
// Draw all objects.
void DrawAllWeapons(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular weapon object.
void ReleaseWeapon(std::weak_ptr<cWeapon> &Object);
// Release particular weapon object during update cycle.
void ReleaseWeaponLazy(std::weak_ptr<cWeapon> &Object);
// Release all objects.
void ReleaseAllWeapons();

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_WEAPON_WEAPON_H
