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

#include "../core/core.h"
#include "../config/config.h"
#include "../platform/platform.h"
#include "../ui/font.h"
#include "../ui/game/text.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../game/camera.h"
#include "../object3d/explosion/explosion.h"
#include "../object3d/space_object/space_object.h"
#include "../object3d/ground_object/ground_object.h"
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/projectile/projectile.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include "SDL2/SDL.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// Immortal player ship mode for debug.
//#define ImmortalDebugMode

// last mouse position, need in order to detect do we have mouse move or not
int LastMouseX = -1;
int LastMouseY = -1;
// last mouse position for restore at exit from game to menu
int LastMouseXR = 0;
int LastMouseYR = 0;

// appropriate range: [-1.0f, 1.0f]
// forward-backward move
float MoveFB = 0.0f;
// left-right move
float MoveLR = 0.0f;

float CurrentPlayerShipEnergy;

// control of maneurous engines
bool PlayerFighterLeftEng = false;
bool PlayerFighterRightEng = false;

std::weak_ptr<cParticleSystem> Shild1{};
std::weak_ptr<cParticleSystem> Shild2{};
float ShildRadius;
float ShildEnergyStatus;
float ShildStartHitStatus;

// voice (index in voice array) with warning about collision detected
unsigned int VoiceWarningCollisionDetected{0};

// voice (index in voice array) with warning about missile detected
unsigned int VoiceMissileDetected{0};
bool VoiceMissileDetectedStatus{false};
// voice (index in voice array) with weapon malfunction + no ammo
unsigned int VoiceWeaponMalfunction{0};
// sound that player have low life (armor)
unsigned int SoundLowLife{0};


// in case of destroy, player ship represented as several parts, store part number that should be explode (part that was hit)
int PlayerDeadObjectPieceNum;


// movement calculation for semulative mode
sVECTOR3D CurrentMovementVel(0.0f, 0.0f, 0.0f);

extern float CurrentAlert2;
extern float CurrentAlert3;

// repeated fire related variables
int PrimaryGroupCurrentFireWeaponNum = 1;
float PrimaryGroupCurrentFireWeaponDelay = 0.0f;
int SecondaryGroupCurrentFireWeaponNum = 1;
float SecondaryGroupCurrentFireWeaponDelay = 0.0f;




