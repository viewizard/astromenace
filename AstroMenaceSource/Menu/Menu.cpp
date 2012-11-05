/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu.cpp

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


#include "../Game.h"



//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------

eParticleSystem *psSpace = 0;

float LastMenuUpdateTime = 0.0f;
float MenuContentTransp = 0.0f;
float LastMenuOnOffUpdateTime = 0.0f;
float MenuBlackTransp = 0.0f;
bool NeedOnMenu = false;// если нужно выйти из черного
bool NeedOffMenu = false;// если нужно выйти из черного
bool NeedShowMenu = false;
bool NeedHideMenu = false;
eGameStatus NextMenu;
eGameStatus PrevMenu = MAIN_MENU;

float Button1Transp = 1.0f;
float LastButton1UpdateTime = 0.0f;
float Button2Transp = 1.0f;
float LastButton2UpdateTime = 0.0f;
float Button3Transp = 1.0f;
float LastButton3UpdateTime = 0.0f;
float Button4Transp = 1.0f;
float LastButton4UpdateTime = 0.0f;
float Button5Transp = 1.0f;
float LastButton5UpdateTime = 0.0f;
float Button6Transp = 1.0f;
float LastButton6UpdateTime = 0.0f;
float Button7Transp = 1.0f;
float LastButton7UpdateTime = 0.0f;
float Button8Transp = 1.0f;
float LastButton8UpdateTime = 0.0f;
float Button9Transp = 1.0f;
float LastButton9UpdateTime = 0.0f;
float Button10Transp = 1.0f;
float LastButton10UpdateTime = 0.0f;
float Button11Transp = 1.0f;
float LastButton11UpdateTime = 0.0f;
float Button12Transp = 1.0f;
float LastButton12UpdateTime = 0.0f;
float Button13Transp = 1.0f;
float LastButton13UpdateTime = 0.0f;
float Button14Transp = 1.0f;
float LastButton14UpdateTime = 0.0f;


// для прорисовки подложки с тайловой анимацией
extern float StarsTileUpdateTime;
extern float StarsTileUpdateTime2;






