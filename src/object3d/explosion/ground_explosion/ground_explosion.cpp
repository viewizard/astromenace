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
				   int ObjectPieceNum, bool NeedExplosionSFX)
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
		for (unsigned int i = 0; i < Object.Model3DBlocks.size(); i++) {
			// трак, пропускаем
			if (Object.TrackObjectNum == static_cast<int>(i))
				continue;
			else {
				// создаем часть
				cSpaceDebris *SpaceDebris = new cSpaceDebris;
				SpaceDebris->ObjectType = eObjectType::SpaceDebris;
				SpaceDebris->DeleteAfterLeaveScene = eDeleteAfterLeaveScene::enabled;

				// только одна текстура (!) 2-ю для подстветки не тянем
				SpaceDebris->Texture.resize(1, 0);
				SpaceDebris->Texture[0] = Object.Texture[i];
				if ((Object.NormalMap.size() > (unsigned)i) && Object.NormalMap[i]) {
					SpaceDebris->NormalMap.resize(1, 0);
					SpaceDebris->NormalMap[0] = Object.NormalMap[i];
				}

				// берем то, что нужно
				SpaceDebris->Model3DBlocks.resize(1);
				// копируем данные (тут уже все есть, с указателями на вбо и массив геометрии)
				SpaceDebris->Model3DBlocks[0] = Object.Model3DBlocks[i];
				// берем стандартные шейдеры
				SpaceDebris->Model3DBlocks[0].ShaderType = 1;
				// если надо было удалить в объекте - ставим не удалять, удалим вместе с этой частью
				if (Object.Model3DBlocks[i].NeedReleaseOpenGLBuffers) {
					Object.Model3DBlocks[i].NeedReleaseOpenGLBuffers = false;
					SpaceDebris->Model3DBlocks[0].NeedReleaseOpenGLBuffers = true;
				}

				// находим точку локального положения объекта в моделе
				sVECTOR3D LocalLocation = Object.Model3DBlocks[i].Location;
				vw_Matrix33CalcPoint(LocalLocation, Object.CurrentRotationMat);
				LocalLocation = Object.HitBB[i].Location - LocalLocation;
				vw_Matrix33CalcPoint(LocalLocation, InvRotationMat);
				// и меняем внутрее положение
				SpaceDebris->Model3DBlocks[0].Location = LocalLocation ^ (-1.0f);

				// находим все данные по геометрии
				SpaceDebris->MetadataInitialization();

				// установка текущего положения и поворота
				SpaceDebris->SetLocation(Object.Location + Object.HitBB[i].Location);
				SpaceDebris->SetRotation(Object.Rotation);


				if (ExplType == 1) {
					SpaceDebris->Speed = 0.0f;
					sVECTOR3D VelocityTMP = SpaceDebris->Location - ExplLocation;
					if (SpaceDebris->Radius != 0)
						SpaceDebris->Velocity = VelocityTMP ^ (1.0f / SpaceDebris->Radius);
					else
						SpaceDebris->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

					SpaceDebris->RotationSpeed.x = 2.0f * vw_fRand0();
					SpaceDebris->RotationSpeed.y = 2.0f * vw_fRand0();
				}
				if (ExplType == 2) {
					// проверяем, это колесо или нет
					bool Wheel = false;
					for (int k = 0; k < Object.WheelQuantity; k++) {
						if (Object.WheelObjectsNum[k] == (int)i)
							Wheel = true;
					}

					if (Wheel) {
						sVECTOR3D VelocityTMP = SpaceDebris->Location - Object.Location;
						// делаем небольшой случайный доворот
						vw_RotatePoint(VelocityTMP, sVECTOR3D(-5.0f - 15.0f * vw_fRand(), 10.0f * vw_fRand0(), 0.0f));
						if (SpaceDebris->Radius != 0.0f)
							SpaceDebris->Velocity = VelocityTMP ^ ((1.0f + 5.0f * vw_fRand()) / SpaceDebris->Radius);
						else
							SpaceDebris->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

						SpaceDebris->RotationSpeed.x = 40.0f + 80.0f * vw_fRand0();
						SpaceDebris->RotationSpeed.y = 40.0f + 80.0f * vw_fRand0();
						SpaceDebris->RotationSpeed.z = 40.0f + 80.0f * vw_fRand0();
					} else {
						sVECTOR3D VelocityTMP = SpaceDebris->Location - Object.Location;
						if (SpaceDebris->Radius != 0.0f)
							SpaceDebris->Velocity = VelocityTMP ^ (5.0f / SpaceDebris->Radius);
						else
							SpaceDebris->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

						SpaceDebris->RotationSpeed.x = 2.0f * vw_fRand0();
						SpaceDebris->RotationSpeed.y = 2.0f * vw_fRand0();
						SpaceDebris->RotationSpeed.z = 2.0f * vw_fRand0();
					}
				}


				SpaceDebris->StrengthStart = SpaceDebris->Strength = 1.0f;
				SpaceDebris->ObjectStatus = Object.ObjectStatus;
				SpaceDebris->ShowStrength = false;


				if (ObjectPieceNum != -1)
					if (ObjectPieceNum == (int)i) {
						// а теперь взрываем ту, в которую попали...
						new cSpaceExplosion(SpaceDebris, 32, SpaceDebris->Location, SpaceDebris->Speed, -1);
						delete SpaceDebris;
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
