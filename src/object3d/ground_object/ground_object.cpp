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

#include "ground_object.h"
#include "../../script/script.h"
#include "../../config/config.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// all ground object list
std::list<std::shared_ptr<cGroundObject>> GroundObjectList{};

} // unnamed namespace


/*
 * Create cCivilianBuilding object.
 */
std::weak_ptr<cGroundObject> CreateCivilianBuilding(int BuildingNum)
{
	GroundObjectList.emplace_front(std::shared_ptr<cGroundObject>{new cCivilianBuilding{BuildingNum},
								      [](cCivilianBuilding *p) {delete p;}});
	return GroundObjectList.front();
}

/*
 * Create cMilitaryBuilding object.
 */
std::weak_ptr<cGroundObject> CreateMilitaryBuilding(int MilitaryBuildingNum)
{
	GroundObjectList.emplace_front(std::shared_ptr<cGroundObject>{new cMilitaryBuilding{MilitaryBuildingNum},
								      [](cMilitaryBuilding *p) {delete p;}});
	return GroundObjectList.front();
}

/*
 * Create cTracked object.
 */
std::weak_ptr<cGroundObject> CreateTracked(int TrackedNum)
{
	GroundObjectList.emplace_front(std::shared_ptr<cGroundObject>{new cTracked{TrackedNum},
								      [](cTracked *p) {delete p;}});
	return GroundObjectList.front();
}

/*
 * Create cWheeled object.
 */
std::weak_ptr<cGroundObject> CreateWheeled(int WheeledNum)
{
	GroundObjectList.emplace_front(std::shared_ptr<cGroundObject>{new cWheeled{WheeledNum},
								      [](cWheeled *p) {delete p;}});
	return GroundObjectList.front();
}

/*
 * Update and remove (erase) dead objects.
 */
void UpdateAllGroundObjects(float Time)
{
	for (auto iter = GroundObjectList.begin(); iter != GroundObjectList.end();) {
		if (!iter->get()->Update(Time))
			iter = GroundObjectList.erase(iter);
		else
			++iter;
	}
}

/*
 * Draw all ground objects.
 */
void DrawAllGroundObjects(bool VertexOnlyPass, unsigned int ShadowMap)
{
	for (auto &tmpObject : GroundObjectList) {
		tmpObject.get()->Draw(VertexOnlyPass, ShadowMap);
	}
}

/*
 * Release particular ground object.
 */
void ReleaseGroundObject(std::weak_ptr<cGroundObject> &Object)
{
	auto sharedObject = Object.lock();
	if (!sharedObject)
		return;

	for (auto iter = GroundObjectList.begin(); iter != GroundObjectList.end();) {
		if (iter->get() == sharedObject.get()) {
			GroundObjectList.erase(iter);
			return;
		}
		++iter;
	}
}

/*
 * Release all ground objects.
 */
void ReleaseAllGroundObjects()
{
	GroundObjectList.clear();
}

/*
 * Managed cycle for each ground object.
 * Note, caller must guarantee, that 'Object' will not released in callback function call.
 */
