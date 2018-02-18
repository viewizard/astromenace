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


#include "game.h"


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
	CursorParticleSystem2D->Texture = vw_FindTextureByName("gfx/flare1.tga");

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
		RECT SrcRect, DstRect;
		int mX,mY;
		vw_GetMousePos(&mX,&mY);

		// если нужно, рисуем перетягиваемое оружие
		if (GameStatus == WORKSHOP) DrawDragingWeaponIcon(mX, mY);


		CursorParticleSystem2D->MoveSystem(VECTOR3D(mX*1.0f+23,mY*1.0f+19,0.0f));
		CursorParticleSystem2D->Update(vw_GetTime());
		CursorParticleSystem2D->Draw();

		SetRect(&SrcRect,0,0,64,64 );
		SetRect(&DstRect,mX-13,mY-13,mX+64-13,mY+64-13 );
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/cursor_shadow.tga"), true, 1.0f);
		switch (CurrentCursorStatus)
		{
			case 0:
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/cursor.tga"), true, 0.80f, 0.0f, RI_UL_CORNER, 0.8f, 0.7f, 0.0f);
				break;

			case 1:
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/cursor.tga"), true, CurrentCursorFlash, 0.0f, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
				break;

			case 2:
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/cursor.tga"), true, CurrentCursorFlash, 0.0f, RI_UL_CORNER, 1.0f, 0.2f, 0.0f);
				break;

			case 3:
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/cursor.tga"), true, CurrentCursorFlash, 0.0f, RI_UL_CORNER, 0.8f, 0.7f, 0.0f);
				break;
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вывод вспомогательной информации
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Setup.ShowFPS)
	{
		// фпс
		if (Setup.VSync == 0) vw_DrawFont(6,5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %.1f", vw_GetText("11_fps"), eFPS);
		else vw_DrawFont(6,5, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %.1f (VSync - %s)", vw_GetText("11_fps"), eFPS, vw_GetText("1_On"));
		// кол-во прорисовываемых триугольников
		vw_DrawFont(6,25, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %i", vw_GetText("11_triangles"), vw_GetPrimCount());
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




	// после обхода всех активных элементов меню, надо подкорректировать состояние выбора через клавиатуру (если оно было)
	if (vw_GetKeys(SDLK_TAB)){CurrentKeyboardSelectMenuElement++;vw_SetKeys(SDLK_TAB, false);}

	// если не в игре, используем и кнопки курсора
	if ((GameStatus != GAME) || ((GameStatus == GAME) && (isDialogBoxDrawing() || (GameContentTransp >= 0.99f))))
	{
		if (vw_GetKeys(SDLK_RIGHT) || vw_GetKeys(SDLK_DOWN))
		{
			CurrentKeyboardSelectMenuElement++;
			vw_SetKeys(SDLK_RIGHT, false);
			vw_SetKeys(SDLK_DOWN, false);
		}
		if (vw_GetKeys(SDLK_LEFT) || vw_GetKeys(SDLK_UP))
		{
			CurrentKeyboardSelectMenuElement--;
			vw_SetKeys(SDLK_LEFT, false);
			vw_SetKeys(SDLK_UP, false);
			if (CurrentKeyboardSelectMenuElement < 1) CurrentKeyboardSelectMenuElement = CurrentActiveMenuElement;
		}
	}

	if (CurrentKeyboardSelectMenuElement > 0)
	{
		// если у нас вообще есть активные элементы, ставим на первый
		if (CurrentActiveMenuElement > 0)
		{
			if (CurrentKeyboardSelectMenuElement > CurrentActiveMenuElement) CurrentKeyboardSelectMenuElement = 1;
		}
		else CurrentKeyboardSelectMenuElement = 0;
	}
	CurrentActiveMenuElement = 0;




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
	if (vw_GetKeys(SDLK_PRINTSCREEN) || vw_GetKeys(SDLK_F12))
	{
		char SaveFileName[MAX_PATH];

		strcpy(SaveFileName, ScreenshotDir);
		strcat(SaveFileName, "/AstroMenaceScreenshot");

		char res[128];
		sprintf(res,"%4.1f",SDL_GetTicks()/1000.0f);
		strcat(SaveFileName, res);
		strcat(SaveFileName, ".bmp");

		vw_Screenshot(Setup.Width, Setup.Height, SaveFileName);
		vw_SetKeys(SDLK_PRINTSCREEN, false);
		vw_SetKeys(SDLK_F12, false);
	}

	// управление скоростью игры, только в самой игре!
	if ((GameStatus == GAME) && (GameContentTransp<=0.0f) && !GameMissionCompleteStatus)
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
#if defined(__APPLE__) && defined(__MACH__)
	if ((vw_GetKeys(SDLK_LSUPER) || vw_GetKeys(SDLK_RSUPER)) && vw_GetKeys(SDLK_q))
#else
	if (((vw_GetKeys(SDLK_LALT) || vw_GetKeys(SDLK_RALT)) && vw_GetKeys(SDLK_F4)) |
		((vw_GetKeys(SDLK_LCTRL) || vw_GetKeys(SDLK_RCTRL)) && vw_GetKeys(SDLK_q)))
#endif // defined(__APPLE__) && defined(__MACH__)
	{
		CanQuit = true;
		Quit = true;

		vw_SetKeys(SDLK_LALT, false);
		vw_SetKeys(SDLK_RALT, false);
		vw_SetKeys(SDLK_F4, false);
		vw_SetKeys(SDLK_q, false);
	}





}


