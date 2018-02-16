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
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/


/// подключаем нужные файлы
#include "BulletExplosion.h"
#include "../../../Game.h"
void DestroyRadiusCollisionAllObject3D(CObject3D *DontTouchObject, VECTOR3D Point, float Radius, float Damage, int ObjectStatus);






void PlayBulletExplosion(VECTOR3D Location, bool NeedExplosionSFX, int ExplType)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// звуковые спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedExplosionSFX)
	{
		float fVol = 1.0f;

		switch (ExplType)
		{
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
				// просто малый взрыв
				Audio_PlaySound3D(6, fVol, Location, false);
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
				// разваливание снарядов
				Audio_PlaySound3D(28, fVol, Location, false);
				break;


			// Ion
			case 5:
			case 6:
			case 7:
			// пришельцы (как Kinetic1)
			case 101:
			// пираты (как Ion2)
			case 207:
				// разваливание снарядов
				Audio_PlaySound3D(29, fVol, Location, false);
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
				// разваливание снарядов
				Audio_PlaySound3D(30, fVol, Location, false);
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
				// разваливание снарядов
				Audio_PlaySound3D(31, fVol, Location, false);
				break;

			// Laser - у него луч, тут не учитываем
		/*	case 14:
				break;*/

			// Gauss
			case 15:
				// разваливание снарядов
				Audio_PlaySound3D(32, fVol, Location, false);
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
				// малый взрыв
				Audio_PlaySound3D(6, fVol, Location, false);
				break;

			// рой
			case 17:
			// ракеты пиратов
			case 206:
				// малый взрыв
				Audio_PlaySound3D(6, fVol, Location, false);
				break;

			// торпеда
			case 18:
			// торпеда пиратов
			case 209:
				// средний взрыв
				Audio_PlaySound3D(7, fVol, Location, false, 2);
				break;

			// бомба
			case 19:
			// бомба пиратов
			case 210:
				// большой взрыв
				Audio_PlaySound3D(8, fVol, Location, false, 2);
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
void CBulletExplosion::Create(CObject3D *Object, CProjectile *Projectile, int ExplType, VECTOR3D ExplLocation, float Speed, bool NeedExplosionSFX)
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
	switch (ExplType)
	{

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
		case -217:
			{
				// поправка в зависимости от скорости объекта до взрыва
				VelocityOrientation = Projectile->Orientation;
				OldSpeed = Projectile->Speed - 0.5f*Projectile->Radius;
				Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

				// эффект
				GraphicFXQuantity = 1;
				GraphicFX = new eParticleSystem*[GraphicFXQuantity];
				for (int i=0; i<GraphicFXQuantity; i++)
				{
					GraphicFX[i] = 0;
				}

				// установка эффекта
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 1);

				GraphicFX[0]->Speed = 0.5f*Projectile->Radius;
				GraphicFX[0]->SpeedVar   = vw_Randf0;
				GraphicFX[0]->SetStartLocation(Projectile->Location);
				if (Projectile->Speed!=0) GraphicFX[0]->Theta = 360.00f;
				GraphicFX[0]->ParticlesPerSec = (int)(30*Projectile->Radius);
				GraphicFX[0]->CreationType = 1;
				GraphicFX[0]->CreationSize = VECTOR3D(AABB[0].x, AABB[0].y, AABB[0].z);
				// разварачиваем взрыв по объекту
				GraphicFX[0]->RotateSystemAndParticlesByAngle(Projectile->Rotation);

				// создаем немного разлетающихся кусков-снарядов
				int ttt = (int)(3*Projectile->Radius) + (int)(vw_Randf0*3*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(1);
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(360.0f*vw_Randf0, 360.0f*vw_Randf0, 360.0f*vw_Randf0));
					VECTOR3D TM1 = Projectile->Orientation^Projectile->Speed;
					ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*6.0f));
					ProjectileTMP->Orientation.Normalize();

					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = 1.5f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					// учитываем пенальти
					ProjectileTMP->Speed = Projectile->Speed + Projectile->Radius*1.5f + 2.0f*vw_Randf0;
					ProjectileTMP->SpeedEnd = 0.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed;
					ProjectileTMP->Lifetime = Projectile->Age = 2.0f+vw_Randf0;
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
			if (Object != 0) VelocityOrientation = Object->Orientation;
			OldSpeed = Speed;

			Lifetime = 0.5f; // должно соотв. максимальной жизни частицы

			// просто делаем вспышку нужного цвета
			GraphicFXQuantity = 1;
			GraphicFX = new eParticleSystem*[GraphicFXQuantity];
			for (int i=0; i<GraphicFXQuantity; i++)
			{
				GraphicFX[i] = 0;
			}

			// установка эффекта вспышки в месте попадания
			if (Projectile->GraphicFX != 0)
			if (Projectile->GraphicFX[0]!=0)
			{
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 0);
				float Rexpl = (Projectile->GraphicFX[0]->ColorStart.r + Projectile->GraphicFX[0]->ColorEnd.r)/2.0f;
				float Gexpl = (Projectile->GraphicFX[0]->ColorStart.g + Projectile->GraphicFX[0]->ColorEnd.g)/2.0f;
				float Bexpl = (Projectile->GraphicFX[0]->ColorStart.b + Projectile->GraphicFX[0]->ColorEnd.b)/2.0f;
				GraphicFX[0]->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), Rexpl, Gexpl, Bexpl, 0.0f, 0.005f);

				GraphicFX[0]->ColorStart.r = Projectile->GraphicFX[0]->ColorStart.r;
				GraphicFX[0]->ColorStart.g = Projectile->GraphicFX[0]->ColorStart.g;
				GraphicFX[0]->ColorStart.b = Projectile->GraphicFX[0]->ColorStart.b;
				GraphicFX[0]->ColorEnd.r = Projectile->GraphicFX[0]->ColorEnd.r;
				GraphicFX[0]->ColorEnd.g = Projectile->GraphicFX[0]->ColorEnd.g;
				GraphicFX[0]->ColorEnd.b = Projectile->GraphicFX[0]->ColorEnd.b;
				GraphicFX[0]->Speed = 150.0f;
				GraphicFX[0]->ParticlesPerSec = Projectile->GraphicFX[0]->ParticlesPerSec;
				GraphicFX[0]->SizeStart = Projectile->GraphicFX[0]->SizeStart;
				GraphicFX[0]->SizeEnd = Projectile->GraphicFX[0]->SizeEnd;
				GraphicFX[0]->SizeVar = Projectile->GraphicFX[0]->SizeVar;
				GraphicFX[0]->Life = Lifetime;
				GraphicFX[0]->NeedStop = false;
				GraphicFX[0]->SetStartLocation(ExplLocation);
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
			if (Object != 0) VelocityOrientation = Object->Orientation;
			OldSpeed = Speed;

			Lifetime = 0.5f; // должно соотв. максимальной жизни частицы

			// просто делаем вспышку нужного цвета
			GraphicFXQuantity = 1;
			GraphicFX = new eParticleSystem*[GraphicFXQuantity];
			for (int i=0; i<GraphicFXQuantity; i++)
			{
				GraphicFX[i] = 0;
			}

			// установка эффекта вспышки в месте попадания
			if (Projectile->GraphicFX != 0)
			if (Projectile->GraphicFX[0]!=0)
			{
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 0);
				float Rexpl = (Projectile->GraphicFX[0]->ColorStart.r + Projectile->GraphicFX[0]->ColorEnd.r)/2.0f;
				float Gexpl = (Projectile->GraphicFX[0]->ColorStart.g + Projectile->GraphicFX[0]->ColorEnd.g)/2.0f;
				float Bexpl = (Projectile->GraphicFX[0]->ColorStart.b + Projectile->GraphicFX[0]->ColorEnd.b)/2.0f;
				GraphicFX[0]->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), Rexpl, Gexpl, Bexpl, 0.0f, 0.005f);

				GraphicFX[0]->ColorStart.r = Projectile->GraphicFX[0]->ColorStart.r;
				GraphicFX[0]->ColorStart.g = Projectile->GraphicFX[0]->ColorStart.g;
				GraphicFX[0]->ColorStart.b = Projectile->GraphicFX[0]->ColorStart.b;
				GraphicFX[0]->ColorEnd.r = Projectile->GraphicFX[0]->ColorEnd.r;
				GraphicFX[0]->ColorEnd.g = Projectile->GraphicFX[0]->ColorEnd.g;
				GraphicFX[0]->ColorEnd.b = Projectile->GraphicFX[0]->ColorEnd.b;
				GraphicFX[0]->Speed = 150.0f;
				GraphicFX[0]->ParticlesPerSec = Projectile->GraphicFX[0]->ParticlesPerSec;
				GraphicFX[0]->SizeStart = Projectile->GraphicFX[0]->SizeStart;
				GraphicFX[0]->SizeEnd = Projectile->GraphicFX[0]->SizeEnd;
				GraphicFX[0]->SizeVar = Projectile->GraphicFX[0]->SizeVar;
				GraphicFX[0]->Life = Lifetime;
				GraphicFX[0]->NeedStop = false;
				GraphicFX[0]->SetStartLocation(ExplLocation);
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
		case 217:
			{
				// поправка в зависимости от скорости объекта до взрыва
				VelocityOrientation = Projectile->Orientation^(-1);
				OldSpeed = Speed = 0.0f;
				Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

				// эффект
				GraphicFXQuantity = 1;
				GraphicFX = new eParticleSystem*[GraphicFXQuantity];
				for (int i=0; i<GraphicFXQuantity; i++)
				{
					GraphicFX[i] = 0;
				}

				// установка эффекта
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 10);
				GraphicFX[0]->SetStartLocation(Projectile->Location);

				// создаем немного разлетающихся кусков-снарядов
				int ttt = (int)(3*Projectile->Radius) + (int)(vw_Randf1*1*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(1);
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(20.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					VECTOR3D TM1 = Projectile->Orientation;
					ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*2.0f));
					ProjectileTMP->Orientation.Normalize();

					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/7.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/4.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 2.0f+vw_Randf0;
				}

				// собираем геометрию и рисуем ее разлет
				InternalExplosionType = 2;
			}
			break;


		// рой
		case 17:
			{
				// поправка в зависимости от скорости объекта до взрыва
				VelocityOrientation = Projectile->Orientation^(-1);
				OldSpeed = Speed = 0.0f;
				Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

				// эффект
				GraphicFXQuantity = 1;
				GraphicFX = new eParticleSystem*[GraphicFXQuantity];
				for (int i=0; i<GraphicFXQuantity; i++)
				{
					GraphicFX[i] = 0;
				}

				// установка эффекта
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 10);
				GraphicFX[0]->ParticlesPerSec = 30;
				GraphicFX[0]->CreationSize = VECTOR3D(2.0f,0.3f,2.0f);
				GraphicFX[0]->SetStartLocation(Projectile->Location);

				// создаем немного разлетающихся кусков-снарядов
				int ttt = (int)(1*Projectile->Radius) + (int)(vw_Randf1*1*Projectile->Radius);
				//vw_LogMessage(LOG_MESS_INF, "%i", ttt);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(1);
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(20.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					VECTOR3D TM1 = Projectile->Orientation;//^Speed;
					ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*2.0f));
					ProjectileTMP->Orientation.Normalize();

					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/7.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/4.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 2.0f+vw_Randf0;
				}

				// собираем геометрию и рисуем ее разлет
				InternalExplosionType = 2;
			}
			break;

		// торпеда
		case 18:
		// торпеда пиратов
		case 209:
			{
				// убиваем всех, кто рядом
				DestroyRadiusCollisionAllObject3D(Object, Projectile->Location, 75.0f, Projectile->DamageHull, Projectile->ObjectStatus);

				VelocityOrientation = Projectile->Orientation^(-1);
				OldSpeed = Speed = 0.0f;
				Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

				// эффект
				GraphicFXQuantity = 1;
				GraphicFX = new eParticleSystem*[GraphicFXQuantity];
				for (int i=0; i<GraphicFXQuantity; i++)
				{
					GraphicFX[i] = 0;
				}

				// установка эффекта
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 8);
				GraphicFX[0]->SetStartLocation(Projectile->Location);

				// создаем немного разлетающихся кусков-снарядов
				int ttt = (int)(2*Projectile->Radius) + (int)(vw_Randf1*1*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(2);
					ProjectileTMP->Num = 1;
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(20.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					VECTOR3D TM1 = Projectile->Orientation;//^Speed;
					ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*2.0f));
					ProjectileTMP->Orientation.Normalize();

					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/7.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/4.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 2.0f+vw_Randf0;
				}

				// собираем геометрию и рисуем ее разлет
				InternalExplosionType = 2;
			}
			break;


		// бомба
		case 19:
		// бомба пиратов
		case 210:
			{
				// убиваем всех, кто рядом
				DestroyRadiusCollisionAllObject3D(Object, Projectile->Location, 150.0f, Projectile->DamageHull, Projectile->ObjectStatus);

				VelocityOrientation = Projectile->Orientation^(-1);
				OldSpeed = Speed = 0.0f;
				Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

				// эффект
				GraphicFXQuantity = 2;
				GraphicFX = new eParticleSystem*[GraphicFXQuantity];
				for (int i=0; i<GraphicFXQuantity; i++)
				{
					GraphicFX[i] = 0;
				}

				// установка эффекта
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 7);
				GraphicFX[0]->SetStartLocation(Projectile->Location);

				GraphicFX[1] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[1], 9);
				GraphicFX[1]->SetStartLocation(Projectile->Location);

				// создаем немного разлетающихся кусков-снарядов
				for (int i=0; i<4; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(3);
					ProjectileTMP->Num = 1;
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(5.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					VECTOR3D TM1 = Projectile->Orientation;
					ProjectileTMP->Orientation = TM1 + (ProjectileTMP->Orientation^(Projectile->Radius*4.0f));
					ProjectileTMP->Orientation.Normalize();

					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_Randf0;
				}
				// создаем немного разлетающихся кусков-снарядов
				int ttt = (int)(3*Projectile->Radius) + (int)(vw_Randf1*1*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(2);
					ProjectileTMP->Num = 1;
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(5.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_Randf0;
				}
				// создаем немного разлетающихся кусков-снарядов
				ttt = (int)(3*Projectile->Radius) + (int)(vw_Randf1*1*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(3);
					ProjectileTMP->Num = 1;
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(5.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_Randf0;
				}

				ttt = (int)(3*Projectile->Radius) + (int)(vw_Randf1*5*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(1);
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(5.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_Randf0;
				}

				// создаем немного разлетающихся кусков-снарядов
				ttt = (int)(5*Projectile->Radius) + (int)(vw_Randf1*3*Projectile->Radius);
				for (int i=0; i<ttt; i++)
				{
					CProjectile *ProjectileTMP  = 0;
					ProjectileTMP  = new CProjectile;
					ProjectileTMP->Create(5);
					ProjectileTMP->Num = 1;
					ProjectileTMP->SetLocation(Location);

					ProjectileTMP->SetRotation(VECTOR3D(20.0f*vw_Randf0, 360.0f*vw_Randf0, 0.0f));
					for (int j=0; j<ProjectileTMP->GraphicFXQuantity; j++)
					{
						ProjectileTMP->GraphicFX[j]->Direction = ProjectileTMP->Orientation^-1;
						ProjectileTMP->GraphicFX[j]->Speed = ProjectileTMP->GraphicFX[j]->Speed/2.0f;
						ProjectileTMP->GraphicFX[j]->SetStartLocation(Location);
					}
					ProjectileTMP->ObjectStatus = ObjectStatus;
					ProjectileTMP->SpeedEnd = ProjectileTMP->Speed/6.0f;
					ProjectileTMP->SpeedStart = ProjectileTMP->Speed/2.0f;
					ProjectileTMP->Lifetime = ProjectileTMP->Age = 3.0f+vw_Randf0;
				}

				// собираем геометрию и рисуем ее разлет
				InternalExplosionType = 2;
			}
			break;

		// ракета пиратов
		case 205:
		case 206:
			{
				// поправка в зависимости от скорости объекта до взрыва
				VelocityOrientation = Projectile->Orientation^(-1);
				OldSpeed = Speed = 0.0f;
				Lifetime = 2.0f; // должно соотв. максимальной жизни частицы

				// эффект
				GraphicFXQuantity = 1;
				GraphicFX = new eParticleSystem*[GraphicFXQuantity];
				for (int i=0; i<GraphicFXQuantity; i++)
				{
					GraphicFX[i] = 0;
				}

				// установка эффекта
				GraphicFX[0] = new eParticleSystem;
				SetExplosionGFX(GraphicFX[0], 10);
				GraphicFX[0]->SetStartLocation(Projectile->Location);

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
	if (InternalExplosionType == 2)
	{
		// создаем новые данные, переносим туда
		// объекты малы, по этому не применяем сдесь настройки качества взрыва, делаем со всей геометрией
		DrawObjectQuantity = Projectile->DrawObjectQuantity;
		Texture = new eTexture*[DrawObjectQuantity];
		DrawObjectList = new eObjectBlock[DrawObjectQuantity];

		for (int i=0; i<DrawObjectQuantity; i++)
		{
			Texture[i] = Projectile->Texture[i];
			// копируем данные
			memcpy(&(DrawObjectList[i]), &(Projectile->DrawObjectList[i]), sizeof(eObjectBlock));
			// делаем изменения
			DrawObjectList[i].VBO = 0;
			DrawObjectList[i].VertexBuffer = 0;
			DrawObjectList[i].IBO = 0;
			DrawObjectList[i].IndexBuffer = 0;
			DrawObjectList[i].VAO = 0;
			DrawObjectList[i].NeedDestroyDataInObjectBlock = true; // удалять в объекте
			DrawObjectList[i].RangeStart = 0;

			// если у нас включены и работают шейдеры, надо приготовить место для данных + изменить формат и шаг
			if (Setup.UseGLSL)
			{
				DrawObjectList[i].VertexStride = 3+3+6;
				DrawObjectList[i].VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX;
			}

			// выделяем память для данных
			DrawObjectList[i].VertexBuffer = new float[DrawObjectList[i].VertexStride*DrawObjectList[i].VertexCount];

			// делаем поворот геометрии объекта чтобы правильно сделать разлет частиц
			VECTOR3D TMP;
			for (int j=0; j<DrawObjectList[i].VertexCount; j++)
			{
				int j1 = j*DrawObjectList[i].VertexStride;
				int j2;
				if (Projectile->DrawObjectList[i].IndexBuffer != 0)
					j2 = Projectile->DrawObjectList[i].IndexBuffer[Projectile->DrawObjectList[i].RangeStart+j]*Projectile->DrawObjectList[i].VertexStride;
				else
					j2 = (Projectile->DrawObjectList[i].RangeStart+j)*Projectile->DrawObjectList[i].VertexStride;


				TMP.x = Projectile->DrawObjectList[i].VertexBuffer[j2] + DrawObjectList[i].Location.x;
				TMP.y = Projectile->DrawObjectList[i].VertexBuffer[j2+1] + DrawObjectList[i].Location.y;
				TMP.z = Projectile->DrawObjectList[i].VertexBuffer[j2+2] + DrawObjectList[i].Location.z;
				Matrix33CalcPoint(&TMP, Projectile->CurrentRotationMat);
				// координаты
				DrawObjectList[i].VertexBuffer[j1] = TMP.x;
				DrawObjectList[i].VertexBuffer[j1+1] = TMP.y;
				DrawObjectList[i].VertexBuffer[j1+2] = TMP.z;
				// нормали
				TMP.x = Projectile->DrawObjectList[i].VertexBuffer[j2+3];
				TMP.y = Projectile->DrawObjectList[i].VertexBuffer[j2+4];
				TMP.z = Projectile->DrawObjectList[i].VertexBuffer[j2+5];
				Matrix33CalcPoint(&TMP, Projectile->CurrentRotationMat);
				DrawObjectList[i].VertexBuffer[j1+3] = TMP.x;
				DrawObjectList[i].VertexBuffer[j1+4] = TMP.y;
				DrawObjectList[i].VertexBuffer[j1+5] = TMP.z;
				// текстура
				DrawObjectList[i].VertexBuffer[j1+6] = Projectile->DrawObjectList[i].VertexBuffer[j2+6];
				DrawObjectList[i].VertexBuffer[j1+7] = Projectile->DrawObjectList[i].VertexBuffer[j2+7];
			}


			DrawObjectList[i].Location = VECTOR3D(0.0f,0.0f,0.0f);


			// копируем индексный буфер блока
			DrawObjectList[i].IndexBuffer = new unsigned int[DrawObjectList[i].VertexCount];
			memcpy(DrawObjectList[i].IndexBuffer, Projectile->DrawObjectList[i].IndexBuffer,
					DrawObjectList[i].VertexCount*sizeof(unsigned int));
		}

		float tRadius2 = Projectile->Radius/1.5f;

		// для каждого треугольника - свои данные (фактически, у нас 1 объект, с ним и работаем)
		int Count = 0;
		ExplosionPieceData = new CExplosionPiece[DrawObjectList[0].VertexCount/3];
		for (int i=0; i<DrawObjectList[0].VertexCount; i+=3)
		{
			ExplosionPieceData[Count].Velocity.x = DrawObjectList[0].VertexBuffer[i*DrawObjectList[0].VertexStride];
			ExplosionPieceData[Count].Velocity.y = DrawObjectList[0].VertexBuffer[i*DrawObjectList[0].VertexStride+1];
			ExplosionPieceData[Count].Velocity.z = DrawObjectList[0].VertexBuffer[i*DrawObjectList[0].VertexStride+2];

			float VelocityTMP = vw_Randf0*tRadius2;

			// записываем центр треугольника, оно же базовое ускорение + цент UV, нужно для шейдера
			if (Setup.UseGLSL)
			{
				// Velocity/центр треугольника
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*i+8] = ExplosionPieceData[Count].Velocity.x;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*i+9] = ExplosionPieceData[Count].Velocity.y;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*i+10] = ExplosionPieceData[Count].Velocity.z;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+1)+8] = ExplosionPieceData[Count].Velocity.x;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+1)+9] = ExplosionPieceData[Count].Velocity.y;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+1)+10] = ExplosionPieceData[Count].Velocity.z;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+2)+8] = ExplosionPieceData[Count].Velocity.x;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+2)+9] = ExplosionPieceData[Count].Velocity.y;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+2)+10] = ExplosionPieceData[Count].Velocity.z;
				// acc
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*i+11] = VelocityTMP;
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+1)+11] = DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*i+11];
				DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*(i+2)+11] = DrawObjectList[0].VertexBuffer[DrawObjectList[0].VertexStride*i+11];
			}


			// в данном случае, это и есть направление, потому что геометрия в точке 0,0,0
			ExplosionPieceData[Count].Velocity = ExplosionPieceData[Count].Velocity^VelocityTMP;

			ExplosionPieceData[Count].Life = 1.0f+vw_Randf1/2.0f;

			// делаем анализ для ААBB, смотрим отлет частицы
			float tmpSpeed = ExplosionPieceData[Count].Velocity.x*ExplosionPieceData[Count].Velocity.x +
							ExplosionPieceData[Count].Velocity.y*ExplosionPieceData[Count].Velocity.y +
							ExplosionPieceData[Count].Velocity.z*ExplosionPieceData[Count].Velocity.z;
			if (tmpSpeed > AABBSpeed) AABBSpeed = tmpSpeed;

			Count++;
		}

		// тк был квадрат, теперь вытягиваем нормальное значение
		AABBSpeed = sqrtf(AABBSpeed);

		// установка шейдера
		if (Setup.UseGLSL)
		{
			DrawObjectList[0].ShaderType = 2;
			// дельта скорости
			DrawObjectList[0].ShaderData[0] = 1.0f;
			// общий коэф расстояния
			DrawObjectList[0].ShaderData[1] = 0.0f;
		}



		// удаляем старые буферы, если они есть, создаем новые
		// ! индексный буфер не трогаем, его не надо пересоздавать вообще

		if (DrawObjectList[0].VBO != 0)
		{
			vw_DeleteVBO(*DrawObjectList[0].VBO); delete DrawObjectList[0].VBO; DrawObjectList[0].VBO = 0;
		}
		if (DrawObjectList[0].VAO != 0)
		{
			vw_DeleteVAO(*DrawObjectList[0].VAO); delete DrawObjectList[0].VAO; DrawObjectList[0].VAO = 0;
		}

		// делаем VBO
		DrawObjectList[0].VBO = new unsigned int;
		if (!vw_BuildVBO(DrawObjectList[0].VertexCount, DrawObjectList[0].VertexBuffer, DrawObjectList[0].VertexStride, DrawObjectList[0].VBO))
		{
			delete DrawObjectList[0].VBO; DrawObjectList[0].VBO=0;
		}

		// делаем IBO, создаем его один раз, если его нет
		if (DrawObjectList[0].IBO == 0)
		{
			DrawObjectList[0].IBO = new unsigned int;
			if (!vw_BuildIBO(DrawObjectList[0].VertexCount, DrawObjectList[0].IndexBuffer, DrawObjectList[0].IBO))
			{
				delete DrawObjectList[0].IBO; DrawObjectList[0].IBO=0;
			}
		}

		// делаем VAO
		DrawObjectList[0].VAO = new unsigned int;
		if (!vw_BuildVAO(DrawObjectList[0].VAO, DrawObjectList[0].VertexCount, DrawObjectList[0].VertexFormat, DrawObjectList[0].VertexBuffer,
							DrawObjectList[0].VertexStride*sizeof(float), DrawObjectList[0].VBO,
							DrawObjectList[0].RangeStart, DrawObjectList[0].IndexBuffer, DrawObjectList[0].IBO))
		{
			delete DrawObjectList[0].VAO; DrawObjectList[0].VAO=0;
		}
	}













	// дальше, если не видем точку взрыва не делать... проверяем...
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0]))
	{
		return;
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// звуковые спец эффекты
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	PlayBulletExplosion(Location, NeedExplosionSFX, ExplType);



    // делаем сотрясание камеры, если нужно
	switch (ExplType)
	{
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







