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

// TODO split earth fighter and player ship classes

#ifndef OBJECT3D_SPACESHIP_SPACESHIP_H
#define OBJECT3D_SPACESHIP_SPACESHIP_H

#include "../object3d.h"
#include "../shake.h"
#include "../weapon/weapon.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eShipCycle {
	Continue,
	Break,
	DeleteObjectAndContinue,
	DeleteObjectAndBreak
};

enum class eShipPairCycle {
	Continue,
	DeleteFirstObjectAndContinue,
	DeleteSecondObjectAndContinue,
	DeleteBothObjectsAndContinue
};

struct sShipWeaponSlot {
	bool SetFire{false};
	std::weak_ptr<cWeapon> Weapon{};
	sVECTOR3D Location{};
	int Type{1};
	float YAngle{0.0f};

	sShipWeaponSlot() = default;
	sShipWeaponSlot(const std::weak_ptr<cWeapon> &_Weapon, const sVECTOR3D &_Location) :
		Weapon{_Weapon},
		Location{_Location}
	{}
	~sShipWeaponSlot()
	{
		ReleaseWeaponLazy(Weapon);
	}

	void operator () (const std::weak_ptr<cWeapon> &_Weapon, const sVECTOR3D &_Location)
	{
		Weapon = _Weapon;
		Location = _Location;
	}
};

class cSpaceShip : public cObject3D {
protected:
	// don't allow object of this class creation
	cSpaceShip() = default;
	~cSpaceShip();

public:
	// Обновление данных объектa
	virtual bool Update(float Time) override;
	// Установка положения объекта
	virtual void SetLocation(const sVECTOR3D &NewLocation) override;
	virtual void SetLocationArcadePlayer(const sVECTOR3D &NewLocation);
	// Установка углов поворота объекта
	virtual void SetRotation(const sVECTOR3D &NewRotation) override;

	// текущей вектор движения корабля
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	// нужно повернуть (пересчитать мэш) при следующем проходе
	// Update + учесть эти данные в Rotation
	sVECTOR3D NeedRotate{0.0f, 0.0f, 0.0f};
	// скорость поворота по каждому раправлению
	sVECTOR3D RotationSpeed{1.0f, 1.0f, 1.0f};

	// максимальная скорость units/sec, зависит от двигателя
	float MaxSpeed{0.0f};
	// максимальное ускорение units/sec*sec, зависит от двигателя
	float MaxAcceler{0.0f};
	// макс. маневровых двигателей
	float MaxSpeedRotate{0.0f};

	// вперед-назад
	// текущая скорость
	float Speed{0.0f};
	// нужная скорость
	float NeedSpeed{0.0f};
	// текущее ускорение
	float Acceler{0.0f};
	// нужное ускорение
	float NeedAcceler{1.0f};

	// влево-вправо
	// текущая скорость
	float SpeedLR{0.0f};
	// нужная скорость
	float NeedSpeedLR{0.0f};
	// текущее ускорение
	float AccelerLR{0.0f};
	// нужное ускорение
	float NeedAccelerLR{1.0f};

	// вверх-вниз
	// текущая скорость
	float SpeedUD{0.0f};
	// нужная скорость
	float NeedSpeedUD{0.0f};
	// текущее ускорение
	float AccelerUD{0.0f};
	// нужное ускорение
	float NeedAccelerUD{1.0f};

	// вперед-назад по вектору камеры
	// текущая скорость
	float SpeedByCamFB{0.0f};
	// нужная скорость
	float NeedSpeedByCamFB{0.0f};
	// текущее ускорение
	float AccelerByCamFB{0.0f};
	// нужное ускорение
	float NeedAccelerByCamFB{1.0f};

	// влево-вправо
	// текущая скорость
	float SpeedByCamLR{0.0f};
	// нужная скорость
	float NeedSpeedByCamLR{0.0f};
	// текущее ускорение
	float AccelerByCamLR{0.0f};
	// нужное ускорение
	float NeedAccelerByCamLR{1.0f};

	// вверх-вниз
	// текущая скорость
	float SpeedByCamUD{0.0f};
	// нужная скорость
	float NeedSpeedByCamUD{0.0f};
	// текущее ускорение
	float AccelerByCamUD{0.0f};
	// нужное ускорение
	float NeedAccelerByCamUD{1.0f};

	// управление - куда перемещаться при сделующем Update'е
	// от 0.0 до 1.0, по "силе" движения в нужную сторону
	float MoveForward{0.0f};
	float MoveBackward{0.0f};
	float MoveLeft{0.0f};
	float MoveRight{0.0f};
	float MoveUp{0.0f};
	float MoveDown{0.0f};

	// небольшая девиация-болтание корабля
	std::vector<cShake> ShipShake{};

	std::vector<sShipWeaponSlot> WeaponSlots{};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int WeaponFireType{2};
	int WeaponGroupCurrentFireNum{-1};
	float WeaponGroupCurrentFireDelay{0.0f};

	std::vector<sShipWeaponSlot> BossWeaponSlots{};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int BossWeaponFireType{2};
	int BossWeaponGroupCurrentFireNum{-1};
	float BossWeaponGroupCurrentFireDelay{0.0f};

