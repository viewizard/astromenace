/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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

#ifndef STRUCT_H
#define STRUCT_H

// Profile name size (changes make previous game configuration file incompatible)
#define PROFILE_NAME_SIZE 1024

enum class eMenuStatus {
	MAIN_MENU,		// main menu
	GAME,			// game
	// submenus
	TOP_SCORES,
	INTERFACE,		// accessible from options menu
	OPTIONS,
	CONFCONTROL,		// accessible from options menu
	OPTIONS_ADVANCED,	// accessible from options menu
	INFORMATION,
	CREDITS,
	PROFILE,		// accessible from mission list
	DIFFICULTY,		// accessible from profile menu
	MISSION,		// mission list
	WORKSHOP		// shipyard/workshop/weaponry
};

// in-game menu (accessible during mission)
enum class eGameMenuStatus {
	GAME_MENU,
	OPTIONS,
	OPTIONS_ADVANCED,
	INTERFACE,
	CONFCONTROL
};

// commands, that should be executed after rendering cycle
enum class eCommand {
	DO_NOTHING,
	SWITCH_TO_MAIN_MENU,
	SWITCH_TO_GAME,
	SWITCH_TO_TOP_SCORES,
	SWITCH_TO_INTERFACE,
	SWITCH_TO_OPTIONS,
	SWITCH_TO_CONFCONTROL,
	SWITCH_TO_OPTIONS_ADVANCED,
	SWITCH_TO_INFORMATION,
	SWITCH_TO_CREDITS,
	SWITCH_TO_PROFILE,
	SWITCH_TO_DIFFICULTY,
	SWITCH_TO_MISSION,
	SWITCH_TO_WORKSHOP,
	TURN_OFF_WORKSHOP_MENU, // preparing for switch to mission
	SWITCH_FROM_GAME_TO_MISSION_MENU,
	SWITCH_FROM_GAME_TO_MAIN_MENU
};

enum class eLoading {
	InitialValue,	// initial value for variables
	MenuWithLogo,	// loading menu with logo at game launch
	Menu,
	Game
};


enum class eDialogBox {
	None,
	QuitFromGame,			// quit from menu, exit from the game
	ProfileCreationError,		// can't create new profile
	DeleteProfile,			// ask for before delete profile
	RepairShip,			// don't have money for full repair
	QuitNoSave,			// quit with warning that progress will be lost
	QuiToMenuNoSave,		// quit to main menu with warning that progress will be lost
	RestartLevelNoSave,		// mission restart with warning that progress will be lost
	ShowShipInfo,			// ship info
	ShowWeaponsInfo,		// weapon info
	ShowSystemsInfo,		// system info
	ProfileTipsAndTricks,		// tips&tricks in profile menu
	ShipyardTipsAndTricks,		// tips&tricks in shipyard menu
	SystemsTipsAndTricks,		// tips&tricks in system menu
	WeaponryTipsAndTricks,		// tips&tricks in weaponry menu
	ShortkeyTipsAndTricks,		// tips&tricks about short keys on mission start
	StartMissionSecondTime,		// warning, if finished mission start second time
	ChoseLanguage,			// language choice on first game start
	RestartOnOptionsChanged,	// on mission (in-game menu), restart game on option changes, warning that progress will be lost
	RestartOnAdvOptChanged		// on mission (in-game menu), restart game on adv-option changes, warning that progress will be lost
};

//------------------------------------------------------------------------------------
// для выбора в списке режимов разрешения
//------------------------------------------------------------------------------------
struct sVideoModes {
	// ширина
	int W;
	// высота
	int H;
	// битность палитры
	int BPP;
};

//------------------------------------------------------------------------------------
// структура данных профайла игрока
//------------------------------------------------------------------------------------
// This structure should be POD, since we "pack" it into the game config file
// as memory block. Don't use std::string or any containers here.
struct sGameProfile {
	bool Used;

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
	uint8_t OpenLevelNum; // uint8_t - 255, у нас макс 100
	// последняя выбранная миссия
	uint8_t LastMission; // uint8_t - 255, у нас макс 100

	// опыт за каждую миссию
	int ByMissionExperience[100]; // ставим 100, на максимум миссий
	// кол-во раз которое играли в миссию
	int MissionReplayCount[100]; // ставим 100, на максимум миссий

