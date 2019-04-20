/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO don't call GetPreloadedTextureAsset() all the time, use cached texture instead

// TODO codestyle should be fixed

#include "explosion.h"
#include "../../config/config.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// all explosion list
std::list<std::unique_ptr<cExplosion, std::function<void (cExplosion *p)>>> ExplosionList{};

} // unnamed namespace


/*
 * Create cBulletExplosion object.
 */
void CreateBulletExplosion(const cObject3D *Object, cProjectile &Projectile,
			   int ExplType, const sVECTOR3D &ExplLocation, float Speed)
{
	ExplosionList.emplace_front(
		new cBulletExplosion{Object, Projectile, ExplType, ExplLocation, Speed},
		[](cExplosion *p) {delete static_cast<cBulletExplosion*>(p);});
}

/*
 * Create cGroundExplosion object.
 */
void CreateGroundExplosion(cGroundObject &Object, int ExplType,
			   const sVECTOR3D &ExplLocation, int ObjectChunkNum)
{
	ExplosionList.emplace_front(
		new cGroundExplosion{Object, ExplType, ExplLocation, ObjectChunkNum},
		[](cExplosion *p) {delete static_cast<cGroundExplosion*>(p);});
}

/*
 * Create cSpaceExplosion object.
 */
void CreateSpaceExplosion(cObject3D &Object, int ExplType, const sVECTOR3D &ExplLocation,
			  float Speed, int ObjectChunkNum)
{
	ExplosionList.emplace_front(
		new cSpaceExplosion{Object, ExplType, ExplLocation, Speed, ObjectChunkNum},
		[](cExplosion *p) {delete static_cast<cSpaceExplosion*>(p);});
}

/*
 * Update and remove (erase) expired explosions.
 */
void UpdateAllExplosion(float Time)
{
	for (auto iter = ExplosionList.begin(); iter != ExplosionList.end();) {
		if (!iter->get()->Update(Time))
			iter = ExplosionList.erase(iter);
		else
			++iter;
	}
}

/*
 * Draw all explosions.
 */
void DrawAllExplosions(bool VertexOnlyPass)
{
	for (auto &tmpObject : ExplosionList) {
		tmpObject.get()->Draw(VertexOnlyPass);
	}
}

/*
 * Release all explosions.
 */
void ReleaseAllExplosions()
{
	ExplosionList.clear();
}

/*
 * Setup explosion gfx.
 */
void SetExplosionGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXNum)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");

	switch (GFXNum) {
	case 0: // projectile
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.20f;
		ParticleSystem->SizeVar = 0.10f;
		ParticleSystem->SizeEnd = 0.05f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 1.0f;
		ParticleSystem->LifeVar = vw_fRand0() / 10.0f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 10;
		ParticleSystem->NeedStop = true;
		break;

	case 1: // asteroid
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 1.00f;
		ParticleSystem->SizeVar = 0.50f;
		ParticleSystem->SizeEnd = 0.05f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 2.0f;
		ParticleSystem->LifeVar = vw_fRand0() / 10.0f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->NeedStop = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    1.0f, 0.35f, 0.15f, 0.0f, 0.005f);
		break;

	case 2: // small alien
		ParticleSystem->ColorStart.r = 0.10f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.20f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 5.0f;
		ParticleSystem->SpeedVar = 1.5f * vw_fRand0();
		ParticleSystem->Life = 3.0f;
		ParticleSystem->LifeVar = vw_fRand0() / 10.0f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->NeedStop = true;
		break;

	case 3: // alien
		ParticleSystem->ColorStart.r = 0.10f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.80f;
		ParticleSystem->SizeVar = 0.40f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 5.0f;
		ParticleSystem->SpeedVar = 1.5f * vw_fRand0();
		ParticleSystem->Life = 2.0f;
		ParticleSystem->LifeVar = vw_fRand0() / 10.0f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{2.8f, 2.8f, 2.8f};
		ParticleSystem->NeedStop = true;
		break;

	case 4: // medium explision
		ParticleSystem->ColorStart.r = 0.10f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 1.00f;
		ParticleSystem->SizeVar = 0.50f;
		ParticleSystem->SizeEnd = 0.20f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 2.3f;
		ParticleSystem->LifeVar = vw_fRand0();
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->NeedStop = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    0.05f, 0.85f, 1.0f, 0.0f, 0.002f);
		break;

	case 5: // earth
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.20f;
		ParticleSystem->SizeEnd = 0.05f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 3.0f;
		ParticleSystem->LifeVar = vw_fRand0() / 10.0f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->NeedStop = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    1.0f, 0.35f, 0.15f, 0.0f, 0.003f);
		break;

	case 6: // earth
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 1.00f;
		ParticleSystem->SizeVar = 0.50f;
		ParticleSystem->SizeEnd = 0.20f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 2.3f;
		ParticleSystem->LifeVar = vw_fRand0();
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->NeedStop = true;
		break;

	case 7: // nuke
		ParticleSystem->ColorStart.r = 0.30f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 3.00f;
		ParticleSystem->SizeVar = 0.10f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 0.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->Life = 1.00f;
		ParticleSystem->ParticlesPerSec = 300;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{2.0f, 0.3f, 2.0f};
		ParticleSystem->DeadZone = 1.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    0.15f, 0.35f, 1.0f, 0.0f, 0.0001f);
		break;

	case 8: // torpedo
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 1.00f;
		ParticleSystem->SizeVar = 0.10f;
		ParticleSystem->SizeEnd = 0.30f;
		ParticleSystem->Speed = 0.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->Life = 1.00f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{3.0f, 0.3f, 3.0f};
		ParticleSystem->DeadZone = 0.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    0.35f, 1.0f, 0.15f, 0.0f, 0.0005f);
		break;

	case 9: // nuke
		ParticleSystem->ColorStart.r = 0.30f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 2.00f;
		ParticleSystem->SizeVar = 0.10f;
		ParticleSystem->SizeEnd = 0.30f;
		ParticleSystem->Speed = 0.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->Life = 1.00f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{3.0f, 0.3f, 3.0f};
		ParticleSystem->DeadZone = 0.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
		break;

	case 10: // missile, swarm
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 1.00f;
		ParticleSystem->SizeVar = 0.10f;
		ParticleSystem->SizeEnd = 0.30f;
		ParticleSystem->Speed = 0.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->Life = 1.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{3.0f, 0.3f, 3.0f};
		ParticleSystem->DeadZone = 0.9f;
		ParticleSystem->AlphaShowHide = false;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    1.0f, 0.35f, 0.15f, 0.0f, 0.005f);
		break;

	case 11: // alien mothership explision
		ParticleSystem->ColorStart.r = 0.10f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.20f;
		ParticleSystem->SizeEnd = 0.05f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 3.0f;
		ParticleSystem->LifeVar = vw_fRand0() / 10.0f;
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->NeedStop = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f},
							    0.05f, 0.85f, 1.0f, 0.0f, 0.001f);
		break;

	case 12: // alien mothership explision
		ParticleSystem->ColorStart.r = 0.10f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 1.00f;
		ParticleSystem->SizeVar = 0.50f;
		ParticleSystem->SizeEnd = 0.20f;
		ParticleSystem->Speed = 2.0f;
		ParticleSystem->SpeedVar = 2.0f * vw_fRand0();
		ParticleSystem->Life = 2.3f;
		ParticleSystem->LifeVar = vw_fRand0();
		ParticleSystem->Theta = 360.00f;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->NeedStop = true;
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong GFXNum.\n";
		break;
	}
}

/*
 * Constructor.
 */
cExplosion::cExplosion()
{
	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::Explosion;
	NeedCullFaces = false;
	Lifetime = 0.0f;
}

/*
 * Destructor.
 */
cExplosion::~cExplosion()
{
	for (auto &tmpGFX : GraphicFX) {
		if (auto sharedGFX = tmpGFX.lock()) {
			sharedGFX->IsSuppressed = true;
			sharedGFX->DestroyIfNoParticles = true;
		}
	}
}

/*
 * Update.
 */
