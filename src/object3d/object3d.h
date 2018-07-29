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

// TODO as soon, as cSpaceShip will be moved to STL usage with weapon (shared_ptr?),
//      switch to eDeleteAfterLeaveScene::enabled by default and remove SetDeleteAfterLeaveScene()
//      also explosions creation code should be corrected (remove DeleteAfterLeaveScene setup)

// TODO in case DeleteAfterLeaveScene is 'enabled', also should be limited by time
//      if object was never shown on the scene (during this time), should be deleted + warning output

#ifndef OBJECT3D_OBJECT3D_H
#define OBJECT3D_OBJECT3D_H

#include "../core/core.h"
#include "../enum.h"
#include "../script/script.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, don't allow global scope interaction for local variables
extern int GameEnemyWeaponPenalty;
extern int GameEnemyArmorPenalty;
extern int GameEnemyTargetingSpeedPenalty;
extern int GameUnlimitedAmmo;
extern int GameUndestroyableWeapon;
extern int GameWeaponTargetingMode;
extern int GameSpaceShipControlMode;
extern int GameEngineSystem;
extern int GameTargetingSystem;
extern int GameAdvancedProtectionSystem;
extern int GamePowerSystem;
extern int GameTargetingMechanicSystem;

enum class eObjectStatus {
	none,
	Enemy,
	Ally,
	Player
};

enum class eObjectType {
	none,
	EarthFighter,
	AlienFighter,
	AlienMotherShip,
	PirateShip,
	PirateVehicle,
	PirateBuilding,
	SmallAsteroid,
	SpaceDebris,
	ShipWeapon,
	Projectile,
	Explosion,
	CivilianBuilding,
	BasePart,
	Planet,
	BigAsteroid
};

enum class eDeleteAfterLeaveScene {
	disabled,
	enabled,	// waiting, object should being shown on the scene first
	showed,		// object shown, waiting when it will be out of the scene
	need_delete,	// object shown and out of the scene, should be deleted
	wait_delay	// will be deleted after delay
};
// delay before object delete, since object could back to the scene
constexpr float DeleteAfterLeaveSceneDelay = 1.0f;

class cObject3D : public sModel3D {
protected:
	// don't allow object of this class creation
	cObject3D() = default;
	virtual ~cObject3D() = default;

public:
	// Прорисовка объектa Object3D
	virtual void Draw(bool VertexOnlyPass, bool ShadowMap = false);
	bool NeedCullFaces{true}; // нужно резать бэк фейсы
	bool NeedAlphaTest{false}; // нужно включить альфа тест
	// Обновление данных объектa Object3D
	virtual bool Update(float Time);

	// Установка положения 1 объекта модели
	void SetChunkLocation(const sVECTOR3D &NewLocation, unsigned ChunkNum);
	// Установка углов поворота 1 объекта модели
	void SetChunkRotation(const sVECTOR3D &NewRotation, unsigned ChunkNum);
	// Установка положения модели
	virtual void SetLocation(const sVECTOR3D &NewLocation);
	// Установка углов поворота модели
	virtual void SetRotation(const sVECTOR3D &NewRotation);

	// in-game object's status relatively to player
	eObjectStatus ObjectStatus{eObjectStatus::none};
	// global object type
	eObjectType ObjectType{eObjectType::none};
	// internal object's type for objects with same ObjectType, usually, same as creation type (num)
	int InternalType{0};

	// in case we need show object and delete after it leave scene (after DeleteAfterLeaveSceneDelay time)
	eDeleteAfterLeaveScene DeleteAfterLeaveScene{eDeleteAfterLeaveScene::disabled};
	// note, Lifetime could be changed by DeleteAfterLeaveScene settings
	float Lifetime{-1.0f};

	// направление, куда ориентирован объект
	sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
	// углы поворота объекта по трем осям на данный момент
	sVECTOR3D Rotation{0.0f, 0.0f, 0.0f};
	// обратный угол поворота, старого
	sVECTOR3D OldRotationInv{0.0f, 0.0f, 0.0f};
	// положение объекта
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	// предыдущее положение объекта
	sVECTOR3D PrevLocation{0.0f, 0.0f, 0.0f};

	// последнее время, когда проверяли-обновляли объект
	float TimeLastUpdate{-1.0f};
	// время между апдейтами
	float TimeDelta{0.0f};

	// текстура для объектов геометрии
	std::vector<GLtexture> Texture{};
	std::vector<GLtexture> TextureIllum{};
	std::vector<GLtexture> NormalMap{};

	// минимальное расстояние (квадрат) с которого надо рисовать пообъектно, если -1 всегда рисовать только пообъектно
	float PromptDrawDist2{-1.0f};
	int InternalLights{0}; // кол-во внутренних источников света

