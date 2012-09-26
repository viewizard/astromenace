/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Struct.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#ifndef STRUCT_H
#define STRUCT_H

#include "Defines.h"




//------------------------------------------------------------------------------------
// стороны
//------------------------------------------------------------------------------------
enum Side
{
	RIGHT	= 0,		// The RIGHT side
	LEFT	= 1,		// The LEFT	 side
	BOTTOM	= 2,		// The BOTTOM side
	TOP		= 3,		// The TOP side
	BACK	= 4,		// The BACK	side
	FRONT	= 5			// The FRONT side
};



//------------------------------------------------------------------------------------
// коэф-ты для плоскости
//------------------------------------------------------------------------------------
enum PlaneData
{
	A = 0,				// The X value of the plane's normal
	B = 1,				// The Y value of the plane's normal
	C = 2,				// The Z value of the plane's normal
	D = 3				// The distance the plane is from the origin
};





//------------------------------------------------------------------------------------
// возможные состояния... для прорисовки и обхода
//------------------------------------------------------------------------------------
enum eGameStatus
{
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
	WORKSHOP,
};





//------------------------------------------------------------------------------------
// для выбора в списке режимов разрешения
//------------------------------------------------------------------------------------
struct sVideoModes
{
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
struct GameProfile
{
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
	int		WeaponAmmo[6];
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

	int		Money;
	int		Experience;


	// как стрелять // 1 - как и раньше, все вместе... 2 - поочереди
	BYTE	PrimaryWeaponFireMode;
	BYTE	SecondaryWeaponFireMode;

	// последний открытый уровень для данной записи
	BYTE	OpenLevelNum; // BYTE - 255, у нас макс 100
	// последняя выбранная миссия
	BYTE	LastMission; // BYTE - 255, у нас макс 100

	// опыт за каждую миссию
	int		ByMissionExperience[100]; // ставим 100, на максимум миссий
	// кол-во раз которое играли в миссию
	int		MissionReplayCount[100]; // ставим 100, на максимум миссий



	// резерв
	int 	i[3];
	BYTE	c[3];
	bool	b[3];
	float	f[3];
};



//------------------------------------------------------------------------------------
// структура лучших результатов
//------------------------------------------------------------------------------------
struct sTopScores
{
	char	Name[128];
	int		Score;
};




//------------------------------------------------------------------------------------
// структура данных настройки игры
//------------------------------------------------------------------------------------
struct GameSetup
{
	// номер билда, в котором был создан файл
	int BuildVersion;

	// язык меню
	int		MenuLanguage; // 1-en, 2-de, 3-ru
	// язык голосовых сообщений
	int		VoiceLanguage; // 1-en, 2-de, 3-ru

	// вкл/откл музыки
	int		MusicSw;
	bool	Music_check; // вообще можем-не можем играть музыку
	// вкл/откл SFX
	int		SoundSw;
	bool	Sound_check; // вообще можем-не можем играть sfx
	// режим отображения игры
	int		Width;
	int		Height;
	int		BPP;
	float	fAspectRatioWidth;
	float	fAspectRatioHeight;
	int		iAspectRatioWidth;
	int		iAspectRatioHeight;
	// поведение камеры при стандартном отношении (перемещать-не перемешать)
	int 	CameraModeWithStandardAspectRatio;

	// частота обновления экрана
	int		ScreenFrequency;
	// гамма
	int		Gamma;
	// режим фильтра текстур
	int		TextureFilteringMode;
	// режим качества прорисовки текстур
	int		TexturesQuality;
	// мультисэмпл антиалиасинг
	int		MultiSampleType;



	// испускание частиц, во сколько раз уменьшить... // 1-3
	int		ParticlesPerSecQuality;
	// осколки, сколько пропускать (0-рисовать как в исходном меше) // 0-2
	int		PartsExplosionQuality;
	// коэф уменьшения отображения звезд //1-3
	int		BackgroundStarsQuality;
	// прорисовка слоев с тайловой анимацией // 0-2
	int		BackgroundTileAnimation;
	// уровень анизотропии при фильтрации текстур
	int 	AnisotropyLevel;
	// компрессия текстур 0-выкл, 1-вкл
	int		TexturesCompression;
	// использование шейдеров
	bool	UseGLSL;
	// кол-во точечных источников света на 1 объект
	int		MaxPointLights;



	// информация о победителях
	sTopScores	TopScores[10];




	int		KeyboardLayout; // 1-en, 2-de, 3-ru
	int		KeyboardDecreaseGameSpeed;
	int		KeyboardResetGameSpeed;
	int		KeyboardIncreaseGameSpeed;
	int		KeyboardGameWeaponInfoType;
	int		KeyboardPrimaryWeaponFireMode;
	int		KeyboardSecondaryWeaponFireMode;
	// управление в игре
	// клавиатура
	int		KeyBoardLeft;
	int		KeyBoardRight;
	int		KeyBoardUp;
	int		KeyBoardDown;
	int		KeyBoardPrimary;
	int		KeyBoardSecondary;
	// мышка
	int		MousePrimary;
	int		MouseSecondary;
	// джойстик
	int		JoystickPrimary;
	int		JoystickSecondary;
	// номер джойстика (может их несколько)
	int		JoystickNum;




	// задействовать мышку
	bool	MouseControl;
	// чуствительность мышки
	int		ControlSensivity;

	// скорость времени в игре
	float 	GameSpeed;

	// показывать fps + треугольники
	bool	ShowFPS;

	// тип отображения панелей информации оружия в игре
	int 	GameWeaponInfoType;

	// профайлы игроков
	GameProfile	Profile[5];

	// последняя выбранный профайл
	int		LastProfile;

	// показываемые хинты в загрузке
	int		LoadingHint;

	// флаги, показывать-нет подсказки на меню
	// ставим 10, чтобы было с запасом
	bool	NeedShowHint[10];




	// параметры управление движком игры
	int		VBOCoreMode;
	// управление менеджером приоритетов текстур
	bool	ForceTexturesPriorManager;
	// тип работы с видео памятью, больше или нет 128 мегабайт
	bool	EqualOrMore128MBVideoRAM;
	// работа с хардварной генерацией мипмеп уровней
	bool	HardwareMipMapGeneration;
};





#endif // STRUCT_H
