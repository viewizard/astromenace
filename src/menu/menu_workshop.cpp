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
#include "../gfx/shadow_map.h"
#include "../command.h"
#include "../object3d/weapon/weapon.h"
#include "../object3d/space_ship/space_ship.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

std::weak_ptr<cSpaceShip> WorkshopFighterGame{};
std::weak_ptr<cSpaceShip> WorkshopNewFighter{};
std::weak_ptr<cWeapon> WorkshopNewWeapon{};
int CurrentWorkshopNewFighter = 1;
int CurrentWorkshopNewWeapon = 1;


sVECTOR3D WorkShopPointCamera;
void WorkshopDrawWeapon(cWeapon *Weapon);
int CurrentWorkshop = 3;


// small deviation/shake for installed weapons
float CurrentDeviation = 0.0f;
float NeedDeviation = vw_fRand0()*5.0f;
float CurrentDeviationSum = 0.0f;
float CurrentTime = 0.0f;

// for blink effect
float CurrentAlert2 = 1.0f;
float CurrentAlert3 = 1.0f;

bool CanDrawWorkshop = false;


void Workshop_Shipyard();
void Workshop_Workshop();
void Workshop_Weaponry();


extern bool DragWeapon;
extern int DragWeaponNum;
extern int DragWeaponLevel;
extern int WeaponSetupSlot;



//------------------------------------------------------------------------------------
// creating a player ship, in the menu (not when buying a new hull!)
//------------------------------------------------------------------------------------
void WorkshopCreateShip(int Num)
{
    // create 3D object
    ReleaseSpaceShip(WorkshopFighterGame);

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    WorkshopFighterGame = CreateEarthSpaceFighter(GameConfig().Profile[CurrentProfile].ShipHull);
    auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
    if (!sharedWorkshopFighterGame) {
        return;
    }

    sharedWorkshopFighterGame->ObjectStatus = eObjectStatus::none;
    sharedWorkshopFighterGame->EngineDestroyType = true;
    sharedWorkshopFighterGame->ShowStatus = false;

    sharedWorkshopFighterGame->ArmorInitialStatus *= GameConfig().Profile[CurrentProfile].ShipHullUpgrade;
    sharedWorkshopFighterGame->ArmorCurrentStatus = GameConfig().Profile[CurrentProfile].ArmorStatus;


    // create weapon (3D objects)
    for (unsigned i = 0; i < sharedWorkshopFighterGame->WeaponSlots.size(); i++) {
        if (GameConfig().Profile[CurrentProfile].Weapon[i]
            && SetEarthSpaceFighterWeapon(WorkshopFighterGame, i + 1, GameConfig().Profile[CurrentProfile].Weapon[i])) {
            if (auto sharedWeapon = sharedWorkshopFighterGame->WeaponSlots[i].Weapon.lock()) {
                // remove light source
                if (auto sharedFire = sharedWeapon->Fire.lock()) {
                    vw_ReleaseLight(sharedFire->Light);
                }

                sharedWeapon->Ammo = GameConfig().Profile[CurrentProfile].WeaponAmmo[i];
                sharedWorkshopFighterGame->WeaponSlots[i].YAngle = -GameConfig().Profile[CurrentProfile].WeaponSlotYAngle[i];

                sVECTOR3D NeedAngle = sharedWorkshopFighterGame->Rotation;
                NeedAngle.y += sharedWorkshopFighterGame->WeaponSlots[i].YAngle;
                sharedWeapon->SetRotation(NeedAngle);
            }
        }
    }


    // create internal systems (that have visual effects)
    SetEarthSpaceFighterEngine(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].EngineSystem);
    SetEarthSpaceFighterArmor(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);

    GameEnemyArmorPenalty = TMPGameEnemyArmorPenalty;
    sharedWorkshopFighterGame->SetLocation(sVECTOR3D{1000.0f,
                                           -1000.0f - (sharedWorkshopFighterGame->Height / 2.0f + sharedWorkshopFighterGame->AABB[6].y),
                                           -(sharedWorkshopFighterGame->Length / 2.0f + sharedWorkshopFighterGame->AABB[6].z)});

    switch (Num) {
    case 1:
        sharedWorkshopFighterGame->SetRotation(sVECTOR3D{0.0f, 150.0f, 0.0f});
        break;
    case 2:
        sharedWorkshopFighterGame->SetRotation(sVECTOR3D{0.0f, 170.0f, 0.0f});
        break;
    case 3:
        sharedWorkshopFighterGame->SetRotation(sVECTOR3D{0.0f, 180.0f, 0.0f});
        break;
    }
}






