/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: GroundExplosion.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


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

				// берем то, что нужно
				ShipPart->DrawObjectQuantity = 1;
				ShipPart->DrawObjectList = new eObjectBlock[ShipPart->DrawObjectQuantity];
				// копируем данные (тут уже все есть, с указателями на вбо и массив геометрии)
				memcpy(&(ShipPart->DrawObjectList[0]), &(Object->DrawObjectList[i]), sizeof(eObjectBlock));
				// если надо было удалить в объекте - ставим не удалять, удалим вместе с этой частью
				Object->DrawObjectList[i].VertexBufferDestrType = 0;

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
				LocalLocation += Object->HitBBLocation[i];

				VECTOR3D LocalLocationTmp = Object->HitBBLocation[i];
				Matrix33CalcPoint(&LocalLocationTmp, InvRotationMat);
				// и меняем внутрее положение
				ShipPart->DrawObjectList[0].Location = LocalLocationTmp^(-1.0f);


				// находим все данные по геометрии
				ShipPart->InitByDrawObjectList();

				// установка текущего положения и поворота
				ShipPart->SetLocation(Object->Location+LocalLocation);
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
					if (ShipPart != 0){delete ShipPart; ShipPart = 0;}
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
	// звуковые спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	float fVol = 1.0f;

	if (NeedExplosionSFX)
	switch (ExplType)
	{

		   // взрыв
        case 1:
        case 2:
			// постройки, транспорт
            Audio_PlaySound3D(8, fVol, ExplLocation, false, 2);
			break;

	}




    // делаем сотрясание камеры, если нужно
	switch (ExplType)
	{
	    // взрыв
        case 1:
        case 2:
            // постройки, транспорт
            GameCameraSetExplosion(ExplLocation, 0.5f);
			break;
	}



}







