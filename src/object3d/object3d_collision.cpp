/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

// TODO codestyle should be fixed

#include "object3d.h"
#include "collision_detection.h"
#include "../config/config.h"
#include "../assets/audio.h"
#include "../game/camera.h"
#include "../game/hud.h"
#include "space_ship/space_ship.h"
#include "ground_object/ground_object.h"
#include "projectile/projectile.h"
#include "space_object/space_object.h"
#include "explosion/explosion.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, don't allow global scope interaction for local variables
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

extern float ShildEnergyStatus;
extern float ShildStartHitStatus;
extern int PlayerDeadObjectPieceNum;


/*
 * Calculate bonus and experience for killed enemy.
 */
static void AddBonusForKilledEnemy(const cObject3D &Object, eObjectStatus KilledBy)
{
    if (KilledBy != eObjectStatus::Player
        || Object.ObjectStatus != eObjectStatus::Enemy
        || Object.ArmorInitialStatus <= 0.0f) {
        return;
    }

    // TODO probably, we should calculate "static part" on mission start only
    float tmpBonus = Object.ArmorInitialStatus * GameEnemyArmorPenalty;
    if (GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
        tmpBonus /= 2.0f * GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission];
    }
    float tmpExperience = Object.ArmorInitialStatus * GameEnemyArmorPenalty * ProfileDifficulty(CurrentProfile) / 100.0f;

    switch (Object.ObjectType) {
    case eObjectType::AlienFighter:
        tmpBonus /= 1.8f;
        tmpExperience /= 1.8f;
        AlienShipsKillBonus += tmpBonus;
        AlienShipsKillQuant++;
        break;

    case eObjectType::AlienMotherShip:
        tmpBonus /= 1.8f;
        tmpExperience /= 1.8f;
        AlienMotherShipsKillBonus += tmpBonus;
        AlienMotherShipsKillQuant++;
        break;

    case eObjectType::PirateShip:
        tmpBonus /= 1.8f;
        tmpExperience /= 1.8f;
        PirateShipsKillBonus += tmpBonus;
        PirateShipsKillQuant++;
        break;

    case eObjectType::PirateVehicle:
        tmpBonus /= 1.8f;
        tmpExperience /= 1.8f;
        PirateVehiclesKillBonus += tmpBonus;
        PirateVehiclesKillQuant++;
        break;

    case eObjectType::PirateBuilding:
        tmpBonus /= 1.8f;
        tmpExperience /= 1.8f;
        PirateBuildingsKillBonus += tmpBonus;
        PirateBuildingsKillQuant++;
        break;

    case eObjectType::SmallAsteroid:
        tmpBonus /= 8.0f;
        tmpExperience /= 8.0f;
        AsteroidsKillBonus += tmpBonus;
        AsteroidsKillQuant++;
        break;

#ifndef NDEBUG
    // don't use 'default' case here, we need compiler's warning if some type was missed
    case eObjectType::none:
    case eObjectType::EarthFighter:
    case eObjectType::SpaceDebris:
    case eObjectType::ShipWeapon:
    case eObjectType::Explosion:
    case eObjectType::CivilianBuilding:
    case eObjectType::BasePart:
    case eObjectType::Planet:
    case eObjectType::Planetoid:
    case eObjectType::BigAsteroid:
    case eObjectType::Projectile:
        return;
#else
    default:
        return;
#endif // NDEBUG
    }

    GameMoney += tmpBonus;
    GameExperience += tmpExperience;

    SetupHUDText(static_cast<int>(GameExperience), static_cast<int>(GameMoney));
}

/*
 * Setup space object explosion.
 */
static void SetupSpaceExplosion(cSpaceObject &SpaceObject)
{
    switch (SpaceObject.ObjectType) {
    case eObjectType::SmallAsteroid:
        CreateSpaceExplosion(SpaceObject, 1, SpaceObject.Location, SpaceObject.Speed, -1);
        break;
    case eObjectType::SpaceDebris:
        CreateSpaceExplosion(SpaceObject, 32, SpaceObject.Location, SpaceObject.Speed, -1);
        break;
    default:
        break;
    }
}

/*
 * Setup space ship explosion.
 */
static void SetupSpaceShipExplosion(cSpaceShip &SpaceShip, int ObjectChunkNum)
{
    switch (SpaceShip.ObjectType) {
    case eObjectType::AlienFighter:
        CreateSpaceExplosion(SpaceShip, 2, SpaceShip.Location, SpaceShip.Speed, ObjectChunkNum);
        break;
    case eObjectType::EarthFighter:
        CreateSpaceExplosion(SpaceShip, 31, SpaceShip.Location, SpaceShip.Speed, ObjectChunkNum);
        break;
    case eObjectType::AlienMotherShip:
        if (ObjectChunkNum == -1) {
            ObjectChunkNum = 0; // FIXME should explode main biggest element for best effect
        }
        CreateSpaceExplosion(SpaceShip, 33, SpaceShip.Location, SpaceShip.Speed, ObjectChunkNum);
        break;
    case eObjectType::PirateShip:
        if (SpaceShip.InternalType <= 5) {
            CreateSpaceExplosion(SpaceShip, 3, SpaceShip.Location, SpaceShip.Speed, ObjectChunkNum);
        } else {
            if (ObjectChunkNum == -1) {
                ObjectChunkNum = 0; // FIXME should explode main biggest element for best effect
            }
            CreateSpaceExplosion(SpaceShip, 31, SpaceShip.Location, SpaceShip.Speed, ObjectChunkNum);
        }
        break;
    default:
        break;
    }
}