	std::vector<sShipWeaponSlot> FlareWeaponSlots{};

	// тип, как будем удалять двигатели -сразу, или глушить
	bool EngineDestroyType{false};
	std::vector<std::weak_ptr<cParticleSystem>> Engines{}; // двигатели
	std::vector<sVECTOR3D> EnginesLocation{}; // положение двигателей

	// двигатели поворотов, левый
	std::vector<std::weak_ptr<cParticleSystem>> EnginesLeft{}; // двигатели
	std::vector<sVECTOR3D> EnginesLeftLocation{}; // положение двигателей

	// двигатели поворотов, правый
	std::vector<std::weak_ptr<cParticleSystem>> EnginesRight{}; // двигатели
	std::vector<sVECTOR3D> EnginesRightLocation{}; // положение двигателей
};

class cAlienSpaceFighter final : public cSpaceShip {
	friend std::weak_ptr<cSpaceShip> CreateAlienSpaceFighter(const int SpaceShipNum);
private:
	// Don't allow direct new/delete usage in code, only CreateAlienSpaceFighter()
	// allowed for cAlienSpaceFighter creation and release setup (deleter must be provided).
	explicit cAlienSpaceFighter(const int SpaceShipNum);
	~cAlienSpaceFighter() = default;
};

class cAlienSpaceMotherShip final : public cSpaceShip {
	friend std::weak_ptr<cSpaceShip> CreateAlienSpaceMotherShip(const int SpaceShipNum);
private:
	// Don't allow direct new/delete usage in code, only CreateAlienSpaceMotherShip()
	// allowed for cAlienSpaceMotherShip creation and release setup (deleter must be provided).
	explicit cAlienSpaceMotherShip(const int SpaceShipNum);
	~cAlienSpaceMotherShip() = default;
};

class cEarthSpaceFighter final : public cSpaceShip {
	friend std::weak_ptr<cSpaceShip> CreateEarthSpaceFighter(const int SpaceShipNum);
private:
	// Don't allow direct new/delete usage in code, only cEarthSpaceFighter()
	// allowed for cEarthSpaceFighter creation and release setup (deleter must be provided).
	explicit cEarthSpaceFighter(const int SpaceShipNum);
	~cEarthSpaceFighter() = default;
};

class cPirateShip final : public cSpaceShip {
	friend std::weak_ptr<cSpaceShip> CreatePirateShip(const int SpaceShipNum);
private:
	// Don't allow direct new/delete usage in code, only cPirateShip()
	// allowed for cPirateShip creation and release setup (deleter must be provided).
	explicit cPirateShip(const int SpaceShipNum);
	~cPirateShip() = default;
};


// Create cAlienSpaceFighter object.
std::weak_ptr<cSpaceShip> CreateAlienSpaceFighter(const int SpaceShipNum);
// Create cAlienSpaceMotherShip object.
std::weak_ptr<cSpaceShip> CreateAlienSpaceMotherShip(const int SpaceShipNum);
// Create cEarthSpaceFighter object.
std::weak_ptr<cSpaceShip> CreateEarthSpaceFighter(const int SpaceShipNum);
// Create cPirateShip object.
std::weak_ptr<cSpaceShip> CreatePirateShip(const int SpaceShipNum);

// Проверяем все объекты, обновляем данные
void UpdateAllSpaceShip(float Time);
// Прорисовываем все объекты
void DrawAllSpaceShips(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular space ship.
void ReleaseSpaceShip(std::weak_ptr<cSpaceShip> &Ship);
// Удаляем все объекты в списке
void ReleaseAllSpaceShips();
// Cycle for each space ship.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceShip(std::function<void (cSpaceShip &Object)> function);
// Managed cycle for each space ship.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceShip(std::function<void (cSpaceShip &Object, eShipCycle &Command)> function);
// Managed cycle for each space ship pair.
// Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
void ForEachSpaceShipPair(std::function<void (cSpaceShip &FirstObject,
					      cSpaceShip &SecondObject,
					      eShipPairCycle &Command)> function);
// Get object ptr by reference.
std::weak_ptr<cObject3D> GetSpaceShipPtr(const cSpaceShip &Object);


// Установка системы двигателей
void SetEarthSpaceFighterEngine(std::weak_ptr<cSpaceShip> &SpaceShip, int EngineType);
// Установка брони для кораблей землян
void SetEarthSpaceFighterArmour(std::weak_ptr<cSpaceShip> &SpaceShip, int ArmourType);
// Установка оружия на корабль
bool SetEarthSpaceFighterWeapon(std::weak_ptr<cSpaceShip> &SpaceShip, int WeaponSlot, int WeaponNum);
// Получаем возможный поворот орудия в данном слоте
void GetShipWeaponSlotAngle(int ShipNum, int SlotNum, float *Min, float *Max);
//
float GetEngineAcceleration(int EngineType);
//
float GetEnginePower(int EngineType);
//
float GetEngineRotatePower(int EngineType);
//
float GetShipArmor(int SpaceShipNum);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_SPACESHIP_SPACESHIP_H
