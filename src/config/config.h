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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "../core/base.h"

// profile name size
// (!) changes make previous game configuration file incompatible
#define PROFILE_NAME_SIZE 1024
// maximum possible missions number for arrays in pilot profile
// (!) changes make previous game configuration file incompatible
#define MAXIMUM_GAME_MISSIONS 100

// This structure should be POD, since we "pack" it into the game config file
// as memory block. Don't use std::string or any containers here.
struct sPilotProfile {
	bool Used{false};

	char Name[PROFILE_NAME_SIZE];
	// замедление снарядов NPC ... 1-3...
	uint8_t NPCWeaponPenalty;
	// ум. защиты NPC объектов
	uint8_t NPCArmorPenalty;
	// "замедление" наведения NPC ... 1-4
	uint8_t NPCTargetingSpeedPenalty;
	// 0-ограничено, 1-нет
	uint8_t LimitedAmmo;
	// 0-может быть уничтожено, 1-нет
	uint8_t DestroyableWeapon;
	// 1-аркада, 0-симулятор
	uint8_t WeaponTargetingMode;
	// 1-аркада, 0-симулятор
	uint8_t SpaceShipControlMode;

	uint8_t Ship;
	uint8_t ShipHullUpgrade;
	float ShipHullCurrentStrength;

	uint8_t Weapon[6];
	int WeaponAmmo[6];
	// для каждого
	// 1-только примари, 2 только секондари, 3 оба
	uint8_t WeaponControl[6];
	// доп. управление, 0-нет, 1-клава,2-мышка,3-джойст
	uint8_t WeaponAltControl[6];
	uint8_t WeaponAltControlData[6];
	// для каждого слота - текущее положение оружия
	float WeaponSlotYAngle[6];

	uint8_t EngineSystem;
	uint8_t TargetingSystem;
	uint8_t AdvancedProtectionSystem;
	uint8_t PowerSystem;
	uint8_t TargetingMechanicSystem;

	uint8_t Difficulty;

	int Money;
	int Experience;

	// как стрелять // 1 - как и раньше, все вместе... 2 - поочереди
	uint8_t PrimaryWeaponFireMode;
	uint8_t SecondaryWeaponFireMode;

	// последний открытый уровень для данной записи
	int OpenLevelNum;
	// последняя выбранная миссия
	int LastMission;

	// опыт за каждую миссию
	int ByMissionExperience[MAXIMUM_GAME_MISSIONS];
	// кол-во раз которое играли в миссию
	int MissionReplayCount[MAXIMUM_GAME_MISSIONS];
};

// This structure should be POD, since we "pack" it into the game config file
// as memory block. Don't use std::string or any containers here.
struct sTopScores {
	char Name[PROFILE_NAME_SIZE];
	int Score;
};

struct sGameConfig {
	unsigned int MenuLanguage{0}; // en
	unsigned int VoiceLanguage{0}; // en
	int FontNumber{0}; // first font from the list

	int MusicSw{8};		// in-game music volume
	bool Music_check{true};	// general music switch (hardware/software related)
	int SoundSw{10};	// in-game sfx volume
	int VoiceSw{10};	// in-game voice volume
	bool Sound_check{true};	// general sfx switch (hardware/software related)

	int Width{1228};	// game resolution's width
	int Height{768};	// game resolution's height
	int BPP{0};		// game resolution's color depth
	float InternalWidth{1228.0f};	// internal resolution's width
	float InternalHeight{768.0f};	// internal resolution's height
	int CameraModeWithStandardAspectRatio{0}; // camera mode

	int VSync{0};
	int Brightness{5};
	int MSAA{0}; // MS anti aliasing
	int CSAA{0}; // CS anti aliasing

	int VisualEffectsQuality{0};	// VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
	int AnisotropyLevel{1};
	int TexturesCompressionType{0};	// 0 - disabled, 1 - S3TC, 2 - BPTC
	bool UseGLSL120{false};		// 120 (OpenGL 2.1)
	// использование шадовмеп
	int ShadowMap{0};		// gfx (shadow map)
	int MaxPointLights{3};		// lights point max quantity

	sTopScores TopScores[10]{{"Viewizard", 100000},
				 {"Alex", 90000},
				 {"Michael", 80000},
				 {"Jeffrey", 70000},
				 {"Christopher Hughson", 60000},
				 {"Becky", 40000},
				 {"Greg", 20000},
				 {"Jay Coleman", 10000},
				 {"Kelvin", 5000},
				 {"Stephan Gregory", 1000}};

	int KeyboardDecreaseGameSpeed{SDLK_F5};
	int KeyboardResetGameSpeed{SDLK_F6};
	int KeyboardIncreaseGameSpeed{SDLK_F7};
	int KeyboardGameWeaponInfoType{SDLK_F8};
	int KeyboardPrimaryWeaponFireMode{SDLK_F9};
	int KeyboardSecondaryWeaponFireMode{SDLK_F10};

	// keyboard
	int KeyBoardLeft{SDLK_LEFT};
	int KeyBoardRight{SDLK_RIGHT};
	int KeyBoardUp{SDLK_UP};
	int KeyBoardDown{SDLK_DOWN};
	int KeyBoardPrimary{SDLK_LCTRL};
	int KeyBoardSecondary{SDLK_SPACE};
	// mouse
	int MousePrimary{SDL_BUTTON_LEFT};
	int MouseSecondary{SDL_BUTTON_RIGHT};
	bool MouseControl{true};
	int ControlSensivity{5};
	// joystick
	int JoystickPrimary{0};
	int JoystickSecondary{1};
	int JoystickNum{0};
	int JoystickDeadZone{2};

	float GameSpeed{1.5f};
	bool ShowFPS{false};
	int GameWeaponInfoType{1};

	sPilotProfile Profile[5]{sPilotProfile{},
				 sPilotProfile{},
				 sPilotProfile{},
				 sPilotProfile{},
				 sPilotProfile{}};

	int LastProfile{-1}; // last used pilot profile

	// tips and hints status
	bool NeedShowHint[10]{true, true, true, true, true,
			      true, true, true, true, true};

	int MenuScript{0}; // last used menu background script number
};


// Get game configuration for read only.
const sGameConfig &GameConfig();
// Get game configuration for read and write.
sGameConfig &ChangeGameConfig();
// Load game configuration file.
bool LoadXMLConfigFile(bool NeedSafeMode);
// Save game configuration file.
void SaveXMLConfigFile();

#endif // GAME_CONFIG_H
