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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

#include "object3d.h"
#include "../config/config.h"
#include "space_ship/space_ship.h"
#include "ground_object/ground_object.h"
#include "projectile/projectile.h"
#include "space_object/space_object.h"
#include "explosion/explosion.h"
#include "explosion/bullet_explosion/bullet_explosion.h"
#include "explosion/ground_explosion/ground_explosion.h"
#include "explosion/space_explosion/space_explosion.h"

float GetProjectileSpeed(int Num);
void PlayBulletExplosion(sVECTOR3D Location, bool NeedExplosionSFX, int ExplType);

// FIXME should be fixed, don't allow global scope interaction for local variables
extern cSpaceShip *StartSpaceShip;
extern cSpaceShip *EndSpaceShip;
extern cProjectile *StartProjectile;
extern cProjectile *EndProjectile;
extern cGroundObject *StartGroundObject;
extern cGroundObject *EndGroundObject;
extern cSpaceObject *StartSpaceObject;
extern cSpaceObject *EndSpaceObject;

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

bool CheckHitBBMeshCollisionDetection(cObject3D *Object1, cObject3D *Object2, int *Object1PieceNum);

bool CheckMeshSphereCollisionDetection(cObject3D *Object1, cObject3D *Object2, sVECTOR3D *NewLoc, int *Object1PieceNum);
bool CheckHitBBHitBBCollisionDetection(cObject3D *Object1, cObject3D *Object2, int *Object1PieceNum, int *Object2PieceNum);
bool CheckHitBBOBBCollisionDetection(cObject3D *Object1, cObject3D *Object2, int *Object1PieceNum);

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

