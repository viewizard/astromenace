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


#include "../game.h"


ScriptEngine *Script = nullptr;
extern eParticleSystem *psSpace;





// замедление снарядов NPC ... 1-3...
int	GameNPCWeaponPenalty = 1;
// ум. защиты NPC объектов
int	GameNPCArmorPenalty = 1;
// "замедление" наведения NPC ... 1-4
int	GameNPCTargetingSpeedPenalty = 1;
// 0-ограничено, 1-нет
int	GameLimitedAmmo = 0;
// 0-может быть уничтожено, 1-нет
int	GameDestroyableWeapon = 0;
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
CEarthSpaceFighter *PlayerFighter = nullptr;



// флаг отображения меню
bool GameMenu = false;
float GameContentTransp = 0.0f;
float LastGameUpdateTime = 0.0f;
int GameMenuStatus = 1;

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
eParticleSystem2D *EnergyParticleSystem2D = nullptr;
eParticleSystem2D *LifeParticleSystem2D = nullptr;
eParticleSystem2D *Life2ParticleSystem2D = nullptr;
eParticleSystem2D *Life3ParticleSystem2D = nullptr;


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
extern eParticleSystem *Shild1;
extern eParticleSystem *Shild2;

// для звука открытия-закрытия меню в игре
int SoundShowHideMenu = 0;

// для прорисовки подложки с тайловой анимацией
extern float StarsTileUpdateTime;
extern float StarsTileUpdateTime2;

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
void GetGameNumFontData(char Char, RECT *SrcRect)
{
	switch (Char) {
	case '0':
		SetRect(SrcRect,232,4,245,25);
		break;
	case '1':
		SetRect(SrcRect,71,4,84,25);
		break;
	case '2':
		SetRect(SrcRect,88,4,101,25);
		break;
	case '3':
		SetRect(SrcRect,106,4,119,25);
		break;
	case '4':
		SetRect(SrcRect,124,4,137,25);
		break;
	case '5':
		SetRect(SrcRect,142,4,155,25);
		break;
	case '6':
		SetRect(SrcRect,160,4,173,25);
		break;
	case '7':
		SetRect(SrcRect,178,4,191,25);
		break;
	case '8':
		SetRect(SrcRect,196,4,209,25);
		break;
	case '9':
		SetRect(SrcRect,214,4,227,25);
		break;

	case 'E':
		SetRect(SrcRect,47,4,66,25);
		break;
	case 'S':
		SetRect(SrcRect,4,4,21,25);
		break;
	case '$':
		SetRect(SrcRect,25,4,41,25);
		break;

	case ' ':
		SetRect(SrcRect,0,0,13,0);
		break;
	}

}

//------------------------------------------------------------------------------------
// прорисовка денег и опыта
//------------------------------------------------------------------------------------
void DrawGameExpMoney(int Exp, int Money)
{

	RECT DstRect, SrcRect;
	SetRect(&SrcRect, 0, 0, 0, 0);
	int Ystart;
	float Xstart;
	eTexture *Tex = vw_FindTextureByName("game/game_num.tga");
	if (Tex == nullptr)
		return;



	float AW;
	float AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTexture(0, Tex);
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

	float ImageHeight = Tex->Height*1.0f;
	float ImageWidth = Tex->Width*1.0f;

	float tmpPosY = 0;

	float R=1.0f;
	float G=1.0f;
	float B=1.0f;
	float Transp=1.0f;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = new float[(2+2+4)*4*16];
	int k=0;



	// вывод эмблем

	Xstart = Setup.iAspectRatioWidth/2-57.0f;
	Ystart = 5;
	GetGameNumFontData('E', &SrcRect);
	SetRect(&DstRect, (int)Xstart,	Ystart,
		(int)Xstart+SrcRect.right-SrcRect.left, Ystart+SrcRect.bottom-SrcRect.top);

	if (ASpresent) tmpPosY = (AH - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));
	else tmpPosY = (AHw - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));

	float FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
	float FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

	float Yst = (SrcRect.top*1.0f)/ImageHeight;
	float Xst = (SrcRect.left*1.0f)/ImageWidth;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-Yst;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top +tmpPosY;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-FrameHeight;

	tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
	tmp[k++] = DstRect.top +tmpPosY;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-FrameHeight;

	tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
	tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-Yst;

	Xstart = Setup.iAspectRatioWidth/2-56.0f;
	Ystart = 31;
	GetGameNumFontData('$', &SrcRect);
	SetRect(&DstRect, (int)Xstart,	Ystart,
		(int)Xstart+SrcRect.right-SrcRect.left, Ystart+SrcRect.bottom-SrcRect.top);

	if (ASpresent) tmpPosY = (AH - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));
	else tmpPosY = (AHw - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));

	FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
	FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

	Yst = (SrcRect.top*1.0f)/ImageHeight;
	Xst = (SrcRect.left*1.0f)/ImageWidth;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-Yst;

	tmp[k++] = DstRect.left;	// X
	tmp[k++] = DstRect.top +tmpPosY;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-FrameHeight;

	tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
	tmp[k++] = DstRect.top +tmpPosY;	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-FrameHeight;

	tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
	tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
	tmp[k++] = R;
	tmp[k++] = G;
	tmp[k++] = B;
	tmp[k++] = Transp;
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-Yst;



	// вывод опыта

	Xstart = Setup.iAspectRatioWidth/2-57+23.0f;
	Ystart = 5;
	char buffer[10];
	if (Exp < 0) Exp = 0;
	sprintf(buffer,"%i",Exp);

	for (unsigned int i=0; i<7; i++) {
		if (7-i > strlen(buffer)) {
			Transp = 0.2f;
			GetGameNumFontData('0', &SrcRect);
		} else {
			Transp = 1.0f;
			GetGameNumFontData(buffer[i+strlen(buffer)-7], &SrcRect);
		}
		SetRect(&DstRect, (int)Xstart,	Ystart,
			(int)Xstart+SrcRect.right-SrcRect.left, Ystart+SrcRect.bottom-SrcRect.top);



		if (ASpresent) tmpPosY = (AH - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));
		else tmpPosY = (AHw - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));

		FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
		FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

		Yst = (SrcRect.top*1.0f)/ImageHeight;
		Xst = (SrcRect.left*1.0f)/ImageWidth;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = Xst;
		tmp[k++] = 1.0f-Yst;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top +tmpPosY;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = Xst;
		tmp[k++] = 1.0f-FrameHeight;

		tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
		tmp[k++] = DstRect.top +tmpPosY;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = FrameWidth;
		tmp[k++] = 1.0f-FrameHeight;

		tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
		tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = FrameWidth;
		tmp[k++] = 1.0f-Yst;


		Xstart += SrcRect.right - SrcRect.left;
	}


	// вывод денег

	Xstart = Setup.iAspectRatioWidth/2-57+23.0f;
	Ystart = 31;
	if (Money < 0) Money = 0;
	sprintf(buffer,"%i",Money);

	for (unsigned int i=0; i<7; i++) {
		if (7-i > strlen(buffer)) {
			Transp = 0.2f;
			GetGameNumFontData('0', &SrcRect);
		} else {
			Transp = 1.0f;
			GetGameNumFontData(buffer[i+strlen(buffer)-7], &SrcRect);
		}
		SetRect(&DstRect, (int)Xstart,	Ystart,
			(int)Xstart+SrcRect.right-SrcRect.left, Ystart+SrcRect.bottom-SrcRect.top);



		if (ASpresent) tmpPosY = (AH - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));
		else tmpPosY = (AHw - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));

		FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
		FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

		Yst = (SrcRect.top*1.0f)/ImageHeight;
		Xst = (SrcRect.left*1.0f)/ImageWidth;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = Xst;
		tmp[k++] = 1.0f-Yst;

		tmp[k++] = DstRect.left;	// X
		tmp[k++] = DstRect.top +tmpPosY;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = Xst;
		tmp[k++] = 1.0f-FrameHeight;

		tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
		tmp[k++] = DstRect.top +tmpPosY;	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = FrameWidth;
		tmp[k++] = 1.0f-FrameHeight;

		tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
		tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
		tmp[k++] = R;
		tmp[k++] = G;
		tmp[k++] = B;
		tmp[k++] = Transp;
		tmp[k++] = FrameWidth;
		tmp[k++] = 1.0f-Yst;


		Xstart += SrcRect.right - SrcRect.left;
	}


	vw_SendVertices(RI_QUADS, 4*16, RI_2f_XY | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

	if (tmp != nullptr)
		delete [] tmp;
	vw_SetTextureBlend(false, 0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);
}