/*
 * Setup ground object explosion.
 */
static void SetupGroundExplosion(cGroundObject &GroundObject, int ObjectChunkNum)
{
    switch (GroundObject.ObjectType) {
    case eObjectType::PirateBuilding:
        CreateGroundExplosion(GroundObject, 1, GroundObject.Location, ObjectChunkNum);
        break;
    case eObjectType::PirateVehicle:
        CreateGroundExplosion(GroundObject, 2, GroundObject.Location, ObjectChunkNum);
        break;
    default:
        break;
    }
}

/*
 * Check distance between points in space.
 * Return square of the distance factor for future calculations.
 */
static bool CheckDistanceBetweenPoints(const sVECTOR3D &Point1, const sVECTOR3D &Point2,
                                       float Distance2, float &Distance2Factor)
{
    float RealDistance2 = (Point1.x - Point2.x) * (Point1.x - Point2.x) +
                          (Point1.y - Point2.y) * (Point1.y - Point2.y) +
                          (Point1.z - Point2.z) * (Point1.z - Point2.z);

    if (RealDistance2 <= Distance2) {
        Distance2Factor = RealDistance2 / Distance2;
        return true;
    }

    // we don't need real square of the distance factor in this case
    Distance2Factor = 1.0f;
    return false;
}

/*
 * Damage all near objects by shock wave.
 */
static void DamageAllNearObjectsByShockWave(const cObject3D &DontTouchObject, const sVECTOR3D &Epicenter,
        float Radius2, float Damage, eObjectStatus ExplosionStatus)
{
    // FIXME
    // we don't destroy projectiles (missiles/bombs/torpedos) since we could have an issue with
    // ForEachProjectile()/ForEachProjectilePair(), just make sure you don't destroy bomb/torpedo
    // itself here, but not only DontTouchObject

    // reduce shock wave damage to 75%, let bomb's/torpedo's hit damage more than shock wave
    Damage = Damage * 0.75f;
    // we need take into account distance factor for damage calculation
    float Distance2Factor;

    ForEachSpaceObject([&] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
        if (NeedCheckCollision(tmpSpace)
            && ObjectsStatusFoe(ExplosionStatus, tmpSpace.ObjectStatus)
            && &DontTouchObject != &tmpSpace
            && CheckDistanceBetweenPoints(tmpSpace.Location, Epicenter, Radius2, Distance2Factor)) {
            // debris is a part of scene, don't let them all explode by only one shock wave
            if (tmpSpace.ObjectType == eObjectType::SpaceDebris && vw_fRand() > 0.5f) {
                return; // eSpaceCycle::Continue;
            }

            tmpSpace.ArmorCurrentStatus -= Damage * (1.0f - Distance2Factor);

            if (tmpSpace.ArmorCurrentStatus <= 0.0f) {
                AddBonusForKilledEnemy(tmpSpace, ExplosionStatus);
                SetupSpaceExplosion(tmpSpace);
                SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
            }
        }
    });

    ForEachSpaceShip([&] (cSpaceShip &tmpShip, eShipCycle &ShipCycleCommand) {
        if (NeedCheckCollision(tmpShip)
            && ObjectsStatusFoe(ExplosionStatus, tmpShip.ObjectStatus)
            && &DontTouchObject != &tmpShip
            && CheckDistanceBetweenPoints(tmpShip.Location, Epicenter, Radius2, Distance2Factor)) {

            tmpShip.ShieldCurrentStatus = 0.0f; // EMP with bomb/torpedo explosion should reduce shields to 0
            tmpShip.ArmorCurrentStatus -= Damage * (1.0f - Distance2Factor);

            if (tmpShip.ArmorCurrentStatus <= 0.0f && tmpShip.ObjectStatus != eObjectStatus::Player) {
                AddBonusForKilledEnemy(tmpShip, ExplosionStatus);
                SetupSpaceShipExplosion(tmpShip, -1);
                ShipCycleCommand = eShipCycle::DeleteObjectAndContinue;
            }
        }
    });

    ForEachGroundObject([&] (cGroundObject &tmpGround, eGroundCycle &GroundCycleCommand) {
        if (NeedCheckCollision(tmpGround)
            && ObjectsStatusFoe(ExplosionStatus, tmpGround.ObjectStatus)
            && &DontTouchObject != &tmpGround
            && CheckDistanceBetweenPoints(tmpGround.Location, Epicenter, Radius2, Distance2Factor)) {

            tmpGround.ArmorCurrentStatus -= Damage * (1.0f - Distance2Factor);

            if (tmpGround.ArmorCurrentStatus <= 0.0f) {
                AddBonusForKilledEnemy(tmpGround, ExplosionStatus);
                SetupGroundExplosion(tmpGround, -1);
                GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
            }
        }
    });
}

/*
 * Interact with player's shield.
 */
static bool InteractWithPlayerShield(const cDamage &ProjectileDamage, cDamage &Damage)
{
    float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
    CurrentStatus -= ProjectileDamage.Kinetic() / 5.0f;
    CurrentStatus -= ProjectileDamage.EM() * 2.0f;
    if (CurrentStatus < 0.0f) {
        CurrentStatus = 0.0f;
    }
    ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

    // 0 damage - is correct, we do all work with player's shield here now...
    Damage = 0.0f;
    // we already absorb projectile damage by shield, but we need destroy projectile
    return true;
}

