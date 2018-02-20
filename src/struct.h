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


#ifndef STRUCT_H
#define STRUCT_H





//------------------------------------------------------------------------------------
// стороны
//------------------------------------------------------------------------------------
enum Side {
	RIGHT	= 0,		// The RIGHT side
	LEFT	= 1,		// The LEFT side
	BOTTOM	= 2,		// The BOTTOM side
	TOP	= 3,		// The TOP side
	BACK	= 4,		// The BACK side
	FRONT	= 5		// The FRONT side
};






//------------------------------------------------------------------------------------
// возможные состояния... для прорисовки и обхода
//------------------------------------------------------------------------------------
enum eGameStatus {
	// основное меню
	MAIN_MENU = 1,


	// игра
	GAME,

	// лучшие результаты
	TOP_SCORES,
	// настройка интерфейса
	INTERFACE,
	// настройка
	OPTIONS,
	// настройка управления
	CONFCONTROL,
	// настройки быстродействия
	OPTIONS_ADVANCED,
	// информация об игре и т.п.
	INFORMATION,
	// кто делал игру
	CREDITS,
	// профайл
	PROFILE,
	// настройка сложности
	DIFFICULTY,
	// выбор миссии
	MISSION,
	// мастерская покупка-усовершенствование корабля
	WORKSHOP
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
struct GameProfile {
	bool	Used;

	char	Name[128];
	// замедление снарядов NPC ... 1-3...
	BYTE	NPCWeaponPenalty;
	// ум. защиты NPC объектов
	BYTE	NPCArmorPenalty;
	// "замедление" наведения NPC ... 1-4
	BYTE	NPCTargetingSpeedPenalty;
	// 0-ограничено, 1-нет
	BYTE	LimitedAmmo;
	// 0-может быть уничтожено, 1-нет
	BYTE	DestroyableWeapon;
	// 1-аркада, 0-симулятор
	BYTE	WeaponTargetingMode;
	// 1-аркада, 0-симулятор
	BYTE	SpaceShipControlMode;


	BYTE	Ship;
	BYTE	ShipHullUpgrade;
	float	ShipHullCurrentStrength;

	BYTE	Weapon[6];
	int	WeaponAmmo[6];
	// для каждого
	// 1-только примари, 2 только секондари, 3 оба
	BYTE	WeaponControl[6];
	// доп. управление, 0-нет, 1-клава,2-мышка,3-джойст
	BYTE	WeaponAltControl[6];
	BYTE	WeaponAltControlData[6];
	// для каждого слота - текущее положение оружия
	float	WeaponSlotYAngle[6];



	BYTE	EngineSystem;
	BYTE	TargetingSystem;
	BYTE	AdvancedProtectionSystem;
	BYTE	PowerSystem;
	BYTE	TargetingMechanicSystem;

	BYTE	Difficulty;

	int	Money;
	int	Experience;


	// как стрелять // 1 - как и раньше, все вместе... 2 - поочереди
	BYTE	PrimaryWeaponFireMode;
	BYTE	SecondaryWeaponFireMode;

	// последний открытый уровень для данной записи
	BYTE	OpenLevelNum; // BYTE - 255, у нас макс 100
	// последняя выбранная миссия
	BYTE	LastMission; // BYTE - 255, у нас макс 100

	// опыт за каждую миссию
	int	ByMissionExperience[100]; // ставим 100, на максимум миссий
	// кол-во раз которое играли в миссию
	int	MissionReplayCount[100]; // ставим 100, на максимум миссий



	// резерв
	int 	i[3];
	BYTE	c[3];
	bool	b[3];
	float	f[3];
};



//------------------------------------------------------------------------------------
// структура лучших результатов
//------------------------------------------------------------------------------------
struct sTopScores {
	char	Name[128];
	int	Score;
};




//------------------------------------------------------------------------------------
// структура данных настройки игры
//------------------------------------------------------------------------------------
struct GameSetup {
	// номер билда, в котором был создан файл
	int BuildVersion;