//------------------------------------------------------------------------------------
// прорисовка цифр
//------------------------------------------------------------------------------------
void DrawGameNumFontWiaFont(int X, int Y, bool Need0, unsigned int NeedNum, const char *Text, ...)
{
	// смотрим значения параметров в строке
	char	text[1024];
	va_list		ap;

	va_start(ap, Text);
	vsprintf(text, Text, ap);
	va_end(ap);
	// в text уже полная строка



	// находим макс. ширину цифр
	int MaxW = 0;
	for (int i=0; i<=9; i++) {
		int TmpMaxW = vw_FontSize("%i", i);
		if (TmpMaxW > MaxW) MaxW = TmpMaxW;
	}



	int Sm = 0;
	if (Need0)
		if (strlen(text)<NeedNum) {
			for (size_t i=0; i<NeedNum-strlen(text); i++) {
				char Cr[2];
				Cr[0] = '0';
				Cr[1] = 0;
				int TmpW = vw_FontSize(Cr);
				Sm = (int)((MaxW - TmpW)/2.0f);
				vw_DrawFont(X+Sm, Y, 0, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, Cr);
				X += MaxW-2;
			}
		}

	// прорисовка текста
	for (size_t i=0; i<strlen(text); i++) {
		char Cr[2];
		Cr[0] = text[i];
		Cr[1] = 0;
		int TmpW = vw_FontSize(Cr);
		Sm = (int)((MaxW - TmpW)/2.0f);
		vw_DrawFont(X+Sm, Y, 0, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, Cr);
		X += MaxW-2;
	}


}