/*
 * Interact with player's deflector.
 */
static bool InteractWithPlayerDeflector(eObjectStatus ObjectStatus, cProjectile &Projectile)
{
    Projectile.ObjectStatus = ObjectStatus;
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

    float CurrentStatus = ShildEnergyStatus * ShildStartHitStatus;
    CurrentStatus -= Projectile.Damage.Kinetic() / 5.0f;
    CurrentStatus -= Projectile.Damage.EM() * 2.0f;
    if (CurrentStatus < 0.0f) {
        CurrentStatus = 0.0f;
    }
    ShildEnergyStatus = CurrentStatus / ShildStartHitStatus;

    Projectile.Damage *= GameEnemyWeaponPenalty;
    // we don't need destroy projectile, we just now deflect it
    return false;
}

/*
 * Detect projectile collision.
 */
// FIXME this one should be fixed after we move to player ship's class (refactor object classes)
static bool DetectProjectileCollision(const cObject3D &Object, int &ObjectPieceNum, cProjectile &Projectile,
                                      cDamage &Damage, float ObjectSpeed)
{
    if (!ObjectsStatusFoe(Object.ObjectStatus, Projectile.ObjectStatus) && NeedCheckCollision(Object)) {
        return false;
    }

    if (Object.ObjectStatus == eObjectStatus::Player) {
        ObjectSpeed += GetCameraSpeed();
    }

    switch (Projectile.ProjectileType) {
    case 0: // projectile
        // player's ship with charged deflector/shield
        if (ShildEnergyStatus * ShildStartHitStatus >= Projectile.Damage.Full() * GameEnemyWeaponPenalty
            && Object.ObjectStatus == eObjectStatus::Player) {
            if (vw_SphereSphereCollision(Object.Radius, Object.Location, Projectile.Radius,
                                         Projectile.Location, Projectile.PrevLocation)) {
                if (GameAdvancedProtectionSystem == 4) { // player's ship with charged deflector
                    return InteractWithPlayerDeflector(Object.ObjectStatus, Projectile);
                } else {
                    CreateBulletExplosion(&Object, Projectile, Projectile.Num, Projectile.Location, ObjectSpeed);
                    return InteractWithPlayerShield(Projectile.Damage, Damage);
                }
            }
        } else {
            sVECTOR3D CollisionPoint;
            if (vw_SphereSphereCollision(Object.Radius, Object.Location,
                                         Projectile.Radius, Projectile.Location, Projectile.PrevLocation)
                && vw_SphereAABBCollision(Object.AABB, Object.Location,
                                          Projectile.Radius, Projectile.Location, Projectile.PrevLocation)
                && vw_SphereOBBCollision(Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat,
                                         Projectile.Radius, Projectile.Location, Projectile.PrevLocation)
                && CheckMeshSphereCollisionDetection(Object, Projectile, CollisionPoint, ObjectPieceNum)) {
                if (NeedCheckCollision(Object)) {
                    Damage = Projectile.Damage;
                    CreateBulletExplosion(&Object, Projectile, Projectile.Num, CollisionPoint, ObjectSpeed);
                } else {
                    Damage = 0.0f;
                    CreateBulletExplosion(&Object, Projectile, Projectile.Num, CollisionPoint, 0.0f);
                }
                return true;
            }
        }
        break;

    case 1: // projectile with 3d model
        // player's ship with charged deflector/shield
        if (ShildEnergyStatus * ShildStartHitStatus >= Projectile.Damage.Full() * GameEnemyWeaponPenalty
            && Object.ObjectStatus == eObjectStatus::Player) {
            if (vw_SphereSphereCollision(Object.Radius, Object.Location,
                                         Projectile.Radius, Projectile.Location, Projectile.PrevLocation)) {
                CreateBulletExplosion(&Object, Projectile, -Projectile.Num, Projectile.Location, ObjectSpeed);
                return InteractWithPlayerShield(Projectile.Damage, Damage);
            }
        } else {
            sVECTOR3D CollisionPoint;
            if (vw_SphereSphereCollision(Object.Radius, Object.Location,
                                         Projectile.Radius, Projectile.Location, Projectile.PrevLocation)
                && vw_SphereAABBCollision(Object.AABB, Object.Location,
                                          Projectile.Radius, Projectile.Location, Projectile.PrevLocation)
                && vw_SphereOBBCollision(Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat,
                                         Projectile.Radius, Projectile.Location, Projectile.PrevLocation)
                && vw_OBBOBBCollision(Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat,
                                      Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat)
                && CheckHitBBOBBCollisionDetection(Object, Projectile, ObjectPieceNum)
                && CheckMeshSphereCollisionDetection(Object, Projectile, CollisionPoint, ObjectPieceNum)) {

                switch (Projectile.Num) {
                case 18: // torpedo
                case 209: // pirate torpedo
                    DamageAllNearObjectsByShockWave(Object, Projectile.Location, 75.0f * 75.0f,
                                                    Projectile.Damage.Kinetic(), Projectile.ObjectStatus);
                    break;
                case 19: // bomb
                case 210: // pirate bomb
                    DamageAllNearObjectsByShockWave(Object, Projectile.Location, 150.0f * 150.0f,
                                                    Projectile.Damage.Kinetic(), Projectile.ObjectStatus);
                    break;
                default:
                    break;
                }

                if (NeedCheckCollision(Object)) {
                    Damage = Projectile.Damage;
                    CreateBulletExplosion(&Object, Projectile, Projectile.Num, Projectile.Location, Projectile.Speed);
                } else {
                    Damage = 0.0f;
                    CreateBulletExplosion(&Object, Projectile, Projectile.Num, Projectile.Location, 0.0f);
                }
                return true;
            }
        }
        break;

    case 2: // beam
        // player's ship with charged deflector/shield
        if (ShildEnergyStatus * ShildStartHitStatus >= Projectile.Damage.Full() * Object.TimeDelta * GameEnemyWeaponPenalty
            && Object.ObjectStatus == eObjectStatus::Player) {
            // note, we use Projectile as first object in tests - this is correct in case of beam
            if (vw_SphereOBBCollision(Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat,
                                      Object.Radius, Object.Location, Object.PrevLocation)) {
                return InteractWithPlayerShield(Projectile.Damage, Damage);
            }
        } else if (vw_AABBAABBCollision(Object.AABB, Object.Location, Projectile.AABB, Projectile.Location)
                   // note, we use Projectile as first object in tests - this is correct in case of beam
                   && vw_SphereOBBCollision(Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat,
                                            Object.Radius, Object.Location, Object.PrevLocation)
                   && vw_OBBOBBCollision(Projectile.OBB.Box, Projectile.OBB.Location, Projectile.Location, Projectile.CurrentRotationMat,
                                         Object.OBB.Box, Object.OBB.Location, Object.Location, Object.CurrentRotationMat)
                   && CheckHitBBOBBCollisionDetection(Object, Projectile, ObjectPieceNum)) {

            Damage = Projectile.Damage * Object.TimeDelta;
            return true;
        }
        break;
    }

    return false;
}

