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

#include "object3d.h"
#include "../config/config.h"
#include "../assets/audio.h"
#include "space_ship/space_ship.h"
#include "ground_object/ground_object.h"
#include "projectile/projectile.h"
#include "space_object/space_object.h"
#include "explosion/explosion.h"
#include "explosion/bullet_explosion/bullet_explosion.h"
#include "explosion/ground_explosion/ground_explosion.h"
#include "explosion/space_explosion/space_explosion.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

float GetProjectileSpeed(int Num);
void PlayBulletExplosion(sVECTOR3D Location, bool NeedExplosionSFX, int ExplType);

// FIXME should be fixed, don't allow global scope interaction for local variables
extern cSpaceShip *StartSpaceShip;
extern cSpaceShip *EndSpaceShip;

extern float GameMoney;
extern float GameExperience;
extern int CurrentProfile;
extern int AlienShipsKillQuant;
extern float AlienShipsKillBonus;
extern int AlienMotherShipsKillQuant;
extern float AlienMotherShipsKillBonus;
extern int PirateShipsKillQuant;
extern float PirateShipsKillBonus;
extern int PirateVehiclesKillQuant;
extern float PirateVehiclesKillBonus;
extern int PirateBuildingsKillQuant;
extern float PirateBuildingsKillBonus;
extern int AsteroidsKillQuant;
extern float AsteroidsKillBonus;

extern int CurrentMission;
float GameCameraGetSpeed();

bool CheckHitBBMeshCollisionDetection(const cObject3D &Object1, const cObject3D &Object2, int &Object1PieceNum);
bool CheckMeshSphereCollisionDetection(const cObject3D &Object1, const cObject3D &Object2,
				       sVECTOR3D &NewLoc, int &Object1PieceNum);
bool CheckHitBBHitBBCollisionDetection(const cObject3D &Object1, const cObject3D &Object2,
				       int &Object1PieceNum, int &Object2PieceNum);
bool CheckHitBBOBBCollisionDetection(const cObject3D &Object1, const cObject3D &Object2, int &Object1PieceNum);

// щит у игрока
extern float ShildEnergyStatus;
extern float ShildStartHitStatus;
// что взорвалось у игрока
extern int PlayerDeadObjectPieceNum;


//-----------------------------------------------------------------------------
// Вспомогательная функция для проверки всех снарядов
//-----------------------------------------------------------------------------
struct sDamagesData {
	float DamageHull;
	float DamageSystems;
};