//------------------------------------------------------------------------------------
// Инициализация игровой части
//------------------------------------------------------------------------------------
void InitGame()
{
	//----
	// только для отладки!!!
	if (CurrentProfile<0 || CurrentProfile>4) CurrentProfile = 0;
	if (CurrentMission == -1) CurrentMission = 0;


	GameNPCWeaponPenalty = Setup.Profile[CurrentProfile].NPCWeaponPenalty;
	GameNPCArmorPenalty = Setup.Profile[CurrentProfile].NPCArmorPenalty;
	GameNPCTargetingSpeedPenalty = Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty;
	GameLimitedAmmo = Setup.Profile[CurrentProfile].LimitedAmmo;
	GameDestroyableWeapon = Setup.Profile[CurrentProfile].DestroyableWeapon;
	GameWeaponTargetingMode = Setup.Profile[CurrentProfile].WeaponTargetingMode;
	GameSpaceShipControlMode = Setup.Profile[CurrentProfile].SpaceShipControlMode;

	GameEngineSystem = Setup.Profile[CurrentProfile].EngineSystem;
	// если симулятивный режим, ставим 1...
	if (GameSpaceShipControlMode == 1)
		if (GameEngineSystem == 0) GameEngineSystem = 1;

	GameTargetingSystem = Setup.Profile[CurrentProfile].TargetingSystem;
	GameAdvancedProtectionSystem = Setup.Profile[CurrentProfile].AdvancedProtectionSystem;
	GamePowerSystem = Setup.Profile[CurrentProfile].PowerSystem;
	GameTargetingMechanicSystem = Setup.Profile[CurrentProfile].TargetingMechanicSystem;
	GameMoney = Setup.Profile[CurrentProfile].Money*1.0f;

	// убираем данные этого уровня
	GameExperience = (Setup.Profile[CurrentProfile].Experience - Setup.Profile[CurrentProfile].ByMissionExperience[CurrentMission])*1.0f;

	// забираем эксклюзивное управление мышкой и клавой, если оконный режим
	if (Setup.BPP == 0) SDL_SetWindowGrab(vw_GetSDL2Windows(),SDL_TRUE);


	// сбрасываем все кнопки мыши
	for (int i=0; i<8; i++)
		SDL_MouseCurrentStatus[i] = false;
	// установка мышки на центр
	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	SDL_WarpMouseInWindow(vw_GetSDL2Windows(), W/2, H/2);
	DrawGameCursor = false;

	LastMouseXR = 0;
	LastMouseYR = 0;
	SDL_GetMouseState(&LastMouseXR, &LastMouseYR);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// инициализация счета времени (всегда первым)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_InitTimeNum(1);
	vw_SetTimeThreadSpeed(1, Setup.GameSpeed);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним камеру, всегда до работы со скриптом (!!!)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	InitGameCamera();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним корабль
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	InitGamePlayerShip();


	CreateCursor();


	// !!! пока загрузка идет полная на уровень, и наверно так и нужно оставить
	// иначе нужно выносить перечень загружаемого в скрипт (менять не смогут уровни)


	if (Script != nullptr)
		delete Script;
	Script = new ScriptEngine;

	if (Script != nullptr) {
		if (GetMissionFileName() != nullptr) {
			if (!Script->RunScript(GetMissionFileName(), vw_GetTime(1))) {
				delete Script;
				Script = nullptr;
			}
		} else {
			delete Script;
			Script = nullptr;
		}
	}


	// выводим номер миссии 3 секунды
	GameSetMissionTitleData(3, CurrentMission+1);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// активные частицы космоса
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	psSpace = new eParticleSystem;
	psSpace->ColorStart.r = 0.80f;
	psSpace->ColorStart.g = 0.80f;
	psSpace->ColorStart.b = 1.00f;
	psSpace->ColorEnd.r = 0.70f;
	psSpace->ColorEnd.g = 0.70f;
	psSpace->ColorEnd.b = 1.00f;
	psSpace->AlphaStart = 0.50f;
	psSpace->AlphaEnd   = 1.00f;
	psSpace->SizeStart = 0.40f;
	psSpace->SizeEnd = 0.05f;
	psSpace->Speed      = 25.00f;
	psSpace->SpeedVar   = 5.00f;
	psSpace->Theta      = 0.00f;
	psSpace->Life       = 14.00f;
	psSpace->LifeVar    = 0.00f;
	psSpace->CreationType = 1;
	psSpace->CreationSize = VECTOR3D(200.0f,30.0f,10.0f);
	psSpace->ParticlesPerSec = 100;
	psSpace->Texture[0] = vw_FindTextureByName("gfx/flare3.tga");
	psSpace->Direction = VECTOR3D(0.0f, 0.0f, -1.0f);
	psSpace->SetStartLocation(VECTOR3D(0,10,250));//поправь ниже, на переносе если изменил!!!



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// немного "прокручиваем", чтобы сразу по появлению было заполнено
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);
	vw_SetCameraLocation(VECTOR3D(0,65,-100+10));
	vw_SetCameraMoveAroundPoint(VECTOR3D(0,0,10), 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));


	float Time = psSpace->TimeLastUpdate;
	for (float i=Time; i<Time+25; i+=1.0f) {
		psSpace->Update(i);
	}
	psSpace->TimeLastUpdate = Time;



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним 2д часть, эмблемы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (EnergyParticleSystem2D != nullptr)
		delete EnergyParticleSystem2D;
	EnergyParticleSystem2D = new eParticleSystem2D;
	EnergyParticleSystem2D->ColorStart.r = 0.70f;
	EnergyParticleSystem2D->ColorStart.g = 0.80f;
	EnergyParticleSystem2D->ColorStart.b = 1.00f;
	EnergyParticleSystem2D->ColorEnd.r = 0.00f;
	EnergyParticleSystem2D->ColorEnd.g = 0.00f;
	EnergyParticleSystem2D->ColorEnd.b = 1.00f;
	EnergyParticleSystem2D->AlphaStart = 1.00f;
	EnergyParticleSystem2D->AlphaEnd   = 1.00f;
	EnergyParticleSystem2D->SizeStart  = 12.00f;
	EnergyParticleSystem2D->SizeVar    = 10.00f;
	EnergyParticleSystem2D->SizeEnd    = 0.00f;
	EnergyParticleSystem2D->Speed      = 70.00f;
	EnergyParticleSystem2D->SpeedVar   = 20.00f;
	EnergyParticleSystem2D->Theta      = 360.00f;
	EnergyParticleSystem2D->Life       = 2.10f;
	EnergyParticleSystem2D->LifeVar       = 0.05f;
	EnergyParticleSystem2D->ParticlesPerSec = 50;
	EnergyParticleSystem2D->IsAttractive = true;
	EnergyParticleSystem2D->AttractiveValue = 150.0f;
	EnergyParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");
	EnergyParticleSystem2D->MoveSystem(VECTOR3D(33.0f,29.0f,0.0f));

	if (LifeParticleSystem2D != nullptr)
		delete LifeParticleSystem2D;
	LifeParticleSystem2D = new eParticleSystem2D;
	LifeParticleSystem2D->ColorStart.r = 1.00f;
	LifeParticleSystem2D->ColorStart.g = 0.60f;
	LifeParticleSystem2D->ColorStart.b = 0.20f;
	LifeParticleSystem2D->ColorEnd.r = 0.50f;
	LifeParticleSystem2D->ColorEnd.g = 0.00f;
	LifeParticleSystem2D->ColorEnd.b = 0.00f;
	LifeParticleSystem2D->AlphaStart = 1.00f;
	LifeParticleSystem2D->AlphaEnd   = 1.00f;
	LifeParticleSystem2D->SizeStart  = 25.00f;
	LifeParticleSystem2D->SizeVar    = 5.00f;
	LifeParticleSystem2D->SizeEnd    = 0.00f;
	LifeParticleSystem2D->Speed      = 0.00f;
	LifeParticleSystem2D->SpeedOnCreation	   = 8.00f;
	LifeParticleSystem2D->SpeedVar   = 10.00f;
	LifeParticleSystem2D->Theta      = 360.00f;
	LifeParticleSystem2D->Life       = 1.50f;
	LifeParticleSystem2D->LifeVar       = 0.05f;
	LifeParticleSystem2D->ParticlesPerSec = 70;
	LifeParticleSystem2D->Direction = VECTOR3D(1.0f, 0.0f, 0.0f);
	LifeParticleSystem2D->CreationType = 2;
	LifeParticleSystem2D->CreationSize = VECTOR3D(25.0f, 25.0f, 0.0f);
	LifeParticleSystem2D->DeadZone = 24.0f;
	LifeParticleSystem2D->IsAttractive = true;
	LifeParticleSystem2D->AttractiveValue = 25.0f;
	LifeParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare.tga");
	LifeParticleSystem2D->MoveSystem(VECTOR3D(Setup.fAspectRatioWidth-33.0f,29.0f,0.0f));
	LifeParticleSystem2D->SetRotation(VECTOR3D(0.0f, 0.0f, 90.0f));

	if (Life2ParticleSystem2D != nullptr)
		delete Life2ParticleSystem2D;
	Life2ParticleSystem2D = new eParticleSystem2D;
	Life2ParticleSystem2D->ColorStart.r = 1.00f;
	Life2ParticleSystem2D->ColorStart.g = 0.40f;
	Life2ParticleSystem2D->ColorStart.b = 0.10f;
	Life2ParticleSystem2D->ColorEnd.r = 0.50f;
	Life2ParticleSystem2D->ColorEnd.g = 0.00f;
	Life2ParticleSystem2D->ColorEnd.b = 0.00f;
	Life2ParticleSystem2D->AlphaStart = 1.00f;
	Life2ParticleSystem2D->AlphaEnd   = 1.00f;
	Life2ParticleSystem2D->SizeStart  = 13.00f;
	Life2ParticleSystem2D->SizeVar    = 5.00f;
	Life2ParticleSystem2D->SizeEnd    = 0.00f;
	Life2ParticleSystem2D->Speed      = 0.00f;
	Life2ParticleSystem2D->SpeedOnCreation	   = 8.00f;
	Life2ParticleSystem2D->SpeedVar   = 0.00f;
	Life2ParticleSystem2D->Theta      = 360.00f;
	Life2ParticleSystem2D->Life       = 2.00f;
	Life2ParticleSystem2D->LifeVar       = 0.05f;
	Life2ParticleSystem2D->ParticlesPerSec = 50;
	Life2ParticleSystem2D->CreationType = 1;
	Life2ParticleSystem2D->CreationSize = VECTOR3D(18.0f, 1.0f, 0.0f);
	Life2ParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");
	Life2ParticleSystem2D->MoveSystem(VECTOR3D(Setup.fAspectRatioWidth-33.0f,29.0f,0.0f));


	if (Life3ParticleSystem2D != nullptr)
		delete Life3ParticleSystem2D;
	Life3ParticleSystem2D = new eParticleSystem2D;
	Life3ParticleSystem2D->ColorStart.r = 1.00f;
	Life3ParticleSystem2D->ColorStart.g = 0.40f;
	Life3ParticleSystem2D->ColorStart.b = 0.10f;
	Life3ParticleSystem2D->ColorEnd.r = 0.50f;
	Life3ParticleSystem2D->ColorEnd.g = 0.00f;
	Life3ParticleSystem2D->ColorEnd.b = 0.00f;
	Life3ParticleSystem2D->AlphaStart = 1.00f;
	Life3ParticleSystem2D->AlphaEnd   = 1.00f;
	Life3ParticleSystem2D->SizeStart  = 13.00f;
	Life3ParticleSystem2D->SizeVar    = 5.00f;
	Life3ParticleSystem2D->SizeEnd    = 0.00f;
	Life3ParticleSystem2D->Speed      = 0.00f;
	Life3ParticleSystem2D->SpeedOnCreation	   = 8.00f;
	Life3ParticleSystem2D->SpeedVar   = 0.00f;
	Life3ParticleSystem2D->Theta      = 360.00f;
	Life3ParticleSystem2D->Life       = 2.00f;
	Life3ParticleSystem2D->LifeVar       = 0.05f;
	Life3ParticleSystem2D->ParticlesPerSec = 50;
	Life3ParticleSystem2D->CreationType = 1;
	Life3ParticleSystem2D->CreationSize = VECTOR3D(1.0f, 18.0f, 0.0f);
	Life3ParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");
	Life3ParticleSystem2D->MoveSystem(VECTOR3D(Setup.fAspectRatioWidth-33.0f,29.0f,0.0f));





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// инициализация игрового меню
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GameContentTransp = 0.0f;
	LastGameUpdateTime = vw_GetTime();
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

	GameMenuStatus = 1;

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

	CurentTime = vw_GetTime();
	CurrentAlert2 = 1.0f;
	CurrentAlert3 = 1.0f;

	GameStatus = GAME;

	GameMissionCompleteStatus = false;
	GameMissionCompleteStatusShowDialog = false;

	SoundShowHideMenu = 0;
	CurrentGameSpeedShowTime = 2.0f;

	StarsTileUpdateTime = vw_GetTime(1);
	StarsTileUpdateTime2 = vw_GetTime(1);


	LastGameOnOffUpdateTime = vw_GetTime();
	GameBlackTransp = 1.0f;
	NeedOnGame = true;

	GameTime = 0.0f;
	LastGameTime = vw_GetTime();
}







