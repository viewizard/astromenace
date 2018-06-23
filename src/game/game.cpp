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

// TODO since we use TRIANGLES, use 4 vertices + index buffer for vw_SendVertices()
//      instead of 6 vertices, so, we send 4 vertices and index buffer for 6 elements,
//      something like {1, 2, 3, 3, 4, 1}
//                               ^  ^  ^ second triangle indexes
//                      ^  ^  ^ first triangle indexes

// TODO DrawGameCursor and SDL_SetWindowGrab() should be changed in the same code blocks
//      probably we could use SDL_GetWindowGrab() instead of DrawGameCursor

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../core/core.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../assets/audio.h"
#include "../gfx/star_system.h"
#include "../gfx/shadow_map.h"
#include "../script/script.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"
#include <stdarg.h> // va_start
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

namespace {

std::unique_ptr<cMissionScript> MissionScript{};

} // unnamed namespace







// замедление снарядов NPC ... 1-3...
int	GameEnemyWeaponPenalty = 1;
// ум. защиты NPC объектов
int	GameEnemyArmorPenalty = 1;
// "замедление" наведения NPC ... 1-4
int	GameEnemyTargetingSpeedPenalty = 1;
// 0-ограничено, 1-нет
int	GameUnlimitedAmmo = 0;
// 0-может быть уничтожено, 1-нет
int	GameUndestroyableWeapon = 0;
// 1-аркада, 0-симулятор
int	GameWeaponTargetingMode = 0;
// 1-аркада, 0-симулятор
int	GameSpaceShipControlMode = 0;

int	GameEngineSystem = 1;
int	GameTargetingSystem = 1;
int	GameAdvancedProtectionSystem = 0;
int	GamePowerSystem = 1;
int	GameTargetingMechanicSystem = 1;

// присваиваем в профайл только по завершению уровня!
float	GameMoney = 0;
float	GameExperience = 0;

int	AlienShipsKillQuant;
float	AlienShipsKillBonus;
int	AlienMotherShipsKillQuant;
float	AlienMotherShipsKillBonus;
int	PirateShipsKillQuant;
float	PirateShipsKillBonus;
int	PirateVehiclesKillQuant;
float	PirateVehiclesKillBonus;
int	PirateBuildingsKillQuant;
float	PirateBuildingsKillBonus;
int	AsteroidsKillQuant;
float	AsteroidsKillBonus;


// статус завершена игра или нет
bool GameMissionCompleteStatus = false;
bool GameMissionCompleteStatusShowDialog = false;

// собственно сам файтер
cEarthSpaceFighter *PlayerFighter = nullptr;



// флаг отображения меню
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


// работа с морганием вывода
extern float CurrentAlert2;
extern float CurrentAlert3;
extern float CurentTime;

// прорисовка эмблем энергии и жизни
std::weak_ptr<cParticleSystem2D> EnergyParticleSystem2D{};
std::weak_ptr<cParticleSystem2D> LifeParticleSystem2D{};
std::weak_ptr<cParticleSystem2D> Life2ParticleSystem2D{};
std::weak_ptr<cParticleSystem2D> Life3ParticleSystem2D{};


// работа с кораблем игрока
void InitGamePlayerShip();
void GamePlayerShip();
float GetShipMaxEnergy(int Num);
extern float CurrentPlayerShipEnergy;
extern int LastMouseX;
extern int LastMouseY;
extern int LastMouseXR;
extern int LastMouseYR;

// состояние жизни и энергии, которые сейчас рисуем
float CurrentDrawEnergNumFull;
float CurrentDrawLifeNumFull;


// щит или дефлектор
extern std::weak_ptr<cParticleSystem> Shild1;
extern std::weak_ptr<cParticleSystem> Shild2;

// для звука открытия-закрытия меню в игре
unsigned int SoundShowHideMenu{0};


// для отображения скорости
float CurrentGameSpeed = 0.0f;
// время, которое показываем скорость
float CurrentGameSpeedShowTime = 0.0f;


float LastGameOnOffUpdateTime = 0.0f;
float GameBlackTransp = 0.0f;
bool NeedOnGame = false;
bool NeedOffGame = false;


// показывать время игры при скорости 1.5
bool ShowGameTime;
float GameTime;
float LastGameTime;


//------------------------------------------------------------------------------------
// данные фонта
//------------------------------------------------------------------------------------
static void GetGameNumFontData(char Char, sRECT &SrcRect)
{
	switch (Char) {
	case '0':
		SrcRect(232,4,245,25);
		break;
	case '1':
		SrcRect(71,4,84,25);
		break;
	case '2':
		SrcRect(88,4,101,25);
		break;
	case '3':
		SrcRect(106,4,119,25);
		break;
	case '4':
		SrcRect(124,4,137,25);
		break;
	case '5':
		SrcRect(142,4,155,25);
		break;
	case '6':
		SrcRect(160,4,173,25);
		break;
	case '7':
		SrcRect(178,4,191,25);
		break;
	case '8':
		SrcRect(196,4,209,25);
		break;
	case '9':
		SrcRect(214,4,227,25);
		break;

	case 'E':
		SrcRect(47,4,66,25);
		break;
	case 'S':
		SrcRect(4,4,21,25);
		break;
	case '$':
		SrcRect(25,4,41,25);
		break;

	case ' ':
		SrcRect(0,0,13,0);
		break;
	}

}

