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

// TODO separate classes for buildings and vehicles (+ separate classes for ground explosions)

// TODO separate class for turrets (use it for second inherited class?)

// TODO translate comments

#ifndef OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H
#define OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H

#include "../object3d.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

class cWeapon;

enum class eGroundCycle {
	Continue,
	Break,
	DeleteObjectAndContinue,
	DeleteObjectAndBreak
};

class cGroundObject : public cObject3D {
	friend void UpdateAllGroundObject(float Time);
	friend void ReleaseAllGroundObject();
	friend void ForEachGroundObject(std::function<void (cGroundObject &Object, eGroundCycle &Command)> function);

protected:
	// don't allow object of this class creation
	cGroundObject();
	virtual ~cGroundObject();

public:
	// Обновление данных объектa
	virtual bool Update(float Time) override;
	// Установка положения объекта
	virtual void SetLocation(sVECTOR3D NewLocation) override;
	// Установка углов поворота объекта
	virtual void SetRotation(sVECTOR3D NewRotation) override;

	// колеса, для транспорта

	// скорость вращения колес
	float WheelTrackSpeed{0.0f};

	// кол-во колес
	int WheelQuantity{0};
	// перечень номеров объектов с колесами
	int *WheelObjectsNum{nullptr};

	// колеса поворотные, для установки угла поворота
	// кол-во колес
	int WheelRotateQuantity{0};
	// перечень номеров объектов с колесами
	int *WheelRotateObjectsNum{nullptr};
	// скорость вращения колес
	float WheelRotateSpeed{0.0f};
	// максимальный угол поворота колес для модели
	float MaxWheelRotateAngle{20.0f};

	// если нужно, номер объекта с траком для тайловой анимации
	int TrackObjectNum{-1};
	int TrackRotationDirection{1};// направление вращения траков

	// нужно повернуть (пересчитать мэш) при следующем проходе
	// Update + учесть эти данные в Rotation
	sVECTOR3D NeedRotate{0.0f, 0.0f, 0.0f};
	// скорость поворота по каждому раправлению
	sVECTOR3D RotationSpeed{1.0f, 1.0f, 1.0f};

	// максимальная скорость units/sec, зависит от двигателя
	float MaxSpeed{0.0f};
	// текущая скорость
	float Speed{0.0f};
	// нужная скорость
	float NeedSpeed{0.0f};

	// максимальное ускорение units/sec*sec, зависит от двигателя
	float MaxAcceler{0.0f};
	// макс. маневровых двигателей
	float MaxSpeedRotate{0.0f};
	// текущее ускорение
	float Acceler{0.0f};
	// нужное ускорение
	float NeedAcceler{1.0f};

	// кол-во оружия доступного на данной моделе
	int WeaponQuantity{0};
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool *WeaponSetFire{nullptr};
	// указатель на массив оружия
	cWeapon **Weapon{nullptr};
	// расположение оружия (относительное)
	sVECTOR3D *WeaponLocation{nullptr};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int WeaponFireType{2};
	int WeaponGroupCurrentFireNum{-1};
	float WeaponGroupCurrentFireDelay{0.0f};
	// наведение на цель
	bool WeaponTargeting{false};

	// для просчета положения точки стрельбы, считаем как кости
	sVECTOR3D BaseBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D MiddleBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D *WeaponBound{nullptr};
	// если точки фиксированы, не нужно менять направление точки выстрела
	// только учитываем общий поворот модели и соотв. точку выстрела
	// работает только если объекты поворота башни и ствола не заданы!
	bool DoNotCalculateRotation{false};

	// коэф. преобразования скорость в скорость вращения колес
	float SpeedToRotate{0.0f};

	// набор указателей на номера частей, которыми нацеливаемся по горизонтале
	int TargetHorizBlocksQuantity{0};
	int *TargetHorizBlocks{nullptr};
	float TargetHorizBlocksCurrentAngle{0.0f};
	float TargetHorizBlocksNeedAngle{0.0f};
	// набор указателей на номера частей, которыми нацеливаемся по вертикали
	int TargetVertBlocksQuantity{0};
	int *TargetVertBlocks{nullptr};
	float TargetVertBlocksMaxAngle{80.0f};
	float TargetVertBlocksMinAngle{0.0f};
	float TargetVertBlocksCurrentAngle{0.0f};
	float TargetVertBlocksNeedAngle{0.0f};
	// если нужно вращать ствол (многоствольный пулемет)
	int BarrelBlocksQuantity{0};
	int *BarrelBlocks{nullptr};

	// небольшая девиация-болтание колес
	bool DeviationOn{false};
	int DeviationObjQuantity{0};
	sVECTOR3D *Deviation{nullptr};
	float *CurentDeviation{nullptr};
	float *NeedDeviation{nullptr};
	float *CurentDeviationSum{nullptr};
	// ассоциированный объект (если нужно)
	int *DeviationObjNum{nullptr};

	// для собственного списка
	cGroundObject *Next{nullptr};
	cGroundObject *Prev{nullptr};
};

class cCivilianBuilding final : public cGroundObject {
public:
	explicit cCivilianBuilding(int BuildingNum);
};

class cMilitaryBuilding final : public cGroundObject {
public:
	explicit cMilitaryBuilding(int MilitaryBuildingNum);
};

class cTracked final : public cGroundObject {
public:
	explicit cTracked(int TrackedNum);
};

class cWheeled final : public cGroundObject {
public:
	explicit cWheeled(int WheeledNum);
};


// Проверяем все объекты, обновляем данные
void UpdateAllGroundObject(float Time);
// Прорисовываем все объекты
void DrawAllGroundObject(bool VertexOnlyPass, unsigned int ShadowMap);
// Удаляем все объекты в списке
void ReleaseAllGroundObject();
// Managed cycle for each ground object.
void ForEachGroundObject(std::function<void (cGroundObject &Object, eGroundCycle &Command)> function);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H
