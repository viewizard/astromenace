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

// TODO should be separated for explosion with geometry and without (see InternalExplosionType)

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "explosion.h"
#include "../projectile/projectile.h"
#include "../../config/config.h"
#include "../../assets/audio.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, use 'include' instead
void GameCameraSetExplosion(sVECTOR3D Location, float Power);


/*
 * Play sfx for bullet explosion.
 */
void PlayBulletExplosionSFX(const sVECTOR3D &Location, bool NeedExplosionSFX, int ExplType)
{
	if (NeedExplosionSFX) {
		float fVol = 1.0f;

		// use same ID as for projectile creation
		// negative value mean self-destruct

		switch (ExplType) {
		// earth missiles
		case -16:
		case -17:
		case -18:
		case -19:
		// pirate missiles
		case -205:
		case -206:
		case -209:
		case -210:
		// pirate mines
		case -214:
		case -215:
		case -216:
		case -217:
			PlayGameSFX(eGameSFX::Explosion_Medium, fVol, Location);
			break;

		// Kinetic
		case 1:
		case 2:
		case 3:
		case 4:
		// pirate (turrets + Kinetic1)
		case 201:
		case 202:
		case 204:
		case 211:
		case 212:
			PlayGameSFX(eGameSFX::Hit_Kinetic, fVol, Location);
			break;

		// Ion
		case 5:
		case 6:
		case 7:
		// aliens (Kinetic1)
		case 101:
		// pirate (Ion2)
		case 207:
			PlayGameSFX(eGameSFX::Hit_Ion, fVol, Location);
			break;

		// Plasma
		case 8:
		case 9:
		case 10:
		// aliens (Kinetic2, Kinetic3)
		case 102:
		case 103:
		case 104:
		case 105:
		// aliens (Plasma3, Plasma2)
		case 108:
		case 109:
		// pirate (Plasma2)
		case 213:
			PlayGameSFX(eGameSFX::Hit_Plasma, fVol, Location);
			break;

		// Antimatter
		case 13:
		// aliens (mine 1)
		case 106:
		// aliens (mine 2)
		case 107:
		// pirate (Antimatter)
		case 208:
			PlayGameSFX(eGameSFX::Hit_Antimatter, fVol, Location);
			break;

		// Gauss
		case 15:
			PlayGameSFX(eGameSFX::Hit_Gauss, fVol, Location);
			break;

		// earth missile
		case 16:
		// pirate missile
		case 205:
		// pirate mines
		case 214:
		case 215:
		case 216:
		case 217:
			PlayGameSFX(eGameSFX::Explosion_Medium, fVol, Location);
			break;

		// earth missile swarm
		case 17:
		// pirate small missile
		case 206:
			PlayGameSFX(eGameSFX::Explosion_Medium, fVol, Location);
			break;

		// earth torpedo
		case 18:
		// pirate torpedo
		case 209:
			PlayGameSFX(eGameSFX::Explosion_Big, fVol, Location, 2);
			break;

		// earth bomb
		case 19:
		// pirate bomb
		case 210:
			PlayGameSFX(eGameSFX::Explosion_Big_Energy, fVol, Location, 2);
			break;

		default:
			break;
		}
	}
}

/*
 * Constructor.
 */