//------------------------------------------------------------------------------------
// инициализация меню
//------------------------------------------------------------------------------------
void InitMenu()
{
	CreateCursor();

	// установка курсора на центр
	// получаем размер клиентской области
	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AWw = W*1.0f;
	float AHw = H*1.0f;
	float ARWidth;
	float ARHeight;
	vw_GetAspectWH(&ARWidth, &ARHeight);
	// установка мышки, чтобы не учитывать перемещения в меню
	SDL_WarpMouse((int)((512.0f+256.0f)/(ARWidth/AWw)), (int)(384.0f/(ARHeight/AHw)));



	Button1Transp = 1.0f;
	LastButton1UpdateTime = 0.0f;
	Button2Transp = 1.0f;
	LastButton2UpdateTime = 0.0f;
	Button3Transp = 1.0f;
	LastButton3UpdateTime = 0.0f;
	Button4Transp = 1.0f;
	LastButton4UpdateTime = 0.0f;
	Button5Transp = 1.0f;
	LastButton5UpdateTime = 0.0f;
	Button6Transp = 1.0f;
	LastButton6UpdateTime = 0.0f;
	Button7Transp = 1.0f;
	LastButton7UpdateTime = 0.0f;
	Button8Transp = 1.0f;
	LastButton8UpdateTime = 0.0f;
	Button9Transp = 1.0f;
	LastButton9UpdateTime = 0.0f;
	Button10Transp = 1.0f;
	LastButton10UpdateTime = 0.0f;
	Button11Transp = 1.0f;
	LastButton11UpdateTime = 0.0f;
	Button12Transp = 1.0f;
	LastButton12UpdateTime = 0.0f;
	Button13Transp = 1.0f;
	LastButton13UpdateTime = 0.0f;
	Button14Transp = 1.0f;
	LastButton14UpdateTime = 0.0f;
	LastMenuUpdateTime = 0.0f;
	MenuContentTransp = 0.0f;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним камеру, всегда до работы со скриптом (!!!)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_ResizeScene(45.0f, Setup.fAspectRatioWidth/Setup.fAspectRatioHeight, 1.0f, 2000.0f);
	InitGameCamera();
	vw_SetCameraLocation(VECTOR3D(-50,30,-50));
	vw_SetCameraMoveAroundPoint(VECTOR3D(0,0,0), 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));


	if (Script != 0){delete Script; Script = 0;}
	Script = new ScriptEngine;

	// делаем случайный выбор, какой скрипт прокручивать
	switch (vw_iRandNum(2))
	{
		case 0: Script->RunScript("DATA/SCRIPT/menu1.xml", vw_GetTime()); break;
		case 1: Script->RunScript("DATA/SCRIPT/menu2.xml", vw_GetTime()); break;
		case 2: Script->RunScript("DATA/SCRIPT/menu3.xml", vw_GetTime()); break;
		// на всякий случай
		default: Script->RunScript("DATA/SCRIPT/menu1.xml", vw_GetTime()); break;
	}

	// немного прокручиваем скрипт
	float Time1 = vw_GetTime();
	Script->StartTime = Time1-30;
	Script->TimeLastOp = Time1-30;
	for (float i=Time1-30; i<Time1;i+=1.0f)
	{
		UpdateAllObject3D(i);
		Script->Update(i);
	}
	Script->StartTime = Time1;
	Script->TimeLastOp = Time1;



	// активные частицы космоса
	psSpace = 0;
	if (psSpace==0) psSpace = new eParticleSystem;
	psSpace->ColorStart.r = 0.80f;
	psSpace->ColorStart.g = 0.80f;
	psSpace->ColorStart.b = 1.00f;
	psSpace->ColorEnd.r = 0.70f;
	psSpace->ColorEnd.g = 0.70f;
	psSpace->ColorEnd.b = 1.00f;
	psSpace->AlphaStart = 1.00f;
	psSpace->AlphaEnd   = 0.00f;
	psSpace->SizeStart = 0.10f;
	psSpace->SizeEnd = 0.50f;
	psSpace->Speed      = 7.00f;
	psSpace->SpeedVar   = 5.00f;
	psSpace->Theta      = 0.00f;
	psSpace->Life       = 12.00f;
	psSpace->LifeVar    = 0.00f;
	psSpace->CreationType = 1;
	psSpace->CreationSize = VECTOR3D(2.0f,50.0f,30.0f);
	psSpace->ParticlesPerSec = 100;
	psSpace->Texture = vw_FindTextureByName("DATA/GFX/flare3.tga");
	psSpace->Direction = VECTOR3D(1.0f, 0.0f, 0.0f);
	psSpace->Resize = 0.1f;
	psSpace->SetStartLocation(VECTOR3D(-50,10,-20));

	// немного "прокручиваем", чтобы сразу по появлению было заполнено
	float Time = psSpace->TimeLastUpdate;
	for (float i=Time; i<Time+20;i+=1.0f)
	{
		psSpace->Update(i);
	}
	psSpace->TimeLastUpdate = Time;


	LastMenuUpdateTime = vw_GetTime();
	NeedShowMenu = true;
	NeedHideMenu = false;
	LastMenuOnOffUpdateTime = vw_GetTime();
	MenuBlackTransp = 1.0f;
	NeedOnMenu = true;
	// подстраховка, если не укажем меню, перейдем в основное
	GameStatus = MAIN_MENU;

	DrawGameCursor = true;

	StarsTileUpdateTime = vw_GetTime();
	StarsTileUpdateTime2 = vw_GetTime();
}









