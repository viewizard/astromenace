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

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// indestructible mode ... debugging
bool UndeadDebugMode = false;

// management state, need to determine what has changed
int LastMouseX = -1;
int LastMouseY = -1;
// to restore the cursor position in the menu, or rather when exiting it
int LastMouseXR = 0;
int LastMouseYR = 0;

// indicators from -1.0f to 1.0f
// backward-forward
float MoveFB = 0.0f;
// left-right
float MoveLR = 0.0f;

// ship's current energy
float CurrentPlayerShipEnergy;

// for control in arcade mode with maneuvering engines
bool PlayerFighterLeftEng = false;
bool PlayerFighterRightEng = false;

std::weak_ptr<cParticleSystem> Shild1{};
std::weak_ptr<cParticleSystem> Shild2{};
float ShildRadius;
float ShildEnergyStatus;
float ShildStartHitStatus;

// voice with worning if collided with an unbreakable part
unsigned int VoiceWarningCollisionDetected{0};

// Sound ID for missile detection voice message
unsigned int VoiceMissileDetected{0};
bool VoiceMissileDetectedStatus{false};
// Sound ID for voice message about problems with the gun
// Sound ID for voice message about the absence of shells in the ammunition
unsigned int VoiceWeaponMalfunction{0};
// Sound ID for low health
unsigned int SoundLowLife{0};


// here we store what part exploded on the player’s ship
int PlayerDeadObjectPieceNum;


// simulation mode
sVECTOR3D CurrentMovementVel(0.0f, 0.0f, 0.0f);

// work with blinking output
extern float CurrentAlert2;
extern float CurrentAlert3;

// for variable type of shooting
int PrimaryGroupCurrentFireWeaponNum = 1;
float PrimaryGroupCurrentFireWeaponDelay = 0.0f;
int SecondaryGroupCurrentFireWeaponNum = 1;
float SecondaryGroupCurrentFireWeaponDelay = 0.0f;