//------------------------------------------------------------------------------------
// Return max energy count produced by energy system by its number
//------------------------------------------------------------------------------------
float GetShipMaxEnergy(int Num)
{
    switch (Num) {
    case 0:
        return 0.0f;
    // accumulator
    case 1:
        return 100.0f;
    // nuclear
    case 2:
        return 200.0f;
    // plasma
    case 3:
        return 400.0f;
    // antimatter
    case 4:
        return 800.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return -1.0f;
}
//------------------------------------------------------------------------------------
// Return energy recharge per second
//------------------------------------------------------------------------------------
float GetShipRechargeEnergy(int Num)
{
    switch (Num) {
    case 0:
        return 0.0f;
    // accumulator
    case 1:
        return 20.0f;
    // nuclear
    case 2:
        return 50.0f;
    // plasma
    case 3:
        return 130.0f;
    // antimatter
    case 4:
        return 250.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return -1.0f;
}
//------------------------------------------------------------------------------------
// Return energy consumption per second for GameAdvancedProtectionSystem by its number
//------------------------------------------------------------------------------------
float GetShipProtectionSystemEnergyUse(int Num)
{
    switch (Num) {
    // nano
    case 1:
        return 10.0f;
    // layer
    case 2:
        return 0.0f;
    // shild
    case 3:
        return 50.0f;
    // deflector
    case 4:
        return 100.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return 0.0f;
}
//------------------------------------------------------------------------------------
// Return energy consumption per second for GameEngineSystem by its number
//------------------------------------------------------------------------------------
float GetShipEngineSystemEnergyUse(int Num)
{
    switch (Num) {
    // jet
    case 1:
        return 5.0f;
    // foton
    case 2:
        return 10.0f;
    // plasma
    case 3:
        return 30.0f;
    // antimatter
    case 4:
        return 60.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return 0.0f;
}






//------------------------------------------------------------------------------------
// Player ship initialization at each game start/restart
//------------------------------------------------------------------------------------
void InitGamePlayerShip()
{
    //create ship
    VoiceMissileDetected = 0;
    VoiceMissileDetectedStatus = false;
    VoiceWeaponMalfunction = 0;
    SoundLowLife = 0;

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    if (GameConfig().Profile[CurrentProfile].ShipHull == 0) {
        std::cerr << __func__ << "(): " << "Error, Pilot Profile not created.\n";
    }

    PlayerFighter = CreateEarthSpaceFighter(GameConfig().Profile[CurrentProfile].ShipHull);
    auto sharedPlayerFighter = PlayerFighter.lock();
    if (!sharedPlayerFighter) {
        return;
    }

    sharedPlayerFighter->ShipShake.emplace_back(sVECTOR3D{0.0f, 0.0f, 1.0f},
                                                0,
                                                0.035f,
                                                [] () {return vw_fRand0() * 0.1f;});

    sharedPlayerFighter->ObjectStatus = eObjectStatus::Player;
    sharedPlayerFighter->ArmorInitialStatus *= GameConfig().Profile[CurrentProfile].ShipHullUpgrade;
    sharedPlayerFighter->ArmorCurrentStatus = GameConfig().Profile[CurrentProfile].ArmorStatus;
    sharedPlayerFighter->ShowStatus = false;

    // create weapon
    for (unsigned i=0; i<sharedPlayerFighter->WeaponSlots.size(); i++) {
        if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
            if (SetEarthSpaceFighterWeapon(PlayerFighter, i+1, GameConfig().Profile[CurrentProfile].Weapon[i])) {
                if (auto sharedWeapon = sharedPlayerFighter->WeaponSlots[i].Weapon.lock()) {
                    sharedWeapon->Ammo = GameConfig().Profile[CurrentProfile].WeaponAmmo[i];
                }
                sharedPlayerFighter->WeaponSlots[i].YAngle = -GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[i];
            }
        }
    }

    // create systems (visual part of internal ship systems)
    SetEarthSpaceFighterEngine(PlayerFighter, GameEngineSystem);
    SetEarthSpaceFighterArmor(PlayerFighter, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);

    GameEnemyArmorPenalty = TMPGameEnemyArmorPenalty;



    float Width2 = sharedPlayerFighter->Width/2.0f;
    float Length2 = sharedPlayerFighter->Length/2.0f;
    ShildRadius = vw_sqrtf(Width2*Width2+Length2*Length2);
    ShildEnergyStatus = 0.0f;
    ShildStartHitStatus = 0.0f;



    if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem == 3) {
        Shild1 = vw_CreateParticleSystem();
        if (auto sharedShild1 = Shild1.lock()) {
            sharedShild1->ColorStart.r = 0.20f;
            sharedShild1->ColorStart.g = 0.50f;
            sharedShild1->ColorStart.b = 0.10f;
            sharedShild1->ColorEnd.r = 0.20f;
            sharedShild1->ColorEnd.g = 0.50f;
            sharedShild1->ColorEnd.b = 0.10f;
            sharedShild1->AlphaStart = 1.00f;
            sharedShild1->AlphaEnd = 0.00f;
            sharedShild1->SizeStart = 0.60f;
            sharedShild1->SizeVar = 0.10f;
            sharedShild1->SizeEnd = 0.10f;
            sharedShild1->Speed = 0.00f;
            sharedShild1->SpeedOnCreation = -1.00f;
            sharedShild1->Theta = 360.00f;
            sharedShild1->Life = 1.00f;
            sharedShild1->ParticlesPerSec = (int)(40 * ShildRadius);
            sharedShild1->CreationType = eParticleCreationType::Sphere;
            sharedShild1->CreationSize = sVECTOR3D{ShildRadius, 0.05f * ShildRadius, ShildRadius};
            sharedShild1->DeadZone = ShildRadius - 0.05f;
            sharedShild1->AlphaShowHide = true;
            sharedShild1->IsMagnet = true;
            sharedShild1->MagnetFactor = -3.0f;
            {
                constexpr unsigned tmpHash = constexpr_hash_djb2a("gfx/flare1.tga");
                sharedShild1->Texture = GetPreloadedTextureAsset(tmpHash);
            }
            sharedShild1->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};
            sharedShild1->SetStartLocation(sharedPlayerFighter->Location + sharedPlayerFighter->OBB.Location);
        }

        ShildStartHitStatus = 100.0f;
        ShildEnergyStatus = 1.0f;
    }
    if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem == 4) {
        Shild1 = vw_CreateParticleSystem();
        if (auto sharedShild1 = Shild1.lock()) {
            sharedShild1->ColorStart.r = 0.50f;
            sharedShild1->ColorStart.g = 0.50f;
            sharedShild1->ColorStart.b = 1.00f;
            sharedShild1->ColorEnd.r = 0.50f;
            sharedShild1->ColorEnd.g = 0.50f;
            sharedShild1->ColorEnd.b = 1.00f;
            sharedShild1->AlphaStart = 0.50f;
            sharedShild1->AlphaEnd = 0.00f;
            sharedShild1->SizeStart = 0.40f;
            sharedShild1->SizeVar = 0.10f;
            sharedShild1->SizeEnd = 0.20f;
            sharedShild1->Speed = 0.00f;
            sharedShild1->SpeedOnCreation = -1.00f;
            sharedShild1->Theta = 360.00f;
            sharedShild1->Life = 1.00f;
            sharedShild1->ParticlesPerSec = (int)(40 * ShildRadius);
            sharedShild1->CreationType = eParticleCreationType::Sphere;
            sharedShild1->CreationSize = sVECTOR3D{ShildRadius, 0.05f * ShildRadius, ShildRadius};
            sharedShild1->DeadZone = ShildRadius - 0.05f;
            sharedShild1->IsMagnet = true;
            sharedShild1->AlphaShowHide = true;
            sharedShild1->MagnetFactor = 2.5f;
            {
                constexpr unsigned tmpHash = constexpr_hash_djb2a("gfx/flare1.tga");
                sharedShild1->Texture = GetPreloadedTextureAsset(tmpHash);
            }
            sharedShild1->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};
            sharedShild1->SetStartLocation(sharedPlayerFighter->Location + sharedPlayerFighter->OBB.Location);
        }

        Shild2 = vw_CreateParticleSystem();
        if (auto sharedShild2 = Shild2.lock()) {
            sharedShild2->ColorStart.r = 0.50f;
            sharedShild2->ColorStart.g = 0.50f;
            sharedShild2->ColorStart.b = 1.00f;
            sharedShild2->ColorEnd.r = 0.50f;
            sharedShild2->ColorEnd.g = 0.50f;
            sharedShild2->ColorEnd.b = 1.00f;
            sharedShild2->AlphaStart = 0.70f;
            sharedShild2->AlphaEnd = 0.10f;
            sharedShild2->SizeStart = 0.50f;
            sharedShild2->SizeVar = 0.10f;
            sharedShild2->SizeEnd = 0.30f;
            sharedShild2->Speed = 0.00f;
            sharedShild2->SpeedOnCreation = -1.00f;
            sharedShild2->Theta = 360.00f;
            sharedShild2->Life = 1.00f;
            sharedShild2->ParticlesPerSec = (int)(5 * ShildRadius);
            sharedShild2->CreationType = eParticleCreationType::Sphere;
            sharedShild2->CreationSize = sVECTOR3D{ShildRadius, 0.05f * ShildRadius, ShildRadius};
            sharedShild2->DeadZone = ShildRadius - 0.05f;
            sharedShild2->IsMagnet = true;
            sharedShild2->MagnetFactor = 20.0f;
            {
                constexpr unsigned tmpHash = constexpr_hash_djb2a("gfx/flare1.tga");
                sharedShild2->Texture = GetPreloadedTextureAsset(tmpHash);
            }
            sharedShild2->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};
        }

        ShildStartHitStatus = 150.0f;
        ShildEnergyStatus = 1.0f;
    }



    // initial setup, ship have full amount of energy
    CurrentPlayerShipEnergy = GetShipMaxEnergy(GamePowerSystem);

    // initial setup for controls
    LastMouseX = -1;
    LastMouseY = -1;
    MoveFB = 0.0f;
    MoveLR = 0.0f;
    CurrentMovementVel = sVECTOR3D{0.0f, 0.0f, 0.0f};

    // initial setup for fire related variables
    PrimaryGroupCurrentFireWeaponNum = 1;
    PrimaryGroupCurrentFireWeaponDelay = 0.0f;
    SecondaryGroupCurrentFireWeaponNum = 1;
    SecondaryGroupCurrentFireWeaponDelay = 0.0f;
}








