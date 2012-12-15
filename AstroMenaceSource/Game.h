/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#ifndef GAME_H
#define GAME_H


// ядро
#include "Core/Core.h"
// структуры
#include "Struct.h"



// скриптовый движок
#include "ScriptEngine/Script.h"
// эффекты
#include "GraphicFX/SpaceStars/SpaceStars.h"
#include "GraphicFX/GameLvlText/GameLvlText.h"
// модели и все объекты
#include "Object3D/Object3D.h"
#include "Object3D/SpaceShip/EarthSpaceFighter/EarthSpaceFighter.h"
#include "Object3D/SpaceShip/AlienSpaceFighter/AlienSpaceFighter.h"
#include "Object3D/SpaceShip/AlienSpaceMotherShip/AlienSpaceMotherShip.h"
#include "Object3D/SpaceShip/PirateShip/PirateShip.h"
#include "Object3D/GroundObject/Building/Building.h"
#include "Object3D/GroundObject/MilitaryBuilding/MilitaryBuilding.h"
#include "Object3D/GroundObject/Wheeled/Wheeled.h"
#include "Object3D/GroundObject/Tracked/Tracked.h"
#include "Object3D/SpaceObject/Asteroid/Asteroid.h"
#include "Object3D/SpaceObject/BigAsteroid/BigAsteroid.h"
#include "Object3D/SpaceObject/BasePart/BasePart.h"
#include "Object3D/SpaceObject/ShipPart/ShipPart.h"
#include "Object3D/SpaceObject/Planet/Planet.h"








//------------------------------------------------------------------------------------
// Main.cpp
//------------------------------------------------------------------------------------
extern GameSetup Setup;
extern eDevCaps *CAPS;
extern eGameStatus GameStatus;
extern SDL_Joystick *Joystick;
extern eParticleSystem2D *CursorParticleSystem2D;
extern CSpaceStars *psSpaceStatic;
extern VECTOR3D GamePoint;
extern VECTOR3D GameCameraMovement;

extern  sVideoModes *VideoModes;
extern  int VideoModesNum;
extern  sVideoModes CurrentVideoMode;

extern bool LoadedTypes[1000];
extern char ConfigFileName[MAX_PATH];
extern char ProgrammDir[MAX_PATH];
extern char ScreenshotDir[MAX_PATH];
extern char VFSFileNamePath1[MAX_PATH];
extern char VFSFileNamePath2[MAX_PATH];
extern char UserPath[MAX_PATH];
extern bool Quit;
extern bool CanQuit;
extern bool NeedReCreate;
extern bool SDL_MouseCurrentStatus[8];
extern int JoystickAxisX;
extern int JoystickAxisY;
extern bool JoysticButtons[100];

void SaveGameData();
void CodeXOR(char *Text, char *Key, int Count);

const int FontQuantity=7;
extern sFontList FontList[FontQuantity];





//------------------------------------------------------------------------------------
// MainFS2VFS.cpp
//------------------------------------------------------------------------------------
int ConvertFS2VFS(char RawDataDir[MAX_PATH]);




//------------------------------------------------------------------------------------
// Setup.cpp
//------------------------------------------------------------------------------------
bool LoadXMLSetupFile(bool NeedSafeMode);
void SaveXMLSetupFile();






//------------------------------------------------------------------------------------
// loop_proc.cpp
//------------------------------------------------------------------------------------
extern ScriptEngine *Script;

extern int ComBuffer;
extern int CurrentCursorStatus;
extern bool DrawGameCursor;

void Loop_Proc();
void CreateCursor();








//------------------------------------------------------------------------------------
// loop_audio.cpp
//------------------------------------------------------------------------------------
extern bool GameMainMusicSet;
extern char GameMainMusic[MAX_PATH];
extern bool GameBossMusicSet;
extern char GameBossMusic[MAX_PATH];
extern bool GameDeathMusicSet;
extern char GameDeathMusic[MAX_PATH];