//------------------------------------------------------------------------------------
// прорисовка денег и опыта
//------------------------------------------------------------------------------------
void DrawGameExpMoney(int Exp, int Money)
{

	sRECT DstRect, SrcRect;
	int Ystart;
	float Xstart;
	GLtexture Texture = vw_FindTextureByName("game/game_num.tga");
	if (!Texture)
		return;

	// Установка текстуры и ее свойств...
	vw_BindTexture(0, Texture);
	vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);

	float ImageHeight{0.0f};
	float ImageWidth{0.0f};
	vw_FindTextureSizeByID(Texture, &ImageWidth, &ImageHeight);

	float R=1.0f;
	float G=1.0f;
	float B=1.0f;
	float Transp=1.0f;

	// выделяем память
	// буфер для последовательности TRIANGLES
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = new float[(2+2+4)*6*16];
	int k=0;



	// вывод эмблем

	Xstart = GameConfig().InternalWidth / 2 - 57.0f;
	Ystart = 5;
	GetGameNumFontData('E', SrcRect);
	DstRect((int)Xstart, Ystart, (int)Xstart + SrcRect.right - SrcRect.left, Ystart + SrcRect.bottom - SrcRect.top);

	// texture's UV coordinates
	float U_Left{(SrcRect.left * 1.0f) / ImageWidth};
	float V_Top{(SrcRect.top * 1.0f) / ImageHeight};
	float U_Right{(SrcRect.right * 1.0f) / ImageWidth};
	float V_Bottom{(SrcRect.bottom * 1.0f) / ImageHeight};

	// first triangle
	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top;		// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Left;
	tmp[k++] = V_Top;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.bottom;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Left;
	tmp[k++] = V_Bottom;

	tmp[k++] = DstRect.right;	// X
	tmp[k++] = DstRect.bottom;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Right;
	tmp[k++] = V_Bottom;


	// second triangle
	tmp[k++] = DstRect.right;	// X
	tmp[k++] = DstRect.bottom;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Right;
	tmp[k++] = V_Bottom;

	tmp[k++] = DstRect.right;	// X
	tmp[k++] = DstRect.top;		// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Right;
	tmp[k++] = V_Top;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top;		// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Left;
	tmp[k++] = V_Top;

	Xstart = GameConfig().InternalWidth / 2 - 56.0f;
	Ystart = 31;
	GetGameNumFontData('$', SrcRect);
	DstRect((int)Xstart, Ystart, (int)Xstart + SrcRect.right - SrcRect.left, Ystart + SrcRect.bottom - SrcRect.top);

	// texture's UV coordinates
	U_Left = (SrcRect.left * 1.0f) / ImageWidth;
	V_Top = (SrcRect.top * 1.0f) / ImageHeight;
	U_Right = (SrcRect.right * 1.0f) / ImageWidth;
	V_Bottom = (SrcRect.bottom * 1.0f) / ImageHeight;

	// first triangle
	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top;		// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Left;
	tmp[k++] = V_Top;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.bottom;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Left;
	tmp[k++] = V_Bottom;

	tmp[k++] = DstRect.right;	// X
	tmp[k++] = DstRect.bottom;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Right;
	tmp[k++] = V_Bottom;


	// second triangle
	tmp[k++] = DstRect.right;	// X
	tmp[k++] = DstRect.bottom;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Right;
	tmp[k++] = V_Bottom;

	tmp[k++] = DstRect.right;	// X
	tmp[k++] = DstRect.top;		// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Right;
	tmp[k++] = V_Top;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top;		// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = U_Left;
	tmp[k++] = V_Top;



	// вывод опыта

	Xstart = GameConfig().InternalWidth / 2 - 57 + 23.0f;
	Ystart = 5;
	if (Exp < 0)
		Exp = 0;
	std::string tmpString{std::to_string(Exp)};

	for (unsigned int i=0; i<7; i++) {
		if ((7 - i) > tmpString.size()) {
			Transp = 0.2f;
			GetGameNumFontData('0', SrcRect);
		} else {
			Transp = 1.0f;
			GetGameNumFontData(tmpString[i + tmpString.size() - 7], SrcRect);
		}
		DstRect((int)Xstart, Ystart, (int)Xstart + SrcRect.right - SrcRect.left, Ystart + SrcRect.bottom - SrcRect.top);

		// texture's UV coordinates
		U_Left = (SrcRect.left * 1.0f) / ImageWidth;
		V_Top = (SrcRect.top * 1.0f) / ImageHeight;
		U_Right = (SrcRect.right * 1.0f) / ImageWidth;
		V_Bottom = (SrcRect.bottom * 1.0f) / ImageHeight;

		// first triangle
		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top;		// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Left;
		tmp[k++] = V_Top;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.bottom;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Left;
		tmp[k++] = V_Bottom;

		tmp[k++] = DstRect.right;	// X
		tmp[k++] = DstRect.bottom;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Right;
		tmp[k++] = V_Bottom;


		// second triangle
		tmp[k++] = DstRect.right;	// X
		tmp[k++] = DstRect.bottom;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Right;
		tmp[k++] = V_Bottom;

		tmp[k++] = DstRect.right;	// X
		tmp[k++] = DstRect.top;		// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Right;
		tmp[k++] = V_Top;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top;		// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Left;
		tmp[k++] = V_Top;

		Xstart += SrcRect.right - SrcRect.left;
	}


	// вывод денег

	Xstart = GameConfig().InternalWidth / 2 - 57 + 23.0f;
	Ystart = 31;
	if (Money < 0)
		Money = 0;
	tmpString = std::to_string(Money);

	for (unsigned int i=0; i<7; i++) {
		if ((7 - i) > tmpString.size()) {
			Transp = 0.2f;
			GetGameNumFontData('0', SrcRect);
		} else {
			Transp = 1.0f;
			GetGameNumFontData(tmpString[i + tmpString.size() - 7], SrcRect);
		}
		DstRect((int)Xstart, Ystart, (int)Xstart + SrcRect.right-SrcRect.left, Ystart + SrcRect.bottom - SrcRect.top);

		// texture's UV coordinates
		U_Left = (SrcRect.left * 1.0f) / ImageWidth;
		V_Top = (SrcRect.top * 1.0f) / ImageHeight;
		U_Right = (SrcRect.right * 1.0f) / ImageWidth;
		V_Bottom = (SrcRect.bottom * 1.0f) / ImageHeight;

		// first triangle
		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top;		// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Left;
		tmp[k++] = V_Top;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.bottom;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Left;
		tmp[k++] = V_Bottom;

		tmp[k++] = DstRect.right;	// X
		tmp[k++] = DstRect.bottom;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Right;
		tmp[k++] = V_Bottom;


		// second triangle
		tmp[k++] = DstRect.right;	// X
		tmp[k++] = DstRect.bottom;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Right;
		tmp[k++] = V_Bottom;

		tmp[k++] = DstRect.right;	// X
		tmp[k++] = DstRect.top;		// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Right;
		tmp[k++] = V_Top;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top;		// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = U_Left;
		tmp[k++] = V_Top;

		Xstart += SrcRect.right - SrcRect.left;
	}

	// 16 - 2 эмблемы + 7 цифр опыта + 7 цифр наград
	vw_Draw3D(ePrimitiveType::TRIANGLES, 6 * 16, RI_2f_XY | RI_1_TEX | RI_4f_COLOR, tmp, 8 * sizeof(tmp[0]));

	if (tmp != nullptr)
		delete [] tmp;
	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);
}