//------------------------------------------------------------------------------------
// Main loop for player ship
//------------------------------------------------------------------------------------
void GamePlayerShip()
{
    auto sharedPlayerFighter = PlayerFighter.lock();
    if (!sharedPlayerFighter) {
        return;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // check player's ship "life" status (in our case "life" is "armor")
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (sharedPlayerFighter->ArmorCurrentStatus <= 0.0f) {
#ifdef ImmortalDebugMode
        sharedPlayerFighter->ArmorCurrentStatus = sharedPlayerFighter->ArmorInitialStatus;
#else
        // create explosion with camera movement increased by 10.0f
        CreateSpaceExplosion(*sharedPlayerFighter, 31, sharedPlayerFighter->Location, sharedPlayerFighter->Speed+10.0f, PlayerDeadObjectPieceNum);

        // switch to "FAILED" music theme and show "mission failed" text
        PlayMusicTheme(eMusicTheme::FAILED, 2000, 2000);
        ReleaseSpaceShip(PlayerFighter);
        SetupMissionFailedText(20.0f);
        return;
#endif
    }


    // play voice only in game (not in menu) and only in case mission not complete yet
    if (GameContentTransp < 0.99f && !GameMissionCompleteStatus) {
        int WarningMessagesCount = 0;

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Check is we have some missile that targeted on player ship
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        bool CheckTargetedStatus{false};
        ForEachProjectile([&] (const cProjectile &Projectile) {
            if (auto sharedTarget = Projectile.Target.lock()) {
                if (sharedTarget.get() == sharedPlayerFighter.get() &&
                    // homing missile targeted on player ship, but not homing mine
                    (Projectile.Num < 26 || Projectile.Num > 29)) {
                    CheckTargetedStatus = true;
                }
            }
        });

        if (CheckTargetedStatus) {
            // check is we play "missile detected" voice (must play only one instance)
            if (!vw_IsSoundAvailable(VoiceMissileDetected) && !VoiceMissileDetectedStatus) {
                VoiceMissileDetected = PlayVoicePhrase(eVoicePhrase::MissileDetected, 1.0f);
                VoiceMissileDetectedStatus = true;
            }

            // draw "Missile Detected" text
            vw_SetFontSize(24);
            int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidthUTF32(vw_GetTextUTF32("Missile Detected"))) / 2;
            vw_DrawTextUTF32(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3, vw_GetTextUTF32("Missile Detected"));
            ResetFontSize();
            WarningMessagesCount++;
        } else {
            if (CurrentAlert3 == 1.0f) {
                VoiceMissileDetectedStatus = false;
            } else if (VoiceMissileDetectedStatus) {
                // draw "Missile Detected" text
                vw_SetFontSize(24);
                int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidthUTF32(vw_GetTextUTF32("Missile Detected"))) / 2;
                vw_DrawTextUTF32(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3, vw_GetTextUTF32("Missile Detected"));
                ResetFontSize();
                WarningMessagesCount++;
            }
        }



        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Check collision with object on player ship course
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        bool CollisionDetected = false;
        ForEachSpaceObject([&CollisionDetected, &sharedPlayerFighter] (const cSpaceObject &tmpSpace, eSpaceCycle &Command) {
            // test with "immortal" big asteroids
            if (tmpSpace.ObjectType == eObjectType::BigAsteroid
                && vw_SphereSphereCollision(sharedPlayerFighter->Radius, sharedPlayerFighter->Location,
                                            tmpSpace.Radius, tmpSpace.Location, tmpSpace.PrevLocation)
                && vw_SphereAABBCollision(tmpSpace.AABB, tmpSpace.Location, sharedPlayerFighter->Radius,
                                          sharedPlayerFighter->Location, sharedPlayerFighter->PrevLocation)) {
                CollisionDetected = true;
                Command = eSpaceCycle::Break;
            }
        });
        ForEachGroundObject([&CollisionDetected, &sharedPlayerFighter] (const cGroundObject &tmpGround, eGroundCycle &Command) {
            // test with "immortal" civilian buildings
            if (tmpGround.ObjectType == eObjectType::CivilianBuilding
                && vw_SphereSphereCollision(sharedPlayerFighter->Radius, sharedPlayerFighter->Location,
                                            tmpGround.Radius, tmpGround.Location, tmpGround.PrevLocation)
                && vw_SphereAABBCollision(tmpGround.AABB, tmpGround.Location, sharedPlayerFighter->Radius,
                                          sharedPlayerFighter->Location, sharedPlayerFighter->PrevLocation)
                && vw_SphereOBBCollision(tmpGround.OBB.Box, tmpGround.OBB.Location, tmpGround.Location,
                                         tmpGround.CurrentRotationMat, sharedPlayerFighter->Radius,
                                         sharedPlayerFighter->Location, sharedPlayerFighter->PrevLocation)) {
                CollisionDetected = true;
                Command = eGroundCycle::Break;
            }
        });
        if (CollisionDetected) {
            // check is we play "collision detected" voice (must play only one instance)
            if (!vw_IsSoundAvailable(VoiceWarningCollisionDetected)) {
                VoiceWarningCollisionDetected = PlayVoicePhrase(eVoicePhrase::Warning, 1.0f);
            }

            // draw "Collision Course Detected" text
            vw_SetFontSize(24);
            int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidthUTF32(vw_GetTextUTF32("Collision Course Detected"))) / 2;
            vw_DrawTextUTF32(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3, vw_GetTextUTF32("Collision Course Detected"));
            ResetFontSize();
            WarningMessagesCount++;
        }





        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Check ammo in player ship's weapon
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if (!sharedPlayerFighter->WeaponSlots.empty()) {
            for (const auto &tmpWeaponSlot : sharedPlayerFighter->WeaponSlots) {
                // in case player try to fire, but some weapon don't have ammo
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    if (tmpWeaponSlot.SetFire
                        && sharedWeapon->Ammo <= 0
                        && !vw_IsSoundAvailable(VoiceWeaponMalfunction)) { // check is we play "weapon malfunction" voice (must play only one instance)
                        VoiceWeaponMalfunction = PlayVoicePhrase(eVoicePhrase::WeaponMalfunction, 1.0f);
                    }
                }
            }
        }


        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // SFX that player ship have less than 10% armor
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus / 10.0f
            // must play only one instance
            && !vw_IsSoundAvailable(SoundLowLife)) {
            SoundLowLife = PlayMenuSFX(eMenuSFX::WarningLowLife, 1.0f);
        }
    }





    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // player ship's control - movement
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (GameContentTransp < 1.0f) { // in case this is not menu, but game
        // get data for both - sim and arcade control mode
        // in case we already have data from mouse/joystick - ignore keyboard
        bool NeedSkip = false;

        // mouse + joystick (since joystick emulate mouse movements)
        if (GameConfig().MouseControl) {
            SDL_GetMouseState(&LastMouseXR, &LastMouseYR);

            int X, Y;
            vw_GetMousePos(X, Y);
            if (LastMouseX == -1 && LastMouseY == -1) {
                LastMouseX = X;
                LastMouseY = Y;
            } else {
                if (X != LastMouseX || Y != LastMouseY) {
                    // 0.9+0.1 = 1.0 - minimal, must be 1.0!
                    float Koef = 0.9f + GameConfig().ControlSensivity / 10.0f;

                    // for any resolution x and y will take into account AspectRatio
                    float AWw2 = GameConfig().InternalWidth / 2.0f;
                    float AHw2 = GameConfig().InternalHeight / 2.0f;

                    MoveFB += (-(Y-LastMouseY)/AHw2)*Koef;
                    MoveLR += ( (X-LastMouseX)/AWw2)*Koef;

                    NeedSkip = true;
                }

                LastMouseX = X;
                LastMouseY = Y;
            }

        }

        // keyboard
        if (!NeedSkip) {
            if (vw_GetKeyStatus(GameConfig().KeyBoardDown)) {
                MoveFB -= 2.0f * (GameConfig().ControlSensivity / 10.0f) * sharedPlayerFighter->TimeDelta;
            }
            if (vw_GetKeyStatus(GameConfig().KeyBoardUp)) {
                MoveFB += 2.0f * (GameConfig().ControlSensivity / 10.0f) * sharedPlayerFighter->TimeDelta;
            }
            if (vw_GetKeyStatus(GameConfig().KeyBoardLeft)) {
                MoveLR -= 2.0f * (GameConfig().ControlSensivity / 10.0f) * sharedPlayerFighter->TimeDelta;
            }
            if (vw_GetKeyStatus(GameConfig().KeyBoardRight)) {
                MoveLR += 2.0f * (GameConfig().ControlSensivity / 10.0f) * sharedPlayerFighter->TimeDelta;
            }
        }



        // additional check, since previously we could calculate out of range value
        if (MoveFB < -1.0f) {
            MoveFB = -1.0f;
        }
        if (MoveFB > 1.0f) {
            MoveFB = 1.0f;
        }
        if (MoveLR < -1.0f) {
            MoveLR = -1.0f;
        }
        if (MoveLR > 1.0f) {
            MoveLR = 1.0f;
        }






        // calculate end point of movement
        sVECTOR3D PlayerFighterEndLocation;
        if (GameConfig().InternalWidth == 1024.0f) {
            PlayerFighterEndLocation = sVECTOR3D{-(73.15f-sharedPlayerFighter->Width/2.0f+MoveFB*(20.05f-sharedPlayerFighter->Length/6.0f))*MoveLR,
                                                 0.0f,
                                                 (46.0f-sharedPlayerFighter->Length/2.0f)*MoveFB};
        } else {
            PlayerFighterEndLocation = sVECTOR3D{-(70.0f-sharedPlayerFighter->Width/2.0f+MoveFB*(23.2f-sharedPlayerFighter->Length/6.0f))*MoveLR,
                                                 0.0f,
                                                 (46.0f-sharedPlayerFighter->Length/2.0f)*MoveFB};
        }

        PlayerFighterEndLocation += GetCameraCoveredDistance();

        // check, that we have engines (yes, player could sell it and don't have any engines on ship)
        if (GameEngineSystem != 0) {
            if (GameConfig().Profile[CurrentProfile].SpaceShipControlMode == 1) {
                // arcade control mode


                // by default, stop maneurouse engines
                PlayerFighterLeftEng = false;
                PlayerFighterRightEng = false;
                // in case ship move left or right, start maneurouse engines
                if ((int)sharedPlayerFighter->Location.x > (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = true;
                }
                else if ((int)sharedPlayerFighter->Location.x < (int)PlayerFighterEndLocation.x) {
                    PlayerFighterRightEng = true;
                }


                // calculate direction and distance from current location to end location
                sVECTOR3D PlayerFighterNewDirection = PlayerFighterEndLocation - sharedPlayerFighter->Location;
                float EndLocationDistance = PlayerFighterNewDirection.Length();
                PlayerFighterNewDirection.Normalize();

                float SimMoveSpeed = EndLocationDistance;

                if (SimMoveSpeed > 30.0f) {
                    SimMoveSpeed = 30.0f;
                }

                SimMoveSpeed = SimMoveSpeed*4.0f*sharedPlayerFighter->TimeDelta;


                // calculate current movement location
                CurrentMovementVel = PlayerFighterNewDirection^SimMoveSpeed;

                // additional check that movement are in allowed range
                float MaxSpeed = CurrentMovementVel.Length();
                CurrentMovementVel.Normalize();
                if (MaxSpeed > 30.0f) {
                    MaxSpeed = 30.0f;
                }

                CurrentMovementVel = CurrentMovementVel^MaxSpeed;

            } else {
                // sim control mode


                // by default, stop maneurouse engines
                PlayerFighterLeftEng = false;
                PlayerFighterRightEng = false;
                // in case ship move left or right, start maneurouse engines
                if ((int)sharedPlayerFighter->Location.x > (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = true;
                }
                else if ((int)sharedPlayerFighter->Location.x < (int)PlayerFighterEndLocation.x) {
                    PlayerFighterRightEng = true;
                }


                // calculate direction and distance from current location to end location
                sVECTOR3D PlayerFighterNewDirection = PlayerFighterEndLocation - sharedPlayerFighter->Location;
                float EndLocationDistance = PlayerFighterNewDirection.Length();
                PlayerFighterNewDirection.Normalize();

                float SimMoveSpeed = EndLocationDistance;

                if (SimMoveSpeed > sharedPlayerFighter->MaxSpeed) {
                    SimMoveSpeed = sharedPlayerFighter->MaxSpeed;
                }

                SimMoveSpeed = SimMoveSpeed*(sharedPlayerFighter->MaxAcceler/14.0f)*sharedPlayerFighter->TimeDelta;


                // calculate current movement location
                CurrentMovementVel = PlayerFighterNewDirection^SimMoveSpeed;

                // additional check that movement are in allowed range
                float MaxSpeed = CurrentMovementVel.Length();
                CurrentMovementVel.Normalize();
                if (MaxSpeed > sharedPlayerFighter->MaxSpeed) {
                    MaxSpeed = sharedPlayerFighter->MaxSpeed;
                }

                CurrentMovementVel = CurrentMovementVel^MaxSpeed;

            }
        }

        // move ship to new location
        sVECTOR3D CurrentVel = sharedPlayerFighter->Location + CurrentMovementVel;
        CurrentVel.y = 0.0f;
        sharedPlayerFighter->SetLocationArcadePlayer(CurrentVel);



        // in case "standard" AspectRatio, care about camera movement too
        // in this case camera "move" left-right inside more wide size of widescreen view
        //
        //   widescreen view
        //   [    <- [standard view] ->        ]
        //   [       [             ]           ]
        //   [       [             ]           ]
        //   [       [             ]           ]
        //
        if (GameConfig().InternalWidth == 1024.0f) {
            float DeviationSize = 14.55f;

            if (sharedPlayerFighter->Location.x < 0.0f) {
                float Diff = sharedPlayerFighter->Location.x / 3.5f;
                if (Diff < -DeviationSize) {
                    Diff = -DeviationSize;
                }

                sVECTOR3D TMPCameraLocation;
                vw_GetCameraLocation(&TMPCameraLocation);
                TMPCameraLocation.x = Diff;
                vw_SetCameraLocation(TMPCameraLocation);
            } else {
                float Diff = sharedPlayerFighter->Location.x / 3.5f;
                if (Diff > DeviationSize) {
                    Diff = DeviationSize;
                }

                sVECTOR3D TMPCameraLocation;
                vw_GetCameraLocation(&TMPCameraLocation);
                TMPCameraLocation.x = Diff;
                vw_SetCameraLocation(TMPCameraLocation);
            }
        }

    }





    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // player ship's control - fire
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (GameContentTransp < 0.5f // in case this is not menu, but game
        && !sharedPlayerFighter->WeaponSlots.empty()) {

        int PrimCount = 0;
        float PrimTime = 0.0f;
        int SecCount = 0;
        float SecTime = 0.0f;

        PrimaryGroupCurrentFireWeaponDelay -= sharedPlayerFighter->TimeDelta;
        SecondaryGroupCurrentFireWeaponDelay -= sharedPlayerFighter->TimeDelta;

        // calculate weapons quantity in groups (primary, secondary)
        for (unsigned i = 0; i < sharedPlayerFighter->WeaponSlots.size(); i++) {
            if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) { // if weapon istalled

                if (GameConfig().Profile[CurrentProfile].WeaponControl[i] == 1
                    || GameConfig().Profile[CurrentProfile].WeaponControl[i] == 3) {
                    if (auto sharedWeapon = sharedPlayerFighter->WeaponSlots[i].Weapon.lock()) {
                        PrimCount++;
                        PrimTime += sharedWeapon->NextFireTime;
                    }
                }

                if (GameConfig().Profile[CurrentProfile].WeaponControl[i] == 2
                    || GameConfig().Profile[CurrentProfile].WeaponControl[i] == 3) {
                    if (auto sharedWeapon = sharedPlayerFighter->WeaponSlots[i].Weapon.lock()) {
                        SecCount++;
                        SecTime += sharedWeapon->NextFireTime;
                    }
                }
            }
        }

        int PrimNum = 0;
        int SecNum = 0;

        for (unsigned i = 0; i < sharedPlayerFighter->WeaponSlots.size(); i++) {
            if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) { // if weapon istalled

                sharedPlayerFighter->WeaponSlots[i].SetFire = false;

                bool primary_fire = false;
                bool secondary_fire = false;
                if (GameConfig().Profile[CurrentProfile].WeaponControl[i] == 1
                    || GameConfig().Profile[CurrentProfile].WeaponControl[i] ==3) {
                    primary_fire = true;
                }
                if (GameConfig().Profile[CurrentProfile].WeaponControl[i] == 2
                    || GameConfig().Profile[CurrentProfile].WeaponControl[i] ==3) {
                    secondary_fire = true;
                }

                // mouse
                if (GameConfig().MouseControl) {
                    // primary fire
                    if (primary_fire) {
                        if (vw_GetMouseButtonStatus(GameConfig().MousePrimary)) {
                            if (GameConfig().Profile[CurrentProfile].PrimaryWeaponFireMode == 1) {
                                sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                            } else {
                                PrimNum++;
                                if (PrimaryGroupCurrentFireWeaponNum == PrimNum
                                    && PrimaryGroupCurrentFireWeaponDelay <= 0.0f) {
                                    PrimaryGroupCurrentFireWeaponDelay = PrimTime / (PrimCount * PrimCount);
                                    sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                                    PrimaryGroupCurrentFireWeaponNum++;
                                    if (PrimaryGroupCurrentFireWeaponNum > PrimCount) {
                                        PrimaryGroupCurrentFireWeaponNum = 1;
                                    }
                                }
                            }
                        }
                    }

                    // secondary fire
                    if (secondary_fire) {
                        if (vw_GetMouseButtonStatus(GameConfig().MouseSecondary)) {
                            if (GameConfig().Profile[CurrentProfile].SecondaryWeaponFireMode == 1) {
                                sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                            } else {
                                SecNum++;
                                if (SecondaryGroupCurrentFireWeaponNum == SecNum
                                    && SecondaryGroupCurrentFireWeaponDelay <= 0.0f) {
                                    SecondaryGroupCurrentFireWeaponDelay = SecTime / (SecCount * SecCount);
                                    sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                                    SecondaryGroupCurrentFireWeaponNum++;
                                    if (SecondaryGroupCurrentFireWeaponNum > SecCount) {
                                        SecondaryGroupCurrentFireWeaponNum = 1;
                                    }
                                }
                            }
                        }
                    }

                    // custom control
                    if (GameConfig().Profile[CurrentProfile].WeaponAltControl[i] == 2
                        && vw_GetMouseButtonStatus(GameConfig().Profile[CurrentProfile].WeaponAltControlData[i])) {
                        sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                    }
                }


                // joystick
                if (isJoystickAvailable()) {
                    // primary fire
                    if (primary_fire) {
                        if (GetJoystickButton(GameConfig().JoystickPrimary)) {
                            if (GameConfig().Profile[CurrentProfile].PrimaryWeaponFireMode == 1) {
                                sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                            } else {
                                PrimNum++;
                                if (PrimaryGroupCurrentFireWeaponNum == PrimNum
                                    && PrimaryGroupCurrentFireWeaponDelay <= 0.0f) {
                                    PrimaryGroupCurrentFireWeaponDelay = PrimTime / (PrimCount * PrimCount);
                                    sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                                    PrimaryGroupCurrentFireWeaponNum++;
                                    if (PrimaryGroupCurrentFireWeaponNum > PrimCount) {
                                        PrimaryGroupCurrentFireWeaponNum = 1;
                                    }
                                }
                            }
                        }
                    }

                    // secondary fire
                    if (secondary_fire) {
                        if (GetJoystickButton(GameConfig().JoystickSecondary)) {
                            if (GameConfig().Profile[CurrentProfile].SecondaryWeaponFireMode == 1) {
                                sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                            } else {
                                SecNum++;
                                if (SecondaryGroupCurrentFireWeaponNum == SecNum
                                    && SecondaryGroupCurrentFireWeaponDelay <= 0.0f) {
                                    SecondaryGroupCurrentFireWeaponDelay = SecTime / (SecCount * SecCount);
                                    sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                                    SecondaryGroupCurrentFireWeaponNum++;
                                    if (SecondaryGroupCurrentFireWeaponNum > SecCount) {
                                        SecondaryGroupCurrentFireWeaponNum = 1;
                                    }
                                }
                            }
                        }
                    }

                    // custom control
                    if (GameConfig().Profile[CurrentProfile].WeaponAltControl[i] == 3
                        && GetJoystickButton(GameConfig().Profile[CurrentProfile].WeaponAltControlData[i])) {
                        sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                    }
                }

                // keyboard

                // primary fire
                if (primary_fire) {
                    if (vw_GetKeyStatus(GameConfig().KeyBoardPrimary)) {
                        if (GameConfig().Profile[CurrentProfile].PrimaryWeaponFireMode == 1) {
                            sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                        } else {
                            PrimNum++;
                            if (PrimaryGroupCurrentFireWeaponNum == PrimNum
                                && PrimaryGroupCurrentFireWeaponDelay <= 0.0f) {
                                PrimaryGroupCurrentFireWeaponDelay = PrimTime / (PrimCount * PrimCount);
                                sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                                PrimaryGroupCurrentFireWeaponNum++;
                                if (PrimaryGroupCurrentFireWeaponNum > PrimCount) {
                                    PrimaryGroupCurrentFireWeaponNum = 1;
                                }
                            }
                        }
                    }
                }

                // secondary fire
                if (secondary_fire) {
                    if (vw_GetKeyStatus(GameConfig().KeyBoardSecondary)) {
                        if (GameConfig().Profile[CurrentProfile].SecondaryWeaponFireMode == 1) {
                            sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                        } else {
                            SecNum++;
                            if (SecondaryGroupCurrentFireWeaponNum == SecNum
                                && SecondaryGroupCurrentFireWeaponDelay <= 0.0f) {
                                SecondaryGroupCurrentFireWeaponDelay = SecTime / (SecCount * SecCount);
                                sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                                SecondaryGroupCurrentFireWeaponNum++;
                                if (SecondaryGroupCurrentFireWeaponNum > SecCount) {
                                    SecondaryGroupCurrentFireWeaponNum = 1;
                                }
                            }
                        }
                    }
                }

                // custom control
                if (GameConfig().Profile[CurrentProfile].WeaponAltControl[i] == 1
                    && vw_GetKeyStatus(GameConfig().Profile[CurrentProfile].WeaponAltControlData[i])) {
                        sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                }
            }
        }
    }





    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // internal ship's systems
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (GameSpaceShipControlMode != 1) { // for sim mode only
        if (CurrentPlayerShipEnergy < GetShipEngineSystemEnergyUse(GameEngineSystem)*sharedPlayerFighter->TimeDelta) {
            // no power, stifle all engines
            sharedPlayerFighter->MaxSpeed = 0.0f;
            sharedPlayerFighter->MaxAcceler = 0.0f;
            sharedPlayerFighter->MaxSpeedRotate = 0.0f;
            for (auto &tmpEngine : sharedPlayerFighter->Engines) {
                if (auto sharedEngine = tmpEngine.lock()) {
                    sharedEngine->IsSuppressed = true;
                }
            }
            if (!sharedPlayerFighter->EnginesLeft.empty()) {
                for (auto &tmpEngineLeft : sharedPlayerFighter->EnginesLeft) {
                    if (auto sharedEngineLeft = tmpEngineLeft.lock()) {
                        sharedEngineLeft->IsSuppressed = true;
                    }
                }
            }
            if (!sharedPlayerFighter->EnginesRight.empty()) {
                for (auto &tmpEngineRight : sharedPlayerFighter->EnginesRight) {
                    if (auto sharedEngineRight = tmpEngineRight.lock()) {
                        sharedEngineRight->IsSuppressed = true;
                    }
                }
            }
        } else {
            sharedPlayerFighter->MaxSpeed = GetEnginePower(GameEngineSystem);
            sharedPlayerFighter->MaxAcceler = GetEngineAcceleration(GameEngineSystem);
            sharedPlayerFighter->MaxSpeedRotate = GetEngineRotatePower(GameEngineSystem);
            for (auto &tmpEngine : sharedPlayerFighter->Engines) {
                if (auto sharedEngine = tmpEngine.lock()) {
                    sharedEngine->IsSuppressed = false;
                }
            }
            if (!sharedPlayerFighter->EnginesLeft.empty()) {
                for (auto &tmpEngineLeft : sharedPlayerFighter->EnginesLeft) {
                    if (auto sharedEngineLeft = tmpEngineLeft.lock()) {
                        sharedEngineLeft->IsSuppressed = false;
                    }
                }
            }
            if (!sharedPlayerFighter->EnginesRight.empty()) {
                for (auto &tmpEngineRight : sharedPlayerFighter->EnginesRight) {
                    if (auto sharedEngineRight = tmpEngineRight.lock()) {
                        sharedEngineRight->IsSuppressed = false;
                    }
                }
            }
            CurrentPlayerShipEnergy -= GetShipEngineSystemEnergyUse(GameEngineSystem)*sharedPlayerFighter->TimeDelta;
        }
    }



    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // weapon energy related routine
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for (unsigned i = 0; i < sharedPlayerFighter->WeaponSlots.size(); i++) {
        if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
            if (auto sharedWeapon = sharedPlayerFighter->WeaponSlots[i].Weapon.lock()) {
                if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse) {
                    // not enough energy for weapon fire
                    if (CurrentPlayerShipEnergy < sharedWeapon->EnergyUse) {
                        // stop energy weapon reload
                        sharedWeapon->LastFireTime += sharedPlayerFighter->TimeDelta;
                        if (auto sharedFire = sharedWeapon->Fire.lock()) {
                            sharedFire->IsSuppressed = true;
                        }
                    } else {
                        sharedWeapon->CurrentEnergyAccumulated = sharedWeapon->EnergyUse;
                        CurrentPlayerShipEnergy -= sharedWeapon->EnergyUse;
                    }
                }
            }
        }
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // defence system energy related routine
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (CurrentPlayerShipEnergy >= GetShipEngineSystemEnergyUse(GameEngineSystem)*sharedPlayerFighter->TimeDelta) {

        switch (GameAdvancedProtectionSystem) {
        // nano
        case 1:
            // repair 0.5% armor per second
            if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus) {
                CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem) * sharedPlayerFighter->TimeDelta;
                sharedPlayerFighter->ArmorCurrentStatus += (sharedPlayerFighter->ArmorInitialStatus / 200.0f) * sharedPlayerFighter->TimeDelta;
                if (sharedPlayerFighter->ArmorCurrentStatus > sharedPlayerFighter->ArmorInitialStatus) {
                    sharedPlayerFighter->ArmorCurrentStatus = sharedPlayerFighter->ArmorInitialStatus;
                }
            }
            break;
        // armor layer
        case 2:
            break; // do nothing, this passive resistance system
        // shield
        case 3:
            // full restore during 4 seconds
            if (ShildEnergyStatus < 1.0f) {
                CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem) * sharedPlayerFighter->TimeDelta;
                ShildEnergyStatus += 0.02f * sharedPlayerFighter->TimeDelta;
                if (ShildEnergyStatus > 1.0f) {
                    ShildEnergyStatus = 1.0f;
                }
            }
            break;
        // deflector
        case 4:
            // full restore during 2 seconds
            if (ShildEnergyStatus < 1.0f) {
                CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem) * sharedPlayerFighter->TimeDelta;
                ShildEnergyStatus += 0.03f * sharedPlayerFighter->TimeDelta;
                if (ShildEnergyStatus > 1.0f) {
                    ShildEnergyStatus = 1.0f;
                }
            }
            break;
        }
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // shield and deflector current strength status visualization
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (auto sharedShild1 = Shild1.lock()) {
        sharedShild1->MoveSystem(sharedPlayerFighter->Location + sharedPlayerFighter->OBB.Location);
        sharedShild1->SetStartLocation(sharedPlayerFighter->Location + sharedPlayerFighter->OBB.Location);
        sharedShild1->RotateSystemAndParticlesByAngle(sharedPlayerFighter->Rotation);
        sharedShild1->ParticlesPerSec = (int)(40 * ShildEnergyStatus * ShildRadius);
    }
    if (auto sharedShild2 = Shild2.lock()) {
        sharedShild2->MoveSystem(sharedPlayerFighter->Location + sharedPlayerFighter->OBB.Location);
        sharedShild2->SetStartLocation(sharedPlayerFighter->Location + sharedPlayerFighter->OBB.Location);
        sharedShild2->RotateSystemAndParticlesByAngle(sharedPlayerFighter->Rotation);
        sharedShild2->ParticlesPerSec = (int)(5 * ShildEnergyStatus * ShildRadius);
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // generate energy for reactors only, not battery
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    CurrentPlayerShipEnergy += GetShipRechargeEnergy(GamePowerSystem)*sharedPlayerFighter->TimeDelta;
    if (CurrentPlayerShipEnergy > GetShipMaxEnergy(GamePowerSystem)) {
        CurrentPlayerShipEnergy = GetShipMaxEnergy(GamePowerSystem);
    }

}

} // astromenace namespace
} // viewizard namespace
