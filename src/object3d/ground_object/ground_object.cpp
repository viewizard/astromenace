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
#include "../weapon/weapon.h"

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
// Деструктор
//-----------------------------------------------------------------------------
cGroundObject::~cGroundObject()
{
	if (!Weapon.empty()) {
		for (auto &tmpWeapon : Weapon) {
			if (tmpWeapon.Weapon != nullptr)
				ReleaseWeapon(tmpWeapon.Weapon);
		}
		Weapon.clear();
	}
	if (WheelRotateObjectsNum != nullptr) {
		delete [] WheelRotateObjectsNum;
		WheelRotateObjectsNum = nullptr;
	};

	if (TargetHorizBlocks != nullptr) {
		delete [] TargetHorizBlocks;
		TargetHorizBlocks = nullptr;
	};
	if (TargetVertBlocks != nullptr) {
		delete [] TargetVertBlocks;
		TargetVertBlocks = nullptr;
	};

	if (BarrelBlocks != nullptr) {
		delete [] BarrelBlocks;
		BarrelBlocks = nullptr;
	};

	if (DeviationObjQuantity != 0) {
		if (Deviation != nullptr)
			delete [] Deviation;
		if (NeedDeviation != nullptr)
			delete [] NeedDeviation;
		if (CurentDeviationSum != nullptr)
			delete [] CurentDeviationSum;
		if (DeviationObjNum != nullptr)
			delete [] DeviationObjNum;
	}
}

//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cGroundObject::SetLocation(sVECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	cObject3D::SetLocation(NewLocation);

	if (!Weapon.empty()) {
		for (auto &tmpWeapon : Weapon) {
			if (tmpWeapon.Weapon != nullptr)
				tmpWeapon.Weapon->SetLocation(NewLocation + tmpWeapon.Location);
		}
	}
}