//------------------------------------------------------------------------------------
// create ship for shopfront
//------------------------------------------------------------------------------------
void WorkshopCreateNewShip()
{
    // create 3D object
    ReleaseSpaceShip(WorkshopNewFighter);

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    WorkshopNewFighter = CreateEarthSpaceFighter(CurrentWorkshopNewFighter);
    auto sharedWorkshopNewFighter = WorkshopNewFighter.lock();
    if (!sharedWorkshopNewFighter) {
        return;
    }

    sharedWorkshopNewFighter->ObjectStatus = eObjectStatus::none;
    sharedWorkshopNewFighter->EngineDestroyType = true;

    GameEnemyArmorPenalty = TMPGameEnemyArmorPenalty;
    sharedWorkshopNewFighter->SetLocation(sVECTOR3D{2000.0f,
                                          -2000.0f - (sharedWorkshopNewFighter->Height / 2.0f + sharedWorkshopNewFighter->AABB[6].y),
                                          -(sharedWorkshopNewFighter->Length / 2.0f + sharedWorkshopNewFighter->AABB[6].z)});

    sharedWorkshopNewFighter->SetRotation(sVECTOR3D{0.0f, -45.0f, 0.0f});
}




//------------------------------------------------------------------------------------
// create weapon for shopfront
//------------------------------------------------------------------------------------
void WorkshopCreateNewWeapon()
{
    ReleaseWeapon(WorkshopNewWeapon);

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    WorkshopNewWeapon = CreateWeapon(CurrentWorkshopNewWeapon);

    auto sharedWeapon = WorkshopNewWeapon.lock();
    if (!sharedWeapon) {
        return;
    }

    sharedWeapon->ObjectStatus = eObjectStatus::none;
    GameEnemyArmorPenalty = TMPGameEnemyArmorPenalty;

    sVECTOR3D Ptmp = sVECTOR3D{0.0f,
                               -(sharedWeapon->Height / 2.0f + sharedWeapon->AABB[6].y),
                               -(sharedWeapon->Length / 2.0f + sharedWeapon->AABB[6].z) - 0.5f};
    vw_RotatePoint(Ptmp, sVECTOR3D{0.0f, -45.0f, 0.0f});

    sharedWeapon->SetLocation(sVECTOR3D{Ptmp.x + 3000.0f,
                                        Ptmp.y - 3000.0f,
                                        Ptmp.z});

    sharedWeapon->SetRotation(sVECTOR3D{0.0f, -45.0f, 0.0f});

    // remove light source
    if (auto sharedFire = sharedWeapon->Fire.lock()) {
        vw_ReleaseLight(sharedFire->Light);
    }
}







//------------------------------------------------------------------------------------
// create all workshop related stuff
//------------------------------------------------------------------------------------
void WorkshopCreate()
{
    // reset all
    CurrentWorkshopNewFighter = 1;
    CurrentWorkshopNewWeapon = 1;
    CurrentDeviation = 0.0f;
    NeedDeviation = vw_fRand0()*5.0f;
    CurrentDeviationSum = 0.0f;
    CurrentTime = 0.0f;
    CurrentAlert2 = 1.0f;
    CurrentAlert3 = 1.0f;

    // don't show in shopfront same ship hull as player already use now
    if (CurrentWorkshopNewFighter == GameConfig().Profile[CurrentProfile].ShipHull) {
        CurrentWorkshopNewFighter++;
    }
    // FIXME 22 - should use some variable with name instead (max weapon number)
    if (CurrentWorkshopNewFighter > 22) {
        CurrentWorkshopNewFighter = 1;
    }

    WorkshopCreateShip(CurrentWorkshop);
    WorkshopCreateNewShip();
    WorkshopCreateNewWeapon();

    // reset custom control setup related switch
    NeedCheck = 0;

    CanDrawWorkshop = true;
}









//------------------------------------------------------------------------------------
// cleanup
//------------------------------------------------------------------------------------
void WorkshopDestroyData()
{
    ReleaseSpaceShip(WorkshopFighterGame);
    ReleaseSpaceShip(WorkshopNewFighter);
    ReleaseWeapon(WorkshopNewWeapon);
}










