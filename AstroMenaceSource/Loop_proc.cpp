/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Loop_proc.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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


#include "Game.h"


// командный буфер
int ComBuffer = 0;

// для просчета фпс
float LastSecond;
#define UPDATE_SPEED_MS 1.0f
float eFPS = 0;
unsigned int eCurrentFrames = 0;

// для работы курсора
// текущий цвет 0-зеленый, 1-можно на нажимать, 2-нельзя нажимать
int CurrentCursorStatus;
// состояние курсора
float CurrentCursorFlash = 1.0f;
float CurrentCursorFlashLastTime = -1.0f;
bool DrawGameCursor = true;

extern eParticleSystem2D *CursorParticleSystem2D;
void DrawDragingWeaponIcon(int X, int Y);

extern float CurrentGameSpeedShowTime;
extern bool NeedOffMenu;



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void CreateCursor()
{
	// курсор
	CursorParticleSystem2D = new eParticleSystem2D;
	CursorParticleSystem2D->ColorStart.r = 1.00f;
	CursorParticleSystem2D->ColorStart.g = 1.00f;
	CursorParticleSystem2D->ColorStart.b = 0.30f;
	CursorParticleSystem2D->ColorEnd.r = 1.00f;
	CursorParticleSystem2D->ColorEnd.g = 0.00f;
	CursorParticleSystem2D->ColorEnd.b = 0.00f;
	CursorParticleSystem2D->AlphaStart = 0.50f;
	CursorParticleSystem2D->AlphaEnd   = 0.00f;
	CursorParticleSystem2D->SizeStart  = 28.00f;
	CursorParticleSystem2D->SizeVar    = 2.50f;
	CursorParticleSystem2D->SizeEnd    = 00.00f;
	CursorParticleSystem2D->Speed      = 100.00f;
	CursorParticleSystem2D->SpeedOnCreation	   = 8.00f;
	CursorParticleSystem2D->SpeedVar   = 20.00f;
	CursorParticleSystem2D->Theta      = 15.00f;
	CursorParticleSystem2D->Life       = 0.40f;
	CursorParticleSystem2D->LifeVar       = 0.05f;
	CursorParticleSystem2D->ParticlesPerSec = 50;
	CursorParticleSystem2D->Direction = VECTOR3D(0.0f, 1.0f, 0.0f);
	CursorParticleSystem2D->SetRotation(VECTOR3D(0.0f, 0.0f, -45.0f));
	CursorParticleSystem2D->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");

}