	// язык меню
	int	MenuLanguage; // 1-en, 2-de, 3-ru
	// язык голосовых сообщений
	int	VoiceLanguage; // 1-en, 2-de, 3-ru
	// номер шрифта
	int	FontNumber;
	// имя шрифта для fontconfig
	char	FontName[1024];
	// размер шрифта
	int	FontSize;

	// вкл/откл музыки
	int	MusicSw;
	bool	Music_check; // вообще можем-не можем играть музыку
	// вкл/откл SFX
	int	SoundSw;
	int	VoiceSw;
	bool	Sound_check; // вообще можем-не можем играть sfx
	// режим отображения игры
	int	Width;
	int	Height;
	int	BPP;
	float	fAspectRatioWidth;
	float	fAspectRatioHeight;
	int	iAspectRatioWidth;
	int	iAspectRatioHeight;
	// поведение камеры при стандартном отношении (перемещать-не перемешать)
	int 	CameraModeWithStandardAspectRatio;

	// синхронизация
	int	VSync;
	// яркость
	int	Brightness;
	// режим фильтра текстур
	int	TextureFilteringMode;
	// режим качества прорисовки текстур
	int	TexturesQuality;
	// мультисэмпл антиалиасинг
	int	MSAA;
	int	CSAA;


	// качество визуальных эффектов (тайловая подложка, взрывы, системы частиц)  //0-2
	int	VisualEffectsQuality;
	// уровень анизотропии при фильтрации текстур
	int 	AnisotropyLevel;
	// компрессия текстур 0-выкл, 1-S3TC, 2-BPTC
	int	TexturesCompressionType;
	// использование шейдеров
	bool	UseGLSL;
	// использование шадовмеп
	int	ShadowMap;
	// кол-во точечных источников света на 1 объект
	int	MaxPointLights;



	// информация о победителях
	sTopScores	TopScores[10];




	int	KeyboardDecreaseGameSpeed;
	int	KeyboardResetGameSpeed;
	int	KeyboardIncreaseGameSpeed;
	int	KeyboardGameWeaponInfoType;
	int	KeyboardPrimaryWeaponFireMode;
	int	KeyboardSecondaryWeaponFireMode;
	// управление в игре
	// клавиатура
	int	KeyBoardLeft;
	int	KeyBoardRight;
	int	KeyBoardUp;
	int	KeyBoardDown;
	int	KeyBoardPrimary;
	int	KeyBoardSecondary;
	// мышка
	int	MousePrimary;
	int	MouseSecondary;
	// джойстик
	int	JoystickPrimary;
	int	JoystickSecondary;
	// номер джойстика (может их несколько)
	int	JoystickNum;
	// мертвая зона хода ручки джойстика
	int	JoystickDeadZone;



	// задействовать мышку
	bool	MouseControl;
	// чуствительность мышки
	int	ControlSensivity;

	// скорость времени в игре
	float 	GameSpeed;

	// показывать fps + треугольники
	bool	ShowFPS;

	// тип отображения панелей информации оружия в игре
	int 	GameWeaponInfoType;

	// профайлы игроков
	GameProfile	Profile[5];

	// последняя выбранный профайл
	int	LastProfile;

	// показываемые хинты в загрузке
	int	LoadingHint;

	// флаги, показывать-нет подсказки на меню
	// ставим 10, чтобы было с запасом
	bool	NeedShowHint[10];

	// номер скрипта подложки меню
	int	MenuScript;



	// параметры управление движком игры
	int	VBOCoreMode;
	int	VAOCoreMode;
	int	FBOCoreMode;
	// тип работы с видео памятью, больше или нет 128 мегабайт
	bool	EqualOrMore128MBVideoRAM;
	// работа с хардварной генерацией мипмеп уровней
	bool	HardwareMipMapGeneration;
};





#endif // STRUCT_H