//------------------------------------------------------------------------------------
// buy/setup/repair for internal systems
//------------------------------------------------------------------------------------
void WorkshopMenu()
{

    // small deviation/shake
    float TimeDelta = vw_GetTimeThread(0) - CurrentTime;
    if (CurrentTime==0.0f) {
        CurrentTime = vw_GetTimeThread(0);
    } else {
        CurrentTime = vw_GetTimeThread(0);
        float Sign = 1.0f;
        if (NeedDeviation < 0.0f) {
            Sign = -1.0f;
        }
        if (Sign == 1.0f) {
            if (NeedDeviation < CurrentDeviationSum) {
                Sign = -1.0f;
            }
        } else {
            if (NeedDeviation > CurrentDeviationSum) {
                Sign = 1.0f;
            }
        }

        CurrentDeviation = Sign*0.7f*TimeDelta;

        if (Sign == 1.0f) {
            if (NeedDeviation <= CurrentDeviationSum+CurrentDeviation) {
                CurrentDeviation -= CurrentDeviationSum+CurrentDeviation-NeedDeviation;
                CurrentDeviationSum += CurrentDeviation;
                NeedDeviation = vw_fRand0()*5.0f;
            } else {
                CurrentDeviationSum += CurrentDeviation;
            }
        } else {
            if (NeedDeviation >= CurrentDeviationSum+CurrentDeviation) {
                CurrentDeviation += CurrentDeviationSum+CurrentDeviation-NeedDeviation;
                CurrentDeviationSum += CurrentDeviation;
                NeedDeviation = vw_fRand0()*5.0f;
            } else {
                CurrentDeviationSum += CurrentDeviation;
            }
        }
    }



    // blink effect related calculations
    CurrentAlert2 += 0.4f*TimeDelta;
    if (CurrentAlert2 > 1.0f) {
        CurrentAlert2 = 0.3f;
    }
    CurrentAlert3 -= 1.9f*TimeDelta;
    if (CurrentAlert3 < 0.1f) {
        CurrentAlert3 = 1.0f;
    }








    // render 3D part
    switch (CurrentWorkshop) {
    // buy/repair ship
    case 1:
        Workshop_Shipyard();
        break;

    // buy/repair internal systems
    case 2:
        Workshop_Workshop();
        break;

    // buy/repair/reload weapons
    case 3:
        Workshop_Weaponry();
        break;
    }






    // buttons

    int X = GameConfig().InternalWidth / 2 - 482;
    int Y = 180+100*5;
    if (DrawButton128_2(X,Y, vw_GetTextUTF32("BACK"), MenuContentTransp, false)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MISSION);
        CanDrawWorkshop = false;
        // reset drag-and-drop mode
        DragWeaponNum = 0;
        DragWeaponLevel = 0;
        DragWeapon = false;
        WeaponSetupSlot = -1;
    }



    X = GameConfig().InternalWidth / 2 - 320;
    bool Off = false;
    if (CurrentWorkshop == 1) {
        Off = true;
    }
    if (DrawButton200_2(X,Y, vw_GetTextUTF32("Shipyard"), MenuContentTransp, Off)) {
        CurrentWorkshop = 1;
        // for each menu we have different 3D objects rotations (for best look)
        WorkshopCreateShip(CurrentWorkshop);
        WorkshopCreateNewShip();
        // reset drag-and-drop mode
        DragWeaponNum = 0;
        DragWeaponLevel = 0;
        DragWeapon = false;
        WeaponSetupSlot = -1;

        if (GameConfig().NeedShowHint[1]) {
            SetCurrentDialogBox(eDialogBox::ShipyardTipsAndTricks);
        }
    }


    X = GameConfig().InternalWidth / 2 - 100;
    Off = false;
    if (CurrentWorkshop == 2) {
        Off = true;
    }
    if (DrawButton200_2(X,Y, vw_GetTextUTF32("Workshop"), MenuContentTransp, Off)) {
        CurrentWorkshop = 2;
        // for each menu we have different 3D objects rotations (for best look)
        WorkshopCreateShip(CurrentWorkshop);
        // reset drag-and-drop mode
        DragWeaponNum = 0;
        DragWeaponLevel = 0;
        DragWeapon = false;
        WeaponSetupSlot = -1;

        if (GameConfig().NeedShowHint[2]) {
            SetCurrentDialogBox(eDialogBox::SystemsTipsAndTricks);
        }
    }


    X = GameConfig().InternalWidth / 2 + 120;
    Off = false;
    if (CurrentWorkshop == 3) {
        Off = true;
    }
    if (DrawButton200_2(X,Y, vw_GetTextUTF32("Weaponry"), MenuContentTransp, Off)) {
        CurrentWorkshop = 3;
        // for each menu we have different 3D objects rotations (for best look)
        WorkshopCreateShip(CurrentWorkshop);
        WorkshopCreateNewWeapon();
        // reset drag-and-drop mode
        DragWeaponNum = 0;
        DragWeaponLevel = 0;
        DragWeapon = false;
        WeaponSetupSlot = -1;

        if (GameConfig().NeedShowHint[3]) {
            SetCurrentDialogBox(eDialogBox::WeaponryTipsAndTricks);
        }
    }



    X = GameConfig().InternalWidth / 2 + 354;
    if (DrawButton128_2(X,Y, vw_GetTextUTF32("START"), MenuContentTransp, false)) {
        if (GameConfig().NeedShowHint[4]) {
            SetCurrentDialogBox(eDialogBox::ShortkeyTipsAndTricks);
        } else {
            MenuContentTransp = 0.98f; // prevent miss double clicking
            // reset drag-and-drop mode
            DragWeaponNum = 0;
            DragWeaponLevel = 0;
            DragWeapon = false;
            WeaponSetupSlot = -1;
            //
            LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
            NeedOffMenu = true;
        }
    }
}










