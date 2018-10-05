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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// FIXME this is really wrong, all this mess should be sorted to headers

#ifndef GAME_H
#define GAME_H

#include "core/base.h"

namespace viewizard {

struct sVECTOR3D;

namespace astromenace {

class cSpaceShip;
enum class eCommand;
enum class eMenuStatus;
enum class eGameMenuStatus;
enum class eDialogBox;

//------------------------------------------------------------------------------------
// loop_proc.cpp
//------------------------------------------------------------------------------------
void Loop_Proc();











//------------------------------------------------------------------------------------
// Button.cpp
//------------------------------------------------------------------------------------
extern int CurrentActiveMenuElement;
extern int CurrentKeyboardSelectMenuElement;

bool DrawButton384(int X, int Y, const std::u32string &Text, float Transp, float &ButTransp, float &Update);
bool DrawButton256(int X, int Y, const std::u32string &Text, float Transp, float &ButTransp, float &Update, bool Off=false);
bool DrawButton128_2(int X, int Y, const std::u32string &Text, float Transp, bool Off, bool SoundClick = true);
bool DrawButton200_2(int X, int Y, const std::u32string &Text, float Transp, bool Off);
void DrawCheckBox(int X, int Y, bool &CheckBoxStatus, const std::u32string &Text, float Transp);
bool DrawListUpButton(int X, int Y, float Transp, bool Off);
bool DrawListDownButton(int X, int Y, float Transp, bool Off);







//------------------------------------------------------------------------------------
// Menu.cpp
//------------------------------------------------------------------------------------
extern float Button1Transp;
extern float LastButton1UpdateTime;
extern float Button2Transp;
extern float LastButton2UpdateTime;
extern float Button3Transp;
extern float LastButton3UpdateTime;
extern float Button4Transp;
extern float LastButton4UpdateTime;
extern float Button5Transp;
extern float LastButton5UpdateTime;
extern float Button6Transp;
extern float LastButton6UpdateTime;
extern float Button7Transp;
extern float LastButton7UpdateTime;
extern float Button8Transp;
extern float LastButton8UpdateTime;
extern float Button9Transp;
extern float LastButton9UpdateTime;
extern float Button10Transp;
extern float LastButton10UpdateTime;
extern float Button11Transp;
extern float LastButton11UpdateTime;
extern float Button12Transp;
extern float LastButton12UpdateTime;
extern float Button13Transp;
extern float LastButton13UpdateTime;
extern float Button14Transp;
extern float LastButton14UpdateTime;
extern float MenuContentTransp;
extern float LastMenuOnOffUpdateTime;

extern bool NeedOffMenu;
extern eMenuStatus MenuStatus;

void InitMenu(eMenuStatus NewMenuStatus);
void SetOptionsMenu(eMenuStatus Menu);
void SetMenu(eMenuStatus Menu);
void DrawMenu();
void MainMenu();










//------------------------------------------------------------------------------------
// Menu_Credits.cpp
//------------------------------------------------------------------------------------
void InitCreditsMenu(float Time);
void CreditsMenu();









//------------------------------------------------------------------------------------
// Menu_Options.cpp
//------------------------------------------------------------------------------------
extern int Options_Width;
extern int Options_Height;
extern int Options_Fullscreen;
extern int Options_VSync;

void OptionsMenu(float ContentTransp, float &ButtonTransp1, float &LastButtonUpdateTime1, float &ButtonTransp2, float &LastButtonUpdateTime2);








//------------------------------------------------------------------------------------
// Menu_ConfControl.cpp
//------------------------------------------------------------------------------------
extern int NeedCheck;
extern int ButQuant;
extern float But[10];

void ConfControlMenu(float ContentTransp, float &ButtonTransp1, float &LastButtonUpdateTime1);
void CheckMouseKeybJState();









//------------------------------------------------------------------------------------
// Menu_Interface.cpp
//------------------------------------------------------------------------------------
void InterfaceMenu(float ContentTransp, float &ButtonTransp1, float &LastButtonUpdateTime1);







//------------------------------------------------------------------------------------
// Menu_OptionsAdvMenu.cpp
//------------------------------------------------------------------------------------
extern int Options_TexturesAnisotropyLevel;
extern int Options_UseGLSL120;
extern int Options_MSAA;
extern int Options_CSAA;
extern int Options_ShadowMap;

void OptionsAdvMenu(float ContentTransp, float &ButtonTransp1, float &LastButtonUpdateTime1, float &ButtonTransp2, float &LastButtonUpdateTime2);
void SaveOptionsAdvMenuTmpData();







//------------------------------------------------------------------------------------
// Menu_Profile.cpp
//------------------------------------------------------------------------------------
extern int CurrentProfile;
extern std::u32string NewProfileName;

void ProfileMenu();
void DeleteRecord();








//------------------------------------------------------------------------------------
// Menu_Information.cpp
//------------------------------------------------------------------------------------
extern int CreateNum;

void InformationMenu();
void InformationDrawObject();
void CreateInfoObject();
void DestroyInfoObject();









//------------------------------------------------------------------------------------
// Menu_TopScores.cpp
//------------------------------------------------------------------------------------
// FIXME should be moved to separate header file
void TopScoresMenu();
void InitTopScoresMenu();








//------------------------------------------------------------------------------------
// Menu_Difficulty.cpp
//------------------------------------------------------------------------------------
void DifficultyMenu();







//------------------------------------------------------------------------------------
// Menu_Mission.cpp
//------------------------------------------------------------------------------------
extern int CurrentMission;
extern int AllMission;
extern int StartMission;
extern int EndMission;

void MissionMenu();
std::string GetCurrentMissionFileName();
void MissionListInit();








//------------------------------------------------------------------------------------
// Menu_Workshop.cpp
//------------------------------------------------------------------------------------
extern int CurrentWorkshop;
extern int NewWeaponControlType;
extern int NewWeaponControlTypeData;
extern unsigned int VoiceNeedMoreEnergy;
extern unsigned int VoiceAmmoOut;

void WorkshopMenu();
void WorkshopCreate();
void WorkshopDestroyData();
std::string GetWeaponIconName(int Num);










//------------------------------------------------------------------------------------
// game.cpp
//------------------------------------------------------------------------------------
extern std::weak_ptr<cSpaceShip> PlayerFighter;

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
extern eGameMenuStatus GameMenuStatus;
extern float GameContentTransp;
extern float GameButton1Transp;
extern float LastGameButton1UpdateTime;
extern bool NeedShowGameMenu;
extern bool NeedHideGameMenu;
extern bool GameMissionCompleteStatus;

void InitGame();
void DrawGame();
void ExitGame(eCommand Command);
void ExitGameWithSave(eCommand Command);
void SetGameMissionComplete();




//------------------------------------------------------------------------------------
// Dialog.cpp
//------------------------------------------------------------------------------------
void InitDialogBoxes();
void SetCurrentDialogBox(eDialogBox DialogBox);
bool isDialogBoxDrawing();
void DrawDialogBox();

} // astromenace namespace
} // viewizard namespace

#endif // GAME_H