//------------------------------------------------------------------------------------
// Инициализация игровой части
//------------------------------------------------------------------------------------
void InitGame()
{
	ShadowMap_SizeSetup(eShadowMapSetup::Game);

	if ((CurrentProfile < 0) || (CurrentProfile > 4))
		CurrentProfile = 0;
	if (CurrentMission == -1)
		CurrentMission = 0;


	GameEnemyWeaponPenalty = GameConfig().Profile[CurrentProfile].EnemyWeaponPenalty;
	GameEnemyArmorPenalty = GameConfig().Profile[CurrentProfile].EnemyArmorPenalty;
	GameEnemyTargetingSpeedPenalty = GameConfig().Profile[CurrentProfile].EnemyTargetingSpeedPenalty;
	GameUnlimitedAmmo = GameConfig().Profile[CurrentProfile].UnlimitedAmmo;
	GameUndestroyableWeapon = GameConfig().Profile[CurrentProfile].UndestroyableWeapon;
	GameWeaponTargetingMode = GameConfig().Profile[CurrentProfile].WeaponTargetingMode;
	GameSpaceShipControlMode = GameConfig().Profile[CurrentProfile].SpaceShipControlMode;

	GameEngineSystem = GameConfig().Profile[CurrentProfile].EngineSystem;
	// если симулятивный режим, ставим 1...
	if (GameSpaceShipControlMode == 1)
		if (GameEngineSystem == 0) GameEngineSystem = 1;

	GameTargetingSystem = GameConfig().Profile[CurrentProfile].TargetingSystem;
	GameAdvancedProtectionSystem = GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem;
	GamePowerSystem = GameConfig().Profile[CurrentProfile].PowerSystem;
	GameTargetingMechanicSystem = GameConfig().Profile[CurrentProfile].TargetingMechanicSystem;
	GameMoney = GameConfig().Profile[CurrentProfile].Money * 1.0f;

	// убираем данные этого уровня
	GameExperience = (GameConfig().Profile[CurrentProfile].Experience - GameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission]) * 1.0f;

	// grab mouse control for both - windows and fullscren mode (need this for multi-monitor systems)
	SDL_SetWindowGrab(vw_GetSDLWindow(), SDL_TRUE);


	// сбрасываем все кнопки мыши
	vw_ResetMouseButtons();
	// установка мышки на центр
	float tmpViewportWidth, tmpViewportHeight;
	vw_GetViewport(nullptr, nullptr, &tmpViewportWidth, &tmpViewportHeight);
	SDL_WarpMouseInWindow(vw_GetSDLWindow(), tmpViewportWidth / 2, tmpViewportHeight / 2);
	DrawGameCursor = false;

	LastMouseXR = 0;
	LastMouseYR = 0;
	SDL_GetMouseState(&LastMouseXR, &LastMouseYR);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// инициализация счета времени (всегда первым)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_InitTimeThread(1);
	vw_SetTimeThreadSpeed(1, GameConfig().GameSpeed);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним камеру, всегда до работы со скриптом (!!!)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	InitGameCamera();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним корабль
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	InitGamePlayerShip();



	// !!! пока загрузка идет полная на уровень, и наверно так и нужно оставить
	// иначе нужно выносить перечень загружаемого в скрипт (менять не смогут уровни)

	StarSystemInitByType(eDrawType::GAME); // should be before RunScript()

	MissionScript.reset(new cMissionScript);
	if (!MissionScript->RunScript(GetCurrentMissionFileName(), vw_GetTimeThread(1)))
		MissionScript.reset();


	// выводим номер миссии 3 секунды
	GameSetMissionTitleData(3, CurrentMission+1);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// немного "прокручиваем", чтобы сразу по появлению было заполнено
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);
	vw_SetCameraLocation(sVECTOR3D(0, 65, -100+10));
	vw_SetCameraMoveAroundPoint(sVECTOR3D(0, 0, 10), 0.0f, sVECTOR3D(0.0f, 0.0f, 0.0f));


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним 2д часть, эмблемы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	EnergyParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedEnergyParticleSystem2D = EnergyParticleSystem2D.lock()) {
		sharedEnergyParticleSystem2D->ColorStart.r = 0.70f;
		sharedEnergyParticleSystem2D->ColorStart.g = 0.80f;
		sharedEnergyParticleSystem2D->ColorStart.b = 1.00f;
		sharedEnergyParticleSystem2D->ColorEnd.r = 0.00f;
		sharedEnergyParticleSystem2D->ColorEnd.g = 0.00f;
		sharedEnergyParticleSystem2D->ColorEnd.b = 1.00f;
		sharedEnergyParticleSystem2D->AlphaStart = 1.00f;
		sharedEnergyParticleSystem2D->AlphaEnd = 1.00f;
		sharedEnergyParticleSystem2D->SizeStart = 12.00f;
		sharedEnergyParticleSystem2D->SizeVar = 10.00f;
		sharedEnergyParticleSystem2D->SizeEnd = 0.00f;
		sharedEnergyParticleSystem2D->Speed = 70.00f;
		sharedEnergyParticleSystem2D->SpeedVar = 20.00f;
		sharedEnergyParticleSystem2D->Theta = 360.00f;
		sharedEnergyParticleSystem2D->Life = 2.10f;
		sharedEnergyParticleSystem2D->LifeVar = 0.05f;
		sharedEnergyParticleSystem2D->ParticlesPerSec = 50;
		sharedEnergyParticleSystem2D->IsMagnet = true;
		sharedEnergyParticleSystem2D->MagnetFactor = 150.0f;
		sharedEnergyParticleSystem2D->CreationType = eParticle2DCreationType::Point;
		sharedEnergyParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");
		sharedEnergyParticleSystem2D->MoveSystem(sVECTOR3D(33.0f, 29.0f, 0.0f));
	}

	Life3ParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedLife3ParticleSystem2D = Life3ParticleSystem2D.lock()) {
		sharedLife3ParticleSystem2D->ColorStart.r = 1.00f;
		sharedLife3ParticleSystem2D->ColorStart.g = 0.40f;
		sharedLife3ParticleSystem2D->ColorStart.b = 0.10f;
		sharedLife3ParticleSystem2D->ColorEnd.r = 0.50f;
		sharedLife3ParticleSystem2D->ColorEnd.g = 0.00f;
		sharedLife3ParticleSystem2D->ColorEnd.b = 0.00f;
		sharedLife3ParticleSystem2D->AlphaStart = 1.00f;
		sharedLife3ParticleSystem2D->AlphaEnd = 1.00f;
		sharedLife3ParticleSystem2D->SizeStart = 13.00f;
		sharedLife3ParticleSystem2D->SizeVar = 5.00f;
		sharedLife3ParticleSystem2D->SizeEnd = 0.00f;
		sharedLife3ParticleSystem2D->Speed = 0.00f;
		sharedLife3ParticleSystem2D->SpeedOnCreation = 8.00f;
		sharedLife3ParticleSystem2D->SpeedVar = 0.00f;
		sharedLife3ParticleSystem2D->Theta = 360.00f;
		sharedLife3ParticleSystem2D->Life = 2.00f;
		sharedLife3ParticleSystem2D->LifeVar = 0.05f;
		sharedLife3ParticleSystem2D->ParticlesPerSec = 50;
		sharedLife3ParticleSystem2D->CreationType = eParticle2DCreationType::Quad;
		sharedLife3ParticleSystem2D->CreationSize = sVECTOR3D(1.0f, 18.0f, 0.0f);
		sharedLife3ParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");
		sharedLife3ParticleSystem2D->MoveSystem(sVECTOR3D(GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f));
	}

	Life2ParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedLife2ParticleSystem2D = Life2ParticleSystem2D.lock()) {
		sharedLife2ParticleSystem2D->ColorStart.r = 1.00f;
		sharedLife2ParticleSystem2D->ColorStart.g = 0.40f;
		sharedLife2ParticleSystem2D->ColorStart.b = 0.10f;
		sharedLife2ParticleSystem2D->ColorEnd.r = 0.50f;
		sharedLife2ParticleSystem2D->ColorEnd.g = 0.00f;
		sharedLife2ParticleSystem2D->ColorEnd.b = 0.00f;
		sharedLife2ParticleSystem2D->AlphaStart = 1.00f;
		sharedLife2ParticleSystem2D->AlphaEnd = 1.00f;
		sharedLife2ParticleSystem2D->SizeStart = 13.00f;
		sharedLife2ParticleSystem2D->SizeVar = 5.00f;
		sharedLife2ParticleSystem2D->SizeEnd = 0.00f;
		sharedLife2ParticleSystem2D->Speed = 0.00f;
		sharedLife2ParticleSystem2D->SpeedOnCreation = 8.00f;
		sharedLife2ParticleSystem2D->SpeedVar = 0.00f;
		sharedLife2ParticleSystem2D->Theta = 360.00f;
		sharedLife2ParticleSystem2D->Life = 2.00f;
		sharedLife2ParticleSystem2D->LifeVar = 0.05f;
		sharedLife2ParticleSystem2D->ParticlesPerSec = 50;
		sharedLife2ParticleSystem2D->CreationType = eParticle2DCreationType::Quad;
		sharedLife2ParticleSystem2D->CreationSize = sVECTOR3D(18.0f, 1.0f, 0.0f);
		sharedLife2ParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");
		sharedLife2ParticleSystem2D->MoveSystem(sVECTOR3D(GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f));
	}

	LifeParticleSystem2D = vw_CreateParticleSystem2D(vw_GetTimeThread(0));
	if (auto sharedLifeParticleSystem2D = LifeParticleSystem2D.lock()) {
		sharedLifeParticleSystem2D->ColorStart.r = 1.00f;
		sharedLifeParticleSystem2D->ColorStart.g = 0.60f;
		sharedLifeParticleSystem2D->ColorStart.b = 0.20f;
		sharedLifeParticleSystem2D->ColorEnd.r = 0.50f;
		sharedLifeParticleSystem2D->ColorEnd.g = 0.00f;
		sharedLifeParticleSystem2D->ColorEnd.b = 0.00f;
		sharedLifeParticleSystem2D->AlphaStart = 1.00f;
		sharedLifeParticleSystem2D->AlphaEnd = 1.00f;
		sharedLifeParticleSystem2D->SizeStart = 25.00f;
		sharedLifeParticleSystem2D->SizeVar = 5.00f;
		sharedLifeParticleSystem2D->SizeEnd = 0.00f;
		sharedLifeParticleSystem2D->Speed = 0.00f;
		sharedLifeParticleSystem2D->SpeedOnCreation = 8.00f;
		sharedLifeParticleSystem2D->SpeedVar = 10.00f;
		sharedLifeParticleSystem2D->Theta = 360.00f;
		sharedLifeParticleSystem2D->Life = 1.50f;
		sharedLifeParticleSystem2D->LifeVar = 0.05f;
		sharedLifeParticleSystem2D->ParticlesPerSec = 70;
		sharedLifeParticleSystem2D->Direction = sVECTOR3D(1.0f, 0.0f, 0.0f);
		sharedLifeParticleSystem2D->CreationType = eParticle2DCreationType::Circle;
		sharedLifeParticleSystem2D->CreationSize = sVECTOR3D(25.0f, 25.0f, 0.0f);
		sharedLifeParticleSystem2D->DeadZone = 24.0f;
		sharedLifeParticleSystem2D->IsMagnet = true;
		sharedLifeParticleSystem2D->MagnetFactor = 25.0f;
		sharedLifeParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare.tga");
		sharedLifeParticleSystem2D->MoveSystem(sVECTOR3D(GameConfig().InternalWidth - 33.0f, 29.0f, 0.0f));
		sharedLifeParticleSystem2D->SetRotation(sVECTOR3D(0.0f, 0.0f, 90.0f));
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// инициализация игрового меню
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

	CurrentDrawEnergNumFull = 1.0f;
	if (GamePowerSystem == 0) CurrentDrawEnergNumFull = 0.0f;
	CurrentDrawLifeNumFull = PlayerFighter->Strength/PlayerFighter->StrengthStart;

	CurentTime = vw_GetTimeThread(0);
	CurrentAlert2 = 1.0f;
	CurrentAlert3 = 1.0f;

	MenuStatus = eMenuStatus::GAME;

	GameMissionCompleteStatus = false;
	GameMissionCompleteStatusShowDialog = false;

	SoundShowHideMenu = 0;
	CurrentGameSpeedShowTime = 2.0f;

	LastGameOnOffUpdateTime = vw_GetTimeThread(0);
	GameBlackTransp = 1.0f;
	NeedOnGame = true;

	GameTime = 0.0f;
	LastGameTime = vw_GetTimeThread(0);
}







