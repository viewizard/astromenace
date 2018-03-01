/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "space_explosion.h"
#include "../../../game.h"


//-----------------------------------------------------------------------------
// Создание взрыва из частей объекта
//-----------------------------------------------------------------------------
cSpaceExplosion::cSpaceExplosion(cObject3D *Object, int ExplType, const sVECTOR3D &ExplLocation,
				 float Speed, int ObjectPieceNum, bool NeedExplosionSFX)
{
	// если ObjectPieceNum==-1 полный взрыв... не частями
	// например ядерная ракета взрывает все объекты, или это астероид

	TimeLastUpdate = Object->TimeLastUpdate;
	ExplosionTypeByClass = 1;
	// 1-взрыв на части, 2-разлет геометрии
	int InternalExplosionType = 0;

	// общий - пенальти, если не игрок
	float CurrentPenalty = GameNPCWeaponPenalty*1.0f+1.0f; // чуть больше убираем...
	// если игрок или свои - ничего не надо...
	if (Object->ObjectStatus >= 2) CurrentPenalty = 1.0f;
	// сохраняем статус объекта, чтобы правильно создавать части-снаряды и обломки
	ObjectStatus = Object->ObjectStatus;
	// сохраняем тип взрыва
	ExplosionType = ExplType;
	// для регулирования ускорения разлета геометрии
	float MeshAcc = 0.0f;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// малый внутренний взрыв для астероидов
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 1) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 10.0f;

		// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
		SetLocation(Object->Location);
		Orientation = Object->Orientation;

		// эффект
		GraphicFXQuantity = 1;
		GraphicFX = new cParticleSystem*[GraphicFXQuantity];
		for (int i=0; i<GraphicFXQuantity; i++) {
			GraphicFX[i] = nullptr;
		}

		// установка эффекта
		float tRadius = Object->Radius/2.0f;
		GraphicFX[0] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[0], 1);
		GraphicFX[0]->Speed = 1.5f*Object->Radius;
		GraphicFX[0]->SpeedVar   = vw_Randf0;
		GraphicFX[0]->MoveSystem(Object->Location);
		GraphicFX[0]->ParticlesPerSec = (int)(10*Object->Radius);
		GraphicFX[0]->Direction = Object->Orientation;
		GraphicFX[0]->CreationType = 2;
		GraphicFX[0]->CreationSize = sVECTOR3D(tRadius,tRadius,tRadius);
		GraphicFX[0]->Life       = 1.5f;
		GraphicFX[0]->SizeStart  = tRadius/1.5f;

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(3*Object->Radius) + (int)(vw_Randf0*3*Object->Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *Projectile  = new cProjectile;
			Projectile->Create(1);
			Projectile->SetLocation(Location);

			Projectile->SetRotation(sVECTOR3D(360.0f*vw_Randf0, 360.0f*vw_Randf0, 360.0f*vw_Randf0));
			sVECTOR3D TM1 = Object->Orientation^Speed;
			Projectile->Orientation = TM1 + (Projectile->Orientation^(Object->Radius*6.0f));
			Projectile->Orientation.Normalize();

			for (int j=0; j<Projectile->GraphicFXQuantity; j++) {
				Projectile->GraphicFX[j]->Direction = Projectile->Orientation^-1;
				Projectile->GraphicFX[j]->Speed = 1.5f;
			}
			Projectile->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			Projectile->Speed = Speed + Object->Radius*2.0f + 2.0f*vw_Randf0;
			Projectile->SpeedEnd = 0.0f;
			Projectile->SpeedStart = Projectile->Speed;
			Projectile->Lifetime = Projectile->Age = 1.0f+vw_Randf0;
			Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
			Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;
		}
	}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// средний внутренний взрыв для пришельцев
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 2) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 20.0f;

		// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
		SetLocation(Object->Location);
		Orientation = Object->Orientation;

		// эффект
		GraphicFXQuantity = 3;
		GraphicFX = new cParticleSystem*[GraphicFXQuantity];
		for (int i=0; i<GraphicFXQuantity; i++) {
			GraphicFX[i] = nullptr;
		}

		// установка эффекта
		GraphicFX[1] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[1], 3);
		GraphicFX[1]->MoveSystem(Object->Location);
		GraphicFX[1]->ParticlesPerSec = (int)(2.5f*Object->Radius);
		GraphicFX[1]->Direction = Object->Orientation;
		GraphicFX[1]->CreationSize = sVECTOR3D(Object->Radius/4,Object->Radius/4,Object->Radius/4);

		GraphicFX[2] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[2], 4);
		GraphicFX[2]->Speed = Object->Radius/3.0f;
		GraphicFX[2]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[2]->MoveSystem(Object->Location);
		if (Speed!=0) GraphicFX[2]->Theta      = 360.00f/(Speed/8);
		GraphicFX[2]->ParticlesPerSec = (int)(2.5*Object->Radius);
		GraphicFX[2]->Direction = Object->Orientation;
		GraphicFX[2]->NeedStop = false;

		GraphicFX[0] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[0], 2);
		GraphicFX[0]->Speed = Object->Radius*1.2f;
		GraphicFX[0]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[0]->MoveSystem(Object->Location);
		if (Speed!=0) GraphicFX[0]->Theta      = 360.00f/(Speed/8);
		GraphicFX[0]->ParticlesPerSec = (int)(10*Object->Radius);
		GraphicFX[0]->Direction = Object->Orientation;

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(Object->Radius) + (int)(vw_Randf0*Object->Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *Projectile  = new cProjectile;
			Projectile->Create(1);
			Projectile->SetLocation(Location);

			Projectile->SetRotation(sVECTOR3D(360.0f*vw_Randf0, 360.0f*vw_Randf0, 360.0f*vw_Randf0));
			sVECTOR3D TM1 = Object->Orientation^Speed;
			Projectile->Orientation = TM1 + (Projectile->Orientation^(Object->Radius/4.0f));
			Projectile->Orientation.Normalize();

			for (int j=0; j<Projectile->GraphicFXQuantity; j++) {
				Projectile->GraphicFX[j]->Direction = Projectile->Orientation^-1;
				Projectile->GraphicFX[j]->Speed = 2.5f;
				Projectile->GraphicFX[j]->ColorStart.r = 0.30f;
				Projectile->GraphicFX[j]->ColorStart.g = 1.00f;
				Projectile->GraphicFX[j]->ColorStart.b = 1.00f;
				Projectile->GraphicFX[j]->ColorEnd.r = 0.00f;
				Projectile->GraphicFX[j]->ColorEnd.g = 1.00f;
				Projectile->GraphicFX[j]->ColorEnd.b = 1.00f;
			}
			Projectile->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			Projectile->Speed = Speed + Object->Radius/2.0f + 2.0f*vw_Randf0;
			Projectile->SpeedEnd = 0.0f;
			Projectile->SpeedStart = Projectile->Speed;
			Projectile->Lifetime = Projectile->Age = 1.5f+vw_Randf0;
			Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
			Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;
		}
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// средний внутренний взрыв для больших пиратов c разлетем частей
	// + именно так взрываем свои корабли и корабль игрока...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 31) {
		InternalExplosionType = 1;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв внутренний части (пираты, земляне)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 32) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 20.0f;

		// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
		SetLocation(Object->Location);
		Orientation = Object->Orientation;

		// эффект
		GraphicFXQuantity = 2;
		GraphicFX = new cParticleSystem*[GraphicFXQuantity];
		for (int i=0; i<GraphicFXQuantity; i++) {
			GraphicFX[i] = nullptr;
		}

		// установка эффекта
		GraphicFX[1] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[1], 6);
		GraphicFX[1]->Speed = Object->Radius/1.4f;
		GraphicFX[1]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[1]->MoveSystem(Object->Location);
		if (Speed>0.01f) GraphicFX[1]->Theta      = 360.00f/(Speed/8);
		GraphicFX[1]->ParticlesPerSec = (int)(7*Object->Radius);
		GraphicFX[1]->Direction = Object->Orientation;

		GraphicFX[0] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[0], 5);
		GraphicFX[0]->Speed = Object->Radius*1.1f;
		GraphicFX[0]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[0]->MoveSystem(Object->Location);
		if (Speed>0.01f) GraphicFX[0]->Theta      = 360.00f/(Speed/8);
		GraphicFX[0]->ParticlesPerSec = (int)(7*Object->Radius);
		GraphicFX[0]->Direction = Object->Orientation;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв на части (босс пришельцев)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 33) {
		InternalExplosionType = 1;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв внутренний части (босс пришельцев)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 34) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 20.0f;

		// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
		SetLocation(Object->Location);
		Orientation = Object->Orientation;

		// эффект
		GraphicFXQuantity = 2;
		GraphicFX = new cParticleSystem*[GraphicFXQuantity];
		for (int i=0; i<GraphicFXQuantity; i++) {
			GraphicFX[i] = nullptr;
		}

		// установка эффекта
		GraphicFX[1] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[1], 12);
		GraphicFX[1]->Speed = Object->Radius/1.4f;
		GraphicFX[1]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[1]->MoveSystem(Object->Location);
		if (Speed!=0) GraphicFX[1]->Theta      = 360.00f/(Speed/8);
		GraphicFX[1]->ParticlesPerSec = (int)(7*Object->Radius);
		GraphicFX[1]->Direction = Object->Orientation;

		GraphicFX[0] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[0], 11);
		GraphicFX[0]->Speed = Object->Radius*1.1f;
		GraphicFX[0]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[0]->MoveSystem(Object->Location);
		if (Speed!=0) GraphicFX[0]->Theta      = 360.00f/(Speed/8);
		GraphicFX[0]->ParticlesPerSec = (int)(7*Object->Radius);
		GraphicFX[0]->Direction = Object->Orientation;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// средний внутренний взрыв для землян и пиратов
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 3) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 20.0f;

		// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
		SetLocation(Object->Location);
		Orientation = Object->Orientation;

		// эффект
		GraphicFXQuantity = 2;
		GraphicFX = new cParticleSystem*[GraphicFXQuantity];
		for (int i=0; i<GraphicFXQuantity; i++) {
			GraphicFX[i] = nullptr;
		}

		// установка эффекта
		GraphicFX[1] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[1], 6);
		GraphicFX[1]->Speed = Object->Radius/1.3f;
		GraphicFX[1]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[1]->MoveSystem(Object->Location);
		if (Speed!=0) GraphicFX[1]->Theta      = 360.00f/(Speed/8);
		GraphicFX[1]->ParticlesPerSec = (int)(7*Object->Radius);
		GraphicFX[1]->Direction = Object->Orientation;

		GraphicFX[0] = new cParticleSystem;
		SetExplosionGFX(GraphicFX[0], 5);
		GraphicFX[0]->Speed = Object->Radius*1.1f;
		GraphicFX[0]->SpeedVar   = 1.0f*vw_Randf0;
		GraphicFX[0]->MoveSystem(Object->Location);
		if (Speed!=0) GraphicFX[0]->Theta      = 360.00f/(Speed/8);
		GraphicFX[0]->ParticlesPerSec = (int)(7*Object->Radius);
		GraphicFX[0]->Direction = Object->Orientation;

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(0.5f*Object->Radius) + (int)(vw_Randf0*Object->Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *Projectile = new cProjectile;
			Projectile->Create(1);
			Projectile->SetLocation(Location);

			Projectile->SetRotation(sVECTOR3D(360.0f*vw_Randf0, 360.0f*vw_Randf0, 360.0f*vw_Randf0));
			sVECTOR3D TM1 = Object->Orientation^Speed;
			Projectile->Orientation = TM1 + (Projectile->Orientation^(Object->Radius/2.0f));
			Projectile->Orientation.Normalize();

			for (int j=0; j<Projectile->GraphicFXQuantity; j++) {
				Projectile->GraphicFX[j]->Direction = Projectile->Orientation^-1;
				Projectile->GraphicFX[j]->Speed = 2.5f;
			}
			Projectile->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			Projectile->Speed = Speed + Object->Radius/2.0f + 2.0f*vw_Randf0;
			Projectile->SpeedEnd = 0.0f;
			Projectile->SpeedStart = Projectile->Speed;
			Projectile->Lifetime = Projectile->Age = 1.5f+vw_Randf0;
			Projectile->DamageHull = Projectile->DamageHull/CurrentPenalty;
			Projectile->DamageSystems = Projectile->DamageSystems/CurrentPenalty;
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// распад ракеты, если она папала в облако фларес
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 4) {
		InternalExplosionType = 2;
		// резко тормозим
		OldSpeed = Speed/5.0f;
		Lifetime = 1.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 5.0f;
	}











	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв на части для больших кораблей
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
		for (int i=0; i<Object->DrawObjectQuantity; i++) {
			// могут быть пустые группы, убираем их и идем смотрим на следующую группу
			if (Object->DrawObjectList[i].VertexCount == 0) continue;

			cShipPart *ShipPart;
			ShipPart = new cShipPart;
			ShipPart->ObjectType = 8;
			ShipPart->ShowDeleteOnHide = 0;

			// только одна текстура (!) 2-ю для подстветки не тянем
			ShipPart->Texture = new sTexture*[1];
			ShipPart->Texture[0] = Object->Texture[i];
			if (Object->NormalMap != nullptr)
				if (Object->NormalMap[i] != nullptr) {
					ShipPart->NormalMap = new sTexture*[1];
					ShipPart->NormalMap[0] = Object->NormalMap[i];
				}

			// берем то, что нужно
			ShipPart->DrawObjectQuantity = 1;
			ShipPart->DrawObjectList = new sObjectBlock[ShipPart->DrawObjectQuantity];
			// копируем данные (тут уже все есть, с указателями на вбо и массив геометрии)
			memcpy(&(ShipPart->DrawObjectList[0]), &(Object->DrawObjectList[i]), sizeof(Object->DrawObjectList[0]));
			// если надо было удалить в объекте - ставим не удалять, удалим вместе с этой частью
			if (Object->DrawObjectList[i].NeedDestroyDataInObjectBlock) {
				Object->DrawObjectList[i].NeedDestroyDataInObjectBlock = false;
				ShipPart->DrawObjectList[0].NeedDestroyDataInObjectBlock = true;
			}

			// резервируем память для HitBB
			ShipPart->HitBBLocation = new sVECTOR3D[ShipPart->DrawObjectQuantity];
			ShipPart->HitBBRadius2 = new float[ShipPart->DrawObjectQuantity];
			ShipPart->HitBBSize = new sVECTOR3D[ShipPart->DrawObjectQuantity];
			ShipPart->HitBB = new sVECTOR3D*[ShipPart->DrawObjectQuantity];
			for (int i1=0; i1<ShipPart->DrawObjectQuantity; i1++) {
				ShipPart->HitBB[i1] = new sVECTOR3D[8];
			}


			// находим точку локального положения объекта в моделе
			sVECTOR3D LocalLocation = Object->DrawObjectList[i].Location;
			vw_Matrix33CalcPoint(LocalLocation, Object->CurrentRotationMat);
			LocalLocation = Object->HitBBLocation[i]-LocalLocation;
			vw_Matrix33CalcPoint(LocalLocation, InvRotationMat);
			// и меняем внутрее положение
			ShipPart->DrawObjectList[0].Location = LocalLocation^(-1.0f);

			// находим все данные по геометрии
			ShipPart->InitByDrawObjectList();

			// установка текущего положения и поворота
			ShipPart->SetLocation(Object->Location+Object->HitBBLocation[i]);
			ShipPart->SetRotation(Object->Rotation);


			ShipPart->Speed = Speed-2*vw_Randf1;

			ShipPart->RotationSpeed.x = 2.0f*vw_Randf0;
			ShipPart->RotationSpeed.y = 2.0f*vw_Randf0;

			ShipPart->StrengthStart = ShipPart->Strength = 1.0f;
			ShipPart->ObjectStatus = Object->ObjectStatus;
			ShipPart->ShowStrength = false;


			// взрываем тот объект, в который попали
			int NeedExplosionType = 32;
			switch (ExplType) {
			case 31: // взрыв пиратов или землян
				NeedExplosionType = 32;
				break;
			case 33: // взрыв боссов пришельцев
				// взорвем все в течении 3-х секунд
				ShipPart->BossPartCountDown = 3.0f * vw_Randf1;
				NeedExplosionType = 34;
				break;
			}
			if (ObjectPieceNum != -1)
				if (ObjectPieceNum == i) {
					new cSpaceExplosion(ShipPart, NeedExplosionType, ShipPart->Location, ShipPart->Speed, -1);
					delete ShipPart;
				}
		}

		Lifetime = 0.0f;
	}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв с разлетом геометрии
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (InternalExplosionType == 2) {
		// AABB нужен, т.к. по нему рисуем, если во фруструме
		// копируем AABB, т.к. они сейчас подходят...
		AABB[0] = Object->AABB[0];
		AABB[1] = Object->AABB[1];
		AABB[2] = Object->AABB[2];
		AABB[3] = Object->AABB[3];
		AABB[4] = Object->AABB[4];
		AABB[5] = Object->AABB[5];
		AABB[6] = Object->AABB[6];
		AABB[7] = Object->AABB[7];

		// поправка в зависимости от скорости объекта до взрыва
		VelocityOrientation = Object->Orientation;


		// общее кол-во элементов прорисовки (т.к. может быть не один объект + разные настройки качества взрыва)
		int TotalCount = 0;

		// создаем последовательность
		DrawObjectQuantity = Object->DrawObjectQuantity;
		Texture = new sTexture*[DrawObjectQuantity];
		DrawObjectList = new sObjectBlock[DrawObjectQuantity];

		// смотрим по настройкам сколько пропускать
		int NeedIn = Setup.VisualEffectsQuality;

		// составляем данные для взрыва
		for (int i=0; i<DrawObjectQuantity; i++) {
			Texture[i] = Object->Texture[i];
			// копируем данные
			memcpy(&(DrawObjectList[i]), &(Object->DrawObjectList[i]), sizeof(Object->DrawObjectList[0]));
			// делаем изменения
			DrawObjectList[i].VBO = nullptr;
			DrawObjectList[i].VertexBuffer = nullptr;
			DrawObjectList[i].IBO = nullptr;
			DrawObjectList[i].IndexBuffer = nullptr;
			DrawObjectList[i].VAO = nullptr;
			DrawObjectList[i].NeedDestroyDataInObjectBlock = true; // удалять в объекте
			DrawObjectList[i].RangeStart = 0;
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles = nullptr;
			DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount = 0;

			// делаем поворот геометрии объекта чтобы правильно сделать разлет частиц
			DrawObjectList[i].VertexCount = 0;
			int k = 0;
			int NeedInCur = NeedIn;

			int tricount = 0;

			// если 2 текстурных координаты, нужно убрать 2-ю...
			if ((Object->DrawObjectList[i].VertexFormat & 0x000000F) >= 2) {
				DrawObjectList[i].VertexFormat = (Object->DrawObjectList[i].VertexFormat & 0xFFFFFF0) | RI_1_TEX;
			} else {
				DrawObjectList[i].VertexFormat = Object->DrawObjectList[i].VertexFormat;
			}
			DrawObjectList[i].VertexStride = Object->DrawObjectList[i].VertexStride;


			// если у нас включены и работают шейдеры, надо приготовить место для данных + изменить формат и шаг
			if (Setup.UseGLSL) {
				DrawObjectList[i].VertexStride = 3+3+6;
				DrawObjectList[i].VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX;
			}

			// выделяем память для данных
			// в отличии от снарядов - тут работаем с VertexBufferLimitedBySizeTriangles, чтобы сделать более красивый взрыв из мелких треугольников
			DrawObjectList[i].VertexBuffer = new float[DrawObjectList[i].VertexStride*Object->DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount];



			// матрица для учета внутреннего состояния объекта
			float TransMatTMP[16];
			vw_Matrix44Identity(TransMatTMP);
			float TransMatTMPNorm[9];
			vw_Matrix33Identity(TransMatTMPNorm);
			// если нужно - создаем матрицу, иначе - копируем ее
			if (Object->DrawObjectList[i].Rotation.x != 0.0f ||
			    Object->DrawObjectList[i].Rotation.y != 0.0f ||
			    Object->DrawObjectList[i].Rotation.z != 0.0f) {
				vw_Matrix44CreateRotate(TransMatTMP, Object->DrawObjectList[i].Rotation);
				vw_Matrix33CreateRotate(TransMatTMPNorm, Object->DrawObjectList[i].Rotation);
			}
			vw_Matrix44Translate(TransMatTMP, DrawObjectList[i].Location);
			vw_Matrix33Mult(TransMatTMPNorm, Object->CurrentRotationMat);


			sVECTOR3D TMP;
			for (int j=0; j<Object->DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount; j++) {
				if (NeedInCur <= 0) {
					int j1 = k*DrawObjectList[i].VertexStride;
					int j2 = j*Object->DrawObjectList[i].VertexStride;

					TMP.x = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2];
					TMP.y = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+1];
					TMP.z = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+2];
					vw_Matrix44CalcPoint(TMP, TransMatTMP);
					vw_Matrix33CalcPoint(TMP, Object->CurrentRotationMat);
					// координаты
					DrawObjectList[i].VertexBuffer[j1] = TMP.x;
					DrawObjectList[i].VertexBuffer[j1+1] = TMP.y;
					DrawObjectList[i].VertexBuffer[j1+2] = TMP.z;
					// нормали
					TMP.x = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+3];
					TMP.y = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+4];
					TMP.z = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+5];
					vw_Matrix33CalcPoint(TMP, TransMatTMPNorm);
					DrawObjectList[i].VertexBuffer[j1+3] = TMP.x;
					DrawObjectList[i].VertexBuffer[j1+4] = TMP.y;
					DrawObjectList[i].VertexBuffer[j1+5] = TMP.z;
					// текстура
					DrawObjectList[i].VertexBuffer[j1+6] = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+6];
					DrawObjectList[i].VertexBuffer[j1+7] = Object->DrawObjectList[i].VertexBufferLimitedBySizeTriangles[j2+7];

					DrawObjectList[i].VertexCount ++;
					k++;

					if (tricount == 2) NeedInCur = NeedIn;
				} else {
					if (tricount == 2) NeedInCur --;
				}

				tricount ++;
				if (tricount >= 3) tricount=0;
			}


			// создаем индексный буфер блока
			DrawObjectList[i].IndexBuffer = new unsigned int[DrawObjectList[i].VertexCount];
			for (int j=0; j<DrawObjectList[i].VertexCount; j++) DrawObjectList[i].IndexBuffer[j] = j;



			DrawObjectList[i].Location = sVECTOR3D(0.0f,0.0f,0.0f);
			DrawObjectList[i].Rotation = sVECTOR3D(0.0f,0.0f,0.0f);

			TotalCount += DrawObjectList[i].VertexCount;
		}

		// расстояние от центра до крайней точки
		float Diag = (Object->Length*Object->Length)+
			     (Object->Height*Object->Height) + (Object->Width*Object->Width);

		// для каждого треугольника - свои данные
		int Count = 0;
		sVECTOR3D TMP;
		ExplosionPieceData = new sExplosionPiece[TotalCount/3];
		for (int j=0; j<DrawObjectQuantity; j++) {
			for (int i=0; i<DrawObjectList[j].VertexCount; i+=3) {
				ExplosionPieceData[Count].Velocity.x = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i];
				ExplosionPieceData[Count].Velocity.x += DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)];
				ExplosionPieceData[Count].Velocity.x += DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)];
				ExplosionPieceData[Count].Velocity.x = ExplosionPieceData[Count].Velocity.x/3.0f;
				ExplosionPieceData[Count].Velocity.y = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+1];
				ExplosionPieceData[Count].Velocity.y += DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+1];
				ExplosionPieceData[Count].Velocity.y += DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+1];
				ExplosionPieceData[Count].Velocity.y = ExplosionPieceData[Count].Velocity.y/3.0f;
				ExplosionPieceData[Count].Velocity.z = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+2];
				ExplosionPieceData[Count].Velocity.z += DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+2];
				ExplosionPieceData[Count].Velocity.z += DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+2];
				ExplosionPieceData[Count].Velocity.z = ExplosionPieceData[Count].Velocity.z/3.0f;

				// находим расстояние, чтобы включить его
				float dist = ExplosionPieceData[Count].Velocity.x*ExplosionPieceData[Count].Velocity.x +
					     ExplosionPieceData[Count].Velocity.y*ExplosionPieceData[Count].Velocity.y+
					     ExplosionPieceData[Count].Velocity.z*ExplosionPieceData[Count].Velocity.z + vw_Randf0;


				float Acc = (MeshAcc/1000.0f)*(Diag/dist)*(MeshAcc/Object->Radius);
				if (Acc > MeshAcc) Acc = MeshAcc+vw_Randf0;
				if (Acc < (-1.0f*MeshAcc)) Acc = (-1.0f*MeshAcc)-vw_Randf0;


				// записываем центр треугольника, оно же базовое ускорение + цент UV, для передачи шейдеру
				if (Setup.UseGLSL) {
					// Velocity/центр треугольника
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+8] = ExplosionPieceData[Count].Velocity.x;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+9] = ExplosionPieceData[Count].Velocity.y;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+10] = ExplosionPieceData[Count].Velocity.z;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+8] = ExplosionPieceData[Count].Velocity.x;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+9] = ExplosionPieceData[Count].Velocity.y;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+10] = ExplosionPieceData[Count].Velocity.z;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+8] = ExplosionPieceData[Count].Velocity.x;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+9] = ExplosionPieceData[Count].Velocity.y;
					DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+10] = ExplosionPieceData[Count].Velocity.z;
					// acc
					if (dist/Diag < 0.01f) {
						DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+11] = Acc+4.0f*vw_Randf0;
						DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+11] = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+11];
						DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+11] = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+11];
					} else {
						DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+11] = Acc;
						DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+11] = Acc;
						DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+11] = Acc;
					}

				}


				if (dist/Diag < 0.01f)
					ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity^(Acc+4.0f*vw_Randf0);
				else
					ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity^Acc;


				ExplosionPieceData[Count].Life = (Lifetime-1.0f)+vw_Randf1;
				if (ExplosionPieceData[Count].Life < 0.0f) ExplosionPieceData[Count].Life = 0.0f;


				// делаем анализ для ААBB, смотрим отлет частицы
				float tmpSpeed = ExplosionPieceData[Count].Velocity.x*ExplosionPieceData[Count].Velocity.x +
						 ExplosionPieceData[Count].Velocity.y*ExplosionPieceData[Count].Velocity.y +
						 ExplosionPieceData[Count].Velocity.z*ExplosionPieceData[Count].Velocity.z;
				if (tmpSpeed > AABBSpeed) AABBSpeed = tmpSpeed;

				Count++;
			}



			// пересоздаем буферы vbo и vao
			// удаляем старые буферы, если они есть, создаем новые
			// ! индексный буфер не трогаем, его не надо пересоздавать вообще

			if (DrawObjectList[j].VBO != nullptr) {
				vw_DeleteVBO(*DrawObjectList[j].VBO);
				delete DrawObjectList[j].VBO;
				DrawObjectList[j].VBO = nullptr;
			}
			if (DrawObjectList[j].VAO != nullptr) {
				vw_DeleteVAO(*DrawObjectList[j].VAO);
				delete DrawObjectList[j].VAO;
				DrawObjectList[j].VAO = nullptr;
			}

			// делаем VBO
			DrawObjectList[j].VBO = new unsigned int;
			if (!vw_BuildVBO(DrawObjectList[j].VertexCount, DrawObjectList[j].VertexBuffer, DrawObjectList[j].VertexStride, DrawObjectList[j].VBO)) {
				delete DrawObjectList[j].VBO;
				DrawObjectList[j].VBO = nullptr;
			}

			// делаем IBO, создаем его один раз, если его нет
			if (DrawObjectList[j].IBO == nullptr) {
				DrawObjectList[j].IBO = new unsigned int;
				if (!vw_BuildIBO(DrawObjectList[j].VertexCount, DrawObjectList[j].IndexBuffer, DrawObjectList[j].IBO)) {
					delete DrawObjectList[j].IBO;
					DrawObjectList[j].IBO = nullptr;
				}
			}

			// делаем VAO
			DrawObjectList[j].VAO = new unsigned int;
			if (!vw_BuildVAO(DrawObjectList[j].VAO, DrawObjectList[j].VertexCount, DrawObjectList[j].VertexFormat, DrawObjectList[j].VertexBuffer,
					 DrawObjectList[j].VertexStride*sizeof(float), DrawObjectList[j].VBO,
					 DrawObjectList[j].RangeStart, DrawObjectList[j].IndexBuffer, DrawObjectList[j].IBO)) {
				delete DrawObjectList[j].VAO;
				DrawObjectList[j].VAO = nullptr;
			}


			// установки по шейдеру для объекта
			if (Setup.UseGLSL) {
				DrawObjectList[j].ShaderType = 2;
				// дельта скорости
				DrawObjectList[j].ShaderData[0] = 1.0f;
				// общий коэф расстояния
				DrawObjectList[j].ShaderData[1] = 0.0f;
			}

		}
		// тк был квадрат, теперь вытягиваем нормальное значение
		AABBSpeed = sqrtf(AABBSpeed);
	}













	// дальше, если не видем точку взрыва не делать... проверяем...
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0])) {
		return;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// звуковые спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedExplosionSFX) {

		float fVol = 1.0f;

		switch (ExplType) {
		// малый взрыв
		case 1:
			// астероид
			Audio_PlaySound3D(33, fVol, ExplLocation, false, 2);
			break;

		// взрыв
		case 2:
			// пришельцы
			Audio_PlaySound3D(7, fVol, ExplLocation, false, 2);
			break;
		case 3:
			// земляне, пираты
			Audio_PlaySound3D(8, fVol, ExplLocation, false, 2);
			break;
		// case 31:
		case 32:
			// внутренняя часть (пираты, земляне)
			//fVol = fVol/2; // ум. т.к. их там очень много
			Audio_PlaySound3D(8, fVol, ExplLocation, false, 2);
			break;
		//case 33:
		case 34:
			// внутренняя часть (босс пришельцев)
			Audio_PlaySound3D(7, fVol, ExplLocation, false, 2);
			break;
		}
	}




	// делаем сотрясание камеры, если нужно
	switch (ExplType) {
	// малый взрыв
	case 1:
		// астероид
		GameCameraSetExplosion(ExplLocation, 0.2f);
		break;

	// взрыв
	case 2:
		// пришельцы
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	case 3:
	//case 31:
	case 32:
		// земляне, пираты
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	//case 33:
	case 34:
		// босс пришельцев
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;
	}

}
