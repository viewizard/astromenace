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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../core/core.h"
#include "../config/config.h"
#include "../ui/cursor.h"
#include "../ui/game_speed.h"
#include "../ui/game/text.h"
#include "../ui/game/stopwatch.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../gfx/star_system.h"
#include "../gfx/shadow_map.h"
#include "../script/script.h"
#include "../object3d/space_ship/space_ship.h"
#include "../command.h"
#include "../game/camera.h"
#include "../game/weapon_panel.h"
#include "../game/hud.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include "SDL2/SDL.h"
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

std::unique_ptr<cMissionScript> MissionScript{};

eCommand GameExitCommand{eCommand::DO_NOTHING};

} // unnamed namespace







// enemy fire rate and bullet speed penalty, range 1-3
int GameEnemyWeaponPenalty = 1;
// enemy armor amount penalty
int GameEnemyArmorPenalty = 1;
// enemy targeting speed (weapon rotation) penalty, range 1-4
int GameEnemyTargetingSpeedPenalty = 1;
// 0 - limited, 1 - unlimited
int GameUnlimitedAmmo = 0;
// 0 - can be destroyed, 1 - can't be destroyed
int GameUndestroyableWeapon = 0;
// 1 - arcade, 0 - sim
int GameWeaponTargetingMode = 0;
// 1 - arcade, 0 - sim
int GameSpaceShipControlMode = 0;

int GameEngineSystem = 1;
int GameTargetingSystem = 1;
int GameAdvancedProtectionSystem = 0;
int GamePowerSystem = 1;
int GameTargetingMechanicSystem = 1;

// temporary money and experience (gathered during mission), added to profile only at mission complete
float GameMoney = 0;
float GameExperience = 0;

int AlienShipsKillQuant;
float AlienShipsKillBonus;
int AlienMotherShipsKillQuant;
float AlienMotherShipsKillBonus;
int PirateShipsKillQuant;
float PirateShipsKillBonus;
int PirateVehiclesKillQuant;
float PirateVehiclesKillBonus;
int PirateBuildingsKillQuant;
float PirateBuildingsKillBonus;
int AsteroidsKillQuant;
float AsteroidsKillBonus;


// mission complete status + dialog box status
bool GameMissionCompleteStatus = false;
bool GameMissionCompleteStatusShowDialog = false;

// player ship "holder" veriable
std::weak_ptr<cSpaceShip> PlayerFighter{};



// in-game menu related variable
bool GameMenu = false;
float GameContentTransp = 0.0f;
float LastGameUpdateTime = 0.0f;
eGameMenuStatus GameMenuStatus = eGameMenuStatus::GAME_MENU;

float GameButton1Transp = 1.0f;
float LastGameButton1UpdateTime = 0.0f;
float GameButton2Transp = 1.0f;
float LastGameButton2UpdateTime = 0.0f;
float GameButton3Transp = 1.0f;
float LastGameButton3UpdateTime = 0.0f;
float GameButton4Transp = 1.0f;
float LastGameButton4UpdateTime = 0.0f;
bool NeedShowGameMenu = false;
bool NeedHideGameMenu = false;


// alarm's blinking internal counters
extern float CurrentAlert2;
extern float CurrentAlert3;
extern float CurrentTime;

// player ship related stuff
void InitGamePlayerShip();
void GamePlayerShip();
float GetShipMaxEnergy(int Num);
extern float CurrentPlayerShipEnergy;
extern int LastMouseX;
extern int LastMouseY;
extern int LastMouseXR;
extern int LastMouseYR;

// shield and deflector "holder" veriables
extern std::weak_ptr<cParticleSystem> Shild1;
extern std::weak_ptr<cParticleSystem> Shild2;


unsigned int SoundShowHideMenu{0};


float LastGameOnOffUpdateTime = 0.0f;
float GameBlackTransp = 0.0f;
bool NeedOnGame = false;
bool NeedOffGame = false;