bool DetectProjectileCollision(cObject3D *Object, int *ObjectPieceNum, cProjectile *Projectile, sVECTOR3D *IntercPoint, sDamagesData *DamagesData, float ObjectSpeed)
{
	DamagesData->DamageHull = 0.0f;
	DamagesData->DamageSystems = 0.0f;
	// поправка на скорость камеры для корабля игрока
	if (Object->ObjectStatus == eObjectStatus::Player)
		ObjectSpeed += GameCameraGetSpeed();

	// проверяем, нужно ли для этого снаряда проверять данный объект
	// снаряды союзников или игрока - с врагами
	if ((((Object->ObjectStatus == eObjectStatus::Enemy) && ((Projectile->ObjectStatus == eObjectStatus::Ally) || (Projectile->ObjectStatus == eObjectStatus::Player))) ||
	     // снаряды врагов - с союзниками или игроком
	     (((Object->ObjectStatus == eObjectStatus::Ally) || (Object->ObjectStatus == eObjectStatus::Player)) && (Projectile->ObjectStatus == eObjectStatus::Enemy)) ||
	     // снаряды игрока со всеми, кроме игрока
	     (((Object->ObjectStatus == eObjectStatus::Enemy) || (Object->ObjectStatus == eObjectStatus::Ally)) && (Projectile->ObjectStatus == eObjectStatus::Player)))
	    // или это не разрушаемый объект и нужно 100% проверить, чтобы не пролетало через него снарядов
	    || (!NeedCheckCollision(Object))) {
		switch (Projectile->ProjectileType) {
		// обычные снаряды
		case 0:
			// если игрок со щитом или дифлектором, и щит заряжен
			if (((ShildEnergyStatus * ShildStartHitStatus) > (Projectile->DamageHull * GameEnemyWeaponPenalty)) &&
			    (Object->ObjectStatus == eObjectStatus::Player) &&
				// у игрока есть щит, просто проверяем, если снаряд приблизился
				// на расстояние =< радиуса - уничтожаем его
			    vw_SphereSphereCollision(Object->Radius, Object->Location,
						     Projectile->Radius, Projectile->Location, Projectile->PrevLocation)) {
				// если отражатель, разворачиваем пулю...
				if (GameAdvancedProtectionSystem == 4) {
					Projectile->ObjectStatus = Object->ObjectStatus;
					Projectile->SetRotation(Projectile->Rotation ^ (-1));

					for (auto tmpGFX : Projectile->GraphicFX) {
						if (auto sharedGFX = tmpGFX.lock()) {
							sharedGFX->ParticlesPerSec = static_cast<int>(sharedGFX->ParticlesPerSec * GameEnemyWeaponPenalty);
							sharedGFX->Speed = sharedGFX->Speed * GameEnemyWeaponPenalty;
							sharedGFX->Life = sharedGFX->Life / GameEnemyWeaponPenalty;
							sharedGFX->MagnetFactor = sharedGFX->MagnetFactor * GameEnemyWeaponPenalty * GameEnemyWeaponPenalty;
						}
					}
					Projectile->SpeedStart = Projectile->Speed * GameEnemyWeaponPenalty;
					Projectile->SpeedEnd = (Projectile->Speed * GameEnemyWeaponPenalty) / 4.0f;
					Projectile->Age = Projectile->Age / GameEnemyWeaponPenalty;
					Projectile->Lifetime = Projectile->Lifetime / GameEnemyWeaponPenalty;

					// корректируем данные щита
					float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
					// физический урон минимален
					CurrentStatus -= Projectile->DamageHull / 5.0f;
					// а ионный на оборот
					CurrentStatus -= Projectile->DamageSystems * 2.0f;
					if (CurrentStatus < 0.0f)
						CurrentStatus = 0.0f;
					// и вычисляем сколько осталось в щите
					ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

					Projectile->DamageHull = Projectile->DamageHull * GameEnemyWeaponPenalty;
					Projectile->DamageSystems = Projectile->DamageSystems * GameEnemyWeaponPenalty;

					// здесь только так! иначе уничтожим снаряд
					return false;
				} else {
					// "разбиваем" снаряд о корпус, звук тянем отдельно!
					new cBulletExplosion(Object, Projectile, Projectile->Num, Projectile->Location, ObjectSpeed, false);

					// где сейчас, там и погибли
					*IntercPoint = Projectile->Location;

					// корректируем данные щита
					float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
					// физический урон минимален
					CurrentStatus -= Projectile->DamageHull / 5.0f;
					// а ионный на оборот
					CurrentStatus -= Projectile->DamageSystems * 2.0f;
					if (CurrentStatus < 0.0f)
						CurrentStatus = 0.0f;
					// и вычисляем сколько осталось в щите
					ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

					// столкновение не было (!!! именно так, иначе ерунда с указателем на снаряд)
					DamagesData->DamageHull = 0.0f;
					DamagesData->DamageSystems = 0.0f;
					return true;
				}
			} else if (vw_SphereSphereCollision(Object->Radius, Object->Location,
							    Projectile->Radius, Projectile->Location, Projectile->PrevLocation) &&
				   vw_SphereAABBCollision(Object->AABB, Object->Location,
							  Projectile->Radius, Projectile->Location, Projectile->PrevLocation) &&
				   vw_SphereOBBCollision(Object->OBB.Box, Object->OBB.Location, Object->Location, Object->CurrentRotationMat,
							 Projectile->Radius, Projectile->Location, Projectile->PrevLocation) &&
				   CheckMeshSphereCollisionDetection(Object, Projectile, IntercPoint, ObjectPieceNum)) {

				if (NeedCheckCollision(Object)) {
					DamagesData->DamageHull = Projectile->DamageHull;
					DamagesData->DamageSystems = Projectile->DamageSystems;
				}
				// "разбиваем" снаряд о корпус
				// звук тянем отдельно!
				if (NeedCheckCollision(Object))
					new cBulletExplosion(Object, Projectile, Projectile->Num, *IntercPoint, ObjectSpeed, false);
				else
					new cBulletExplosion(Object, Projectile, Projectile->Num, *IntercPoint, 0.0f, false);

				// столкновение было
				return true;
			}
			break;

		// ракеты-торпеды-бомбы
		case 1:
			// если игрок со щитом или дифлектором, и щит заряжен
			if (((ShildEnergyStatus * ShildStartHitStatus) > (Projectile->DamageHull * GameEnemyWeaponPenalty)) &&
			    (Object->ObjectStatus == eObjectStatus::Player)) {
				// у игрока есть щит, просто проверяем, если снаряд приблизился
				// на расстояние =< радиуса - уничтожаем его
				if (vw_SphereSphereCollision(Object->Radius, Object->Location,
							     Projectile->Radius, Projectile->Location, Projectile->PrevLocation)) {
					*IntercPoint = Projectile->Location;

					// "разбиваем" снаряд о корпус
					new cBulletExplosion(Object, Projectile, -Projectile->Num, Projectile->Location, ObjectSpeed);

					// корректируем данные щита
					float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
					// физический урон минимален
					CurrentStatus -= Projectile->DamageHull / 5.0f;
					// а ионный на оборот
					CurrentStatus -= Projectile->DamageHull * 2.0f;
					if (CurrentStatus < 0.0f)
						CurrentStatus = 0.0f;
					// и вычисляем сколько осталось в щите
					ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

					// передаем, столкновение было, чтобы корректно удалить снаряд в общей процедуре
					DamagesData->DamageHull = 0.0f;
					DamagesData->DamageSystems = 0.0f;
					return true;
				}
			} else if (vw_SphereSphereCollision(Object->Radius, Object->Location,
							    Projectile->Radius, Projectile->Location, Projectile->PrevLocation) &&
				   // это ракета, просто проверяем, что она близко - и взрываем
				   vw_SphereAABBCollision(Object->AABB, Object->Location,
							  Projectile->Radius, Projectile->Location, Projectile->PrevLocation) &&
				   vw_SphereOBBCollision(Object->OBB.Box, Object->OBB.Location, Object->Location, Object->CurrentRotationMat,
							 Projectile->Radius, Projectile->Location, Projectile->PrevLocation) &&
				// ставим так, т.к.на больших кораблях плохо
				   vw_OBBOBBCollision(Object->OBB.Box, Object->OBB.Location, Object->Location, Object->CurrentRotationMat,
						      Projectile->OBB.Box, Projectile->OBB.Location, Projectile->Location, Projectile->CurrentRotationMat) &&
				   CheckHitBBOBBCollisionDetection(Object, Projectile, ObjectPieceNum)) {

				// если это не убиваемый объект, должны столкнуться с геометрией
				if (!NeedCheckCollision(Object)) {
					if (CheckMeshSphereCollisionDetection(Object, Projectile, IntercPoint, ObjectPieceNum)) {
						// взрываем...
						if (NeedCheckCollision(Object))
							new cBulletExplosion(Object, Projectile, Projectile->Num, Projectile->Location, Projectile->Speed);
						else
							new cBulletExplosion(Object, Projectile, Projectile->Num, Projectile->Location, 0.0f);

						return true;
					} else
						return false;
				}

				*IntercPoint = Projectile->Location;

				if (NeedCheckCollision(Object)) {
					DamagesData->DamageHull = Projectile->DamageHull;
					DamagesData->DamageSystems = Projectile->DamageSystems;
				}
				// взрываем...
				if (NeedCheckCollision(Object))
					new cBulletExplosion(Object, Projectile, Projectile->Num, Projectile->Location, Projectile->Speed);
				else
					new cBulletExplosion(Object, Projectile, Projectile->Num, Projectile->Location, 0.0f);
				// столкновение было
				return true;
			}
			break;

		// лучевое оружие
		case 2:
			if (vw_AABBAABBCollision(Object->AABB, Object->Location, Projectile->AABB, Projectile->Location) &&
			// в данном случае именно Projectile на первом месте!!!
			    vw_SphereOBBCollision(Projectile->OBB.Box, Projectile->OBB.Location, Projectile->Location, Projectile->CurrentRotationMat,
						  Object->Radius, Object->Location, Object->PrevLocation) &&
			    vw_OBBOBBCollision(Projectile->OBB.Box, Projectile->OBB.Location, Projectile->Location, Projectile->CurrentRotationMat,
					       Object->OBB.Box, Object->OBB.Location, Object->Location, Object->CurrentRotationMat) &&
			    CheckHitBBOBBCollisionDetection(Object, Projectile, ObjectPieceNum)) {
				*IntercPoint = Object->Location;

				// находим по дельте повреждения
				DamagesData->DamageHull = Projectile->DamageHull*Object->TimeDelta;
				// всегда ноль
				DamagesData->DamageSystems = 0.0f;
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
void DestroyProjectileWithExplosion(cProjectile *Projectile, sVECTOR3D IntercPoint)
{
	if (!Projectile)
		return;

	switch (Projectile->ProjectileType) {
	// обычные снаряды
	case 0:
		// "разбиваем" снаряд о корпус
		/*	cBulletExplosion *TMPBulletExplosion;
			TMPBulletExplosion = new cBulletExplosion;
			TMPBulletExplosion->Create(0, Projectile, Projectile->Num, IntercPoint, ObjectSpeed, Projectile->NeedDeadSound);
		*/
		PlayBulletExplosion(IntercPoint, Projectile->NeedDeadSound, Projectile->Num);
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
void AddPlayerBonus(cObject3D *Object, eObjectStatus KilledByObjectStatus)
{
	// убили врага
	if ((Object->ObjectStatus == eObjectStatus::Enemy) &&
	    (KilledByObjectStatus == eObjectStatus::Player)) {
		// вычисляем на какое значение нужно делить, в зависимости от кол-ва раз пройденной миссии
		float BonusDiv{1.0f};
		for (int i = 0; i < GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission]; i++) {
			BonusDiv = BonusDiv * 2.0f;
		}

		float TTTExperience{0.0f};
		// don't use 'default' case here, we need compiler's warning if anyone was missed
		switch (Object->ObjectType) {
		case eObjectType::none:
		case eObjectType::EarthFighter:
		case eObjectType::ShipPart:
		case eObjectType::ShipWeapon:
		case eObjectType::Explosion:
		case eObjectType::CivilianBuilding:
		case eObjectType::BasePart:
		case eObjectType::Planet:
		case eObjectType::BigAsteroid:
		case eObjectType::Projectile:
			break;

		case eObjectType::AlienFighter:
			AlienShipsKillBonus += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			AlienShipsKillQuant += 1;
			TTTExperience += (Object->StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::AlienMotherShip:
			AlienMotherShipsKillBonus += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			AlienMotherShipsKillQuant += 1;
			TTTExperience += (Object->StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::PirateShip:
			PirateShipsKillBonus += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			PirateShipsKillQuant += 1;
			TTTExperience += (Object->StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::PirateVehicle:
			PirateVehiclesKillBonus += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			PirateVehiclesKillQuant += 1;
			TTTExperience += (Object->StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::PirateBuilding:
			PirateBuildingsKillBonus += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			GameMoney += ((Object->StrengthStart * GameEnemyArmorPenalty) / 1.8f) / BonusDiv;
			PirateBuildingsKillQuant += 1;
			TTTExperience += (Object->StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 1.8f;
			break;

		case eObjectType::Asteroids:
			AsteroidsKillBonus += ((Object->StrengthStart*GameEnemyArmorPenalty)/8.0f)/BonusDiv;
			GameMoney += ((Object->StrengthStart * GameEnemyArmorPenalty) / 8.0f) / BonusDiv;
			AsteroidsKillQuant += 1;
			TTTExperience += (Object->StrengthStart * GameEnemyArmorPenalty * (ProfileDifficulty(CurrentProfile) / 100.0f)) / 8.0f;
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

		// точка попадания, если оно есть
		sVECTOR3D IntercPoint;

		// проверяем со снарядами
		cProjectile *tmpProjectile = StartProjectile;
		while (tmpProjectile && tmpShip) {
			cProjectile *tmpProjectile2 = tmpProjectile->Next;

			// проверка на попадание в корпус корабля
			sDamagesData DamagesData;
			int ObjectPieceNum;
			if (DetectProjectileCollision(tmpShip, &ObjectPieceNum, tmpProjectile, &IntercPoint, &DamagesData, tmpShip->Speed)) {
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

				// если уже все... удаляем
				if (tmpShip->Strength <= 0.0f) {
					// проверка, нужно начислять или нет
					AddPlayerBonus(tmpShip, tmpProjectile->ObjectStatus);

					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(tmpShip, 2, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(tmpShip, 31, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(tmpShip, 33, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(tmpShip, 3, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							else
								new cSpaceExplosion(tmpShip, 31, IntercPoint, tmpShip->Speed, ObjectPieceNum);
							break;
						default:
							break;
						}
						delete tmpShip;
						tmpShip = nullptr;
						// убираем звук попадания-разбивания снаряда
						tmpProjectile->NeedDeadSound = false;
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
					if (tmpShip->WeaponFlare &&
					    (Rand > 0.5f-DR) && (Rand < 0.8f)) {
						delete tmpShip->WeaponFlare;
						tmpShip->WeaponFlare = nullptr;
					}
				}


				// удаляем только те, которые разбились
				if (tmpProjectile->ProjectileType != 2) {
					DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
					delete tmpProjectile;
					tmpProjectile = nullptr;
				}

			}

			// проверка на попадание в оружие (только для игрока и если включено в настройках)
			// проверять только до OBB
			if (tmpShip && tmpProjectile &&
			    (tmpShip->ObjectStatus == eObjectStatus::Player) &&
			    (tmpProjectile->ObjectStatus == eObjectStatus::Enemy) &&
			    !GameUndestroyableWeapon && tmpShip->Weapon) {
				for (int i = 0; i < tmpShip->WeaponQuantity; i++) {
					if (tmpProjectile &&
					    tmpShip->Weapon[i] &&
					// если еще целое
					    (tmpShip->Weapon[i]->Strength > 0.0f)) {
						// делаем проверку
						sDamagesData DamagesDataWeapon;
						int ObjectPieceNumWeapon;
						if(DetectProjectileCollision(tmpShip->Weapon[i], &ObjectPieceNumWeapon, tmpProjectile, &IntercPoint, &DamagesDataWeapon, tmpShip->Speed)) {
							tmpShip2 = tmpShip->Next; // обязательно!!! если попали торпедой или бомбой!!!

							// просто делаем изменения в прочности... и больше ничего
							tmpShip->Weapon[i]->Strength -= DamagesDataWeapon.DamageHull / tmpShip->Weapon[i]->ResistanceHull;
							if (tmpShip->Weapon[i]->Strength <= 0.0f) {
								tmpShip->Weapon[i]->Strength = 0.0f;
								PlayVoicePhrase(eVoicePhrase::WeaponDestroyed, 1.0f);
								// убираем звук попадания-разбивания снаряда
								tmpProjectile->NeedDeadSound = false;
							} else
								PlayVoicePhrase(eVoicePhrase::WeaponDamaged, 1.0f);

							// удаляем только те, которые разбились
							if (tmpProjectile->ProjectileType != 2) {
								DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
								delete tmpProjectile;
								tmpProjectile = nullptr;
							}
						}
					}
				}
			}

			// берем следующий снаряд
			tmpProjectile = tmpProjectile2;
		}

		// проверяем столкновение
		// cSpaceObject
		cSpaceObject *tmpS = StartSpaceObject;
		while (tmpS && tmpShip) {
			cSpaceObject *tmpSpace2 = tmpS->Next;

			int ObjectPieceNum;
			if (vw_SphereSphereCollision(tmpShip->Radius, tmpShip->Location,
						     tmpS->Radius, tmpS->Location, tmpS->PrevLocation) &&
			    vw_SphereAABBCollision(tmpShip->AABB, tmpShip->Location,
						   tmpS->Radius, tmpS->Location, tmpS->PrevLocation) &&
			    vw_SphereOBBCollision(tmpShip->OBB.Box, tmpShip->OBB.Location, tmpShip->Location, tmpShip->CurrentRotationMat,
						  tmpS->Radius, tmpS->Location, tmpS->PrevLocation) &&
			    CheckHitBBOBBCollisionDetection(tmpShip, tmpS, &ObjectPieceNum)) {

				if (((tmpS->ObjectType == eObjectType::BasePart) ||
				     (tmpS->ObjectType == eObjectType::BigAsteroid)) &&
				    !CheckHitBBMeshCollisionDetection(tmpShip, tmpS, &ObjectPieceNum)) {
					// если не столкнулись = выходим
					goto exitN1;
				}

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpS)) {
					// ум. из расчета, что полностью разрушим за 2 секунды для игрока и 0.5 секунду для остальных
					if (tmpShip->ObjectStatus != eObjectStatus::Player)
						tmpShip->Strength -= (tmpShip->StrengthStart / 0.5f) * tmpShip->TimeDelta;
					else
						tmpShip->Strength -= (tmpShip->StrengthStart / 2.0f) * tmpShip->TimeDelta;
				} else {
					float StrTMP = tmpShip->Strength;
					tmpShip->Strength -= tmpS->Strength / tmpShip->ResistanceHull;
					tmpS->Strength -= StrTMP / tmpS->ResistanceHull;
				}
				if (!NeedCheckCollision(tmpShip))
					tmpS->Strength = 0.0f;


				// если уже все... удаляем
				if (NeedCheckCollision(tmpS) &&
				    (tmpS->Strength <= 0.0f)) {
					AddPlayerBonus(tmpS, tmpShip->ObjectStatus);

					switch (tmpS->ObjectType) {
					case eObjectType::Asteroids:
						new cSpaceExplosion(tmpS, 1, tmpS->Location, tmpS->Speed, -1);
						break;
					case eObjectType::ShipPart:
						new cSpaceExplosion(tmpS, 32, tmpS->Location, tmpS->Speed, -1);
						break;
					default:
						break;
					}
					delete tmpS;
					tmpS = nullptr;
				}

				if (NeedCheckCollision(tmpShip) &&
					// если уже все... удаляем
				    (tmpShip->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(tmpShip, 2, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(tmpShip, 33, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(tmpShip, 3, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
							else
								new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum);
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
				}
			}
exitN1:
			tmpS = tmpSpace2;
		}

		// проверяем на столкновение с наземнымы объектами
		// 1 - радиус-радиус
		// 2 - AABB-AABB
		// 3 - OBB-OBB
		// 4 - HitBB-HitBB
		cGroundObject *tmpG = StartGroundObject;
		while (tmpG && tmpShip) {
			cGroundObject *tmpGround2 = tmpG->Next;

			int ObjectPieceNum1;
			int ObjectPieceNum2;
			if (vw_SphereSphereCollision(tmpShip->Radius, tmpShip->Location,
						     tmpG->Radius, tmpG->Location, tmpG->PrevLocation) &&
			    vw_SphereAABBCollision(tmpShip->AABB, tmpShip->Location,
						   tmpG->Radius, tmpG->Location, tmpG->PrevLocation) &&
			    vw_SphereOBBCollision(tmpShip->OBB.Box, tmpShip->OBB.Location, tmpShip->Location, tmpShip->CurrentRotationMat,
						  tmpG->Radius, tmpG->Location, tmpG->PrevLocation) &&
			    CheckHitBBHitBBCollisionDetection(tmpShip, tmpG, &ObjectPieceNum1, &ObjectPieceNum2)) {

				if ((tmpG->ObjectType == eObjectType::CivilianBuilding) &&
				    !CheckHitBBMeshCollisionDetection(tmpShip, tmpG, &ObjectPieceNum2)) {
						// если не столкнулись = выходим
						goto exitN2;
				}

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpG)) {
					// ум. из расчета, что полностью разрушим за 2 секунды для игрока и 0.5 секунду для остальных
					if (tmpShip->ObjectStatus != eObjectStatus::Player)
						tmpShip->Strength -= (tmpShip->StrengthStart / 0.5f) * tmpShip->TimeDelta;
					else
						tmpShip->Strength -= (tmpShip->StrengthStart / 2.0f) * tmpShip->TimeDelta;
				} else {
					float StrTMP = tmpShip->Strength;
					tmpShip->Strength -= tmpG->Strength / tmpShip->ResistanceHull;
					tmpG->Strength -= StrTMP / tmpG->ResistanceHull;
				}
				if (!NeedCheckCollision(tmpShip))
					tmpG->Strength = 0.0f;

				// если уже все... удаляем
				if (NeedCheckCollision(tmpG) &&
				    (tmpG->Strength <= 0.0f)) {
					AddPlayerBonus(tmpG, tmpShip->ObjectStatus);

					switch (tmpG->ObjectType) {
					case eObjectType::PirateBuilding:
						new cGroundExplosion(tmpG, 1, tmpG->Location, ObjectPieceNum2);
						break;
					case eObjectType::PirateVehicle:
						new cGroundExplosion(tmpG, 2, tmpG->Location, ObjectPieceNum2);
						break;
					default:
						break;
					}
					delete tmpG;
					tmpG = nullptr;
				}

				if (NeedCheckCollision(tmpShip) &&
					// если уже все... удаляем
				    (tmpShip->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(tmpShip, 2, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(tmpShip, 33, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(tmpShip, 3, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							else
								new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
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

exitN2:
			tmpG = tmpGround2;
		}

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
			    CheckHitBBHitBBCollisionDetection(tmpShip, tmpCollisionShip1, &ObjectPieceNum1, &ObjectPieceNum2)) {

				float StrTMP = tmpShip->Strength;
				tmpShip->Strength -= tmpCollisionShip1->Strength / tmpShip->ResistanceHull;
				tmpCollisionShip1->Strength -= StrTMP / tmpCollisionShip1->ResistanceHull;

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpCollisionShip1))
					tmpShip->Strength = 0.0f;
				if (!NeedCheckCollision(tmpShip))
					tmpCollisionShip1->Strength = 0.0f;

				// проверка на бонус
				if (NeedCheckCollision(tmpCollisionShip1) &&
				    (tmpCollisionShip1->Strength <= 0.0f))
					AddPlayerBonus(tmpCollisionShip1, tmpShip->ObjectStatus);
				if (NeedCheckCollision(tmpShip) &&
				    (tmpShip->Strength <= 0.0f))
					AddPlayerBonus(tmpShip, tmpCollisionShip1->ObjectStatus);

				if (NeedCheckCollision(tmpCollisionShip1) &&
					// если уже все... удаляем
				    (tmpCollisionShip1->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpCollisionShip1->ObjectStatus != eObjectStatus::Player) {
						switch (tmpCollisionShip1->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(tmpCollisionShip1, 2, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(tmpCollisionShip1, 31, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(tmpCollisionShip1, 33, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							break;
						case eObjectType::PirateShip:
							if (tmpCollisionShip1->InternalType <= 5)
								new cSpaceExplosion(tmpCollisionShip1, 3, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
							else
								new cSpaceExplosion(tmpCollisionShip1, 31, tmpCollisionShip1->Location, tmpCollisionShip1->Speed, ObjectPieceNum2);
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

				if (NeedCheckCollision(tmpShip) &&
					// если уже все... удаляем
				    (tmpShip->Strength <= 0.0f)) {
					// если не корабль игрока! его удалим сами
					if (tmpShip->ObjectStatus != eObjectStatus::Player) {
						switch (tmpShip->ObjectType) {
						case eObjectType::AlienFighter:
							new cSpaceExplosion(tmpShip, 2, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::EarthFighter:
							new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::AlienMotherShip:
							new cSpaceExplosion(tmpShip, 33, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							break;
						case eObjectType::PirateShip:
							if (tmpShip->InternalType <= 5)
								new cSpaceExplosion(tmpShip, 3, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
							else
								new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, ObjectPieceNum1);
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
	cGroundObject *tmpG = StartGroundObject;
	while (tmpG) {
		cGroundObject *tmpGround2 = tmpG->Next;

		// точка попадания, если оно есть
		sVECTOR3D IntercPoint;
		// проверяем со снарядами
		cProjectile *tmpProjectile = StartProjectile;
		while (tmpProjectile && tmpG) {
			cProjectile *tmpProjectile2 = tmpProjectile->Next;
			sDamagesData DamagesData;
			int ObjectPieceNum;

			if(DetectProjectileCollision(tmpG, &ObjectPieceNum, tmpProjectile, &IntercPoint, &DamagesData, tmpG->Speed)) {
				tmpGround2 = tmpG->Next; // обязательно!!! если попали торпедой или бомбой!!!

				if (NeedCheckCollision(tmpG)) {
					tmpG->Strength -= DamagesData.DamageHull / tmpG->ResistanceHull;

					// если уже все... удаляем
					if (tmpG->Strength <= 0.0f) {
						// проверка, нужно начислять или нет
						AddPlayerBonus(tmpG, tmpProjectile->ObjectStatus);

						switch (tmpG->ObjectType) {
						case eObjectType::PirateBuilding:
							new cGroundExplosion(tmpG, 1, IntercPoint, ObjectPieceNum);
							break;
						case eObjectType::PirateVehicle:
							new cGroundExplosion(tmpG, 2, IntercPoint, ObjectPieceNum);
							break;
						default:
							break;
						}
						delete tmpG;
						tmpG = nullptr;

						// убираем звук попадания-разбивания снаряда
						tmpProjectile->NeedDeadSound = false;
					}
				}

				// удаляем только те, которые разбились
				if (tmpProjectile->ProjectileType != 2) {
					DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
					delete tmpProjectile;
					tmpProjectile = nullptr;
				}
			}

			tmpProjectile = tmpProjectile2;
		}

		// проверяем столкновение
		// cSpaceObject
		cSpaceObject *tmpS = StartSpaceObject;
		while (tmpS && tmpG) {
			cSpaceObject *tmpSpace2 = tmpS->Next;

			int ObjectPieceNum;

			// не проверяем с частями базы
			if ((tmpS->ObjectType != eObjectType::BasePart) &&
				// не проверяем если оба не можем уничтожить
			    (NeedCheckCollision(tmpG) || NeedCheckCollision(tmpS)) &&
			    vw_SphereSphereCollision(tmpG->Radius, tmpG->Location,
						    tmpS->Radius, tmpS->Location, tmpS->PrevLocation) &&
			    vw_SphereAABBCollision(tmpG->AABB, tmpG->Location,
						   tmpS->Radius, tmpS->Location, tmpS->PrevLocation) &&
			    vw_SphereOBBCollision(tmpG->OBB.Box, tmpG->OBB.Location, tmpG->Location, tmpG->CurrentRotationMat,
						  tmpS->Radius, tmpS->Location, tmpS->PrevLocation) &&
			    CheckHitBBOBBCollisionDetection(tmpG, tmpS, &ObjectPieceNum)) {

				// если столкновение с преградой которую не можем уничтожить
				if (!NeedCheckCollision(tmpS))
					tmpG->Strength -= (tmpG->StrengthStart / 0.5f) * tmpG->TimeDelta;
				else {
					float StrTMP = tmpG->Strength;
					tmpG->Strength -= tmpS->Strength/tmpG->ResistanceHull;
					tmpS->Strength -= StrTMP/tmpS->ResistanceHull;
				}
				if (!NeedCheckCollision(tmpG))
					tmpS->Strength = 0.0f;


				// если уже все... удаляем
				if (NeedCheckCollision(tmpS) &&
				    (tmpS->Strength <= 0.0f)) {
					AddPlayerBonus(tmpS, tmpG->ObjectStatus);

					switch (tmpS->ObjectType) {
					case eObjectType::Asteroids:
						new cSpaceExplosion(tmpS, 1, tmpS->Location, tmpS->Speed, -1);
						break;
					case eObjectType::ShipPart:
						new cSpaceExplosion(tmpS, 32, tmpS->Location, tmpS->Speed, -1);
						break;
					default:
						break;
					}
					delete tmpS;
					tmpS = nullptr;
				}

				if (NeedCheckCollision(tmpG) &&
				    (tmpG->Strength <= 0.0f)) {
					switch (tmpG->ObjectType) {
					case eObjectType::PirateBuilding:
						new cGroundExplosion(tmpG, 1, tmpG->Location, ObjectPieceNum);
						break;
					case eObjectType::PirateVehicle:
						new cGroundExplosion(tmpG, 2, tmpG->Location, ObjectPieceNum);
						break;
					default:
						break;
						}
					delete tmpG;
					tmpG = nullptr;
				}
			}

			tmpS = tmpSpace2;
		}

		tmpG = tmpGround2;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем все cSpaceObject
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cSpaceObject *tmpS = StartSpaceObject;
	while (tmpS) {
		cSpaceObject *tmpSpace2 = tmpS->Next;

		// точка попадания, если оно есть
		sVECTOR3D IntercPoint;
		// проверяем со снарядами
		cProjectile *tmpProjectile = StartProjectile;
		while (tmpProjectile && tmpS) {
			cProjectile *tmpProjectile2 = tmpProjectile->Next;
			sDamagesData DamagesData;
			int ObjectPieceNum;

			if(DetectProjectileCollision(tmpS, &ObjectPieceNum, tmpProjectile, &IntercPoint, &DamagesData, tmpS->Speed)) {
				tmpSpace2 = tmpS->Next; // обязательно!!! если попали торпедой или бомбой!!!

				if (NeedCheckCollision(tmpS)) {
					tmpS->Strength -= DamagesData.DamageHull/tmpS->ResistanceHull;
					// если уже все... удаляем
					if (tmpS->Strength <= 0.0f) {
						// проверка, нужно начислять или нет
						AddPlayerBonus(tmpS, tmpProjectile->ObjectStatus);

						switch (tmpS->ObjectType) {
						case eObjectType::Asteroids:
							new cSpaceExplosion(tmpS, 1, IntercPoint, tmpS->Speed, -1);
							break;
						case eObjectType::ShipPart:
							new cSpaceExplosion(tmpS, 32, IntercPoint, tmpS->Speed, -1);
							break;
						default:
							break;
						}
						delete tmpS;
						tmpS = nullptr;

						// убираем звук попадания-разбивания снаряда
						tmpProjectile->NeedDeadSound = false;
					}
				}

				// удаляем только те, которые разбились
				if (tmpProjectile->ProjectileType != 2) {
					DestroyProjectileWithExplosion(tmpProjectile, IntercPoint);
					delete tmpProjectile;
					tmpProjectile = nullptr;
				}
			}

			tmpProjectile = tmpProjectile2;
		}

		// cSpaceObject на столкновения только по Сфера-Сфера
		// начиная со следующего объекта, чтобы 2 раза не сравнивать
		cSpaceObject *tmpCollisionSpace1 = tmpSpace2;
		while ((tmpCollisionSpace1 != nullptr) && (tmpS != nullptr)) {
			cSpaceObject *tmpCollisionSpace2 = tmpCollisionSpace1->Next;

			// если хоть один из них уничтожаемый
			if ((NeedCheckCollision(tmpCollisionSpace1) || NeedCheckCollision(tmpS)) &&
			    vw_SphereSphereCollision(tmpS->Radius, tmpS->Location,
						     tmpCollisionSpace1->Radius, tmpCollisionSpace1->Location, tmpCollisionSpace1->PrevLocation) &&
			    vw_OBBOBBCollision(tmpS->OBB.Box, tmpS->OBB.Location, tmpS->Location, tmpS->CurrentRotationMat,
					       tmpCollisionSpace1->OBB.Box, tmpCollisionSpace1->OBB.Location, tmpCollisionSpace1->Location, tmpCollisionSpace1->CurrentRotationMat)) {
				// если попали в часть базы - просто летим в другую сторону,
				// если это обломок корабля или модели
				// и если большой астероид
				if (((tmpS->ObjectType == eObjectType::BasePart) && (tmpCollisionSpace1->ObjectType == eObjectType::ShipPart)) ||
				    ((tmpS->ObjectType == eObjectType::ShipPart) && (tmpCollisionSpace1->ObjectType == eObjectType::BasePart))) {
					if (tmpS->ObjectType == eObjectType::ShipPart)
						vw_RotatePoint(tmpS->Orientation, sVECTOR3D(0.0f,180.0f,0.0f));
					if (tmpCollisionSpace1->ObjectType == eObjectType::ShipPart)
						vw_RotatePoint(tmpCollisionSpace1->Orientation, sVECTOR3D(0.0f,180.0f,0.0f));

					goto exitN4;
				}

				// смотрим, чтобы это были не только обломки с обломками (иначе не красиво взрываются корабли)
				if ((tmpCollisionSpace1->ObjectType != eObjectType::ShipPart) ||
				    (tmpS->ObjectType != eObjectType::ShipPart)) {

					int ObjectPieceNum;

					// проверка, если это столкновение с базой - надо внимательно смотреть
					if ((tmpS->ObjectType == eObjectType::BasePart) &&
					    (!CheckHitBBMeshCollisionDetection(tmpCollisionSpace1, tmpS, &ObjectPieceNum)))
							// если не столкнулись = выходим
							goto exitN4;
					// проверка, если это столкновение с базой - надо внимательно смотреть
					if ((tmpCollisionSpace1->ObjectType == eObjectType::BasePart) &&
					    (!CheckHitBBMeshCollisionDetection(tmpS, tmpCollisionSpace1, &ObjectPieceNum)))
							// если не столкнулись = выходим
							goto exitN4;

					bool SFXplayed = false;

					if ((NeedCheckCollision(tmpCollisionSpace1)) &&
					    ((tmpCollisionSpace1->ObjectType == eObjectType::Asteroids) ||
					     (tmpCollisionSpace1->ObjectType == eObjectType::ShipPart))) {
						switch (tmpCollisionSpace1->ObjectType) {
						case eObjectType::Asteroids:
							new cSpaceExplosion(tmpCollisionSpace1, 1, tmpCollisionSpace1->Location, tmpCollisionSpace1->Speed, -1);
							break;
						case eObjectType::ShipPart:
							new cSpaceExplosion(tmpCollisionSpace1, 32, tmpCollisionSpace1->Location, tmpCollisionSpace1->Speed, -1);
							break;
						default:
							break;
						}
						SFXplayed = true;
						// проверка, можем удалить следующий (tmpSpace2), берем другой
						if (tmpCollisionSpace1 == tmpSpace2)
							tmpSpace2 = tmpSpace2->Next;
						delete tmpCollisionSpace1;// tmpCollisionSpace1 = 0;
					}

					if (NeedCheckCollision(tmpS) &&
					    ((tmpS->ObjectType == eObjectType::Asteroids) || (tmpS->ObjectType == eObjectType::ShipPart))) {
						switch (tmpS->ObjectType) {
						case eObjectType::Asteroids:
							new cSpaceExplosion(tmpS, 1, tmpS->Location, tmpS->Speed, -1, !SFXplayed);
							break;
						case eObjectType::ShipPart:
							new cSpaceExplosion(tmpS, 32, tmpS->Location, tmpS->Speed, -1, !SFXplayed);
							break;
						default:
							break;
						}
						delete tmpS;
						tmpS = nullptr;
					}
				}
			}
exitN4:
			tmpCollisionSpace1 = tmpCollisionSpace2;
		}

		tmpS = tmpSpace2;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// для cProjectile игрока проверяем со всеми
	// cProjectile врага, которые можно уничтожить
	// на попадание
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cProjectile *tmpProjectile = StartProjectile;
	while (tmpProjectile != nullptr) {
		cProjectile *tmpProjectile2 = tmpProjectile->Next;

		cProjectile *tmpProject1 = tmpProjectile2;

		while (tmpProject1 && tmpProjectile) {
			cProjectile *tmpProject2 = tmpProject1->Next;

			// делаем все тут , а не в общей процедуре, тк не обрабатываем там фларес

			if (((tmpProjectile->ObjectStatus == eObjectStatus::Enemy) && ((tmpProject1->ObjectStatus == eObjectStatus::Ally) || (tmpProject1->ObjectStatus == eObjectStatus::Player))) ||
			    // снаряды врагов - с союзниками или игроком
			    (((tmpProjectile->ObjectStatus == eObjectStatus::Ally) || (tmpProjectile->ObjectStatus == eObjectStatus::Player)) && (tmpProject1->ObjectStatus == eObjectStatus::Enemy)) ||
			    // снаряды игрока со всеми, кроме игрока
			    ((tmpProjectile->ObjectStatus != eObjectStatus::Player) && (tmpProject1->ObjectStatus == eObjectStatus::Player))) {

				// если это уничтожаемая цель...
				if ((tmpProjectile->ProjectileType == 1) &&
					// проверка на попадание
				    vw_SphereSphereCollision(tmpProjectile->Radius, tmpProjectile->Location,
							     tmpProject1->Radius, tmpProject1->Location, tmpProject1->PrevLocation) &&
				    vw_SphereAABBCollision(tmpProjectile->AABB, tmpProjectile->Location,
							   tmpProject1->Radius, tmpProject1->Location, tmpProject1->PrevLocation) &&
				    vw_SphereOBBCollision(tmpProjectile->OBB.Box, tmpProjectile->OBB.Location, tmpProjectile->Location, tmpProjectile->CurrentRotationMat,
							  tmpProject1->Radius, tmpProject1->Location, tmpProject1->PrevLocation)) {

					// если оба - ракеты, их можно уничтожить, надо проверить по OBB-OBB
					bool NeedCheck = true;
					if ((tmpProjectile->ProjectileType == 1) &&
					    (tmpProject1->ProjectileType == 1) &&
					    !vw_OBBOBBCollision(tmpProjectile->OBB.Box, tmpProjectile->OBB.Location, tmpProjectile->Location, tmpProjectile->CurrentRotationMat,
								tmpProject1->OBB.Box, tmpProject1->OBB.Location, tmpProject1->Location, tmpProject1->CurrentRotationMat))
						NeedCheck = false;

					if (NeedCheck) {
						// удаляем только те, которые разбились
						if ((tmpProject1->ProjectileType != 2) && (tmpProject1->ProjectileType != 3)) {
							// корректировка указателя
							if (tmpProjectile2 == tmpProject1) tmpProjectile2 = tmpProjectile2->Next;

							// тоже ракета
							if (tmpProject1->ProjectileType == 1) {
								tmpProject1->Speed = 0.0f;
								new cBulletExplosion(nullptr, tmpProject1, -tmpProject1->Num, tmpProject1->Location, tmpProject1->Speed);
							}
							delete tmpProject1;
							tmpProject1 = nullptr;

							// взрываем...
							tmpProjectile->Speed = 0.0f;
							new cBulletExplosion(nullptr, tmpProjectile, -tmpProjectile->Num, tmpProjectile->Location, tmpProjectile->Speed);
							// в самый последний момент - удаляем снаряд... он разбился
							delete tmpProjectile;
							tmpProjectile = nullptr;
						} else {
							// разрушаем
							new cSpaceExplosion(tmpProjectile, 4, tmpProjectile->Location, tmpProjectile->Speed, -1);
							// в самый последний момент - удаляем снаряд... он разбился
							delete tmpProjectile;
							tmpProjectile = nullptr;
						}
					}
				}

				// если не уничтожили, проверяем другие
				if (tmpProjectile && tmpProject1 &&
				    (tmpProject1->ProjectileType == 1) &&
					// проверка на попадание
				    vw_SphereSphereCollision(tmpProject1->Radius, tmpProject1->Location,
							      tmpProjectile->Radius, tmpProjectile->Location, tmpProjectile->PrevLocation) &&
				    vw_SphereAABBCollision(tmpProject1->AABB, tmpProject1->Location,
							    tmpProjectile->Radius, tmpProjectile->Location, tmpProjectile->PrevLocation) &&
				    vw_SphereOBBCollision(tmpProject1->OBB.Box, tmpProject1->OBB.Location, tmpProject1->Location, tmpProject1->CurrentRotationMat,
							   tmpProjectile->Radius, tmpProjectile->Location, tmpProjectile->PrevLocation)) {

					// если оба - ракеты, их можно уничтожить, надо проверить по OBB-OBB
					bool NeedCheck{true};
					if ((tmpProjectile->ProjectileType == 1) && (tmpProject1->ProjectileType == 1) &&
					    !vw_OBBOBBCollision(tmpProject1->OBB.Box, tmpProject1->OBB.Location, tmpProject1->Location, tmpProject1->CurrentRotationMat,
								tmpProjectile->OBB.Box, tmpProjectile->OBB.Location, tmpProjectile->Location, tmpProjectile->CurrentRotationMat))
						NeedCheck = false;

					if (NeedCheck) {
						// удаляем только те, которые разбились
						if ((tmpProjectile->ProjectileType != 2) &&
						    (tmpProjectile->ProjectileType != 3)) {
							// тоже ракета
							if (tmpProjectile->ProjectileType == 1) {
								tmpProjectile->Speed = 0.0f;
								new cBulletExplosion(nullptr, tmpProjectile, -tmpProjectile->Num, tmpProjectile->Location, tmpProjectile->Speed);
							}
							delete tmpProjectile;
							tmpProjectile = nullptr;

							// взрываем...
							tmpProject1->Speed = 0.0f;
							new cBulletExplosion(nullptr, tmpProject1, -tmpProject1->Num, tmpProject1->Location, tmpProject1->Speed);
							// корректировка указателя
							if (tmpProjectile2 == tmpProject1)
								tmpProjectile2 = tmpProjectile2->Next;
							// в самый последний момент - удаляем снаряд... он разбился
							delete tmpProject1;
							tmpProject1 = nullptr;
						} else {
							// разрушаем
							new cSpaceExplosion(tmpProject1, 4, tmpProject1->Location, tmpProject1->Speed, -1);
							// корректировка указателя
							if (tmpProjectile2 == tmpProject1)
								tmpProjectile2 = tmpProjectile2->Next;
							// в самый последний момент - удаляем снаряд... он разбился
							delete tmpProject1;
							tmpProject1 = nullptr;
						}
					}
				}
			}

			tmpProject1 = tmpProject2;
		}

		tmpProjectile = tmpProjectile2;
	}
}

//-----------------------------------------------------------------------------
// Проверка попадания в сферу Sphere-Sphere
//-----------------------------------------------------------------------------
bool CheckSphereSphereDestroyDetection(cObject3D *Object1, sVECTOR3D Point, float Radius, float *Distance2)
{
	// квадрат расстояния между объектами
	*Distance2 = (Object1->Location.x - Point.x) * (Object1->Location.x - Point.x) +
		     (Object1->Location.y - Point.y) * (Object1->Location.y - Point.y) +
		     (Object1->Location.z - Point.z) * (Object1->Location.z - Point.z);

	// если расстояние меньше или равно - значит есть столкновение по радиусам
	if (*Distance2 <= Radius * Radius)
		return true;

	// объекты очень далеко
	return false;
}

//-----------------------------------------------------------------------------
// Удаление всех удаляемых объектов, если они ближе радиуса
//-----------------------------------------------------------------------------
void DestroyRadiusCollisionAllObject3D(cObject3D *DontTouchObject, sVECTOR3D Point, float Radius, float Damage, eObjectStatus ObjectStatus)
{
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
	cSpaceObject *tmpS = StartSpaceObject;
	while (tmpS) {
		cSpaceObject *tmpSpace2 = tmpS->Next;

		if (NeedCheckCollision(tmpS) &&
		    ((((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpS->ObjectStatus == eObjectStatus::Enemy)) ||
		     ((ObjectStatus == eObjectStatus::Enemy) && ((tmpS->ObjectStatus == eObjectStatus::Ally) || (tmpS->ObjectStatus == eObjectStatus::Player)))) &&
		    (DontTouchObject != tmpS) && CheckSphereSphereDestroyDetection(tmpS, Point, Radius, &Distance2)) {
			if ((tmpS->ObjectType == eObjectType::ShipPart) &&
			    (vw_fRand() > 0.4f))
				goto NexttmpS;

			float DamageHull = Damage * (1.0f - Distance2 / (Radius * Radius));

			// отнимаем у всех по Damage
			tmpS->Strength -= DamageHull/tmpS->ResistanceHull;

			// если уже все... удаляем
			if (tmpS->Strength <= 0.0f) {
				// проверка, нужно начислять или нет
				AddPlayerBonus(tmpS, ObjectStatus);

				new cSpaceExplosion(tmpS, 1, tmpS->Location, tmpS->Speed, -1);
				delete tmpS;
				tmpS = nullptr;
			}
		}

NexttmpS:
		tmpS = tmpSpace2;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверка для всех кораблей
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cSpaceShip *tmpShip = StartSpaceShip;
	while (tmpShip) {
		cSpaceShip *tmpShip2 = tmpShip->Next;

		if (NeedCheckCollision(tmpShip) &&
		    (((((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip->ObjectStatus == eObjectStatus::Enemy)) ||
		      ((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip->ObjectStatus == eObjectStatus::Ally) || (tmpShip->ObjectStatus == eObjectStatus::Player)))) &&
		     (DontTouchObject != tmpShip) && CheckSphereSphereDestroyDetection(tmpShip, Point, Radius, &Distance2))) {

			float DamageHull = Damage * (1.0f - Distance2 / (Radius * Radius));

			// просто убираем щит
			tmpShip->ShieldStrength = 0.0f;

			// отнимаем у всех по DamageHull
			tmpShip->Strength -= DamageHull / tmpShip->ResistanceHull;

			// если уже все... удаляем
			if (tmpShip->Strength <= 0.0f) {
				// проверка, нужно начислять или нет
				AddPlayerBonus(tmpShip, ObjectStatus);

				// если не корабль игрока! его удалим сами
				if (tmpShip->ObjectStatus != eObjectStatus::Player) {
					switch (tmpShip->ObjectType) {
					case eObjectType::AlienFighter:
						new cSpaceExplosion(tmpShip, 2, tmpShip->Location, tmpShip->Speed, -1);
						break;
					case eObjectType::EarthFighter:
						new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, -1);
						break;
					case eObjectType::AlienMotherShip:
						new cSpaceExplosion(tmpShip, 33, tmpShip->Location, tmpShip->Speed, 0);
						break;
					case eObjectType::PirateShip:
						if (tmpShip->InternalType <= 5)
							new cSpaceExplosion(tmpShip, 3, tmpShip->Location, tmpShip->Speed, -1);
						else
							new cSpaceExplosion(tmpShip, 31, tmpShip->Location, tmpShip->Speed, 0);
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
	// проверяем все cGroundObject с
	// cProjectile
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	cGroundObject *tmpG = StartGroundObject;
	while (tmpG) {
		cGroundObject *tmpGround2 = tmpG->Next;

		if (NeedCheckCollision(tmpG) &&
		    (((((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpG->ObjectStatus == eObjectStatus::Enemy)) ||
		      ((ObjectStatus == eObjectStatus::Enemy) && ((tmpG->ObjectStatus == eObjectStatus::Ally) || (tmpG->ObjectStatus == eObjectStatus::Player)))) &&
		     (DontTouchObject != tmpG) && CheckSphereSphereDestroyDetection(tmpG, Point, Radius, &Distance2))) {
			float DamageHull = Damage * (1.0f - Distance2 / (Radius * Radius));

			// отнимаем у всех по Damage
			tmpG->Strength -= DamageHull / tmpG->ResistanceHull;

			// если уже все... удаляем
			if (tmpG->Strength <= 0.0f) {
				// проверка, нужно начислять или нет
				AddPlayerBonus(tmpG, ObjectStatus);

				switch (tmpG->ObjectType) {
				case eObjectType::PirateBuilding:
					new cGroundExplosion(tmpG, 1, tmpG->Location, -1);
					break;
				case eObjectType::PirateVehicle:
					new cGroundExplosion(tmpG, 2, tmpG->Location, -1);
					break;
				default:
					break;
				}

				delete tmpG;
				tmpG = nullptr;
			}
		}

		tmpG = tmpGround2;
	}

	// снаряды не учитываем, иначе - поправь указатели, чтобы при удалении
	// следующего, не было сбоя... при обновлении указателя у первого
}
