/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


/// подключаем нужные файлы
#include "GroundExplosion.h"
#include "../../../Game.h"



//-----------------------------------------------------------------------------
// Создание взрыва из частей объекта
//-----------------------------------------------------------------------------
void CGroundExplosion::Create(CGroundObject *Object, int ExplType, VECTOR3D ExplLocation, int ObjectPieceNum, bool NeedExplosionSFX)
{
	TimeLastUpdate = Object->TimeLastUpdate;
	ExplosionTypeByClass = 3;
	// тип взрыва, используем только один, но все равно нужно для проверки (может передали левый ExplType)
	// 1-взрыв на части
	int InternalExplosionType = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 1 - взрыв постройки (турелей)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 1)
	{
		InternalExplosionType = 1;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2 - взрыв транспорта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 2)
	{
		InternalExplosionType = 1;
	}













	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв на части
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (InternalExplosionType == 1)
	{
		// строим обратную матрицу
		float InvRotationMat[9];
		// сохраняем старые значения + пересчет новых
		memcpy(InvRotationMat, Object->CurrentRotationMat, 9*sizeof(float));
		// делаем инверсную старую матрицу
		Matrix33InverseRotate(InvRotationMat);

		// содаем части, отделяем их от общей модели
		// ставим свои ориентейшины и скорость
		for (int i=0; i<Object->DrawObjectQuantity; i++)
		{
			// могут быть пустые группы, убираем их и идем смотрим на следующую группу
			// или это гусеница, тоже ее пропускаем
			if (Object->DrawObjectList[i].VertexCount == 0 || Object->TrackObjectNum == i)
			{
				continue;
			}
			else
			{
				// создаем часть
				CShipPart *ShipPart;
				ShipPart = new CShipPart;
				ShipPart->ObjectType = 8;
				ShipPart->ShowDeleteOnHide = 0;

				// только одна текстура (!) 2-ю для подстветки не тянем
				ShipPart->Texture = new eTexture*[1];
				ShipPart->Texture[0] = Object->Texture[i];
				if (Object->NormalMap != 0)
				if (Object->NormalMap[i] != 0)
				{
					ShipPart->NormalMap = new eTexture*[1];
					ShipPart->NormalMap[0] = Object->NormalMap[i];
				}

				// берем то, что нужно
				ShipPart->DrawObjectQuantity = 1;
				ShipPart->DrawObjectList = new eObjectBlock[ShipPart->DrawObjectQuantity];
				// копируем данные (тут уже все есть, с указателями на вбо и массив геометрии)
				memcpy(&(ShipPart->DrawObjectList[0]), &(Object->DrawObjectList[i]), sizeof(eObjectBlock));
				// берем стандартные шейдеры
				ShipPart->DrawObjectList[0].ShaderType = 1;
				// если надо было удалить в объекте - ставим не удалять, удалим вместе с этой частью
				if (Object->DrawObjectList[i].NeedDestroyDataInObjectBlock)
				{
					Object->DrawObjectList[i].NeedDestroyDataInObjectBlock = false;
					ShipPart->DrawObjectList[0].NeedDestroyDataInObjectBlock = true;
				}

				// резервируем память для HitBB
				ShipPart->HitBBLocation = new VECTOR3D[ShipPart->DrawObjectQuantity];
				ShipPart->HitBBRadius2 = new float[ShipPart->DrawObjectQuantity];
				ShipPart->HitBBSize = new VECTOR3D[ShipPart->DrawObjectQuantity];
				ShipPart->HitBB = new VECTOR3D*[ShipPart->DrawObjectQuantity];
				for (int i1=0; i1<ShipPart->DrawObjectQuantity; i1++)
				{
					ShipPart->HitBB[i1] = new VECTOR3D[8];
				}


				// находим точку локального положения объекта в моделе
				VECTOR3D LocalLocation = Object->DrawObjectList[i].Location;
				Matrix33CalcPoint(&LocalLocation, Object->CurrentRotationMat);
				LocalLocation = Object->HitBBLocation[i]-LocalLocation;
				Matrix33CalcPoint(&LocalLocation, InvRotationMat);
				// и меняем внутрее положение
				ShipPart->DrawObjectList[0].Location = LocalLocation^(-1.0f);

				// находим все данные по геометрии
				ShipPart->InitByDrawObjectList();

				// установка текущего положения и поворота
				ShipPart->SetLocation(Object->Location+Object->HitBBLocation[i]);
				ShipPart->SetRotation(Object->Rotation);


				if (ExplType == 1)
				{
					ShipPart->Speed = 0.0f;
					VECTOR3D VelocityTMP = ShipPart->Location - ExplLocation;
					if (ShipPart->Radius != 0) ShipPart->Velocity = VelocityTMP^(1.0f/ShipPart->Radius);
					else ShipPart->Velocity = VelocityTMP^(1.0f+5.0f*vw_Randf1);

					ShipPart->RotationSpeed.x = 2.0f*vw_Randf0;
					ShipPart->RotationSpeed.y = 2.0f*vw_Randf0;
				}
				if (ExplType == 2)
				{
					// проверяем, это колесо или нет
					bool Wheel = false;
					for (int k=0; k<Object->WheelQuantity; k++)
					{
						if (Object->WheelObjectsNum[k] == i) Wheel = true;
					}

					if (Wheel)
					{
						//if(ShipPart->Speed != 0.0f) Speed-2*vw_Randf1;
						VECTOR3D VelocityTMP = ShipPart->Location - Object->Location;
						// делаем небольшой случайный доворот
						RotatePoint(&VelocityTMP, VECTOR3D(-5.0f-15.0f*vw_Randf1, 10.0f*vw_Randf0, 0.0f));
						if(ShipPart->Radius != 0.0f) ShipPart->Velocity = VelocityTMP^((1.0f+5.0f*vw_Randf1)/ShipPart->Radius);
						else ShipPart->Velocity = VelocityTMP^(1.0f+5.0f*vw_Randf1);

						ShipPart->RotationSpeed.x = 40.0f+80.0f*vw_Randf0;
						ShipPart->RotationSpeed.y = 40.0f+80.0f*vw_Randf0;
						ShipPart->RotationSpeed.z = 40.0f+80.0f*vw_Randf0;
					}
					else
					{
						//if(ShipPart->Speed != 0.0f) Speed-2*vw_Randf1;
						VECTOR3D VelocityTMP = ShipPart->Location - Object->Location;
						if(ShipPart->Radius != 0.0f) ShipPart->Velocity = VelocityTMP^(5.0f/ShipPart->Radius);
						else ShipPart->Velocity = VelocityTMP^(1.0f+5.0f*vw_Randf1);

						ShipPart->RotationSpeed.x = 2.0f*vw_Randf0;
						ShipPart->RotationSpeed.y = 2.0f*vw_Randf0;
						ShipPart->RotationSpeed.z = 2.0f*vw_Randf0;
					}
				}


				ShipPart->StrengthStart = ShipPart->Strength = 1.0f;
				ShipPart->ObjectStatus = Object->ObjectStatus;
				ShipPart->ShowStrength = false;


				if (ObjectPieceNum != -1)
				if (ObjectPieceNum == i)
				{
					// а теперь взрываем ту, в которую попали...
					CSpaceExplosion *TMPExplosion;
					TMPExplosion = new CSpaceExplosion;
					TMPExplosion->Create(ShipPart, 32, ShipPart->Location, ShipPart->Speed, -1);
					delete ShipPart;
				}
			}
		}

		Lifetime = 0.0f;
	}








	// дальше, если не видем точку взрыва не делать... проверяем...
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0]))
	{
		return;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (ExplType == 1 || ExplType == 2)
	{
		// постройки, транспорт
		GameCameraSetExplosion(ExplLocation, 0.5f); // делаем сотрясание камеры, если нужно
		if (NeedExplosionSFX) Audio_PlaySound3D(8, 1.0f, ExplLocation, false, 2);
	}


}