//------------------------------------------------------------------------------------
// 3D menu part rendering
//------------------------------------------------------------------------------------
void WorkshopDrawShip(std::weak_ptr<cSpaceShip> &SpaceShip, int Mode)
{
    if (!CanDrawWorkshop) {
        return;
    }

    auto sharedSpaceShip = SpaceShip.lock();
    if (!sharedSpaceShip) {
        return;
    }

    float tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight;
    vw_GetViewport(&tmpViewportX, &tmpViewportY, &tmpViewportWidth, &tmpViewportHeight);

    if (Mode == 1) {
        WorkShopPointCamera = sVECTOR3D{0.0f, 4.0f, -32.0f};
        sharedSpaceShip->SetRotation(sVECTOR3D{0.0f, 0.0f, CurrentDeviation});
        sharedSpaceShip->SetRotation(sVECTOR3D{0.0f, CurrentDeviation / 2.0f, 0.0f});

        vw_SetViewport((GLint)((GameConfig().InternalWidth / 2 - 512) * (tmpViewportWidth / GameConfig().InternalWidth)),
                       0,
                       (GLsizei)(1024 * (tmpViewportWidth / GameConfig().InternalWidth)),
                       (GLsizei)(768 * (tmpViewportHeight / GameConfig().InternalHeight)));
        vw_ResizeScene(45.0f, 1024.0f/768.0f, 1.0f, 2000.0f);
        vw_Clear(RI_DEPTH_BUFFER);

        vw_LoadIdentity();
        vw_SetCameraLocation(sVECTOR3D{WorkShopPointCamera.x + 1000.0f,
                                       WorkShopPointCamera.y - 1000.0f,
                                       WorkShopPointCamera.z});
        vw_SetCameraMoveAroundPoint(sVECTOR3D{1000.0f, -1000.0f - 6.0f, 0.0f}, 0.0f, sVECTOR3D{0.0f, 0.0f, 0.0f});
        vw_CameraLookAt();


        bool ShadowMap = false;

        if (GameConfig().ShadowMap > 0) {
            float EffectiveDistance = 20.0f;
            ShadowMap_StartRenderToFBO(sVECTOR3D{0.0f, 5.0f, 0.0f}, EffectiveDistance, EffectiveDistance * 2);

            sharedSpaceShip->Draw(true);
            if (!sharedSpaceShip->WeaponSlots.empty()) {
                for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
                    if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                        sharedWeapon->Draw(true);
                    }
                }
            }

            ShadowMap_EndRenderToFBO();
            ShadowMap = true;
            ShadowMap_StartFinalRender();
        }

        sharedSpaceShip->Draw(false, ShadowMap);
        if (!sharedSpaceShip->WeaponSlots.empty()) {
            for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    sharedWeapon->Draw(false, ShadowMap);
                }
            }
        }

        if (GameConfig().ShadowMap > 0) {
            ShadowMap_EndFinalRender();
        }

        // render engines visual effects for this ship only
        vw_DrawParticleSystems(sharedSpaceShip->Engines);

        vw_SetCameraLocation(sVECTOR3D{-50.0f, 30.0f, -50.0f});
        vw_SetViewport(tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight);
        vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);
        return;
    }



    if (Mode == 4) {
        WorkShopPointCamera = sVECTOR3D{0.0f, 35.0f, -0.01f};
        sharedSpaceShip->SetRotation(sVECTOR3D{0.0f, 0.0f, CurrentDeviation});
        vw_SetViewport((GLint)((GameConfig().InternalWidth / 2) * (tmpViewportWidth / GameConfig().InternalWidth)),
                       (GLint)(30 * (tmpViewportHeight / GameConfig().InternalHeight)),
                       (GLsizei)(512 * (tmpViewportWidth / GameConfig().InternalWidth)),
                       (GLsizei)(638 * (tmpViewportHeight / GameConfig().InternalHeight)));
        vw_ResizeScene(45.0f, 512.0f/608.0f, 1.0f, 2000.0f);
        vw_Clear(RI_DEPTH_BUFFER);
        vw_LoadIdentity();
        vw_SetCameraLocation(sVECTOR3D{WorkShopPointCamera.x + 1000.0f,
                                       WorkShopPointCamera.y - 1000.0f,
                                       WorkShopPointCamera.z});
        vw_SetCameraMoveAroundPoint(sVECTOR3D{1000.0f, -1000.0f, 0.0f}, 0.0f, sVECTOR3D{0.0f, 180.0f, 0.0f});
        vw_CameraLookAt();


        bool ShadowMap = false;

        if (GameConfig().ShadowMap > 0) {
            float EffectiveDistance = 20.0f;
            ShadowMap_StartRenderToFBO(sVECTOR3D{0.0f, 0.0f, 0.0f}, EffectiveDistance, EffectiveDistance*2);

            sharedSpaceShip->Draw(true);
            if (!sharedSpaceShip->WeaponSlots.empty()) {
                for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
                    if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                        sharedWeapon->Draw(true);
                    }
                }
            }

            ShadowMap_EndRenderToFBO();
            ShadowMap = true;
            ShadowMap_StartFinalRender();
        }

        sharedSpaceShip->Draw(false, ShadowMap);
        if (!sharedSpaceShip->WeaponSlots.empty()) {
            for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    sharedWeapon->Draw(false, ShadowMap);
                }
            }
        }

        if (GameConfig().ShadowMap > 0) {
            ShadowMap_EndFinalRender();
        }

        // render engines visual effects for this ship only
        vw_DrawParticleSystems(sharedSpaceShip->Engines);

        vw_SetCameraLocation(sVECTOR3D{-50.0f, 30.0f, -50.0f});
        vw_SetViewport(tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight);
        vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);
        return;
    }






    if (Mode == 3) {
        WorkShopPointCamera = sVECTOR3D{0.0f, 10.0f, -34.0f};
        sharedSpaceShip->SetRotation(sVECTOR3D{0.0f, CurrentDeviation / 2.0f, 0.0f});
        vw_SetViewport((GLint)((GameConfig().InternalWidth / 2) * (tmpViewportWidth / GameConfig().InternalWidth)),
                       0,
                       (GLsizei)(512 * (tmpViewportWidth / GameConfig().InternalWidth)),
                       (GLsizei)(512 * (tmpViewportHeight / GameConfig().InternalHeight)));
        vw_ResizeScene(45.0f, 512.0f/512.0f, 1.0f, 2000.0f);
        vw_Clear(RI_DEPTH_BUFFER);
        vw_LoadIdentity();
        vw_SetCameraLocation(sVECTOR3D{WorkShopPointCamera.x / 1.2f + 1000.0f,
                                       WorkShopPointCamera.y / 1.2f - 1000.0f,
                                       WorkShopPointCamera.z / 1.2f});
        vw_SetCameraMoveAroundPoint(sVECTOR3D{1000.0f, -1000.0f - sharedSpaceShip->AABB[6].y - sharedSpaceShip->Height / 3.0f, 0.0f}, 0.0f, sVECTOR3D{0.0f, 0.0f, 0.0f});
    }
    if (Mode == 2) {
        WorkShopPointCamera = sVECTOR3D{0.0f, 10.0f, -34.0f};
        sVECTOR3D PointCameraTMP = WorkShopPointCamera;
        vw_RotatePoint(PointCameraTMP, sVECTOR3D{0.0f, -90.0f, 0.0f});
        sharedSpaceShip->SetRotation(sVECTOR3D{0.0f, CurrentDeviation / 2.0f, 0.0f});
        vw_SetViewport((GLint)((GameConfig().InternalWidth / 2 - 512) * (tmpViewportWidth / GameConfig().InternalWidth)),
                       0,
                       (GLsizei)(512 * (tmpViewportWidth / GameConfig().InternalWidth)),
                       (GLsizei)(512 * (tmpViewportHeight / GameConfig().InternalHeight)));
        vw_ResizeScene(45.0f, 512.0f/512.0f, 1.0f, 2000.0f);
        vw_Clear(RI_DEPTH_BUFFER);
        vw_LoadIdentity();
        vw_SetCameraLocation(sVECTOR3D{PointCameraTMP.x / 1.2f + 2000.0f,
                                       PointCameraTMP.y / 1.2f - 2000.0f,
                                       PointCameraTMP.z / 1.2f});
        vw_SetCameraMoveAroundPoint(sVECTOR3D{2000.0f, -2000.0f - sharedSpaceShip->AABB[6].y - sharedSpaceShip->Height / 3.0f, 0.0f}, 0.0f, sVECTOR3D{0.0f, 170.0f, 0.0f});
    }
    vw_CameraLookAt();


    bool ShadowMap = false;

    if (GameConfig().ShadowMap > 0) {
        float EffectiveDistance = 20.0f;
        ShadowMap_StartRenderToFBO(sVECTOR3D{0.0f, -2.0f, 0.0f}, EffectiveDistance, EffectiveDistance * 2.0f);

        sharedSpaceShip->Draw(true);
        if (!sharedSpaceShip->WeaponSlots.empty()) {
            for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    sharedWeapon->Draw(true);
                }
            }
        }

        ShadowMap_EndRenderToFBO();
        ShadowMap = true;
        ShadowMap_StartFinalRender();
    }

    sharedSpaceShip->Draw(false, ShadowMap);

    if (!sharedSpaceShip->WeaponSlots.empty()) {
        for (auto &tmpWeaponSlot : sharedSpaceShip->WeaponSlots) {
            if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                sharedWeapon->Draw(false, ShadowMap);
            }
        }
    }

    if (GameConfig().ShadowMap > 0) {
        ShadowMap_EndFinalRender();
    }

    // render engines visual effects for this ship only
    vw_DrawParticleSystems(sharedSpaceShip->Engines);

    vw_SetCameraLocation(sVECTOR3D{-50.0f, 30.0f, -50.0f});
    vw_SetViewport(tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight);
    vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);

}


