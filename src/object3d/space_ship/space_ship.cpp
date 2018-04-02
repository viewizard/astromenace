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

#include "space_ship.h"
#include "../../game.h"
#include "earth_space_fighter/earth_space_fighter.h"


extern bool PlayerFighterLeftEng;
extern bool PlayerFighterRightEng;
extern cProjectile *StartProjectile;
extern cProjectile *EndProjectile;


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cSpaceShip::cSpaceShip()
{
	// болтание объекта, в случае кораблей болтаем всю модель
	DeviationOn = false;
	DeviationObjQuantity = 1;
	Deviation = new sVECTOR3D[DeviationObjQuantity];
	NeedDeviation = new float[DeviationObjQuantity];
	CurentDeviation = new float[DeviationObjQuantity];
	CurentDeviationSum = new float[DeviationObjQuantity];

	Deviation[0].Set(0.0f, 1.0f, 0.0f);
	NeedDeviation[0] = vw_Randf0*0.1f;
	CurentDeviation[0] = CurentDeviationSum[0] = 0.0f;

	// подключаем к своему списку
	AttachSpaceShip(this);
}



//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cSpaceShip::~cSpaceShip()
{


	if (WeaponSetFire != nullptr) {
		delete [] WeaponSetFire;
		WeaponSetFire = nullptr;
	}
	if (WeaponLocation != nullptr) {
		delete [] WeaponLocation;
		WeaponLocation = nullptr;
	}
	if (WeaponType != nullptr) {
		delete [] WeaponType;
		WeaponType = nullptr;
	}
	if (WeaponYAngle != nullptr) {
		delete [] WeaponYAngle;
		WeaponYAngle = nullptr;
	}
	if (Weapon != nullptr) {
		for (int i = 0; i < WeaponQuantity; i++)
			if (Weapon[i] != nullptr) {
				delete Weapon[i];
				Weapon[i] = nullptr;
			}

		delete [] Weapon;
		Weapon = nullptr;
	}
	if (BossWeaponSetFire != nullptr) {
		delete [] BossWeaponSetFire;
		BossWeaponSetFire = nullptr;
	}
	if (BossWeaponLocation != nullptr) {
		delete [] BossWeaponLocation;
		BossWeaponLocation = nullptr;
	}
	if (BossWeaponType != nullptr) {
		delete [] BossWeaponType;
		BossWeaponType = nullptr;
	}
	if (BossWeaponYAngle != nullptr) {
		delete [] BossWeaponYAngle;
		BossWeaponYAngle = nullptr;
	}
	if (BossWeapon != nullptr) {
		for (int i = 0; i < BossWeaponQuantity; i++)
			if (BossWeapon[i] != nullptr) {
				delete BossWeapon[i];
				BossWeapon[i] = nullptr;
			}

		delete [] BossWeapon;
		BossWeapon = nullptr;
	}
	if (WeaponFlare != nullptr) {
		delete WeaponFlare;
		WeaponFlare = nullptr;
	}

	if (EngineLocation != nullptr) {
		delete [] EngineLocation;
		EngineLocation = nullptr;
	}
	if (Engine != nullptr) {
		for (int i = 0; i < EngineQuantity; i++)
			if (Engine[i] != nullptr) {
				if (!EngineDestroyType) {
					Engine[i]->IsSuppressed = true;
					Engine[i]->DestroyIfNoParticles = true;
				} else {
					vw_ReleaseParticleSystem(Engine[i]);
					Engine[i] = nullptr;
				}
			}
		delete [] Engine;
		Engine = nullptr;
	}

	if (EngineLeftLocation != nullptr) {
		delete [] EngineLeftLocation;
		EngineLeftLocation = nullptr;
	}
	if (EngineLeft != nullptr) {
		for (int i = 0; i < EngineLeftQuantity; i++)
			if (EngineLeft[i] != nullptr) {
				if (!EngineDestroyType) {
					EngineLeft[i]->IsSuppressed = true;
					EngineLeft[i]->DestroyIfNoParticles = true;
				} else {
					vw_ReleaseParticleSystem(EngineLeft[i]);
					EngineLeft[i] = nullptr;
				}
			}
		delete [] EngineLeft;
		EngineLeft = nullptr;
	}

	if (EngineRightLocation != nullptr) {
		delete [] EngineRightLocation;
		EngineRightLocation = nullptr;
	}
	if (EngineRight != nullptr) {
		for (int i = 0; i < EngineRightQuantity; i++)
			if (EngineRight[i] != nullptr) {
				if (!EngineDestroyType) {
					EngineRight[i]->IsSuppressed = true;
					EngineRight[i]->DestroyIfNoParticles = true;
				} else {
					vw_ReleaseParticleSystem(EngineRight[i]);
					EngineRight[i] = nullptr;
				}
			}
		delete [] EngineRight;
		EngineRight = nullptr;
	}


	DetachSpaceShip(this);

}










//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cSpaceShip::SetLocation(sVECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::cObject3D::SetLocation(NewLocation);


	// если оружие вообще есть
	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++) {
			if (Weapon[i] != nullptr)
				Weapon[i]->SetLocation(NewLocation + WeaponLocation[i]);
		}
	if (BossWeapon != nullptr)
		for (int i = 0; i < BossWeaponQuantity; i++) {
			if (BossWeapon[i] != nullptr)
				BossWeapon[i]->SetLocation(NewLocation + BossWeaponLocation[i]);
		}
	if (WeaponFlare != nullptr)
		WeaponFlare->SetLocation(NewLocation + WeaponFlareLocation);


	// положение двигателей
	if (Engine != nullptr)
		for (int i = 0; i < EngineQuantity; i++) {
			if (Engine[i] != nullptr) {
				Engine[i]->MoveSystem(NewLocation + EngineLocation[i]);
				Engine[i]->SetStartLocation(EngineLocation[i] + NewLocation);
			}
		}
	if (EngineLeft != nullptr)
		for (int i = 0; i < EngineLeftQuantity; i++) {
			if (EngineLeft[i] != nullptr) {
				EngineLeft[i]->MoveSystem(NewLocation + EngineLeftLocation[i]);
				EngineLeft[i]->SetStartLocation(EngineLeftLocation[i] + NewLocation);
			}
		}
	if (EngineRight != nullptr)
		for (int i = 0; i < EngineRightQuantity; i++) {
			if (EngineRight[i] != nullptr) {
				EngineRight[i]->MoveSystem(NewLocation + EngineRightLocation[i]);
				EngineRight[i]->SetStartLocation(EngineRightLocation[i] + NewLocation);
			}
		}
}




