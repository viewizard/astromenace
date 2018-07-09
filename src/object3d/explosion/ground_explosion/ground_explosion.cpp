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

#include "ground_explosion.h"
#include "../../../assets/audio.h"
#include "../../../object3d/space_object/space_object.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

void GameCameraSetExplosion(sVECTOR3D Location, float Power);


//-----------------------------------------------------------------------------
// Создание взрыва из частей объекта
//-----------------------------------------------------------------------------
cGroundExplosion::cGroundExplosion(cGroundObject &Object, int ExplType, const sVECTOR3D &ExplLocation,
				   int ObjectChunkNum, bool NeedExplosionSFX)
{
	TimeLastUpdate = Object.TimeLastUpdate;
	ExplosionTypeByClass = 3;
	// тип взрыва, используем только один, но все равно нужно для проверки (может передали левый ExplType)
	// 1-взрыв на части
	int InternalExplosionType = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1 - взрыв постройки (турелей)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 1) {
		InternalExplosionType = 1;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2 - взрыв транспорта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 2) {
		InternalExplosionType = 1;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв на части
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (InternalExplosionType == 1) {
		// строим обратную матрицу
		float InvRotationMat[9];
		// сохраняем старые значения + пересчет новых
		memcpy(InvRotationMat, Object.CurrentRotationMat, 9*sizeof(Object.CurrentRotationMat[0]));
		// делаем инверсную старую матрицу
		vw_Matrix33InverseRotate(InvRotationMat);

		// содаем части, отделяем их от общей модели
		// ставим свои ориентейшины и скорость
		for (unsigned int i = 0; i < Object.Chunks.size(); i++) {
			// трак, пропускаем
			if (Object.TrackChunkNum == static_cast<int>(i))
				continue;
			else {
				// создаем часть
				std::weak_ptr<cSpaceObject> SpaceDebris = CreateSpaceDebris();
				auto sharedSpaceDebris = SpaceDebris.lock();
				if (!sharedSpaceDebris)
					return;

				sharedSpaceDebris->ObjectType = eObjectType::SpaceDebris;
				sharedSpaceDebris->DeleteAfterLeaveScene = eDeleteAfterLeaveScene::enabled;

				// только одна текстура (!) 2-ю для подстветки не тянем
				sharedSpaceDebris->Texture.resize(1, 0);
				sharedSpaceDebris->Texture[0] = Object.Texture[i];
				if ((Object.NormalMap.size() > (unsigned)i) && Object.NormalMap[i]) {
					sharedSpaceDebris->NormalMap.resize(1, 0);
					sharedSpaceDebris->NormalMap[0] = Object.NormalMap[i];
				}

				// берем то, что нужно
				sharedSpaceDebris->Chunks.resize(1);
				// копируем данные (тут уже все есть, с указателями на вбо и массив геометрии)
				sharedSpaceDebris->Chunks[0] = Object.Chunks[i];
				// берем стандартные шейдеры
				sharedSpaceDebris->Chunks[0].ShaderType = 1;
				// если надо было удалить в объекте - ставим не удалять, удалим вместе с этой частью
				if (Object.Chunks[i].NeedReleaseOpenGLBuffers) {
					Object.Chunks[i].NeedReleaseOpenGLBuffers = false;
					sharedSpaceDebris->Chunks[0].NeedReleaseOpenGLBuffers = true;
				}

				// находим точку локального положения объекта в моделе
				sVECTOR3D LocalLocation = Object.Chunks[i].Location;
				vw_Matrix33CalcPoint(LocalLocation, Object.CurrentRotationMat);
				LocalLocation = Object.HitBB[i].Location - LocalLocation;
				vw_Matrix33CalcPoint(LocalLocation, InvRotationMat);
				// и меняем внутрее положение
				sharedSpaceDebris->Chunks[0].Location = LocalLocation ^ (-1.0f);

				// находим все данные по геометрии
				sharedSpaceDebris->MetadataInitialization();

				// установка текущего положения и поворота
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
					// проверяем, это колесо или нет
					bool Wheel{false};
					for (unsigned j = 0; (j < Object.WheelChunkNums.size()) && !Wheel; j++) {
						Wheel = (Object.WheelChunkNums[j] == i);
					}

					if (Wheel) {
						sVECTOR3D VelocityTMP = sharedSpaceDebris->Location - Object.Location;
						// делаем небольшой случайный доворот
						vw_RotatePoint(VelocityTMP, sVECTOR3D(-5.0f - 15.0f * vw_fRand(), 10.0f * vw_fRand0(), 0.0f));
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


				sharedSpaceDebris->StrengthStart = sharedSpaceDebris->Strength = 1.0f;
				sharedSpaceDebris->ObjectStatus = Object.ObjectStatus;
				sharedSpaceDebris->ShowStrength = false;


				if (ObjectChunkNum != -1)
					if (ObjectChunkNum == (int)i) {
						// а теперь взрываем ту, в которую попали...
						new cSpaceExplosion(*sharedSpaceDebris, 32, sharedSpaceDebris->Location, sharedSpaceDebris->Speed, -1);
						ReleaseSpaceObject(SpaceDebris);
					}
			}
		}

		Lifetime = 0.0f;
	}

	// дальше, если не видем точку взрыва не делать... проверяем...
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0])) {
		return;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (ExplType == 1 || ExplType == 2) {
		// постройки, транспорт
		GameCameraSetExplosion(ExplLocation, 0.5f); // делаем сотрясание камеры, если нужно
		if (NeedExplosionSFX)
			PlayGameSFX(eGameSFX::Explosion_Big_Energy, 1.0f, ExplLocation, 2);
	}
}

} // astromenace namespace
} // viewizard namespace