int Audio_PlaySound2D(int SoundID, float fVol, bool Loop = false);
int Audio_PlayVoice(int VoiceID, float fVol, bool Loop = false);
int Audio_PlaySound3D(int SoundID, float fVol, VECTOR3D Location, bool Loop, int AtType);
void Audio_LoopProc();
bool InitAudio();
void StartMusicWithFade(int StartMusic, float FadeInTime, float FadeOutTime);
void Audio_SetSound2DMainVolume(float NewMainVolume);
void Audio_SetVoiceMainVolume(float NewMainVolume);




//------------------------------------------------------------------------------------
// loading.cpp
//------------------------------------------------------------------------------------
void LoadGameData(int LoadType);






//------------------------------------------------------------------------------------
// Camera.cpp, GameCamera.cpp, CameraMath.cpp
//------------------------------------------------------------------------------------
void GameCameraUpdate(float Time);
void InitGameCamera();
float GameCameraGetDeviation();
float GameCameraGetSpeed();
void GameCameraSetExplosion(VECTOR3D Location, float Power);






//------------------------------------------------------------------------------------
// SkyBox.cpp
//------------------------------------------------------------------------------------
void SkyBoxSetTexture(eTexture *nTexture, int Side);
void SkyBoxCreate(float nx, float ny, float nz, float nwidth, float nheight, float nlength);
void SkyBoxDraw(void);






//------------------------------------------------------------------------------------
// StarSystem.cpp
//------------------------------------------------------------------------------------
void StarSystemInit(int Num, VECTOR3D SetBaseRotation);
void StarSystemDraw(int DrawType); // 1- меню, 2- игра
void StarSystemDrawSecondLayer(int DrawType);
void StarSystemUpdate();
void StarSystemRelease();





//------------------------------------------------------------------------------------
// ShadowMap.cpp
//------------------------------------------------------------------------------------
bool ShadowMap_Init(int Width, int Height);
void ShadowMap_Release();
void ShadowMap_StartRenderToFBO(VECTOR3D FocusPointCorrection, float Distance, float fFarClip);
void ShadowMap_EndRenderToFBO();
void ShadowMap_StartFinalRender();
void ShadowMap_EndFinalRender();
float ShadowMap_Get_xPixelOffset();
float ShadowMap_Get_yPixelOffset();




//------------------------------------------------------------------------------------
// Button.cpp
//------------------------------------------------------------------------------------
extern int CurrentActiveMenuElement;
extern int CurrentKeyboardSelectMenuElement;

bool DrawButton384(int X, int Y, const char *Text, float Transp, float *ButTransp, float *Update);
bool DrawButton256(int X, int Y, const char *Text, float Transp, float *ButTransp, float *Update, bool Off=false);
bool DrawButton128_2(int X, int Y, const char *Text, float Transp, bool Off, bool SoundClick = true);
bool DrawButton200_2(int X, int Y, const char *Text, float Transp, bool Off);
void DrawCheckBox(int X, int Y, bool *CheckBoxStatus, const char *Text, float Transp);








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

void InitMenu();
void SetMenu(eGameStatus Menu);
void DrawMenu();
void DrawTransparent(RECT *DstRest, RECT *SrcRest, eTexture *Tex, eTexture *Tex2, bool Alpha, float Transp, float RotateAngle, int DrawCorner, float R, float G, float B);
void MainMenu();










//------------------------------------------------------------------------------------
// Menu_Credits.cpp
//------------------------------------------------------------------------------------
extern float CreditsCurrentPos;
extern float LastCreditsCurrentPosUpdateTime;

void CreditsMenu();









//------------------------------------------------------------------------------------
// Menu_Options.cpp
//------------------------------------------------------------------------------------
extern int Options_Width;
extern int Options_Height;
extern int Options_BPP;
extern int Options_VSync;
extern int Options_iAspectRatioWidth;

void OptionsMenu();









//------------------------------------------------------------------------------------
// Menu_ConfControl.cpp
//------------------------------------------------------------------------------------
extern int NeedCheck;
extern int ButQuant;
extern float But[10];

void ConfControlMenu();
const char * MouseCodeName(char Num);
const char * JoystickCodeName(char Num);
void CheckMouseKeybJState();







