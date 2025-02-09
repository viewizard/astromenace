/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


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

#ifndef ENUM_H
#define ENUM_H

#include "core/base.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eDrawType {
    MENU,
    GAME
};

enum class eMenuStatus {
    MAIN_MENU,          // main menu
    GAME,               // game
    // submenus
    TOP_SCORES,
    INTERFACE,          // accessible from options menu
    OPTIONS,
    CONFCONTROL,        // accessible from options menu
    OPTIONS_ADVANCED,   // accessible from options menu
    INFORMATION,
    CREDITS,
    PROFILE,            // accessible from mission list
    DIFFICULTY,         // accessible from profile menu
    MISSION,            // mission list
    WORKSHOP            // shipyard/workshop/weaponry
};

// in-game menu (accessible during mission)
enum class eGameMenuStatus {
    GAME_MENU,
    OPTIONS,
    OPTIONS_ADVANCED,
    INTERFACE,
    CONFCONTROL
};

enum class eDialogBox {
    None,
    QuitFromGame,           // quit from menu, exit from the game
    ProfileCreationError,   // can't create new profile
    DeleteProfile,          // ask for before delete profile
    RepairShip,             // don't have money for full repair
    QuitNoSave,             // quit with warning that progress will be lost
    QuiToMenuNoSave,        // quit to main menu with warning that progress will be lost
    RestartLevelNoSave,     // mission restart with warning that progress will be lost
    ShowShipInfo,           // ship info
    ShowWeaponsInfo,        // weapon info
    ShowSystemsInfo,        // system info
    ProfileTipsAndTricks,   // tips&tricks in profile menu
    ShipyardTipsAndTricks,  // tips&tricks in shipyard menu
    SystemsTipsAndTricks,   // tips&tricks in system menu
    WeaponryTipsAndTricks,  // tips&tricks in weaponry menu
    ShortkeyTipsAndTricks,  // tips&tricks about short keys on mission start
    StartMissionSecondTime, // warning, if finished mission start second time
    ChoseLanguage,          // language choice on first game start
    RestartOnOptionsChanged,// on mission (in-game menu), restart game on option changes, warning that progress will be lost
    RestartOnAdvOptChanged  // on mission (in-game menu), restart game on adv-option changes, warning that progress will be lost
};

} // astromenace namespace
} // viewizard namespace

#endif // ENUM_H
