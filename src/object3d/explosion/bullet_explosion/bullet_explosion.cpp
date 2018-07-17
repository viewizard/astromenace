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

#include "bullet_explosion.h"
#include "../../../config/config.h"
#include "../../../assets/audio.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

void GameCameraSetExplosion(sVECTOR3D Location, float Power);
void DestroyRadiusCollisionAllObject3D(const cObject3D &DontTouchObject, const sVECTOR3D &Point,
				       float Radius, float Damage, eObjectStatus ObjectStatus);


void PlayBulletExplosion(sVECTOR3D Location, bool NeedExplosionSFX, int ExplType)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// звуковые спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedExplosionSFX) {
		float fVol = 1.0f;

		switch (ExplType) {
		// самоликвидация рокет-торпед... и вообще всех снарядов...
		// идет со знаком "-" к номеру снаряда при создании

		// ракеты землян
		case -16:
		case -17:
		case -18:
		case -19:
		// ракеты пиратов
		case -205:
		case -206:
		case -209:
		case -210:
		// мины пиратов
		case -214:
		case -215:
		case -216:
		case -217:
			PlayGameSFX(eGameSFX::Explosion_Medium, fVol, Location);
			break;


		// взрыв и разлет при попадании снарядов в объект
		// используем аналогичный номер, как при создании


		// Kinetic
		case 1:
		case 2:
		case 3:
		case 4:
		// пираты (турели + стрельба как Kinetic1)
		case 201:
		case 202:
		case 204:
		case 211:
		case 212:
			PlayGameSFX(eGameSFX::Hit_Kinetic, fVol, Location);
			break;


		// Ion
		case 5:
		case 6:
		case 7:
		// пришельцы (как Kinetic1)
		case 101:
		// пираты (как Ion2)
		case 207:
			PlayGameSFX(eGameSFX::Hit_Ion, fVol, Location);
			break;

		// Plasma
		case 8:
		case 9:
		case 10:
		// пришельцы (как Kinetic2 и Kinetic3)
		case 102:
		case 103:
		case 104:
		case 105:
		// пришельцы (как Plasma3 и Plasma2)
		case 108:
		case 109:
		// пираты (как Plasma2)
		case 213:
			PlayGameSFX(eGameSFX::Hit_Plasma, fVol, Location);
			break;


		// Maser - у него луч, тут не учитываем
		/*	case 11:
				break;
			case 12:
				break;*/


		// Antimatter
		case 13:
		// пришельцы (мина 1 тип)
		case 106:
		// пришельцы (мина 2 тип)
		case 107:
		// пираты (как Antimatter)
		case 208:
			PlayGameSFX(eGameSFX::Hit_Antimatter, fVol, Location);
			break;

		// Laser - у него луч, тут не учитываем
		/*	case 14:
				break;*/

		// Gauss
		case 15:
			PlayGameSFX(eGameSFX::Hit_Gauss, fVol, Location);
			break;

		// ракета
		case 16:
		// ракеты пиратов
		case 205:
		// мины пиратов
		case 214:
		case 215:
		case 216:
		case 217:
			PlayGameSFX(eGameSFX::Explosion_Medium, fVol, Location);
			break;

		// рой
		case 17:
		// ракеты пиратов
		case 206:
			PlayGameSFX(eGameSFX::Explosion_Medium, fVol, Location);
			break;

		// торпеда
		case 18:
		// торпеда пиратов
		case 209:
			PlayGameSFX(eGameSFX::Explosion_Big, fVol, Location, 2);
			break;

		// бомба
		case 19:
		// бомба пиратов
		case 210:
			PlayGameSFX(eGameSFX::Explosion_Big_Energy, fVol, Location, 2);
			break;



		default:
			// если тут, значит взрыв делать не нужно... все и так само уничтожется

			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Создание взрыва из частей объекта
//-----------------------------------------------------------------------------
cBulletExplosion::cBulletExplosion(const cObject3D *Object, cProjectile *Projectile, int ExplType,
				   const sVECTOR3D &ExplLocation, float Speed, bool NeedExplosionSFX)
{
	TimeLastUpdate = Projectile->TimeLastUpdate;
	ExplosionTypeByClass = 2;

	// 1-взрыв на части, 2-разлет геометрии
	int InternalExplosionType = 0;

	// AABB нужен, т.к. по нему рисуем, если во фруструме
	// копируем AABB, т.к. они сейчас подходят...
	AABB[0] = Projectile->AABB[0];
	AABB[1] = Projectile->AABB[1];
	AABB[2] = Projectile->AABB[2];
	AABB[3] = Projectile->AABB[3];
	AABB[4] = Projectile->AABB[4];
	AABB[5] = Projectile->AABB[5];
	AABB[6] = Projectile->AABB[6];
	AABB[7] = Projectile->AABB[7];

	// сохраняем данные объекта, чтобы не было скачков и не учитывать смещение в меше
	SetLocation(Projectile->Location);
	Orientation = Projectile->Orientation;

	// сохраняем статус объекта, чтобы правильно создавать части-снаряды и обломки
	ObjectStatus = Projectile->ObjectStatus;

	ExplosionType = ExplType;

	// поправка в зависимости от скорости объекта до взрыва
	VelocityOrientation = Projectile->Orientation^(-1);
	OldSpeed = Speed;



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// визуальные эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch (ExplType) {

	// самоликвидация рокет-торпед... и вообще всех снарядов...
	// идет со знаком "-" к номеру снаряда при создании

	// !!! на минусах не учитываем Object, указатель = 0

	// ракеты землян
	case -16:
	case -17:
	case -18:
	case -19:
	// ракеты пиратов
	case -205:
	case -206:
	case -209:
	case -210:
	// мины пиратов
	case -214:
	case -215:
	case -216:
	case -217: {
		// поправка в зависимости от скорости объекта до взрыва
		VelocityOrientation = Projectile->Orientation;
		OldSpeed = Projectile->Speed - 0.5f*Projectile->Radius;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

		// эффект
		GraphicFX.resize(1);

		// установка эффекта
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 1);
			sharedGFX->Speed = 0.5f * Projectile->Radius;
			sharedGFX->SpeedVar = vw_fRand0();
			sharedGFX->SetStartLocation(Projectile->Location);
			if (Projectile->Speed != 0.0f)
				sharedGFX->Theta = 360.00f;
			sharedGFX->ParticlesPerSec = (int)(30 * Projectile->Radius);
			sharedGFX->CreationType = eParticleCreationType::Sphere;
			sharedGFX->CreationSize = sVECTOR3D{AABB[0].x, AABB[0].y, AABB[0].z};
			// разварачиваем взрыв по объекту
			sharedGFX->RotateSystemAndParticlesByAngle(Projectile->Rotation);
		}

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (int)(3*Projectile->Radius) + (int)(vw_fRand0()*3*Projectile->Radius);
		for (int i=0; i<ttt; i++) {
			cProjectile *ProjectileTMP  = nullptr;
			ProjectileTMP  = new cProjectile{1};
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{360.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     360.0f * vw_fRand0()});
			sVECTOR3D TM1 = Projectile->Orientation^Projectile->Speed;
			ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*6.0f));
			ProjectileTMP->Orientation.Normalize();

			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = 1.5f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			// учитываем пенальти
			ProjectileTMP->Speed = Projectile->Speed + Projectile->Radius*1.5f + 2.0f*vw_fRand0();
			ProjectileTMP->SpeedEnd = 0.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed;
			ProjectileTMP->Lifetime = Projectile->Age = 2.0f+vw_fRand0();
		}

		// собираем геометрию и рисуем ее разлет
		InternalExplosionType = 2;
	}
	break;


	// взрыв и разлет при попадании снарядов в объект
	// используем аналогичный номер, как при создании

	// пришельцев
	case 101:
	case 102:
	case 103:
	case 104:
	case 105:

	// фларес
	case 203:
	// пираты как Kinetic1
	case 204:
	// пираты как Kinetic2
	case 211:
	// пираты как Kinetic3
	case 212:

	// Kinetic
	case 1:
	case 2:
	case 3:
	case 4:
	// Antimatter
	case 13:
	// пришельцы (мина 1 тип)
	case 106:
	// пришельцы (мина 2 тип)
	case 107:
	// пираты (как Antimatter)
	case 208:
	// Gauss
	case 15:

		// поправка в зависимости от скорости объекта до взрыва
		if (Object != nullptr)
			VelocityOrientation = Object->Orientation;
		OldSpeed = Speed;

		Lifetime = 0.5f; // должно соотв. максимальной жизни частицы

		// просто делаем вспышку нужного цвета
		GraphicFX.resize(1);

		// установка эффекта вспышки в месте попадания
		if (!Projectile->GraphicFX.empty() && !Projectile->GraphicFX[0].expired()) {
			GraphicFX[0] = vw_CreateParticleSystem();
			auto sharedGFX = GraphicFX[0].lock();
			auto sharedProjectileGFX = Projectile->GraphicFX[0].lock();
			if (sharedGFX && sharedProjectileGFX) {
				SetExplosionGFX(sharedGFX, 0);
				float Rexpl = (sharedProjectileGFX->ColorStart.r + sharedProjectileGFX->ColorEnd.r) / 2.0f;
				float Gexpl = (sharedProjectileGFX->ColorStart.g + sharedProjectileGFX->ColorEnd.g) / 2.0f;
				float Bexpl = (sharedProjectileGFX->ColorStart.b + sharedProjectileGFX->ColorEnd.b) / 2.0f;
				sharedGFX->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, Rexpl, Gexpl, Bexpl, 0.0f, 0.005f);

				sharedGFX->ColorStart.r = sharedProjectileGFX->ColorStart.r;
				sharedGFX->ColorStart.g = sharedProjectileGFX->ColorStart.g;
				sharedGFX->ColorStart.b = sharedProjectileGFX->ColorStart.b;
				sharedGFX->ColorEnd.r = sharedProjectileGFX->ColorEnd.r;
				sharedGFX->ColorEnd.g = sharedProjectileGFX->ColorEnd.g;
				sharedGFX->ColorEnd.b = sharedProjectileGFX->ColorEnd.b;
				sharedGFX->Speed = 150.0f;
				sharedGFX->ParticlesPerSec = sharedProjectileGFX->ParticlesPerSec;
				sharedGFX->SizeStart = sharedProjectileGFX->SizeStart;
				sharedGFX->SizeEnd = sharedProjectileGFX->SizeEnd;
				sharedGFX->SizeVar = sharedProjectileGFX->SizeVar;
				sharedGFX->Life = Lifetime;
				sharedGFX->NeedStop = false;
				sharedGFX->SetStartLocation(ExplLocation);
			}
		}

		Projectile->NeedStopPartic = true;
		NeedStop = false;

		break;


	// Ion
	case 5:
	case 6:
	case 7:
	// пираты (как Ion2)
	case 207:
	// Plasma
	case 8:
	case 9:
	case 10:
	// пришельцы (как Plasma3 и Plasma2)
	case 108:
	case 109:
	// пираты (как Plasma2)
	case 213:

		// поправка в зависимости от скорости объекта до взрыва
		if (Object != nullptr)
			VelocityOrientation = Object->Orientation;
		OldSpeed = Speed;

		Lifetime = 0.5f; // должно соотв. максимальной жизни частицы

		// просто делаем вспышку нужного цвета
		GraphicFX.resize(1);

		// установка эффекта вспышки в месте попадания
		if (!Projectile->GraphicFX.empty() && !Projectile->GraphicFX[0].expired()) {
			GraphicFX[0] = vw_CreateParticleSystem();
			auto sharedGFX = GraphicFX[0].lock();
			auto sharedProjectileGFX = Projectile->GraphicFX[0].lock();
			if (sharedGFX && sharedProjectileGFX) {
				SetExplosionGFX(sharedGFX, 0);
				float Rexpl = (sharedProjectileGFX->ColorStart.r + sharedProjectileGFX->ColorEnd.r) / 2.0f;
				float Gexpl = (sharedProjectileGFX->ColorStart.g + sharedProjectileGFX->ColorEnd.g) / 2.0f;
				float Bexpl = (sharedProjectileGFX->ColorStart.b + sharedProjectileGFX->ColorEnd.b) / 2.0f;
				sharedGFX->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, Rexpl, Gexpl, Bexpl, 0.0f, 0.005f);

				sharedGFX->ColorStart.r = sharedProjectileGFX->ColorStart.r;
				sharedGFX->ColorStart.g = sharedProjectileGFX->ColorStart.g;
				sharedGFX->ColorStart.b = sharedProjectileGFX->ColorStart.b;
				sharedGFX->ColorEnd.r = sharedProjectileGFX->ColorEnd.r;
				sharedGFX->ColorEnd.g = sharedProjectileGFX->ColorEnd.g;
				sharedGFX->ColorEnd.b = sharedProjectileGFX->ColorEnd.b;
				sharedGFX->Speed = 150.0f;
				sharedGFX->ParticlesPerSec = sharedProjectileGFX->ParticlesPerSec;
				sharedGFX->SizeStart = sharedProjectileGFX->SizeStart;
				sharedGFX->SizeEnd = sharedProjectileGFX->SizeEnd;
				sharedGFX->SizeVar = sharedProjectileGFX->SizeVar;
				sharedGFX->Life = Lifetime;
				sharedGFX->NeedStop = false;
				sharedGFX->SetStartLocation(ExplLocation);
			}
		}

		Projectile->NeedStopPartic = true;
		NeedStop = false;

		break;


	// Maser
	/*	case 11:
			break;
		case 12:
			break;*/
	// Laser
	/*	case 14:
			break;*/


	// ракета
	case 16:
	// мины пиратов
	case 214:
	case 215:
	case 216:
	case 217: {
		// поправка в зависимости от скорости объекта до взрыва
		VelocityOrientation = Projectile->Orientation^(-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

		// эффект
		GraphicFX.resize(1);

		// установка эффекта
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 10);
			sharedGFX->SetStartLocation(Projectile->Location);
		}

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (3 + vw_fRand()) * Projectile->Radius;
		for (int i = 0; i < ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{1};
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			sVECTOR3D TM1 = Projectile->Orientation;
			ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*2.0f));
			ProjectileTMP->Orientation.Normalize();

			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/7.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/4.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 2.0f+vw_fRand0();
		}

		// собираем геометрию и рисуем ее разлет
		InternalExplosionType = 2;
	}
	break;


	// рой
	case 17: {
		// поправка в зависимости от скорости объекта до взрыва
		VelocityOrientation = Projectile->Orientation^(-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

		// эффект
		GraphicFX.resize(1);

		// установка эффекта
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 10);
			sharedGFX->ParticlesPerSec = 30;
			sharedGFX->CreationSize = sVECTOR3D{2.0f, 0.3f, 2.0f};
			sharedGFX->SetStartLocation(Projectile->Location);
		}

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (1 + vw_fRand()) * Projectile->Radius;
		//vw_LogMessage(LOG_MESS_INF, "%i", ttt);
		for (int i = 0; i < ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{1};
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			sVECTOR3D TM1 = Projectile->Orientation;//^Speed;
			ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*2.0f));
			ProjectileTMP->Orientation.Normalize();

			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/7.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/4.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 2.0f+vw_fRand0();
		}

		// собираем геометрию и рисуем ее разлет
		InternalExplosionType = 2;
	}
	break;

	// торпеда
	case 18:
	// торпеда пиратов
	case 209: {
		// убиваем всех, кто рядом
		// FIXME this act as hidden trap (objects release), should be moved to collision detection code
		DestroyRadiusCollisionAllObject3D(*Object, Projectile->Location, 75.0f, Projectile->DamageHull, Projectile->ObjectStatus);

		VelocityOrientation = Projectile->Orientation^(-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

		// эффект
		GraphicFX.resize(1);

		// установка эффекта
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 8);
			sharedGFX->SetStartLocation(Projectile->Location);
		}

		// создаем немного разлетающихся кусков-снарядов
		int ttt = (2 + vw_fRand()) * Projectile->Radius;
		for (int i=0; i<ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{2};
			ProjectileTMP->Num = 1;
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			sVECTOR3D TM1 = Projectile->Orientation;//^Speed;
			ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*2.0f));
			ProjectileTMP->Orientation.Normalize();

			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/7.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/4.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 2.0f+vw_fRand0();
		}

		// собираем геометрию и рисуем ее разлет
		InternalExplosionType = 2;
	}
	break;


	// бомба
	case 19:
	// бомба пиратов
	case 210: {
		// убиваем всех, кто рядом
		// FIXME this act as hidden trap (objects release), should be moved to collision detection code
		DestroyRadiusCollisionAllObject3D(*Object, Projectile->Location, 150.0f, Projectile->DamageHull, Projectile->ObjectStatus);

		VelocityOrientation = Projectile->Orientation^(-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

		// эффект
		GraphicFX.resize(2);

		// установка эффекта
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 7);
			sharedGFX->SetStartLocation(Projectile->Location);
		}

		GraphicFX[1] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[1].lock()) {
			SetExplosionGFX(sharedGFX, 9);
			sharedGFX->SetStartLocation(Projectile->Location);
		}

		// создаем немного разлетающихся кусков-снарядов
		for (int i=0; i<4; i++) {
			cProjectile *ProjectileTMP = new cProjectile{3};
			ProjectileTMP->Num = 1;
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			sVECTOR3D TM1 = Projectile->Orientation;
			ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*4.0f));
			ProjectileTMP->Orientation.Normalize();

			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_fRand0();
		}
		// создаем немного разлетающихся кусков-снарядов
		int ttt = (3  + vw_fRand()) * Projectile->Radius;
		for (int i=0; i<ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{2};
			ProjectileTMP->Num = 1;
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_fRand0();
		}
		// создаем немного разлетающихся кусков-снарядов
		ttt = (3 + vw_fRand()) * Projectile->Radius;
		for (int i=0; i<ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{3};
			ProjectileTMP->Num = 1;
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_fRand0();
		}

		ttt = (3 + vw_fRand() * 5) * Projectile->Radius;
		for (int i=0; i<ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{1};
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{5.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_fRand0();
		}

		// создаем немного разлетающихся кусков-снарядов
		ttt = (5 + vw_fRand() * 3) * Projectile->Radius;
		for (int i=0; i<ttt; i++) {
			cProjectile *ProjectileTMP = new cProjectile{5};
			ProjectileTMP->Num = 1;
			ProjectileTMP->SetLocation(Location);

			ProjectileTMP->SetRotation(sVECTOR3D{20.0f * vw_fRand0(),
							     360.0f * vw_fRand0(),
							     0.0f});
			for (auto &tmpGFX : ProjectileTMP->GraphicFX) {
				if (auto sharedGFX = tmpGFX.lock()) {
					sharedGFX->Direction = ProjectileTMP->Orientation ^ -1;
					sharedGFX->Speed = sharedGFX->Speed / 2.0f;
					sharedGFX->SetStartLocation(Location);
				}
			}
			ProjectileTMP->ObjectStatus = ObjectStatus;
			ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
			ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
			ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_fRand0();
		}

		// собираем геометрию и рисуем ее разлет
		InternalExplosionType = 2;
	}
	break;

	// ракета пиратов
	case 205:
	case 206: {
		// поправка в зависимости от скорости объекта до взрыва
		VelocityOrientation = Projectile->Orientation^(-1);
		OldSpeed = Speed = 0.0f;
		Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

		// эффект
		GraphicFX.resize(1);

		// установка эффекта
		GraphicFX[0] = vw_CreateParticleSystem();
		if (auto sharedGFX = GraphicFX[0].lock()) {
			SetExplosionGFX(sharedGFX, 10);
			sharedGFX->SetStartLocation(Projectile->Location);
		}

		// собираем геометрию и рисуем ее разлет
		InternalExplosionType = 2;
	}
	break;



	default:
		// если тут, значит взрыв делать не нужно... все и так само уничтожется

		break;
	}
















	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// взрыв с разлетом геометрии
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (InternalExplosionType == 2) {
		// создаем новые данные, переносим туда
		// объекты малы, по этому не применяем сдесь настройки качества взрыва, делаем со всей геометрией
		// копируем данные снаряда
		Texture = Projectile->Texture;
		Chunks = Projectile->Chunks;

		for (unsigned int i = 0; i < Chunks.size(); i++) {
			// делаем изменения
			Chunks[i].VBO = 0;
			Chunks[i].IBO = 0;
			Chunks[i].VAO = 0;
			Chunks[i].NeedReleaseOpenGLBuffers = true; // удалять в объекте
			Chunks[i].RangeStart = 0;

			// если у нас включены и работают шейдеры, надо приготовить место для данных + изменить формат и шаг
			if (GameConfig().UseGLSL120) {
				Chunks[i].VertexStride = 3 + 3 + 6;
				Chunks[i].VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX;
			}

			// выделяем память для данных
			Chunks[i].VertexArray.reset(new float[Chunks[i].VertexStride * Chunks[i].VertexQuantity],
							  std::default_delete<float[]>());

			// делаем поворот геометрии объекта чтобы правильно сделать разлет частиц
			sVECTOR3D TMP;
			for (unsigned int j = 0; j < Chunks[i].VertexQuantity; j++) {
				int j1 = j * Chunks[i].VertexStride;
				int j2;
				if (Projectile->Chunks[i].IndexArray)
					j2 = Projectile->Chunks[i].IndexArray.get()[Projectile->Chunks[i].RangeStart + j] *
					     Projectile->Chunks[i].VertexStride;
				else
					j2 = (Projectile->Chunks[i].RangeStart + j) *
					     Projectile->Chunks[i].VertexStride;


				TMP.x = Projectile->Chunks[i].VertexArray.get()[j2] + Chunks[i].Location.x;
				TMP.y = Projectile->Chunks[i].VertexArray.get()[j2 + 1] + Chunks[i].Location.y;
				TMP.z = Projectile->Chunks[i].VertexArray.get()[j2 + 2] + Chunks[i].Location.z;
				vw_Matrix33CalcPoint(TMP, Projectile->CurrentRotationMat);
				// координаты
				Chunks[i].VertexArray.get()[j1] = TMP.x;
				Chunks[i].VertexArray.get()[j1 + 1] = TMP.y;
				Chunks[i].VertexArray.get()[j1 + 2] = TMP.z;
				// нормали
				TMP.x = Projectile->Chunks[i].VertexArray.get()[j2 + 3];
				TMP.y = Projectile->Chunks[i].VertexArray.get()[j2 + 4];
				TMP.z = Projectile->Chunks[i].VertexArray.get()[j2 + 5];
				vw_Matrix33CalcPoint(TMP, Projectile->CurrentRotationMat);
				Chunks[i].VertexArray.get()[j1 + 3] = TMP.x;
				Chunks[i].VertexArray.get()[j1 + 4] = TMP.y;
				Chunks[i].VertexArray.get()[j1 + 5] = TMP.z;
				// текстура
				Chunks[i].VertexArray.get()[j1 + 6] = Projectile->Chunks[i].VertexArray.get()[j2 + 6];
				Chunks[i].VertexArray.get()[j1 + 7] = Projectile->Chunks[i].VertexArray.get()[j2 + 7];
			}


			Chunks[i].Location = sVECTOR3D{0.0f, 0.0f, 0.0f};
			// сбрасываем индексный буфер блока (если он был), мы "распаковали" все в VertexArray
			Chunks[i].IndexArray.reset();
		}

		float tRadius2 = Projectile->Radius/1.5f;

		// для каждого треугольника - свои данные (фактически, у нас 1 объект, с ним и работаем)
		int Count = 0;
		ExplosionPieceData = new sExplosionPiece[Chunks[0].VertexQuantity / 3];
		for (unsigned int i = 0; i < Chunks[0].VertexQuantity; i += 3) {
			ExplosionPieceData[Count].Velocity.x = Chunks[0].VertexArray.get()[i * Chunks[0].VertexStride];
			ExplosionPieceData[Count].Velocity.y = Chunks[0].VertexArray.get()[i * Chunks[0].VertexStride + 1];
			ExplosionPieceData[Count].Velocity.z = Chunks[0].VertexArray.get()[i * Chunks[0].VertexStride + 2];

			float VelocityTMP = vw_fRand0()*tRadius2;

			// записываем центр треугольника, оно же базовое ускорение + цент UV, нужно для шейдера
			if (GameConfig().UseGLSL120) {
				// Velocity/центр треугольника
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * i + 8] = ExplosionPieceData[Count].Velocity.x;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * i + 9] = ExplosionPieceData[Count].Velocity.y;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * i + 10] = ExplosionPieceData[Count].Velocity.z;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 1) + 8] = ExplosionPieceData[Count].Velocity.x;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 1) + 9] = ExplosionPieceData[Count].Velocity.y;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 1) + 10] = ExplosionPieceData[Count].Velocity.z;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 2) + 8] = ExplosionPieceData[Count].Velocity.x;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 2) + 9] = ExplosionPieceData[Count].Velocity.y;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 2) + 10] = ExplosionPieceData[Count].Velocity.z;
				// acc
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * i + 11] = VelocityTMP;
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 1) + 11] = Chunks[0].VertexArray.get()[Chunks[0].VertexStride * i + 11];
				Chunks[0].VertexArray.get()[Chunks[0].VertexStride * (i + 2) + 11] = Chunks[0].VertexArray.get()[Chunks[0].VertexStride * i + 11];
			}


			// в данном случае, это и есть направление, потому что геометрия в точке 0,0,0
			ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity^VelocityTMP;

			ExplosionPieceData[Count].Life = 1.0f + vw_fRand() / 2.0f;

			// делаем анализ для ААBB, смотрим отлет частицы
			float tmpSpeed = ExplosionPieceData[Count].Velocity.x * ExplosionPieceData[Count].Velocity.x +
					 ExplosionPieceData[Count].Velocity.y * ExplosionPieceData[Count].Velocity.y +
					 ExplosionPieceData[Count].Velocity.z * ExplosionPieceData[Count].Velocity.z;
			if (tmpSpeed > AABBSpeed) AABBSpeed = tmpSpeed;

			Count++;
		}

		// тк был квадрат, теперь вытягиваем нормальное значение
		AABBSpeed = sqrtf(AABBSpeed);

		// установка шейдера
		if (GameConfig().UseGLSL120) {
			Chunks[0].ShaderType = 2;
			// дельта скорости
			Chunks[0].ShaderData[0] = 1.0f;
			// общий коэф расстояния
			Chunks[0].ShaderData[1] = 0.0f;
		}



		// удаляем старые буферы, если они есть, создаем новые
		// IBO у нас быть не должно, мы "распаковали" уже все в VertexArray, IndexArray сбошен

		if (Chunks[0].VBO)
			vw_DeleteBufferObject(Chunks[0].VBO);
		if (Chunks[0].VAO)
			vw_DeleteVAO(Chunks[0].VAO);

		// делаем VBO
		if (!vw_BuildBufferObject(eBufferObject::Vertex,
					  Chunks[0].VertexQuantity * Chunks[0].VertexStride * sizeof(float),
					  Chunks[0].VertexArray.get(), Chunks[0].VBO))
			Chunks[0].VBO = 0;

		// делаем VAO
		if (!vw_BuildVAO(Chunks[0].VAO, Chunks[0].VertexFormat,
				 Chunks[0].VertexStride * sizeof(float),
				 Chunks[0].VBO, Chunks[0].IBO))
			Chunks[0].VAO = 0;
	}













	// дальше, если не видем точку взрыва не делать... проверяем...
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0])) {
		return;
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// звуковые спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	PlayBulletExplosion(Location, NeedExplosionSFX, ExplType);



	// делаем сотрясание камеры, если нужно
	switch (ExplType) {
	// торпеда
	case 18:
	// торпеда пиратов
	case 209:
		GameCameraSetExplosion(ExplLocation, 0.5f);
		break;

	// бомба
	case 19:
	// бомба пиратов
	case 210:
		GameCameraSetExplosion(ExplLocation, 1.5f);
		break;

	}

}

} // astromenace namespace
} // viewizard namespace