	// данные материала объекта
	float Diffuse[4]{1.0f, 1.0f, 1.0f, 1.0f};
	float Specular[4]{1.0f, 1.0f, 1.0f, 1.0f};
	float Ambient[4]{0.1f, 0.1f, 0.1f, 0.1f};
	float Power[1]{64.0f};

	// вес объекта
	float Weight{1.0f};
	// прочность
	float Strength{0.0f};
	float StrengthStart{0.0f};
	// прочность щита
	float ShieldStrength{0.0f};
	float ShieldStrengthStart{0.0f};
	float ShieldRecharge{0.0f}; // перезарядка щита в секунду
	// показывать полосу или нет
	bool ShowStrength{true};
	// этот параметр устанавливается автоматически!
	// если попали, будем всегда показывать (даже если щит перезарядился)
	bool NeedShowStrengthNow{false};

	// максимально возможное ослабление воздействий на корпус
	float ResistanceHull{1.0f};
	// максимально возможное ослабление воздействий на системы
	float ResistanceSystems{1.0f};

	// матрицы поворота, для оптимизации просчетов
	float CurrentRotationMat[9]{1.0f, 0.0f, 0.0f,
				    0.0f, 1.0f, 0.0f,
				    0.0f, 0.0f, 1.0f};
	float OldInvRotationMat[9]{1.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f,
				   0.0f, 0.0f, 1.0f};

	// debug info, line number in script file
	std::u32string ScriptLineNumberUTF32{};

	// данные по текущему сценарию действий объекта
	std::list<sTimeSheet> TimeSheetList{};
};


//-----------------------------------------------------------------------------
// Менеджер cObject3D
//-----------------------------------------------------------------------------

// Удаляем все объекты в списке
void ReleaseAllObject3D();
// Прорисовываем все объекты
void DrawAllObject3D(eDrawType DrawType);
// Проверяем все объекты на столкновение
void DetectCollisionAllObject3D();
// Проверяем все объекты, обновляем данные
void UpdateAllObject3D(float Time);


//-----------------------------------------------------------------------------
// Дополнительные функции для cObject3D
//-----------------------------------------------------------------------------

// Проверяем, нужно ли для данного объекта проверка коллизии и наведение на него
bool NeedCheckCollision(const cObject3D &Object3D);
// Загрузка в модель нужной геометрии
void LoadObjectData(const std::string &FileName, cObject3D &Object3D);
// Получение угла поворота оружия на врага
void GetShipOnTargetOrientateion(eObjectStatus ObjectStatus, const sVECTOR3D &Location,
				 const sVECTOR3D &CurrentObjectRotation,
				 float MinDistance, const float (&RotationMatrix)[9],
				 sVECTOR3D &NeedAngle, float ObjectWidth, bool NeedCenterOrientation,
				 bool NeedByWeaponOrientation, const sVECTOR3D &WeponLocation, int WeaponType);
// Получение угла поворота оружия на врага для противника
void GetEnemyShipOnTargetOrientateion(eObjectStatus ObjectStatus, const sVECTOR3D &Location,
				      const sVECTOR3D &CurrentObjectRotation, const float (&RotationMatrix)[9],
				      sVECTOR3D &NeedAngle, int WeaponType);
// Получение угла поворота турели на врага
bool GetTurretOnTargetOrientateion(eObjectStatus ObjectStatus, const sVECTOR3D &Location,
				   const sVECTOR3D &CurrentObjectRotation, const float (&RotationMatrix)[9],
				   sVECTOR3D &NeedAngle, int WeaponType);
// Получение данных для наведение ракет
std::weak_ptr<cObject3D> GetMissileOnTargetOrientateion(eObjectStatus ObjectStatus, const sVECTOR3D &Location,
					const sVECTOR3D &CurrentObjectRotation, const float (&RotationMatrix)[9],
					sVECTOR3D &NeedAngle, float MaxDistance);
bool GetMissileOnTargetOrientateion(const sVECTOR3D &Location, const sVECTOR3D &CurrentObjectRotation,
				    const float (&RotationMatrix)[9],
				    std::weak_ptr<cObject3D> &TargetObject, sVECTOR3D &NeedAngle);
bool GetMissileTargetStatus(std::weak_ptr<cObject3D> &TargetObject, const sVECTOR3D &Location,
			    const float (&RotationMatrix)[9]);
// Получение положения ближайшего врага, для мин
std::weak_ptr<cObject3D> GetCloserTargetPosition(eObjectStatus ObjectStatus, const sVECTOR3D &Location);
// Setup shaders.
bool SetupObject3DShaders();

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_OBJECT3D_H