//------------------------------------------------------------------------------------
// Game_Options.cpp
//------------------------------------------------------------------------------------
void GameOptions();








//------------------------------------------------------------------------------------
// Menu_Interface.cpp
//------------------------------------------------------------------------------------
extern int Options_FontNumber;

void CreateMenuLanguageEntryLinks();
void ReCreateMenuLanguageEntryLinks();
void CreateVoiceLanguageEntryLinks();
void ReCreateVoiceLanguageEntryLinks();
void InterfaceMenu();







//------------------------------------------------------------------------------------
// Menu_OptionsAdvMenu.cpp
//------------------------------------------------------------------------------------
extern int Options_TexturesCompression;
extern int Options_UseGLSL;
extern int Options_MSAA;
extern int Options_CSAA;
extern int Options_ShadowMap;
extern int Options_TexturesQuality;

void OptionsAdvMenu();








//------------------------------------------------------------------------------------
// Menu_Profile.cpp
//------------------------------------------------------------------------------------
extern int CurrentProfile;

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
extern char GameName[10][128];
extern int GameScore[10];

void TopScoresMenu();
void AddTopScores(int Score, char Name[128], bool Type);








//------------------------------------------------------------------------------------
// Menu_Difficulty.cpp
//------------------------------------------------------------------------------------
void DifficultyMenu();







//------------------------------------------------------------------------------------
// Menu_Mission.cpp
//------------------------------------------------------------------------------------
extern int CurrentMission;
extern int MissionLimitation;
extern int AllMission;
extern int StartMission;
extern int EndMission;

void MissionMenu();
char *GetMissionFileName();
void MissionsListRelease();
void MissionsListInit();








//------------------------------------------------------------------------------------
// Menu_Workshop.cpp
//------------------------------------------------------------------------------------
extern int CurrentWorkshop;
extern int NewWeaponControlType;
extern int NewWeaponControlTypeData;
extern int VoiceNeedMoreEnergy;
extern int VoiceAmmoOut;

void WorkshopMenu();
void WorkshopCreate();
void WorkshopDestroyData();
const char *GetWeaponIconName(int Num);










//------------------------------------------------------------------------------------
// game.cpp
//------------------------------------------------------------------------------------
extern CEarthSpaceFighter *PlayerFighter;

extern int GameNPCWeaponPenalty;
extern int GameNPCArmorPenalty;
extern int GameNPCTargetingSpeedPenalty;
extern int GameLimitedAmmo;
extern int GameDestroyableWeapon;
extern int GameWeaponTargetingMode;
extern int GameSpaceShipControlMode;
extern int GameEngineSystem;
extern int GameTargetingSystem;
extern int GameAdvancedProtectionSystem;
extern int GamePowerSystem;
extern int GameTargetingMechanicSystem;
extern int GameMenuStatus;
extern float GameContentTransp;
extern float GameButton1Transp;
extern float LastGameButton1UpdateTime;
extern bool NeedShowGameMenu;
extern bool NeedHideGameMenu;
extern bool GameMissionCompleteStatus;
extern float StarsTileStartTransparentLayer1;
extern float StarsTileEndTransparentLayer1;
extern float StarsTileStartTransparentLayer2;
extern float StarsTileEndTransparentLayer2;

void InitGame();
void DrawGame();
void ExitGame();
void RealExitGame();
void ExitGameWithSave();
void SetGameMissionComplete();









//------------------------------------------------------------------------------------
// Dialog.cpp
//------------------------------------------------------------------------------------
void SetCurrentDialogBox(int DialogBox);
bool isDialogBoxDrawing();
void DrawDialogBox();









//------------------------------------------------------------------------------------
// Game_Mission.cpp
//------------------------------------------------------------------------------------
void GameSetMissionTitleData(float ShowTime, int Num);
void GameDrawMissionTitle();
void GameSetMissionFailedData(float ShowTime);
void GameDrawMissionFailed();








//------------------------------------------------------------------------------------
// Game_WeaponSlot.cpp
//------------------------------------------------------------------------------------
void DrawGameWeaponSlots();












#endif // GAME_H
