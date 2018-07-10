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

// TODO split code for classes (? also separate classes for ground explosions)
//      inheritance: cGroundObject -> cCivilianBuilding
//                   cGroundObject + turret code -> cMilitaryBuilding
//                   cMilitaryBuilding + wheels code -> cWheeled
//                   cWheeled + tracks code -> cTracked

// TODO WeaponFireType should be enumeration

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

struct sGroundWeaponSlot {
	bool SetFire{false};
	cWeapon *Weapon{nullptr};
	sVECTOR3D Location{};
	sVECTOR3D Bound{};

	sGroundWeaponSlot() = default;
	sGroundWeaponSlot(cWeapon *_Weapon, const sVECTOR3D &_Location) :
		Weapon{_Weapon},
		Location{_Location}
	{}

	void operator () (cWeapon *_Weapon, const sVECTOR3D &_Location)
	{
		Weapon = _Weapon;
		Location = _Location;
	}
};

class cGroundObject : public cObject3D {
protected:
	// don't allow object of this class creation
	cGroundObject();
	virtual ~cGroundObject();

public:
	// Обновление данных объектa
	virtual bool Update(float Time) override;
	// Установка положения объекта
	virtual void SetLocation(const sVECTOR3D &NewLocation) override;
	// Установка углов поворота объекта
	virtual void SetRotation(const sVECTOR3D &NewRotation) override;

	// rotary speed (deg/sec) for all wheels (for both, wheeled and tracked)
	float WheelRotarySpeed{0.0f};
	float SpeedToWheelRotarySpeedFactor{0.0f};

	// wheel chunks (for both, wheeled and tracked)
	std::vector<unsigned> WheelChunkNums{};
	// small wheel deviation (for both, wheeled and tracked)
	std::vector<sDeviation> WheelDeviation{};
	// steerable wheel chunks (for wheeled only)
	std::vector<unsigned> SteerableWheelChunkNums{};
	float MaxSteerableWheelAngle{20.0f};

	// если нужно, номер объекта с траком для тайловой анимации
	int TrackChunkNum{-1};
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

	std::vector<sGroundWeaponSlot> WeaponSlots{};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int WeaponFireType{2};
	int WeaponGroupCurrentFireNum{-1};
	float WeaponGroupCurrentFireDelay{0.0f};
	// наведение на цель
	bool WeaponTargeting{false};

	// для просчета положения точки стрельбы, считаем как кости
	sVECTOR3D BaseBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D MiddleBound{0.0f, 0.0f, 0.0f};
	// если точки фиксированы, не нужно менять направление точки выстрела
	// только учитываем общий поворот модели и соотв. точку выстрела
	// работает только если объекты поворота башни и ствола не заданы!
	bool DoNotCalculateRotation{false};

	// horizontal targeting related (turret can rotate at 360 deg)
	std::vector<unsigned> TargetHorizChunkNums{};
	float TargetHorizChunksCurrentAngle{0.0f};
	float TargetHorizChunksNeedAngle{0.0f};
	// vertical targeting related
	std::vector<unsigned> TargetVertChunkNums{};
	float TargetVertChunksMaxAngle{80.0f};
	float TargetVertChunksMinAngle{0.0f};
	float TargetVertChunksCurrentAngle{0.0f};
	float TargetVertChunksNeedAngle{0.0f};
	// barrel chunks (we may need rotate barrels)
	std::vector<unsigned> BarrelChunkNums{};
};

class cCivilianBuilding final : public cGroundObject {
	friend std::weak_ptr<cGroundObject> CreateCivilianBuilding(int BuildingNum);
private:
	// Don't allow direct new/delete usage in code, only CreateCivilianBuilding()
	// allowed for cCivilianBuilding creation and release setup (deleter must be provided).
	explicit cCivilianBuilding(int BuildingNum);
};

class cMilitaryBuilding final : public cGroundObject {
	friend std::weak_ptr<cGroundObject> CreateMilitaryBuilding(int MilitaryBuildingNum);
private:
	// Don't allow direct new/delete usage in code, only CreateMilitaryBuilding()
	// allowed for cMilitaryBuilding creation and release setup (deleter must be provided).
	explicit cMilitaryBuilding(int MilitaryBuildingNum);
};

class cTracked final : public cGroundObject {
	friend std::weak_ptr<cGroundObject> CreateTracked(int TrackedNum);
private:
	// Don't allow direct new/delete usage in code, only CreateTracked()
	// allowed for cTracked creation and release setup (deleter must be provided).
	explicit cTracked(int TrackedNum);
};

class cWheeled final : public cGroundObject {
	friend std::weak_ptr<cGroundObject> CreateWheeled(int WheeledNum);
private:
	// Don't allow direct new/delete usage in code, only CreateWheeled()
	// allowed for cWheeled creation and release setup (deleter must be provided).
	explicit cWheeled(int WheeledNum);
};


// Create cCivilianBuilding object.
std::weak_ptr<cGroundObject> CreateCivilianBuilding(int BuildingNum);
// Create cMilitaryBuilding object.
std::weak_ptr<cGroundObject> CreateMilitaryBuilding(int MilitaryBuildingNum);
// Create cTracked object.
std::weak_ptr<cGroundObject> CreateTracked(int TrackedNum);
// Create cWheeled object.
std::weak_ptr<cGroundObject> CreateWheeled(int WheeledNum);
// Update and remove (erase) dead objects.
void UpdateAllGroundObjects(float Time);
// Draw all ground objects.
void DrawAllGroundObjects(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular ground object.
void ReleaseGroundObject(std::weak_ptr<cGroundObject> &Object);
// Release all ground objects.
void ReleaseAllGroundObjects();
// Managed cycle for each ground object.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachGroundObject(std::function<void (cGroundObject &Object, eGroundCycle &Command)> function);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H