void ForEachGroundObject(std::function<void (cGroundObject &Object, eGroundCycle &Command)> function)
{
	for (auto iter = GroundObjectList.begin(); iter != GroundObjectList.end();) {
		eGroundCycle Command{eGroundCycle::Continue};
		function(*iter->get(), Command);

		switch (Command) {
		case eGroundCycle::Continue:
			++iter;
			break;
		case eGroundCycle::Break:
			return;
		case eGroundCycle::DeleteObjectAndContinue:
			iter = GroundObjectList.erase(iter);
			break;
		case eGroundCycle::DeleteObjectAndBreak:
			GroundObjectList.erase(iter);
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cGroundObject::cGroundObject()
{
	ObjectStatus = eObjectStatus::Enemy;
}

//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cGroundObject::SetLocation(const sVECTOR3D &NewLocation)
{
	// вызываем родительскую функцию
	cObject3D::SetLocation(NewLocation);

	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
				sharedWeapon->SetLocation(NewLocation + tmpWeaponSlot.Location);
		}
	}
}

//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void cGroundObject::SetRotation(const sVECTOR3D &NewRotation)
{
	// вызываем родительскую функцию
	cObject3D::SetRotation(NewRotation);

	// оружие
	sVECTOR3D RotationBase = Rotation;
	sVECTOR3D BaseBoundTMP = BaseBound;
	vw_RotatePoint(BaseBoundTMP, RotationBase);

	sVECTOR3D RotationMiddle = Rotation;
	sVECTOR3D MiddleBoundTMP = MiddleBound;
	if (!TargetHorizChunkNums.empty())
		RotationMiddle = Chunks[TargetHorizChunkNums[0]].Rotation + Rotation;
	vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

	sVECTOR3D RotationWeapon = Rotation;
	if (!TargetVertChunkNums.empty())
		RotationWeapon = Chunks[TargetVertChunkNums[0]].Rotation + Rotation;

	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
				sVECTOR3D WeaponBoundTMP{tmpWeaponSlot.Bound};
				vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

				tmpWeaponSlot.Location = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

				// особый случай, испускаем без вращающихся частей (антиматерия, ион)
				if (TargetHorizChunkNums.empty() &&
				    TargetVertChunkNums.empty() &&
				    !DoNotCalculateRotation) // и если нужно считать...
					RotationWeapon = sVECTOR3D{TargetVertChunksNeedAngle,
								   TargetHorizChunksNeedAngle,
								   0.0f} +
							 Rotation;
				sharedWeapon->SetRotation(sharedWeapon->Rotation ^ (-1.0f));
				sharedWeapon->SetRotation(RotationWeapon);
				sharedWeapon->SetLocation(Location + tmpWeaponSlot.Location);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Обновление данных объектa Object3D
//-----------------------------------------------------------------------------
bool cGroundObject::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!cObject3D::Update(Time))
		return false;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f)
		return true;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// обработка скрипта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!TimeSheetList.empty() &&
	    !TimeSheetList.front().InUse) {
		TimeSheetList.front().InUse = true;

		NeedSpeed = TimeSheetList.front().Speed;
		NeedAcceler = TimeSheetList.front().Acceler;
		NeedRotate = TimeSheetList.front().Rotation;
		RotationSpeed = TimeSheetList.front().RotationAcceler;
		WeaponTargeting = TimeSheetList.front().Targeting;

		if (!WeaponSlots.empty()) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (!tmpWeaponSlot.Weapon.expired())
					tmpWeaponSlot.SetFire = TimeSheetList.front().Fire;
			}
		}
	}

	// если нужно наводить на цель
	if (WeaponTargeting) {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// наводим на цель, если есть возможность
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		// находимся в начальном состоянии поворота ствола
		int WeapNum{204}; // номер самого простого из пиратского оружия
		sVECTOR3D FirePos(0.0f, 0.0f, 0.0f);
		if (!WeaponSlots.empty()) {
			if (auto sharedWeapon = WeaponSlots[0].Weapon.lock())
				WeapNum = sharedWeapon->InternalType;

			int Count{0};
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (!tmpWeaponSlot.Weapon.expired()) {
					FirePos += tmpWeaponSlot.Location;
					Count++;
				}
			}
			FirePos = FirePos ^ (1.0f / Count);
		}
		sVECTOR3D NeedAngle(TargetVertChunksNeedAngle,TargetHorizChunksNeedAngle,0);
		if (GetTurretOnTargetOrientateion(ObjectStatus, Location + FirePos, Rotation,
						  CurrentRotationMat, NeedAngle, WeapNum)) {
			// наводим на цель
			TargetHorizChunksNeedAngle = NeedAngle.y;
			TargetVertChunksNeedAngle = NeedAngle.x;
		} else {
			// врагов нет, нужно просто поднять ствол
			TargetVertChunksNeedAngle = TargetVertChunksMaxAngle * 0.5f;
		}
	} else {
		// устанавливаем в начальное положение
		TargetHorizChunksNeedAngle = 0.0f;
		TargetVertChunksNeedAngle = 0.0f;

	}

	// собственно повороты элементов модели на углы
	{
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот башни по горизонтале
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (!TargetHorizChunkNums.empty() &&
		    (TargetHorizChunksNeedAngle != TargetHorizChunksCurrentAngle)) {
			if (fabsf(TargetHorizChunksNeedAngle-TargetHorizChunksCurrentAngle) > 180.0f) {
				if (TargetHorizChunksCurrentAngle - TargetHorizChunksNeedAngle > 180.0f)
					TargetHorizChunksCurrentAngle -= 360.0f;
				if (TargetHorizChunksNeedAngle - TargetHorizChunksCurrentAngle > 180.0f)
					TargetHorizChunksCurrentAngle += 360.0f;
			}

			// находим угол, на который нужно повернуть
			float NeedRotateCalculation = TargetHorizChunksCurrentAngle;

			if (TargetHorizChunksNeedAngle>TargetHorizChunksCurrentAngle) {
				NeedRotateCalculation += 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation > TargetHorizChunksNeedAngle)
					NeedRotateCalculation = TargetHorizChunksNeedAngle;
			} else {
				NeedRotateCalculation -= 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation < TargetHorizChunksNeedAngle)
					NeedRotateCalculation = TargetHorizChunksNeedAngle;
			}

			// устанавливаем текущий поворот
			TargetHorizChunksCurrentAngle = NeedRotateCalculation;

			// поворачиваем все объекты
			for (auto tmpChunkNum : TargetHorizChunkNums) {

				sVECTOR3D tmpLocation = Chunks[tmpChunkNum].Location -
							Chunks[TargetHorizChunkNums[0]].Location;

				vw_RotatePointInv(tmpLocation, Chunks[tmpChunkNum].Rotation ^ (-1.0f));

				SetChunkRotation(sVECTOR3D{Chunks[tmpChunkNum].Rotation.x,
							   -NeedRotateCalculation,
							   Chunks[tmpChunkNum].Rotation.z},
						  tmpChunkNum);

				vw_RotatePoint(tmpLocation, Chunks[tmpChunkNum].Rotation);

				SetChunkLocation(tmpLocation + Chunks[TargetHorizChunkNums[0]].Location, tmpChunkNum);
			}
		}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот стволов по вертикали
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (!TargetVertChunkNums.empty() &&
		    (TargetVertChunksNeedAngle != TargetVertChunksCurrentAngle)) {

			// находим угол, на который нужно повернуть
			float NeedRotateCalculation = TargetVertChunksCurrentAngle;
			if (TargetVertChunksNeedAngle>TargetVertChunksCurrentAngle) {
				NeedRotateCalculation += 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation > TargetVertChunksNeedAngle)
					NeedRotateCalculation = TargetVertChunksNeedAngle;
				if (NeedRotateCalculation > TargetVertChunksMaxAngle)
					NeedRotateCalculation = TargetVertChunksMaxAngle;
			} else {
				NeedRotateCalculation -= 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation < TargetVertChunksNeedAngle)
					NeedRotateCalculation = TargetVertChunksNeedAngle;
				if (NeedRotateCalculation < TargetVertChunksMinAngle)
					NeedRotateCalculation = TargetVertChunksMinAngle;
			}

			// устанавливаем текущий поворот
			TargetVertChunksCurrentAngle = NeedRotateCalculation;

			// поворачиваем все объекты
			for (auto tmpChunkNum : TargetVertChunkNums) {

				sVECTOR3D tmpLocation = Chunks[tmpChunkNum].Location -
							Chunks[TargetVertChunkNums[0]].Location;

				vw_RotatePointInv(tmpLocation, Chunks[tmpChunkNum].Rotation ^ (-1.0f));

				SetChunkRotation(sVECTOR3D{-NeedRotateCalculation,
							   Chunks[tmpChunkNum].Rotation.y,
							   Chunks[tmpChunkNum].Rotation.z},
						  tmpChunkNum);

				vw_RotatePoint(tmpLocation, Chunks[tmpChunkNum].Rotation);

				SetChunkLocation(tmpLocation + Chunks[TargetVertChunkNums[0]].Location, tmpChunkNum);
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// находим точку стрельбы и учитываем направление
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	sVECTOR3D RotationBase = Rotation;
	sVECTOR3D BaseBoundTMP = BaseBound;
	vw_RotatePoint(BaseBoundTMP, RotationBase);

	sVECTOR3D RotationMiddle = Rotation;
	sVECTOR3D MiddleBoundTMP = MiddleBound;
	if (!TargetHorizChunkNums.empty())
		RotationMiddle = Chunks[TargetHorizChunkNums[0]].Rotation + Rotation;
	vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

	sVECTOR3D RotationWeapon = Rotation;
	if (!TargetVertChunkNums.empty())
		RotationWeapon = Chunks[TargetVertChunkNums[0]].Rotation + Rotation;

	if (!WeaponSlots.empty()) {
		for (auto &tmpWeaponSlot : WeaponSlots) {
			if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
				sVECTOR3D WeaponBoundTMP = tmpWeaponSlot.Bound;
				vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

				tmpWeaponSlot.Location = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

				// особый случай, испускаем без вращающихся частей (антиматерия, ион)
				if (TargetHorizChunkNums.empty() &&
				    TargetVertChunkNums.empty() &&
				    !DoNotCalculateRotation) // и если нужно считать...
					RotationWeapon = Rotation - sVECTOR3D{TargetVertChunksNeedAngle,
									      TargetHorizChunksNeedAngle,
									      0.0f};

				sharedWeapon->SetRotation(sharedWeapon->Rotation ^ (-1.0f));
				sharedWeapon->SetRotation(RotationWeapon);
				sharedWeapon->SetLocation(Location + tmpWeaponSlot.Location);
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим, есть ли команда открыть огонь
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!WeaponSlots.empty()) {
		// если залп или игрок (игроку регулируем сами)
		// FIXME switch from if-else-if to case (?)
		if (WeaponFireType == 1) {
			for (auto &tmpWeaponSlot : WeaponSlots) {
				if (tmpWeaponSlot.SetFire)
					if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock())
						sharedWeapon->WeaponFire(Time);
			}
		} else if (WeaponFireType == 2) { // переменный огонь
			int PrimCount{0};
			float PrimTime{0.0f};
			unsigned FirstWeapon{config::MAX_WEAPONS};
			unsigned LastWeapon{0};

			WeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					PrimCount++;
					PrimTime += sharedWeapon->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			}
			// если еще не было начальной установки
			if (WeaponGroupCurrentFireNum == -1)
				WeaponGroupCurrentFireNum = FirstWeapon;

			// стреляем
			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (!WeaponSlots[i].Weapon.expired() &&
				    WeaponSlots[i].SetFire) {
					if ((WeaponGroupCurrentFireNum == static_cast<int>(i)) &&
					    (WeaponGroupCurrentFireDelay <= 0.0f)) {
						if (auto sharedWeapon = WeaponSlots[i].Weapon.lock())
							sharedWeapon->WeaponFire(Time);

						WeaponGroupCurrentFireDelay = (PrimTime/PrimCount)*((1.0f+GameEnemyWeaponPenalty)/2.0f);
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
		} else if (WeaponFireType == 3) { // переменный огонь2 (залп ракет или чего-то еще)
			int PrimCount{0};
			float PrimTime{0.0f};
			unsigned FirstWeapon{config::MAX_WEAPONS};
			unsigned LastWeapon{0};

			WeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (auto sharedWeapon = WeaponSlots[i].Weapon.lock()) {
					PrimCount++;
					PrimTime += sharedWeapon->NextFireTime;
					if (FirstWeapon > i)
						FirstWeapon = i;
					if (LastWeapon < i)
						LastWeapon = i;
				}
			}
			// если еще не было начальной установки
			if (WeaponGroupCurrentFireNum == -1)
				WeaponGroupCurrentFireNum = FirstWeapon;

			// стреляем
			for (unsigned i = 0; i < WeaponSlots.size(); i++) {
				if (!WeaponSlots[i].Weapon.expired() &&
				    WeaponSlots[i].SetFire) {
					if ((WeaponGroupCurrentFireNum == static_cast<int>(i)) &&
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
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вращение стволов пулемета
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!BarrelChunkNums.empty()) {
		for (auto tmpChunkNum : BarrelChunkNums) {
			Chunks[tmpChunkNum].NeedGeometryAnimation = true;
			Chunks[tmpChunkNum].GeometryAnimation.z += 500.0f * TimeDelta;
			if (Chunks[tmpChunkNum].GeometryAnimation.z > 360.0f)
				Chunks[tmpChunkNum].GeometryAnimation.z -= 360.0f;
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вращение колес в транспорте
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!WheelChunkNums.empty() &&
	    ((WheelRotarySpeed >= 0.00001f) || (WheelRotarySpeed <= -0.00001f))) {
		// перебираем все и ув. их угол вращения
		for (auto tmpChunkNum : WheelChunkNums) {
			Chunks[tmpChunkNum].Rotation.x += WheelRotarySpeed * TimeDelta;

			if (Chunks[tmpChunkNum].Rotation.x > 360.0f)
				Chunks[tmpChunkNum].Rotation.x -= 360.0f;
			if (Chunks[tmpChunkNum].Rotation.x < -360.0f)
				Chunks[tmpChunkNum].Rotation.x += 360.0f;
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// тайловая анимация для траков
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((TrackChunkNum != -1) && ((WheelRotarySpeed >= 0.00001f) || (WheelRotarySpeed <= -0.00001f))) {
		Chunks[TrackChunkNum].NeedTextureAnimation = true;
		Chunks[TrackChunkNum].TextureAnimation.x +=
				(WheelRotarySpeed / 500.0f) * TimeDelta * TrackRotationDirection;
		if (Chunks[TrackChunkNum].TextureAnimation.x > 1.0f)
			Chunks[TrackChunkNum].TextureAnimation.x -= 1.0f;
		if (Chunks[TrackChunkNum].TextureAnimation.x < -1.0f)
			Chunks[TrackChunkNum].TextureAnimation.x += 1.0f;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// поворот
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((NeedRotate.x != 0.0f) || (NeedRotate.y != 0.0f) || (NeedRotate.z != 0.0f)) {
		// Находим допустимый поворот по углу
		sVECTOR3D tmpRotate(0.0f, 0.0f, 0.0f);

		// угол по x
		if (NeedRotate.x != 0.0f) {
			float Sign{1.0f};
			if (NeedRotate.x < 0.0f)
				Sign = -1.0f;
			// вычисляем скорость поворота по данным двигателя
			tmpRotate.x = Sign * MaxSpeedRotate * RotationSpeed.x * TimeDelta;
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
			float Sign{1.0f};
			if (NeedRotate.y < 0.0f)
				Sign = -1.0f;
			// вычисляем скорость поворота по данным двигателя
			tmpRotate.y = Sign * MaxSpeedRotate * RotationSpeed.y * TimeDelta;
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
		}

		// угол по z
		if (NeedRotate.z != 0.0f) {
			float Sign{1.0f};
			if (NeedRotate.z < 0.0f)
				Sign = -1.0f;
			// вычисляем скорость поворота по данным двигателя
			tmpRotate.z = Sign * MaxSpeedRotate * RotationSpeed.z * TimeDelta;
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

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот колес в транспорте
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (!SteerableWheelChunkNums.empty()) {
			for (auto tmpChunkNum : SteerableWheelChunkNums) {
				float tmpRotateY = Chunks[tmpChunkNum].Rotation.y;

				if (Chunks[tmpChunkNum].Rotation.y > NeedRotate.y) {
					tmpRotateY -= 90.0f * TimeDelta;
					if (tmpRotateY < NeedRotate.y)
						tmpRotateY = NeedRotate.y;
				} else if (Chunks[tmpChunkNum].Rotation.y < NeedRotate.y) {
					tmpRotateY += 90.0f * TimeDelta;
					if (tmpRotateY > NeedRotate.y)
						tmpRotateY = NeedRotate.y;
				}

				if (tmpRotateY > MaxSteerableWheelAngle)
					tmpRotateY = MaxSteerableWheelAngle;
				if (tmpRotateY < -MaxSteerableWheelAngle)
					tmpRotateY = -MaxSteerableWheelAngle;

				SetChunkRotation(sVECTOR3D{Chunks[tmpChunkNum].Rotation.x,
							   tmpRotateY,
							   Chunks[tmpChunkNum].Rotation.z},
						  tmpChunkNum);
			}
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ускорение-замедление
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// если нужно разогнаться, или управление на игроке - и нужно стремиться к нулю
	if (NeedSpeed != 0.0f) {
		float Sign{1.0f};
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
		Acceler = Sign * MaxAcceler * NeedAcceler;

		// считаем текущую скорость
		Speed = Speed + Acceler * TimeDelta;

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
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// небольшая девиация-болтание колес, если нужно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!WheelShake.empty()) {
		for (auto &tmpWheelShake : WheelShake) {
			tmpWheelShake.Update(TimeDelta, [&] (const float ShakeIncrement,
							     const sVECTOR3D &ShakeDirection,
							     const unsigned ShakeChunkNum) {
				sVECTOR3D tmpLocation = ShakeDirection ^ ShakeIncrement;
				SetChunkLocation(Chunks[ShakeChunkNum].Location + tmpLocation, ShakeChunkNum);
			});
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// считаем вектор движения
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	sVECTOR3D tmpShift = (Orientation ^ (Speed * TimeDelta));

	// перемещение объекта, если нужно
	if ((tmpShift.x != 0.0f) || (tmpShift.y != 0.0f) || (tmpShift.z != 0.0f)) {
		WheelRotarySpeed = Speed * SpeedToWheelRotarySpeedFactor;
		// делаем сдвиг модели в указанную точку
		SetLocation(Location + tmpShift);
	}

	// объект в порядке - удалять не нужно
	return true;
}

} // astromenace namespace
} // viewizard namespace