void WorkshopDrawWeapon(cWeapon *Weapon)
{
    if (!CanDrawWorkshop || Weapon == nullptr) {
        return;
    }

    float tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight;
    vw_GetViewport(&tmpViewportX, &tmpViewportY, &tmpViewportWidth, &tmpViewportHeight);

    // correct distance to weapon in shopfront according to weapon size
    if (Weapon->InternalType == 18 || Weapon->InternalType == 19) {
        WorkShopPointCamera = sVECTOR3D{0.0f, 1.25f, -5.0f};
    } else {
        WorkShopPointCamera = sVECTOR3D{0.0f, 1.0f, -4.0f};
    }

    sVECTOR3D PointCameraTMP = WorkShopPointCamera;
    vw_RotatePoint(PointCameraTMP, sVECTOR3D{0.0f, -90.0f, 0.0f});

    Weapon->SetRotation(sVECTOR3D{0.0f, CurrentDeviation / 2.0f, 0.0f});
    vw_SetViewport((GLint)((GameConfig().InternalWidth / 2 - 448) * (tmpViewportWidth / GameConfig().InternalWidth)),
                   (GLint)(105 * (tmpViewportHeight / GameConfig().InternalHeight)),
                   (GLsizei)(384 * (tmpViewportWidth / GameConfig().InternalWidth)),
                   (GLsizei)(350 * (tmpViewportHeight / GameConfig().InternalHeight)));
    vw_ResizeScene(45.0f, 384.0f/350.0f, 1.0f, 2000.0f);
    vw_Clear(RI_DEPTH_BUFFER);
    vw_LoadIdentity();
    vw_SetCameraLocation(sVECTOR3D{PointCameraTMP.x + 3000.0f,
                                   PointCameraTMP.y - 3000.0f,
                                   PointCameraTMP.z});
    vw_SetCameraMoveAroundPoint(sVECTOR3D{3000.0f, -3000.0f, 0.0f}, 0.0f, sVECTOR3D{0.0f, 170.0f, 0.0f});
    vw_CameraLookAt();

    Weapon->Draw(false);

    vw_SetCameraLocation(sVECTOR3D{-50.0f, 30.0f, -50.0f});
    vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);
    vw_SetViewport(tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight);
}

} // astromenace namespace
} // viewizard namespace
