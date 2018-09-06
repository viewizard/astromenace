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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

// TODO codestyle should be fixed

#include "explosion.h"
#include "../projectile/projectile.h"
#include "../../config/config.h"
#include "../../assets/audio.h"
#include "../space_object/space_object.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, use 'include' instead
void GameCameraSetExplosion(sVECTOR3D Location, float Power);


/*
 * Constructor.
 */
cSpaceExplosion::cSpaceExplosion(cObject3D &Object, int ExplType, const sVECTOR3D &ExplLocation,
				 float Speed, int ObjectChunkNum)
{
	TimeLastUpdate = Object.TimeLastUpdate;
	ExplosionTypeByClass = 1;
	int InternalExplosionType = 0;

	float CurrentPenalty{1.0f};
	if (ObjectStatus == eObjectStatus::Enemy)
		CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty) + 1.0f;

	ObjectStatus = Object.ObjectStatus;
	ExplosionType = ExplType;
	float MeshAcc = 0.0f;

	// asteroid
	if (ExplType == 1) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 2.0f;
		MeshAcc = 10.0f;

		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		GraphicFX.resize(1);
		float tRadius = Object.Radius / 2.0f;
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 1);
			sharedGFX->Speed = 1.5f * Object.Radius;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			sharedGFX->ParticlesPerSec = (int)(20 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
			sharedGFX->CreationType = eParticleCreationType::Sphere;
			sharedGFX->CreationSize = sVECTOR3D{tRadius, tRadius, tRadius};
			sharedGFX->Life = 1.5f;
			sharedGFX->SizeStart = tRadius / 1.5f;
		}

		int ttt = (int)(3 * Object.Radius) + (int)(vw_fRand0() * 3 * Object.Radius);
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{360.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									360.0f * vw_fRand0()});
				sVECTOR3D TM1 = Object.Orientation ^ Speed;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^ (Object.Radius * 6.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = 1.5f;
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->Speed = Speed + Object.Radius * 2.0f + 2.0f * vw_fRand0();
				sharedProjectile->SpeedEnd = 0.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed;
				sharedProjectile->Lifetime = sharedProjectile->Age = 1.0f + vw_fRand0();
				sharedProjectile->Damage /= CurrentPenalty;
			}
		}
	}

	// aliens
	if (ExplType == 2) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f;
		MeshAcc = 20.0f;

		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		GraphicFX.resize(3);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 3);
			sharedGFX->MoveSystem(Object.Location);
			sharedGFX->ParticlesPerSec = (int)(5 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
			sharedGFX->CreationSize = sVECTOR3D{Object.Radius / 4,
							    Object.Radius / 4,
							    Object.Radius / 4};
		}

		GraphicFX[2] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[2].lock()) {
			SetExplosionGFX(sharedGFX, 4);
			sharedGFX->Speed = Object.Radius / 3.0f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed != 0.0f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(5 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
			sharedGFX->NeedStop = false;
		}

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 2);
			sharedGFX->Speed = Object.Radius * 1.2f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed != 0.0f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(10 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}

		int ttt = (int)(Object.Radius) + (int)(vw_fRand0() * Object.Radius);
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);
	
				sharedProjectile->SetRotation(sVECTOR3D{360.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									360.0f * vw_fRand0()});
				sVECTOR3D TM1 = Object.Orientation ^ Speed;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^ (Object.Radius / 4.0f));
				sharedProjectile->Orientation.Normalize();
	
				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = 2.5f;
						sharedGFX->ColorStart.r = 0.30f;
						sharedGFX->ColorStart.g = 1.00f;
						sharedGFX->ColorStart.b = 1.00f;
						sharedGFX->ColorEnd.r = 0.00f;
						sharedGFX->ColorEnd.g = 1.00f;
						sharedGFX->ColorEnd.b = 1.00f;
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->Speed = Speed + Object.Radius / 2.0f + 2.0f * vw_fRand0();
				sharedProjectile->SpeedEnd = 0.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed;
				sharedProjectile->Lifetime = sharedProjectile->Age = 1.5f + vw_fRand0();
				sharedProjectile->Damage /= CurrentPenalty;
			}
		}
	}

	// pirates, earth to debris
	if (ExplType == 31)
		InternalExplosionType = 1;

	// pirates, earth
	if (ExplType == 32) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f;
		MeshAcc = 20.0f;

		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		GraphicFX.resize(2);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 6);
			sharedGFX->Speed = Object.Radius / 1.4f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed > 0.01f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(15 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 5);
			sharedGFX->Speed = Object.Radius * 1.1f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed > 0.01f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(15 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}
	}

	// alien mothership to debris
	if (ExplType == 33)
		InternalExplosionType = 1;

	// alien mothership
	if (ExplType == 34) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f;
		MeshAcc = 20.0f;

		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		GraphicFX.resize(2);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 12);
			sharedGFX->Speed = Object.Radius / 1.4f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed != 0.0f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(15 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 11);
			sharedGFX->Speed = Object.Radius * 1.1f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed != 0.0f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(15 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}
	}

	// pirates, earth
	if (ExplType == 3) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f;
		MeshAcc = 20.0f;

		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		GraphicFX.resize(2);
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 6);
			sharedGFX->Speed = Object.Radius / 1.3f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed != 0.0f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(15 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}

		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 5);
			sharedGFX->Speed = Object.Radius * 1.1f;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->MoveSystem(Object.Location);
			if (Speed != 0.0f)
				sharedGFX->Theta = 360.00f / (Speed / 8);
			sharedGFX->ParticlesPerSec = (int)(15 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
		}

		int ttt = (int)(0.5f * Object.Radius) + (int)(vw_fRand0() * Object.Radius);
		for (int i = 0; i < ttt; i++) {
			std::weak_ptr<cProjectile> tmpProjectile = CreateProjectile(1);
			if (auto sharedProjectile = tmpProjectile.lock()) {
				sharedProjectile->SetLocation(Location);

				sharedProjectile->SetRotation(sVECTOR3D{360.0f * vw_fRand0(),
									360.0f * vw_fRand0(),
									360.0f * vw_fRand0()});
				sVECTOR3D TM1 = Object.Orientation ^ Speed;
				sharedProjectile->Orientation = TM1 + (sharedProjectile->Orientation ^ (Object.Radius / 2.0f));
				sharedProjectile->Orientation.Normalize();

				for (auto &tmpGFX : sharedProjectile->GraphicFX) {
					if (auto sharedGFX = tmpGFX.lock()) {
						sharedGFX->Direction = sharedProjectile->Orientation ^ -1;
						sharedGFX->Speed = 2.5f;
					}
				}
				sharedProjectile->ObjectStatus = ObjectStatus;
				sharedProjectile->Speed = Speed + Object.Radius / 2.0f + 2.0f * vw_fRand0();
				sharedProjectile->SpeedEnd = 0.0f;
				sharedProjectile->SpeedStart = sharedProjectile->Speed;
				sharedProjectile->Lifetime = sharedProjectile->Age = 1.5f + vw_fRand0();
				sharedProjectile->Damage /= CurrentPenalty;
			}
		}
	}

	// missile hit to flares
	if (ExplType == 4) {
		InternalExplosionType = 2;
		OldSpeed = Speed / 5.0f;
		Lifetime = 1.0f;
		MeshAcc = 5.0f;
	}

	// explosion to debris (for large ships and earth ships)
	if (InternalExplosionType == 1) {
		float InvRotationMat[9];
		memcpy(InvRotationMat, Object.CurrentRotationMat, 9 * sizeof(Object.CurrentRotationMat[0]));
		vw_Matrix33InverseRotate(InvRotationMat);

		for (unsigned int i = 0; i < Object.Chunks.size(); i++) {
			std::weak_ptr<cSpaceObject> SpaceDebris = CreateSpaceDebris();
			auto sharedSpaceDebris = SpaceDebris.lock();
			if (!sharedSpaceDebris)
				return;

			sharedSpaceDebris->ObjectType = eObjectType::SpaceDebris;
			sharedSpaceDebris->DeleteAfterLeaveScene = eDeleteAfterLeaveScene::enabled;

			sharedSpaceDebris->Texture.resize(1, 0);
			sharedSpaceDebris->Texture[0] = Object.Texture[i];
			if ((Object.NormalMap.size() > (unsigned)i) && Object.NormalMap[i]) {
				sharedSpaceDebris->NormalMap.resize(1, 0);
				sharedSpaceDebris->NormalMap[0] = Object.NormalMap[i];
			}

			sharedSpaceDebris->Chunks.resize(1);
			sharedSpaceDebris->Chunks[0] = Object.Chunks[i];

			if (Object.Chunks[i].NeedReleaseOpenGLBuffers) {
				Object.Chunks[i].NeedReleaseOpenGLBuffers = false;
				sharedSpaceDebris->Chunks[0].NeedReleaseOpenGLBuffers = true;
			}

			sVECTOR3D LocalLocation = Object.Chunks[i].Location;
			vw_Matrix33CalcPoint(LocalLocation, Object.CurrentRotationMat);
			LocalLocation = Object.HitBB[i].Location - LocalLocation;
			vw_Matrix33CalcPoint(LocalLocation, InvRotationMat);

			sharedSpaceDebris->Chunks[0].Location = LocalLocation ^ (-1.0f);

			sharedSpaceDebris->MetadataInitialization();
			sharedSpaceDebris->SetLocation(Object.Location + Object.HitBB[i].Location);
			sharedSpaceDebris->SetRotation(Object.Rotation);
			sharedSpaceDebris->Speed = Speed - 2 * vw_fRand();
			sharedSpaceDebris->RotationSpeed.x = 2.0f * vw_fRand0();
			sharedSpaceDebris->RotationSpeed.y = 2.0f * vw_fRand0();
			sharedSpaceDebris->StrengthStart = sharedSpaceDebris->ArmorCurrentStatus = 1.0f;
			sharedSpaceDebris->ObjectStatus = Object.ObjectStatus;
			sharedSpaceDebris->ShowStrength = false;

			int NeedExplosionType = 32;
			switch (ExplType) {
			case 31: // pirates, earth
				NeedExplosionType = 32;
				break;
			case 33: // alien mothership
				sharedSpaceDebris->BossPartCountDown = 3.0f * vw_fRand();
				NeedExplosionType = 34;
				break;
			}
			if ((ObjectChunkNum != -1) &&
			    (ObjectChunkNum == static_cast<int>(i))) {
					CreateSpaceExplosion(*sharedSpaceDebris, NeedExplosionType,
							     sharedSpaceDebris->Location, sharedSpaceDebris->Speed, -1);
					ReleaseSpaceObject(SpaceDebris);
				}
		}

		Lifetime = 0.0f;
	}

	// explosion with geometry animation
	if (InternalExplosionType == 2) {
		AABB[0] = Object.AABB[0];
		AABB[1] = Object.AABB[1];
		AABB[2] = Object.AABB[2];
		AABB[3] = Object.AABB[3];
		AABB[4] = Object.AABB[4];
		AABB[5] = Object.AABB[5];
		AABB[6] = Object.AABB[6];
		AABB[7] = Object.AABB[7];

		VelocityOrientation = Object.Orientation;

		int TotalCount = 0;

		Texture = Object.Texture;
		Chunks = Object.Chunks;

		int NeedIn = GameConfig().VisualEffectsQuality;

		for (unsigned int i = 0; i < Chunks.size(); i++) {
			Chunks[i].VBO = 0;
			Chunks[i].IBO = 0;
			Chunks[i].VAO = 0;
			Chunks[i].NeedReleaseOpenGLBuffers = true;
			Chunks[i].RangeStart = 0;
			Chunks[i].IndexArray.reset();
			Chunks[i].VertexArrayWithSmallTriangles.reset();
			Chunks[i].VertexArrayWithSmallTrianglesCount = 0;

			Chunks[i].VertexQuantity = 0;
			int k = 0;
			int NeedInCur = NeedIn;

			int tricount = 0;

			// we don't need second texture coord. here
			if ((Object.Chunks[i].VertexFormat & 0x000000F) >= 2)
				Chunks[i].VertexFormat = (Object.Chunks[i].VertexFormat & 0xFFFFFF0) | RI_1_TEX;
			else
				Chunks[i].VertexFormat = Object.Chunks[i].VertexFormat;

			Chunks[i].VertexStride = Object.Chunks[i].VertexStride;

			if (GameConfig().UseGLSL120) {
				Chunks[i].VertexStride = 3 + 3 + 6;
				Chunks[i].VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX;
			}

			Chunks[i].VertexArray.reset(new float[Chunks[i].VertexStride * Object.Chunks[i].VertexArrayWithSmallTrianglesCount],
						    std::default_delete<float[]>());

			float TransMat[16]{Object.CurrentRotationMat[0], Object.CurrentRotationMat[1], Object.CurrentRotationMat[2], 0.0f,
					   Object.CurrentRotationMat[3], Object.CurrentRotationMat[4], Object.CurrentRotationMat[5], 0.0f,
					   Object.CurrentRotationMat[6], Object.CurrentRotationMat[7], Object.CurrentRotationMat[8], 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f};

			float TransMatTMP[16];
			vw_Matrix44Identity(TransMatTMP);
			float TransMatNorm[9];
			vw_Matrix33Identity(TransMatNorm);

			if (Object.Chunks[i].Rotation.x != 0.0f ||
			    Object.Chunks[i].Rotation.y != 0.0f ||
			    Object.Chunks[i].Rotation.z != 0.0f) {
				vw_Matrix44CreateRotate(TransMatTMP, Object.Chunks[i].Rotation);
				vw_Matrix33CreateRotate(TransMatNorm, Object.Chunks[i].Rotation);
			}
			if (Object.Chunks[i].GeometryAnimation.x != 0.0f ||
			    Object.Chunks[i].GeometryAnimation.y != 0.0f ||
			    Object.Chunks[i].GeometryAnimation.z != 0.0f) {
				float TransMatAnimTMP[16];
				vw_Matrix44CreateRotate(TransMatAnimTMP, Object.Chunks[i].GeometryAnimation);
				vw_Matrix44Mult(TransMatTMP, TransMatAnimTMP);
				float TransMatAnimTMPNorm[9];
				vw_Matrix33CreateRotate(TransMatAnimTMPNorm, Object.Chunks[i].GeometryAnimation);
				vw_Matrix33Mult(TransMatNorm, TransMatAnimTMPNorm);
			}

			vw_Matrix44Translate(TransMatTMP, Chunks[i].Location);
			vw_Matrix44Mult(TransMat, TransMatTMP);
			vw_Matrix33Mult(TransMatNorm, Object.CurrentRotationMat);


			sVECTOR3D TMP;
			for (unsigned int j = 0; j < Object.Chunks[i].VertexArrayWithSmallTrianglesCount; j++) {
				if (NeedInCur <= 0) {
					int j1 = k * Chunks[i].VertexStride;
					int j2 = j * Object.Chunks[i].VertexStride;

					TMP.x = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2];
					TMP.y = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 1];
					TMP.z = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 2];
					vw_Matrix44CalcPoint(TMP, TransMat);
					// coordinates
					Chunks[i].VertexArray.get()[j1] = TMP.x;
					Chunks[i].VertexArray.get()[j1 + 1] = TMP.y;
					Chunks[i].VertexArray.get()[j1 + 2] = TMP.z;
					// normals
					TMP.x = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 3];
					TMP.y = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 4];
					TMP.z = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 5];
					vw_Matrix33CalcPoint(TMP, TransMatNorm);
					Chunks[i].VertexArray.get()[j1 + 3] = TMP.x;
					Chunks[i].VertexArray.get()[j1 + 4] = TMP.y;
					Chunks[i].VertexArray.get()[j1 + 5] = TMP.z;
					// texture UV
					Chunks[i].VertexArray.get()[j1 + 6] = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 6];
					Chunks[i].VertexArray.get()[j1 + 7] = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 7];

					Chunks[i].VertexQuantity++;
					k++;

					if (tricount == 2)
						NeedInCur = NeedIn;
				} else if (tricount == 2)
					NeedInCur--;

				tricount++;
				if (tricount >= 3)
					tricount = 0;
			}

			Chunks[i].Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Chunks[i].Rotation = sVECTOR3D{0.0f, 0.0f, 0.0f};
			Chunks[i].GeometryAnimation = sVECTOR3D{0.0f, 0.0f, 0.0f};

			TotalCount += Chunks[i].VertexQuantity;
		}

		// расстояние от центра до крайней точки
		float Diag = Object.Length * Object.Length +
			     Object.Height * Object.Height +
			     Object.Width * Object.Width;

		// для каждого треугольника - свои данные
		int Count = 0;
		ExplosionPieceData.reset(new sExplosionPiece[TotalCount / 3]);
		for (auto &tmpChunk : Chunks) {
			for (unsigned int i = 0; i < tmpChunk.VertexQuantity; i += 3) {
				unsigned tmpIndex1 = tmpChunk.VertexStride * i;
				unsigned tmpIndex2 = tmpIndex1 + tmpChunk.VertexStride;
				unsigned tmpIndex3 = tmpIndex2 + tmpChunk.VertexStride;
				ExplosionPieceData[Count].Velocity.x = tmpChunk.VertexArray.get()[tmpIndex1];
				ExplosionPieceData[Count].Velocity.x += tmpChunk.VertexArray.get()[tmpIndex2];
				ExplosionPieceData[Count].Velocity.x += tmpChunk.VertexArray.get()[tmpIndex3];
				ExplosionPieceData[Count].Velocity.x = ExplosionPieceData[Count].Velocity.x / 3.0f;
				ExplosionPieceData[Count].Velocity.y = tmpChunk.VertexArray.get()[tmpIndex1 + 1];
				ExplosionPieceData[Count].Velocity.y += tmpChunk.VertexArray.get()[tmpIndex2 + 1];
				ExplosionPieceData[Count].Velocity.y += tmpChunk.VertexArray.get()[tmpIndex3 + 1];
				ExplosionPieceData[Count].Velocity.y = ExplosionPieceData[Count].Velocity.y / 3.0f;
				ExplosionPieceData[Count].Velocity.z = tmpChunk.VertexArray.get()[tmpIndex1 + 2];
				ExplosionPieceData[Count].Velocity.z += tmpChunk.VertexArray.get()[tmpIndex2 + 2];
				ExplosionPieceData[Count].Velocity.z += tmpChunk.VertexArray.get()[tmpIndex3 + 2];
				ExplosionPieceData[Count].Velocity.z = ExplosionPieceData[Count].Velocity.z / 3.0f;

				// находим расстояние, чтобы включить его
				float dist = ExplosionPieceData[Count].Velocity.x * ExplosionPieceData[Count].Velocity.x +
					     ExplosionPieceData[Count].Velocity.y * ExplosionPieceData[Count].Velocity.y +
					     ExplosionPieceData[Count].Velocity.z * ExplosionPieceData[Count].Velocity.z +
					     vw_fRand0();


				float Acc = (MeshAcc / 1000.0f) * (Diag / dist) * (MeshAcc / Object.Radius);
				if (Acc > MeshAcc)
					Acc = MeshAcc + vw_fRand0();
				if (Acc < -MeshAcc)
					Acc = -MeshAcc - vw_fRand0();


				// записываем центр треугольника, оно же базовое ускорение + цент UV, для передачи шейдеру
				if (GameConfig().UseGLSL120) {
					tmpChunk.VertexArray.get()[tmpIndex1 + 8] = ExplosionPieceData[Count].Velocity.x;
					tmpChunk.VertexArray.get()[tmpIndex1 + 9] = ExplosionPieceData[Count].Velocity.y;
					tmpChunk.VertexArray.get()[tmpIndex1 + 10] = ExplosionPieceData[Count].Velocity.z;
					tmpChunk.VertexArray.get()[tmpIndex2 + 8] = ExplosionPieceData[Count].Velocity.x;
					tmpChunk.VertexArray.get()[tmpIndex2 + 9] = ExplosionPieceData[Count].Velocity.y;
					tmpChunk.VertexArray.get()[tmpIndex2 + 10] = ExplosionPieceData[Count].Velocity.z;
					tmpChunk.VertexArray.get()[tmpIndex3 + 8] = ExplosionPieceData[Count].Velocity.x;
					tmpChunk.VertexArray.get()[tmpIndex3 + 9] = ExplosionPieceData[Count].Velocity.y;
					tmpChunk.VertexArray.get()[tmpIndex3 + 10] = ExplosionPieceData[Count].Velocity.z;
					if (dist/Diag < 0.01f) {
						tmpChunk.VertexArray.get()[tmpIndex1 + 11] = Acc + 4.0f * vw_fRand0();
						tmpChunk.VertexArray.get()[tmpIndex2 + 11] = tmpChunk.VertexArray.get()[tmpIndex1 + 11];
						tmpChunk.VertexArray.get()[tmpIndex3 + 11] = tmpChunk.VertexArray.get()[tmpIndex1 + 11];
					} else {
						tmpChunk.VertexArray.get()[tmpIndex1 + 11] = Acc;
						tmpChunk.VertexArray.get()[tmpIndex2 + 11] = Acc;
						tmpChunk.VertexArray.get()[tmpIndex3 + 11] = Acc;
					}

				}


				if (dist / Diag < 0.01f)
					ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity ^
									     (Acc + 4.0f * vw_fRand0());
				else
					ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity ^ Acc;


				ExplosionPieceData[Count].RemainTime = (Lifetime - 1.0f) + vw_fRand();
				if (ExplosionPieceData[Count].RemainTime < 0.0f)
					ExplosionPieceData[Count].RemainTime = 0.0f;

				float tmpSpeed = ExplosionPieceData[Count].Velocity.x * ExplosionPieceData[Count].Velocity.x +
						 ExplosionPieceData[Count].Velocity.y * ExplosionPieceData[Count].Velocity.y +
						 ExplosionPieceData[Count].Velocity.z * ExplosionPieceData[Count].Velocity.z;
				if (tmpSpeed > AABBResizeSpeed)
					AABBResizeSpeed = tmpSpeed;

				Count++;
			}

			if (tmpChunk.VBO)
				vw_DeleteBufferObject(tmpChunk.VBO);
			if (!vw_BuildBufferObject(eBufferObject::Vertex,
						  tmpChunk.VertexQuantity * tmpChunk.VertexStride * sizeof(float),
						  tmpChunk.VertexArray.get(), tmpChunk.VBO))
				tmpChunk.VBO = 0;

			if (tmpChunk.VAO)
				vw_DeleteVAO(tmpChunk.VAO);
			if (!vw_BuildVAO(tmpChunk.VAO, tmpChunk.VertexFormat,
					 tmpChunk.VertexStride * sizeof(float),
					 tmpChunk.VBO, tmpChunk.IBO))
				tmpChunk.VAO = 0;

			if (GameConfig().UseGLSL120) {
				tmpChunk.ShaderType = 2;
				tmpChunk.ShaderData[0] = 1.0f; // speed delta
				tmpChunk.ShaderData[1] = 0.0f; // range factor
			}
		}
		AABBResizeSpeed = sqrtf(AABBResizeSpeed);
	}

	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0]))
		return;

	switch (ExplType) {
	case 1: // asteroid
		PlayGameSFX(eGameSFX::Explosion_Small, 1.0f, ExplLocation, 2);
		break;
	case 2: // alien
		PlayGameSFX(eGameSFX::Explosion_Big, 1.0f, ExplLocation, 2);
		break;
	case 3: // pirate, earth
		PlayGameSFX(eGameSFX::Explosion_Big_Energy, 1.0f, ExplLocation, 2);
		break;
	case 32: // derbis - pirate, earth
		PlayGameSFX(eGameSFX::Explosion_Big_Energy, 1.0f, ExplLocation, 2);
		break;
	case 34: // alien mothership
		PlayGameSFX(eGameSFX::Explosion_Big, 1.0f, ExplLocation, 2);
		break;
	}

	// camera shake
	switch (ExplType) {
	case 1: // asteroid
		GameCameraSetExplosion(ExplLocation, 0.2f);
		break;

	case 2: // alien
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	case 3: // pirate, earth
	case 32: // derbis - pirate, earth
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	case 34: // alien mothership
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	}

}

} // astromenace namespace
} // viewizard namespace