//------------------------------------------------------------------------------------
// Returns the maximum possible amount of energy for the type of reactor/battery
//------------------------------------------------------------------------------------
float GetShipMaxEnergy(int Num)
{
    switch (Num) {
    case 0:
        return 0.0f;
    // Battery
    case 1:
        return 100.0f;
    // Nuclear
    case 2:
        return 200.0f;
    // Plasma
    case 3:
        return 400.0f;
    // Antimatter
    case 4:
        return 800.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return -1.0f;
}
//------------------------------------------------------------------------------------
// Returns energy recharge rate per second
//------------------------------------------------------------------------------------
float GetShipRechargeEnergy(int Num)
{
    switch (Num) {
    case 0:
        return 0.0f;
    // Battery
    case 1:
        return 20.0f;
    // Nuclear
    case 2:
        return 50.0f;
    // Plasma
    case 3:
        return 130.0f;
    // Antimatter
    case 4:
        return 250.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return -1.0f;
}
//------------------------------------------------------------------------------------
// Returns energy consumption rate per second for additional systems (GameAdvancedProtectionSystem)
//------------------------------------------------------------------------------------
float GetShipProtectionSystemEnergyUse(int Num)
{
    switch (Num) {
    // nano robots
    case 1:
        return 10.0f;
    // special defence layer
    case 2:
        return 0.0f;
    // shield
    case 3:
        return 50.0f;
    // reflector
    case 4:
        return 100.0f;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return 0.0f;
}
//------------------------------------------------------------------------------------
// Returns engine energy consumption rate per second GameEngineSystem
//------------------------------------------------------------------------------------
float GetShipEngineSystemEnergyUse(int Num)
{
    switch (Num) {
    // regular
    case 1:
        return 5.0f;
    // photon
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
// Player's ship initialization
//------------------------------------------------------------------------------------
void InitGamePlayerShip()
{
    // create player’s ship per settings in profile
    VoiceMissileDetected = 0;
    VoiceMissileDetectedStatus = false;
    VoiceWeaponMalfunction = 0;
    SoundLowLife = 0;

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    // if profile is not created, here will probably be zero
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

    // create weapons
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

    // create systems (visual)
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
            sharedShild1->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
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
            sharedShild1->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
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
            sharedShild2->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
            sharedShild2->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};
        }

        ShildStartHitStatus = 150.0f;
        ShildEnergyStatus = 1.0f;
    }



    // preset, fully charged device
    CurrentPlayerShipEnergy = GetShipMaxEnergy(GamePowerSystem);

    // preliminary control states
    LastMouseX = -1;
    LastMouseY = -1;
    MoveFB = 0.0f;
    MoveLR = 0.0f;
    CurrentMovementVel = sVECTOR3D{0.0f, 0.0f, 0.0f};

    // reset shooting...
    PrimaryGroupCurrentFireWeaponNum = 1;
    PrimaryGroupCurrentFireWeaponDelay = 0.0f;
    SecondaryGroupCurrentFireWeaponNum = 1;
    SecondaryGroupCurrentFireWeaponDelay = 0.0f;
}








//------------------------------------------------------------------------------------
// Main procedure for processing the state of the player’s ship
//------------------------------------------------------------------------------------
void GamePlayerShip()
{
    auto sharedPlayerFighter = PlayerFighter.lock();
    if (!sharedPlayerFighter) {
        return;
    }

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // check if the ship is still alive, or shot down and needs to be removed ...
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (sharedPlayerFighter->ArmorCurrentStatus <= 0.0f) {
        // rare case
        if (UndeadDebugMode) {
            sharedPlayerFighter->ArmorCurrentStatus = sharedPlayerFighter->ArmorInitialStatus;
        } else {
            // make an explosion
            // + 10.0f camera movement
            CreateSpaceExplosion(*sharedPlayerFighter, 31, sharedPlayerFighter->Location, sharedPlayerFighter->Speed+10.0f, PlayerDeadObjectPieceNum);

            // play the music and display  "mission failed"
            PlayMusicTheme(eMusicTheme::FAILED, 2000, 2000);

            // release ship and exit
            ReleaseSpaceShip(PlayerFighter);

            SetupMissionFailedText(20.0f);

            return;
        }
    }



    // voice output only in game! pauses in menu
    // and if the level has not ended
    if (GameContentTransp < 0.99f && !GameMissionCompleteStatus) {
        int WarningMessagesCount = 0;

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Recap the voice warning if the rocket was aimed
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        bool CheckStatus{false};
        ForEachProjectile([&] (const cProjectile &Projectile) {
            if (auto sharedTarget = Projectile.Target.lock()) {
                if (sharedTarget.get() == sharedPlayerFighter.get() &&
                    // homing missile targeted on this ship, but not homing mine
                    (Projectile.Num < 26 || Projectile.Num > 29)) {
                    CheckStatus = true;
                }
            }
        });

        if (CheckStatus) {
            // check if still in game (gaming just once!)
            if (!vw_IsSoundAvailable(VoiceMissileDetected) && !VoiceMissileDetectedStatus) {
                VoiceMissileDetected = PlayVoicePhrase(eVoicePhrase::MissileDetected, 1.0f);
                VoiceMissileDetectedStatus = true;
            }

            // visual output - output continuously
            vw_SetFontSize(24);
            int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidthUTF32(vw_GetTextUTF32("Missile Detected"))) / 2;
            vw_DrawTextUTF32(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3, vw_GetTextUTF32("Missile Detected"));
            ResetFontSize();
            WarningMessagesCount++;
        } else {
            if (CurrentAlert3 == 1.0f) { // full loop complete, last counter value was minimal
                VoiceMissileDetectedStatus = false;
            } else if (VoiceMissileDetectedStatus) {
                // visual output - output continuously
                vw_SetFontSize(24);
                int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidthUTF32(vw_GetTextUTF32("Missile Detected"))) / 2;
                vw_DrawTextUTF32(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, CurrentAlert3, vw_GetTextUTF32("Missile Detected"));
                ResetFontSize();
                WarningMessagesCount++;
            }
        }



        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Output voice warning if possible colision
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
            // voice warning, possible colision with object
            // Check if still in game
            if (!vw_IsSoundAvailable(VoiceWarningCollisionDetected)) {
                VoiceWarningCollisionDetected = PlayVoicePhrase(eVoicePhrase::Warning, 1.0f);
            }

            // visual output - output continuously
            vw_SetFontSize(24);
            int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidthUTF32(vw_GetTextUTF32("Collision Course Detected"))) / 2;
            vw_DrawTextUTF32(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3, vw_GetTextUTF32("Collision Course Detected"));
            ResetFontSize();
            WarningMessagesCount++;
        }





        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Output voice warning if weapon has no munition
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if (!sharedPlayerFighter->WeaponSlots.empty()) { // if weapon is available
            for (const auto &tmpWeaponSlot : sharedPlayerFighter->WeaponSlots) {
                // if fire button is pressed and one weapon has no munition
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    if (tmpWeaponSlot.SetFire
                        && sharedWeapon->Ammo <= 0
                        && !vw_IsSoundAvailable(VoiceWeaponMalfunction)) { // check if still in game
                        VoiceWeaponMalfunction = PlayVoicePhrase(eVoicePhrase::WeaponMalfunction, 1.0f);
                    }
                }
            }
        }


        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Voice warning if health is below 10%
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // if less than 10% must be a warning
        if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus / 10.0f
            && !vw_IsSoundAvailable(SoundLowLife)) {
            // if still in game, play siren sound
            SoundLowLife = PlayMenuSFX(eMenuSFX::WarningLowLife, 1.0f);
        }
    }





    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // ship control - movement
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (GameContentTransp < 1.0f) { // click if not in the menu
        // receiving data for both types of control
        // data already received, should ignore the rest
        bool NeedSkip = false;

        // mouse + joystick (since we emulate mouse movements)
        if (GameConfig().MouseControl) {
            SDL_GetMouseState(&LastMouseXR, &LastMouseYR);

            int X, Y;
            vw_GetMousePos(X, Y);
            if (LastMouseX == -1 && LastMouseY == -1) {
                LastMouseX = X;
                LastMouseY = Y;
            } else {
                if (X != LastMouseX || Y != LastMouseY) {
                    // 0.9+0.1 = 1.0 - minimum, must always be 1.0!
                    float Koef = 0.9f + GameConfig().ControlSensivity / 10.0f;

                    // at any real resolution, x and y change with AspectRatio
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



        // additional check not to move beyond boundaries
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






        // find the end point of displacement
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

        // if game engine is present
        if (GameEngineSystem != 0) {
            // execute actions according to control type
            if (GameConfig().Profile[CurrentProfile].SpaceShipControlMode == 1) {
                // arcade mode

                // start maneuvering engine if ship is pulled to one side
                if ((int)sharedPlayerFighter->Location.x > (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = true;
                    PlayerFighterRightEng = false;
                }
                if ((int)sharedPlayerFighter->Location.x < (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = false;
                    PlayerFighterRightEng = true;
                }
                // if not moving, stop maneuvering engines
                if ((int)sharedPlayerFighter->Location.x == (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = false;
                    PlayerFighterRightEng = false;
                }


                // calculate distance
                sVECTOR3D PlayerFighterNewDirection = PlayerFighterEndLocation - sharedPlayerFighter->Location;
                float EndLocationDistance = PlayerFighterNewDirection.Length();

                // calculate movement direction
                PlayerFighterNewDirection.Normalize();

                float SimMoveSpeed = EndLocationDistance;

                if (SimMoveSpeed > 30.0f) {
                    SimMoveSpeed = 30.0f;
                }

                SimMoveSpeed = SimMoveSpeed*4.0f*sharedPlayerFighter->TimeDelta;


                // get current movement
                CurrentMovementVel = PlayerFighterNewDirection^SimMoveSpeed;

                // check
                float MaxSpeed = CurrentMovementVel.Length();
                CurrentMovementVel.Normalize();
                if (MaxSpeed > 30.0f) {
                    MaxSpeed = 30.0f;
                }

                CurrentMovementVel = CurrentMovementVel^MaxSpeed;

            } else {
                // simulation mode


                // start maneuvering engine if ship is pulled to one side
                if ((int)sharedPlayerFighter->Location.x > (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = true;
                    PlayerFighterRightEng = false;
                }
                if ((int)sharedPlayerFighter->Location.x < (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = false;
                    PlayerFighterRightEng = true;
                }
                // if not moving, stop maneuvering engines
                if ((int)sharedPlayerFighter->Location.x == (int)PlayerFighterEndLocation.x) {
                    PlayerFighterLeftEng = false;
                    PlayerFighterRightEng = false;
                }


                // calculate distance
                sVECTOR3D PlayerFighterNewDirection = PlayerFighterEndLocation - sharedPlayerFighter->Location;
                float EndLocationDistance = PlayerFighterNewDirection.Length();

                // calculate movement direction
                PlayerFighterNewDirection.Normalize();

                float SimMoveSpeed = EndLocationDistance;

                if (SimMoveSpeed > sharedPlayerFighter->MaxSpeed) {
                    SimMoveSpeed = sharedPlayerFighter->MaxSpeed;
                }

                SimMoveSpeed = SimMoveSpeed*(sharedPlayerFighter->MaxAcceler/14.0f)*sharedPlayerFighter->TimeDelta;


                // get current movement
                CurrentMovementVel = PlayerFighterNewDirection^SimMoveSpeed;

                // check
                float MaxSpeed = CurrentMovementVel.Length();
                CurrentMovementVel.Normalize();
                if (MaxSpeed > sharedPlayerFighter->MaxSpeed) {
                    MaxSpeed = sharedPlayerFighter->MaxSpeed;
                }

                CurrentMovementVel = CurrentMovementVel^MaxSpeed;

            }
        }

        // displace ship
        sVECTOR3D CurrentVel = sharedPlayerFighter->Location + CurrentMovementVel;
        CurrentVel.y = 0.0f;
        sharedPlayerFighter->SetLocationArcadePlayer(CurrentVel);



        // if standard aspect ratio, move the camera together with the ship
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
    // ship control - shooting
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (GameContentTransp < 0.5f // if click is not in the menu
        && !sharedPlayerFighter->WeaponSlots.empty()) { // if there is a weapon at all

        int PrimCount = 0;
        float PrimTime = 0.0f;
        int SecCount = 0;
        float SecTime = 0.0f;

        PrimaryGroupCurrentFireWeaponDelay -= sharedPlayerFighter->TimeDelta;
        SecondaryGroupCurrentFireWeaponDelay -= sharedPlayerFighter->TimeDelta;

        // find the number of weapons in the group
        for (unsigned i = 0; i < sharedPlayerFighter->WeaponSlots.size(); i++) {
            if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) { // if this weapon is installed

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
            if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) { // if this weapon is installed

                sharedPlayerFighter->WeaponSlots[i].SetFire = false;

                // data arrives in accordance to its group
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

                    // alternative control
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

                    // alternative control
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

                // alternative control
                if (GameConfig().Profile[CurrentProfile].WeaponAltControl[i] == 1
                    && vw_GetKeyStatus(GameConfig().Profile[CurrentProfile].WeaponAltControlData[i])) {
                        sharedPlayerFighter->WeaponSlots[i].SetFire = true;
                }
            }
        }
    }





    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // control and operation of ship’s internal systems
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // TODO:
    // consider what the enging is doing... stand or fly...
    // if not in arcade mode...
    if (GameSpaceShipControlMode != 1) {
        if (CurrentPlayerShipEnergy < GetShipEngineSystemEnergyUse(GameEngineSystem)*sharedPlayerFighter->TimeDelta) {
            sharedPlayerFighter->MaxSpeed = 0.0f;
            sharedPlayerFighter->MaxAcceler = 0.0f;
            sharedPlayerFighter->MaxSpeedRotate = 0.0f;
            // suppress engines
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
            // initiate drawing
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
    // energy for recharging and firing...
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // now receiving all energy for recharging and firing
    // then it would be better to redo the gradual selection of energy
    for (unsigned i = 0; i < sharedPlayerFighter->WeaponSlots.size(); i++) {
        if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
            if (auto sharedWeapon = sharedPlayerFighter->WeaponSlots[i].Weapon.lock()) {
                if (sharedWeapon->CurrentEnergyAccumulated < sharedWeapon->EnergyUse) {
                    // if the energy is not enough to charge weapons
                    if (CurrentPlayerShipEnergy < sharedWeapon->EnergyUse) {
                        // stop charging weapons
                        sharedWeapon->LastFireTime += sharedPlayerFighter->TimeDelta;
                        if (auto sharedFire = sharedWeapon->Fire.lock()) {
                            sharedFire->IsSuppressed = true;
                        }
                    } else {
                        // if energy is enough, everything is fine, we take it and recharge weapons
                        sharedWeapon->CurrentEnergyAccumulated = sharedWeapon->EnergyUse;
                        CurrentPlayerShipEnergy -= sharedWeapon->EnergyUse;
                    }
                }
            }
        }
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // energy supply to other (defense) systems
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (CurrentPlayerShipEnergy >= GetShipEngineSystemEnergyUse(GameEngineSystem)*sharedPlayerFighter->TimeDelta) {

        switch (GameAdvancedProtectionSystem) {
        // nano robots
        case 1:
            // recovery at 0.5% per second
            if (sharedPlayerFighter->ArmorCurrentStatus < sharedPlayerFighter->ArmorInitialStatus) {
                CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem) * sharedPlayerFighter->TimeDelta;
                sharedPlayerFighter->ArmorCurrentStatus += (sharedPlayerFighter->ArmorInitialStatus / 200.0f) * sharedPlayerFighter->TimeDelta;
                if (sharedPlayerFighter->ArmorCurrentStatus > sharedPlayerFighter->ArmorInitialStatus) {
                    sharedPlayerFighter->ArmorCurrentStatus = sharedPlayerFighter->ArmorInitialStatus;
                }
            }
            break;
        // special defensive layer
        case 2:
            break; // doing nothing
        // shield
        case 3:
            // restore completely in 4 seconds
            if (ShildEnergyStatus < 1.0f) {
                CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem) * sharedPlayerFighter->TimeDelta;
                ShildEnergyStatus += 0.02f * sharedPlayerFighter->TimeDelta;
                if (ShildEnergyStatus > 1.0f) {
                    ShildEnergyStatus = 1.0f;
                }
            }
            break;
        // reflector
        case 4:
            // restore completely in 2 seconds
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
    // visualization of deflector shields
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
    // if reactor - may generate energy, if battery - no
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    CurrentPlayerShipEnergy += GetShipRechargeEnergy(GamePowerSystem)*sharedPlayerFighter->TimeDelta;
    if (CurrentPlayerShipEnergy > GetShipMaxEnergy(GamePowerSystem)) {
        CurrentPlayerShipEnergy = GetShipMaxEnergy(GamePowerSystem);
    }

}

} // astromenace namespace
} // viewizard namespace