bool cExplosion::Update(float Time)
{
	if (!cObject3D::Update(Time)) {
		for (auto &tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				sharedGFX->StopAllParticles();

				// FIXME why we need this in update? should be moved to initialization?
				if (!(ExplosionTypeByClass == 2 && (ExplosionType == 16 || ExplosionType == 17
								    || ExplosionType == 18 || ExplosionType == 19
								    || ExplosionType == 205 || ExplosionType == 206
								    || ExplosionType == 209 || ExplosionType == 210))) {
					vw_ReleaseParticleSystem(sharedGFX);
				}
			}
		}
		return false;
	}

	if (ExplosionTypeByClass == 1) { // FIXME why we need this on each update? should be moved to initialization?
		for (auto &tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				if (Lifetime < sharedGFX->Life)
					sharedGFX->IsSuppressed = true;
			}
		}
	}
	if (ExplosionTypeByClass == 2) { // FIXME why we need this on each update? should be moved to initialization?
		switch (ExplosionType) {
		case -16:
		case -17:
		case -18:
		case -19:
		case -205:
		case -206:
		case -209:
		case -210:
		case -214:
		case -215:
		case -216:
		case -217:
		case 16:
		case 17:
		case 18:
		case 19:
		case 205:
		case 206:
		case 209:
		case 210:
		case 214:
		case 215:
		case 216:
		case 217:
			for (auto &tmpGFX : GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					if (Lifetime < sharedGFX->Life)
						sharedGFX->IsSuppressed = true;
				}
			}
			break;

		default:
			break;
		}

		switch (ExplosionType) {
		case 16:
		case 17:
		case 205:
		case 206:
			if (auto sharedGFX = GraphicFX[0].lock()) {
				sharedGFX->CreationSize.x -= sharedGFX->CreationSize.x * TimeDelta;
				sharedGFX->CreationSize.z -= sharedGFX->CreationSize.z * TimeDelta;
				sharedGFX->DeadZone -= sharedGFX->DeadZone * TimeDelta;
			}
			break;

		case 18:
		case 209:
			if (auto sharedGFX = GraphicFX[0].lock()) {
				sharedGFX->CreationSize.x -= sharedGFX->CreationSize.x * TimeDelta;
				sharedGFX->CreationSize.z -= sharedGFX->CreationSize.z * TimeDelta;
				sharedGFX->DeadZone -= sharedGFX->DeadZone * TimeDelta;
			}
			break;

		case 19:
		case 210:
			if (auto sharedGFX = GraphicFX[0].lock()) {
				sharedGFX->CreationSize.x += 10.0f * TimeDelta;
				sharedGFX->CreationSize.z += 10.0f * TimeDelta;
				sharedGFX->DeadZone += 10.0f * TimeDelta;
			}
			if (auto sharedGFX = GraphicFX[1].lock()) {
				sharedGFX->CreationSize.x -= sharedGFX->CreationSize.x * TimeDelta;
				sharedGFX->CreationSize.z -= sharedGFX->CreationSize.z * TimeDelta;
				sharedGFX->DeadZone -= sharedGFX->DeadZone * TimeDelta;
			}
			break;

		default:
			break;
		}
	}

	if (!GameConfig().UseGLSL120) {
		if (ExplosionGeometryMoveLastTime == -1)
			ExplosionGeometryMoveLastTime = Time;

		// re-calculation only about 30 times per second
		if (ExplosionGeometryMoveLastTime + 0.035f < Time) {
			float ExplosionGeometryMove = Time - ExplosionGeometryMoveLastTime;
			ExplosionGeometryMoveLastTime = Time;

			if (!Chunks.empty()) {
				int Count = 0;

				for (auto &tmpChunk : Chunks) {
					for (unsigned int i = 0; i < tmpChunk.VertexQuantity; i += 3) {
						if (ExplosionPieceData[Count].RemainTime > 0.0f) {
							sVECTOR3D TMP = ExplosionPieceData[Count].Velocity ^ ExplosionGeometryMove;
							ExplosionPieceData[Count].Velocity -= TMP;
							ExplosionPieceData[Count].RemainTime -= ExplosionGeometryMove;

							if (ExplosionPieceData[Count].RemainTime <= 0.001f) {
								ExplosionPieceData[Count].RemainTime = 0.0f;

								tmpChunk.VertexArray.get()[(i + 1) * tmpChunk.VertexStride] = tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride];
								tmpChunk.VertexArray.get()[(i + 1) * tmpChunk.VertexStride + 1] = tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride + 1];
								tmpChunk.VertexArray.get()[(i + 1) * tmpChunk.VertexStride + 2] = tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride + 2];

								tmpChunk.VertexArray.get()[(i + 2) * tmpChunk.VertexStride] = tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride];
								tmpChunk.VertexArray.get()[(i + 2) * tmpChunk.VertexStride + 1] = tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride + 1];
								tmpChunk.VertexArray.get()[(i + 2) * tmpChunk.VertexStride + 2] = tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride + 2];
							} else {
								float tmp = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1)] - tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i];
								tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1)] -= (tmp / ExplosionPieceData[Count].RemainTime) * ExplosionGeometryMove;
								tmp = tmpChunk.VertexArray.get()[tmpChunk.VertexStride*(i + 1) + 1] - tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 1];
								tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 1] -= (tmp/ExplosionPieceData[Count].RemainTime)*ExplosionGeometryMove;
								tmp = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 2] - tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 2];
								tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 2] -= (tmp/ExplosionPieceData[Count].RemainTime)*ExplosionGeometryMove;

								tmp = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2)] - tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i];
								tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2)] -= (tmp / ExplosionPieceData[Count].RemainTime) * ExplosionGeometryMove;
								tmp = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 1] - tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 1];
								tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 1] -= (tmp / ExplosionPieceData[Count].RemainTime) * ExplosionGeometryMove;
								tmp = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 2] - tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 2];
								tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 2] -= (tmp / ExplosionPieceData[Count].RemainTime) * ExplosionGeometryMove;

								tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride] += TMP.x;
								tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride + 1] += TMP.y;
								tmpChunk.VertexArray.get()[i * tmpChunk.VertexStride + 2] += TMP.z;

								tmpChunk.VertexArray.get()[(i + 1) * tmpChunk.VertexStride] += TMP.x;
								tmpChunk.VertexArray.get()[(i + 1) * tmpChunk.VertexStride + 1] += TMP.y;
								tmpChunk.VertexArray.get()[(i + 1) * tmpChunk.VertexStride + 2] += TMP.z;

								tmpChunk.VertexArray.get()[(i + 2) * tmpChunk.VertexStride] += TMP.x;
								tmpChunk.VertexArray.get()[(i + 2) * tmpChunk.VertexStride + 1] += TMP.y;
								tmpChunk.VertexArray.get()[(i + 2) * tmpChunk.VertexStride + 2] += TMP.z;
							}
						}

						Count++;
					}

					if (tmpChunk.VBO)
						vw_DeleteBufferObject(tmpChunk.VBO);
					if (!vw_BuildBufferObject(eBufferObject::Vertex, tmpChunk.VertexQuantity * tmpChunk.VertexStride * sizeof(float),
								  tmpChunk.VertexArray.get(), tmpChunk.VBO))
						tmpChunk.VBO = 0;

					if (!tmpChunk.IBO) {
						if (!vw_BuildBufferObject(eBufferObject::Index, tmpChunk.VertexQuantity * sizeof(unsigned),
									  tmpChunk.IndexArray.get(), tmpChunk.IBO))
							tmpChunk.IBO = 0;
					}

					if (tmpChunk.VAO)
						vw_DeleteVAO(tmpChunk.VAO);
					if (!vw_BuildVAO(tmpChunk.VAO, tmpChunk.VertexFormat,
							 tmpChunk.VertexStride * sizeof(float),
							 tmpChunk.VBO, tmpChunk.IBO))
						tmpChunk.VAO = 0;
				}
			}
		}
	} else {
		for (auto &tmpChunk : Chunks) {
			// speed delta
			tmpChunk.ShaderData[1] += tmpChunk.ShaderData[0] * TimeDelta;
			// range factor
			tmpChunk.ShaderData[0] -= tmpChunk.ShaderData[0] * TimeDelta;
		}
	}

	// simplified AABB recalculation (we don't need real AABB for sure here)
	float AABBSpeedTmp = AABBResizeSpeed * TimeDelta;
	AABBResizeSpeed -= AABBSpeedTmp;
	if (!Chunks.empty()) {
		float MinX = AABB[6].x - AABBSpeedTmp;
		float MaxX = AABB[0].x + AABBSpeedTmp;
		float MinY = AABB[6].y - AABBSpeedTmp;
		float MaxY = AABB[0].y + AABBSpeedTmp;
		float MinZ = AABB[6].z - AABBSpeedTmp;
		float MaxZ = AABB[0].z + AABBSpeedTmp;

		AABB[0] = sVECTOR3D{MaxX, MaxY, MaxZ};
		AABB[1] = sVECTOR3D{MinX, MaxY, MaxZ};
		AABB[2] = sVECTOR3D{MinX, MaxY, MinZ};
		AABB[3] = sVECTOR3D{MaxX, MaxY, MinZ};
		AABB[4] = sVECTOR3D{MaxX, MinY, MaxZ};
		AABB[5] = sVECTOR3D{MinX, MinY, MaxZ};
		AABB[6] = sVECTOR3D{MinX, MinY, MinZ};
		AABB[7] = sVECTOR3D{MaxX, MinY, MinZ};
	}

	sVECTOR3D TMP2 = VelocityOrientation ^ (OldSpeed * TimeDelta);

	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		if (auto sharedGFX = GraphicFX[i].lock()) {
			sVECTOR3D tmpLocation{sharedGFX->GetLocation()};
			tmpLocation += TMP2;

			if ((ExplosionTypeByClass == 1) && (ExplosionType == 2)) {
				if (i == 1)
					sharedGFX->MoveSystemLocation(tmpLocation);
				else
					sharedGFX->MoveSystem(tmpLocation);
			} else
				sharedGFX->MoveSystem(tmpLocation);

			sharedGFX->SetStartLocation(tmpLocation);
		}
	}

	if (NeedStop)
		OldSpeed -= OldSpeed * TimeDelta;
	if (OldSpeed < 0.0f)
		OldSpeed = 0.0f;

	SetLocation(Location + TMP2);

	return true;
}

} // astromenace namespace
} // viewizard namespace