bool DetectProjectileCollision(const cObject3D &Object, int &ObjectPieceNum, cProjectile &Projectile,
			       sVECTOR3D &IntercPoint, sDamagesData &DamagesData, float ObjectSpeed)
{
	DamagesData.DamageHull = 0.0f;
	DamagesData.DamageSystems = 0.0f;
	// поправка на скорость камеры для корабля игрока
	if (Object.ObjectStatus == eObjectStatus::Player)
		ObjectSpeed += GameCameraGetSpeed();

	// проверяем, нужно ли для этого снаряда проверять данный объект
	// снаряды союзников или игрока - с врагами
	if ((((Object.ObjectStatus == eObjectStatus::Enemy) && ((Projectile.ObjectStatus == eObjectStatus::Ally) || (Projectile.ObjectStatus == eObjectStatus::Player))) ||
	     // снаряды врагов - с союзниками или игроком
	     (((Object.ObjectStatus == eObjectStatus::Ally) || (Object.ObjectStatus == eObjectStatus::Player)) && (Projectile.ObjectStatus == eObjectStatus::Enemy)) ||
	     // снаряды игрока со всеми, кроме игрока
	     (((Object.ObjectStatus == eObjectStatus::Enemy) || (Object.ObjectStatus == eObjectStatus::Ally)) && (Projectile.ObjectStatus == eObjectStatus::Player))) ||
	    // или это не разрушаемый объект и нужно 100% проверить, чтобы не пролетало через него снарядов
	    !NeedCheckCollision(Object)) {
		switch (Projectile.ProjectileType) {
		// обычные снаряды
		case 0:
			// если игрок со щитом или дифлектором, и щит заряжен
			if (((ShildEnergyStatus * ShildStartHitStatus) > (Projectile.DamageHull * GameEnemyWeaponPenalty)) &&
			    (Object.ObjectStatus == eObjectStatus::Player) &&
				// у игрока есть щит, просто проверяем, если снаряд приблизился
				// на расстояние =< радиуса - уничтожаем его
			    vw_SphereSphereCollision(Object.Radius, Object.Location,
						     Projectile.Radius, Projectile.Location, Projectile.PrevLocation)) {
				// если отражатель, разворачиваем пулю...
				if (GameAdvancedProtectionSystem == 4) {
					Projectile.ObjectStatus = Object.ObjectStatus;
					Projectile.SetRotation(Projectile.Rotation ^ (-1));

					for (auto &tmpGFX : Projectile.GraphicFX) {
						if (auto sharedGFX = tmpGFX.lock()) {
							sharedGFX->ParticlesPerSec = static_cast<int>(sharedGFX->ParticlesPerSec * GameEnemyWeaponPenalty);
							sharedGFX->Speed = sharedGFX->Speed * GameEnemyWeaponPenalty;
							sharedGFX->Life = sharedGFX->Life / GameEnemyWeaponPenalty;
							sharedGFX->MagnetFactor = sharedGFX->MagnetFactor * GameEnemyWeaponPenalty * GameEnemyWeaponPenalty;
						}
					}
					Projectile.SpeedStart = Projectile.Speed * GameEnemyWeaponPenalty;
					Projectile.SpeedEnd = (Projectile.Speed * GameEnemyWeaponPenalty) / 4.0f;
					Projectile.Age = Projectile.Age / GameEnemyWeaponPenalty;
					Projectile.Lifetime = Projectile.Lifetime / GameEnemyWeaponPenalty;

					// корректируем данные щита
					float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
					// физический урон минимален
					CurrentStatus -= Projectile.DamageHull / 5.0f;
					// а ионный на оборот
					CurrentStatus -= Projectile.DamageSystems * 2.0f;
					if (CurrentStatus < 0.0f)
						CurrentStatus = 0.0f;
					// и вычисляем сколько осталось в щите
					ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

					Projectile.DamageHull = Projectile.DamageHull * GameEnemyWeaponPenalty;
					Projectile.DamageSystems = Projectile.DamageSystems * GameEnemyWeaponPenalty;

					// здесь только так! иначе уничтожим снаряд
					return false;
				} else {
					// "разбиваем" снаряд о корпус, звук тянем отдельно!
					new cBulletExplosion(&Object, &Projectile, Projectile.Num, Projectile.Location, ObjectSpeed, false);

					// где сейчас, там и погибли
					IntercPoint = Projectile.Location;

					// корректируем данные щита
					float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
					// физический урон минимален
					CurrentStatus -= Projectile.DamageHull / 5.0f;
					// а ионный на оборот
					CurrentStatus -= Projectile.DamageSystems * 2.0f;
					if (CurrentStatus < 0.0f)
						CurrentStatus = 0.0f;
					// и вычисляем сколько осталось в щите
					ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

					// столкновение не было (!!! именно так, иначе ерунда с указателем на снаряд)
					DamagesData.DamageHull = 0.0f;
					DamagesData.DamageSystems = 0.0f;
					return true;
				}
			} else if (vw_SphereSphereCollision(Object.Radius, Object.Location,
							    Projectile.Radius, Projectile.Location, Projectile.PrevLocation) &&
				   vw_SphereAABBCollision(Object.AABB, Object.Location,
							  Projectile.Radius, Projectile.Location, Projectile.PrevLocation) &&
				   vw_SphereOBBCollision(Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat,
							 Projectile.Radius, Projectile.Location, Projectile.PrevLocation) &&
				   CheckMeshSphereCollisionDetection(Object, Projectile, IntercPoint, ObjectPieceNum)) {

				if (NeedCheckCollision(Object)) {
					DamagesData.DamageHull = Projectile.DamageHull;
					DamagesData.DamageSystems = Projectile.DamageSystems;
				}
				// "разбиваем" снаряд о корпус
				// звук тянем отдельно!
				if (NeedCheckCollision(Object))
					new cBulletExplosion(&Object, &Projectile, Projectile.Num, IntercPoint, ObjectSpeed, false);
				else
					new cBulletExplosion(&Object, &Projectile, Projectile.Num, IntercPoint, 0.0f, false);

				// столкновение было
				return true;
			}
			break;

		// ракеты-торпеды-бомбы
		case 1:
			// если игрок со щитом или дифлектором, и щит заряжен
			if (((ShildEnergyStatus * ShildStartHitStatus) > (Projectile.DamageHull * GameEnemyWeaponPenalty)) &&
			    (Object.ObjectStatus == eObjectStatus::Player)) {
				// у игрока есть щит, просто проверяем, если снаряд приблизился
				// на расстояние =< радиуса - уничтожаем его
				if (vw_SphereSphereCollision(Object.Radius, Object.Location,
							     Projectile.Radius, Projectile.Location, Projectile.PrevLocation)) {
					IntercPoint = Projectile.Location;

					// "разбиваем" снаряд о корпус
					new cBulletExplosion(&Object, &Projectile, -Projectile.Num, Projectile.Location, ObjectSpeed);

					// корректируем данные щита
					float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
					// физический урон минимален
					CurrentStatus -= Projectile.DamageHull / 5.0f;
					// а ионный на оборот
					CurrentStatus -= Projectile.DamageHull * 2.0f;
					if (CurrentStatus < 0.0f)
						CurrentStatus = 0.0f;
					// и вычисляем сколько осталось в щите
					ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

					// передаем, столкновение было, чтобы корректно удалить снаряд в общей процедуре
					DamagesData.DamageHull = 0.0f;
					DamagesData.DamageSystems = 0.0f;
					return true;
				}
			} else if (vw_SphereSphereCollision(Object.Radius, Object.Location,
							    Projectile.Radius, Projectile.Location, Projectile.PrevLocation) &&
				   // это ракета, просто проверяем, что она близко - и взрываем
				   vw_SphereAABBCollision(Object.AABB, Object.Location,
							  Projectile.Radius, Projectile.Location, Projectile.PrevLocation) &&
				   vw_SphereOBBCollision(Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat,
							 Projectile.Radius, Projectile.Location, Projectile.PrevLocation) &&
				// ставим так, т.к.на больших кораблях плохо
				   vw_OBBOBBCollision(Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat,
						      Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat) &&
				   CheckHitBBOBBCollisionDetection(Object, Projectile, ObjectPieceNum)) {

				// если это не убиваемый объект, должны столкнуться с геометрией
				if (!NeedCheckCollision(Object)) {
					if (CheckMeshSphereCollisionDetection(Object, Projectile, IntercPoint, ObjectPieceNum)) {
						// взрываем...
						if (NeedCheckCollision(Object))
							new cBulletExplosion(&Object, &Projectile, Projectile.Num, Projectile.Location, Projectile.Speed);
						else
							new cBulletExplosion(&Object, &Projectile, Projectile.Num, Projectile.Location, 0.0f);

						return true;
					} else
						return false;
				}

				IntercPoint = Projectile.Location;

				if (NeedCheckCollision(Object)) {
					DamagesData.DamageHull = Projectile.DamageHull;
					DamagesData.DamageSystems = Projectile.DamageSystems;
				}
				// взрываем...
				if (NeedCheckCollision(Object))
					new cBulletExplosion(&Object, &Projectile, Projectile.Num, Projectile.Location, Projectile.Speed);
				else
					new cBulletExplosion(&Object, &Projectile, Projectile.Num, Projectile.Location, 0.0f);
				// столкновение было
				return true;
			}
			break;

		// лучевое оружие
		case 2:
			if (vw_AABBAABBCollision(Object.AABB, Object.Location, Projectile.AABB, Projectile.Location) &&
			// в данном случае именно Projectile на первом месте!!!
			    vw_SphereOBBCollision(Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat,
						  Object.Radius, Object.Location, Object.PrevLocation) &&
			    vw_OBBOBBCollision(Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat,
					       Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat) &&
			    CheckHitBBOBBCollisionDetection(Object, Projectile, ObjectPieceNum)) {
				IntercPoint = Object.Location;

				// находим по дельте повреждения
				DamagesData.DamageHull = Projectile.DamageHull * Object.TimeDelta;
				// всегда ноль
				DamagesData.DamageSystems = 0.0f;
				// столкновение было
				return true;
			}
			break;
		}

	}

	// не было столкновения
	return false;
}

// единая процедура для проигрывания эффекта разлета всех снарядов
void DestroyProjectileWithExplosion(const cProjectile &Projectile, const sVECTOR3D &IntercPoint)
{
	switch (Projectile.ProjectileType) {
	// обычные снаряды
	case 0:
		// "разбиваем" снаряд о корпус
		/*	cBulletExplosion *TMPBulletExplosion;
			TMPBulletExplosion = new cBulletExplosion;
			TMPBulletExplosion->Create(0, Projectile, Projectile->Num, IntercPoint, ObjectSpeed, Projectile->NeedDeadSound);
		*/
		PlayBulletExplosion(IntercPoint, Projectile.NeedDeadSound, Projectile.Num);
		break;

	// ракеты-торпеды-бомбы
	// уже взорвали...
	case 1:
		break;

	// лучевое оружие
	// никогда не "взрываются"
	case 2:
		break;
	}
}

