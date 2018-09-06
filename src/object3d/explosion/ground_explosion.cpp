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

// TODO codestyle should be fixed

#include "explosion.h"
#include "../../assets/audio.h"
#include "../../object3d/space_object/space_object.h"
#include "../../object3d/ground_object/ground_object.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, use 'include' instead
void GameCameraSetExplosion(sVECTOR3D Location, float Power);


/*
 * Constructor.
 */
cGroundExplosion::cGroundExplosion(cGroundObject &Object, int ExplType,
				   const sVECTOR3D &ExplLocation, int ObjectChunkNum)
{
	TimeLastUpdate = Object.TimeLastUpdate;
	ExplosionTypeByClass = 3;
	int InternalExplosionType = 0;

	if (ExplType == 1)
		InternalExplosionType = 1;
	if (ExplType == 2)
		InternalExplosionType = 1;

	if (InternalExplosionType == 1) {
		float InvRotationMat[9];
		memcpy(InvRotationMat, Object.CurrentRotationMat, 9 * sizeof(Object.CurrentRotationMat[0]));
		vw_Matrix33InverseRotate(InvRotationMat);

		for (unsigned int i = 0; i < Object.Chunks.size(); i++) {
			if (Object.TrackChunkNum == static_cast<int>(i))
				continue;
			else {
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
				sharedSpaceDebris->Chunks[0].ShaderType = 1;
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

				if (ExplType == 1) {
					sharedSpaceDebris->Speed = 0.0f;
					sVECTOR3D VelocityTMP = sharedSpaceDebris->Location - ExplLocation;
					if (sharedSpaceDebris->Radius != 0)
						sharedSpaceDebris->Velocity = VelocityTMP ^ (1.0f / sharedSpaceDebris->Radius);
					else
						sharedSpaceDebris->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

					sharedSpaceDebris->RotationSpeed.x = 2.0f * vw_fRand0();
					sharedSpaceDebris->RotationSpeed.y = 2.0f * vw_fRand0();
				}
				if (ExplType == 2) {
					bool Wheel{false};
					for (unsigned j = 0; (j < Object.WheelChunkNums.size()) && !Wheel; j++) {
						Wheel = (Object.WheelChunkNums[j] == i);
					}

					if (Wheel) {
						sVECTOR3D VelocityTMP = sharedSpaceDebris->Location - Object.Location;
						vw_RotatePoint(VelocityTMP, sVECTOR3D{-5.0f - 15.0f * vw_fRand(),
										      10.0f * vw_fRand0(),
										      0.0f});
						if (sharedSpaceDebris->Radius != 0.0f)
							sharedSpaceDebris->Velocity = VelocityTMP ^ ((1.0f + 5.0f * vw_fRand()) / sharedSpaceDebris->Radius);
						else
							sharedSpaceDebris->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

						sharedSpaceDebris->RotationSpeed.x = 40.0f + 80.0f * vw_fRand0();
						sharedSpaceDebris->RotationSpeed.y = 40.0f + 80.0f * vw_fRand0();
						sharedSpaceDebris->RotationSpeed.z = 40.0f + 80.0f * vw_fRand0();
					} else {
						sVECTOR3D VelocityTMP = sharedSpaceDebris->Location - Object.Location;
						if (sharedSpaceDebris->Radius != 0.0f)
							sharedSpaceDebris->Velocity = VelocityTMP ^ (5.0f / sharedSpaceDebris->Radius);
						else
							sharedSpaceDebris->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

						sharedSpaceDebris->RotationSpeed.x = 2.0f * vw_fRand0();
						sharedSpaceDebris->RotationSpeed.y = 2.0f * vw_fRand0();
						sharedSpaceDebris->RotationSpeed.z = 2.0f * vw_fRand0();
					}
				}

				sharedSpaceDebris->ArmorInitialStatus = sharedSpaceDebris->ArmorCurrentStatus = 1.0f;
				sharedSpaceDebris->ObjectStatus = Object.ObjectStatus;
				sharedSpaceDebris->ShowStrength = false;

				if ((ObjectChunkNum != -1) &&
				    (ObjectChunkNum == static_cast<int>(i))) {
					CreateSpaceExplosion(*sharedSpaceDebris, 32, sharedSpaceDebris->Location, sharedSpaceDebris->Speed, -1);
					ReleaseSpaceObject(SpaceDebris);
				}
			}
		}

		Lifetime = 0.0f;
	}

	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0]))
		return;

	if ((ExplType == 1) ||
	    (ExplType == 2)) {
		GameCameraSetExplosion(ExplLocation, 0.5f);
		PlayGameSFX(eGameSFX::Explosion_Big_Energy, 1.0f, ExplLocation, 2);
	}
}

} // astromenace namespace
} // viewizard namespace