//------------------------------------------------------------------------------------
// Завершаем игру
//------------------------------------------------------------------------------------
eCommand NewComBuffer;
void ExitGame()
{
	NewComBuffer = ComBuffer;
	ComBuffer = eCommand::DO_NOTHING; // пока сбрасываем в ноль, чтобы не переключилось до затухания
	NeedOffGame = true;
	LastGameOnOffUpdateTime = vw_GetTimeThread(0);

	// убираем меню
	if (GameMenu) {
		GameMenu = false;
		NeedShowGameMenu = false;
		NeedHideGameMenu = true;
		DrawGameCursor = false;
		// установка в последюю точку указателя
		SDL_WarpMouseInWindow(vw_GetSDLWindow(), LastMouseXR, LastMouseYR);
	}
}
void RealExitGame()
{
	// удаляем корабль игрока
	if (PlayerFighter != nullptr) {
		delete PlayerFighter;
		PlayerFighter = nullptr;
	}

	vw_ReleaseAllParticleSystems2D();

	vw_ReleaseParticleSystem(Shild1);
	vw_ReleaseParticleSystem(Shild2);

	// release mouse control
	SDL_SetWindowGrab(vw_GetSDLWindow(), SDL_FALSE);
}

//------------------------------------------------------------------------------------
// Завершаем игру, нужно сохранить параметры
//------------------------------------------------------------------------------------
void ExitGameWithSave()
{
	// данные по деньгам и опыту
	ChangeGameConfig().Profile[CurrentProfile].Money = static_cast<int>(GameMoney);

	// если получили больше опыта
	if (GameConfig().Profile[CurrentProfile].Experience < static_cast<int>(GameExperience)) {
		int Incr = static_cast<int>(GameExperience) - GameConfig().Profile[CurrentProfile].Experience;
		ChangeGameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission] += Incr;
		ChangeGameConfig().Profile[CurrentProfile].Experience += Incr;
	}
	// увеличиваем счетчик пройденной миссии
	ChangeGameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission]++;

	// состояние корпуса коробля
	ChangeGameConfig().Profile[CurrentProfile].ShipHullCurrentStrength = PlayerFighter->Strength;

	// учет состояния оружия
	for (int i = 0; i < PlayerFighter->WeaponQuantity; i++) {
		if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
			// если оружие было уничтожено во время игры
			if (PlayerFighter->Weapon[i]->Strength <= 0.0f) {
				ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = 0;
				ChangeGameConfig().Profile[CurrentProfile].Weapon[i] = 0;
			} else {
				// если все ок, нужно запомнить сколько осталось в боекомплекте
				ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = PlayerFighter->Weapon[i]->Ammo;
			}
		}
	}

	// ставим следующую миссию
	CurrentMission++;
	// перемещаем ограничитель дальше, если это нужно
	if (GameConfig().Profile[CurrentProfile].OpenLevelNum < CurrentMission)
		ChangeGameConfig().Profile[CurrentProfile].OpenLevelNum = CurrentMission;

	// если дальше уже ничего нет, просто снимаем все... пусть игрок сам выберет
	if (CurrentMission > (AllMission - 1))
		CurrentMission = -1;

	vw_ResetWheelStatus();
	// ставим нужный лист миссий
	StartMission = 0;
	EndMission = 4;
	if (CurrentMission != -1)
		if (CurrentMission > 2) { // нужно сдвинуть лист, чтобы выбранный элемент был по середине списка
			StartMission = CurrentMission-2;
			EndMission = CurrentMission+2;

			if (CurrentMission >= AllMission-2) {
				StartMission = AllMission-5;
				EndMission = AllMission-1;
			}
		}


	ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;

	ExitGame();
}