//------------------------------------------------------------------------------------
// game initialization
//------------------------------------------------------------------------------------
void InitGame()
{
    ShadowMap_SizeSetup(eShadowMapSetup::Game);

    if (CurrentProfile < 0 || CurrentProfile > 4) {
        CurrentProfile = 0;
    }
    if (CurrentMission == -1) {
        CurrentMission = 0;
    }


    GameEnemyWeaponPenalty = GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty;
    GameEnemyArmorPenalty = GameConfig().Profile[CurrentProfile].EnemyArmorPenalty;
    GameEnemyTargetingSpeedPenalty = GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty;
    GameUnlimitedAmmo = GameConfig().Profile[CurrentProfile].UnlimitedAmmo;
    GameUndestroyableWeapon = GameConfig().Profile[CurrentProfile].UndestroyableWeapon;
    GameWeaponTargetingMode = GameConfig().Profile[CurrentProfile].WeaponTargetingMode;
    GameSpaceShipControlMode = GameConfig().Profile[CurrentProfile].SpaceShipControlMode;

    GameEngineSystem = GameConfig().Profile[CurrentProfile].EngineSystem;
    // for sim ship control mode, we need limit ship movements to engine capabilities
    if (GameSpaceShipControlMode == 1 && GameEngineSystem == 0) {
            GameEngineSystem = 1;
    }

    GameTargetingSystem = GameConfig().Profile[CurrentProfile].TargetingSystem;
    GameAdvancedProtectionSystem = GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem;
    GamePowerSystem = GameConfig().Profile[CurrentProfile].PowerSystem;
    GameTargetingMechanicSystem = GameConfig().Profile[CurrentProfile].TargetingMechanicSystem;
    GameMoney = GameConfig().Profile[CurrentProfile].Money * 1.0f;

    GameExperience = (GameConfig().Profile[CurrentProfile].Experience - GameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission]) * 1.0f;

    // grab mouse control for both - windows and fullscren mode (need this for multi-monitor systems)
    SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), SDL_TRUE);


    vw_ResetMouseButtons();
    // set mouse position to center
    float tmpViewportWidth, tmpViewportHeight;
    vw_GetViewport(nullptr, nullptr, &tmpViewportWidth, &tmpViewportHeight);
    SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), tmpViewportWidth / 2, tmpViewportHeight / 2);
    SetShowGameCursor(false);

    LastMouseXR = 0;
    LastMouseYR = 0;
    SDL_GetMouseState(&LastMouseXR, &LastMouseYR);



    cGameSpeed::GetInstance().InitGameSpeed();
    cStopwatch::GetInstance().Reset(false); // should be after cGameSpeed init



    // camera initialization, must be before script
    InitCamera();

    InitGamePlayerShip();



    StarSystemInitByType(eDrawType::GAME); // should be before RunScript()

    MissionScript.reset(new cMissionScript);
    if (!MissionScript->RunScript(GetCurrentMissionFileName(), vw_GetTimeThread(1))) {
        MissionScript.reset();
    }


    SetupMissionNumberText(3.0f, CurrentMission + 1);
    SetupMissionFailedText(0.0f); // reset previous status on game restart after fail


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // "scroll" a little so space will be looks filled immediately upon game start
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);
    vw_SetCameraLocation(sVECTOR3D{0.0f, 65.0f, -100.0f + 10.0f});
    vw_SetCameraMoveAroundPoint(sVECTOR3D{0.0f, 0.0f, 10.0f}, 0.0f, sVECTOR3D{0.0f, 0.0f, 0.0f});


    InitHUD(PlayerFighter,
            GamePowerSystem ? (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)) : 0.0f,
            GameConfig().Profile[CurrentProfile].Experience -
            GameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission],
            GameConfig().Profile[CurrentProfile].Money);



    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // game menu initialization
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    GameContentTransp = 0.0f;
    LastGameUpdateTime = vw_GetTimeThread(0);
    GameButton1Transp = 1.0f;
    LastGameButton1UpdateTime = 0.0f;
    GameButton2Transp = 1.0f;
    LastGameButton2UpdateTime = 0.0f;
    GameButton3Transp = 1.0f;
    LastGameButton3UpdateTime = 0.0f;
    GameButton4Transp = 1.0f;
    LastGameButton4UpdateTime = 0.0f;
    GameMenu = false;
    NeedShowGameMenu = false;
    NeedHideGameMenu = false;

    GameMenuStatus = eGameMenuStatus::GAME_MENU;

    AlienShipsKillQuant = 0;
    AlienShipsKillBonus = 0.0f;
    AlienMotherShipsKillQuant = 0;
    AlienMotherShipsKillBonus = 0.0f;
    PirateShipsKillQuant = 0;
    PirateShipsKillBonus = 0.0f;
    PirateVehiclesKillQuant = 0;
    PirateVehiclesKillBonus = 0.0f;
    PirateBuildingsKillQuant = 0;
    PirateBuildingsKillBonus = 0.0f;
    AsteroidsKillQuant = 0;
    AsteroidsKillBonus = 0.0f;

    CurrentTime = vw_GetTimeThread(0);
    CurrentAlert2 = 1.0f;
    CurrentAlert3 = 1.0f;

    MenuStatus = eMenuStatus::GAME;

    GameMissionCompleteStatus = false;
    GameMissionCompleteStatusShowDialog = false;

    SoundShowHideMenu = 0;

    LastGameOnOffUpdateTime = vw_GetTimeThread(0);
    GameBlackTransp = 1.0f;
    NeedOnGame = true;
}