//-----------------------------------------------------------------------------
// Считаем, награду за збитого противника...  или штраф за збитого своего
//-----------------------------------------------------------------------------
void AddPlayerBonus(const cObject3D &Object, eObjectStatus KilledByObjectStatus)
{
	// убили врага
	if ((Object.ObjectStatus == eObjectStatus::Enemy) &&
	    (KilledByObjectStatus == eObjectStatus::Player)) {
		// вычисляем на какое значение нужно делить, в зависимости от кол-ва раз пройденной миссии
		float BonusDiv{1.0f};
		for (int i = 0; i < GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission]; i++) {
			BonusDiv = BonusDiv * 2.0f;
		}

		float TTTExperience{0.0f};
		// don't use 'default' case here, we need compiler's warning if anyone was missed
		switch (Object.ObjectType) {
		case eObjectType::none:
		case eObjectType::EarthFighter:
		case eObjectType::SpaceDebris:
		case eObjectType::ShipWeapon:
		case eObjectType::Explosion:
		case eObjectType::CivilianBuilding:
		case eObjectType::BasePart:
		case eObjectType::Planet:
		case eObjectType::BigAsteroid:
		case eObjectType::Projectile:
			break;

		case eObjectType::AlienFighter:
			AlienShipsKillBonus += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			AlienShipsKillQuant += 1;
			TTTExperience += (Object.StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::AlienMotherShip:
			AlienMotherShipsKillBonus += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			AlienMotherShipsKillQuant += 1;
			TTTExperience += (Object.StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::PirateShip:
			PirateShipsKillBonus += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			PirateShipsKillQuant += 1;
			TTTExperience += (Object.StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::PirateVehicle:
			PirateVehiclesKillBonus += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			PirateVehiclesKillQuant += 1;
			TTTExperience += (Object.StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::PirateBuilding:
			PirateBuildingsKillBonus += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object.StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			PirateBuildingsKillQuant += 1;
			TTTExperience += (Object.StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::SmallAsteroid:
			AsteroidsKillBonus += ((Object.StrengthStart*GameEnemyArmorPenalty)/8.0f)/BonusDiv;
			GameMoney += ((Object.StrengthStart * GameEnemyArmorPenalty) / 8.0f) / BonusDiv;
			AsteroidsKillQuant += 1;
			TTTExperience += (Object.StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 8.0f;
			break;
		}

		GameExperience += TTTExperience;
	}
}

//-----------------------------------------------------------------------------
// Главная функция проверки столкновения
//-----------------------------------------------------------------------------
void DetectCollisionAllObject3D()
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверка для всех кораблей
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cSpaceShip *tmpShip = StartSpaceShip;
	while (tmpShip) {
		cSpaceShip *tmpShip2 = tmpShip->Next;

		ForEachProjectile([&tmpShip, &tmpShip2] (cProjectile &tmpProjectile, eProjectileCycle &ProjectileCycleCommand) {
			sVECTOR3D IntercPoint;
			sDamagesData DamagesData;
			int ObjectPieceNum;

			if (DetectProjectileCollision(*tmpShip, ObjectPieceNum, tmpProjectile, IntercPoint, DamagesData, tmpShip->Speed)) {
				tmpShip2 = tmpShip->Next; // обязательно!!! если попали торпедой или бомбой!!!

				// если на корабле есть щит, сначала его уничтожаем
				if (tmpShip->ShieldStrength > DamagesData.DamageSystems) {
					tmpShip->ShieldStrength -= DamagesData.DamageSystems;
					DamagesData.DamageSystems = 0.0f;
				} else {
					// если щита мало
					DamagesData.DamageSystems -= tmpShip->ShieldStrength;
					tmpShip->ShieldStrength = 0.0f;
				}

				if (tmpShip->ShieldStrength > DamagesData.DamageHull) {
					tmpShip->ShieldStrength -= DamagesData.DamageHull;
					DamagesData.DamageHull = 0.0f;
				} else {
					// если щита мало
					DamagesData.DamageHull -= tmpShip->ShieldStrength;
					tmpShip->ShieldStrength = 0.0f;
				}

				if (tmpShip->ShieldStrength < 0.0f)
					tmpShip->ShieldStrength = 0.0f;

				tmpShip->Strength -= DamagesData.DamageHull / tmpShip->ResistanceHull;

				tmpShip->Strength -= (DamagesData.DamageSystems / tmpShip->ResistanceHull) * vw_fRand();
				// есть шанс полностью убить пришельца
				if ((DamagesData.DamageSystems > 0.0f) &&
				    (tmpShip->ObjectType == eObjectType::AlienFighter) &&
				    (vw_fRand() > 0.7f))
					tmpShip->Strength = 0;

				if (tmpShip->Strength <= 0.0f) {
					AddPlayerBonus(*tmpShip, tmpProjectile.ObjectStatus);

					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(*tmpShip, 2, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(*tmpShip, 31, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(*tmpShip, 33, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(*tmpShip, 3, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							else
								new cSpaceExplosion(*tmpShip, 31, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						default:
							break;
						}
						delete tmpShip;
						tmpShip = nullptr;
					} else {
						// запоминаем, что взорвалось
						PlayerDeadObjectPieceNum = ObjectPieceNum;
					}
				} else if ((tmpShip->ObjectStatus != eObjectStatus::Player) && // игроку тут ничего не делаем!.. с него хватит и щита
					// если это не босс уровня (Alien MotherShip)
					   (tmpShip->ObjectType != eObjectType::AlienMotherShip) &&
					// если нужно, смотрим что делать с системами
					   (DamagesData.DamageSystems > 0.0f)) {

					float Rand = vw_fRand();
					// поправка на мощьность выстрела
					float DR = DamagesData.DamageSystems/300.0f;

					// выводим из строя управляемость кораблем
					if (Rand > 0.6f-DR)
						tmpShip->MaxSpeed = tmpShip->MaxSpeed / 2.0f;
					if ((Rand > 0.3f - DR) && (Rand < 0.6f))
						tmpShip->MaxSpeedRotate = tmpShip->MaxSpeedRotate / 2.0f;

					// если есть фларес, есть шанс его вырубить
					if (!tmpShip->FlareWeaponSlots.empty() &&
					    (Rand > 0.5f-DR) && (Rand < 0.8f))
						tmpShip->FlareWeaponSlots.clear();
				}

				if (tmpProjectile.ProjectileType != 2) {
					if (!tmpShip)
						tmpProjectile.NeedDeadSound = false;
					DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
					ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndContinue;
				}

				if (!tmpShip) {
					// break projectile cycle
					switch (ProjectileCycleCommand) {
					case eProjectileCycle::Continue:
						ProjectileCycleCommand = eProjectileCycle::Break;
						break;
					case eProjectileCycle::DeleteObjectAndContinue:
						ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndBreak;
						break;
					default:
						break;
					}
				}
			}
			if (ProjectileCycleCommand != eProjectileCycle::Continue)
				return;

			// проверка на попадание в оружие (только для игрока и если включено в настройках)
			// проверять только до OBB
			if (tmpShip &&
			    (tmpShip->ObjectStatus == eObjectStatus::Player) &&
			    (tmpProjectile.ObjectStatus == eObjectStatus::Enemy) &&
			    !GameUndestroyableWeapon && !tmpShip->WeaponSlots.empty()) {
				for (auto &tmpWeaponSlot : tmpShip->WeaponSlots) {
					if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
						sDamagesData DamagesDataWeapon;
						int ObjectPieceNumWeapon;

						if ((sharedWeapon->Strength > 0.0f) &&
						    DetectProjectileCollision(*sharedWeapon, ObjectPieceNumWeapon, tmpProjectile, IntercPoint, DamagesDataWeapon, tmpShip->Speed)) {
							tmpShip2 = tmpShip->Next; // обязательно!!! если попали торпедой или бомбой!!!
							// FIXME вот тут все очень плохо, т.к. можем убить и сам tmpShip

							// просто делаем изменения в прочности... и больше ничего
							sharedWeapon->Strength -= DamagesDataWeapon.DamageHull / sharedWeapon->ResistanceHull;
							if (sharedWeapon->Strength <= 0.0f) {
								sharedWeapon->Strength = 0.0f;
								PlayVoicePhrase(eVoicePhrase::WeaponDestroyed, 1.0f);
							} else
								PlayVoicePhrase(eVoicePhrase::WeaponDamaged, 1.0f);

							// удаляем только те, которые разбились
							if (tmpProjectile.ProjectileType != 2) {
								DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
								ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndContinue;
								return;
							}
						}
					}
				}
			}
		});

		// проверяем столкновение
		// cSpaceObject
		if (tmpShip) // FIXME should be revised, when tmpShip will also have ForEach...()
		ForEachSpaceObject([&tmpShip] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
			int ObjectPieceNum;
			if (vw_SphereSphereCollision(tmpShip->Radius, tmpShip->Location,
						     tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation) &&
			    vw_SphereAABBCollision(tmpShip->AABB, tmpShip->Location,
						   tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation) &&
			    vw_SphereOBBCollision(tmpShip->OBB.Box, tmpShip->OBB.Location, tmpShip->Location, tmpShip->CurrentRotationMat,
						  tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation) &&
			    CheckHitBBOBBCollisionDetection(*tmpShip, tmpSpace, ObjectPieceNum)) {

				if (((tmpSpace.ObjectType == eObjectType::BasePart) ||
				     (tmpSpace.ObjectType == eObjectType::BigAsteroid)) &&
				    !CheckHitBBMeshCollisionDetection(*tmpShip, tmpSpace, ObjectPieceNum))
					return; // eSpaceCycle::Continue

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpSpace)) {
					// ум. из расчета, что полностью разрушим за 2 секунды для игрока и 0.5 секунду для остальных
					if (tmpShip->ObjectStatus != eObjectStatus::Player)
						tmpShip->Strength -= (tmpShip->StrengthStart / 0.5f) * tmpShip->TimeDelta;
					else
						tmpShip->Strength -= (tmpShip->StrengthStart / 2.0f) * tmpShip->TimeDelta;
				} else {
					float StrTMP = tmpShip->Strength;
					tmpShip->Strength -= tmpSpace.Strength / tmpShip->ResistanceHull;
					tmpSpace.Strength -= StrTMP / tmpSpace.ResistanceHull;
				}
				if (!NeedCheckCollision(*tmpShip))
					tmpSpace.Strength = 0.0f;


				// если уже все... удаляем
				if (NeedCheckCollision(tmpSpace) &&
				    (tmpSpace.Strength <= 0.0f)) {
					AddPlayerBonus(tmpSpace, tmpShip->ObjectStatus);

					switch (tmpSpace.ObjectType) {
					case eObjectType::SmallAsteroid:
						new cSpaceExplosion(tmpSpace, 1, tmpSpace.Location, tmpSpace.Speed, -1);
						break;
					case eObjectType::SpaceDebris:
						new cSpaceExplosion(tmpSpace, 32, tmpSpace.Location, tmpSpace.Speed, -1);
						break;
					default:
						break;
					}
					SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
				}

				if (NeedCheckCollision(*tmpShip) &&
					// если уже все... удаляем
				    (tmpShip->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(*tmpShip, 2, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(*tmpShip, 33, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(*tmpShip, 3, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							else
								new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						default:
							break;
						}
						delete tmpShip;
						tmpShip = nullptr;

						// break space cycle
						switch (SpaceCycleCommand) {
						case eSpaceCycle::Continue:
							SpaceCycleCommand = eSpaceCycle::Break;
							break;
						case eSpaceCycle::DeleteObjectAndContinue:
							SpaceCycleCommand = eSpaceCycle::DeleteObjectAndBreak;
							break;
						default:
							break;
						}

					} else {
						// запоминаем, что взорвалось
						PlayerDeadObjectPieceNum = ObjectPieceNum;
					}
				}
			}
		});

		// проверяем на столкновение с наземнымы объектами
		// 1 - радиус-радиус
		// 2 - AABB-AABB
		// 3 - OBB-OBB
		// 4 - HitBB-HitBB
		if (tmpShip) // FIXME should be revised, when tmpShip will also have ForEach...()
		ForEachGroundObject([&tmpShip] (cGroundObject &tmpGround, eGroundCycle &GroundCycleCommand) {
			int ObjectPieceNum1;
			int ObjectPieceNum2;
			if (vw_SphereSphereCollision(tmpShip->Radius, tmpShip->Location,
						     tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation) &&
			    vw_SphereAABBCollision(tmpShip->AABB, tmpShip->Location,
						   tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation) &&
			    vw_SphereOBBCollision(tmpShip->OBB.Box, tmpShip->OBB.Location, tmpShip->Location, tmpShip->CurrentRotationMat,
						  tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation) &&
			    CheckHitBBHitBBCollisionDetection(*tmpShip, tmpGround, ObjectPieceNum1, ObjectPieceNum2)) {

				if ((tmpGround.ObjectType == eObjectType::CivilianBuilding) &&
				    !CheckHitBBMeshCollisionDetection(*tmpShip, tmpGround, ObjectPieceNum2))
						return; // eGroundCycle::Continue

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpGround)) {
					// ум. из расчета, что полностью разрушим за 2 секунды для игрока и 0.5 секунду для остальных
					if (tmpShip->ObjectStatus != eObjectStatus::Player)
						tmpShip->Strength -= (tmpShip->StrengthStart / 0.5f) * tmpShip->TimeDelta;
					else
						tmpShip->Strength -= (tmpShip->StrengthStart / 2.0f) * tmpShip->TimeDelta;
				} else {
					float StrTMP = tmpShip->Strength;
					tmpShip->Strength -= tmpGround.Strength / tmpShip->ResistanceHull;
					tmpGround.Strength -= StrTMP / tmpGround.ResistanceHull;
				}
				if (!NeedCheckCollision(*tmpShip))
					tmpGround.Strength = 0.0f;

				// если уже все... удаляем
				if (NeedCheckCollision(tmpGround) &&
				    (tmpGround.Strength <= 0.0f)) {
					AddPlayerBonus(tmpGround, tmpShip->ObjectStatus);

					switch (tmpGround.ObjectType) {
					case eObjectType::PirateBuilding:
						new cGroundExplosion(tmpGround, 1, tmpGround.Location, ObjectPieceNum2);
						break;
					case eObjectType::PirateVehicle:
						new cGroundExplosion(tmpGround, 2, tmpGround.Location, ObjectPieceNum2);
						break;
					default:
						break;
					}
					GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
				}

				if (NeedCheckCollision(*tmpShip) &&
					// если уже все... удаляем
				    (tmpShip->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(*tmpShip, 2, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(*tmpShip, 33, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(*tmpShip, 3, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							else
								new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						default:
							break;
						}
						delete tmpShip;
						tmpShip = nullptr;

						// break ground cycle
						switch (GroundCycleCommand) {
						case eGroundCycle::Continue:
							GroundCycleCommand = eGroundCycle::Break;
							break;
						case eGroundCycle::DeleteObjectAndContinue:
							GroundCycleCommand = eGroundCycle::DeleteObjectAndBreak;
							break;
						default:
							break;
						}

					} else {
						// запоминаем, что взорвалось
						PlayerDeadObjectPieceNum = ObjectPieceNum1;
					}
				}
			}
		});

		// проверяем все cSpaceShip с cSpaceShip
		// 1 - радиус-радиус
		// 2 - AABB-AABB
		// 3 - OBB-OBB
		// 4 - HitBB-HitBB
		// проверяем со всеми, начиная со следующего и до конца
		cSpaceShip *tmpCollisionShip1 = tmpShip2;
		while (tmpCollisionShip1 && tmpShip) {
			cSpaceShip *tmpCollisionShip2 = tmpCollisionShip1->Next;

			int ObjectPieceNum1;
			int ObjectPieceNum2;
			if (vw_SphereSphereCollision(tmpShip->Radius, tmpShip->Location,
						     tmpCollisionShip1->Radius, tmpCollisionShip1->Location, tmpCollisionShip1->PrevLocation) &&
			    vw_AABBAABBCollision(tmpShip->AABB, tmpShip->Location, tmpCollisionShip1->AABB, tmpCollisionShip1->Location) &&
			    vw_OBBOBBCollision(tmpShip->OBB.Box, tmpShip->OBB.Location, tmpShip->Location, tmpShip->CurrentRotationMat,
					       tmpCollisionShip1->OBB.Box, tmpCollisionShip1->OBB.Location, tmpCollisionShip1->Location, tmpCollisionShip1->CurrentRotationMat) &&
			    CheckHitBBHitBBCollisionDetection(*tmpShip, *tmpCollisionShip1, ObjectPieceNum1, ObjectPieceNum2)) {

				float StrTMP = tmpShip->Strength;
				tmpShip->Strength -= tmpCollisionShip1->Strength / tmpShip->ResistanceHull;
				tmpCollisionShip1->Strength -= StrTMP / tmpCollisionShip1->ResistanceHull;

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(*tmpCollisionShip1))
					tmpShip->Strength = 0.0f;
				if (!NeedCheckCollision(*tmpShip))
					tmpCollisionShip1->Strength = 0.0f;

				// проверка на бонус
				if (NeedCheckCollision(*tmpCollisionShip1) &&
				    (tmpCollisionShip1->Strength <= 0.0f))
					AddPlayerBonus(*tmpCollisionShip1, tmpShip->ObjectStatus);
				if (NeedCheckCollision(*tmpShip) &&
				    (tmpShip->Strength <= 0.0f))
					AddPlayerBonus(*tmpShip, tmpCollisionShip1->ObjectStatus);

				if (NeedCheckCollision(*tmpCollisionShip1) &&
					// если уже все... удаляем
				    (tmpCollisionShip1->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpCollisionShip1->ObjectStatus != eObjectStatus::Player) {
						switch (tmpCollisionShip1->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(*tmpCollisionShip1, 2, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(*tmpCollisionShip1, 31, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(*tmpCollisionShip1, 33, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						case eObjectType::PirateShip:
							if (tmpCollisionShip1->InternalType <= 5)
								new cSpaceExplosion(*tmpCollisionShip1, 3, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							else
								new cSpaceExplosion(*tmpCollisionShip1, 31, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						default:
							break;
						}
						// если удаляем, нужно подправить указатель...
						if (tmpCollisionShip1 == tmpShip2)
							tmpShip2 = tmpShip2->Next;
						delete tmpCollisionShip1;
						tmpCollisionShip1 = nullptr;
					} else {
						// запоминаем, что взорвалось
						PlayerDeadObjectPieceNum = ObjectPieceNum2;
					}
				}

				if (NeedCheckCollision(*tmpShip) &&
					// если уже все... удаляем
				    (tmpShip->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(*tmpShip, 2, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(*tmpShip, 33, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(*tmpShip, 3, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							else
								new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						default:
							break;
						}
						delete tmpShip;
						tmpShip = nullptr;
					} else {
						// запоминаем, что взорвалось
						PlayerDeadObjectPieceNum = ObjectPieceNum1;
					}
				}
			}

			tmpCollisionShip1 = tmpCollisionShip2;
		}

		tmpShip = tmpShip2;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем все cGroundObject с
	// cProjectile
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ForEachGroundObject([] (cGroundObject &tmpGround, eGroundCycle &GroundCycleCommand) {
		ForEachProjectile([&tmpGround, &GroundCycleCommand] (cProjectile &tmpProjectile, eProjectileCycle &ProjectileCycleCommand) {
			sVECTOR3D IntercPoint;
			sDamagesData DamagesData;
			int ObjectPieceNum;

			if (DetectProjectileCollision(tmpGround, ObjectPieceNum, tmpProjectile, IntercPoint, DamagesData, tmpGround.Speed)) {
				if (NeedCheckCollision(tmpGround)) {
					tmpGround.Strength -= DamagesData.DamageHull / tmpGround.ResistanceHull;

					if (tmpGround.Strength <= 0.0f) {
						AddPlayerBonus(tmpGround, tmpProjectile.ObjectStatus);

						switch (tmpGround.ObjectType) {
						case eObjectType::PirateBuilding:
							new cGroundExplosion(tmpGround, 1, IntercPoint, ObjectPieceNum);
							break;
						case eObjectType::PirateVehicle:
							new cGroundExplosion(tmpGround, 2, IntercPoint, ObjectPieceNum);
							break;
						default:
							break;
						}
						GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
					}
				}

				if (tmpProjectile.ProjectileType != 2) {
					// if ground object destroyed, we should play only "explosion" sfx, without "hit" sfx
					if (GroundCycleCommand == eGroundCycle::DeleteObjectAndContinue)
						tmpProjectile.NeedDeadSound = false;
					DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
					ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndContinue;
				}

				if (GroundCycleCommand == eGroundCycle::DeleteObjectAndContinue) {
					// break projectile cycle
					switch (ProjectileCycleCommand) {
					case eProjectileCycle::Continue:
						ProjectileCycleCommand = eProjectileCycle::Break;
						break;
					case eProjectileCycle::DeleteObjectAndContinue:
						ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndBreak;
						break;
					default:
						break;
					}
				}
			}
		});
		if (GroundCycleCommand == eGroundCycle::DeleteObjectAndContinue)
			return;

		// проверяем столкновение
		// cSpaceObject
		ForEachSpaceObject([&tmpGround, &GroundCycleCommand] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
			int ObjectPieceNum;

			// не проверяем с частями базы
			if ((tmpSpace.ObjectType != eObjectType::BasePart) &&
				// не проверяем если оба не можем уничтожить
			    (NeedCheckCollision(tmpGround) || NeedCheckCollision(tmpSpace)) &&
			    vw_SphereSphereCollision(tmpGround.Radius, tmpGround.Location,
						    tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation) &&
			    vw_SphereAABBCollision(tmpGround.AABB, tmpGround.Location,
						   tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation) &&
			    vw_SphereOBBCollision(tmpGround.OBB.Box, tmpGround.OBB.Location, tmpGround.Location, tmpGround.CurrentRotationMat,
						  tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation) &&
			    CheckHitBBOBBCollisionDetection(tmpGround, tmpSpace, ObjectPieceNum)) {

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpSpace))
					tmpGround.Strength -= (tmpGround.StrengthStart / 0.5f) * tmpGround.TimeDelta;
				else {
					float StrTMP = tmpGround.Strength;
					tmpGround.Strength -= tmpSpace.Strength / tmpGround.ResistanceHull;
					tmpSpace.Strength -= StrTMP / tmpSpace.ResistanceHull;
				}
				if (!NeedCheckCollision(tmpGround))
					tmpSpace.Strength = 0.0f;


				// если уже все... удаляем
				if (NeedCheckCollision(tmpSpace) &&
				    (tmpSpace.Strength <= 0.0f)) {
					AddPlayerBonus(tmpSpace, tmpGround.ObjectStatus);

					switch (tmpSpace.ObjectType) {
					case eObjectType::SmallAsteroid:
						new cSpaceExplosion(tmpSpace, 1, tmpSpace.Location, tmpSpace.Speed, -1);
						break;
					case eObjectType::SpaceDebris:
						new cSpaceExplosion(tmpSpace, 32, tmpSpace.Location, tmpSpace.Speed, -1);
						break;
					default:
						break;
					}
					SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
				}

				if (NeedCheckCollision(tmpGround) &&
				    (tmpGround.Strength <= 0.0f)) {
					switch (tmpGround.ObjectType) {
					case eObjectType::PirateBuilding:
						new cGroundExplosion(tmpGround, 1, tmpGround.Location, ObjectPieceNum);
						break;
					case eObjectType::PirateVehicle:
						new cGroundExplosion(tmpGround, 2, tmpGround.Location, ObjectPieceNum);
						break;
					default:
						break;
						}

					GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
					// break space cycle
					switch (SpaceCycleCommand) {
					case eSpaceCycle::Continue:
						SpaceCycleCommand = eSpaceCycle::Break;
						break;
					case eSpaceCycle::DeleteObjectAndContinue:
						SpaceCycleCommand = eSpaceCycle::DeleteObjectAndBreak;
						break;
					default:
						break;
					}
				}
			}
		});
	});

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем все cSpaceObject
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ForEachSpaceObject([] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
		ForEachProjectile([&tmpSpace, &SpaceCycleCommand] (cProjectile &tmpProjectile, eProjectileCycle &ProjectileCycleCommand) {
			int ObjectPieceNum;
			sVECTOR3D IntercPoint;
			sDamagesData DamagesData;

			if (DetectProjectileCollision(tmpSpace, ObjectPieceNum, tmpProjectile, IntercPoint, DamagesData, tmpSpace.Speed)) {
				if (NeedCheckCollision(tmpSpace)) {
					tmpSpace.Strength -= DamagesData.DamageHull / tmpSpace.ResistanceHull;
					if (tmpSpace.Strength <= 0.0f) {
						AddPlayerBonus(tmpSpace, tmpProjectile.ObjectStatus);

						switch (tmpSpace.ObjectType) {
						case eObjectType::SmallAsteroid:
							new cSpaceExplosion(tmpSpace, 1, IntercPoint, tmpSpace.Speed, -1);
							break;
						case eObjectType::SpaceDebris:
							new cSpaceExplosion(tmpSpace, 32, IntercPoint, tmpSpace.Speed, -1);
							break;
						default:
							break;
						}
						SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
					}
				}

				if (tmpProjectile.ProjectileType != 2) {
					// if space object destroyed, we should play only "explosion" sfx, without "hit" sfx
					if (SpaceCycleCommand == eSpaceCycle::DeleteObjectAndContinue)
						tmpProjectile.NeedDeadSound = false;
					DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
					ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndContinue;
				}

				if (SpaceCycleCommand == eSpaceCycle::DeleteObjectAndContinue) {
					// break projectile cycle
					switch (ProjectileCycleCommand) {
					case eProjectileCycle::Continue:
						ProjectileCycleCommand = eProjectileCycle::Break;
						break;
					case eProjectileCycle::DeleteObjectAndContinue:
						ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndBreak;
						break;
					default:
						break;
					}
				}
			}
		});
	});

	ForEachSpaceObjectPair([] (cSpaceObject &FirstObject, cSpaceObject &SecondObject, eSpacePairCycle &Command) {
		if ((NeedCheckCollision(SecondObject) || NeedCheckCollision(FirstObject)) &&
		    vw_SphereSphereCollision(FirstObject.Radius, FirstObject.Location,
					     SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
		    vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
				       SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {
			// если попали в часть базы - просто летим в другую сторону,
			// если это обломок корабля или модели
			if (((FirstObject.ObjectType == eObjectType::BasePart) && (SecondObject.ObjectType == eObjectType::SpaceDebris)) ||
			    ((FirstObject.ObjectType == eObjectType::SpaceDebris) && (SecondObject.ObjectType == eObjectType::BasePart))) {
				if (FirstObject.ObjectType == eObjectType::SpaceDebris)
					vw_RotatePoint(FirstObject.Orientation, sVECTOR3D{0.0f, 180.0f, 0.0f});
				if (SecondObject.ObjectType == eObjectType::SpaceDebris)
					vw_RotatePoint(SecondObject.Orientation, sVECTOR3D{0.0f, 180.0f, 0.0f});

				return; // eSpacePairCycle::Continue
			}

			// смотрим, чтобы это были не только обломки с обломками (иначе не красиво взрываются корабли)
			if ((SecondObject.ObjectType != eObjectType::SpaceDebris) ||
			    (FirstObject.ObjectType != eObjectType::SpaceDebris)) {

				int ObjectPieceNum;

				// проверка, если это столкновение с базой - надо внимательно смотреть
				if ((FirstObject.ObjectType == eObjectType::BasePart) &&
				    (!CheckHitBBMeshCollisionDetection(SecondObject, FirstObject, ObjectPieceNum)))
						return; // eSpacePairCycle::Continue
				// проверка, если это столкновение с базой - надо внимательно смотреть
				if ((SecondObject.ObjectType == eObjectType::BasePart) &&
				    (!CheckHitBBMeshCollisionDetection(FirstObject, SecondObject, ObjectPieceNum)))
						return; // eSpacePairCycle::Continue

				bool SFXplayed = false;

				if ((NeedCheckCollision(SecondObject)) &&
				    ((SecondObject.ObjectType == eObjectType::SmallAsteroid) ||
				     (SecondObject.ObjectType == eObjectType::SpaceDebris))) {
					switch (SecondObject.ObjectType) {
					case eObjectType::SmallAsteroid:
						new cSpaceExplosion(SecondObject, 1, SecondObject.Location, SecondObject.Speed, -1);
						break;
					case eObjectType::SpaceDebris:
						new cSpaceExplosion(SecondObject, 32, SecondObject.Location, SecondObject.Speed, -1);
						break;
					default:
						break;
					}
					SFXplayed = true;

					Command = eSpacePairCycle::DeleteSecondObjectAndContinue;
				}

				if (NeedCheckCollision(FirstObject) &&
				    ((FirstObject.ObjectType == eObjectType::SmallAsteroid) ||
				     (FirstObject.ObjectType == eObjectType::SpaceDebris))) {
					switch (FirstObject.ObjectType) {
					case eObjectType::SmallAsteroid:
						new cSpaceExplosion(FirstObject, 1, FirstObject.Location, FirstObject.Speed, -1, !SFXplayed);
						break;
					case eObjectType::SpaceDebris:
						new cSpaceExplosion(FirstObject, 32, FirstObject.Location, FirstObject.Speed, -1, !SFXplayed);
						break;
					default:
						break;
					}

					if (Command == eSpacePairCycle::DeleteSecondObjectAndContinue)
						Command = eSpacePairCycle::DeleteBothObjectsAndContinue;
					else
						Command = eSpacePairCycle::DeleteFirstObjectAndContinue;
				}
			}
		}
	});

	ForEachProjectilePair([] (cProjectile &FirstObject, cProjectile &SecondObject, eProjectilePairCycle &Command) {
		if (((FirstObject.ObjectStatus == eObjectStatus::Enemy) && ((SecondObject.ObjectStatus == eObjectStatus::Ally) || (SecondObject.ObjectStatus == eObjectStatus::Player))) ||
		    (((FirstObject.ObjectStatus == eObjectStatus::Ally) || (FirstObject.ObjectStatus == eObjectStatus::Player)) && (SecondObject.ObjectStatus == eObjectStatus::Enemy)) ||
		    ((FirstObject.ObjectStatus != eObjectStatus::Player) && (SecondObject.ObjectStatus == eObjectStatus::Player))) {

			// missile/mine with missile/mine
			if (((FirstObject.ProjectileType == 1) || (FirstObject.ProjectileType == 4)) &&
			    ((SecondObject.ProjectileType == 1) || (SecondObject.ProjectileType == 4))) {
				if (vw_SphereSphereCollision(FirstObject.Radius, FirstObject.Location,
							     SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
				    vw_SphereAABBCollision(FirstObject.AABB, FirstObject.Location,
							   SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
				    vw_SphereOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
							  SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
				    vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
						       SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {

					if ((FirstObject.ProjectileType == 1) || (FirstObject.ProjectileType == 4)) {
						FirstObject.Speed = 0.0f;
						new cBulletExplosion(nullptr, &FirstObject, -FirstObject.Num, FirstObject.Location, FirstObject.Speed);
						Command = eProjectilePairCycle::DeleteFirstObjectAndContinue;
					}
					if ((SecondObject.ProjectileType == 1) || (SecondObject.ProjectileType == 4)) {
						SecondObject.Speed = 0.0f;
						new cBulletExplosion(nullptr, &SecondObject, -SecondObject.Num, SecondObject.Location, SecondObject.Speed);

						if (Command == eProjectilePairCycle::DeleteFirstObjectAndContinue)
							Command = eProjectilePairCycle::DeleteBothObjectsAndContinue;
						else
							Command = eProjectilePairCycle::DeleteSecondObjectAndContinue;
					}
				}
			// missile/mine with beam
			} else if (((FirstObject.ProjectileType == 1) || (FirstObject.ProjectileType == 4)) && (SecondObject.ProjectileType == 2)) {
				if (vw_SphereOBBCollision(SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat,
							  FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation) &&
				    vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
						       SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {
					FirstObject.Speed = 0.0f;
					new cBulletExplosion(nullptr, &FirstObject, -FirstObject.Num, FirstObject.Location, FirstObject.Speed);
					Command = eProjectilePairCycle::DeleteFirstObjectAndContinue;
				}
			} else if (((SecondObject.ProjectileType == 1) || (SecondObject.ProjectileType == 4)) && (FirstObject.ProjectileType == 2)) {
				if (vw_SphereOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
							  SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
				    vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
						       SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {
					SecondObject.Speed = 0.0f;
					new cBulletExplosion(nullptr, &SecondObject, -SecondObject.Num, SecondObject.Location, SecondObject.Speed);
					Command = eProjectilePairCycle::DeleteSecondObjectAndContinue;
				}
			// missile/mine with projectile/flare
			} else if ((FirstObject.ProjectileType == 1) || (FirstObject.ProjectileType == 4)) {
				if (vw_SphereSphereCollision(FirstObject.Radius, FirstObject.Location,
							     SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
				    vw_SphereAABBCollision(FirstObject.AABB, FirstObject.Location,
							   SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation) &&
				    vw_SphereOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
							  SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)) {
					FirstObject.Speed = 0.0f;
					new cBulletExplosion(nullptr, &FirstObject, -FirstObject.Num, FirstObject.Location, FirstObject.Speed);
					Command = eProjectilePairCycle::DeleteFirstObjectAndContinue;
				}
			} else if ((SecondObject.ProjectileType == 1) || (SecondObject.ProjectileType == 4)) {
				if (vw_SphereSphereCollision(SecondObject.Radius, SecondObject.Location,
							     FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation) &&
				    vw_SphereAABBCollision(SecondObject.AABB, SecondObject.Location,
							   FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation) &&
				    vw_SphereOBBCollision(SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat,
							  FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation)) {
					SecondObject.Speed = 0.0f;
					new cBulletExplosion(nullptr, &SecondObject, -SecondObject.Num, SecondObject.Location, SecondObject.Speed);
					Command = eProjectilePairCycle::DeleteSecondObjectAndContinue;
				}
			}
		}
	});
}

//-----------------------------------------------------------------------------
// Проверка попадания в сферу Sphere-Sphere
//-----------------------------------------------------------------------------
bool CheckSphereSphereDestroyDetection(const cObject3D &Object1, const sVECTOR3D &Point, float Radius, float &Distance2)
{
	// квадрат расстояния между объектами
	Distance2 = (Object1.Location.x - Point.x) * (Object1.Location.x - Point.x) +
		    (Object1.Location.y - Point.y) * (Object1.Location.y - Point.y) +
		    (Object1.Location.z - Point.z) * (Object1.Location.z - Point.z);

	// если расстояние меньше или равно - значит есть столкновение по радиусам
	if (Distance2 <= Radius * Radius)
		return true;

	// объекты очень далеко
	return false;
}

//-----------------------------------------------------------------------------
// Удаление всех удаляемых объектов, если они ближе радиуса
//-----------------------------------------------------------------------------
void DestroyRadiusCollisionAllObject3D(const cObject3D &DontTouchObject, const sVECTOR3D &Point,
				       float Radius, float Damage, eObjectStatus ObjectStatus)
{
	// важно!!! в этой функции не удаляем снаряды (в т.ч. разрушаемые), иначе будут проблемы с ForEachProjectilePair()

	// важно!!!
	// у нас мощность ударной волны отличается от мощности детонации, и состовляет только 75%
	Damage = Damage * 0.75f;

	// квадрат расстояния между точкой и объектом
	float Distance2;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем все cSpaceObject
	//
	// делаем это первым, т.к. потом тут будет куча осколков которые тоже
	// придется взрывать
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ForEachSpaceObject([&] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
		if (NeedCheckCollision(tmpSpace) &&
		    ((((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpSpace.ObjectStatus == eObjectStatus::Enemy)) ||
		     ((ObjectStatus == eObjectStatus::Enemy) && ((tmpSpace.ObjectStatus == eObjectStatus::Ally) || (tmpSpace.ObjectStatus == eObjectStatus::Player)))) &&
		    (&DontTouchObject != &tmpSpace) && CheckSphereSphereDestroyDetection(tmpSpace, Point, Radius, Distance2)) {
			if ((tmpSpace.ObjectType == eObjectType::SpaceDebris) &&
			    (vw_fRand() > 0.4f))
				return; // eSpaceCycle::Continue;

			float DamageHull = Damage * (1.0f - Distance2 / (Radius * Radius));

			// отнимаем у всех по Damage
			tmpSpace.Strength -= DamageHull / tmpSpace.ResistanceHull;

			// если уже все... удаляем
			if (tmpSpace.Strength <= 0.0f) {
				// проверка, нужно начислять или нет
				AddPlayerBonus(tmpSpace, ObjectStatus);

				new cSpaceExplosion(tmpSpace, 1, tmpSpace.Location, tmpSpace.Speed, -1);

				SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
			}
		}
	});

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверка для всех кораблей
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cSpaceShip *tmpShip = StartSpaceShip;
	while (tmpShip) {
		cSpaceShip *tmpShip2 = tmpShip->Next;

		if (NeedCheckCollision(*tmpShip) &&
		    (((((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip->ObjectStatus == eObjectStatus::Enemy)) ||
		      ((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip->ObjectStatus == eObjectStatus::Ally) || (tmpShip->ObjectStatus == eObjectStatus::Player)))) &&
		     (&DontTouchObject != tmpShip) && CheckSphereSphereDestroyDetection(*tmpShip, Point, Radius, Distance2))) {

			float DamageHull = Damage * (1.0f - Distance2 / (Radius * Radius));

			// просто убираем щит
			tmpShip->ShieldStrength = 0.0f;

			// отнимаем у всех по DamageHull
			tmpShip->Strength -= DamageHull / tmpShip->ResistanceHull;

			// если уже все... удаляем
			if (tmpShip->Strength <= 0.0f) {
				// проверка, нужно начислять или нет
				AddPlayerBonus(*tmpShip, ObjectStatus);

				// если не корабль игрока! его удалим сами
				if (tmpShip->ObjectStatus != eObjectStatus::Player) {
					switch (tmpShip->ObjectType) {
					case eObjectType::AlienFighter:
						new cSpaceExplosion(*tmpShip, 2, tmpShip->Location, tmpShip->Speed, -1);
						break;
					case eObjectType::EarthFighter:
						new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, -1);
						break;
					case eObjectType::AlienMotherShip:
						new cSpaceExplosion(*tmpShip, 33, tmpShip->Location, tmpShip->Speed, 0);
						break;
					case eObjectType::PirateShip:
						if (tmpShip->InternalType <= 5)
							new cSpaceExplosion(*tmpShip, 3, tmpShip->Location, tmpShip->Speed, -1);
						else
							new cSpaceExplosion(*tmpShip, 31, tmpShip->Location, tmpShip->Speed, 0);
						break;
					default:
						break;
					}
					delete tmpShip;
					tmpShip = nullptr;
				}
			}
		}

		tmpShip = tmpShip2;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем все cGroundObject
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ForEachGroundObject([&] (cGroundObject &tmpGround, eGroundCycle &GroundCycleCommand) {
		if (NeedCheckCollision(tmpGround) &&
		    (((((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpGround.ObjectStatus == eObjectStatus::Enemy)) ||
		      ((ObjectStatus == eObjectStatus::Enemy) && ((tmpGround.ObjectStatus == eObjectStatus::Ally) || (tmpGround.ObjectStatus == eObjectStatus::Player)))) &&
		     (&DontTouchObject != &tmpGround) && CheckSphereSphereDestroyDetection(tmpGround, Point, Radius, Distance2))) {
			float DamageHull = Damage * (1.0f - Distance2 / (Radius * Radius));

			// отнимаем у всех по Damage
			tmpGround.Strength -= DamageHull / tmpGround.ResistanceHull;

			// если уже все... удаляем
			if (tmpGround.Strength <= 0.0f) {
				// проверка, нужно начислять или нет
				AddPlayerBonus(tmpGround, ObjectStatus);

				switch (tmpGround.ObjectType) {
				case eObjectType::PirateBuilding:
					new cGroundExplosion(tmpGround, 1, tmpGround.Location, -1);
					break;
				case eObjectType::PirateVehicle:
					new cGroundExplosion(tmpGround, 2, tmpGround.Location, -1);
					break;
				default:
					break;
				}

				GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
			}
		}
	});

	// снаряды не учитываем, иначе - поправь указатели, чтобы при удалении
	// следующего, не было сбоя... при обновлении указателя у первого
}

} // astromenace namespace
} // viewizard namespace