	// резерв
	int  i[3];
	uint8_t c[3];
	bool b[3];
	float f[3];
};

//------------------------------------------------------------------------------------
// структура лучших результатов
//------------------------------------------------------------------------------------
// This structure should be POD, since we "pack" it into the game config file
// as memory block. Don't use std::string or any containers here.
struct sTopScores {
	char Name[PROFILE_NAME_SIZE];
	int Score;
};

//------------------------------------------------------------------------------------
// структура данных настройки игры
//------------------------------------------------------------------------------------
struct sGameSetup {
	// номер билда, в котором был создан файл
	int BuildVersion;

	// язык меню
	unsigned int MenuLanguage; // 1-en, 2-de, 3-ru
	// язык голосовых сообщений
	unsigned int VoiceLanguage; // 1-en, 2-de, 3-ru
	// номер шрифта
	int FontNumber;
	// имя шрифта для fontconfig
	char FontName[1024];
	// размер шрифта
	int FontSize;

	// вкл/откл музыки
	int MusicSw;
	bool Music_check; // вообще можем-не можем играть музыку
	// вкл/откл SFX
	int SoundSw;
	int VoiceSw;
	bool Sound_check; // вообще можем-не можем играть sfx
	// режим отображения игры
	int Width;
	int Height;
	int BPP;
	float fAspectRatioWidth;
	float fAspectRatioHeight;
	int iAspectRatioWidth;
	int iAspectRatioHeight;
	// поведение камеры при стандартном отношении (перемещать-не перемешать)
	int CameraModeWithStandardAspectRatio;

	// синхронизация
	int VSync;
	// яркость
	int Brightness;
	// режим фильтра текстур
	int TextureFilteringMode;
	// режим качества прорисовки текстур
	int TexturesQuality;
	// мультисэмпл антиалиасинг
	int MSAA;
	int CSAA;

	// качество визуальных эффектов (тайловая подложка, взрывы, системы частиц)  //0-2
	int VisualEffectsQuality;
	// уровень анизотропии при фильтрации текстур
	int AnisotropyLevel;
	// компрессия текстур 0-выкл, 1-S3TC, 2-BPTC
	int TexturesCompressionType;
	// использование шейдеров
	bool UseGLSL;
	// использование шадовмеп
	int ShadowMap;
	// кол-во точечных источников света на 1 объект
	int MaxPointLights;

	// информация о победителях
	sTopScores TopScores[10];

	int KeyboardDecreaseGameSpeed;
	int KeyboardResetGameSpeed;
	int KeyboardIncreaseGameSpeed;
	int KeyboardGameWeaponInfoType;
	int KeyboardPrimaryWeaponFireMode;
	int KeyboardSecondaryWeaponFireMode;
	// управление в игре
	// клавиатура
	int KeyBoardLeft;
	int KeyBoardRight;
	int KeyBoardUp;
	int KeyBoardDown;
	int KeyBoardPrimary;
	int KeyBoardSecondary;
	// мышка
	int MousePrimary;
	int MouseSecondary;
	// джойстик
	int JoystickPrimary;
	int JoystickSecondary;
	// номер джойстика (может их несколько)
	int JoystickNum;
	// мертвая зона хода ручки джойстика
	int JoystickDeadZone;

	// задействовать мышку
	bool MouseControl;
	// чуствительность мышки
	int ControlSensivity;

	// скорость времени в игре
	float GameSpeed;

	// показывать fps + треугольники
	bool ShowFPS;

	// тип отображения панелей информации оружия в игре
	int GameWeaponInfoType;

	// профайлы игроков
	sGameProfile Profile[5];

	// последняя выбранный профайл
	int LastProfile;

	// показываемые хинты в загрузке
	int LoadingHint;

	// флаги, показывать-нет подсказки на меню
	// ставим 10, чтобы было с запасом
	bool NeedShowHint[10];

	// номер скрипта подложки меню
	int MenuScript;

	// параметры управление движком игры
	int VBOCoreMode;
	int VAOCoreMode;
	int FBOCoreMode;
	// работа с хардварной генерацией мипмеп уровней
	bool HardwareMipMapGeneration;
};

#endif // STRUCT_H
