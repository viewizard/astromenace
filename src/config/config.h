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

	// game difficulty related settings
	uint8_t NPCWeaponPenalty{3};		// NPC weapon fire penalty [1, 3]
	uint8_t NPCArmorPenalty{2};		// NPC armor strength penalty [1, 4]
	uint8_t NPCTargetingSpeedPenalty{2};	// NPC targeting speed penalty [1, 4]
	uint8_t LimitedAmmo{0};			// 0 - on, 1 - off
	uint8_t DestroyableWeapon{1};		// 0 - destroyable, 1 - immortal
	uint8_t WeaponTargetingMode{0};		// 0 - simulator, 1 - arcade
	uint8_t SpaceShipControlMode{1};	// 0 - simulator, 1 - arcade
	uint8_t Difficulty{50};			// default difficulty in %, this parameter based on all previous parameters

	// default player's ship (first ship, without additional hull upgrades)
	uint8_t Ship{1};
	uint8_t ShipHullUpgrade{1};
	float ShipHullCurrentStrength{30.0f};

	// weapon related settings
	uint8_t Weapon[6];
	int WeaponAmmo[6];
	float WeaponSlotYAngle[6];	// wealon Y angle
	uint8_t WeaponControl[6];	// 1 - primary fire control, 2 - secondary fire control, 3 - both
	uint8_t WeaponAltControl[6];		// 0 - disabled, 1 - keyboard, 2 - mouse, 3 - joystick
	uint8_t WeaponAltControlData[6];	// alt control data

	// default systems for default ship
	uint8_t EngineSystem{1};
	uint8_t TargetingSystem{1};
	uint8_t AdvancedProtectionSystem{1};
	uint8_t PowerSystem{1};
	uint8_t TargetingMechanicSystem{1};

	int Money{200};
	int Experience{0};

	uint8_t PrimaryWeaponFireMode{2};	// 1 - shoot a volley, 2 - shoot a burst
	uint8_t SecondaryWeaponFireMode{2};	// 1 - shoot a volley, 2 - shoot a burst

	int OpenLevelNum{0};	// allowed missions
	int LastMission{0};	// current chosen mission

	int ByMissionExperience[MAXIMUM_GAME_MISSIONS];	// experience for each mission
	int MissionReplayCount[MAXIMUM_GAME_MISSIONS];	// how many times mission was replayed

	sPilotProfile()
	{
		memset(Name, 0, PROFILE_NAME_SIZE);

		for (int i = 0; i < 6; i++) {
			Weapon[i] = 0;
			WeaponAmmo[i] = 0;
			WeaponSlotYAngle[i] = 0.0f;
			WeaponControl[i] = 0;
			WeaponAltControl[i] = 0;
			WeaponAltControlData[i] = 0;
		}

		// default weapons for default ship
		Weapon[2] = 1;
		WeaponAmmo[2] = 3000;
		WeaponControl[2] = 1;
		Weapon[3] = 1;
		WeaponAmmo[3] = 3000;
		WeaponControl[3] = 1;
		Weapon[4] = 16;
		WeaponAmmo[4] = 200;
		WeaponControl[4] = 2;

		for (int i = 0; i < 100; i++) {
			ByMissionExperience[i] = 0;
			MissionReplayCount[i] = 0;
		}
	}
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

	int MusicVolume{8};	// in-game music volume [0, 10]
	bool Music_check{true};	// general music switch (hardware/software related)
	int SoundVolume{10};	// in-game sfx volume [0, 10]
	int VoiceVolume{10};	// in-game voice volume [0, 10]
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
	int AnisotropyLevel{1};		// textures anisotropic filtering level
	int TexturesCompressionType{0};	// 0 - disabled, 1 - S3TC, 2 - BPTC
	bool UseGLSL120{false};		// 120 (OpenGL 2.1)
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

	sPilotProfile Profile[5];

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