//------------------------------------------------------------------------------------
// установка меню
//------------------------------------------------------------------------------------
void SetMenu(eGameStatus Menu)
{
	Audio_PlaySound2D(3,1.0f);

	switch (Menu)
	{
		case INTERFACE:
			Options_MenuLanguage = Setup.MenuLanguage;
			Options_VoiceLanguage = Setup.VoiceLanguage;
			Options_FontNumber = Setup.FontNumber;
			break;


		case OPTIONS:
			Options_Width = Setup.Width;
			Options_Height = Setup.Height;
			Options_BPP = Setup.BPP;
			Options_VSync = Setup.VSync;
			Options_TexturesQuality = Setup.TexturesQuality;
			Options_iAspectRatioWidth = Setup.iAspectRatioWidth;
			break;

		case OPTIONS_ADVANCED:
			Options_TexturesCompression = Setup.TexturesCompression;
			Options_UseGLSL = Setup.UseGLSL;
			Options_MSAA = Setup.MSAA;
			Options_CSAA = Setup.CSAA;
			Options_ShadowMap = Setup.ShadowMap;
			break;

		case CONFCONTROL:
			if (Setup.KeyBoardUp == 0) Setup.KeyBoardUp = SDLK_UP;
			if (Setup.KeyBoardDown == 0) Setup.KeyBoardDown = SDLK_DOWN;
			if (Setup.KeyBoardLeft == 0) Setup.KeyBoardLeft = SDLK_LEFT;
			if (Setup.KeyBoardRight == 0) Setup.KeyBoardRight = SDLK_RIGHT;
			if (Setup.KeyBoardPrimary == 0) Setup.KeyBoardPrimary = SDLK_LCTRL;
			if (Setup.KeyBoardSecondary == 0) Setup.KeyBoardSecondary = SDLK_SPACE;
			if (Setup.MousePrimary == 0) Setup.MousePrimary = SDL_BUTTON_LEFT;
			if (Setup.MouseSecondary == 0) Setup.MouseSecondary = SDL_BUTTON_RIGHT;
			if (Setup.JoystickPrimary == -1) Setup.JoystickPrimary = 0;
			if (Setup.JoystickSecondary == -1) Setup.JoystickSecondary = 1;
			break;

		case TOP_SCORES:
			// копируем исходные данные
			for (int i=0; i<10; i++)
			{
				strcpy(GameName[i], Setup.TopScores[i].Name);
				GameScore[i] = Setup.TopScores[i].Score;
			}

			// проверяем профайлы
			for (int j=0; j<5; j++)
			if (Setup.Profile[j].Used)
			{
				AddTopScores(Setup.Profile[j].Experience, Setup.Profile[j].Name, true);
			}
			break;

		case MISSION:
			// ставим нужный лист миссий
			StartMission = 0;
			EndMission = 4;
			if (CurrentMission != -1)
			while (!(StartMission<=CurrentMission && CurrentMission<=EndMission))
			{
				StartMission += 5;
				EndMission += 5;
			}
			break;

		case CREDITS:
			CreditsCurrentPos = 0.0f;
			LastCreditsCurrentPosUpdateTime = vw_GetTime();
			break;

		default:
			break;

	}

	NextMenu = Menu;

	// прячем текущее меню
	NeedShowMenu = false;
	NeedHideMenu = true;
	LastMenuUpdateTime = vw_GetTime();



}



void SetMenu2(eGameStatus Menu)
{

	switch (Menu)
	{
		case INFORMATION:
			CreateInfoObject();
			break;
		default:
			break;
	}


	switch (GameStatus)
	{
		case INFORMATION:
			CreateNum = 1;
			break;
		case WORKSHOP:
			WorkshopDestroyData();
			VoiceNeedMoreEnergy = 0;
			VoiceAmmoOut = 0;
			break;
		case MISSION:
			// ставим нужный лист миссий
			StartMission = 0;
			EndMission = 4;
			if (CurrentMission != -1)
			while (!(StartMission<=CurrentMission && CurrentMission<=EndMission))
			{
				StartMission += 5;
				EndMission += 5;
			}
			break;
		default:
			break;
	}



	PrevMenu = GameStatus;
	GameStatus = Menu;

	float Time = vw_GetTime();
	Button1Transp = 1.0f;
	LastButton1UpdateTime = Time;
	Button2Transp = 1.0f;
	LastButton2UpdateTime = Time;
	Button3Transp = 1.0f;
	LastButton3UpdateTime = Time;
	Button4Transp = 1.0f;
	LastButton4UpdateTime = Time;
	Button5Transp = 1.0f;
	LastButton5UpdateTime = Time;
	Button6Transp = 1.0f;
	LastButton6UpdateTime = Time;
	Button7Transp = 1.0f;
	LastButton7UpdateTime = Time;
	Button8Transp = 1.0f;
	LastButton8UpdateTime = Time;
	Button9Transp = 1.0f;
	LastButton9UpdateTime = Time;
	Button10Transp = 1.0f;
	LastButton10UpdateTime = Time;
	Button11Transp = 1.0f;
	LastButton11UpdateTime = Time;
	Button12Transp = 1.0f;
	LastButton12UpdateTime = Time;
	Button13Transp = 1.0f;
	LastButton13UpdateTime = Time;
	Button14Transp = 1.0f;
	LastButton14UpdateTime = Time;
}






