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

#include "space_explosion.h"
#include "../../../config/config.h"
#include "../../../assets/audio.h"
#include "../../space_object/space_object.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

void GameCameraSetExplosion(sVECTOR3D Location, float Power);


//-----------------------------------------------------------------------------
// Создание взрыва из частей объекта
//-----------------------------------------------------------------------------
cSpaceExplosion::cSpaceExplosion(cObject3D &Object, int ExplType, const sVECTOR3D &ExplLocation,
				 float Speed, int ObjectChunkNum, bool NeedExplosionSFX)
{
	// если ObjectPieceNum==-1 полный взрыв... не частями
	// например ядерная ракета взрывает все объекты, или это астероид

	TimeLastUpdate = Object.TimeLastUpdate;
	ExplosionTypeByClass = 1;
	// 1-взрыв на части, 2-разлет геометрии
	int InternalExplosionType = 0;

	float CurrentPenalty{1.0f};
	if (ObjectStatus == eObjectStatus::Enemy)
		CurrentPenalty = static_cast<float>(GameEnemyWeaponPenalty) + 1.0f; // чуть больше убираем...

	// сохраняем статус объекта, чтобы правильно создавать части-снаряды и обломки
	ObjectStatus = Object.ObjectStatus;
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
		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		// эффект
		GraphicFX.resize(1);

		// установка эффекта
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
			sharedGFX->CreationSize = sVECTOR3D(tRadius, tRadius, tRadius);
			sharedGFX->Life = 1.5f;
			sharedGFX->SizeStart = tRadius / 1.5f;
		}

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(3 * Object.Radius) + (int)(vw_fRand0() * 3 * Object.Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *Projectile  = new cProjectile;
			Projectile->Create(1);
			Projectile->SetLocation(Location);

			Projectile->SetRotation(sVECTOR3D(360.0f*vw_fRand0(), 360.0f*vw_fRand0(), 360.0f*vw_fRand0()));
			sVECTOR3D TM1 = Object.Orientation ^ Speed;
			Projectile->Orientation = TM1 + (Projectile->Orientation ^ (Object.Radius * 6.0f));
			Projectile->Orientation.Normalize();

			for (auto &tmpGFX : Projectile->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = Projectile->Orientation ^ -1;
					sharedGFX->Speed = 1.5f;
				}
			}
			Projectile->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			Projectile->Speed = Speed + Object.Radius * 2.0f + 2.0f * vw_fRand0();
			Projectile->SpeedEnd = 0.0f;
			Projectile->SpeedStart = Projectile->Speed;
			Projectile->Lifetime = Projectile->Age = 1.0f+vw_fRand0();
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
		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		// эффект
		GraphicFX.resize(3);

		// установка эффекта
		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 3);
			sharedGFX->MoveSystem(Object.Location);
			sharedGFX->ParticlesPerSec = (int)(5 * Object.Radius);
			sharedGFX->Direction = Object.Orientation;
			sharedGFX->CreationSize = sVECTOR3D(Object.Radius / 4, Object.Radius / 4, Object.Radius / 4);
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

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(Object.Radius) + (int)(vw_fRand0() * Object.Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *Projectile  = new cProjectile;
			Projectile->Create(1);
			Projectile->SetLocation(Location);

			Projectile->SetRotation(sVECTOR3D(360.0f*vw_fRand0(), 360.0f*vw_fRand0(), 360.0f*vw_fRand0()));
			sVECTOR3D TM1 = Object.Orientation ^ Speed;
			Projectile->Orientation = TM1 + (Projectile->Orientation ^ (Object.Radius / 4.0f));
			Projectile->Orientation.Normalize();

			for (auto &tmpGFX : Projectile->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = Projectile->Orientation ^ -1;
					sharedGFX->Speed = 2.5f;
					sharedGFX->ColorStart.r = 0.30f;
					sharedGFX->ColorStart.g = 1.00f;
					sharedGFX->ColorStart.b = 1.00f;
					sharedGFX->ColorEnd.r = 0.00f;
					sharedGFX->ColorEnd.g = 1.00f;
					sharedGFX->ColorEnd.b = 1.00f;
				}
			}
			Projectile->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			Projectile->Speed = Speed + Object.Radius / 2.0f + 2.0f * vw_fRand0();
			Projectile->SpeedEnd = 0.0f;
			Projectile->SpeedStart = Projectile->Speed;
			Projectile->Lifetime = Projectile->Age = 1.5f+vw_fRand0();
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
		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		// эффект
		GraphicFX.resize(2);

		// установка эффекта
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
		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		// эффект
		GraphicFX.resize(2);

		// установка эффекта
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

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// средний внутренний взрыв для землян и пиратов
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ExplType == 3) {
		InternalExplosionType = 2;
		OldSpeed = Speed;
		Lifetime = 3.0f; // должно соотв. максимальной жизни частицы
		MeshAcc = 20.0f;

		// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
		SetLocation(Object.Location);
		Orientation = Object.Orientation;

		// эффект
		GraphicFX.resize(2);

		// установка эффекта
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

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(0.5f * Object.Radius) + (int)(vw_fRand0() * Object.Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *Projectile = new cProjectile;
			Projectile->Create(1);
			Projectile->SetLocation(Location);

			Projectile->SetRotation(sVECTOR3D(360.0f*vw_fRand0(), 360.0f*vw_fRand0(), 360.0f*vw_fRand0()));
			sVECTOR3D TM1 = Object.Orientation^Speed;
			Projectile->Orientation = TM1 + (Projectile->Orientation ^ (Object.Radius / 2.0f));
			Projectile->Orientation.Normalize();

			for (auto &tmpGFX : Projectile->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = Projectile->Orientation ^ -1;
					sharedGFX->Speed = 2.5f;
				}
			}
			Projectile->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			Projectile->Speed = Speed + Object.Radius / 2.0f + 2.0f * vw_fRand0();
			Projectile->SpeedEnd = 0.0f;
			Projectile->SpeedStart = Projectile->Speed;
			Projectile->Lifetime = Projectile->Age = 1.5f+vw_fRand0();
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
		memcpy(InvRotationMat, Object.CurrentRotationMat, 9 * sizeof(Object.CurrentRotationMat[0]));
		// делаем инверсную старую матрицу
		vw_Matrix33InverseRotate(InvRotationMat);

		// содаем части, отделяем их от общей модели
		// ставим свои ориентейшины и скорость
		for (unsigned int i = 0; i < Object.Chunks.size(); i++) {
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
			sharedSpaceDebris->Chunks[0].Location = LocalLocation^(-1.0f);

			// находим все данные по геометрии
			sharedSpaceDebris->MetadataInitialization();

			// установка текущего положения и поворота
			sharedSpaceDebris->SetLocation(Object.Location + Object.HitBB[i].Location);
			sharedSpaceDebris->SetRotation(Object.Rotation);


			sharedSpaceDebris->Speed = Speed - 2 * vw_fRand();

			sharedSpaceDebris->RotationSpeed.x = 2.0f * vw_fRand0();
			sharedSpaceDebris->RotationSpeed.y = 2.0f * vw_fRand0();

			sharedSpaceDebris->StrengthStart = sharedSpaceDebris->Strength = 1.0f;
			sharedSpaceDebris->ObjectStatus = Object.ObjectStatus;
			sharedSpaceDebris->ShowStrength = false;


			// взрываем тот объект, в который попали
			int NeedExplosionType = 32;
			switch (ExplType) {
			case 31: // взрыв пиратов или землян
				NeedExplosionType = 32;
				break;
			case 33: // взрыв боссов пришельцев
				// взорвем все в течении 3-х секунд
				sharedSpaceDebris->BossPartCountDown = 3.0f * vw_fRand();
				NeedExplosionType = 34;
				break;
			}
			if (ObjectChunkNum != -1)
				if (ObjectChunkNum == (int)i) {
					new cSpaceExplosion(*sharedSpaceDebris, NeedExplosionType, sharedSpaceDebris->Location, sharedSpaceDebris->Speed, -1);
					ReleaseSpaceObject(SpaceDebris);
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
		AABB[0] = Object.AABB[0];
		AABB[1] = Object.AABB[1];
		AABB[2] = Object.AABB[2];
		AABB[3] = Object.AABB[3];
		AABB[4] = Object.AABB[4];
		AABB[5] = Object.AABB[5];
		AABB[6] = Object.AABB[6];
		AABB[7] = Object.AABB[7];

		// поправка в зависимости от скорости объекта до взрыва
		VelocityOrientation = Object.Orientation;


		// общее кол-во элементов прорисовки (т.к. может быть не один объект + разные настройки качества взрыва)
		int TotalCount = 0;

		// копируем данные
		Texture = Object.Texture;
		Chunks = Object.Chunks;

		// смотрим по настройкам сколько пропускать
		// VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
		int NeedIn = GameConfig().VisualEffectsQuality;

		// составляем данные для взрыва
		for (unsigned int i = 0; i < Chunks.size(); i++) {
			// делаем изменения
			Chunks[i].VBO = 0;
			Chunks[i].IBO = 0;
			Chunks[i].VAO = 0;
			Chunks[i].NeedReleaseOpenGLBuffers = true; // удалять в объекте
			Chunks[i].RangeStart = 0;
			Chunks[i].IndexArray.reset();
			Chunks[i].VertexArrayWithSmallTriangles.reset();
			Chunks[i].VertexArrayWithSmallTrianglesCount = 0;

			// делаем поворот геометрии объекта чтобы правильно сделать разлет частиц
			Chunks[i].VertexQuantity = 0;
			int k = 0;
			int NeedInCur = NeedIn;

			int tricount = 0;

			// если 2 текстурных координаты, нужно убрать 2-ю...
			if ((Object.Chunks[i].VertexFormat & 0x000000F) >= 2)
				Chunks[i].VertexFormat = (Object.Chunks[i].VertexFormat & 0xFFFFFF0) | RI_1_TEX;
			else
				Chunks[i].VertexFormat = Object.Chunks[i].VertexFormat;

			Chunks[i].VertexStride = Object.Chunks[i].VertexStride;


			// если у нас включены и работают шейдеры, надо приготовить место для данных + изменить формат и шаг
			if (GameConfig().UseGLSL120) {
				Chunks[i].VertexStride = 3 + 3 + 6;
				Chunks[i].VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX;
			}

			// выделяем память для данных
			// в отличии от снарядов - тут работаем с VertexBufferLimitedBySizeTriangles, чтобы сделать более красивый взрыв из мелких треугольников
			Chunks[i].VertexArray.reset(new float[Chunks[i].VertexStride * Object.Chunks[i].VertexArrayWithSmallTrianglesCount],
						    std::default_delete<float[]>());


			// матрица для учета внутреннего состояния объекта
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
					// координаты
					Chunks[i].VertexArray.get()[j1] = TMP.x;
					Chunks[i].VertexArray.get()[j1 + 1] = TMP.y;
					Chunks[i].VertexArray.get()[j1 + 2] = TMP.z;
					// нормали
					TMP.x = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 3];
					TMP.y = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 4];
					TMP.z = Object.Chunks[i].VertexArrayWithSmallTriangles.get()[j2 + 5];
					vw_Matrix33CalcPoint(TMP, TransMatNorm);
					Chunks[i].VertexArray.get()[j1 + 3] = TMP.x;
					Chunks[i].VertexArray.get()[j1 + 4] = TMP.y;
					Chunks[i].VertexArray.get()[j1 + 5] = TMP.z;
					// текстура
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

			Chunks[i].Location = sVECTOR3D(0.0f,0.0f,0.0f);
			Chunks[i].Rotation = sVECTOR3D(0.0f,0.0f,0.0f);
			Chunks[i].GeometryAnimation = sVECTOR3D(0.0f,0.0f,0.0f);

			TotalCount += Chunks[i].VertexQuantity;
		}

		// расстояние от центра до крайней точки
		float Diag = Object.Length * Object.Length +
			     Object.Height * Object.Height +
			     Object.Width * Object.Width;

		// для каждого треугольника - свои данные
		int Count = 0;
		ExplosionPieceData = new sExplosionPiece[TotalCount/3];
		for (auto &tmpChunk : Chunks) {
			for (unsigned int i = 0; i < tmpChunk.VertexQuantity; i+=3) {
				ExplosionPieceData[Count].Velocity.x = tmpChunk.VertexArray.get()[tmpChunk.VertexStride *  i];
				ExplosionPieceData[Count].Velocity.x += tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1)];
				ExplosionPieceData[Count].Velocity.x += tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2)];
				ExplosionPieceData[Count].Velocity.x = ExplosionPieceData[Count].Velocity.x / 3.0f;
				ExplosionPieceData[Count].Velocity.y = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 1];
				ExplosionPieceData[Count].Velocity.y += tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 1];
				ExplosionPieceData[Count].Velocity.y += tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 1];
				ExplosionPieceData[Count].Velocity.y = ExplosionPieceData[Count].Velocity.y / 3.0f;
				ExplosionPieceData[Count].Velocity.z = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 2];
				ExplosionPieceData[Count].Velocity.z += tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 2];
				ExplosionPieceData[Count].Velocity.z += tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 2];
				ExplosionPieceData[Count].Velocity.z = ExplosionPieceData[Count].Velocity.z / 3.0f;

				// находим расстояние, чтобы включить его
				float dist = ExplosionPieceData[Count].Velocity.x * ExplosionPieceData[Count].Velocity.x +
					     ExplosionPieceData[Count].Velocity.y * ExplosionPieceData[Count].Velocity.y +
					     ExplosionPieceData[Count].Velocity.z * ExplosionPieceData[Count].Velocity.z + vw_fRand0();


				float Acc = (MeshAcc / 1000.0f) * (Diag / dist) * (MeshAcc / Object.Radius);
				if (Acc > MeshAcc)
					Acc = MeshAcc + vw_fRand0();
				if (Acc < -MeshAcc)
					Acc = -MeshAcc - vw_fRand0();


				// записываем центр треугольника, оно же базовое ускорение + цент UV, для передачи шейдеру
				if (GameConfig().UseGLSL120) {
					// Velocity/центр треугольника
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 8] = ExplosionPieceData[Count].Velocity.x;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 9] = ExplosionPieceData[Count].Velocity.y;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 10] = ExplosionPieceData[Count].Velocity.z;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 8] = ExplosionPieceData[Count].Velocity.x;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 9] = ExplosionPieceData[Count].Velocity.y;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 10] = ExplosionPieceData[Count].Velocity.z;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 8] = ExplosionPieceData[Count].Velocity.x;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 9] = ExplosionPieceData[Count].Velocity.y;
					tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 10] = ExplosionPieceData[Count].Velocity.z;
					// acc
					if (dist/Diag < 0.01f) {
						tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 11] = Acc + 4.0f * vw_fRand0();
						tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 11] = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 11];
						tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 11] = tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 11];
					} else {
						tmpChunk.VertexArray.get()[tmpChunk.VertexStride * i + 11] = Acc;
						tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 1) + 11] = Acc;
						tmpChunk.VertexArray.get()[tmpChunk.VertexStride * (i + 2) + 11] = Acc;
					}

				}


				if (dist/Diag < 0.01f)
					ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity^(Acc+4.0f*vw_fRand0());
				else
					ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity^Acc;


				ExplosionPieceData[Count].Life = (Lifetime - 1.0f) + vw_fRand();
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
			// IBO у нас быть не должно, используем VertexArrayWithSmallTriangles

			if (tmpChunk.VBO)
				vw_DeleteBufferObject(tmpChunk.VBO);
			if (tmpChunk.VAO)
				vw_DeleteVAO(tmpChunk.VAO);

			// делаем VBO
			if (!vw_BuildBufferObject(eBufferObject::Vertex,
						  tmpChunk.VertexQuantity * tmpChunk.VertexStride * sizeof(float),
						  tmpChunk.VertexArray.get(), tmpChunk.VBO))
				tmpChunk.VBO = 0;

			// делаем VAO
			if (!vw_BuildVAO(tmpChunk.VAO, tmpChunk.VertexFormat,
					 tmpChunk.VertexStride * sizeof(float),
					 tmpChunk.VBO, tmpChunk.IBO))
				tmpChunk.VAO = 0;


			// установки по шейдеру для объекта
			if (GameConfig().UseGLSL120) {
				tmpChunk.ShaderType = 2;
				// дельта скорости
				tmpChunk.ShaderData[0] = 1.0f;
				// общий коэф расстояния
				tmpChunk.ShaderData[1] = 0.0f;
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
			PlayGameSFX(eGameSFX::Explosion_Small, fVol, ExplLocation, 2);
			break;

		// взрыв
		case 2:
			// пришельцы
			PlayGameSFX(eGameSFX::Explosion_Big, fVol, ExplLocation, 2);
			break;
		case 3:
			// земляне, пираты
			PlayGameSFX(eGameSFX::Explosion_Big_Energy, fVol, ExplLocation, 2);
			break;
		// case 31:
		case 32:
			// внутренняя часть (пираты, земляне)
			//fVol = fVol/2; // ум. т.к. их там очень много
			PlayGameSFX(eGameSFX::Explosion_Big_Energy, fVol, ExplLocation, 2);
			break;
		//case 33:
		case 34:
			// внутренняя часть (босс пришельцев)
			PlayGameSFX(eGameSFX::Explosion_Big, fVol, ExplLocation, 2);
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

} // astromenace namespace
} // viewizard namespace