cBulletExplosion::cBulletExplosion(const cObject3D *Object, cProjectile &Projectile, int ExplType,
				   const sVECTOR3D &ExplLocation, float Speed, bool NeedExplosionSFX)
{
	TimeLastUpdate = Projectile.TimeLastUpdate;
	ExplosionTypeByClass = 2;
	int InternalExplosionType = 0;

	AABB[0] = Projectile.AABB[0];
	AABB[1] = Projectile.AABB[1];
	AABB[2] = Projectile.AABB[2];
	AABB[3] = Projectile.AABB[3];
	AABB[4] = Projectile.AABB[4];
	AABB[5] = Projectile.AABB[5];
	AABB[6] = Projectile.AABB[6];
	AABB[7] = Projectile.AABB[7];

	SetLocation(Projectile.Location);
	Orientation = Projectile.Orientation;
	ObjectStatus = Projectile.ObjectStatus;
	ExplosionType = ExplType;
	VelocityOrientation = Projectile.Orientation ^ (-1);
	OldSpeed = Speed;

	switch (ExplType) {
	// negative - self destruct
	case -16: // missle
	case -17: // missle
	case -18: // missle
	case -19: // missle
	case -205: // pirate missle
	case -206: // pirate missle
	case -209: // pirate missle
	case -210: // pirate missle
	case -214: // pirate mine 1
	case -215: // pirate mine 2
	case -216: // pirate mine 3
	case -217: { // pirate mine 4
		VelocityOrientation = Projectile.Orientation;
		OldSpeed = Projectile.Speed - 0.5f * Projectile.Radius;
		Lifetime = 2.0f;

		GraphicFX.resize(1);
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 1);
			sharedGFX->Speed = 0.5f * Projectile.Radius;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->SetStartLocation(Projectile.Location);
			if (Projectile.Speed != 0.0f)
				sharedGFX->Theta = 360.00f;
			sharedGFX->ParticlesPerSec = (int)(30 * Projectile.Radius);
			sharedGFX->CreationType = eParticleCreationType::Sphere;
			sharedGFX->CreationSize = sVECTOR3D{AABB[0].x, AABB[0].y, AABB[0].z};
			sharedGFX->RotateSystemAndParticlesByAngle(Projectile.Rotation);
		}

		int ttt = (int)(3 * Projectile.Radius) + (int)(vw_fRand0() * 3 * Projectile.Radius);
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{360.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									360.0f * vw_fRand0()});
				sVECTOR3D TM1 = Projectile.Orientation ^ Projectile.Speed;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^
								       (Projectile.Radius * 6.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = 1.5f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->Speed = Projectile.Speed + Projectile.Radius * 1.5f + 2.0f * vw_fRand0();
				sharedProjectile->SpeedEnd = 0.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed;
				sharedProjectile->Lifetime = Projectile.Age = 2.0f + vw_fRand0();
			}
		}
		InternalExplosionType = 2;
	}
	break;

	case 101: // alien
	case 102: // alien
	case 103: // alien
	case 104: // alien
	case 105: // alien
	case 203: // flares (anti-missile)
	case 204: // pirate (Kinetic1)
	case 211: // pirate (Kinetic2)
	case 212: // pirate (Kinetic3)
	case 1: // Kinetic1
	case 2: // Kinetic2
	case 3: // Kinetic3
	case 4: // Kinetic4
	case 13: // Antimatter
	case 106: // alien (energy mine 1)
	case 107: // alien (energy mine 2)
	case 208: // pirate (Antimatter)
	case 15: // Gauss
		if (Object != nullptr)
			VelocityOrientation = Object->Orientation;
		OldSpeed = Speed;
		Lifetime = 0.5f;

		if (!Projectile.GraphicFX.empty() && !Projectile.GraphicFX[0].expired()) {
			GraphicFX.resize(1);
			GraphicFX[0] = vw_CreateParticleSystem();
			auto sharedGFX = GraphicFX[0].lock();
			auto sharedProjectileGFX = Projectile.GraphicFX[0].lock();
			if (sharedGFX && sharedProjectileGFX) {
				SetExplosionGFX(sharedGFX, 0);
				float Rexpl = (sharedProjectileGFX->ColorStart.r + sharedProjectileGFX->ColorEnd.r) / 2.0f;
				float Gexpl = (sharedProjectileGFX->ColorStart.g + sharedProjectileGFX->ColorEnd.g) / 2.0f;
				float Bexpl = (sharedProjectileGFX->ColorStart.b + sharedProjectileGFX->ColorEnd.b) / 2.0f;
				sharedGFX->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
								       Rexpl, Gexpl, Bexpl, 0.0f, 0.005f);

				sharedGFX->ColorStart.r = sharedProjectileGFX->ColorStart.r;
				sharedGFX->ColorStart.g = sharedProjectileGFX->ColorStart.g;
				sharedGFX->ColorStart.b = sharedProjectileGFX->ColorStart.b;
				sharedGFX->ColorEnd.r = sharedProjectileGFX->ColorEnd.r;
				sharedGFX->ColorEnd.g = sharedProjectileGFX->ColorEnd.g;
				sharedGFX->ColorEnd.b = sharedProjectileGFX->ColorEnd.b;
				sharedGFX->Speed = 150.0f;
				sharedGFX->ParticlesPerSec = sharedProjectileGFX->ParticlesPerSec;
				sharedGFX->SizeStart = sharedProjectileGFX->SizeStart;
				sharedGFX->SizeEnd = sharedProjectileGFX->SizeEnd;
				sharedGFX->SizeVar = sharedProjectileGFX->SizeVar;
				sharedGFX->Life = Lifetime;
				sharedGFX->NeedStop = false;
				sharedGFX->SetStartLocation(ExplLocation);
			}
		}
		Projectile.NeedStopPartic = true;
		NeedStop = false;
		break;

	case 5: // Ion1
	case 6: // Ion2
	case 7: // Ion3
	case 207:// pirate (Ion2)
	case 8: // Plasma
	case 9: // Plasma
	case 10: // Plasma
	case 108: // alien (Plasma3)
	case 109: // alien (Plasma2)
	case 213: // pirate (Plasma2)
		if (Object != nullptr)
			VelocityOrientation = Object->Orientation;
		OldSpeed = Speed;
		Lifetime = 0.5f;

		if (!Projectile.GraphicFX.empty() && !Projectile.GraphicFX[0].expired()) {
			GraphicFX.resize(1);
			GraphicFX[0] = vw_CreateParticleSystem();
			auto sharedGFX = GraphicFX[0].lock();
			auto sharedProjectileGFX = Projectile.GraphicFX[0].lock();
			if (sharedGFX && sharedProjectileGFX) {
				SetExplosionGFX(sharedGFX, 0);
				float Rexpl = (sharedProjectileGFX->ColorStart.r + sharedProjectileGFX->ColorEnd.r) / 2.0f;
				float Gexpl = (sharedProjectileGFX->ColorStart.g + sharedProjectileGFX->ColorEnd.g) / 2.0f;
				float Bexpl = (sharedProjectileGFX->ColorStart.b + sharedProjectileGFX->ColorEnd.b) / 2.0f;
				sharedGFX->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
								       Rexpl, Gexpl, Bexpl, 0.0f, 0.005f);

				sharedGFX->ColorStart.r = sharedProjectileGFX->ColorStart.r;
				sharedGFX->ColorStart.g = sharedProjectileGFX->ColorStart.g;
				sharedGFX->ColorStart.b = sharedProjectileGFX->ColorStart.b;
				sharedGFX->ColorEnd.r = sharedProjectileGFX->ColorEnd.r;
				sharedGFX->ColorEnd.g = sharedProjectileGFX->ColorEnd.g;
				sharedGFX->ColorEnd.b = sharedProjectileGFX->ColorEnd.b;
				sharedGFX->Speed = 150.0f;
				sharedGFX->ParticlesPerSec = sharedProjectileGFX->ParticlesPerSec;
				sharedGFX->SizeStart = sharedProjectileGFX->SizeStart;
				sharedGFX->SizeEnd = sharedProjectileGFX->SizeEnd;
				sharedGFX->SizeVar = sharedProjectileGFX->SizeVar;
				sharedGFX->Life = Lifetime;
				sharedGFX->NeedStop = false;
				sharedGFX->SetStartLocation(ExplLocation);
			}
		}
		Projectile.NeedStopPartic = true;
		NeedStop = false;
		break;

	case 16: // missile
	case 214: // pirate mine1
	case 215: // pirate mine2
	case 216: // pirate mine3
	case 217: { // pirate mine4
		VelocityOrientation = Projectile.Orientation ^ (-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f;

		GraphicFX.resize(1);
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 10);
			sharedGFX->SetStartLocation(Projectile.Location);
		}

		int ttt = (3 + vw_fRand()) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				sVECTOR3D TM1 = Projectile.Orientation;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^
								       (Projectile.Radius * 2.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 7.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 4.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 2.0f + vw_fRand0();
			}
		}
		InternalExplosionType = 2;
	}
	break;

	case 17: { // swarm
		VelocityOrientation = Projectile.Orientation ^ (-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f;

		GraphicFX.resize(1);
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 10);
			sharedGFX->ParticlesPerSec = 30;
			sharedGFX->CreationSize = sVECTOR3D{2.0f, 0.3f, 2.0f};
			sharedGFX->SetStartLocation(Projectile.Location);
		}

		int ttt = (1 + vw_fRand()) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				sVECTOR3D TM1 = Projectile.Orientation;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^
								       (Projectile.Radius * 2.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 7.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 4.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 2.0f + vw_fRand0();
			}
		}
		InternalExplosionType = 2;
	}
	break;

	case 18:  // torpedo
	case 209: { // pirate torpedo
		VelocityOrientation = Projectile.Orientation ^ (-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f;

		GraphicFX.resize(1);
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 8);
			sharedGFX->SetStartLocation(Projectile.Location);
		}

		int ttt = (2 + vw_fRand()) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(2);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->Num = 1;
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				sVECTOR3D TM1 = Projectile.Orientation;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^
								       (Projectile.Radius * 2.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 7.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 4.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 2.0f + vw_fRand0();
			}
		}
		InternalExplosionType = 2;
	}
	break;

	case 19: // bomb
	case 210: { // pirate bomb
		VelocityOrientation = Projectile.Orientation ^ (-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f;

		GraphicFX.resize(2);
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 7);
			sharedGFX->SetStartLocation(Projectile.Location);
		}

		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 9);
			sharedGFX->SetStartLocation(Projectile.Location);
		}

		for (int i = 0; i < 4; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(3);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->Num = 1;
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				sVECTOR3D TM1 = Projectile.Orientation;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^
								       (Projectile.Radius * 4.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 6.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 2.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 3.0f + vw_fRand0();
			}
		}

		int ttt = (3  + vw_fRand()) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(2);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->Num = 1;
				sharedProjectile->SetLocation(Location);
	
				sharedProjectile->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 6.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 2.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 3.0f + vw_fRand0();
			}
		}

		ttt = (3 + vw_fRand()) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(3);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->Num = 1;
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 6.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 2.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 3.0f + vw_fRand0();
			}
		}

		ttt = (3 + vw_fRand() * 5) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 6.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 2.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 3.0f + vw_fRand0();
			}
		}

		ttt = (5 + vw_fRand() * 3) * Projectile.Radius;
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(5);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->Num = 1;
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									0.0f});
				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = sharedGFX->Speed / 2.0f;
						sharedGFX->SetStartLocation(Location);
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->SpeedEnd = sharedProjectile->Speed / 6.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed / 2.0f;
				sharedProjectile->Lifetime = sharedProjectile->Age = 3.0f + vw_fRand0();
			}
		}
		InternalExplosionType = 2;
	}
	break;

	case 205: // pirate missile
	case 206: {
		VelocityOrientation = Projectile.Orientation ^ (-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f;

		GraphicFX.resize(1);
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 10);
			sharedGFX->SetStartLocation(Projectile.Location);
		}
		InternalExplosionType = 2;
	}
	break;

	default:
		break;
	}

	// explosion with geometry animation
	if (InternalExplosionType == 2) {
		Texture = Projectile.Texture;
		Chunks = Projectile.Chunks;

		for (unsigned int i = 0; i < Chunks.size(); i++) {
			Chunks[i].VBO = 0;
			Chunks[i].IBO = 0;
			Chunks[i].VAO = 0;
			Chunks[i].NeedReleaseOpenGLBuffers = true; // this one should be released on destroy
			Chunks[i].RangeStart = 0;

			if (GameConfig().UseGLSL120) {
				Chunks[i].VertexStride = 3 + 3 + 6;
				Chunks[i].VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX;
			}

			Chunks[i].VertexArray.reset(new float[Chunks[i].VertexStride * Chunks[i].VertexQuantity],
						    std::default_delete<float[]>());

			// model's mesh rotation
			for (unsigned int j = 0; j < Chunks[i].VertexQuantity; j++) {
				int j1 = j * Chunks[i].VertexStride;
				int j2;
				if (Projectile.Chunks[i].IndexArray)
					j2 = Projectile.Chunks[i].IndexArray.get()[Projectile.Chunks[i].RangeStart + j] *
					     Projectile.Chunks[i].VertexStride;
				else
					j2 = (Projectile.Chunks[i].RangeStart + j) *
					     Projectile.Chunks[i].VertexStride;

				sVECTOR3D TMP;
				TMP.x = Projectile.Chunks[i].VertexArray.get()[j2] + Chunks[i].Location.x;
				TMP.y = Projectile.Chunks[i].VertexArray.get()[j2 + 1] + Chunks[i].Location.y;
				TMP.z = Projectile.Chunks[i].VertexArray.get()[j2 + 2] + Chunks[i].Location.z;
				vw_Matrix33CalcPoint(TMP, Projectile.CurrentRotationMat);
				// coordinates
				Chunks[i].VertexArray.get()[j1] = TMP.x;
				Chunks[i].VertexArray.get()[j1 + 1] = TMP.y;
				Chunks[i].VertexArray.get()[j1 + 2] = TMP.z;
				// normals
				TMP.x = Projectile.Chunks[i].VertexArray.get()[j2 + 3];
				TMP.y = Projectile.Chunks[i].VertexArray.get()[j2 + 4];
				TMP.z = Projectile.Chunks[i].VertexArray.get()[j2 + 5];
				vw_Matrix33CalcPoint(TMP, Projectile.CurrentRotationMat);
				Chunks[i].VertexArray.get()[j1 + 3] = TMP.x;
				Chunks[i].VertexArray.get()[j1 + 4] = TMP.y;
				Chunks[i].VertexArray.get()[j1 + 5] = TMP.z;
				// texture UV
				Chunks[i].VertexArray.get()[j1 + 6] = Projectile.Chunks[i].VertexArray.get()[j2 + 6];
				Chunks[i].VertexArray.get()[j1 + 7] = Projectile.Chunks[i].VertexArray.get()[j2 + 7];
			}

			Chunks[i].Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Chunks[i].IndexArray.reset();
		}

		float tRadius2 = Projectile.Radius / 1.5f;
		int Count = 0;
		ExplosionPieceData.reset(new sExplosionPiece[Chunks[0].VertexQuantity / 3]);
		for (unsigned int i = 0; i < Chunks[0].VertexQuantity; i += 3) {
			unsigned tmpIndex1 = Chunks[0].VertexStride * i;
			ExplosionPieceData[Count].Velocity.x = Chunks[0].VertexArray.get()[tmpIndex1];
			ExplosionPieceData[Count].Velocity.y = Chunks[0].VertexArray.get()[tmpIndex1 + 1];
			ExplosionPieceData[Count].Velocity.z = Chunks[0].VertexArray.get()[tmpIndex1 + 2];

			float VelocityTMP = vw_fRand0() * tRadius2;

			// acceleration and UV center for shader
			if (GameConfig().UseGLSL120) {
				unsigned tmpIndex2 = tmpIndex1 + Chunks[0].VertexStride;
				unsigned tmpIndex3 = tmpIndex2 + Chunks[0].VertexStride;
				Chunks[0].VertexArray.get()[tmpIndex1 + 8] = ExplosionPieceData[Count].Velocity.x;
				Chunks[0].VertexArray.get()[tmpIndex1 + 9] = ExplosionPieceData[Count].Velocity.y;
				Chunks[0].VertexArray.get()[tmpIndex1 + 10] = ExplosionPieceData[Count].Velocity.z;
				Chunks[0].VertexArray.get()[tmpIndex2 + 8] = ExplosionPieceData[Count].Velocity.x;
				Chunks[0].VertexArray.get()[tmpIndex2 + 9] = ExplosionPieceData[Count].Velocity.y;
				Chunks[0].VertexArray.get()[tmpIndex2 + 10] = ExplosionPieceData[Count].Velocity.z;
				Chunks[0].VertexArray.get()[tmpIndex3 + 8] = ExplosionPieceData[Count].Velocity.x;
				Chunks[0].VertexArray.get()[tmpIndex3 + 9] = ExplosionPieceData[Count].Velocity.y;
				Chunks[0].VertexArray.get()[tmpIndex3 + 10] = ExplosionPieceData[Count].Velocity.z;
				Chunks[0].VertexArray.get()[tmpIndex1 + 11] = VelocityTMP;
				Chunks[0].VertexArray.get()[tmpIndex2 + 11] = Chunks[0].VertexArray.get()[tmpIndex1 + 11];
				Chunks[0].VertexArray.get()[tmpIndex3 + 11] = Chunks[0].VertexArray.get()[tmpIndex1 + 11];
			}

			ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity ^ VelocityTMP;
			ExplosionPieceData[Count].RemainTime = 1.0f + vw_fRand() / 2.0f;

			float tmpSpeed = ExplosionPieceData[Count].Velocity.x * ExplosionPieceData[Count].Velocity.x +
					 ExplosionPieceData[Count].Velocity.y * ExplosionPieceData[Count].Velocity.y +
					 ExplosionPieceData[Count].Velocity.z * ExplosionPieceData[Count].Velocity.z;
			if (tmpSpeed > AABBResizeSpeed)
				AABBResizeSpeed = tmpSpeed;

			Count++;
		}
		AABBResizeSpeed = sqrtf(AABBResizeSpeed);

		if (GameConfig().UseGLSL120) {
			Chunks[0].ShaderType = 2;
			Chunks[0].ShaderData[0] = 1.0f; // speed delta
			Chunks[0].ShaderData[1] = 0.0f; // range factor
		}

		if (Chunks[0].VBO)
			vw_DeleteBufferObject(Chunks[0].VBO);
		if (!vw_BuildBufferObject(eBufferObject::Vertex,
					  Chunks[0].VertexQuantity * Chunks[0].VertexStride * sizeof(float),
					  Chunks[0].VertexArray.get(), Chunks[0].VBO))
			Chunks[0].VBO = 0;

		if (Chunks[0].VAO)
			vw_DeleteVAO(Chunks[0].VAO);
		if (!vw_BuildVAO(Chunks[0].VAO, Chunks[0].VertexFormat,
				 Chunks[0].VertexStride * sizeof(float),
				 Chunks[0].VBO, Chunks[0].IBO))
			Chunks[0].VAO = 0;
	}


	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0]))
		return;

	PlayBulletExplosionSFX(Location, NeedExplosionSFX, ExplType);

	// camera shake
	switch (ExplType) {
	case 18: // torpedo
	case 209: // pirate torpedo
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	case 19: // bomb
	case 210: // pirate bomb
		GameCameraSetExplosion(ExplLocation, 1.5f);
		break;
	}
}

} // astromenace namespace
} // viewizard namespace