/*
 * Collision detection for all 3D objects.
 */
void DetectCollisionAllObject3D()
{
    ForEachSpaceShip([] (cSpaceShip &tmpShip, eShipCycle &ShipCycleCommand) {
        ForEachProjectile([&tmpShip, &ShipCycleCommand] (cProjectile &tmpProjectile, eProjectileCycle &ProjectileCycleCommand) {
            cDamage Damage;
            int ObjectPieceNum;

            if (DetectProjectileCollision(tmpShip, ObjectPieceNum, tmpProjectile, Damage, tmpShip.Speed)) {
                // we don't provide any global "resistance" concept for all 3D objects,
                // reduce damage for player ship only in case of "a-b layer" installed
                if (tmpShip.ObjectStatus == eObjectStatus::Player && GameAdvancedProtectionSystem == 2) {
                    Damage.Kinetic() /= 4.0f;
                    Damage.EM() /= 5.0f;
                }

                if (tmpShip.ShieldCurrentStatus >= Damage.EM()) {
                    tmpShip.ShieldCurrentStatus -= Damage.EM();
                    Damage.EM() = 0.0f;
                } else {
                    Damage.EM() -= tmpShip.ShieldCurrentStatus;
                    tmpShip.ShieldCurrentStatus = 0.0f;
                }

                if (tmpShip.ShieldCurrentStatus >= Damage.Kinetic()) {
                    tmpShip.ShieldCurrentStatus -= Damage.Kinetic();
                    Damage.Kinetic() = 0.0f;
                } else {
                    Damage.Kinetic() -= tmpShip.ShieldCurrentStatus;
                    tmpShip.ShieldCurrentStatus = 0.0f;
                }

                if (tmpShip.ShieldCurrentStatus < 0.0f) {
                    tmpShip.ShieldCurrentStatus = 0.0f;
                }

                tmpShip.ArmorCurrentStatus -= Damage.Kinetic();
                // let EM occasionally corrupt armor in some way
                tmpShip.ArmorCurrentStatus -= Damage.EM() * vw_fRand();

                // since AlienFighter is "energy", we have a chance kill it by EM
                if (Damage.EM() > 0.0f
                    && tmpShip.ObjectType == eObjectType::AlienFighter
                    && vw_fRand() > 0.7f) {
                    tmpShip.ArmorCurrentStatus = 0.0f;
                }

                if (tmpShip.ArmorCurrentStatus <= 0.0f) {
                    AddBonusForKilledEnemy(tmpShip, tmpProjectile.ObjectStatus);
                    if (tmpShip.ObjectStatus != eObjectStatus::Player) {
                        SetupSpaceShipExplosion(tmpShip, ObjectPieceNum);
                        ShipCycleCommand = eShipCycle::DeleteObjectAndContinue;
                    } else {
                        PlayerDeadObjectPieceNum = ObjectPieceNum;
                    }
                } else if (tmpShip.ObjectStatus != eObjectStatus::Player
                           && tmpShip.ObjectType != eObjectType::AlienMotherShip
                           && Damage.EM() > 0.0f) {

                    float Rand = vw_fRand();

                    if (Rand > 0.7f) {
                        tmpShip.MaxSpeed = tmpShip.MaxSpeed / 2.0f;
                    } else if (Rand > 0.5f) {
                        tmpShip.MaxSpeedRotate = tmpShip.MaxSpeedRotate / 2.0f;
                    } else if (Rand > 0.2f && !tmpShip.FlareWeaponSlots.empty()) {
                        tmpShip.FlareWeaponSlots.clear();
                    }
                }

                if (tmpProjectile.ProjectileType != 2) {
                    ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndContinue;
                }

                if (ShipCycleCommand == eShipCycle::DeleteObjectAndContinue) {
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

            // player's ship weapons
            if (tmpShip.ObjectStatus == eObjectStatus::Player
                && tmpProjectile.ObjectStatus == eObjectStatus::Enemy
                && !GameUndestroyableWeapon
                && !tmpShip.WeaponSlots.empty()) {
                for (auto &tmpWeaponSlot : tmpShip.WeaponSlots) {
                    if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                        int ObjectPieceNumWeapon;

                        if (sharedWeapon->ArmorCurrentStatus > 0.0f
                            && DetectProjectileCollision(*sharedWeapon, ObjectPieceNumWeapon, tmpProjectile, Damage, tmpShip.Speed)) {
                            // FIXME should be fixed, since tmpShip may be destroyed in DamageAllNearObjectsByShockWave()

                            // note, we don't really destroy this weapon here
                            sharedWeapon->ArmorCurrentStatus -= Damage.Kinetic();
                            if (sharedWeapon->ArmorCurrentStatus <= 0.0f) {
                                sharedWeapon->ArmorCurrentStatus = 0.0f;
                                PlayVoicePhrase(eVoicePhrase::WeaponDestroyed, 1.0f);
                            } else {
                                PlayVoicePhrase(eVoicePhrase::WeaponDamaged, 1.0f);
                            }

                            if (tmpProjectile.ProjectileType != 2) {
                                ProjectileCycleCommand = eProjectileCycle::DeleteObjectAndContinue;
                                return;
                            }
                        }
                    }
                }
            }
        });
        if (ShipCycleCommand == eShipCycle::DeleteObjectAndContinue) {
            return;
        }

        ForEachSpaceObject([&tmpShip, &ShipCycleCommand] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
            int ObjectPieceNum;
            if (vw_SphereSphereCollision(tmpShip.Radius, tmpShip.Location,
                                         tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && vw_SphereAABBCollision(tmpShip.AABB, tmpShip.Location,
                                          tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && vw_SphereOBBCollision(tmpShip.OBB.Box, tmpShip.OBB.Location, tmpShip.Location, tmpShip.CurrentRotationMat,
                                         tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && CheckHitBBOBBCollisionDetection(tmpShip, tmpSpace, ObjectPieceNum)) {

                if ((tmpSpace.ObjectType == eObjectType::BasePart || tmpSpace.ObjectType == eObjectType::BigAsteroid)
                    && !CheckHitBBMeshCollisionDetection(tmpShip, tmpSpace, ObjectPieceNum)) {
                    return; // eSpaceCycle::Continue
                }

                if (!NeedCheckCollision(tmpSpace)) {
                    // we just get into the immortal object here, but don't destroy ship instantly
                    if (tmpShip.ObjectStatus != eObjectStatus::Player) {
                        tmpShip.ArmorCurrentStatus -= (tmpShip.ArmorInitialStatus / 0.5f) * tmpShip.TimeDelta;
                    } else {
                        tmpShip.ArmorCurrentStatus -= (tmpShip.ArmorInitialStatus / 2.0f) * tmpShip.TimeDelta;
                    }
                } else {
                    float StrTMP = tmpShip.ArmorCurrentStatus;
                    tmpShip.ArmorCurrentStatus -= tmpSpace.ArmorCurrentStatus;
                    tmpSpace.ArmorCurrentStatus -= StrTMP;
                }
                if (!NeedCheckCollision(tmpShip)) {
                    tmpSpace.ArmorCurrentStatus = 0.0f;
                }

                if (NeedCheckCollision(tmpSpace) && tmpSpace.ArmorCurrentStatus <= 0.0f) {
                    AddBonusForKilledEnemy(tmpSpace, tmpShip.ObjectStatus);
                    SetupSpaceExplosion(tmpSpace);
                    SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
                }

                if (NeedCheckCollision(tmpShip) && tmpShip.ArmorCurrentStatus <= 0.0f) {
                    if (tmpShip.ObjectStatus != eObjectStatus::Player) {
                        SetupSpaceShipExplosion(tmpShip, ObjectPieceNum);
                        ShipCycleCommand = eShipCycle::DeleteObjectAndContinue;

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
                        PlayerDeadObjectPieceNum = ObjectPieceNum;
                    }
                }
            }
        });
        if (ShipCycleCommand == eShipCycle::DeleteObjectAndContinue) {
            return;
        }

        ForEachGroundObject([&tmpShip, &ShipCycleCommand] (cGroundObject &tmpGround, eGroundCycle &GroundCycleCommand) {
            int ObjectPieceNum1;
            int ObjectPieceNum2;
            if (vw_SphereSphereCollision(tmpShip.Radius, tmpShip.Location,
                                         tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation)
                && vw_SphereAABBCollision(tmpShip.AABB, tmpShip.Location,
                                          tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation)
                && vw_SphereOBBCollision(tmpShip.OBB.Box, tmpShip.OBB.Location, tmpShip.Location, tmpShip.CurrentRotationMat,
                                         tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation)
                && CheckHitBBHitBBCollisionDetection(tmpShip, tmpGround, ObjectPieceNum1, ObjectPieceNum2)) {

                if (tmpGround.ObjectType == eObjectType::CivilianBuilding
                    && !CheckHitBBMeshCollisionDetection(tmpShip, tmpGround, ObjectPieceNum2)) {
                    return; // eGroundCycle::Continue
                }

                if (!NeedCheckCollision(tmpGround)) {
                    // we just get into the immortal object here, but don't destroy ship instantly
                    if (tmpShip.ObjectStatus != eObjectStatus::Player) {
                        tmpShip.ArmorCurrentStatus -= (tmpShip.ArmorInitialStatus / 0.5f) * tmpShip.TimeDelta;
                    } else {
                        tmpShip.ArmorCurrentStatus -= (tmpShip.ArmorInitialStatus / 2.0f) * tmpShip.TimeDelta;
                    }
                } else {
                    float StrTMP = tmpShip.ArmorCurrentStatus;
                    tmpShip.ArmorCurrentStatus -= tmpGround.ArmorCurrentStatus;
                    tmpGround.ArmorCurrentStatus -= StrTMP;
                }
                if (!NeedCheckCollision(tmpShip)) {
                    tmpGround.ArmorCurrentStatus = 0.0f;
                }

                if (NeedCheckCollision(tmpGround) && tmpGround.ArmorCurrentStatus <= 0.0f) {
                    AddBonusForKilledEnemy(tmpGround, tmpShip.ObjectStatus);
                    SetupGroundExplosion(tmpGround, ObjectPieceNum2);
                    GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
                }

                if (NeedCheckCollision(tmpShip) && tmpShip.ArmorCurrentStatus <= 0.0f) {
                    if (tmpShip.ObjectStatus != eObjectStatus::Player) {
                        SetupSpaceShipExplosion(tmpShip, ObjectPieceNum1);
                        ShipCycleCommand = eShipCycle::DeleteObjectAndContinue;

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
                        PlayerDeadObjectPieceNum = ObjectPieceNum1;
                    }
                }
            }
        });
    });

    ForEachSpaceShipPair([] (cSpaceShip &FirstShip, cSpaceShip &SecondShip, eShipPairCycle &Command) {
        int ObjectPieceNum1;
        int ObjectPieceNum2;
        if (vw_SphereSphereCollision(FirstShip.Radius, FirstShip.Location,
                                     SecondShip.Radius, SecondShip.Location, SecondShip.PrevLocation)
            && vw_AABBAABBCollision(FirstShip.AABB, FirstShip.Location, SecondShip.AABB, SecondShip.Location)
            && vw_OBBOBBCollision(FirstShip.OBB.Box, FirstShip.OBB.Location, FirstShip.Location, FirstShip.CurrentRotationMat,
                                  SecondShip.OBB.Box, SecondShip.OBB.Location, SecondShip.Location, SecondShip.CurrentRotationMat)
            && CheckHitBBHitBBCollisionDetection(FirstShip, SecondShip, ObjectPieceNum1, ObjectPieceNum2)) {

            float StrTMP = FirstShip.ArmorCurrentStatus;
            FirstShip.ArmorCurrentStatus -= SecondShip.ArmorCurrentStatus;
            SecondShip.ArmorCurrentStatus -= StrTMP;

            if (!NeedCheckCollision(SecondShip)) {
                FirstShip.ArmorCurrentStatus = 0.0f;
            }
            if (!NeedCheckCollision(FirstShip)) {
                SecondShip.ArmorCurrentStatus = 0.0f;
            }

            if (NeedCheckCollision(SecondShip) && SecondShip.ArmorCurrentStatus <= 0.0f) {
                AddBonusForKilledEnemy(SecondShip, FirstShip.ObjectStatus);
            }
            if (NeedCheckCollision(FirstShip) && FirstShip.ArmorCurrentStatus <= 0.0f) {
                AddBonusForKilledEnemy(FirstShip, SecondShip.ObjectStatus);
            }

            if (NeedCheckCollision(SecondShip) && SecondShip.ArmorCurrentStatus <= 0.0f) {
                if (SecondShip.ObjectStatus != eObjectStatus::Player) {
                    SetupSpaceShipExplosion(SecondShip, ObjectPieceNum2);
                    Command = eShipPairCycle::DeleteSecondObjectAndContinue;
                } else {
                    PlayerDeadObjectPieceNum = ObjectPieceNum2;
                }
            }

            if (NeedCheckCollision(FirstShip) && FirstShip.ArmorCurrentStatus <= 0.0f) {
                if (FirstShip.ObjectStatus != eObjectStatus::Player) {
                    SetupSpaceShipExplosion(FirstShip, ObjectPieceNum1);
                    if (Command == eShipPairCycle::DeleteSecondObjectAndContinue) {
                        Command = eShipPairCycle::DeleteBothObjectsAndContinue;
                    } else {
                        Command = eShipPairCycle::DeleteFirstObjectAndContinue;
                    }
                } else {
                    PlayerDeadObjectPieceNum = ObjectPieceNum1;
                }
            }
        }
    });

    ForEachGroundObject([] (cGroundObject &tmpGround, eGroundCycle &GroundCycleCommand) {
        ForEachProjectile([&tmpGround, &GroundCycleCommand] (cProjectile &tmpProjectile, eProjectileCycle &ProjectileCycleCommand) {
            cDamage Damage;
            int ObjectPieceNum;

            if (DetectProjectileCollision(tmpGround, ObjectPieceNum, tmpProjectile, Damage, tmpGround.Speed)) {
                if (NeedCheckCollision(tmpGround)) {
                    tmpGround.ArmorCurrentStatus -= Damage.Kinetic();

                    // FIXME let EM occasionally corrupt armor in some way (see space ship code above)

                    if (tmpGround.ArmorCurrentStatus <= 0.0f) {
                        AddBonusForKilledEnemy(tmpGround, tmpProjectile.ObjectStatus);
                        SetupGroundExplosion(tmpGround, ObjectPieceNum);
                        GroundCycleCommand = eGroundCycle::DeleteObjectAndContinue;
                    }
                }

                if (tmpProjectile.ProjectileType != 2) {
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
        if (GroundCycleCommand == eGroundCycle::DeleteObjectAndContinue) {
            return;
        }

        ForEachSpaceObject([&tmpGround, &GroundCycleCommand] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
            int ObjectPieceNum;

            if (tmpSpace.ObjectType != eObjectType::BasePart
                && (NeedCheckCollision(tmpGround) || NeedCheckCollision(tmpSpace))
                && vw_SphereSphereCollision(tmpGround.Radius, tmpGround.Location,
                                            tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && vw_SphereAABBCollision(tmpGround.AABB, tmpGround.Location,
                                          tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && vw_SphereOBBCollision(tmpGround.OBB.Box, tmpGround.OBB.Location, tmpGround.Location, tmpGround.CurrentRotationMat,
                                         tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && CheckHitBBOBBCollisionDetection(tmpGround, tmpSpace, ObjectPieceNum)) {

                if (!NeedCheckCollision(tmpSpace)) {
                    // we just get into the immortal object here, but don't destroy space object instantly
                    tmpGround.ArmorCurrentStatus -= (tmpGround.ArmorInitialStatus / 0.5f) * tmpGround.TimeDelta;
                } else {
                    float StrTMP = tmpGround.ArmorCurrentStatus;
                    tmpGround.ArmorCurrentStatus -= tmpSpace.ArmorCurrentStatus;
                    tmpSpace.ArmorCurrentStatus -= StrTMP;
                }
                if (!NeedCheckCollision(tmpGround)) {
                    tmpSpace.ArmorCurrentStatus = 0.0f;
                }

                if (NeedCheckCollision(tmpSpace) && tmpSpace.ArmorCurrentStatus <= 0.0f) {
                    AddBonusForKilledEnemy(tmpSpace, tmpGround.ObjectStatus);
                    SetupSpaceExplosion(tmpSpace);
                    SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
                }

                if (NeedCheckCollision(tmpGround) && tmpGround.ArmorCurrentStatus <= 0.0f) {
                    SetupGroundExplosion(tmpGround, ObjectPieceNum);
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

    ForEachSpaceObject([] (cSpaceObject &tmpSpace, eSpaceCycle &SpaceCycleCommand) {
        ForEachProjectile([&tmpSpace, &SpaceCycleCommand] (cProjectile &tmpProjectile, eProjectileCycle &ProjectileCycleCommand) {
            cDamage Damage;
            int ObjectPieceNum;

            if (DetectProjectileCollision(tmpSpace, ObjectPieceNum, tmpProjectile, Damage, tmpSpace.Speed)) {
                if (NeedCheckCollision(tmpSpace)) {
                    tmpSpace.ArmorCurrentStatus -= Damage.Kinetic();

                    // FIXME let EM occasionally corrupt armor in some way (see space ship code above)

                    if (tmpSpace.ArmorCurrentStatus <= 0.0f) {
                        AddBonusForKilledEnemy(tmpSpace, tmpProjectile.ObjectStatus);
                        SetupSpaceExplosion(tmpSpace);
                        SpaceCycleCommand = eSpaceCycle::DeleteObjectAndContinue;
                    }
                }

                if (tmpProjectile.ProjectileType != 2) {
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
        if ((NeedCheckCollision(SecondObject) || NeedCheckCollision(FirstObject))
            && vw_SphereSphereCollision(FirstObject.Radius, FirstObject.Location,
                                        SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
            && vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                  SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {

            // in case of debris, just rebound to another direction
            if ((FirstObject.ObjectType == eObjectType::BasePart && SecondObject.ObjectType == eObjectType::SpaceDebris)
                || (FirstObject.ObjectType == eObjectType::SpaceDebris && SecondObject.ObjectType == eObjectType::BasePart)) {
                if (FirstObject.ObjectType == eObjectType::SpaceDebris) {
                    vw_RotatePoint(FirstObject.Orientation, sVECTOR3D{0.0f, 180.0f, 0.0f});
                }
                if (SecondObject.ObjectType == eObjectType::SpaceDebris) {
                    vw_RotatePoint(SecondObject.Orientation, sVECTOR3D{0.0f, 180.0f, 0.0f});
                }

                return; // eSpacePairCycle::Continue
            }

            if (SecondObject.ObjectType != eObjectType::SpaceDebris || FirstObject.ObjectType != eObjectType::SpaceDebris) {

                int ObjectPieceNum;

                if (FirstObject.ObjectType == eObjectType::BasePart
                    && !CheckHitBBMeshCollisionDetection(SecondObject, FirstObject, ObjectPieceNum)) {
                    return; // eSpacePairCycle::Continue
                }

                if (SecondObject.ObjectType == eObjectType::BasePart
                    && !CheckHitBBMeshCollisionDetection(FirstObject, SecondObject, ObjectPieceNum)) {
                    return; // eSpacePairCycle::Continue
                }

                if (NeedCheckCollision(SecondObject)
                    && (SecondObject.ObjectType == eObjectType::SmallAsteroid
                        || SecondObject.ObjectType == eObjectType::SpaceDebris)) {
                    SetupSpaceExplosion(SecondObject);
                    Command = eSpacePairCycle::DeleteSecondObjectAndContinue;
                }

                if (NeedCheckCollision(FirstObject)
                    && (FirstObject.ObjectType == eObjectType::SmallAsteroid
                        || FirstObject.ObjectType == eObjectType::SpaceDebris)) {
                    SetupSpaceExplosion(FirstObject);
                    if (Command == eSpacePairCycle::DeleteSecondObjectAndContinue) {
                        Command = eSpacePairCycle::DeleteBothObjectsAndContinue;
                    } else {
                        Command = eSpacePairCycle::DeleteFirstObjectAndContinue;
                    }
                }
            }
        }
    });

    ForEachProjectilePair([] (cProjectile &FirstObject, cProjectile &SecondObject, eProjectilePairCycle &Command) {
        if (ObjectsStatusFoe(FirstObject.ObjectStatus, SecondObject.ObjectStatus)) {
            // missile/mine with missile/mine
            if ((FirstObject.ProjectileType == 1 || FirstObject.ProjectileType == 4)
                && (SecondObject.ProjectileType == 1 || SecondObject.ProjectileType == 4)) {
                if (vw_SphereSphereCollision(FirstObject.Radius, FirstObject.Location,
                                             SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
                    && vw_SphereAABBCollision(FirstObject.AABB, FirstObject.Location,
                                              SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
                    && vw_SphereOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                             SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
                    && vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                          SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {

                    if (FirstObject.ProjectileType == 1 || FirstObject.ProjectileType == 4) {
                        FirstObject.Speed = 0.0f;
                        CreateBulletExplosion(nullptr, FirstObject, -FirstObject.Num, FirstObject.Location, FirstObject.Speed);
                        Command = eProjectilePairCycle::DeleteFirstObjectAndContinue;
                    }
                    if (SecondObject.ProjectileType == 1 || SecondObject.ProjectileType == 4) {
                        SecondObject.Speed = 0.0f;
                        CreateBulletExplosion(nullptr, SecondObject, -SecondObject.Num, SecondObject.Location, SecondObject.Speed);

                        if (Command == eProjectilePairCycle::DeleteFirstObjectAndContinue) {
                            Command = eProjectilePairCycle::DeleteBothObjectsAndContinue;
                        } else {
                            Command = eProjectilePairCycle::DeleteSecondObjectAndContinue;
                        }
                    }
                }
                // missile/mine with beam
            } else if ((FirstObject.ProjectileType == 1 || FirstObject.ProjectileType == 4) && SecondObject.ProjectileType == 2) {
                if (vw_SphereOBBCollision(SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat,
                                          FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation)
                    && vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                          SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {
                    FirstObject.Speed = 0.0f;
                    CreateBulletExplosion(nullptr, FirstObject, -FirstObject.Num, FirstObject.Location, FirstObject.Speed);
                    Command = eProjectilePairCycle::DeleteFirstObjectAndContinue;
                }
            } else if ((SecondObject.ProjectileType == 1 || SecondObject.ProjectileType == 4) && FirstObject.ProjectileType == 2) {
                if (vw_SphereOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                          SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
                    && vw_OBBOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                          SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat)) {
                    SecondObject.Speed = 0.0f;
                    CreateBulletExplosion(nullptr, SecondObject, -SecondObject.Num, SecondObject.Location, SecondObject.Speed);
                    Command = eProjectilePairCycle::DeleteSecondObjectAndContinue;
                }
                // missile/mine with projectile/flare
            } else if (FirstObject.ProjectileType == 1 || FirstObject.ProjectileType == 4) {
                if (vw_SphereSphereCollision(FirstObject.Radius, FirstObject.Location,
                                             SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
                    && vw_SphereAABBCollision(FirstObject.AABB, FirstObject.Location,
                                              SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)
                    && vw_SphereOBBCollision(FirstObject.OBB.Box, FirstObject.OBB.Location, FirstObject.Location, FirstObject.CurrentRotationMat,
                                             SecondObject.Radius, SecondObject.Location, SecondObject.PrevLocation)) {
                    FirstObject.Speed = 0.0f;
                    CreateBulletExplosion(nullptr, FirstObject, -FirstObject.Num, FirstObject.Location, FirstObject.Speed);
                    Command = eProjectilePairCycle::DeleteFirstObjectAndContinue;
                }
            } else if (SecondObject.ProjectileType == 1 || SecondObject.ProjectileType == 4) {
                if (vw_SphereSphereCollision(SecondObject.Radius, SecondObject.Location,
                                             FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation)
                    && vw_SphereAABBCollision(SecondObject.AABB, SecondObject.Location,
                                              FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation)
                    && vw_SphereOBBCollision(SecondObject.OBB.Box, SecondObject.OBB.Location, SecondObject.Location, SecondObject.CurrentRotationMat,
                                             FirstObject.Radius, FirstObject.Location, FirstObject.PrevLocation)) {
                    SecondObject.Speed = 0.0f;
                    CreateBulletExplosion(nullptr, SecondObject, -SecondObject.Num, SecondObject.Location, SecondObject.Speed);
                    Command = eProjectilePairCycle::DeleteSecondObjectAndContinue;
                }
            }
        }
    });
}

} // astromenace namespace
} // viewizard namespace