//------------------------------------------------------------------------------------
// Exit game
//------------------------------------------------------------------------------------
void ExitGame(eCommand Command)
{
    GameExitCommand = Command;
    NeedOffGame = true;
    LastGameOnOffUpdateTime = vw_GetTimeThread(0);

    // remove game menu
    if (GameMenu) {
        GameMenu = false;
        NeedShowGameMenu = false;
        NeedHideGameMenu = true;
        SetShowGameCursor(false);
        // restore last mouse position for main menu
        SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), LastMouseXR, LastMouseYR);
    }
}
void RealExitGame()
{
    ReleaseSpaceShip(PlayerFighter);

    vw_ReleaseAllParticleSystems2D();

    vw_ReleaseParticleSystem(Shild1);
    vw_ReleaseParticleSystem(Shild2);

    // release mouse control
    SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), SDL_FALSE);
}

//------------------------------------------------------------------------------------
// Exit game, save all data
//------------------------------------------------------------------------------------
void ExitGameWithSave(eCommand Command)
{
    ChangeGameConfig().Profile[CurrentProfile].Money = static_cast<int>(GameMoney);

    // in case player replayed mission, save best results only
    if (GameConfig().Profile[CurrentProfile].Experience < static_cast<int>(GameExperience)) {
        int Incr = static_cast<int>(GameExperience) - GameConfig().Profile[CurrentProfile].Experience;
        ChangeGameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission] += Incr;
        ChangeGameConfig().Profile[CurrentProfile].Experience += Incr;
    }
    ChangeGameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission]++;

    if (auto sharedPlayerFighter = PlayerFighter.lock()) {
        ChangeGameConfig().Profile[CurrentProfile].ArmorStatus = sharedPlayerFighter->ArmorCurrentStatus;

        for (unsigned i = 0; i < sharedPlayerFighter->WeaponSlots.size(); i++) {
            if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
                if (auto sharedWeapon = sharedPlayerFighter->WeaponSlots[i].Weapon.lock()) {
                    if (sharedWeapon->ArmorCurrentStatus <= 0.0f) { // weapon was destroyed on mission
                        ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = 0;
                        ChangeGameConfig().Profile[CurrentProfile].Weapon[i] = 0;
                    } else {
                        ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = sharedWeapon->Ammo;
                    }
                }
            }
        }
    }

    // set next mission as current
    CurrentMission++;
    if (GameConfig().Profile[CurrentProfile].OpenLevelNum < CurrentMission) {
        ChangeGameConfig().Profile[CurrentProfile].OpenLevelNum = CurrentMission;
    }

    // if this is was last mission, don't set any mission as current (let player chose)
    if (CurrentMission > AllMission - 1) {
        CurrentMission = -1;
        // that was last mission - switch to Credits (show authors) instead of Main Menu
        Command = eCommand::SWITCH_FROM_GAME_TO_CREDITS;
    }

    vw_ResetWheelStatus();
    // set proper mission list, according to current mission
    StartMission = 0;
    EndMission = 4;
    if (CurrentMission != -1 && CurrentMission > 2) { // move range start and end, so, selected element will be in the middle of the list
        StartMission = CurrentMission-2;
        EndMission = CurrentMission+2;

        if (CurrentMission >= AllMission-2) {
            StartMission = AllMission-5;
            EndMission = AllMission-1;
        }
    }


    ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;

    ExitGame(Command);
}