//------------------------------------------------------------------------------------
// Завершаем игру
//------------------------------------------------------------------------------------
int NewComBuffer;
void ExitGame()
{
	NewComBuffer = ComBuffer;
	ComBuffer = 0; // пока сбрасываем в ноль, чтобы не переключилось до затухания
	NeedOffGame = true;
	LastGameOnOffUpdateTime = vw_GetTime();

	// убираем меню
	if (GameMenu) {
		GameMenu = false;
		NeedShowGameMenu = false;
		NeedHideGameMenu = true;
		DrawGameCursor = false;
		// установка в последюю точку указателя
		SDL_WarpMouseInWindow(vw_GetSDL2Windows(), LastMouseXR, LastMouseYR);
	}
}
void RealExitGame()
{
	// удаляем корабль игрока
	if (PlayerFighter != nullptr) {
		delete PlayerFighter;
		PlayerFighter = nullptr;
	}

	if (EnergyParticleSystem2D != nullptr) {
		delete EnergyParticleSystem2D;
		EnergyParticleSystem2D = nullptr;
	}
	if (LifeParticleSystem2D != nullptr) {
		delete LifeParticleSystem2D;
		LifeParticleSystem2D = nullptr;
	}
	if (Life2ParticleSystem2D != nullptr) {
		delete Life2ParticleSystem2D;
		Life2ParticleSystem2D = nullptr;
	}
	if (Life3ParticleSystem2D != nullptr) {
		delete Life3ParticleSystem2D;
		Life3ParticleSystem2D = nullptr;
	}

	if (Shild1 != nullptr) {
		delete Shild1;
		Shild1 = nullptr;
	}
	if (Shild2 != nullptr) {
		delete Shild2;
		Shild2 = nullptr;
	}

	// отдаем управление
	if (Setup.BPP == 0)
		SDL_SetWindowGrab(vw_GetSDL2Windows(),SDL_FALSE);

	// выгружаем AI файл
	ReleaseGameAI();
}