//-----------------------------------------------------------------------------
// Установка положения объекта, для аркадного режима
//-----------------------------------------------------------------------------
void cSpaceShip::SetLocationArcadePlayer(sVECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::cObject3D::SetLocation(NewLocation);


	// если оружие вообще есть
	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++) {
			if (Weapon[i] != nullptr) Weapon[i]->SetLocation(NewLocation + WeaponLocation[i]);
		}
	if (BossWeapon != nullptr)
		for (int i = 0; i < BossWeaponQuantity; i++) {
			if (BossWeapon[i] != nullptr) BossWeapon[i]->SetLocation(NewLocation + BossWeaponLocation[i]);
		}
	if (WeaponFlare != nullptr) {
		WeaponFlare->SetLocation(NewLocation + WeaponFlareLocation);
	}


	// положение двигателей
	if (Engine != nullptr)
		for (int i = 0; i < EngineQuantity; i++)
			if (Engine[i] != nullptr) {
				Engine[i]->MoveSystem(NewLocation + EngineLocation[i]);
				Engine[i]->SetStartLocation(NewLocation + EngineLocation[i]);
			}
	if (EngineLeft != nullptr)
		for (int i = 0; i < EngineLeftQuantity; i++)
			if (EngineLeft[i] != nullptr) {
				EngineLeft[i]->MoveSystem(NewLocation + EngineLeftLocation[i]);
				EngineLeft[i]->SetStartLocation(NewLocation + EngineLeftLocation[i]);
			}
	if (EngineRight != nullptr)
		for (int i = 0; i < EngineRightQuantity; i++)
			if (EngineRight[i] != nullptr) {
				EngineRight[i]->MoveSystem(NewLocation + EngineRightLocation[i]);
				EngineRight[i]->SetStartLocation(NewLocation + EngineRightLocation[i]);
			}
}