//------------------------------------------------------------------------------------
// зацикленная процедура работы программы...
//------------------------------------------------------------------------------------
void Loop_Proc()
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работа с курсором
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// начальная установка
	CurrentCursorStatus = 0;
	if (CurrentCursorFlashLastTime == -1.0f)
	{
		CurrentCursorFlashLastTime = vw_GetTime();
	}
	else
	{
		CurrentCursorFlash -= vw_GetTime() - CurrentCursorFlashLastTime;
		if (CurrentCursorFlash < 0.3f) CurrentCursorFlash = 1.0f;
		CurrentCursorFlashLastTime = vw_GetTime();
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// начало прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// рисуем то, что нужно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(GameStatus)
	{
		case MAIN_MENU:
		case TOP_SCORES:
		case INTERFACE:
		case OPTIONS:
		case INFORMATION:
		case CREDITS:
		case CONFCONTROL:
		case OPTIONS_ADVANCED:
		case PROFILE:
		case DIFFICULTY:
		case MISSION:
		case WORKSHOP:
			DrawMenu();
			break;

		case GAME:
			DrawGame();
			break;
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переходим в 2д режим
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_Start2DMode(-1,1);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вывод всех диалогов
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	DrawDialogBox();



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// рисуем курсор
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (DrawGameCursor)
	{
		RECT SrcRest, DstRest;
		int mX,mY;
		vw_GetMousePos(&mX,&mY);

		// если нужно, рисуем перетягиваемое оружие
		if (GameStatus == WORKSHOP) DrawDragingWeaponIcon(mX, mY);


		CursorParticleSystem2D->MoveSystem(VECTOR3D(mX*1.0f+23,mY*1.0f+19,0.0f));
		CursorParticleSystem2D->Update(vw_GetTime());
		CursorParticleSystem2D->Draw();

		SetRect(&SrcRest,0,0,64,64 );
		SetRect(&DstRest,mX-13,mY-13,mX+64-13,mY+64-13 );
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/cursor_shadow.tga"), true, 1.0f);
		switch (CurrentCursorStatus)
		{
			case 0:
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/cursor.tga"), true, 0.80f, 0.0f, RI_UL_CORNER, 0.8f, 0.7f, 0.0f);
				break;

			case 1:
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/cursor.tga"), true, CurrentCursorFlash, 0.0f, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
				break;

			case 2:
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/cursor.tga"), true, CurrentCursorFlash, 0.0f, RI_UL_CORNER, 1.0f, 0.2f, 0.0f);
				break;

			case 3:
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/cursor.tga"), true, CurrentCursorFlash, 0.0f, RI_UL_CORNER, 0.8f, 0.7f, 0.0f);
				break;
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вывод вспомогательной информации
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Setup.ShowFPS)
	{
		// фпс
		if (Setup.VSync == 0) vw_DrawFont(6,5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %.1f", GetText("11_fps"), eFPS);
		else vw_DrawFont(6,5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %.1f (VSync - %s)", GetText("11_fps"), eFPS, GetText("1_On"));
		// кол-во прорисовываемых триугольников
		vw_DrawFont(6,25, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %i", GetText("11_triangles"), vw_GetPrimCount());
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_End2DMode();
	vw_EndRendering();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выход по нажатию на Esc
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (vw_GetKeys(SDLK_ESCAPE))
	{
		SetCurrentDialogBox(0);
		vw_SetKeys(SDLK_ESCAPE, false);
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// основной цикл проигрывания звука
	// если это однопоточная компиляция
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Audio_LoopProc();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выполняем то, что есть в буфере команд, если там что-то есть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ComBuffer != 0)
	{

		switch(ComBuffer)
		{
			case MAIN_MENU:				SetMenu(MAIN_MENU); break;
			case TOP_SCORES:			SetMenu(TOP_SCORES); break;
			case INTERFACE:				SetMenu(INTERFACE); break;
			case OPTIONS:				SetMenu(OPTIONS); break;
			case OPTIONS_ADVANCED:		SetMenu(OPTIONS_ADVANCED); break;
			case INFORMATION:			SetMenu(INFORMATION); break;
			case CREDITS:				SetMenu(CREDITS); break;
			case CONFCONTROL:			SetMenu(CONFCONTROL); break;
			case PROFILE:				SetMenu(PROFILE); break;
			case DIFFICULTY:			SetMenu(DIFFICULTY); break;
			case MISSION:				SetMenu(MISSION); break;
			case WORKSHOP:				SetMenu(WORKSHOP); break;


			// переходим на игру
			case 99:					NeedOffMenu = true; LastMenuOnOffUpdateTime = vw_GetTime(); break;
			case GAME:					LoadGameData(1); break;

			// переход игра-меню (выбор миссии)
			case 100:					LoadGameData(0); break;
			// переход игра-главное меню
			case 101:					LoadGameData(0);
										GameStatus = MAIN_MENU;
										break;

			default:
				fprintf(stderr, "ComBuffer = %i... error!", ComBuffer);
				break;

		}

		ComBuffer = 0;
	}




	// самым последним - просчет фпс
	float FPSTime = vw_GetTime();
	if (FPSTime - LastSecond > UPDATE_SPEED_MS)
	{
		eFPS = eCurrentFrames * (FPSTime - LastSecond);
		eCurrentFrames = 0;
		LastSecond = FPSTime;
	}
	++eCurrentFrames;




#ifdef WIN32
	POINT mouse;
	// чтобы не перешли на скринсейвер, показываем "активность"
	GetCursorPos(&mouse);
	// координаты теже, главное - установка
	SetCursorPos(mouse.x, mouse.y);
#endif // WIN32











	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// дополнительные функции, недокументированные возможности
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// выключение fps и примитивов
	if (vw_GetKeys(SDLK_F2))
	{
		Setup.ShowFPS = !Setup.ShowFPS;
		vw_SetKeys(SDLK_F2, false);
	}

	// делаем на рабочем столе бмп скриншоты
	if (vw_GetKeys(SDLK_PRINT) || vw_GetKeys(SDLK_F12))
	{
		char SaveFileName[MAX_PATH];

		strcpy(SaveFileName, ScreenshotDir);

		char res[128];
		sprintf(res,"%4.1f",SDL_GetTicks()/1000.0f);
		strcat(SaveFileName, res);
		strcat(SaveFileName, ".bmp");

		SDL_Surface *GameScreen = SDL_GetVideoSurface();
		vw_Screenshot(GameScreen, SaveFileName);
		vw_SetKeys(SDLK_PRINT, false);
		vw_SetKeys(SDLK_F12, false);
	}

	// управление скоростью игры, только в самой игре!
	if ((GameStatus == GAME) & (GameContentTransp<=0.0f) & !GameMissionCompleteStatus)
	{
		if (vw_GetKeys(Setup.KeyboardDecreaseGameSpeed))
		{
			Setup.GameSpeed -= 0.1f;
			if (Setup.GameSpeed < 0.1f) Setup.GameSpeed = 0.1f;
			CurrentGameSpeedShowTime = 2.0f;
			vw_SetTimeThreadSpeed(1, Setup.GameSpeed);
			vw_SetKeys(Setup.KeyboardDecreaseGameSpeed, false);
		}
		if (vw_GetKeys(Setup.KeyboardResetGameSpeed))
		{
			Setup.GameSpeed = 1.5f;
			vw_SetTimeThreadSpeed(1, Setup.GameSpeed);
			CurrentGameSpeedShowTime = 2.0f;
			vw_SetKeys(Setup.KeyboardResetGameSpeed, false);
		}
		if (vw_GetKeys(Setup.KeyboardIncreaseGameSpeed))
		{
			Setup.GameSpeed += 0.1f;
			if (Setup.GameSpeed > 3.0f) Setup.GameSpeed = 3.0f;
			CurrentGameSpeedShowTime = 2.0f;
			vw_SetTimeThreadSpeed(1, Setup.GameSpeed);
			vw_SetKeys(Setup.KeyboardIncreaseGameSpeed, false);
		}
	}

	if (GameStatus == GAME)
	{
		// изменение вывода состояния вооружения
		if (vw_GetKeys(Setup.KeyboardGameWeaponInfoType))
		{
			Setup.GameWeaponInfoType ++;
			if (Setup.GameWeaponInfoType > 4) Setup.GameWeaponInfoType = 1;
			vw_SetKeys(Setup.KeyboardGameWeaponInfoType, false);
		}
		// изменение типов стрельбы
		if (vw_GetKeys(Setup.KeyboardPrimaryWeaponFireMode))
		{
			Setup.Profile[CurrentProfile].PrimaryWeaponFireMode ++;
			if (Setup.Profile[CurrentProfile].PrimaryWeaponFireMode > 2) Setup.Profile[CurrentProfile].PrimaryWeaponFireMode = 1;
			vw_SetKeys(Setup.KeyboardPrimaryWeaponFireMode, false);
		}
		if (vw_GetKeys(Setup.KeyboardSecondaryWeaponFireMode))
		{
			Setup.Profile[CurrentProfile].SecondaryWeaponFireMode ++;
			if (Setup.Profile[CurrentProfile].SecondaryWeaponFireMode > 2) Setup.Profile[CurrentProfile].SecondaryWeaponFireMode = 1;
			vw_SetKeys(Setup.KeyboardSecondaryWeaponFireMode, false);
		}

	}




	// делаем принудительный выход из игры
	if ((vw_GetKeys(SDLK_LALT) || vw_GetKeys(SDLK_RALT)) && (vw_GetKeys(SDLK_F4) || vw_GetKeys(SDLK_q)))
	{
		CanQuit = true;
		Quit = true;

		vw_SetKeys(SDLK_LALT, false);
		vw_SetKeys(SDLK_RALT, false);
		vw_SetKeys(SDLK_F4, false);
		vw_SetKeys(SDLK_q, false);
	}





}