//------------------------------------------------------------------------------------
// прорисовка меню
//------------------------------------------------------------------------------------
void DrawMenu()
{

	// делаем плавное появление меню
	if (NeedShowMenu)
	{
		MenuContentTransp = 2.4f*(vw_GetTime()-LastMenuUpdateTime);
		if (MenuContentTransp >= 1.0f)
		{
			MenuContentTransp = 1.0f;
			NeedShowMenu = false;
			LastMenuUpdateTime = vw_GetTime();

			// выводим подсказку, если нужно
			if (GameStatus == PROFILE)
				if (Setup.NeedShowHint[0]) SetCurrentDialogBox(9);
			if (GameStatus == WORKSHOP)
			{
				if (CurrentWorkshop == 1)
					if (Setup.NeedShowHint[1]) SetCurrentDialogBox(10);
				if (CurrentWorkshop == 2)
					if (Setup.NeedShowHint[2]) SetCurrentDialogBox(11);
				if (CurrentWorkshop == 3)
					if (Setup.NeedShowHint[3]) SetCurrentDialogBox(12);
			}
			if (GameStatus == MISSION)
			{
				// проверяем, если в одном из профайлов уже открыли 3 миссии - выводим хинт с пожертвованием
				bool NeedShowDonate = false;
				for (int i=0; i<5; i++)
				{
					if (Setup.Profile[i].Used)
						if (Setup.Profile[i].ByMissionExperience[2] > 0) NeedShowDonate = true;
				}

				if (NeedShowDonate)
					if (Setup.NeedShowHint[6]) SetCurrentDialogBox(15);
			}

		}
	}

	// делаем полавное угасание меню
	if (NeedHideMenu)
	{
		MenuContentTransp = 1.0f - 2.4f*(vw_GetTime() - LastMenuUpdateTime);
		if (MenuContentTransp <= 0.0f)
		{
			MenuContentTransp = 0.0f;
			SetMenu2(NextMenu);
			LastMenuUpdateTime = vw_GetTime();
			NeedShowMenu = true;
			NeedHideMenu = false;
		}
	}


	vw_SetCameraMoveAroundPoint(VECTOR3D(0,0,0), 0.0f, VECTOR3D(0.0f, 0.0f, 0.0f));
	vw_CameraLookAt();




	// всегда первым рисуем скайбокс и "далекое" окружение
	StarSystemUpdate();
	StarSystemDraw(1);




	// рисуем название игры, чтобы звезды и корабли пролетали перед ним
	vw_Start2DMode(-1,1);
	// надпись AstroMenace
	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,863,128 );
	int StartX = (Setup.iAspectRatioWidth - 863)/2;
	SetRect(&DstRest,StartX,10,StartX+863,10+128);

	if (GameStatus != WORKSHOP &&
		GameStatus != INFORMATION &&
		GameStatus != INTERFACE &&
		GameStatus != OPTIONS &&
		GameStatus != CONFCONTROL &&
		GameStatus != OPTIONS_ADVANCED)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/astromenace.tga"),
			true, MenuContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
	}

	vw_End2DMode();



	// рисуем все 3д объекты
	DrawAllObject3D(1);



	// после полной прорисовки делаем обновление данных
	UpdateAllObject3D(vw_GetTime());
	vw_UpdateAllParticleSystems(vw_GetTime());




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работаем со скриптом, пока он есть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Script != 0)
	if (!Script->Update(vw_GetTime()))
	{
		// удаляем скрипт
		delete Script; Script = 0;
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переходим в 2д режим для прорисовки всех 2д частей
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_Start2DMode(-1,1);




	switch(GameStatus)
	{
		case MAIN_MENU:		MainMenu(); break;
		case TOP_SCORES:	TopScoresMenu(); break;
		case INTERFACE:		InterfaceMenu(); break;
		case OPTIONS:		OptionsMenu(); break;
		case OPTIONS_ADVANCED: OptionsAdvMenu(); break;
		case INFORMATION:	InformationMenu(); break;
		case CREDITS:		CreditsMenu(); break;
		case CONFCONTROL:	ConfControlMenu(); break;
		case PROFILE:		ProfileMenu(); break;
		case DIFFICULTY:	DifficultyMenu(); break;
		case MISSION:		MissionMenu(); break;
		case WORKSHOP:		WorkshopMenu(); break;

		default:
			break;
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// последнее - вывод версии и копирайта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// Version
	vw_DrawFont(6, 740, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %s %s %i", GetText("11_Version"), GAME_VERSION, GetText("11_build"), GAME_BUILD);

	// Copyright
	int CSize = vw_FontSize("%s © 2007-2012, Viewizard", GetText("11_Copyright"));
	vw_DrawFont(Setup.iAspectRatioWidth-7-CSize, 740, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s © 2007-2012, Viewizard", GetText("11_Copyright"));




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_End2DMode();


	// если нужно - рисуем в окошке еще одном
	switch(GameStatus)
	{
		case MAIN_MENU:		break;
		case TOP_SCORES:	break;
		case OPTIONS:		break;
		case CREDITS:		break;

		case INFORMATION:	InformationDrawObject(); break;

		default:
			break;
	}




	// черное затемнение, если нужно
	if (NeedOnMenu)
	{
		MenuBlackTransp = 1.0f - 2.4f*(vw_GetTime() - LastMenuOnOffUpdateTime);
		if (MenuBlackTransp <= 0.0f)
		{
			MenuBlackTransp = 0.0f;
			NeedOnMenu = false;
			LastMenuOnOffUpdateTime = vw_GetTime();
		}

		vw_Start2DMode(-1,1);

		RECT SrcRest, DstRest;
		SetRect(&SrcRest,0,0,2,2);
		SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, MenuBlackTransp);

		vw_End2DMode();
	}

	// черное затемнение, если нужно
	if (NeedOffMenu)
	{
		MenuBlackTransp = 2.4f*(vw_GetTime() - LastMenuOnOffUpdateTime);
		if (MenuBlackTransp >= 1.0f)
		{
			MenuBlackTransp = 1.0f;
			NeedOffMenu = false;
			LastMenuOnOffUpdateTime = vw_GetTime();
			// переход на игру
			WorkshopDestroyData();
			ComBuffer = GAME;
		}

		vw_Start2DMode(-1,1);

		RECT SrcRest, DstRest;
		SetRect(&SrcRest,0,0,2,2);
		SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, MenuBlackTransp);

		vw_End2DMode();
	}

}












//------------------------------------------------------------------------------------
// прорисовка основного меню
//------------------------------------------------------------------------------------
void MainMenu()
{

	int Prir = 100;
	int X = (Setup.iAspectRatioWidth - 384)/2;
	int Y = 165;

	if (DrawButton384(X,Y, GetText("1_START_GAME"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
	{
		ComBuffer = PROFILE;
	}





	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_TOP_SCORES"), MenuContentTransp, &Button2Transp, &LastButton2UpdateTime)) ComBuffer = TOP_SCORES;

	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_OPTIONS"), MenuContentTransp, &Button3Transp, &LastButton3UpdateTime)) ComBuffer = OPTIONS;

	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_INFORMATION"), MenuContentTransp, &Button4Transp, &LastButton4UpdateTime)) ComBuffer = INFORMATION;

	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_CREDITS"), MenuContentTransp, &Button5Transp, &LastButton5UpdateTime)) ComBuffer = CREDITS;

	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_QUIT"), MenuContentTransp, &Button6Transp, &LastButton6UpdateTime))SetCurrentDialogBox(0);

}