//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void cGroundObject::SetRotation(sVECTOR3D NewRotation)
{
	// вызываем родительскую функцию
	cObject3D::SetRotation(NewRotation);

	// оружие
	sVECTOR3D RotationBase = Rotation;
	sVECTOR3D BaseBoundTMP = BaseBound;
	vw_RotatePoint(BaseBoundTMP, RotationBase);

	sVECTOR3D RotationMiddle = Rotation;
	sVECTOR3D MiddleBoundTMP = MiddleBound;
	if (TargetHorizBlocks != nullptr)
		RotationMiddle = Model3DBlocks[TargetHorizBlocks[0]].Rotation + Rotation;
	vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

	sVECTOR3D RotationWeapon = Rotation;
	if (TargetVertBlocks != nullptr)
		RotationWeapon = Model3DBlocks[TargetVertBlocks[0]].Rotation + Rotation;

	if (!Weapon.empty()) {
		for (auto &tmpWeapon : Weapon) {
			if (tmpWeapon.Weapon != nullptr) {
				sVECTOR3D WeaponBoundTMP{tmpWeapon.Bound};
				vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

				tmpWeapon.Location = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

				// особый случай, испускаем без вращающихся частей (антиматерия, ион)
				if ((TargetHorizBlocks == nullptr) &&
				    (TargetVertBlocks == nullptr) &&
				    !DoNotCalculateRotation) // и если нужно считать...
					RotationWeapon = sVECTOR3D(TargetVertBlocksNeedAngle, TargetHorizBlocksNeedAngle, 0.0f) + Rotation;
				tmpWeapon.Weapon->SetRotation(tmpWeapon.Weapon->Rotation ^ (-1.0f));
				tmpWeapon.Weapon->SetRotation(RotationWeapon);
				tmpWeapon.Weapon->SetLocation(Location + tmpWeapon.Location);
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

		if (!Weapon.empty()) {
			for (auto &tmpWeapon : Weapon) {
				if (tmpWeapon.Weapon != nullptr)
					tmpWeapon.SetFire = TimeSheetList.front().Fire;
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
		if (!Weapon.empty()) {
			if (Weapon[0].Weapon != nullptr)
				WeapNum = Weapon[0].Weapon->InternalType;

			int Count{0};
			for (auto &tmpWeapon : Weapon) {
				if (tmpWeapon.Weapon != nullptr) {
					FirePos += tmpWeapon.Location;
					Count++;
				}
			}
			FirePos = FirePos ^ (1.0f / Count);
		}
		sVECTOR3D NeedAngle(TargetVertBlocksNeedAngle,TargetHorizBlocksNeedAngle,0);
		if (GetTurretOnTargetOrientateion(ObjectStatus, Location + FirePos, Rotation,
						  CurrentRotationMat, &NeedAngle, WeapNum)) {
			// наводим на цель
			TargetHorizBlocksNeedAngle = NeedAngle.y;
			TargetVertBlocksNeedAngle = NeedAngle.x;
		} else {
			// врагов нет, нужно просто поднять ствол
			TargetVertBlocksNeedAngle = TargetVertBlocksMaxAngle * 0.5f;
		}
	} else {
		// устанавливаем в начальное положение
		TargetHorizBlocksNeedAngle = 0.0f;
		TargetVertBlocksNeedAngle = 0.0f;

	}

	// собственно повороты элементов модели на углы
	{
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот башни по горизонтале
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if ((TargetHorizBlocks != nullptr) &&
		    (TargetHorizBlocksNeedAngle != TargetHorizBlocksCurrentAngle)) {
			if (fabsf(TargetHorizBlocksNeedAngle-TargetHorizBlocksCurrentAngle) > 180.0f) {
				if (TargetHorizBlocksCurrentAngle - TargetHorizBlocksNeedAngle > 180.0f)
					TargetHorizBlocksCurrentAngle -= 360.0f;
				if (TargetHorizBlocksNeedAngle - TargetHorizBlocksCurrentAngle > 180.0f)
					TargetHorizBlocksCurrentAngle += 360.0f;
			}

			// находим угол, на который нужно повернуть
			float NeedRotateCalculation = TargetHorizBlocksCurrentAngle;

			if (TargetHorizBlocksNeedAngle>TargetHorizBlocksCurrentAngle) {
				NeedRotateCalculation += 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation > TargetHorizBlocksNeedAngle)
					NeedRotateCalculation = TargetHorizBlocksNeedAngle;
			} else {
				NeedRotateCalculation -= 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation < TargetHorizBlocksNeedAngle)
					NeedRotateCalculation = TargetHorizBlocksNeedAngle;
			}

			// устанавливаем текущий поворот
			TargetHorizBlocksCurrentAngle = NeedRotateCalculation;

			// поворачиваем все объекты
			for (int i = 0; i < TargetHorizBlocksQuantity; i++) {

				sVECTOR3D tmp = Model3DBlocks[TargetHorizBlocks[i]].Location -
						Model3DBlocks[TargetHorizBlocks[0]].Location;

				vw_RotatePointInv(tmp, Model3DBlocks[TargetHorizBlocks[i]].Rotation ^ (-1.0f));

				SetObjectRotation(sVECTOR3D(Model3DBlocks[TargetHorizBlocks[i]].Rotation.x,
							    -NeedRotateCalculation,
							    Model3DBlocks[TargetHorizBlocks[i]].Rotation.z), TargetHorizBlocks[i]);

				vw_RotatePoint(tmp, Model3DBlocks[TargetHorizBlocks[i]].Rotation);

				SetObjectLocation(tmp + Model3DBlocks[TargetHorizBlocks[0]].Location, TargetHorizBlocks[i]);
			}
		}

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// поворот стволов по вертикали
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if ((TargetVertBlocks != nullptr) &&
		    (TargetVertBlocksNeedAngle != TargetVertBlocksCurrentAngle)) {

			// находим угол, на который нужно повернуть
			float NeedRotateCalculation = TargetVertBlocksCurrentAngle;
			if (TargetVertBlocksNeedAngle>TargetVertBlocksCurrentAngle) {
				NeedRotateCalculation += 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation > TargetVertBlocksNeedAngle)
					NeedRotateCalculation = TargetVertBlocksNeedAngle;
				if (NeedRotateCalculation > TargetVertBlocksMaxAngle)
					NeedRotateCalculation = TargetVertBlocksMaxAngle;
			} else {
				NeedRotateCalculation -= 80.0f * TimeDelta/GameEnemyTargetingSpeedPenalty;
				if (NeedRotateCalculation < TargetVertBlocksNeedAngle)
					NeedRotateCalculation = TargetVertBlocksNeedAngle;
				if (NeedRotateCalculation < TargetVertBlocksMinAngle)
					NeedRotateCalculation = TargetVertBlocksMinAngle;
			}

			// устанавливаем текущий поворот
			TargetVertBlocksCurrentAngle = NeedRotateCalculation;

			// поворачиваем все объекты
			for (int i = 0; i < TargetVertBlocksQuantity; i++) {

				sVECTOR3D tmp = Model3DBlocks[TargetVertBlocks[i]].Location -
						Model3DBlocks[TargetVertBlocks[0]].Location;

				vw_RotatePointInv(tmp, Model3DBlocks[TargetVertBlocks[i]].Rotation ^ (-1.0f));

				SetObjectRotation(sVECTOR3D(-NeedRotateCalculation,
							    Model3DBlocks[TargetVertBlocks[i]].Rotation.y,
							    Model3DBlocks[TargetVertBlocks[i]].Rotation.z), TargetVertBlocks[i]);

				vw_RotatePoint(tmp, Model3DBlocks[TargetVertBlocks[i]].Rotation);

				SetObjectLocation(tmp + Model3DBlocks[TargetVertBlocks[0]].Location, TargetVertBlocks[i]);
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
	if (TargetHorizBlocks != nullptr)
		RotationMiddle = Model3DBlocks[TargetHorizBlocks[0]].Rotation + Rotation;
	vw_RotatePoint(MiddleBoundTMP, RotationMiddle);

	sVECTOR3D RotationWeapon = Rotation;
	if (TargetVertBlocks != nullptr)
		RotationWeapon = Model3DBlocks[TargetVertBlocks[0]].Rotation + Rotation;

	if (!Weapon.empty()) {
		for (auto &tmpWeapon : Weapon) {
			if (tmpWeapon.Weapon != nullptr) {
				sVECTOR3D WeaponBoundTMP = tmpWeapon.Bound;
				vw_RotatePoint(WeaponBoundTMP, RotationWeapon);

				tmpWeapon.Location = BaseBoundTMP + MiddleBoundTMP + WeaponBoundTMP;

				// особый случай, испускаем без вращающихся частей (антиматерия, ион)
				if ((TargetHorizBlocks == nullptr) &&
				    (TargetVertBlocks == nullptr) &&
				    !DoNotCalculateRotation) // и если нужно считать...
					RotationWeapon = Rotation - sVECTOR3D(TargetVertBlocksNeedAngle, TargetHorizBlocksNeedAngle, 0.0f);

				tmpWeapon.Weapon->SetRotation(tmpWeapon.Weapon->Rotation ^ (-1.0f));
				tmpWeapon.Weapon->SetRotation(RotationWeapon);
				tmpWeapon.Weapon->SetLocation(Location + tmpWeapon.Location);
			}
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим, есть ли команда открыть огонь
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!Weapon.empty()) {
		// если залп или игрок (игроку регулируем сами)
		// FIXME switch from if-else-if to case (?)
		if (WeaponFireType == 1) {
			for (auto &tmpWeapon : Weapon) {
				if ((tmpWeapon.Weapon != nullptr) &&
				    tmpWeapon.SetFire)
					tmpWeapon.Weapon->WeaponFire(Time);
			}
		} else if (WeaponFireType == 2) { // переменный огонь
			int PrimCount{0};
			float PrimTime{0.0f};
			unsigned FirstWeapon{config::MAX_WEAPONS};
			unsigned LastWeapon{0};

			WeaponGroupCurrentFireDelay -= TimeDelta;

			// находим кол-во оружия
			for (unsigned i = 0; i < Weapon.size(); i++) {
				if (Weapon[i].Weapon != nullptr) {
					PrimCount++;
					PrimTime += Weapon[i].Weapon->NextFireTime;
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
			for (unsigned i = 0; i < Weapon.size(); i++) {
				if ((Weapon[i].Weapon != nullptr) &&
				    Weapon[i].SetFire) {
					if ((WeaponGroupCurrentFireNum == static_cast<int>(i)) &&
					    (WeaponGroupCurrentFireDelay <= 0.0f)) {
						Weapon[i].Weapon->WeaponFire(Time);

						WeaponGroupCurrentFireDelay = (PrimTime/PrimCount)*((1.0f+GameEnemyWeaponPenalty)/2.0f);
						WeaponGroupCurrentFireNum++;
						if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
							WeaponGroupCurrentFireNum = FirstWeapon;

						// если такого оружия нет, берем что есть
						if (Weapon[WeaponGroupCurrentFireNum].Weapon == nullptr) {
							bool exit = false;
							while (!exit) {
								WeaponGroupCurrentFireNum++;
								if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
									WeaponGroupCurrentFireNum = FirstWeapon;
								if (Weapon[WeaponGroupCurrentFireNum].Weapon != nullptr)
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
			for (unsigned i = 0; i < Weapon.size(); i++) {
				if (Weapon[i].Weapon != nullptr) {
					PrimCount++;
					PrimTime += Weapon[i].Weapon->NextFireTime;
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
			for (unsigned i = 0; i < Weapon.size(); i++) {
				if ((Weapon[i].Weapon != nullptr) &&
				    Weapon[i].SetFire) {
					if ((WeaponGroupCurrentFireNum == static_cast<int>(i)) &&
					    (WeaponGroupCurrentFireDelay <= 0.0f)) {
						Weapon[i].Weapon->WeaponFire(Time);

						WeaponGroupCurrentFireDelay = PrimTime/(PrimCount*PrimCount);
						WeaponGroupCurrentFireNum++;
						if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
							WeaponGroupCurrentFireNum = FirstWeapon;

						// если такого оружия нет, берем что есть
						if (Weapon[WeaponGroupCurrentFireNum].Weapon == nullptr) {
							bool exit = false;
							while (!exit) {
								WeaponGroupCurrentFireNum++;
								if (WeaponGroupCurrentFireNum > static_cast<int>(LastWeapon))
									WeaponGroupCurrentFireNum = FirstWeapon;
								if (Weapon[WeaponGroupCurrentFireNum].Weapon != nullptr)
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
	if ((BarrelBlocksQuantity != 0) &&
	    (BarrelBlocks != nullptr)) {
		for (int i = 0; i < BarrelBlocksQuantity; i++) {
			Model3DBlocks[BarrelBlocks[i]].NeedGeometryAnimation = true;
			Model3DBlocks[BarrelBlocks[i]].GeometryAnimation += sVECTOR3D(0.0f, 0.0f, 500.0f * TimeDelta);
			if (Model3DBlocks[BarrelBlocks[i]].GeometryAnimation.z > 360.0f)
				Model3DBlocks[BarrelBlocks[i]].GeometryAnimation.z -= 360.0f;
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вращение колес в транспорте
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((!WheelObjects.empty() &&
	    ((WheelTrackSpeed >= 0.00001f) || (WheelTrackSpeed <= -0.00001f)))) {
		// перебираем все и ув. их угол вращения
		for (auto tmpObjects : WheelObjects) {
			Model3DBlocks[tmpObjects].Rotation.x += WheelTrackSpeed * TimeDelta;

			if (Model3DBlocks[tmpObjects].Rotation.x > 360.0f)
				Model3DBlocks[tmpObjects].Rotation.x -= 360.0f;
			if (Model3DBlocks[tmpObjects].Rotation.x < -360.0f)
				Model3DBlocks[tmpObjects].Rotation.x += 360.0f;
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// тайловая анимация для траков
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((TrackObjectNum != -1) && ((WheelTrackSpeed >= 0.00001f) || (WheelTrackSpeed <= -0.00001f))) {
		Model3DBlocks[TrackObjectNum].NeedTextureAnimation = true;
		Model3DBlocks[TrackObjectNum].TextureAnimation.x +=
				(WheelTrackSpeed / 500.0f) * TimeDelta * TrackRotationDirection;
		if (Model3DBlocks[TrackObjectNum].TextureAnimation.x > 1.0f)
			Model3DBlocks[TrackObjectNum].TextureAnimation.x -= 1.0f;
		if (Model3DBlocks[TrackObjectNum].TextureAnimation.x < -1.0f)
			Model3DBlocks[TrackObjectNum].TextureAnimation.x += 1.0f;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// повотор
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
		if (WheelRotateObjectsNum != nullptr) {
			// перебираем все и ув. их угол вращения
			for (int i = 0; i < WheelRotateQuantity; i++) {
				float NeedRotateY = Model3DBlocks[WheelRotateObjectsNum[i]].Rotation.y;
				if (Model3DBlocks[WheelRotateObjectsNum[i]].Rotation.y > NeedRotate.y) {
					NeedRotateY -= 90.0f * TimeDelta;
					if (NeedRotateY < NeedRotate.y)
						NeedRotateY = NeedRotate.y;
				} else if (Model3DBlocks[WheelRotateObjectsNum[i]].Rotation.y < NeedRotate.y) {
					NeedRotateY += 90.0f * TimeDelta;
					if (NeedRotateY > NeedRotate.y)
						NeedRotateY = NeedRotate.y;
				}

				if (NeedRotateY>MaxWheelRotateAngle)
					NeedRotateY = MaxWheelRotateAngle;
				if (NeedRotateY<-MaxWheelRotateAngle)
					NeedRotateY = -MaxWheelRotateAngle;

				SetObjectRotation(sVECTOR3D(Model3DBlocks[WheelRotateObjectsNum[i]].Rotation.x,
							    NeedRotateY,
							    Model3DBlocks[WheelRotateObjectsNum[i]].Rotation.z),
						  WheelRotateObjectsNum[i]);
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
	if (DeviationOn) {
		for (unsigned i = 0; i < DeviationObjQuantity; i++) {
			float Sign{1.0f};
			// нужно двигать
			if (NeedDeviation[i] < 0.0f)
				Sign = -1.0f;
			if (Sign == 1.0f) {
				if (NeedDeviation[i] < CurentDeviationSum[i])
					Sign = -1.0f;
			} else {
				if (NeedDeviation[i] > CurentDeviationSum[i])
					Sign = 1.0f;
			}

			float tmpDeviation = Sign * 0.35f * TimeDelta;

			if (Sign == 1.0f) {
				if (NeedDeviation[i] <= CurentDeviationSum[i] + tmpDeviation) {
					tmpDeviation -= CurentDeviationSum[i] + tmpDeviation - NeedDeviation[i];
					CurentDeviationSum[i] += tmpDeviation;
					NeedDeviation[i] = vw_fRand0() * 0.1f;
				} else CurentDeviationSum[i] += tmpDeviation;
			} else {
				if (NeedDeviation[i] >= CurentDeviationSum[i] + tmpDeviation) {
					tmpDeviation += CurentDeviationSum[i] + tmpDeviation - NeedDeviation[i];
					CurentDeviationSum[i] += tmpDeviation;
					NeedDeviation[i] = vw_fRand0() * 0.1f;
				} else CurentDeviationSum[i] += tmpDeviation;
			}

			sVECTOR3D Tmp = Deviation[i] ^ tmpDeviation;
			SetObjectLocation(Model3DBlocks[DeviationObjNum[i]].Location + Tmp, DeviationObjNum[i]);
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// считаем вектор движения
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	sVECTOR3D Velocity = (Orientation ^ (Speed * TimeDelta));

	// перемещение объекта, если нужно
	if ((Velocity.x != 0.0f) || (Velocity.y != 0.0f) || (Velocity.z != 0.0f)) {
		WheelTrackSpeed = Speed * SpeedToRotate;
		// делаем сдвиг модели в указанную точку
		SetLocation(Location + Velocity);
	}

	// объект в порядке - удалять не нужно
	return true;
}

} // astromenace namespace
} // viewizard namespace
