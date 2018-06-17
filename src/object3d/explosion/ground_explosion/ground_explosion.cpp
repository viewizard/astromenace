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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "ground_explosion.h"
#include "../../../object3d/space_object/ship_part/ship_part.h"

void GameCameraSetExplosion(sVECTOR3D Location, float Power);


//-----------------------------------------------------------------------------
// Создание взрыва из частей объекта
//-----------------------------------------------------------------------------
cGroundExplosion::cGroundExplosion(cGroundObject *Object, int ExplType, const sVECTOR3D &ExplLocation,
				   int ObjectPieceNum, bool NeedExplosionSFX)
{
	TimeLastUpdate = Object->TimeLastUpdate;
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
		memcpy(InvRotationMat, Object->CurrentRotationMat, 9*sizeof(Object->CurrentRotationMat[0]));
		// делаем инверсную старую матрицу
		vw_Matrix33InverseRotate(InvRotationMat);

		// содаем части, отделяем их от общей модели
		// ставим свои ориентейшины и скорость
		for (unsigned int i = 0; i < Object->Model3DBlocks.size(); i++) {
			// трак, пропускаем
			if (Object->TrackObjectNum == static_cast<int>(i))
				continue;
			else {
				// создаем часть
				cShipPart *ShipPart;
				ShipPart = new cShipPart;
				ShipPart->ObjectType = eObjectType::ShipPart;
				ShipPart->DeleteAfterLeaveScene = eDeleteAfterLeaveScene::enabled;

				// только одна текстура (!) 2-ю для подстветки не тянем
				ShipPart->Texture.resize(1, 0);
				ShipPart->Texture[0] = Object->Texture[i];
				if ((Object->NormalMap.size() > (unsigned)i) && Object->NormalMap[i]) {
					ShipPart->NormalMap.resize(1, 0);
					ShipPart->NormalMap[0] = Object->NormalMap[i];
				}

				// берем то, что нужно
				ShipPart->Model3DBlocks.resize(1);
				// копируем данные (тут уже все есть, с указателями на вбо и массив геометрии)
				ShipPart->Model3DBlocks[0] = Object->Model3DBlocks[i];
				// берем стандартные шейдеры
				ShipPart->Model3DBlocks[0].ShaderType = 1;
				// если надо было удалить в объекте - ставим не удалять, удалим вместе с этой частью
				if (Object->Model3DBlocks[i].NeedReleaseOpenGLBuffers) {
					Object->Model3DBlocks[i].NeedReleaseOpenGLBuffers = false;
					ShipPart->Model3DBlocks[0].NeedReleaseOpenGLBuffers = true;
				}

				// находим точку локального положения объекта в моделе
				sVECTOR3D LocalLocation = Object->Model3DBlocks[i].Location;
				vw_Matrix33CalcPoint(LocalLocation, Object->CurrentRotationMat);
				LocalLocation = Object->HitBB[i].Location - LocalLocation;
				vw_Matrix33CalcPoint(LocalLocation, InvRotationMat);
				// и меняем внутрее положение
				ShipPart->Model3DBlocks[0].Location = LocalLocation ^ (-1.0f);

				// находим все данные по геометрии
				ShipPart->MetadataInitialization();

				// установка текущего положения и поворота
				ShipPart->SetLocation(Object->Location + Object->HitBB[i].Location);
				ShipPart->SetRotation(Object->Rotation);


				if (ExplType == 1) {
					ShipPart->Speed = 0.0f;
					sVECTOR3D VelocityTMP = ShipPart->Location - ExplLocation;
					if (ShipPart->Radius != 0) ShipPart->Velocity = VelocityTMP^(1.0f/ShipPart->Radius);
					else ShipPart->Velocity = VelocityTMP ^ (1.0f + 5.0f * vw_fRand());

					ShipPart->RotationSpeed.x = 2.0f*vw_Randf0;
					ShipPart->RotationSpeed.y = 2.0f*vw_Randf0;
				}
				if (ExplType == 2) {
					// проверяем, это колесо или нет
					bool Wheel = false;
					for (int k=0; k<Object->WheelQuantity; k++) {
						if (Object->WheelObjectsNum[k] == (int)i) Wheel = true;
					}

					if (Wheel) {
						sVECTOR3D VelocityTMP = ShipPart->Location - Object->Location;
						// делаем небольшой случайный доворот
						vw_RotatePoint(VelocityTMP, sVECTOR3D(-5.0f - 15.0f * vw_fRand(), 10.0f*vw_Randf0, 0.0f));
						if(ShipPart->Radius != 0.0f) ShipPart->Velocity = VelocityTMP^((1.0f + 5.0f * vw_fRand())/ShipPart->Radius);
						else ShipPart->Velocity = VelocityTMP^(1.0f + 5.0f * vw_fRand());

						ShipPart->RotationSpeed.x = 40.0f+80.0f*vw_Randf0;
						ShipPart->RotationSpeed.y = 40.0f+80.0f*vw_Randf0;
						ShipPart->RotationSpeed.z = 40.0f+80.0f*vw_Randf0;
					} else {
						sVECTOR3D VelocityTMP = ShipPart->Location - Object->Location;
						if(ShipPart->Radius != 0.0f) ShipPart->Velocity = VelocityTMP^(5.0f/ShipPart->Radius);
						else ShipPart->Velocity = VelocityTMP^(1.0f + 5.0f * vw_fRand());

						ShipPart->RotationSpeed.x = 2.0f*vw_Randf0;
						ShipPart->RotationSpeed.y = 2.0f*vw_Randf0;
						ShipPart->RotationSpeed.z = 2.0f*vw_Randf0;
					}
				}


				ShipPart->StrengthStart = ShipPart->Strength = 1.0f;
				ShipPart->ObjectStatus = Object->ObjectStatus;
				ShipPart->ShowStrength = false;


				if (ObjectPieceNum != -1)
					if (ObjectPieceNum == (int)i) {
						// а теперь взрываем ту, в которую попали...
						new cSpaceExplosion(ShipPart, 32, ShipPart->Location, ShipPart->Speed, -1);
						delete ShipPart;
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