//------------------------------------------------------------------------------------
// Завершаем игру, нужно сохранить параметры
//------------------------------------------------------------------------------------
void ExitGameWithSave()
{
	// данные по деньгам и опыту
	Setup.Profile[CurrentProfile].Money = (int)GameMoney;

	// если получили больше опыта
	if (Setup.Profile[CurrentProfile].Experience < (int)GameExperience) {
		int Incr = (int)GameExperience - Setup.Profile[CurrentProfile].Experience;
		Setup.Profile[CurrentProfile].ByMissionExperience[CurrentMission] += Incr;
		Setup.Profile[CurrentProfile].Experience += Incr;
	}
	// увеличиваем счетчик пройденной миссии
	Setup.Profile[CurrentProfile].MissionReplayCount[CurrentMission] ++;

	// состояние корпуса коробля
	Setup.Profile[CurrentProfile].ShipHullCurrentStrength = PlayerFighter->Strength;

	// учет состояния оружия
	for (int i=0; i<PlayerFighter->WeaponQuantity; i++) {
		if (Setup.Profile[CurrentProfile].Weapon[i] != 0) {
			// если оружие было уничтожено во время игры
			if (PlayerFighter->Weapon[i]->Strength <= 0.0f) {
				Setup.Profile[CurrentProfile].WeaponAmmo[i] = 0;
				Setup.Profile[CurrentProfile].Weapon[i] = 0;
			} else {
				// если все ок, нужно запомнить сколько осталось в боекомплекте
				Setup.Profile[CurrentProfile].WeaponAmmo[i] = PlayerFighter->Weapon[i]->Ammo;
			}
		}
	}

	// ставим следующую миссию
	CurrentMission ++;
	// перемещаем ограничитель дальше, если это нужно
	if (Setup.Profile[CurrentProfile].OpenLevelNum < CurrentMission)
		Setup.Profile[CurrentProfile].OpenLevelNum = CurrentMission;

	// если дальше уже ничего нет, просто снимаем все... пусть игрок сам выберет
	if (CurrentMission > AllMission-1) {
		CurrentMission = -1;
	}

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


	Setup.Profile[CurrentProfile].LastMission = CurrentMission;

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

	float TimeDelta = vw_GetTime() - CurentTime;
	CurentTime = vw_GetTime();


	// если не в меню - считаем время
	if (GameContentTransp == 0.0f) GameTime += vw_GetTime() - LastGameTime;
	LastGameTime = vw_GetTime();



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// просчитываем индикацию
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CurrentAlert2 -= 1.0f*TimeDelta;
	if (CurrentAlert2 < 0.1f) CurrentAlert2 = 1.0f;
	CurrentAlert3 -= 1.9f*TimeDelta;
	if (CurrentAlert3 < 0.1f) CurrentAlert3 = 1.0f;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Работа с 3д частью... прорисовка, просчет
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GameCameraUpdate(vw_GetTime(1));
	vw_CameraLookAt();


	// всегда первым рисуем скайбокс и "далекое" окружение
	StarSystemUpdate();
	StarSystemDraw(2);


	// рисуем все 3д объекты
	DrawAllObject3D(2);


	// после полной прорисовки делаем обновление данных
	UpdateAllObject3D(vw_GetTime(1));
	vw_UpdateAllParticleSystems(vw_GetTime(1));
	UpdateAllGameLvlText(vw_GetTime(1));

	// проверяем на столкновения
	if (GameContentTransp < 0.99f) // не нужно проверять коллизии, включено меню
		DetectCollisionAllObject3D();


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работаем со скриптом, пока он есть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((Script != nullptr) &&
	    (!Script->Update(vw_GetTime(1)))) {
		// удаляем скрипт
		delete Script;
		Script = nullptr;
	}







	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// 2д часть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_Start2DMode(-1,1);
	RECT SrcRect, DstRect;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Обработка состояния корабля игрока
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	GamePlayerShip();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Выводим верхнюю информационную панель
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Setup.iAspectRatioWidth == 1024) {
		SetRect(&SrcRect,0,0,1024,74);
		SetRect(&DstRect,0,0,1024,74);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("game/game_panel.tga"), true, 1.0f);
	}
	if (Setup.iAspectRatioWidth == 1228) {
		SetRect(&SrcRect,0,0,466,73);
		SetRect(&DstRect,0,0,466,73);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("game/game_panel2.tga"), true, 1.0f);


		SetRect(&SrcRect,1,74,150,145);
		SetRect(&DstRect,540,0,540+149,71);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("game/game_panel2.tga"), true, 1.0f);


		SetRect(&SrcRect,150,74,610,145);
		SetRect(&DstRect,768,0,768+460,71);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("game/game_panel2.tga"), true, 1.0f);
	}


	// эмблема энергии
	EnergyParticleSystem2D->Update(vw_GetTime());
	EnergyParticleSystem2D->Draw();
	{
		// учитываем в эмблеме энергии, сколько у нас ее (визуально меняем вид эмблемы)
		EnergyParticleSystem2D->ParticlesPerSec = (unsigned int) (50 * (CurrentPlayerShipEnergy / GetShipMaxEnergy(GamePowerSystem)));
		if (EnergyParticleSystem2D->ParticlesPerSec == 0) EnergyParticleSystem2D->ParticlesPerSec = 1;
	}
	// эмблема жизни
	LifeParticleSystem2D->Update(vw_GetTime());
	LifeParticleSystem2D->Draw();
	Life2ParticleSystem2D->Update(vw_GetTime());
	Life2ParticleSystem2D->Draw();
	Life3ParticleSystem2D->Update(vw_GetTime());
	Life3ParticleSystem2D->Draw();
	if (PlayerFighter != nullptr) {

		LifeParticleSystem2D->ColorStart.r = 1.00f;
		LifeParticleSystem2D->ColorStart.g = 0.60f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
		LifeParticleSystem2D->ColorStart.b = 0.20f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
		Life2ParticleSystem2D->ColorStart.r = 1.00f;
		Life2ParticleSystem2D->ColorStart.g = 0.60f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
		Life2ParticleSystem2D->ColorStart.b = 0.20f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
		Life3ParticleSystem2D->ColorStart.r = 1.00f;
		Life3ParticleSystem2D->ColorStart.g = 0.60f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);
		Life3ParticleSystem2D->ColorStart.b = 0.20f*(PlayerFighter->Strength/PlayerFighter->StrengthStart);

		// если меньше 10% нужно бить тревогу
		if (PlayerFighter->Strength < PlayerFighter->StrengthStart/10.0f) {
			LifeParticleSystem2D->AlphaStart = 1.00f*CurrentAlert2;
			LifeParticleSystem2D->AlphaEnd   = 1.00f*CurrentAlert2;

			if (CurrentAlert2 > 0.6f) {
				Life2ParticleSystem2D->AlphaStart = 1.00f*CurrentAlert2;
				Life2ParticleSystem2D->AlphaEnd   = 1.00f*CurrentAlert2;
				Life3ParticleSystem2D->AlphaStart = 1.00f*CurrentAlert2;
				Life3ParticleSystem2D->AlphaEnd   = 1.00f*CurrentAlert2;
			} else {
				Life2ParticleSystem2D->AlphaStart = 0.00f;
				Life2ParticleSystem2D->AlphaEnd   = 0.00f;
				Life3ParticleSystem2D->AlphaStart = 0.00f;
				Life3ParticleSystem2D->AlphaEnd   = 0.00f;
			}
		} else { // подчинились, восстанавливаем данные
			LifeParticleSystem2D->AlphaStart = 1.00f;
			LifeParticleSystem2D->AlphaEnd   = 1.00f;
			Life2ParticleSystem2D->AlphaStart = 1.00f;
			Life2ParticleSystem2D->AlphaEnd   = 1.00f;
			Life3ParticleSystem2D->AlphaStart = 1.00f;
			Life3ParticleSystem2D->AlphaEnd   = 1.00f;
		}
	}



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
			CurrentDrawEnergNumFull += GamePowerSystem*0.5f*(vw_GetTime() - LastGameUpdateTime);
			if (CurrentDrawEnergNumFull > NeedDrawEnergNumFull) CurrentDrawEnergNumFull = NeedDrawEnergNumFull;
		} else {
			if (NeedDrawEnergNumFull < CurrentDrawEnergNumFull) {
				CurrentDrawEnergNumFull -= GamePowerSystem*0.5f*(vw_GetTime() - LastGameUpdateTime);
				if (CurrentDrawEnergNumFull < NeedDrawEnergNumFull) CurrentDrawEnergNumFull = NeedDrawEnergNumFull;
			}
		}
		// находим целую часть... т.е. номер последней, которую будем рисовать уже с прозрачностью
		int DrawEnergNum = (int)ceil(CurrentDrawEnergNumFull * 19);

		// находим правильное отображение
		if (NeedDrawLifeNumFull > CurrentDrawLifeNumFull) {
			CurrentDrawLifeNumFull += 0.3f*(vw_GetTime() - LastGameUpdateTime);
			if (CurrentDrawLifeNumFull > NeedDrawLifeNumFull) CurrentDrawLifeNumFull = NeedDrawLifeNumFull;
		} else {
			if (NeedDrawLifeNumFull < CurrentDrawLifeNumFull) {
				CurrentDrawLifeNumFull -= 0.3f*(vw_GetTime() - LastGameUpdateTime);
				if (CurrentDrawLifeNumFull < NeedDrawLifeNumFull) CurrentDrawLifeNumFull = NeedDrawLifeNumFull;
			}
		}
		// находим целую часть... т.е. номер последней, которую будем рисовать уже с прозрачностью
		int DrawLifeNum = (int)ceil(CurrentDrawLifeNumFull * 19);

		if (DrawLifeNum+DrawEnergNum > 0) {
			float R=1.0f;
			float G=1.0f;
			float B=1.0f;

			eTexture *Tex = vw_FindTextureByName("game/game_panel_el.tga");
			if (Tex == nullptr)
				return;

			float AW;
			float AH;
			bool ASpresent = vw_GetAspectWH(&AW, &AH);

			int W, H;
			vw_GetViewport(nullptr, nullptr, &W, &H);
			float AHw = H*1.0f;

			// Установка текстуры и ее свойств...
			vw_SetTexture(0, Tex);
			vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

			float ImageHeight = Tex->Height*1.0f;
			float ImageWidth = Tex->Width*1.0f;

			float tmpPosY = 0;

			// выделяем память
			// буфер для последовательности RI_TRIANGLE_STRIP
			// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
			float *tmp = new float[(2+2+4)*4*(DrawLifeNum+DrawEnergNum)];
			int k = 0;



			// вывод текущего заряда энергии
			// прорисовываем все элементы
			for (int i=0; i<DrawEnergNum; i++) {
				// получаем данные текущего фрагмента
				SetRect(&SrcRect,67+i*20,0,85+i*20,64);
				DstRect = SrcRect;
				// находим прозначность
				float Transp = (CurrentDrawEnergNumFull * 19) - i;
				if (Transp > 1.0f) Transp = 1.0f;


				if (ASpresent) tmpPosY = (AH - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));
				else tmpPosY = (AHw - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));

				float FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
				float FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

				float Yst = (SrcRect.top*1.0f)/ImageHeight;
				float Xst = (SrcRect.left*1.0f)/ImageWidth;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
				tmp[k++] = DstRect.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
				tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;

			}

			// вывод текущего состояния жизни
			// прорисовываем все элементы
			for (int i=0; i<DrawLifeNum; i++) {
				// получаем данные текущего фрагмента
				SetRect(&SrcRect,582+i*20,0,599+i*20,64);
				if (Setup.iAspectRatioWidth == 1024) {
					DstRect = SrcRect;
				}
				if (Setup.iAspectRatioWidth == 1228) {
					//SetRect(&DstRect,Setup.iAspectRatioWidth-446+i*20,0,Setup.iAspectRatioWidth-425+i*20,64);
					SetRect(&DstRect,204+582+i*20,0,204+599+i*20,64);
				}
				// находим прозначность
				float Transp = (CurrentDrawLifeNumFull * 19) - i;
				if (Transp > 1.0f) Transp = 1.0f;

				if (ASpresent) tmpPosY = (AH - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));
				else tmpPosY = (AHw - DstRect.top - DstRect.top - (DstRect.bottom - DstRect.top));

				float FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
				float FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

				float Yst = (SrcRect.top*1.0f)/ImageHeight;
				float Xst = (SrcRect.left*1.0f)/ImageWidth;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DstRect.left;	// X
				tmp[k++] = DstRect.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
				tmp[k++] = DstRect.top +tmpPosY;	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DstRect.left + (DstRect.right - DstRect.left);	// X
				tmp[k++] = DstRect.top +tmpPosY +(DstRect.bottom - DstRect.top);	// Y
				tmp[k++] = R;
				tmp[k++] = G;
				tmp[k++] = B;
				tmp[k++] = Transp;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;
			}



			vw_SendVertices(RI_QUADS, 4*(DrawLifeNum+DrawEnergNum), RI_2f_XY | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

			if (tmp != nullptr) {
				delete [] tmp;
				tmp = nullptr;
			}
			vw_SetTextureBlend(false, 0, 0);
			vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			vw_BindTexture(0, 0);
		}
	}












	// выводим кол-во опыта
	DrawGameExpMoney((int)GameExperience, (int)GameMoney);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Скорость игры, если была изменена
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (GameContentTransp <= 0.0f) CurrentGameSpeed = Setup.GameSpeed;
	if (CurrentGameSpeedShowTime > 0.0f) {
		float GameSpeedShowTransp = CurrentGameSpeedShowTime;
		if (GameSpeedShowTransp>1.0f) GameSpeedShowTransp = 1.0f;

		vw_SetFontSize(20);
		vw_SetFontOffsetY(2);
		int TmpFontSize = (Setup.iAspectRatioWidth-vw_FontSize("%s x%1.1f", vw_GetText("4_Game_Speed:"), CurrentGameSpeed))/2;
		vw_DrawFont(TmpFontSize, 80, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameSpeedShowTransp, "%s x%1.1f", vw_GetText("4_Game_Speed:"), CurrentGameSpeed);
		vw_SetFontSize(Setup.FontSize);
		vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);

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
	// прорисовываем весь 2д текст который есть
	// !!! Важно, должно стоять после надписей, но до меню!!!
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	DrawAllGameLvlText();





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Рисуем меню, всегда самое последнее в игровой 2д части
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// делаем плавное появление меню
	if (NeedShowGameMenu) {
		GameContentTransp += 2.0f*(vw_GetTime()-LastGameUpdateTime);
		if (GameContentTransp >= 1.0f) {
			GameContentTransp = 1.0f;
			NeedShowGameMenu = false;
			DrawGameCursor = true;
			if (Setup.BPP == 0) {
				SDL_SetWindowGrab(vw_GetSDL2Windows(),SDL_FALSE);
				SDL_WarpMouseInWindow(vw_GetSDL2Windows(), LastMouseXR, LastMouseYR);
			}
		}
		// плавно возвращаем игре сокрость
		if (GameContentTransp != 0.0f)
			vw_SetTimeThreadSpeed(1, (1.0f - GameContentTransp)*Setup.GameSpeed);
	}
	// делаем полавное угасание меню
	if (NeedHideGameMenu) {
		GameContentTransp -= 1.0f*(vw_GetTime() - LastGameUpdateTime);
		if (GameContentTransp <= 0.0f) {
			GameContentTransp = 0.0f;
			NeedHideGameMenu = false;
			GameMenuStatus = 1;
			if (Setup.BPP == 0) {
				SDL_SetWindowGrab(vw_GetSDL2Windows(),SDL_TRUE);
				SDL_WarpMouseInWindow(vw_GetSDL2Windows(), LastMouseXR, LastMouseYR);
			}
		}
		// останавливаем игру
		vw_SetTimeThreadSpeed(1, (1.0f - GameContentTransp)*Setup.GameSpeed);
	}
	LastGameUpdateTime = vw_GetTime();

	// если можем - рисуем игровое меню
	if (GameContentTransp > 0.0f) {
		if (GameMissionCompleteStatus) {
			// выводим подложку меню
			SetRect(&SrcRect,2,2,564-2,564-2);
			SetRect(&DstRect,Setup.iAspectRatioWidth/2-256+4-30,128+2-30,Setup.iAspectRatioWidth/2-256+564-30,128+564-2-30);
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/dialog512_512.tga"),
					   true, 1.0f*GameContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
			// название меню
			int Size = vw_FontSize(vw_GetText("1_Mission_Complete"));
			float WScale = 0;
			if (Size > 190) {
				Size = 190;
				WScale = -190;
			}
			vw_DrawFont(Setup.iAspectRatioWidth/2-123-Size/2, 128+21, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, 0.7f*GameContentTransp, vw_GetText("1_Mission_Complete"));


			int Y = 128+90;
			int Prir = 36;

			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, 0, 0, 1.0f, 1.0f,1.0f,0.0f, 0.5f*GameContentTransp, vw_GetText("3_Type"));
			Size = vw_FontSize(vw_GetText("3_Killed"));
			WScale = 0;
			if (Size > 70) {
				Size = 70;
				WScale = -70;
			}
			vw_DrawFont(Setup.iAspectRatioWidth/2-31+Size/2, Y, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, 0.5f*GameContentTransp, vw_GetText("3_Killed"));
			Size = vw_FontSize(vw_GetText("3_Bonus"));
			WScale = 0;
			if (Size > 70) {
				Size = 70;
				WScale = -70;
			}
			vw_DrawFont(Setup.iAspectRatioWidth/2+97+Size/2, Y, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, 0.5f*GameContentTransp, vw_GetText("3_Bonus"));
			Y += Prir;

			WScale = -210;

			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameContentTransp,  vw_GetText("4_Alien_Spaceships"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", AlienShipsKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)AlienShipsKillBonus);
			Y += Prir;
			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameContentTransp, vw_GetText("4_Alien_Motherships"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", AlienMotherShipsKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)AlienMotherShipsKillBonus);
			Y += Prir;
			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameContentTransp, vw_GetText("4_Pirate_Spaceships"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", PirateShipsKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)PirateShipsKillBonus);
			Y += Prir;
			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameContentTransp, vw_GetText("4_Pirate_Vehicles"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", PirateVehiclesKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)PirateVehiclesKillBonus);
			Y += Prir;
			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameContentTransp, vw_GetText("4_Pirate_Buildings"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", PirateBuildingsKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)PirateBuildingsKillBonus);
			Y += Prir;
			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 1.0f*GameContentTransp, vw_GetText("4_Asteroids"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", AsteroidsKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)AsteroidsKillBonus);

			Y += (int)(Prir*1.5);
			vw_DrawFont(Setup.iAspectRatioWidth/2-256+38, Y, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, 1.0f*GameContentTransp, vw_GetText("3_Total"));
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+17, Y, true, 4, "%i", AlienShipsKillQuant+AlienMotherShipsKillQuant+
					       PirateShipsKillQuant+PirateVehiclesKillQuant+PirateBuildingsKillQuant+AsteroidsKillQuant);
			DrawGameNumFontWiaFont(Setup.iAspectRatioWidth/2+127, Y, true, 6, "%i", (int)(GameMoney - Setup.Profile[CurrentProfile].Money*1.0f));

			// выводим кнопки меню
			int X = Setup.iAspectRatioWidth/2-192;
			Y = 545;
			// продолжение игры
			if (DrawButton384(X,Y, vw_GetText("1_NEXT"), GameContentTransp, &GameButton4Transp, &LastGameButton4UpdateTime)) {
				// переходим к выбору уровня
				ComBuffer = 100;
				ExitGameWithSave();
			}

		} else {
			switch(GameMenuStatus) {
			// основное меню игры
			case 1: {
				// выводим подложку меню
				SetRect(&SrcRect,2,2,564-2,564-2);
				SetRect(&DstRect,Setup.iAspectRatioWidth/2-256+4-30,128+2-30,Setup.iAspectRatioWidth/2-256+564-30,128+564-2-30);
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/dialog512_512.tga"),
						   true, GameContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
				// название меню
				int SizeI = 17 + (234-vw_FontSize(vw_GetText("1_GAME_MENU")))/2;
				vw_DrawFont(Setup.iAspectRatioWidth/2-256+SizeI, 128+22, 0, 0, 1.0f, 1.0f,1.0f,0.0f, 0.7f*GameContentTransp, vw_GetText("1_GAME_MENU"));

				// выводим кнопки меню


				int X = Setup.iAspectRatioWidth/2-192;
				int Y = 225;
				int Prir = 100;

				// продолжаем игру
				if (DrawButton384(X,Y, vw_GetText("1_RESUME"), GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime)) {
					GameMenu = false;
					NeedShowGameMenu = false;
					NeedHideGameMenu = true;
					DrawGameCursor = false;
					// установка в последюю точку указателя
					SDL_WarpMouseInWindow(vw_GetSDL2Windows(), LastMouseXR, LastMouseYR);

					if ((SoundShowHideMenu != 0) &&
					    (vw_FindSoundByNum(SoundShowHideMenu) != nullptr))
						vw_FindSoundByNum(SoundShowHideMenu)->Stop(0.15f);
					SoundShowHideMenu = Audio_PlaySound2D(13, 1.0f);
				}

				// выход в настройки
				Y = Y+Prir;
				if (DrawButton384(X,Y, vw_GetText("1_OPTIONS"), GameContentTransp, &GameButton2Transp, &LastGameButton2UpdateTime)) {
					SetOptionsMenu(OPTIONS);
					GameMenuStatus = 2;
				}

				// прерываем игру
				Y = Y+Prir;
				if (DrawButton384(X,Y, vw_GetText("1_RESTART"), GameContentTransp, &GameButton3Transp, &LastGameButton3UpdateTime)) {
					// если убили, выводить диалог не нужно
					if (PlayerFighter == nullptr) {
						ComBuffer = GAME;
						ExitGame();
					} else
						SetCurrentDialogBox(5);
				}

				// выход из игры
				Y = Y+Prir;
				if (DrawButton384(X,Y, vw_GetText("1_QUIT"), GameContentTransp, &GameButton4Transp, &LastGameButton4UpdateTime)) {
					// если убили, выводить диалог не нужно
					if (PlayerFighter == nullptr) {
						ComBuffer = 101;
						ExitGame();
					} else
						SetCurrentDialogBox(41);
				}

				break;
			}

			// основное меню настроек
			case 2:
				OptionsMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime, &GameButton2Transp, &LastGameButton2UpdateTime);
				break;
			// меню продвинутых настроек
			case 3:
				OptionsAdvMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime, &GameButton2Transp, &LastGameButton2UpdateTime);
				break;
			// меню настройки интерфейса
			case 4:
				InterfaceMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime);
				break;
			// меню настройки управления
			case 5:
				ConfControlMenu(GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime);
				break;


			}


			// вывод надписи пауза
			SetRect(&SrcRect,0,0,256,64);
			SetRect(&DstRect,Setup.iAspectRatioWidth-256+60,768-64+10,Setup.iAspectRatioWidth+60,768+10);
			if (GameContentTransp == 1.0f)
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(vw_GetText("12_pause.tga")), true, CurrentAlert2*GameContentTransp);
			else
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(vw_GetText("12_pause.tga")), true, GameContentTransp);

		}

	}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// открываем меню
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если в игре - меню, если в меню - выход
	if (!isDialogBoxDrawing()) {
		if (PlayerFighter != nullptr) { // если не убили
			if (vw_GetKeys(SDLK_ESCAPE) || GameMissionCompleteStatusShowDialog) {
				bool NeedPlaySfx = true;
				// если нужно показать конец игры, все равно его показываем
				if (GameMissionCompleteStatusShowDialog) {
					// чтобы постоянно не проигрывать звук
					if (GameMenu)
						NeedPlaySfx = false;
					else
						GameMenu = true;
				} else
					GameMenu = !GameMenu;

				if (GameMenu) {
					NeedShowGameMenu = true;
					NeedHideGameMenu = false;
					if ((NeedPlaySfx && SoundShowHideMenu != 0) &&
					    (vw_FindSoundByNum(SoundShowHideMenu) != nullptr))
						vw_FindSoundByNum(SoundShowHideMenu)->Stop(0.15f);
					if (NeedPlaySfx)
						SoundShowHideMenu = Audio_PlaySound2D(12, 1.0f);
					// сброс кнопки мышки, чтобы случайно не нажали
					vw_GetWindowLBMouse(true);
				} else {
					NeedShowGameMenu = false;
					NeedHideGameMenu = true;
					// установка в последюю точку указателя
					SDL_WarpMouseInWindow(vw_GetSDL2Windows(), LastMouseXR, LastMouseYR);

					if (NeedPlaySfx &&
					    (SoundShowHideMenu != 0) &&
					    (vw_FindSoundByNum(SoundShowHideMenu) != nullptr))
						vw_FindSoundByNum(SoundShowHideMenu)->Stop(0.15f);
					if (NeedPlaySfx)
						SoundShowHideMenu = Audio_PlaySound2D(13, 1.0f);
					DrawGameCursor = false;
				}

				GameMissionCompleteStatusShowDialog = false;
				vw_SetKeys(SDLK_ESCAPE, false);
			}
		} else {
			if (vw_GetKeys(SDLK_ESCAPE)) {
				ComBuffer = 101;
				ExitGame();

				vw_SetKeys(SDLK_ESCAPE, false);
			}
		}
	}




	// черное затемнение, если нужно
	if (NeedOnGame) {
		GameBlackTransp = 1.0f - 2.4f*(vw_GetTime() - LastGameOnOffUpdateTime);
		if (GameBlackTransp <= 0.0f) {
			GameBlackTransp = 0.0f;
			NeedOnGame = false;
		}

		SetRect(&SrcRect,0,0,2,2);
		SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, GameBlackTransp);
	}

	// черное затемнение, если нужно
	if (NeedOffGame) {
		GameBlackTransp = 2.4f*(vw_GetTime() - LastGameOnOffUpdateTime);
		if (GameBlackTransp >= 1.0f) {
			GameBlackTransp = 1.0f;
			NeedOffGame = false;

			// выходим из игры
			RealExitGame();
			ComBuffer = NewComBuffer;
		}

		SetRect(&SrcRect,0,0,2,2);
		SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, GameBlackTransp);
	}



	if (ShowGameTime) vw_DrawFont(6,45, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %.2f", "mission time:", GameTime);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение 2д части
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_End2DMode();
}






