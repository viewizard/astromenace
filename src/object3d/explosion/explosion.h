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

// TODO switch to enumerations

// TODO correct hierarchy, explosion to "dust" and explosion to "debris" should be separated

#ifndef OBJECT3D_EXPLOSION_EXPLOSION_H
#define OBJECT3D_EXPLOSION_EXPLOSION_H

#include "../object3d.h"
#include "../projectile/projectile.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

class cGroundObject;

struct sExplosionPiece {
	sVECTOR3D Velocity{};
	float RemainTime{0.0f};
};

class cExplosion : public cObject3D {
protected:
	// don't allow object of this class creation
	cExplosion();
	virtual ~cExplosion();

public:
	virtual bool Update(float Time) override;

	int ExplosionType{0};
	int ExplosionTypeByClass{0};

	// std::unique_ptr, we need only memory allocation without container's features
	// don't use std::vector here, since it allocates AND value-initializes
	std::unique_ptr<sExplosionPiece[]> ExplosionPieceData{};

	// NOTE this is part of non shader geometry calculation
	float ExplosionGeometryMoveLastTime{-1};

	sVECTOR3D VelocityOrientation{0.0f, 0.0f, 0.0f};
	float OldSpeed{0.0f};
	bool NeedStop{true};

	float AABBResizeSpeed{0.0f};

	std::vector<std::weak_ptr<cParticleSystem>> GraphicFX{};
};

class cBulletExplosion final : public cExplosion {
	friend void CreateBulletExplosion(const cObject3D *Object, cProjectile &Projectile,
					  int ExplType, const sVECTOR3D &ExplLocation,
					  float Speed, bool NeedExplosionSFX);
private:
	// Don't allow direct new/delete usage in code, only CreateBulletExplosion()
	// allowed for cBulletExplosion creation and release setup (deleter must be provided).
	explicit cBulletExplosion(const cObject3D *Object, cProjectile &Projectile,
				  int ExplType, const sVECTOR3D &ExplLocation,
				  float Speed, bool NeedExplosionSFX);
	~cBulletExplosion() = default;
};

class cGroundExplosion final : public cExplosion {
	friend void CreateGroundExplosion(cGroundObject &Object, int ExplType,
					  const sVECTOR3D &ExplLocation,
					  int ObjectChunkNum, bool NeedExplosionSFX);
private:
	// Don't allow direct new/delete usage in code, only CreateGroundExplosion()
	// allowed for cGroundExplosion creation and release setup (deleter must be provided).
	explicit cGroundExplosion(cGroundObject &Object, int ExplType, const sVECTOR3D &ExplLocation,
				  int ObjectChunkNum, bool NeedExplosionSFX);
	~cGroundExplosion() = default;
};

class cSpaceExplosion final : public cExplosion {
	friend void CreateSpaceExplosion(cObject3D &Object, int ExplType,
					 const sVECTOR3D &ExplLocation, float Speed,
					 int ObjectChunkNum, bool NeedExplosionSFX);
private:
	// Don't allow direct new/delete usage in code, only CreateSpaceExplosion()
	// allowed for cSpaceExplosion creation and release setup (deleter must be provided).
	explicit cSpaceExplosion(cObject3D &Object, int ExplType,
				 const sVECTOR3D &ExplLocation, float Speed,
				 int ObjectChunkNum, bool NeedExplosionSFX);
	~cSpaceExplosion() = default;
};


// Create cBulletExplosion object.
void CreateBulletExplosion(const cObject3D *Object, cProjectile &Projectile,
			   int ExplType, const sVECTOR3D &ExplLocation,
			   float Speed, bool NeedExplosionSFX = true);
// Create cGroundExplosion object.
void CreateGroundExplosion(cGroundObject &Object, int ExplType,
			   const sVECTOR3D &ExplLocation,
			   int ObjectChunkNum, bool NeedExplosionSFX = true);
// Create cSpaceExplosion object.
void CreateSpaceExplosion(cObject3D &Object, int ExplType,
			  const sVECTOR3D &ExplLocation, float Speed,
			  int ObjectChunkNum, bool NeedExplosionSFX = true);
// Update and remove (erase) expired explosions.
void UpdateAllExplosion(float Time);
// Draw all explosions.
void DrawAllExplosions(bool VertexOnlyPass);
// Release all explosions.
void ReleaseAllExplosions();
// Setup explosion gfx.
void SetExplosionGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXNum);
// Play sfx for bullet explosion.
void PlayBulletExplosionSFX(const sVECTOR3D &Location, bool NeedExplosionSFX, int ExplType);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_EXPLOSION_EXPLOSION_H
