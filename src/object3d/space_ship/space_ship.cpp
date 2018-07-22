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

// TODO translate comments

#include "space_ship.h"
#include "../../game.h"
#include "../../config/config.h"
#include "../../script/script.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// Указатели на начальный и конечный объект в списке
cSpaceShip *StartSpaceShip = nullptr;
cSpaceShip *EndSpaceShip = nullptr;

extern bool PlayerFighterLeftEng;
extern bool PlayerFighterRightEng;


//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
static void AttachSpaceShip(cSpaceShip *SpaceShip)
{
	if (SpaceShip == nullptr)
		return;

	// первый в списке...
	if (EndSpaceShip == nullptr) {
		SpaceShip->Prev = nullptr;
		SpaceShip->Next = nullptr;
		StartSpaceShip = SpaceShip;
		EndSpaceShip = SpaceShip;
	} else { // продолжаем заполнение...
		SpaceShip->Prev = EndSpaceShip;
		SpaceShip->Next = nullptr;
		EndSpaceShip->Next = SpaceShip;
		EndSpaceShip = SpaceShip;
	}
}

//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
static void DetachSpaceShip(cSpaceShip *SpaceShip)
{
	if (SpaceShip == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartSpaceShip == SpaceShip)
		StartSpaceShip = SpaceShip->Next;
	if (EndSpaceShip == SpaceShip)
		EndSpaceShip = SpaceShip->Prev;

	if (SpaceShip->Next != nullptr)
		SpaceShip->Next->Prev = SpaceShip->Prev;
	else if (SpaceShip->Prev != nullptr)
		SpaceShip->Prev->Next = nullptr;

	if (SpaceShip->Prev != nullptr)
		SpaceShip->Prev->Next = SpaceShip->Next;
	else if (SpaceShip->Next != nullptr)
		SpaceShip->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceShip(float Time)
{
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp != nullptr) {
		cSpaceShip *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			// если его нужно уничтожить - делаем это
			delete tmp;
		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllSpaceShips(bool VertexOnlyPass, unsigned int ShadowMap)
{
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp != nullptr) {
		cSpaceShip *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceShips()
{
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp != nullptr) {
		cSpaceShip *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartSpaceShip = nullptr;
	EndSpaceShip = nullptr;
}

/*
 * Managed cycle for each space ship.
 */
void ForEachSpaceShip(std::function<void (cSpaceShip &Object, eShipCycle &Command)> function)
{
	cSpaceShip *tmpShip = StartSpaceShip;
	while (tmpShip) {
		cSpaceShip *tmpShipNext = tmpShip->Next;
		eShipCycle Command{eShipCycle::Continue};
		function(*tmpShip, Command);
		switch (Command) {
		case eShipCycle::Continue:
			break;
		case eShipCycle::Break:
			return;
		case eShipCycle::DeleteObjectAndContinue:
			delete tmpShip;
			break;
		case eShipCycle::DeleteObjectAndBreak:
			delete tmpShip;
			return;
		}
		tmpShip = tmpShipNext;
	}
}

//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cSpaceShip::cSpaceShip()
{
	// подключаем к своему списку
	AttachSpaceShip(this);
}

//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cSpaceShip::~cSpaceShip()
{
	for (auto &tmpEngine : Engines) {
		if (auto sharedEngine = tmpEngine.lock()) {
			if (!EngineDestroyType) {
				sharedEngine->IsSuppressed = true;
				sharedEngine->DestroyIfNoParticles = true;
			} else
				vw_ReleaseParticleSystem(sharedEngine);
		}
	}

	if (!EnginesLeft.empty()) {
		for (auto &tmpEngineLeft : EnginesLeft) {
			if (auto sharedEngineLeft = tmpEngineLeft.lock()) {
				if (!EngineDestroyType) {
					sharedEngineLeft->IsSuppressed = true;
					sharedEngineLeft->DestroyIfNoParticles = true;
				} else
					vw_ReleaseParticleSystem(sharedEngineLeft);
			}
		}
	}

	if (!EnginesRight.empty()) {
		for (auto &tmpEngineRight : EnginesRight) {
			if (auto sharedEngineRight = tmpEngineRight.lock()) {
				if (!EngineDestroyType) {
					sharedEngineRight->IsSuppressed = true;
					sharedEngineRight->DestroyIfNoParticles = true;
				} else
					vw_ReleaseParticleSystem(sharedEngineRight);
			}
		}
	}

	DetachSpaceShip(this);
}

//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cSpaceShip::SetLocation(const sVECTOR3D &NewLocation)
{
	// вызываем родительскую функцию
	cObject3D::SetLocation(NewLocation);

	// если оружие вообще есть
	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpWeaponSlot.Location + NewLocation);
		}
	}
	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpBossWeaponSlot.Location + NewLocation);
		}
	}
	if (!FlareWeaponSlots.empty()) {
		for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
			if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpFlareWeaponSlot.Location + NewLocation);
		}
	}

	// положение двигателей
	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			sharedEngine->MoveSystem(NewLocation + EnginesLocation[i]);
			sharedEngine->SetStartLocation(EnginesLocation[i] + NewLocation);
		}
	}
	if (!EnginesLeft.empty()) {
		for (unsigned int i = 0; i < EnginesLeft.size(); i++) {
			if (auto sharedEngineLeft = EnginesLeft[i].lock()) {
				sharedEngineLeft->MoveSystem(NewLocation + EnginesLeftLocation[i]);
				sharedEngineLeft->SetStartLocation(EnginesLeftLocation[i] + NewLocation);
			}
		}
	}
	if (!EnginesRight.empty()) {
		for (unsigned int i = 0; i < EnginesRight.size(); i++) {
			if (auto sharedEngineRight = EnginesRight[i].lock()) {
				sharedEngineRight->MoveSystem(NewLocation + EnginesRightLocation[i]);
				sharedEngineRight->SetStartLocation(EnginesRightLocation[i] + NewLocation);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Установка положения объекта, для аркадного режима
//-----------------------------------------------------------------------------
void cSpaceShip::SetLocationArcadePlayer(const sVECTOR3D &NewLocation)
{
	// вызываем родительскую функцию
	cObject3D::SetLocation(NewLocation);

	// если оружие вообще есть
	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpWeaponSlot.Location + NewLocation);
		}
	}
	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpBossWeaponSlot.Location + NewLocation);
		}
	}
	if (!FlareWeaponSlots.empty()) {
		for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
			if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(tmpFlareWeaponSlot.Location + NewLocation);
		}
	}

	// положение двигателей
	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			sharedEngine->MoveSystem(NewLocation + EnginesLocation[i]);
			sharedEngine->SetStartLocation(NewLocation + EnginesLocation[i]);
		}
	}
	if (!EnginesLeft.empty()) {
		for (unsigned int i = 0; i < EnginesLeft.size(); i++) {
			if (auto sharedEngineLeft = EnginesLeft[i].lock()) {
				sharedEngineLeft->MoveSystem(NewLocation + EnginesLeftLocation[i]);
				sharedEngineLeft->SetStartLocation(NewLocation + EnginesLeftLocation[i]);
			}
		}
	}
	if (!EnginesRight.empty()) {
		for (unsigned int i = 0; i < EnginesRight.size(); i++) {
			if (auto sharedEngineRight = EnginesRight[i].lock()) {
				sharedEngineRight->MoveSystem(NewLocation + EnginesRightLocation[i]);
				sharedEngineRight->SetStartLocation(NewLocation + EnginesRightLocation[i]);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void cSpaceShip::SetRotation(const sVECTOR3D &NewRotation)
{
	// вызываем родительскую функцию
	cObject3D::SetRotation(NewRotation);

	// оружие
	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
				vw_Matrix33CalcPoint(tmpWeaponSlot.Location, OldInvRotationMat);
				vw_Matrix33CalcPoint(tmpWeaponSlot.Location, CurrentRotationMat);

				sharedWeapon->SetRotation(NewRotation);
				sharedWeapon->SetLocation(Location + tmpWeaponSlot.Location);
			}
		}
	}
	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock()) {
				vw_Matrix33CalcPoint(tmpBossWeaponSlot.Location, OldInvRotationMat);
				vw_Matrix33CalcPoint(tmpBossWeaponSlot.Location, CurrentRotationMat);

				sharedWeapon->SetRotation(NewRotation);
				sharedWeapon->SetLocation(Location + tmpBossWeaponSlot.Location);
			}
		}
	}
	if (!FlareWeaponSlots.empty()) {
		for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
			if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock()) {
				vw_Matrix33CalcPoint(tmpFlareWeaponSlot.Location, OldInvRotationMat);
				vw_Matrix33CalcPoint(tmpFlareWeaponSlot.Location, CurrentRotationMat);

				sharedWeapon->SetRotation(NewRotation);
				sharedWeapon->SetLocation(Location + tmpFlareWeaponSlot.Location);
			}
		}
	}

	// двигатели
	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			vw_Matrix33CalcPoint(EnginesLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(EnginesLocation[i], CurrentRotationMat);

			if (sharedEngine->SpeedOnCreation == -1.0f) {
				sharedEngine->MoveSystem(EnginesLocation[i] + Location);
				sharedEngine->SetStartLocation(EnginesLocation[i] + Location);
				sharedEngine->RotateSystemAndParticlesByAngle(Rotation);
			} else {
				sharedEngine->MoveSystemLocation(EnginesLocation[i] + Location);
				sharedEngine->RotateSystemByAngle(Rotation);
			}
		}
	}
	if (!EnginesLeft.empty()) {
		for (unsigned int i = 0; i < EnginesLeft.size(); i++) {
			if (auto sharedEngineLeft = EnginesLeft[i].lock()) {
				vw_Matrix33CalcPoint(EnginesLeftLocation[i], OldInvRotationMat);
				vw_Matrix33CalcPoint(EnginesLeftLocation[i], CurrentRotationMat);

				sharedEngineLeft->MoveSystemLocation(EnginesLeftLocation[i] + Location);
				sharedEngineLeft->SetStartLocation(EnginesLeftLocation[i] + Location);
				sharedEngineLeft->RotateSystemByAngle(Rotation);
			}
		}
	}
	if (!EnginesRight.empty()) {
		for (unsigned int i = 0; i < EnginesRight.size(); i++) {
			if (auto sharedEngineRight = EnginesRight[i].lock()) {
				vw_Matrix33CalcPoint(EnginesRightLocation[i], OldInvRotationMat);
				vw_Matrix33CalcPoint(EnginesRightLocation[i], CurrentRotationMat);

				sharedEngineRight->MoveSystemLocation(EnginesRightLocation[i] + Location);
				sharedEngineRight->SetStartLocation(EnginesRightLocation[i] + Location);
				sharedEngineRight->RotateSystemByAngle(Rotation);
			}
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
	if (!cObject3D::Update(Time)) return false;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// обработка скрипта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!TimeSheetList.empty() &&
	    !TimeSheetList.front().InUse) {
		TimeSheetList.front().InUse = true;

		NeedSpeed = TimeSheetList.front().Speed;
		NeedAcceler = TimeSheetList.front().Acceler;
		NeedSpeedLR = TimeSheetList.front().SpeedLR;
		NeedAccelerLR = TimeSheetList.front().AccelerLR;
		NeedSpeedUD = TimeSheetList.front().SpeedUD;
		NeedAccelerUD = TimeSheetList.front().AccelerUD;

		NeedSpeedByCamFB = TimeSheetList.front().SpeedByCamFB;
		NeedAccelerByCamFB = TimeSheetList.front().AccelerByCamFB;
		NeedSpeedByCamLR = TimeSheetList.front().SpeedByCamLR;
		NeedAccelerByCamLR = TimeSheetList.front().AccelerByCamLR;
		NeedSpeedByCamUD = TimeSheetList.front().SpeedByCamUD;
		NeedAccelerByCamUD = TimeSheetList.front().AccelerByCamUD;

		NeedRotate = TimeSheetList.front().Rotation;
		RotationSpeed = TimeSheetList.front().RotationAcceler;

		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (!tmpWeaponSlot.Weapon.expired())
					tmpWeaponSlot.SetFire = TimeSheetList.front().Fire;
			}
		}
		if (!BossWeaponSlots.empty()) {
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (!tmpBossWeaponSlot.Weapon.expired())
					tmpBossWeaponSlot.SetFire = TimeSheetList.front().BossFire;
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если у корабля есть спец средства против ракет...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!FlareWeaponSlots.empty()) {
		bool NeedFlare{false};
		ForEachProjectile([&] (cProjectile &Projectile, eProjectileCycle &UNUSED(Command)) {
			// homing missile or homing mine targeted on this ship
			if (Projectile.Target == this) {
				NeedFlare = true;
				Projectile.Target = nullptr; // reset target, since we will fire flares
			}
		});

		if (NeedFlare) {
			for (auto &tmpFlareWeaponSlot : FlareWeaponSlots) {
				if (auto sharedWeapon = tmpFlareWeaponSlot.Weapon.lock())
					sharedWeapon->WeaponFire(Time);
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// обработка указателей действия, нужно для управления кораблем игрока
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ObjectStatus == eObjectStatus::Player) {
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

	// если нужно поворачивать
	if (ObjectStatus != eObjectStatus::Player) {

		// выключаем двигатели, если нужно - включим
		if (!EnginesLeft.empty()) {
			for (auto &tmpEngineLeft : EnginesLeft) {
				if (auto sharedEngineLeft = tmpEngineLeft.lock())
					sharedEngineLeft->IsSuppressed = true;
			}
		}
		if (!EnginesRight.empty()) {
			for (auto &tmpEngineRight : EnginesRight) {
				if (auto sharedEngineRight = tmpEngineRight.lock())
					sharedEngineRight->IsSuppressed = true;
			}
		}

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
				if (NeedRotate.x != 0.0f)
					NeedRotate.x -= tmpRotate.x;
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
				if (NeedRotate.y != 0.0f)
					NeedRotate.y -= tmpRotate.y;

				// включаем двигатель на поворот
				if (NeedRotate.y < 0.0f) {
					if (!EnginesLeft.empty()) {
						for (auto &tmpEngineLeft : EnginesLeft) {
							if (auto sharedEngineLeft = tmpEngineLeft.lock())
								sharedEngineLeft->IsSuppressed = false;
						}
					}
				} else {
					if (!EnginesRight.empty()) {
						for (auto &tmpEngineRight : EnginesRight) {
							if (auto sharedEngineRight = tmpEngineRight.lock())
								sharedEngineRight->IsSuppressed = false;
						}
					}
				}
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
				if (NeedRotate.z != 0.0f)
					NeedRotate.z -= tmpRotate.z;
			}


			// установка поворота там же сохраняем, какой общий поворот модели
			SetRotation(tmpRotate);

		}
	} else {
		if (!EnginesLeft.empty()) {
			for (auto &tmpEngineLeft : EnginesLeft) {
				if (auto sharedEngineLeft = tmpEngineLeft.lock())
					sharedEngineLeft->IsSuppressed = !PlayerFighterLeftEng;
			}
		}
		if (!EnginesRight.empty()) {
			for (auto &tmpEngineRight : EnginesRight) {
				if (auto sharedEngineRight = tmpEngineRight.lock())
					sharedEngineRight->IsSuppressed = !PlayerFighterRightEng;
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ускорение-замедление
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// вперед-назад
	// если нужно разогнаться, или управление на игроке - и нужно стремиться к нулю
	if ((NeedSpeed != 0.0f) || ((ObjectStatus == eObjectStatus::Player) && (Speed != 0.0f))) {
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
			if (!EnginesLeft.empty()) {
				for (auto &tmpEngineLeft : EnginesLeft) {
					if (auto sharedEngineLeft = tmpEngineLeft.lock())
						sharedEngineLeft->IsSuppressed = false;
				}
			}
			if (!EnginesRight.empty()) {
				for (auto &tmpEngineRight : EnginesRight) {
					if (auto sharedEngineRight = tmpEngineRight.lock())
						sharedEngineRight->IsSuppressed = false;
				}
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
	if (!WeaponSlots.empty()) {
		// если залп или игрок (игроку регулируем сами)
		if ((WeaponFireType == 1) || (ObjectStatus == eObjectStatus::Player)) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (tmpWeaponSlot.SetFire)
					if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
						sharedWeapon->WeaponFire(Time);
			}
		} else { // переменный огонь

			int PrimCount = 0;
			float PrimTime = 0.0f;
			unsigned FirstWeapon = WeaponSlots.size();
			unsigned LastWeapon = 0;

			WeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (unsigned i = 0; i < WeaponSlots.size(); i++)
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					PrimCount++;
					PrimTime += sharedWeapon->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			// если еще не было начальной установки
			if (WeaponGroupCurrentFireNum == -1)
				WeaponGroupCurrentFireNum = FirstWeapon;


			// стреляем
			for (unsigned i = 0; i < WeaponSlots.size(); i++)
				if (!WeaponSlots[i].Weapon.expired() &&
				    WeaponSlots[i].SetFire &&
				    (WeaponGroupCurrentFireNum == static_cast<int>(i)) &&
				    (WeaponGroupCurrentFireDelay <= 0.0f)) {
					if (auto sharedWeapon = WeaponSlots[i].Weapon.lock())
						sharedWeapon->WeaponFire(Time);

					WeaponGroupCurrentFireDelay = PrimTime/(PrimCount*PrimCount);
					WeaponGroupCurrentFireNum++;
					if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
						WeaponGroupCurrentFireNum = FirstWeapon;

					// если такого оружия нет, берем что есть
					if (WeaponSlots[WeaponGroupCurrentFireNum].Weapon.expired()) {
						bool exit = false;
						while (!exit) {
							WeaponGroupCurrentFireNum++;
							if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
								WeaponGroupCurrentFireNum = FirstWeapon;
							if (!WeaponSlots[WeaponGroupCurrentFireNum].Weapon.expired())
								exit = true;
						}
					}
				}
		}
	}
	if (!BossWeaponSlots.empty()) {
		// если залп
		if (BossWeaponFireType == 1) {
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (tmpBossWeaponSlot.SetFire)
					if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock())
						sharedWeapon->WeaponFire(Time);
			}
		} else { // переменный огонь

			int PrimCount = 0;
			float PrimTime = 0.0f;
			unsigned FirstWeapon = BossWeaponSlots.size();
			unsigned LastWeapon = 0;

			BossWeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (unsigned i = 0; i < BossWeaponSlots.size(); i++) {
				if (auto sharedWeapon = BossWeaponSlots[i].Weapon.lock()) {
					PrimCount++;
					PrimTime += sharedWeapon->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			}
			// если еще не было начальной установки
			if (BossWeaponGroupCurrentFireNum == -1)
				BossWeaponGroupCurrentFireNum = FirstWeapon;

			// стреляем
			for (unsigned i = 0; i < BossWeaponSlots.size(); i++) {
				if (!BossWeaponSlots[i].Weapon.expired() &&
				    BossWeaponSlots[i].SetFire &&
				    (BossWeaponGroupCurrentFireNum == static_cast<int>(i)) &&
				    (BossWeaponGroupCurrentFireDelay <= 0.0f)) {
					if (auto sharedWeapon = BossWeaponSlots[i].Weapon.lock())
						sharedWeapon->WeaponFire(Time);

					BossWeaponGroupCurrentFireDelay = PrimTime/(PrimCount*PrimCount);
					BossWeaponGroupCurrentFireNum++;
					if (BossWeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
						BossWeaponGroupCurrentFireNum = FirstWeapon;

					// если такого оружия нет, берем что есть
					if (BossWeaponSlots[BossWeaponGroupCurrentFireNum].Weapon.expired()) {
						bool exit = false;
						while (!exit) {
							BossWeaponGroupCurrentFireNum++;
							if (BossWeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
								BossWeaponGroupCurrentFireNum = FirstWeapon;
							if (!BossWeaponSlots[BossWeaponGroupCurrentFireNum].Weapon.expired())
								exit = true;
						}
					}
				}
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// небольшая девиация-болтание корпуса
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!ShipShake.empty()) {
		ShipShake[0].Update(TimeDelta, [&] (const float ShakeIncrement,
						    const sVECTOR3D &ShakeDirection,
						    const unsigned UNUSED(ShakeChunkNum)) {
			SetRotation(ShakeDirection ^ (ShakeIncrement * 50.0f));
		});
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
		vw_RotatePoint(tmp, sVECTOR3D{0.0, -90.0f ,0.0f});
		Velocity += tmp;
	}
	if (fabs(SpeedByCamUD) > 0.01f) {
		sVECTOR3D tmp = GameCameraMovement^(SpeedByCamUD*TimeDelta);
		vw_RotatePoint(tmp, sVECTOR3D{90.0f, 0.0f, 0.0f});
		Velocity += tmp;
	}

	// перемещение объекта, если нужно
	if (Velocity.x != 0.0f || Velocity.y != 0.0f  || Velocity.z != 0.0f ) {
		// делаем сдвиг модели в указанную точку
		SetLocation(Location+Velocity);
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Делаем установку работы двигателей, согласуем со скоростью корабля
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!Engines.empty()) {
		float tmpSpeed = Speed;
		if (tmpSpeed > 6.0f)
			tmpSpeed=6.0f;
		else if (tmpSpeed < -6.0f)
			tmpSpeed=-6.0f;
		tmpSpeed/=2.0f;

		for (auto &tmpEngine : Engines) {
			if (auto sharedEngine = tmpEngine.lock())
				if (sharedEngine->SpeedOnCreation != -1.0f)
					sharedEngine->Speed = sharedEngine->SpeedOnCreation + tmpSpeed;
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работа с оружием - наведение его на ближайшую цель
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// а нужно ли стерять
	bool NeedFire = false;
	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (!tmpWeaponSlot.Weapon.expired() &&
			    tmpWeaponSlot.SetFire) {
				NeedFire = true;
				break;
			}
		}
	}
	bool NeedBossFire = false;
	if (!BossWeaponSlots.empty()) {
		for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
			if (!tmpBossWeaponSlot.Weapon.expired() &&
			    tmpBossWeaponSlot.SetFire) {
				NeedBossFire = true;
				break;
			}
		}
	}

	// если не игрок игрок и есть режим действия - нужно "искать" противника
	if ((ObjectStatus == eObjectStatus::Enemy) && NeedFire) {
		sVECTOR3D NeedAngle = Rotation;

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						WeaponAvLocation = WeaponAvLocation + tmpWeaponSlot.Location + sharedWeapon->FireLocation + Location;
						UsedWeaponQunt++;
					}
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;


		int WeapNum = -1;
		sVECTOR3D	FirePos(0.0f,0.0f,0.0f);
		if (!WeaponSlots.empty()) {
			// находим номер
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						WeapNum = sharedWeapon->InternalType;
						break;
					}
				}
			}
			if (WeapNum == -1)
				WeapNum = 204; // номер самого простого из пиратского оружия


			int Count = 0;
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						FirePos += tmpWeaponSlot.Location;
						Count++;
					}
				}
			}
			FirePos = FirePos^(1.0f/Count);
		}

		GetEnemyShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation,
						 CurrentRotationMat, NeedAngle, WeapNum);

		// всему оружию ставим нужную ориентацию
		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						NeedAngle.y = sharedWeapon->Rotation.y;
						NeedAngle.z = sharedWeapon->Rotation.z;

						sharedWeapon->SetRotation(sharedWeapon->Rotation^(-1));
						sharedWeapon->SetRotation(NeedAngle);
					}
				}
			}
		}
	}

	if ((ObjectStatus == eObjectStatus::Enemy) && NeedBossFire) {
		sVECTOR3D NeedAngle = Rotation;

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (!BossWeaponSlots.empty()) {
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						WeaponAvLocation = WeaponAvLocation + tmpBossWeaponSlot.Location + sharedWeapon->FireLocation + Location;
						UsedWeaponQunt++;
					}
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;

		int WeapNum{-1};
		sVECTOR3D FirePos(0.0f,0.0f,0.0f);
		if (!BossWeaponSlots.empty()) {
			// находим номер
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						WeapNum = sharedWeapon->InternalType;
						break;
					}
				}
			}
			if (WeapNum == -1)
				WeapNum = 204; // номер самого простого из пиратского оружия


			int Count = 0;
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						FirePos += tmpBossWeaponSlot.Location;
						Count++;
					}
				}
			}
			FirePos = FirePos^(1.0f/Count);
		}

		GetEnemyShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation,
						 CurrentRotationMat, NeedAngle, WeapNum);

		// всему оружию ставим нужную ориентацию
		if (!BossWeaponSlots.empty()) {
			for (auto &tmpBossWeaponSlot : BossWeaponSlots) {
				if (auto sharedWeapon = tmpBossWeaponSlot.Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						NeedAngle.y = sharedWeapon->Rotation.y;
						NeedAngle.z = sharedWeapon->Rotation.z;

						sharedWeapon->SetRotation(sharedWeapon->Rotation ^ -1);
						sharedWeapon->SetRotation(NeedAngle);
					}
				}
			}
		}
	}

	// так стреляют только свои :)
	if (ObjectStatus == eObjectStatus::Ally) {

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					WeaponAvLocation = WeaponAvLocation + tmpWeaponSlot.Location + sharedWeapon->FireLocation + Location;
					UsedWeaponQunt++;
				}
			}
		}
		WeaponAvLocation.x = WeaponAvLocation.x / UsedWeaponQunt;
		WeaponAvLocation.y = WeaponAvLocation.y / UsedWeaponQunt;
		WeaponAvLocation.z = WeaponAvLocation.z / UsedWeaponQunt;

		sVECTOR3D NeedAngle = Rotation;

		// всему оружию ставим нужную ориентацию
		if (!WeaponSlots.empty()) {
			// ставим скорость наведения оружия
			float TargetingSpeed = 1.0f;

			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						NeedAngle = Rotation;
						// добавляем базовый угол, чтобы по умолчанию устанавливало его
						NeedAngle.y += WeaponSlots[i].YAngle;

						GetShipOnTargetOrientateion(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, Rotation,
									    Length, CurrentRotationMat, NeedAngle, Width, true, true,
									    Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->InternalType);

						sVECTOR3D NeedAngleTmp = NeedAngle;

						// учитываем скорость поворота по вертикали
						if (sharedWeapon->Rotation.x < NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x+40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_x > NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;

						}
						if (sharedWeapon->Rotation.x > NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x-40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_x < NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;
						}

						// учитываем скорость поворота по горизонтали
						float Min = 0.0f;
						float Max = 0.0f;
						GetShipWeaponSlotAngle(GameConfig().Profile[CurrentProfile].Ship, i, &Min, &Max);
						if (sharedWeapon->Rotation.y < NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y+40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_y > NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							// проверка на достижение предела поворота
							if (NeedAngle.y > Max+Rotation.y)
								NeedAngle.y = Max+Rotation.y;
						}
						if (sharedWeapon->Rotation.y > NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y-40.0f*TargetingSpeed*TimeDelta;
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
						if (sharedWeapon->InternalType < 16) {
							sharedWeapon->SetRotation(sharedWeapon->Rotation^(-1));
							sharedWeapon->SetRotation(NeedAngle);
						}
					}
				}
			}
		}
	}

	// если корабль игрока
	if (ObjectStatus == eObjectStatus::Player) {

		// ставим скорость наведения оружия
		float TargetingSpeed = GameTargetingMechanicSystem*1.0f;

		// находим среднюю точку положение оружия
		sVECTOR3D WeaponAvLocation(0.0f,0.0f,0.0f);
		int UsedWeaponQunt = 0;
		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
					WeaponAvLocation = WeaponAvLocation + tmpWeaponSlot.Location + sharedWeapon->FireLocation + Location;
					UsedWeaponQunt++;
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

		// всему оружию ставим нужную ориентацию
		if (!WeaponSlots.empty()) {
			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					if (sharedWeapon->NeedRotateOnTargeting) {
						NeedAngle = Rotation;
						// добавляем базовый угол, чтобы по умолчанию устанавливало его
						NeedAngle.y += WeaponSlots[i].YAngle;

						switch (GameTargetingSystem) {
						case 1:
							GetShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation,
										    Length, CurrentRotationMat, NeedAngle, Width, false, true,
										    Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->InternalType);
							break;
						case 2:
							GetShipOnTargetOrientateion(ObjectStatus, WeaponAvLocation, Rotation,
										    Length, CurrentRotationMat, NeedAngle, Width, true, true,
										    Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->InternalType);
							break;
						case 3:
							GetShipOnTargetOrientateion(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->Rotation,
										    Length, sharedWeapon->CurrentRotationMat, NeedAngle, sharedWeapon->Width, false, true,
										    Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->InternalType);
							break;
						case 4:
							GetShipOnTargetOrientateion(ObjectStatus, Location + WeaponSlots[i].Location + sharedWeapon->FireLocation,
										    sVECTOR3D{sharedWeapon->Rotation.x, 0.0f, sharedWeapon->Rotation.z} + sVECTOR3D{0.0f, WeaponSlots[i].YAngle, 0.0f},
										    Length, sharedWeapon->CurrentRotationMat, NeedAngle, Width, false, true,
										    Location + WeaponSlots[i].Location + sharedWeapon->FireLocation, sharedWeapon->InternalType);
							break;
						}

						sVECTOR3D NeedAngleTmp = NeedAngle;

						// учитываем скорость поворота по вертикали
						if (sharedWeapon->Rotation.x < NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x+40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_x > NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;

						}
						if (sharedWeapon->Rotation.x > NeedAngle.x) {
							float NeedAngle_x = sharedWeapon->Rotation.x-40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_x < NeedAngle.x)
								NeedAngle_x = NeedAngle.x;
							NeedAngle.x = NeedAngle_x;
						}

						// учитываем скорость поворота по горизонтали
						float Min = 0.0f;
						float Max = 0.0f;
						GetShipWeaponSlotAngle(GameConfig().Profile[CurrentProfile].Ship, i, &Min, &Max);
						if (sharedWeapon->Rotation.y > NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y+40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_y > NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							// проверка на достижение предела поворота
							if (NeedAngle.y > Max+sharedWeapon->Rotation.y)
								NeedAngle.y = Max+sharedWeapon->Rotation.y;
						}
						if (sharedWeapon->Rotation.y < NeedAngle.y) {
							float NeedAngle_y = sharedWeapon->Rotation.y-40.0f*TargetingSpeed*TimeDelta;
							if (NeedAngle_y < NeedAngle.y)
								NeedAngle_y = NeedAngle.y;
							NeedAngle.y = NeedAngle_y;
							// проверка на достижение предела поворота
							if (NeedAngle.y < Min+sharedWeapon->Rotation.y)
								NeedAngle.y = Min+sharedWeapon->Rotation.y;
						}

						// если выключен прикол с поворотом - моментально поворачиваем ствол
						if (GameWeaponTargetingMode == 1)
							NeedAngle = NeedAngleTmp;

						sharedWeapon->SetRotation(sharedWeapon->Rotation^(-1));
						sharedWeapon->SetRotation(NeedAngle);
					}
				}
			}
		}
	}

	// объект в порядке - удалять не нужно
	return true;
}

} // astromenace namespace
} // viewizard namespace