//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void cSpaceShip::SetRotation(sVECTOR3D NewRotation)
{
	// вызываем родительскую функцию
	::cObject3D::SetRotation(NewRotation);



	// оружие
	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++) {
			vw_Matrix33CalcPoint(WeaponLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(WeaponLocation[i], CurrentRotationMat);

			if (Weapon[i] != nullptr)
				// если нужно поворачивать, или не нужно и в этом оружие наведение не используем (ракетная система)
				if (NeedWeaponRotate || (!NeedWeaponRotate && !Weapon[i]->NeedRotateOnTargeting)) {
					Weapon[i]->SetRotation(NewRotation);
					Weapon[i]->SetLocation(Location + WeaponLocation[i]);
				}
		}
	if (BossWeapon != nullptr)
		for (int i = 0; i < BossWeaponQuantity; i++) {
			vw_Matrix33CalcPoint(BossWeaponLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(BossWeaponLocation[i], CurrentRotationMat);

			if (BossWeapon[i] != nullptr)
				// если нужно поворачивать, или не нужно и в этом оружие наведение не используем (ракетная система)
				if (NeedWeaponRotate || (!NeedWeaponRotate && !BossWeapon[i]->NeedRotateOnTargeting)) {
					BossWeapon[i]->SetRotation(NewRotation);
					BossWeapon[i]->SetLocation(Location + BossWeaponLocation[i]);
				}
		}
	if (WeaponFlare != nullptr) {
		vw_Matrix33CalcPoint(WeaponFlareLocation, OldInvRotationMat);
		vw_Matrix33CalcPoint(WeaponFlareLocation, CurrentRotationMat);
		WeaponFlare->SetRotation(NewRotation);
		WeaponFlare->SetLocation(Location + WeaponFlareLocation);
	}




	// двигатели
	if (Engine != nullptr)
		for (int i = 0; i < EngineQuantity; i++) {
			vw_Matrix33CalcPoint(EngineLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(EngineLocation[i], CurrentRotationMat);

			if (Engine[i] != nullptr) {
				if (Engine[i]->SpeedOnCreation == -1.0f) {
					Engine[i]->MoveSystem(EngineLocation[i] + Location);
					Engine[i]->SetStartLocation(EngineLocation[i] + Location);
					Engine[i]->RotateSystemAndParticlesByAngle(Rotation);
				} else {
					Engine[i]->MoveSystemLocation(EngineLocation[i] + Location);
					Engine[i]->RotateSystemByAngle(Rotation);
				}
			}
		}
	if (EngineLeft != nullptr)
		for (int i = 0; i < EngineLeftQuantity; i++) {
			vw_Matrix33CalcPoint(EngineLeftLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(EngineLeftLocation[i], CurrentRotationMat);

			if (EngineLeft[i] != nullptr) {
				EngineLeft[i]->MoveSystemLocation(EngineLeftLocation[i] + Location);
				EngineLeft[i]->SetStartLocation(EngineLeftLocation[i] + Location);
				EngineLeft[i]->RotateSystemByAngle(Rotation);
			}
		}
	if (EngineRight != nullptr)
		for (int i = 0; i < EngineRightQuantity; i++) {
			vw_Matrix33CalcPoint(EngineRightLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(EngineRightLocation[i], CurrentRotationMat);

			if (EngineRight[i] != nullptr) {
				EngineRight[i]->MoveSystemLocation(EngineRightLocation[i] + Location);
				EngineRight[i]->SetStartLocation(EngineRightLocation[i] + Location);
				EngineRight[i]->RotateSystemByAngle(Rotation);
			}
		}
}








//-----------------------------------------------------------------------------
// Обновление данных объектa Object3D
//-----------------------------------------------------------------------------
bool cSpaceShip::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!::cObject3D::Update(Time)) return false;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// обработка скрипта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (StartTimeSheet != nullptr)
		if (!StartTimeSheet->InUse) {
			StartTimeSheet->InUse = true;

			NeedSpeed = StartTimeSheet->Speed;
			NeedAcceler = StartTimeSheet->Acceler;
			NeedSpeedLR = StartTimeSheet->SpeedLR;
			NeedAccelerLR = StartTimeSheet->AccelerLR;
			NeedSpeedUD = StartTimeSheet->SpeedUD;
			NeedAccelerUD = StartTimeSheet->AccelerUD;

			NeedSpeedByCamFB = StartTimeSheet->SpeedByCamFB;
			NeedAccelerByCamFB = StartTimeSheet->AccelerByCamFB;
			NeedSpeedByCamLR = StartTimeSheet->SpeedByCamLR;
			NeedAccelerByCamLR = StartTimeSheet->AccelerByCamLR;
			NeedSpeedByCamUD = StartTimeSheet->SpeedByCamUD;
			NeedAccelerByCamUD = StartTimeSheet->AccelerByCamUD;


			NeedRotate = StartTimeSheet->Rotation;
			RotationSpeed = StartTimeSheet->RotationAcceler;



			if (Weapon != nullptr)
				for (int i = 0; i < WeaponQuantity; i++) {
					if (Weapon[i] != nullptr)
						WeaponSetFire[i] = StartTimeSheet->Fire;
				}
			if (BossWeapon != nullptr)
				for (int i = 0; i < BossWeaponQuantity; i++) {
					if (BossWeapon[i] != nullptr)
						BossWeaponSetFire[i] = StartTimeSheet->BossFire;
				}
		}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если у корабля есть спец средства против ракет...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (WeaponFlare != nullptr) {
		// проверка, есть ли вражеские ракеты?
		bool NeedFlare = false;
		cProjectile *tmpProjectile = StartProjectile;
		while (!NeedFlare && (tmpProjectile != nullptr)) {
			cProjectile *tmpProjectile2 = tmpProjectile->Next;

			if ((((ObjectStatus == 1) && (tmpProjectile->ObjectStatus > 1)) ||
			     // снаряды врагов - с союзниками или игроком
			     ((ObjectStatus > 1) && (tmpProjectile->ObjectStatus == 1)) ||
			     // снаряды игрока со всеми, кроме игрока
			     ((ObjectStatus != 3) && (tmpProjectile->ObjectStatus == 3))) &&
			    (tmpProjectile->ProjectileType == 1))
					NeedFlare = true;

			tmpProjectile = tmpProjectile2;
		}


		tmpProjectile = StartProjectile;
		if (NeedFlare)
			while (tmpProjectile != nullptr) {
				cProjectile *tmpProjectile2 = tmpProjectile->Next;
				// если навелись на этот объект ракетой
				// т.к. только у ракеты тут не ноль
				if (tmpProjectile->Target == this) {
					// начинаем отстреливать фларес
					WeaponFlareSetFire = true;
				}

				tmpProjectile = tmpProjectile2;
			}
		else
			WeaponFlareSetFire = false;
	}







	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// обработка указателей действия, нужно для управления кораблем игрока
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ObjectStatus == 3) {
		if (MoveForward > 0.0f || MoveBackward > 0.0f) {
			NeedSpeed = MaxSpeed*MoveForward-MaxSpeed*MoveBackward;
		} else NeedSpeed = 0.0f;

		if (MoveLeft > 0.0f || MoveRight > 0.0f) {
			NeedRotate.y = MaxAcceler*MaxSpeed*MoveRight-MaxAcceler*MaxSpeed*MoveLeft;
		} else NeedRotate.y = 0.0f;

		if (MoveUp > 0.0f || MoveDown > 0.0f) {
			NeedRotate.x = MaxAcceler*MaxSpeed*MoveUp-MaxAcceler*MaxSpeed*MoveDown;
		} else NeedRotate.x = 0.0f;
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// повотор
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// выключаем двигатели, если нужно - включим
	if (EngineLeft != nullptr)
		for (int i = 0; i < EngineLeftQuantity; i++) {
			if (EngineLeft[i] != nullptr) EngineLeft[i]->IsSuppressed = true;
		}
	if (EngineRight != nullptr)
		for (int i = 0; i < EngineRightQuantity; i++) {
			if (EngineRight[i] != nullptr) EngineRight[i]->IsSuppressed = true;
		}



	// если нужно поворачивать
	if ( !(ObjectStatus == 3) ) {
		if (NeedRotate.x != 0.0f || NeedRotate.y != 0.0f || NeedRotate.z != 0.0f) {
			// Находим допустимый поворот по углу
			sVECTOR3D tmpRotate(0.0f, 0.0f, 0.0f);

			// угол по x
			if (NeedRotate.x != 0.0f) {
				float Sign = 1.0f;
				if (NeedRotate.x < 0.0f) Sign = -1.0f;
				// вычисляем скорость поворота по данным двигателя
				tmpRotate.x = Sign*MaxSpeedRotate*RotationSpeed.x*TimeDelta;
				// смотрим, если уже повернули - снимаем
				if (Sign == 1.0f) {
					if (tmpRotate.x >= NeedRotate.x) {
						tmpRotate.x = NeedRotate.x;
						NeedRotate.x = 0.0f;
					}
				} else {
					if (tmpRotate.x <= NeedRotate.x) {
						tmpRotate.x = NeedRotate.x;
						NeedRotate.x = 0.0f;
					}
				}
				// меняем значение
				if (NeedRotate.x != 0.0f) NeedRotate.x -= tmpRotate.x;
			}

			// угол по y
			if (NeedRotate.y != 0.0f) {
				float Sign = 1.0f;
				if (NeedRotate.y < 0.0f) Sign = -1.0f;
				// вычисляем скорость поворота по данным двигателя
				tmpRotate.y = Sign*MaxSpeedRotate*RotationSpeed.y*TimeDelta;
				// смотрим, если уже повернули - снимаем
				if (Sign == 1.0f) {
					if (tmpRotate.y >= NeedRotate.y) {
						tmpRotate.y = NeedRotate.y;
						NeedRotate.y = 0.0f;
					}
				} else {
					if (tmpRotate.y <= NeedRotate.y) {
						tmpRotate.y = NeedRotate.y;
						NeedRotate.y = 0.0f;
					}
				}
				// меняем значение
				if (NeedRotate.y != 0.0f) NeedRotate.y -= tmpRotate.y;

				// включаем двигатель на поворот
				if (NeedRotate.y > 0.0f) {
					if (EngineLeft != nullptr)
						for (int i = 0; i < EngineLeftQuantity; i++) {
							if (EngineLeft[i] != nullptr)
								EngineLeft[i]->IsSuppressed = false;
						}
				} else {
					if (EngineRight != nullptr)
						for (int i = 0; i < EngineRightQuantity; i++) {
							if (EngineRight[i] != nullptr)
								EngineRight[i]->IsSuppressed = false;
						}
				}
				// создаем эффект торможения другим двигателем
				NeedStopRotation = true;
			}


			// угол по z
			if (NeedRotate.z != 0.0f) {
				float Sign = 1.0f;
				if (NeedRotate.z < 0.0f) Sign = -1.0f;
				// вычисляем скорость поворота по данным двигателя
				tmpRotate.z = Sign*MaxSpeedRotate*RotationSpeed.z*TimeDelta;
				// смотрим, если уже повернули - снимаем
				if (Sign == 1.0f) {
					if (tmpRotate.z >= NeedRotate.z) {
						tmpRotate.z = NeedRotate.z;
						NeedRotate.z = 0.0f;
					}
				} else {
					if (tmpRotate.z <= NeedRotate.z) {
						tmpRotate.z = NeedRotate.z;
						NeedRotate.z = 0.0f;
					}
				}
				// меняем значение
				if (NeedRotate.z != 0.0f) NeedRotate.z -= tmpRotate.z;

				// включаем двигатель на поворот
				if (NeedRotate.z < 0.0f) {
					if (EngineLeft != nullptr)
						for (int i = 0; i < EngineLeftQuantity; i++) {
							if (EngineLeft[i] != nullptr)
								EngineLeft[i]->IsSuppressed = false;
						}
				} else {
					if (EngineRight != nullptr)
						for (int i = 0; i < EngineRightQuantity; i++) {
							if (EngineRight[i] != nullptr)
								EngineRight[i]->IsSuppressed = false;
						}
				}
				// создаем эффект торможения другим двигателем
				NeedStopRotation = true;
			}


			// установка поворота там же сохраняем, какой общий поворот модели
			SetRotation(tmpRotate);

		} else {
			if (NeedStopRotation) {

				if (EngineLeft != nullptr)
					for (int i = 0; i < EngineLeftQuantity; i++) {
						if (EngineLeft[i] != nullptr)
							EngineLeft[i]->IsSuppressed = false;
					}
				if (EngineRight != nullptr)
					for (int i = 0; i < EngineRightQuantity; i++) {
						if (EngineRight[i] != nullptr)
							EngineRight[i]->IsSuppressed = false;
					}

				NeedStopRotation = false;
			} else {
				if (EngineLeft != nullptr)
					for (int i = 0; i < EngineLeftQuantity; i++) {
						if (EngineLeft[i] != nullptr)
							EngineLeft[i]->IsSuppressed = true;
					}
				if (EngineRight != nullptr)
					for (int i = 0; i < EngineRightQuantity; i++) {
						if (EngineRight[i] != nullptr)
							EngineRight[i]->IsSuppressed = true;
					}
			}
		}
	} else {

		if (EngineLeft != nullptr)
			for (int i = 0; i < EngineLeftQuantity; i++) {
				if (EngineLeft[i] != nullptr)
					EngineLeft[i]->IsSuppressed = !PlayerFighterLeftEng;
			}

		if (EngineRight != nullptr)
			for (int i = 0; i < EngineRightQuantity; i++) {
				if (EngineRight[i] != nullptr)
					EngineRight[i]->IsSuppressed = !PlayerFighterRightEng;
			}
	}










	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ускорение-замедление
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// вперед-назад
	// если нужно разогнаться, или управление на игроке - и нужно стремиться к нулю
	if (NeedSpeed != 0.0f || (ObjectStatus == 3 && Speed != 0.0f)) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeed < 0.0f)
			Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f)
			vw_Clamp(NeedSpeed, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeed, -MaxSpeed, 0.0f);

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeed < Speed)
				Sign = -1.0f;
		} else {
			if (NeedSpeed > Speed)
				Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		Acceler = Sign*MaxAcceler*NeedAcceler;

		// считаем текущую скорость
		Speed = Speed + Acceler*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeed <= Speed) {
				Speed = NeedSpeed;
				NeedSpeed = 0.0f;
			}
		} else {
			if (NeedSpeed >= Speed) {
				Speed = NeedSpeed;
				NeedSpeed = 0.0f;
			}
		}


		// если нужны двигатели торможения - включаем маневровые
		if (Sign == -1.0f) {
			if (EngineLeft != nullptr)
				for (int i = 0; i < EngineLeftQuantity; i++) {
					if (EngineLeft[i] != nullptr)
						EngineLeft[i]->IsSuppressed = false;
				}
			if (EngineRight != nullptr)
				for (int i = 0; i < EngineRightQuantity; i++) {
					if (EngineRight[i] != nullptr)
						EngineRight[i]->IsSuppressed = false;
				}
		}
	}
	// влево-вправо
	if (NeedSpeedLR != 0.0f) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeedLR < 0.0f) Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f) {
			vw_Clamp(NeedSpeedLR, 0.0f, MaxSpeed);
		} else {
			vw_Clamp(NeedSpeedLR, -MaxSpeed, 0.0f);
		}

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeedLR < SpeedLR) Sign = -1.0f;
		} else {
			if (NeedSpeedLR > SpeedLR) Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		AccelerLR = Sign*MaxAcceler*NeedAccelerLR;

		// считаем текущую скорость
		SpeedLR = SpeedLR + AccelerLR*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeedLR <= SpeedLR) {
				SpeedLR = NeedSpeedLR;
				NeedSpeedLR = 0.0f;
			}
		} else {
			if (NeedSpeedLR >= SpeedLR) {
				SpeedLR = NeedSpeedLR;
				NeedSpeedLR = 0.0f;
			}
		}

	}
	// вверх-вниз
	if (NeedSpeedUD != 0.0f) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeedUD < 0.0f) Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedUD, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedUD, -MaxSpeed, 0.0f);

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeedUD < SpeedUD)
				Sign = -1.0f;
		} else {
			if (NeedSpeedUD > SpeedUD)
				Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		AccelerUD = Sign*MaxAcceler*NeedAccelerUD;

		// считаем текущую скорость
		SpeedUD = SpeedUD + AccelerUD*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeedUD <= SpeedUD) {
				SpeedUD = NeedSpeedUD;
				NeedSpeedUD = 0.0f;
			}
		} else {
			if (NeedSpeedUD >= SpeedUD) {
				SpeedUD = NeedSpeedUD;
				NeedSpeedUD = 0.0f;
			}
		}
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ускорение-замедление по вектору камеры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// вперед-назад
	if (NeedSpeedByCamFB != 0.0f) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeedByCamFB < 0.0f) Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedByCamFB, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedByCamFB, -MaxSpeed, 0.0f);

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeedByCamFB < SpeedByCamFB)
				Sign = -1.0f;
		} else {
			if (NeedSpeedByCamFB > SpeedByCamFB)
				Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		AccelerByCamFB = Sign*MaxAcceler*NeedAccelerByCamFB;

		// считаем текущую скорость
		SpeedByCamFB = SpeedByCamFB + AccelerByCamFB*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeedByCamFB <= SpeedByCamFB) {
				SpeedByCamFB = NeedSpeedByCamFB;
				NeedSpeedByCamFB = 0.0f;
			}
		} else {
			if (NeedSpeedByCamFB >= SpeedByCamFB) {
				SpeedByCamFB = NeedSpeedByCamFB;
				NeedSpeedByCamFB = 0.0f;
			}
		}
	}
	// влево-вправо
	if (NeedSpeedByCamLR != 0.0f) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeedByCamLR < 0.0f) Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedByCamLR, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedByCamLR, -MaxSpeed, 0.0f);

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeedByCamLR < SpeedByCamLR)
				Sign = -1.0f;
		} else {
			if (NeedSpeedByCamLR > SpeedByCamLR)
				Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		AccelerByCamLR = Sign*MaxAcceler*NeedAccelerByCamLR;

		// считаем текущую скорость
		SpeedByCamLR = SpeedByCamLR + AccelerByCamLR*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeedByCamLR <= SpeedByCamLR) {
				SpeedByCamLR = NeedSpeedByCamLR;
				NeedSpeedByCamLR = 0.0f;
			}
		} else {
			if (NeedSpeedByCamLR >= SpeedByCamLR) {
				SpeedByCamLR = NeedSpeedByCamLR;
				NeedSpeedByCamLR = 0.0f;
			}
		}

	}
	// вверх-вниз
	if (NeedSpeedByCamUD != 0.0f) {
		float Sign = 1.0f;
		// нужно двигать назад
		if (NeedSpeedByCamUD < 0.0f) Sign = -1.0f;

		// "ровняем" скорость под модель
		if (Sign == 1.0f)
			vw_Clamp(NeedSpeedByCamUD, 0.0f, MaxSpeed);
		else
			vw_Clamp(NeedSpeedByCamUD, -MaxSpeed, 0.0f);

		// случай, когда нужно затормозить а не менять направление
		if (Sign == 1.0f) {
			if (NeedSpeedByCamUD < SpeedByCamUD)
				Sign = -1.0f;
		} else {
			if (NeedSpeedByCamUD > SpeedByCamUD)
				Sign = 1.0f;
		}


		// даем полный газ, учитывая сколько процентов можем выдать
		AccelerByCamUD = Sign*MaxAcceler*NeedAccelerByCamUD;

		// считаем текущую скорость
		SpeedByCamUD = SpeedByCamUD + AccelerByCamUD*TimeDelta;

		// смотрим, если уже разогнались - снимаем
		if (Sign == 1.0f) {
			if (NeedSpeedByCamUD <= SpeedByCamUD) {
				SpeedByCamUD = NeedSpeedByCamUD;
				NeedSpeedByCamUD = 0.0f;
			}
		} else {
			if (NeedSpeedByCamUD >= SpeedByCamUD) {
				SpeedByCamUD = NeedSpeedByCamUD;
				NeedSpeedByCamUD = 0.0f;
			}
		}
	}













	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим, есть ли команда открыть огонь
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Weapon != nullptr) {
		// если залп или игрок (игроку регулируем сами)
		if (WeaponFireType == 1 || ObjectStatus == 3) {
			for (int i=0; i<WeaponQuantity; i++)
				if ((Weapon[i] != nullptr) &&
				    (WeaponSetFire[i]))
					Weapon[i]->WeaponFire(Time);
		} else { // переменный огонь

			int PrimCount = 0;
			float PrimTime = 0.0f;
			int FirstWeapon = 6;
			int LastWeapon = 0;

			WeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (int i = 0; i < WeaponQuantity; i++)
				if (Weapon[i] != nullptr) {
					PrimCount++;
					PrimTime += Weapon[i]->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			// если еще не было начальной установки
			if (WeaponGroupCurrentFireNum == -1)
				WeaponGroupCurrentFireNum = FirstWeapon;


			// стреляем
			for (int i = 0; i < WeaponQuantity; i++)
				if ((Weapon[i] != nullptr) &&
				    WeaponSetFire[i] &&
				    (WeaponGroupCurrentFireNum == i) &&
				    (WeaponGroupCurrentFireDelay <= 0.0f)) {
					Weapon[i]->WeaponFire(Time);

					WeaponGroupCurrentFireDelay = PrimTime/(PrimCount*PrimCount);
					WeaponGroupCurrentFireNum ++;
					if (WeaponGroupCurrentFireNum > LastWeapon) WeaponGroupCurrentFireNum = FirstWeapon;

					// если такого оружия нет, берем что есть
					if (Weapon[WeaponGroupCurrentFireNum] == nullptr) {
						bool exit = false;
						while (!exit) {
							WeaponGroupCurrentFireNum ++;
							if (WeaponGroupCurrentFireNum > LastWeapon)
								WeaponGroupCurrentFireNum = FirstWeapon;
							if (Weapon[WeaponGroupCurrentFireNum] != nullptr)
								exit = true;
						}
					}
				}

		}
	}
	if (BossWeapon != nullptr) {
		// если залп
		if (BossWeaponFireType == 1) {
			for (int i=0; i<BossWeaponQuantity; i++) {
				if ((BossWeapon[i] != nullptr) &&
				    (BossWeaponSetFire[i]))
					BossWeapon[i]->WeaponFire(Time);
			}
		} else { // переменный огонь

			int PrimCount = 0;
			float PrimTime = 0.0f;
			int FirstWeapon = 6;
			int LastWeapon = 0;

			BossWeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (int i = 0; i < BossWeaponQuantity; i++)
				if (BossWeapon[i] != nullptr) {
					PrimCount++;
					PrimTime += BossWeapon[i]->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			// если еще не было начальной установки
			if (BossWeaponGroupCurrentFireNum == -1)
				BossWeaponGroupCurrentFireNum = FirstWeapon;


			// стреляем
			for (int i = 0; i < BossWeaponQuantity; i++) {
				if ((BossWeapon[i] != nullptr) &&
				    (BossWeaponSetFire[i]) &&
				    (BossWeaponGroupCurrentFireNum == i) &&
				    (BossWeaponGroupCurrentFireDelay <= 0.0f)) {
					BossWeapon[i]->WeaponFire(Time);

					BossWeaponGroupCurrentFireDelay = PrimTime/(PrimCount*PrimCount);
					BossWeaponGroupCurrentFireNum ++;
					if (BossWeaponGroupCurrentFireNum > LastWeapon)
						BossWeaponGroupCurrentFireNum = FirstWeapon;

					// если такого оружия нет, берем что есть
					if (BossWeapon[BossWeaponGroupCurrentFireNum] == nullptr) {
						bool exit = false;
						while (!exit) {
							BossWeaponGroupCurrentFireNum ++;
							if (BossWeaponGroupCurrentFireNum > LastWeapon)
								BossWeaponGroupCurrentFireNum = FirstWeapon;
							if (BossWeapon[BossWeaponGroupCurrentFireNum] != nullptr)
								exit = true;
						}
					}
				}
			}
		}
	}

	if ((WeaponFlare != nullptr) &&
	    (WeaponFlareSetFire))
		WeaponFlare->WeaponFire(Time);





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// небольшая девиация-болтание корпуса
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (DeviationOn)
		for (int i=0; i<DeviationObjQuantity; i++) {
			float Sign = 1.0f;
			// нужно двигать
			if (NeedDeviation[i] < 0.0f) Sign = -1.0f;
			if (Sign == 1.0f) {
				if (NeedDeviation[i] < CurentDeviationSum[i]) Sign = -1.0f;
			} else {
				if (NeedDeviation[i] > CurentDeviationSum[i]) Sign = 1.0f;
			}

			CurentDeviation[i] = Sign*0.035f*TimeDelta;

			if (Sign == 1.0f) {
				if (NeedDeviation[i] <= CurentDeviationSum[i]+CurentDeviation[i]) {
					CurentDeviation[i] -= CurentDeviationSum[i]+CurentDeviation[i]-NeedDeviation[i];
					CurentDeviationSum[i] += CurentDeviation[i];
					NeedDeviation[i] = vw_Randf0*0.1f;
				} else CurentDeviationSum[i] += CurentDeviation[i];
			} else {
				if (NeedDeviation[i] >= CurentDeviationSum[i]+CurentDeviation[i]) {
					CurentDeviation[i] += CurentDeviationSum[i]+CurentDeviation[i]-NeedDeviation[i];
					CurentDeviationSum[i] += CurentDeviation[i];
					NeedDeviation[i] = vw_Randf0*0.1f;
				} else CurentDeviationSum[i] += CurentDeviation[i];
			}

		}

	// только для корабля игрока - небольшое болтание во время полета
	if ((ObjectStatus == 3) &&
	    (DeviationOn)) {
			NeedWeaponRotate = false;
			SetRotation((Deviation[0]^(CurentDeviation[0]*50.0f)));
		}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// считаем вектор движения
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Velocity = Orientation^(Speed*TimeDelta);
	if (fabs(SpeedLR) > 0.01f) {
		sVECTOR3D tmp(SpeedLR*TimeDelta,0.0f,0.0f);
		vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
		Velocity += tmp;
	}
	if (fabs(SpeedUD) > 0.01f) {
		sVECTOR3D tmp(0.0f,SpeedUD*TimeDelta,0.0f);
		vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
		Velocity += tmp;
	}


	if (fabs(SpeedByCamFB) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement^(SpeedByCamFB*TimeDelta);
		Velocity += tmp;
	}
	if (fabs(SpeedByCamLR) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement^(SpeedByCamLR*TimeDelta);
		vw_RotatePoint(tmp, sVECTOR3D(0.0,-90.0f,0.0f));
		Velocity += tmp;
	}
	if (fabs(SpeedByCamUD) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement^(SpeedByCamUD*TimeDelta);
		vw_RotatePoint(tmp, sVECTOR3D(90.0f,0.0f,0.0f));
		Velocity += tmp;
	}


	// если это не корабль игрока и включена девиация
	if (ObjectStatus != 3)
		if (DeviationOn) Velocity += Deviation[0]^CurentDeviation[0];



	// перемещение объекта, если нужно
	if (Velocity.x != 0.0f || Velocity.y != 0.0f  || Velocity.z != 0.0f ) {
		// делаем сдвиг модели в указанную точку
		SetLocation(Location+Velocity);
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Делаем установку работы двигателей, согласуем со скоростью корабля
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Engine != nullptr) {
		float tmpSpeed = Speed;
		if (tmpSpeed > 6.0f)
			tmpSpeed=6.0f;
		else if (tmpSpeed < -6.0f)
			tmpSpeed=-6.0f;
		tmpSpeed/=2.0f;

		for (int i = 0; i < EngineQuantity; i++) {
			if ((Engine[i] != nullptr) &&
			    (Engine[i]->SpeedOnCreation != -1.0f))
				Engine[i]->Speed = Engine[i]->SpeedOnCreation + tmpSpeed;
		}
	}









	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работа с оружием - наведение его на ближайшую цель
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// а нужно ли стерять
	bool NeedFire = false;
	if (Weapon != nullptr)
		for (int i = 0; i < WeaponQuantity; i++) {
			if (Weapon[i] != nullptr)
				if (WeaponSetFire[i]) NeedFire = true;
		}
	bool NeedBossFire = false;
	if (BossWeapon != nullptr)
		for (int i = 0; i < BossWeaponQuantity; i++) {
			if (BossWeapon[i] != nullptr)
				if (BossWeaponSetFire[i]) NeedBossFire = true;
		}



	// если не игрок игрок и есть режим действия - нужно "искать" противника
	if (ObjectStatus == 1 && NeedFire) {
		sVECTOR3D NeedAngle = Rotation;

		// используем ID как маркер, чтобы не обрабатывать этот объект в процедуре
		int tmpID = ID;
		ID = 111111;

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (Weapon != nullptr) {
			for (int i = 0; i < WeaponQuantity; i++) {
				if ((Weapon[i] != nullptr) &&
				    (Weapon[i]->NeedRotateOnTargeting)) {
					WeaponAvLocation = WeaponAvLocation + WeaponLocation[i] + Weapon[i]->FireLocation + Location;
					UsedWeaponQunt ++;
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;


		int WeapNum = -1;
		sVECTOR3D	FirePos(0.0f,0.0f,0.0f);
		if (Weapon != nullptr) {
			// находим номер
			for (int i = 0; i < WeaponQuantity; i++) {
				if ((Weapon[i] != nullptr) &&
				    (Weapon[i]->NeedRotateOnTargeting) &&
				    (WeapNum == -1)) {
					WeapNum = Weapon[0]->ObjectCreationType;
				}
			}
			if (WeapNum == -1)
				WeapNum = 204; // номер самого простого из пиратского оружия


			int Count = 0;
			for (int i = 0; i < WeaponQuantity; i++) {
				if ((Weapon[i] != nullptr) &&
				    (Weapon[i]->NeedRotateOnTargeting)) {
					FirePos += WeaponLocation[i];
					Count++;
				}
			}
			FirePos = FirePos^(1.0f/Count);
		}

		GetEnemyShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation,
						 CurrentRotationMat, &NeedAngle,	WeapNum);

		// всему оружию ставим нужную ориентацию
		if (Weapon != nullptr) {
			for (int i = 0; i < WeaponQuantity; i++) {
				if ((Weapon[i] != nullptr) &&
				    (Weapon[i]->NeedRotateOnTargeting)) {
					NeedAngle.y = Weapon[i]->Rotation.y;
					NeedAngle.z = Weapon[i]->Rotation.z;

					Weapon[i]->SetRotation(Weapon[i]->Rotation^(-1));
					Weapon[i]->SetRotation(NeedAngle);
				}
			}
		}

		ID = tmpID;
	}
	if ((ObjectStatus == 1) && NeedBossFire) {
		sVECTOR3D NeedAngle = Rotation;

		// используем ID как маркер, чтобы не обрабатывать этот объект в процедуре
		int tmpID = ID;
		ID = 111111;

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (BossWeapon != nullptr) {
			for (int i=0; i<BossWeaponQuantity; i++) {
				if ((BossWeapon[i] != nullptr) &&
				    (BossWeapon[i]->NeedRotateOnTargeting)) {
					WeaponAvLocation = WeaponAvLocation + BossWeaponLocation[i] + BossWeapon[i]->FireLocation + Location;
					UsedWeaponQunt ++;
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;

		int WeapNum = -1;
		sVECTOR3D FirePos(0.0f,0.0f,0.0f);
		if (BossWeapon != nullptr) {
			// находим номер
			for (int i = 0; i < BossWeaponQuantity; i++) {
				if ((BossWeapon[i] != nullptr) &&
				    (BossWeapon[i]->NeedRotateOnTargeting) &&
				    (WeapNum == -1)) {
					WeapNum = BossWeapon[0]->ObjectCreationType;
				}
			}
			if (WeapNum == -1)
				WeapNum = 204; // номер самого простого из пиратского оружия


			int Count = 0;
			for (int i = 0; i < BossWeaponQuantity; i++) {
				if ((BossWeapon[i] != nullptr) &&
				    (BossWeapon[i]->NeedRotateOnTargeting)) {
					FirePos += BossWeaponLocation[i];
					Count++;
				}
			}
			FirePos = FirePos^(1.0f/Count);
		}

		GetEnemyShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation,
						 CurrentRotationMat, &NeedAngle,	WeapNum);

		// всему оружию ставим нужную ориентацию
		if (BossWeapon != nullptr) {
			for (int i = 0; i < BossWeaponQuantity; i++) {
				if ((BossWeapon[i] != nullptr) &&
				    (BossWeapon[i]->NeedRotateOnTargeting)) {
					NeedAngle.y = BossWeapon[i]->Rotation.y;
					NeedAngle.z = BossWeapon[i]->Rotation.z;

					BossWeapon[i]->SetRotation(BossWeapon[i]->Rotation^(-1));
					BossWeapon[i]->SetRotation(NeedAngle);
				}
			}
		}

		ID = tmpID;
	}





	// если в игре, а не в меню - иначе в оружейной дергается оружие
	if (MenuStatus == eMenuStatus::GAME)
		// так стреляют только свои :)
		if (ObjectStatus == 2) {
			// используем ID как маркер, чтобы не обрабатывать этот объект в процедуре
			int tmpID = ID;
			ID = 111111;

			// находим среднюю точку положение оружия
			sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
			int UsedWeaponQunt = 0;
			if (Weapon != nullptr) {
				for (int i=0; i<WeaponQuantity; i++) {
					if (Weapon[i] != nullptr) {
						WeaponAvLocation = WeaponAvLocation + WeaponLocation[i] + Weapon[i]->FireLocation + Location;
						UsedWeaponQunt ++;
					}
				}
			}
			WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
			WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
			WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;


			sVECTOR3D NeedAngle = Rotation;

			// всему оружию ставим нужную ориентацию
			if (Weapon != nullptr) {
				// ставим скорость наведения оружия
				float TargetingSpeed = 1.0f;

				for (int i = 0; i < WeaponQuantity; i++) {
					if ((Weapon[i] !=nullptr) &&
					    (Weapon[i]->NeedRotateOnTargeting)) {
						NeedAngle = Rotation;
						// добавляем базовый угол, чтобы по умолчанию устанавливало его
						NeedAngle.y += WeaponYAngle[i];

						GetShipOnTargetOrientateion(ObjectStatus, Location + WeaponLocation[i] + Weapon[i]->FireLocation, Rotation,
									    Length, CurrentRotationMat, &NeedAngle, Width, true, true,
									    Location + WeaponLocation[i] + Weapon[i]->FireLocation, Weapon[i]->ObjectCreationType);

						sVECTOR3D NeedAngleTmp = NeedAngle;

						// учитываем скорость поворота по вертикали
						if (Weapon[i]->Rotation.x < NeedAngle.x) {
							float NeedAngle_x = Weapon[i]->Rotation.x+40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_x > NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;

						}
						if (Weapon[i]->Rotation.x > NeedAngle.x) {
							float NeedAngle_x = Weapon[i]->Rotation.x-40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_x < NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;
						}

						// учитываем скорость поворота по горизонтали
						float Min = 0.0f;
						float Max = 0.0f;
						GetShipWeaponSlotAngle(Setup.Profile[CurrentProfile].Ship, i, &Min, &Max);
						if (Weapon[i]->Rotation.y < NeedAngle.y) {
							float NeedAngle_y = Weapon[i]->Rotation.y+40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_y > NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							// проверка на достижение предела поворота
							if (NeedAngle.y > Max+Rotation.y)
								NeedAngle.y = Max+Rotation.y;
						}
						if (Weapon[i]->Rotation.y > NeedAngle.y) {
							float NeedAngle_y = Weapon[i]->Rotation.y-40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_y < NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							// проверка на достижение предела поворота
							if (NeedAngle.y < Min+Rotation.y)
								NeedAngle.y = Min+Rotation.y;
						}


						// если выключен прикол с поворотом - моментально поворачиваем ствол
						if (GameWeaponTargetingMode == 1)
							NeedAngle = NeedAngleTmp;


						// если это не ракетные системы, нужно повернуть
						if (Weapon[i]->ObjectCreationType < 16) {
							Weapon[i]->SetRotation(Weapon[i]->Rotation^(-1));
							Weapon[i]->SetRotation(NeedAngle);
						}
					}
				}
			}

			ID = tmpID;
		}






	// если корабль игрока
	if (ObjectStatus == 3) {
		// используем ID как маркер, чтобы не обрабатывать этот объект в процедуре
		int tmpID = ID;
		ID = 111111;

		// ставим скорость наведения оружия
		float TargetingSpeed = GameTargetingMechanicSystem*1.0f;

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (Weapon != nullptr) {
			for (int i = 0; i < WeaponQuantity; i++) {
				if (Weapon[i] != nullptr) {
					WeaponAvLocation = WeaponAvLocation + WeaponLocation[i] + Weapon[i]->FireLocation + Location;
					UsedWeaponQunt ++;
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;


		sVECTOR3D NeedAngle = Rotation;



// у оружия есть (для каждого слота в корабле свои)
// мин-макс и базовый (выставляет пользователь) углы поворота орудия
// передавать на проверку всегда базовый угол!!! не текущий!

		/* типы наведения
		1 - простое наведение всех орудий по кораблю
		2 - простое наведение всех орудий по кораблю + доворот на центр
		3 - наведение каждого орудия на свою цель
		4 - наведение каждого орудия на свою цель + доворот

		*/

		// т.к. у нас включена девиация, нужно принять меры
		float RotationMat2[9];
		memcpy(RotationMat2, CurrentRotationMat, sizeof(CurrentRotationMat[0])*9);
		sVECTOR3D Rotation2 = Rotation;

		if (DeviationOn) {
			Rotation2 = Rotation - (Deviation[0]^(CurentDeviation[0]*50.0f));

			vw_Matrix33CreateRotate(RotationMat2, Rotation2);
		}



		// всему оружию ставим нужную ориентацию
		if (Weapon != nullptr)
			for (int i = 0; i < WeaponQuantity; i++) {
				if ((Weapon[i] != nullptr) &&
				    (Weapon[i]->NeedRotateOnTargeting)) {
					NeedAngle = Rotation2;
					// добавляем базовый угол, чтобы по умолчанию устанавливало его
					NeedAngle.y += WeaponYAngle[i];

					switch (GameTargetingSystem) {
					case 1:
						GetShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation2,
									    Length, RotationMat2, &NeedAngle, Width, false, true,
									    Location + WeaponLocation[i] + Weapon[i]->FireLocation, Weapon[i]->ObjectCreationType);
						break;
					case 2:
						GetShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation2,
									    Length, RotationMat2, &NeedAngle, Width, true, true,
									    Location + WeaponLocation[i] + Weapon[i]->FireLocation, Weapon[i]->ObjectCreationType);
						break;
					case 3:
						GetShipOnTargetOrientateion(ObjectStatus, Location + WeaponLocation[i] + Weapon[i]->FireLocation, Weapon[i]->Rotation,
									    Length,Weapon[i]->CurrentRotationMat, &NeedAngle, Weapon[i]->Width, false, true,
									    Location + WeaponLocation[i] + Weapon[i]->FireLocation, Weapon[i]->ObjectCreationType);
						break;
					case 4:
						GetShipOnTargetOrientateion(ObjectStatus, Location + WeaponLocation[i] + Weapon[i]->FireLocation,
									    sVECTOR3D(Weapon[i]->Rotation.x, 0 , Weapon[i]->Rotation.z) + sVECTOR3D(0,WeaponYAngle[i],0),
									    Length,Weapon[i]->CurrentRotationMat, &NeedAngle, Width, false, true,
									    Location + WeaponLocation[i] + Weapon[i]->FireLocation, Weapon[i]->ObjectCreationType);
						break;
					}


					sVECTOR3D NeedAngleTmp = NeedAngle;

					// учитываем скорость поворота по вертикали
					if (Weapon[i]->Rotation.x < NeedAngle.x) {
						float NeedAngle_x = Weapon[i]->Rotation.x+40.0f*TargetingSpeed*TimeDelta;
						if (NeedAngle_x > NeedAngle.x)
							NeedAngle_x = NeedAngle.x;
						NeedAngle.x = NeedAngle_x;

					}
					if (Weapon[i]->Rotation.x > NeedAngle.x) {
						float NeedAngle_x = Weapon[i]->Rotation.x-40.0f*TargetingSpeed*TimeDelta;
						if (NeedAngle_x < NeedAngle.x)
							NeedAngle_x = NeedAngle.x;
						NeedAngle.x = NeedAngle_x;
					}

					// учитываем скорость поворота по горизонтали
					float Min = 0.0f;
					float Max = 0.0f;
					GetShipWeaponSlotAngle(Setup.Profile[CurrentProfile].Ship, i, &Min, &Max);
					if (Weapon[i]->Rotation.y > NeedAngle.y) {
						float NeedAngle_y = Weapon[i]->Rotation.y+40.0f*TargetingSpeed*TimeDelta;
						if (NeedAngle_y > NeedAngle.y)
							NeedAngle_y = NeedAngle.y;
						NeedAngle.y = NeedAngle_y;
						// проверка на достижение предела поворота
						if (NeedAngle.y > Max+Weapon[i]->Rotation.y)
							NeedAngle.y = Max+Weapon[i]->Rotation.y;
					}
					if (Weapon[i]->Rotation.y < NeedAngle.y) {
						float NeedAngle_y = Weapon[i]->Rotation.y-40.0f*TargetingSpeed*TimeDelta;
						if (NeedAngle_y < NeedAngle.y)
							NeedAngle_y = NeedAngle.y;
						NeedAngle.y = NeedAngle_y;
						// проверка на достижение предела поворота
						if (NeedAngle.y < Min+Weapon[i]->Rotation.y)
							NeedAngle.y = Min+Weapon[i]->Rotation.y;
					}


					// если выключен прикол с поворотом - моментально поворачиваем ствол
					if (GameWeaponTargetingMode == 1)
						NeedAngle = NeedAngleTmp;


					Weapon[i]->SetRotation(Weapon[i]->Rotation^(-1));
					Weapon[i]->SetRotation(NeedAngle);
				}
			}

		ID = tmpID;
	}




	// объект в порядке - удалять не нужно
	return true;
}






