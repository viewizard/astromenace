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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "../core/base.h"

// since we have POD structures that could be changed in new version,
// make sure we load proper profiles and top scores
// note, xml tag can't contain space characters, use underline instead
#define CONFIG_VERSION "1.4"
// TODO remove "reserved" in sPilotProfile on CONFIG_VERSION's update

namespace config {

// (!) changes make previous game configuration file incompatible

// maximum profiles
constexpr int MAX_PROFILES{5};
// profile name size
constexpr unsigned PROFILE_NAME_SIZE{1024};
// maximum possible missions number for arrays in pilot profile
constexpr unsigned MAX_MISSIONS{100};
// maximum ships weapons slots
constexpr unsigned MAX_WEAPONS{6};
// maximum hints
constexpr unsigned MAX_HINTS{10};

} // config

// This structure should be POD, since we "pack" it into the game config file
// as memory block. Don't use std::string or any containers here.
struct sPilotProfile {
	bool Used{false};

	char Name[config::PROFILE_NAME_SIZE];

	// game difficulty related settings
	uint8_t EnemyWeaponPenalty{3};		// Enemy weapon fire penalty [1, 3]
	uint8_t EnemyArmorPenalty{2};		// Enemy armor strength penalty [1, 4]
	uint8_t EnemyTargetingSpeedPenalty{2};	// Enemy targeting speed penalty [1, 4]
	uint8_t UnlimitedAmmo{0};		// 0 - limited ammo, 1 - unlimited ammo
						// note, we use "Limited Ammo" option in menu, but UnlimitedAmmo variable
	uint8_t DestroyableWeapon{1};		// 0 - destroyable, 1 - immortal
	uint8_t WeaponTargetingMode{0};		// 0 - simulator, 1 - arcade
	uint8_t SpaceShipControlMode{1};	// 0 - simulator, 1 - arcade
	uint8_t reserved{0};

	// default player's ship (first ship, without additional hull upgrades)
	uint8_t Ship{1};
	uint8_t ShipHullUpgrade{1};
	float ShipHullCurrentStrength{30.0f};

	// weapon related settings
	uint8_t Weapon[config::MAX_WEAPONS];
	int WeaponAmmo[config::MAX_WEAPONS];
	float WeaponSlotYAngle[config::MAX_WEAPONS];		// wealon Y angle
	uint8_t WeaponControl[config::MAX_WEAPONS];		// 1 - primary fire control, 2 - secondary fire control, 3 - both
	uint8_t WeaponAltControl[config::MAX_WEAPONS];		// 0 - disabled, 1 - keyboard, 2 - mouse, 3 - joystick
	uint8_t WeaponAltControlData[config::MAX_WEAPONS];	// alt control data

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

	int ByMissionExperience[config::MAX_MISSIONS];	// experience for each mission
	int MissionReplayCount[config::MAX_MISSIONS];	// how many times mission was replayed

	sPilotProfile()
	{
		memset(Name, 0, config::PROFILE_NAME_SIZE);

		for (unsigned i = 0; i < config::MAX_WEAPONS; i++) {
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

		for (unsigned i = 0; i < config::MAX_MISSIONS; i++) {
			ByMissionExperience[i] = 0;
			MissionReplayCount[i] = 0;
		}
	}
};

struct sGameConfig {
	unsigned int MenuLanguage{0}; // en
	unsigned int VoiceLanguage{0}; // en
	int FontNumber{0}; // first font from the list

	int MusicVolume{8};	// in-game music volume [0, 10]
	int SoundVolume{10};	// in-game sfx volume [0, 10]
	int VoiceVolume{10};	// in-game voice volume [0, 10]

	int Width{1280};	// game resolution's width
	int Height{768};	// game resolution's height
	int BPP{0};		// game resolution's color depth
	float InternalWidth{1228.0f};	// internal resolution's width
	float InternalHeight{768.0f};	// internal resolution's height

	int VSync{0};
	int Brightness{5};
	int MSAA{0}; // MS anti aliasing
	int CSAA{0}; // CS anti aliasing

	int VisualEffectsQuality{0};	// VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
	int AnisotropyLevel{1};		// textures anisotropic filtering level
	bool UseGLSL120{false};		// 120 (OpenGL 2.1)
	int ShadowMap{0};		// gfx (shadow map)
	int MaxPointLights{3};		// lights point max quantity

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

	sPilotProfile Profile[config::MAX_PROFILES];

	int LastProfile{-1}; // last used pilot profile

	// tips and hints status
	bool NeedShowHint[config::MAX_HINTS];

	int MenuScript{0}; // last used menu background script number

	sGameConfig()
	{
		for (unsigned i = 0; i < config::MAX_HINTS; i++) {
			NeedShowHint[i] = true;
		}
	}
};


// Get game configuration for read only.
const sGameConfig &GameConfig();
// Get game configuration for read and write.
sGameConfig &ChangeGameConfig();
// Load game configuration file.
bool LoadXMLConfigFile(bool NeedResetConfig);
// Save game configuration file.
void SaveXMLConfigFile();

// Game's difficulty in %, calculated by profile settings (result is cached).
// For more speed, we don't check ProfileNumber for [0, config::MAX_PROFILES) range.
enum class eDifficultyAction {
	Get,
	Update,
	UpdateAll
};
int ProfileDifficulty(int ProfileNumber, eDifficultyAction Action = eDifficultyAction::Get);

#endif // GAME_CONFIG_H