//------------------------------------------------------------------------------------
// Mission complete
//------------------------------------------------------------------------------------
void SetGameMissionComplete()
{
    // player failed, mission was completed after player ship destroyed
    // this happens, when player ship was destroyed and game scroll some part of mission (and reach end of mission)
    if (PlayerFighter.expired()) {
        return;
    }
    GameMissionCompleteStatus = true;
    GameMissionCompleteStatusShowDialog = true;
}






//------------------------------------------------------------------------------------
// draw game, main method
//------------------------------------------------------------------------------------
void DrawGame()
{

    float TimeDelta = vw_GetTimeThread(0) - CurrentTime;
    CurrentTime = vw_GetTimeThread(0);


    // blink effect related calculations
    CurrentAlert2 -= TimeDelta;
    if (CurrentAlert2 < 0.1f) {
        CurrentAlert2 = 1.0f;
    }
    CurrentAlert3 -= 1.9f * TimeDelta;
    if (CurrentAlert3 < 0.1f) {
        CurrentAlert3 = 1.0f;
    }




    CameraUpdate(vw_GetTimeThread(1));
    vw_CameraLookAt();


    // skybox + far-far away space first
    StarSystemDraw(eDrawType::GAME);


    // render 3D objets
    DrawAllObject3D(eDrawType::GAME);


    // update 3D objects data + particle systems data
    UpdateAllObject3D(vw_GetTimeThread(1));
    vw_UpdateAllParticleSystems(vw_GetTimeThread(1));

    // collision detection
    if (GameContentTransp < 0.99f) {
        DetectCollisionAllObject3D();
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // script
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (MissionScript && !MissionScript->Update(vw_GetTimeThread(1))) {
        MissionScript.reset();
    }







    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 2d part
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    vw_Start2DMode(-1,1);





    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // handling the state of the player's ship
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    GamePlayerShip();


    UpdateHUD(PlayerFighter,
              GamePowerSystem ? (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)) : 0.0f);
    DrawHUD();
    DrawWeaponPanels(PlayerFighter); // (?) part of HUD

    cGameSpeed::GetInstance().Draw();


    DrawMissionNumberText();
    DrawMissionFailedText();




    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Draw menu, must be the last one
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // smooth fade-in
    if (NeedShowGameMenu) {
        GameContentTransp += 2.0f*(vw_GetTimeThread(0)-LastGameUpdateTime);
        if (GameContentTransp >= 1.0f) {
            GameContentTransp = 1.0f;
            NeedShowGameMenu = false;
            SetShowGameCursor(true);

            // release mouse control
            SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), SDL_FALSE);
            SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), LastMouseXR, LastMouseYR);
        }
        // smooth increase game speed
        if (GameContentTransp != 0.0f) {
            cGameSpeed::GetInstance().SetThreadSpeed((1.0f - GameContentTransp) * GameConfig().GameSpeed);
        }
    }
    // smooth fade-out
    if (NeedHideGameMenu) {
        GameContentTransp -= vw_GetTimeThread(0) - LastGameUpdateTime;
        if (GameContentTransp <= 0.0f) {
            GameContentTransp = 0.0f;
            NeedHideGameMenu = false;
            GameMenuStatus = eGameMenuStatus::GAME_MENU;

            // grab mouse control for both - windows and fullscren mode (need this for multi-monitor systems)
            SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), SDL_TRUE);
            SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), LastMouseXR, LastMouseYR);
        }
        // set game speed to zero (should looks like all 3D objects are freeze in time)
        cGameSpeed::GetInstance().SetThreadSpeed((1.0f - GameContentTransp) * GameConfig().GameSpeed);
    }
    LastGameUpdateTime = vw_GetTimeThread(0);

    // in-game menu
    if (GameContentTransp > 0.0f) {
        if (GameMissionCompleteStatus) {
            // back
            sRECT SrcRect(2, 2, 564-2, 564-2);
            sRECT DstRect(GameConfig().InternalWidth / 2 - 256 - 26, 128 - 28, GameConfig().InternalWidth / 2 - 256 + 534, 128 + 532);
            constexpr unsigned tmpHash1 = constexpr_hash_djb2a("menu/dialog512_512.tga");
            vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash1),
                      true, GameContentTransp);
            // title
            int Size = vw_TextWidthUTF32(vw_GetTextUTF32("Mission Complete"));
            float WScale = 0;
            if (Size > 190) {
                Size = 190;
                WScale = -190;
            }
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 123 - Size / 2, 128+21, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, 0.7f * GameContentTransp, vw_GetTextUTF32("Mission Complete"));


            int Y = 128+90;
            int Prir = 36;

            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, 0.5f * GameContentTransp, vw_GetTextUTF32("Type"));
            Size = vw_TextWidthUTF32(vw_GetTextUTF32("Killed"));
            WScale = 0;
            if (Size > 70) {
                Size = 70;
                WScale = -70;
            }
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 31 + Size / 2, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, 0.5f * GameContentTransp, vw_GetTextUTF32("Killed"));
            Size = vw_TextWidthUTF32(vw_GetTextUTF32("Bonus"));
            WScale = 0;
            if (Size > 70) {
                Size = 70;
                WScale = -70;
            }
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 + 97 + Size / 2, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, 0.5f * GameContentTransp, vw_GetTextUTF32("Bonus"));
            Y += Prir;

            WScale = -210;

            // FIXME ostringstream is not so fast, move all string initialization into setup,
            //       all ostringstream-related code should be called only one time in dialog init
            std::ostringstream tmpStream;

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << AlienShipsKillQuant;
            std::string AlienShipsKillQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << AlienShipsKillBonus;
            std::string AlienShipsKillBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << AlienMotherShipsKillQuant;
            std::string AlienMotherShipsKillQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << AlienMotherShipsKillBonus;
            std::string AlienMotherShipsKillBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << PirateShipsKillQuant;
            std::string PirateShipsKillQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << PirateShipsKillBonus;
            std::string PirateShipsKillBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << PirateVehiclesKillQuant;
            std::string PirateVehiclesKillQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << PirateVehiclesKillBonus;
            std::string PirateVehiclesKillBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << PirateBuildingsKillQuant;
            std::string PirateBuildingsKillQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << PirateBuildingsKillBonus;
            std::string PirateBuildingsKillBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << AsteroidsKillQuant;
            std::string AsteroidsKillQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << AsteroidsKillBonus;
            std::string AsteroidsKillBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            tmpStream << std::fixed << std::setw(4) << std::setfill('0') << AlienShipsKillQuant +
                      AlienMotherShipsKillQuant + PirateShipsKillQuant + PirateVehiclesKillQuant +
                      PirateBuildingsKillQuant + AsteroidsKillQuant;
            std::string SummaryQuantString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::fixed << std::setw(6) << std::setprecision(0) << std::setfill('0') << GameMoney - GameConfig().Profile[CurrentProfile].Money;
            std::string SummaryBonusString{tmpStream.str()};
            tmpStream.clear();
            tmpStream.str(std::string{});

            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp,  vw_GetTextUTF32("Alien Spaceships"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, AlienShipsKillQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, AlienShipsKillBonusString);
            Y += Prir;
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, vw_GetTextUTF32("Alien Motherships"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, AlienMotherShipsKillQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, AlienMotherShipsKillBonusString);
            Y += Prir;
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, vw_GetTextUTF32("Pirate Spaceships"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, PirateShipsKillQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, PirateShipsKillBonusString);
            Y += Prir;
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, vw_GetTextUTF32("Pirate Vehicles"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, PirateVehiclesKillQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, PirateVehiclesKillBonusString);
            Y += Prir;
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, vw_GetTextUTF32("Pirate Buildings"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, PirateBuildingsKillQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, PirateBuildingsKillBonusString);
            Y += Prir;
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, vw_GetTextUTF32("Asteroids"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, AsteroidsKillQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, AsteroidsKillBonusString);

            Y += (int)(Prir*1.5);
            vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, GameContentTransp, vw_GetTextUTF32("Total"));
            vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, SummaryQuantString);
            vw_DrawText(GameConfig().InternalWidth / 2+126, Y, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, GameContentTransp, SummaryBonusString);

            // button
            int X = GameConfig().InternalWidth / 2 - 192;
            Y = 545;
            if (DrawButton384(X,Y, vw_GetTextUTF32("NEXT"), GameContentTransp, GameButton4Transp, LastGameButton4UpdateTime)) {
                ExitGameWithSave(eCommand::SWITCH_FROM_GAME_TO_MISSION_MENU);
            }
        } else {
            switch (GameMenuStatus) {
            // in-game main menu
            case eGameMenuStatus::GAME_MENU: {
                // back
                sRECT SrcRect(2, 2, 564-2, 564-2);
                sRECT DstRect(GameConfig().InternalWidth / 2 - 256+4-30, 128+2-30, GameConfig().InternalWidth / 2 - 256+564-30, 128+564-2-30);
                constexpr unsigned tmpHash2 = constexpr_hash_djb2a("menu/dialog512_512.tga");
                vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash2),
                          true, GameContentTransp);
                // title
                int SizeI = 17 + (234-vw_TextWidthUTF32(vw_GetTextUTF32("GAME MENU")))/2;
                vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - 256 + SizeI, 128+22, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, 0.7f * GameContentTransp, vw_GetTextUTF32("GAME MENU"));

                // buttons


                int X = GameConfig().InternalWidth / 2 - 192;
                int Y = 225;
                int Prir = 100;

                if (DrawButton384(X,Y, vw_GetTextUTF32("RESUME"), GameContentTransp, GameButton1Transp, LastGameButton1UpdateTime)) {
                    GameMenu = false;
                    NeedShowGameMenu = false;
                    NeedHideGameMenu = true;
                    SetShowGameCursor(false);
                    // set mouse position to last menu mouse position
                    SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), LastMouseXR, LastMouseYR);

                    if (vw_IsSoundAvailable(SoundShowHideMenu)) {
                        vw_StopSound(SoundShowHideMenu, 150);
                    }
                    SoundShowHideMenu = PlayMenuSFX(eMenuSFX::MissionHideMenu, 1.0f);
                }

                Y = Y+Prir;
                if (DrawButton384(X,Y, vw_GetTextUTF32("OPTIONS"), GameContentTransp, GameButton2Transp, LastGameButton2UpdateTime)) {
                    SetOptionsMenu(eMenuStatus::OPTIONS);
                    GameMenuStatus = eGameMenuStatus::OPTIONS;
                }

                Y = Y+Prir;
                if (DrawButton384(X,Y, vw_GetTextUTF32("RESTART"), GameContentTransp, GameButton3Transp, LastGameButton3UpdateTime)) {
                    if (PlayerFighter.expired()) {
                        ExitGame(eCommand::SWITCH_FROM_MENU_TO_GAME);
                    } else {
                        SetCurrentDialogBox(eDialogBox::RestartLevelNoSave);
                    }
                }

                Y = Y+Prir;
                if (DrawButton384(X,Y, vw_GetTextUTF32("QUIT"), GameContentTransp, GameButton4Transp, LastGameButton4UpdateTime)) {
                    if (PlayerFighter.expired()) {
                        ExitGame(eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU);
                    } else {
                        SetCurrentDialogBox(eDialogBox::QuiToMenuNoSave);
                    }
                }

                break;
            }

            case eGameMenuStatus::OPTIONS:
                OptionsMenu(GameContentTransp, GameButton1Transp, LastGameButton1UpdateTime, GameButton2Transp, LastGameButton2UpdateTime);
                break;
            case eGameMenuStatus::OPTIONS_ADVANCED:
                OptionsAdvMenu(GameContentTransp, GameButton1Transp, LastGameButton1UpdateTime, GameButton2Transp, LastGameButton2UpdateTime);
                break;
            case eGameMenuStatus::INTERFACE:
                InterfaceMenu(GameContentTransp, GameButton1Transp, LastGameButton1UpdateTime);
                break;
            case eGameMenuStatus::CONFCONTROL:
                ConfControlMenu(GameContentTransp, GameButton1Transp, LastGameButton1UpdateTime);
                break;


            }


            // show "Pause" text
            sRECT SrcRect(0, 0, 256, 64);
            sRECT DstRect(GameConfig().InternalWidth - 256 + 60, 768 - 54, GameConfig().InternalWidth + 60, 768 + 10);
            if (GameContentTransp == 1.0f) {
                vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(hash_djb2a(vw_GetText("lang/en/game/pause.tga").c_str())), true, CurrentAlert2*GameContentTransp);
            } else {
                vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(hash_djb2a(vw_GetText("lang/en/game/pause.tga").c_str())), true, GameContentTransp);
            }

        }

    }






    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // open menu
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (!isDialogBoxDrawing()) {
        if (!PlayerFighter.expired()) {
            if (vw_GetKeyStatus(SDLK_ESCAPE) || GameMissionCompleteStatusShowDialog) {
                bool NeedPlaySfx = true;
                if (GameMissionCompleteStatusShowDialog) {
                    if (GameMenu) {
                        NeedPlaySfx = false;
                    } else {
                        GameMenu = true;
                    }
                } else {
                    GameMenu = !GameMenu;
                }

                if (GameMenu && (!GameMissionCompleteStatus || GameMissionCompleteStatusShowDialog)) {
                    NeedShowGameMenu = true;
                    NeedHideGameMenu = false;
                    if (NeedPlaySfx && vw_IsSoundAvailable(SoundShowHideMenu)) {
                        vw_StopSound(SoundShowHideMenu, 150);
                    }
                    if (NeedPlaySfx) {
                        SoundShowHideMenu = PlayMenuSFX(eMenuSFX::MissionShowMenu, 1.0f);
                    }
                    // reset mouse click, prevent miss clicking here
                    vw_GetMouseLeftClick(true);
                } else if (!GameMenu && !GameMissionCompleteStatus) { // if menu with mission results are opened - don't allow close it
                    NeedShowGameMenu = false;
                    NeedHideGameMenu = true;
                    // set mouse position to last menu mouse position
                    SDL_WarpMouseInWindow(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), LastMouseXR, LastMouseYR);

                    if (NeedPlaySfx && vw_IsSoundAvailable(SoundShowHideMenu)) {
                        vw_StopSound(SoundShowHideMenu, 150);
                    }
                    if (NeedPlaySfx) {
                        SoundShowHideMenu = PlayMenuSFX(eMenuSFX::MissionHideMenu, 1.0f);
                    }
                    SetShowGameCursor(false);
                }

                if (GameMissionCompleteStatus && !GameMissionCompleteStatusShowDialog) { // if displaying the mission results, only allow exit to the main menu
                    SetCurrentDialogBox(eDialogBox::QuiToMenuNoSave);
                }
                GameMissionCompleteStatusShowDialog = false;
                vw_SetKeyReleased(SDLK_ESCAPE);
            }
        }
    }




    // fade-in
    if (NeedOnGame) {
        GameBlackTransp = 1.0f - 2.4f*(vw_GetTimeThread(0) - LastGameOnOffUpdateTime);
        if (GameBlackTransp <= 0.0f) {
            GameBlackTransp = 0.0f;
            NeedOnGame = false;
        }

        sRECT SrcRect(0, 0, 2, 2);
        sRECT DstRect(0, 0, GameConfig().InternalWidth, 768);
        constexpr unsigned tmpHash3 = constexpr_hash_djb2a("menu/blackpoint.tga");
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash3), true, GameBlackTransp);
    }

    // fade-out
    if (NeedOffGame) {
        GameBlackTransp = 2.4f * (vw_GetTimeThread(0) - LastGameOnOffUpdateTime);
        if (GameBlackTransp >= 1.0f) {
            GameBlackTransp = 1.0f;
            NeedOffGame = false;

            // game exit
            RealExitGame();
            cCommand::GetInstance().Set(GameExitCommand);
        }

        sRECT SrcRect(0, 0, 2, 2);
        sRECT DstRect(0, 0, GameConfig().InternalWidth, 768);
        constexpr unsigned tmpHash4 = constexpr_hash_djb2a("menu/blackpoint.tga");
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash4), true, GameBlackTransp);
    }



    cStopwatch::GetInstance().Update();
    cStopwatch::GetInstance().Draw();


    vw_End2DMode();
}

} // astromenace namespace
} // viewizard namespace