//------------------------------------------------------------------------------------
// Завершение игры, выиграли
//------------------------------------------------------------------------------------
void SetGameMissionComplete()
{
	// если убили, не устанавливаем!
	if (PlayerFighter == nullptr)
		return;
	GameMissionCompleteStatus = true;
	GameMissionCompleteStatusShowDialog = true;
}






//------------------------------------------------------------------------------------
// прорисовка игровой части
//------------------------------------------------------------------------------------
void DrawGame()
{

	float TimeDelta = vw_GetTimeThread(0) - CurentTime;
	CurentTime = vw_GetTimeThread(0);


	// если не в меню - считаем время
	if (GameContentTransp == 0.0f) GameTime += vw_GetTimeThread(0) - LastGameTime;
	LastGameTime = vw_GetTimeThread(0);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// просчитываем индикацию
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CurrentAlert2 -= TimeDelta;
	if (CurrentAlert2 < 0.1f) CurrentAlert2 = 1.0f;
	CurrentAlert3 -= 1.9f*TimeDelta;
	if (CurrentAlert3 < 0.1f) CurrentAlert3 = 1.0f;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Работа с 3д частью... прорисовка, просчет
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GameCameraUpdate(vw_GetTimeThread(1));
	vw_CameraLookAt();


	// всегда первым рисуем скайбокс и "далекое" окружение
	StarSystemDraw(eDrawType::GAME);


	// рисуем все 3д объекты
	DrawAllObject3D(eDrawType::GAME);


	// после полной прорисовки делаем обновление данных
	UpdateAllObject3D(vw_GetTimeThread(1));
	vw_UpdateAllParticleSystems(vw_GetTimeThread(1));

	// проверяем на столкновения
	if (GameContentTransp < 0.99f) // не нужно проверять коллизии, включено меню
		DetectCollisionAllObject3D();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работаем со скриптом, пока он есть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (MissionScript &&
	    (!MissionScript->Update(vw_GetTimeThread(1))))
		MissionScript.reset();







	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2д часть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_Start2DMode(-1,1);
	sRECT SrcRect, DstRect;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Обработка состояния корабля игрока
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GamePlayerShip();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Выводим верхнюю информационную панель
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (GameConfig().InternalWidth == 1024) {
		SrcRect(0, 0, 1024, 74);
		DstRect(0, 0, 1024, 74);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("game/game_panel.tga"), true, 1.0f);
	}
	if (GameConfig().InternalWidth == 1228) {
		SrcRect(0, 0, 466, 73);
		DstRect(0, 0, 466, 73);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("game/game_panel2.tga"), true, 1.0f);

		SrcRect(1, 74, 150, 145);
		DstRect(540, 0, 540+149, 71);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("game/game_panel2.tga"), true, 1.0f);

		SrcRect(150, 74, 610, 145);
		DstRect(768, 0, 768+460, 71);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("game/game_panel2.tga"), true, 1.0f);
	}



	if (auto sharedEnergyParticleSystem2D = EnergyParticleSystem2D.lock()) {
		// учитываем в эмблеме энергии, сколько у нас ее (визуально меняем вид эмблемы)
		sharedEnergyParticleSystem2D->ParticlesPerSec =
				(unsigned int)(50 * (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)));
		if (sharedEnergyParticleSystem2D->ParticlesPerSec == 0)
			sharedEnergyParticleSystem2D->ParticlesPerSec = 1;
	}
	if (PlayerFighter != nullptr) {
		if (auto sharedLifeParticleSystem2D = LifeParticleSystem2D.lock()) {
			sharedLifeParticleSystem2D->ColorStart.r = 1.00f;
			sharedLifeParticleSystem2D->ColorStart.g = 0.60f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
			sharedLifeParticleSystem2D->ColorStart.b = 0.20f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
			// если меньше 10% нужно бить тревогу
			if (PlayerFighter->Strength < PlayerFighter->StrengthStart/10.0f) {
				sharedLifeParticleSystem2D->AlphaStart = 1.00f*CurrentAlert2;
				sharedLifeParticleSystem2D->AlphaEnd   = 1.00f*CurrentAlert2;
			} else { // подчинились, восстанавливаем данные
				sharedLifeParticleSystem2D->AlphaStart = 1.00f;
				sharedLifeParticleSystem2D->AlphaEnd   = 1.00f;
			}
		}

		if (auto sharedLife2ParticleSystem2D = Life2ParticleSystem2D.lock()) {
			sharedLife2ParticleSystem2D->ColorStart.r = 1.00f;
			sharedLife2ParticleSystem2D->ColorStart.g = 0.60f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
			sharedLife2ParticleSystem2D->ColorStart.b = 0.20f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
			// если меньше 10% нужно бить тревогу
			if (PlayerFighter->Strength < PlayerFighter->StrengthStart/10.0f) {
				if (CurrentAlert2 > 0.6f) {
					sharedLife2ParticleSystem2D->AlphaStart = 1.00f*CurrentAlert2;
					sharedLife2ParticleSystem2D->AlphaEnd   = 1.00f*CurrentAlert2;
				} else {
					sharedLife2ParticleSystem2D->AlphaStart = 0.00f;
					sharedLife2ParticleSystem2D->AlphaEnd   = 0.00f;
				}
			} else { // подчинились, восстанавливаем данные
				sharedLife2ParticleSystem2D->AlphaStart = 1.00f;
				sharedLife2ParticleSystem2D->AlphaEnd   = 1.00f;
			}
		}

		if (auto sharedLife3ParticleSystem2D = Life3ParticleSystem2D.lock()) {
			sharedLife3ParticleSystem2D->ColorStart.r = 1.00f;
			sharedLife3ParticleSystem2D->ColorStart.g = 0.60f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
			sharedLife3ParticleSystem2D->ColorStart.b = 0.20f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);

			// если меньше 10% нужно бить тревогу
			if (PlayerFighter->Strength < PlayerFighter->StrengthStart/10.0f) {
				if (CurrentAlert2 > 0.6f) {
					sharedLife3ParticleSystem2D->AlphaStart = 1.00f*CurrentAlert2;
					sharedLife3ParticleSystem2D->AlphaEnd   = 1.00f*CurrentAlert2;
				} else {
					sharedLife3ParticleSystem2D->AlphaStart = 0.00f;
					sharedLife3ParticleSystem2D->AlphaEnd   = 0.00f;
				}
			} else { // подчинились, восстанавливаем данные
				sharedLife3ParticleSystem2D->AlphaStart = 1.00f;
				sharedLife3ParticleSystem2D->AlphaEnd   = 1.00f;
			}
		}
	}

	vw_UpdateAllParticleSystems2D(vw_GetTimeThread(0));
	vw_DrawAllParticleSystems2D();



	// выводим состояние жизни и энергии
	float NeedDrawEnergNumFull = 0.0f;
	float NeedDrawLifeNumFull = 0.0f;
	if (PlayerFighter != nullptr) {
		// находим целую часть... т.е. номер последней, которую будем рисовать уже с прозрачностью
		NeedDrawEnergNumFull = CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem);
		// находим целую часть... т.е. номер последней, которую будем рисовать уже с прозрачностью
		NeedDrawLifeNumFull = PlayerFighter->Strength / PlayerFighter->StrengthStart;
	}









	// рисуем жизнь и энергию одним проходом, чтобы не гонять по 4 вертекса
	{

		// находим правильное отображение
		if (NeedDrawEnergNumFull > CurrentDrawEnergNumFull) {
			CurrentDrawEnergNumFull += GamePowerSystem*0.5f*(vw_GetTimeThread(0) - LastGameUpdateTime);
			if (CurrentDrawEnergNumFull > NeedDrawEnergNumFull) CurrentDrawEnergNumFull = NeedDrawEnergNumFull;
		} else {
			if (NeedDrawEnergNumFull < CurrentDrawEnergNumFull) {
				CurrentDrawEnergNumFull -= GamePowerSystem*0.5f*(vw_GetTimeThread(0) - LastGameUpdateTime);
				if (CurrentDrawEnergNumFull < NeedDrawEnergNumFull) CurrentDrawEnergNumFull = NeedDrawEnergNumFull;
			}
		}
		// находим целую часть... т.е. номер последней, которую будем рисовать уже с прозрачностью
		int DrawEnergNum = (int)ceil(CurrentDrawEnergNumFull * 19);

		// находим правильное отображение
		if (NeedDrawLifeNumFull > CurrentDrawLifeNumFull) {
			CurrentDrawLifeNumFull += 0.3f*(vw_GetTimeThread(0) - LastGameUpdateTime);
			if (CurrentDrawLifeNumFull > NeedDrawLifeNumFull) CurrentDrawLifeNumFull = NeedDrawLifeNumFull;
		} else {
			if (NeedDrawLifeNumFull < CurrentDrawLifeNumFull) {
				CurrentDrawLifeNumFull -= 0.3f*(vw_GetTimeThread(0) - LastGameUpdateTime);
				if (CurrentDrawLifeNumFull < NeedDrawLifeNumFull) CurrentDrawLifeNumFull = NeedDrawLifeNumFull;
			}
		}
		// находим целую часть... т.е. номер последней, которую будем рисовать уже с прозрачностью
		int DrawLifeNum = (int)ceil(CurrentDrawLifeNumFull * 19);

		if (DrawLifeNum+DrawEnergNum > 0) {
			float R=1.0f;
			float G=1.0f;
			float B=1.0f;

			GLtexture Texture = vw_FindTextureByName("game/game_panel_el.tga");
			if (!Texture)
				return;

			// Установка текстуры и ее свойств...
			vw_BindTexture(0, Texture);
			vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);

			float ImageHeight{0.0f};
			float ImageWidth{0.0f};
			vw_FindTextureSizeByID(Texture, &ImageWidth, &ImageHeight);

			// выделяем память
			// буфер для последовательности TRIANGLES
			// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
			float *tmp = new float[(2+2+4)*6*(DrawLifeNum+DrawEnergNum)];
			int k = 0;



			// вывод текущего заряда энергии
			// прорисовываем все элементы
			for (int i=0; i<DrawEnergNum; i++) {
				// получаем данные текущего фрагмента
				SrcRect(67+i*20, 0, 85+i*20, 64);
				DstRect = SrcRect;
				// находим прозначность
				float Transp = (CurrentDrawEnergNumFull * 19) - i;
				if (Transp > 1.0f) Transp = 1.0f;

				// texture's UV coordinates
				float U_Left{(SrcRect.left * 1.0f) / ImageWidth};
				float V_Top{(SrcRect.top * 1.0f)/ImageHeight};
				float U_Right{(SrcRect.right * 1.0f) / ImageWidth};
				float V_Bottom{(SrcRect.bottom * 1.0f) / ImageHeight};

				// first triangle
				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top;		// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Left;
				tmp[k++] = V_Top;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.bottom;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Left;
				tmp[k++] = V_Bottom;

				tmp[k++] = DstRect.right;	// X
				tmp[k++] = DstRect.bottom;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Right;
				tmp[k++] = V_Bottom;


				// second triangle
				tmp[k++] = DstRect.right;	// X
				tmp[k++] = DstRect.bottom;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Right;
				tmp[k++] = V_Bottom;

				tmp[k++] = DstRect.right;	// X
				tmp[k++] = DstRect.top;		// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Right;
				tmp[k++] = V_Top;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top;		// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Left;
				tmp[k++] = V_Top;

			}

			// вывод текущего состояния жизни
			// прорисовываем все элементы
			for (int i=0; i<DrawLifeNum; i++) {
				// получаем данные текущего фрагмента
				SrcRect(582 + i * 20, 0, 599 + i * 20, 64);
				if (GameConfig().InternalWidth == 1024)
					DstRect = SrcRect;
				if (GameConfig().InternalWidth == 1228)
					DstRect(204 + 582 + i * 20, 0, 204 + 599 + i * 20, 64);
				// находим прозначность
				float Transp = (CurrentDrawLifeNumFull * 19) - i;
				if (Transp > 1.0f)
					Transp = 1.0f;

				// texture's UV coordinates
				float U_Left{(SrcRect.left * 1.0f) / ImageWidth};
				float V_Top{(SrcRect.top * 1.0f)/ImageHeight};
				float U_Right{(SrcRect.right * 1.0f) / ImageWidth};
				float V_Bottom{(SrcRect.bottom * 1.0f) / ImageHeight};

				// first triangle
				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top;		// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Left;
				tmp[k++] = V_Top;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.bottom;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Left;
				tmp[k++] = V_Bottom;

				tmp[k++] = DstRect.right;	// X
				tmp[k++] = DstRect.bottom;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Right;
				tmp[k++] = V_Bottom;


				// second triangle
				tmp[k++] = DstRect.right;	// X
				tmp[k++] = DstRect.bottom;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Right;
				tmp[k++] = V_Bottom;

				tmp[k++] = DstRect.right;	// X
				tmp[k++] = DstRect.top;		// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Right;
				tmp[k++] = V_Top;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top;		// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = U_Left;
				tmp[k++] = V_Top;
			}



			vw_Draw3D(ePrimitiveType::TRIANGLES, 6 * (DrawLifeNum + DrawEnergNum), RI_2f_XY | RI_1_TEX | RI_4f_COLOR, tmp, 8 * sizeof(tmp[0]));

			if (tmp != nullptr) {
				delete [] tmp;
				tmp = nullptr;
			}
			vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
			vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			vw_BindTexture(0, 0);
		}
	}












	// выводим кол-во опыта
	DrawGameExpMoney((int)GameExperience, (int)GameMoney);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Скорость игры, если была изменена
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (GameContentTransp <= 0.0f)
		CurrentGameSpeed = GameConfig().GameSpeed;
	if (CurrentGameSpeedShowTime > 0.0f) {
		float GameSpeedShowTransp = CurrentGameSpeedShowTime;
		if (GameSpeedShowTransp > 1.0f)
			GameSpeedShowTransp = 1.0f;

		vw_SetFontSize(20);
		int TmpFontSize = (GameConfig().InternalWidth - vw_TextWidth("%s x%1.1f", vw_GetText("Game Speed:"), CurrentGameSpeed)) / 2;
		vw_DrawText(TmpFontSize, 80, 0, 0, 1.0f, eRGBCOLOR::white, GameSpeedShowTransp, "%s x%1.1f", vw_GetText("Game Speed:"), CurrentGameSpeed);
		ResetFontSize();

		CurrentGameSpeedShowTime -= TimeDelta;
		if (CurrentGameSpeedShowTime < 0.0f) CurrentGameSpeedShowTime = 0.0f;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Выводим данные по оружию и его состоянию
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	DrawGameWeaponSlots();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Выводим название миссии... нужно-не нужно, там разберемся
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GameDrawMissionTitle();



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Выводим... миссия провалена... там же выходим
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GameDrawMissionFailed();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Рисуем меню, всегда самое последнее в игровой 2д части
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// делаем плавное появление меню
	if (NeedShowGameMenu) {
		GameContentTransp += 2.0f*(vw_GetTimeThread(0)-LastGameUpdateTime);
		if (GameContentTransp >= 1.0f) {
			GameContentTransp = 1.0f;
			NeedShowGameMenu = false;
			DrawGameCursor = true;

			// release mouse control
			SDL_SetWindowGrab(vw_GetSDLWindow(), SDL_FALSE);
			SDL_WarpMouseInWindow(vw_GetSDLWindow(), LastMouseXR, LastMouseYR);
		}
		// плавно возвращаем игре сокрость
		if (GameContentTransp != 0.0f)
			vw_SetTimeThreadSpeed(1, (1.0f - GameContentTransp) * GameConfig().GameSpeed);
	}
	// делаем полавное угасание меню
	if (NeedHideGameMenu) {
		GameContentTransp -= vw_GetTimeThread(0) - LastGameUpdateTime;
		if (GameContentTransp <= 0.0f) {
			GameContentTransp = 0.0f;
			NeedHideGameMenu = false;
			GameMenuStatus = eGameMenuStatus::GAME_MENU;

			// grab mouse control for both - windows and fullscren mode (need this for multi-monitor systems)
			SDL_SetWindowGrab(vw_GetSDLWindow(), SDL_TRUE);
			SDL_WarpMouseInWindow(vw_GetSDLWindow(), LastMouseXR, LastMouseYR);
		}
		// останавливаем игру
		vw_SetTimeThreadSpeed(1, (1.0f - GameContentTransp) * GameConfig().GameSpeed);
	}
	LastGameUpdateTime = vw_GetTimeThread(0);

	// если можем - рисуем игровое меню
	if (GameContentTransp > 0.0f) {
		if (GameMissionCompleteStatus) {
			// выводим подложку меню
			SrcRect(2, 2, 564-2, 564-2);
			DstRect(GameConfig().InternalWidth / 2 - 256 - 26, 128 - 28, GameConfig().InternalWidth / 2 - 256 + 534, 128 + 532);
			vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/dialog512_512.tga"),
					   true, GameContentTransp);
			// название меню
			int Size = vw_TextWidth(vw_GetText("Mission Complete"));
			float WScale = 0;
			if (Size > 190) {
				Size = 190;
				WScale = -190;
			}
			vw_DrawText(GameConfig().InternalWidth / 2 - 123 - Size / 2, 128+21, WScale, 0, 1.0f, eRGBCOLOR::yellow, 0.7f * GameContentTransp, vw_GetText("Mission Complete"));


			int Y = 128+90;
			int Prir = 36;

			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, 0, 0, 1.0f, eRGBCOLOR::yellow, 0.5f * GameContentTransp, vw_GetText("Type"));
			Size = vw_TextWidth(vw_GetText("Killed"));
			WScale = 0;
			if (Size > 70) {
				Size = 70;
				WScale = -70;
			}
			vw_DrawText(GameConfig().InternalWidth / 2 - 31 + Size / 2, Y, WScale, 0, 1.0f, eRGBCOLOR::yellow, 0.5f * GameContentTransp, vw_GetText("Killed"));
			Size = vw_TextWidth(vw_GetText("Bonus"));
			WScale = 0;
			if (Size > 70) {
				Size = 70;
				WScale = -70;
			}
			vw_DrawText(GameConfig().InternalWidth / 2 + 97 + Size / 2, Y, WScale, 0, 1.0f, eRGBCOLOR::yellow, 0.5f * GameContentTransp, vw_GetText("Bonus"));
			Y += Prir;

			WScale = -210;

			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  vw_GetText("Alien Spaceships"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", AlienShipsKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)AlienShipsKillBonus);
			Y += Prir;
			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, eRGBCOLOR::white, GameContentTransp, vw_GetText("Alien Motherships"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", AlienMotherShipsKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)AlienMotherShipsKillQuant);
			Y += Prir;
			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, eRGBCOLOR::white, GameContentTransp, vw_GetText("Pirate Spaceships"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", PirateShipsKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)PirateShipsKillBonus);
			Y += Prir;
			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, eRGBCOLOR::white, GameContentTransp, vw_GetText("Pirate Vehicles"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", PirateVehiclesKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)PirateVehiclesKillBonus);
			Y += Prir;
			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, eRGBCOLOR::white, GameContentTransp, vw_GetText("Pirate Buildings"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", PirateBuildingsKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)PirateBuildingsKillBonus);
			Y += Prir;
			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp, vw_GetText("Asteroids"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", AsteroidsKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2 + 126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)AsteroidsKillBonus);

			Y += (int)(Prir*1.5);
			vw_DrawText(GameConfig().InternalWidth / 2 - 256+38, Y, WScale, 0, 1.0f, eRGBCOLOR::yellow, GameContentTransp, vw_GetText("Total"));
			vw_DrawText(GameConfig().InternalWidth / 2 + 10, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%04i", AlienShipsKillQuant+AlienMotherShipsKillQuant+
					       PirateShipsKillQuant+PirateVehiclesKillQuant+PirateBuildingsKillQuant+AsteroidsKillQuant);
			vw_DrawText(GameConfig().InternalWidth / 2+126, Y, 0, 0, 1.0f, eRGBCOLOR::white, GameContentTransp,  "%06i", (int)(GameMoney - GameConfig().Profile[CurrentProfile].Money * 1.0f));

			// выводим кнопки меню
			int X = GameConfig().InternalWidth / 2 - 192;
			Y = 545;
			// продолжение игры
			if (DrawButton384(X,Y, vw_GetText("NEXT"), GameContentTransp, &GameButton4Transp, &LastGameButton4UpdateTime)) {
				// переходим к выбору уровня
				ComBuffer = eCommand::SWITCH_FROM_GAME_TO_MISSION_MENU;
				ExitGameWithSave();
			}

		} else {
			switch(GameMenuStatus) {
			// основное меню игры
			case eGameMenuStatus::GAME_MENU: {
				// выводим подложку меню
				SrcRect(2, 2, 564-2, 564-2);
				DstRect(GameConfig().InternalWidth / 2 - 256+4-30, 128+2-30, GameConfig().InternalWidth / 2 - 256+564-30, 128+564-2-30);
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/dialog512_512.tga"),
					  true, GameContentTransp);
				// название меню
				int SizeI = 17 + (234-vw_TextWidth(vw_GetText("GAME MENU")))/2;
				vw_DrawText(GameConfig().InternalWidth / 2 - 256 + SizeI, 128+22, 0, 0, 1.0f, eRGBCOLOR::yellow, 0.7f * GameContentTransp, vw_GetText("GAME MENU"));

				// выводим кнопки меню


				int X = GameConfig().InternalWidth / 2 - 192;
				int Y = 225;
				int Prir = 100;

				// продолжаем игру
				if (DrawButton384(X,Y, vw_GetText("RESUME"), GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime)) {
					GameMenu = false;
					NeedShowGameMenu = false;
					NeedHideGameMenu = true;
					DrawGameCursor = false;
					// установка в последюю точку указателя
					SDL_WarpMouseInWindow(vw_GetSDLWindow(), LastMouseXR, LastMouseYR);

					if (vw_IsSoundAvailable(SoundShowHideMenu))
						vw_StopSound(SoundShowHideMenu, 150);
					SoundShowHideMenu = PlayMenuSFX(eMenuSFX::MissionHideMenu, 1.0f);
				}

				// выход в настройки
				Y = Y+Prir;
				if (DrawButton384(X,Y, vw_GetText("OPTIONS"), GameContentTransp, &GameButton2Transp, &LastGameButton2UpdateTime)) {
					SetOptionsMenu(eMenuStatus::OPTIONS);
					GameMenuStatus = eGameMenuStatus::OPTIONS;
				}

				// прерываем игру
				Y = Y+Prir;
				if (DrawButton384(X,Y, vw_GetText("RESTART"), GameContentTransp, &GameButton3Transp, &LastGameButton3UpdateTime)) {
					// если убили, выводить диалог не нужно
					if (PlayerFighter == nullptr) {
						ComBuffer = eCommand::SWITCH_TO_GAME;
						ExitGame();
					} else
						SetCurrentDialogBox(eDialogBox::RestartLevelNoSave);
				}

				// выход из игры
				Y = Y+Prir;
				if (DrawButton384(X,Y, vw_GetText("QUIT"), GameContentTransp, &GameButton4Transp, &LastGameButton4UpdateTime)) {
					// если убили, выводить диалог не нужно
					if (PlayerFighter == nullptr) {
						ComBuffer = eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU;
						ExitGame();
					} else
						SetCurrentDialogBox(eDialogBox::QuiToMenuNoSave);
				}

				break;
			}

			// основное меню настроек
			case eGameMenuStatus::OPTIONS:
				OptionsMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime, &GameButton2Transp, &LastGameButton2UpdateTime);
				break;
			// меню продвинутых настроек
			case eGameMenuStatus::OPTIONS_ADVANCED:
				OptionsAdvMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime, &GameButton2Transp, &LastGameButton2UpdateTime);
				break;
			// меню настройки интерфейса
			case eGameMenuStatus::INTERFACE:
				InterfaceMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime);
				break;
			// меню настройки управления
			case eGameMenuStatus::CONFCONTROL:
				ConfControlMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime);
				break;


			}


			// вывод надписи пауза
			SrcRect(0, 0, 256, 64);
			DstRect(GameConfig().InternalWidth - 256 + 60, 768 - 54, GameConfig().InternalWidth + 60, 768 + 10);
			if (GameContentTransp == 1.0f)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName(vw_GetText("lang/en/game/pause.tga")), true, CurrentAlert2*GameContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName(vw_GetText("lang/en/game/pause.tga")), true, GameContentTransp);

		}

	}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// открываем меню
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если в игре - меню, если в меню - выход
	if (!isDialogBoxDrawing()) {
		if (PlayerFighter != nullptr) { // если не убили
			if (vw_GetKeyStatus(SDLK_ESCAPE) || GameMissionCompleteStatusShowDialog) {
				bool NeedPlaySfx = true;
				if (GameMissionCompleteStatusShowDialog) {
					// если уже было открыто меню и появляется меню окончания миссии, не проигрываем sfx
					if (GameMenu)
						NeedPlaySfx = false;
					else
						GameMenu = true;
				} else
					GameMenu = !GameMenu;

				if (GameMenu && (!GameMissionCompleteStatus || GameMissionCompleteStatusShowDialog)) { // открытываем меню с результатом миссии и больше не даем ничего открывать, после завершения миссии
					NeedShowGameMenu = true;
					NeedHideGameMenu = false;
					if (NeedPlaySfx && vw_IsSoundAvailable(SoundShowHideMenu))
						vw_StopSound(SoundShowHideMenu, 150);
					if (NeedPlaySfx)
						SoundShowHideMenu = PlayMenuSFX(eMenuSFX::MissionShowMenu, 1.0f);
					// сброс кнопки мышки, чтобы случайно не нажали
					vw_GetMouseLeftClick(true);
				} else if (!GameMenu && !GameMissionCompleteStatus) { // открыто меню с выводом результата миссии, нельзя давать его закрывать
					NeedShowGameMenu = false;
					NeedHideGameMenu = true;
					// установка в последюю точку указателя
					SDL_WarpMouseInWindow(vw_GetSDLWindow(), LastMouseXR, LastMouseYR);

					if (NeedPlaySfx && vw_IsSoundAvailable(SoundShowHideMenu))
						vw_StopSound(SoundShowHideMenu, 150);
					if (NeedPlaySfx)
						SoundShowHideMenu = PlayMenuSFX(eMenuSFX::MissionHideMenu, 1.0f);
					DrawGameCursor = false;
				}

				if (GameMissionCompleteStatus && !GameMissionCompleteStatusShowDialog) // в процессе вывода результатов разрешаем только выход в основное меню (отображение диалога)
					SetCurrentDialogBox(eDialogBox::QuiToMenuNoSave);
				GameMissionCompleteStatusShowDialog = false;
				vw_SetKeyStatus(SDLK_ESCAPE, false);
			}
		} else {
			if (vw_GetKeyStatus(SDLK_ESCAPE)) {
				ComBuffer = eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU;
				ExitGame();

				vw_SetKeyStatus(SDLK_ESCAPE, false);
			}
		}
	}




	// черное затемнение, если нужно
	if (NeedOnGame) {
		GameBlackTransp = 1.0f - 2.4f*(vw_GetTimeThread(0) - LastGameOnOffUpdateTime);
		if (GameBlackTransp <= 0.0f) {
			GameBlackTransp = 0.0f;
			NeedOnGame = false;
		}

		SrcRect(0, 0, 2, 2);
		DstRect(0, 0, GameConfig().InternalWidth, 768);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, GameBlackTransp);
	}

	// черное затемнение, если нужно
	if (NeedOffGame) {
		GameBlackTransp = 2.4f * (vw_GetTimeThread(0) - LastGameOnOffUpdateTime);
		if (GameBlackTransp >= 1.0f) {
			GameBlackTransp = 1.0f;
			NeedOffGame = false;

			// выходим из игры
			RealExitGame();
			ComBuffer = NewComBuffer;
		}

		SrcRect(0, 0, 2, 2);
		DstRect(0, 0, GameConfig().InternalWidth, 768);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, GameBlackTransp);
	}



	if (ShowGameTime) vw_DrawText(6,45, 0, 0, 1.0f, eRGBCOLOR::white, 0.99f, "%s %.2f", "mission time:", GameTime);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение 2д части
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_End2DMode();
}






