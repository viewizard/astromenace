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

#include "../game.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../script/script.h"
#include "../object3d/object3d.h"
#include "../gfx/star_system.h"


//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------

float LastMenuUpdateTime = 0.0f;
float MenuContentTransp = 0.0f;
float LastMenuOnOffUpdateTime = 0.0f;
float MenuBlackTransp = 0.0f;
bool NeedOnMenu = false;// если нужно выйти из черного
bool NeedShowMenu = false;
bool NeedHideMenu = false;
eMenuStatus NextMenu;
eMenuStatus PrevMenu{eMenuStatus::MAIN_MENU};

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








//------------------------------------------------------------------------------------
// инициализация меню
//------------------------------------------------------------------------------------
void InitMenu()
{
	// установка курсора на центр
	// получаем размер клиентской области
	float tmpViewportWidth, tmpViewportHeight;
	vw_GetViewport(nullptr, nullptr, &tmpViewportWidth, &tmpViewportHeight);
	// установка мышки, чтобы не учитывать перемещения в меню
	SDL_WarpMouseInWindow(vw_GetSDLWindow(),
			      static_cast<int>((512.0f + 256.0f) / (GameConfig().InternalWidth / tmpViewportWidth)),
			      static_cast<int>(384.0f / (GameConfig().InternalHeight / tmpViewportHeight)));



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
	vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);
	vw_SetCameraLocation(sVECTOR3D(-50,30,-50));
	vw_SetCameraMoveAroundPoint(sVECTOR3D(0,0,0), 0.0f, sVECTOR3D(0.0f, 0.0f, 0.0f));

	// since we use scripts with background animation in the same way as
	// games levels do, we should reset game camera first
	ResetGameCamera();

	if (Script != nullptr) {
		delete Script;
		Script = nullptr;
	}
	Script = new cMissionScript;

	if (GameConfig().MenuScript > 2)
		ChangeGameConfig().MenuScript = 0;
	switch (GameConfig().MenuScript) {
	case 0:
		Script->RunScript("script/menu1.xml", vw_GetTimeThread(0));
		break;
	case 1:
		Script->RunScript("script/menu2.xml", vw_GetTimeThread(0));
		break;
	case 2:
		Script->RunScript("script/menu3.xml", vw_GetTimeThread(0));
		break;
	// на всякий случай
	default:
		Script->RunScript("script/menu1.xml", vw_GetTimeThread(0));
		break;
	}
	ChangeGameConfig().MenuScript++;

	// немного прокручиваем скрипт
	float Time1 = vw_GetTimeThread(0);
	Script->StartTime = Time1-30;
	Script->TimeLastOp = Time1-30;
	for (float i=Time1-30; i<Time1; i+=1.0f) {
		UpdateAllObject3D(i);
		Script->Update(i);
	}
	Script->StartTime = Time1;
	Script->TimeLastOp = Time1;



	LastMenuUpdateTime = vw_GetTimeThread(0);
	NeedShowMenu = true;
	NeedHideMenu = false;
	LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
	MenuBlackTransp = 1.0f;
	NeedOnMenu = true;
	// подстраховка, если не укажем меню, перейдем в основное
	MenuStatus = eMenuStatus::MAIN_MENU;

	DrawGameCursor = true;

	StarSystemInitByType(eDrawType::MENU);
}









//------------------------------------------------------------------------------------
// установка меню
//------------------------------------------------------------------------------------
void SetOptionsMenu(eMenuStatus Menu)
{
	switch (Menu) {
	case eMenuStatus::OPTIONS:
		Options_Width = GameConfig().Width;
		Options_Height = GameConfig().Height;
		Options_BPP = GameConfig().BPP;
		Options_VSync = GameConfig().VSync;
		Options_iAspectRatioWidth = GameConfig().InternalWidth;
		break;

	case eMenuStatus::OPTIONS_ADVANCED:
		Options_TexturesCompressionType = GameConfig().TexturesCompressionType;
		Options_UseGLSL120 = GameConfig().UseGLSL120;
		Options_MSAA = GameConfig().MSAA;
		Options_CSAA = GameConfig().CSAA;
		Options_TexturesAnisotropyLevel = GameConfig().AnisotropyLevel;
		Options_ShadowMap = GameConfig().ShadowMap;
		break;

	case eMenuStatus::CONFCONTROL:
		if (!GameConfig().KeyBoardUp)
			ChangeGameConfig().KeyBoardUp = SDLK_UP;
		if (!GameConfig().KeyBoardDown)
			ChangeGameConfig().KeyBoardDown = SDLK_DOWN;
		if (!GameConfig().KeyBoardLeft)
			ChangeGameConfig().KeyBoardLeft = SDLK_LEFT;
		if (!GameConfig().KeyBoardRight)
			ChangeGameConfig().KeyBoardRight = SDLK_RIGHT;
		if (!GameConfig().KeyBoardPrimary)
			ChangeGameConfig().KeyBoardPrimary = SDLK_LCTRL;
		if (!GameConfig().KeyBoardSecondary)
			ChangeGameConfig().KeyBoardSecondary = SDLK_SPACE;
		if (!GameConfig().MousePrimary)
			ChangeGameConfig().MousePrimary = SDL_BUTTON_LEFT;
		if (!GameConfig().MouseSecondary)
			ChangeGameConfig().MouseSecondary = SDL_BUTTON_RIGHT;
		if (GameConfig().JoystickPrimary == -1)
			ChangeGameConfig().JoystickPrimary = 0;
		if (GameConfig().JoystickSecondary == -1)
			ChangeGameConfig().JoystickSecondary = 1;
		break;

	default:
		break;
	}
}
void SetMenu(eMenuStatus Menu)
{
	Audio_PlaySound2D(3,1.0f);

	switch (Menu) {
	case eMenuStatus::PROFILE:
		NewProfileName.clear();
		vw_SetCurrentUnicodeChar(nullptr);
		break;

	case eMenuStatus::OPTIONS:
	case eMenuStatus::OPTIONS_ADVANCED:
	case eMenuStatus::CONFCONTROL:
		SetOptionsMenu(Menu);
		break;

	case eMenuStatus::TOP_SCORES:
		// копируем исходные данные
		for (int i = 0; i < 10; i++) {
			strcpy(GameName[i], GameConfig().TopScores[i].Name);
			GameScore[i] = GameConfig().TopScores[i].Score;
		}

		// проверяем профайлы
		for (int j = 0; j < 5; j++) {
			if (GameConfig().Profile[j].Used)
				AddTopScores(GameConfig().Profile[j].Experience, GameConfig().Profile[j].Name, true);
		}
		break;

	case eMenuStatus::MISSION:
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
		break;

	case eMenuStatus::INFORMATION:
		vw_ResetWheelStatus();
		CreateNum = 1;
		CreateInfoObject();
		break;

	case eMenuStatus::CREDITS:
		InitCreditsMenu(vw_GetTimeThread(0));
		break;

	default:
		break;

	}

	NextMenu = Menu;

	// прячем текущее меню
	NeedShowMenu = false;
	NeedHideMenu = true;
	LastMenuUpdateTime = vw_GetTimeThread(0);



}



void SetMenu2(eMenuStatus Menu)
{
	// текущее меню уже стало невидимым, освобождаем память после воркшопа + выключаем голосовые сообщения
	// раньше удалять нельзя - для вывода используем данные из 3д объектов (!)
	switch (MenuStatus) {
	case eMenuStatus::WORKSHOP:
		WorkshopDestroyData();
		VoiceNeedMoreEnergy = 0;
		VoiceAmmoOut = 0;
		break;
	default:
		break;
	}



	PrevMenu = MenuStatus;
	MenuStatus = Menu;

	float Time = vw_GetTimeThread(0);
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
	if (NeedShowMenu) {
		MenuContentTransp = 2.4f*(vw_GetTimeThread(0) - LastMenuUpdateTime);
		if (MenuContentTransp >= 1.0f) {
			MenuContentTransp = 1.0f;
			NeedShowMenu = false;
			LastMenuUpdateTime = vw_GetTimeThread(0);

			// выводим подсказку, если нужно
			if ((MenuStatus == eMenuStatus::PROFILE) &&
			    GameConfig().NeedShowHint[0])
				SetCurrentDialogBox(eDialogBox::ProfileTipsAndTricks);
			if (MenuStatus == eMenuStatus::WORKSHOP) {
				if ((CurrentWorkshop == 1) &&
				    GameConfig().NeedShowHint[1])
					SetCurrentDialogBox(eDialogBox::ShipyardTipsAndTricks);
				if ((CurrentWorkshop == 2) &&
				    GameConfig().NeedShowHint[2])
					SetCurrentDialogBox(eDialogBox::SystemsTipsAndTricks);
				if ((CurrentWorkshop == 3) &&
				    GameConfig().NeedShowHint[3])
					SetCurrentDialogBox(eDialogBox::WeaponryTipsAndTricks);
			}
		}
	}

	// делаем полавное угасание меню
	if (NeedHideMenu) {
		MenuContentTransp = 1.0f - 2.4f*(vw_GetTimeThread(0) - LastMenuUpdateTime);
		if (MenuContentTransp <= 0.0f) {
			MenuContentTransp = 0.0f;
			SetMenu2(NextMenu);
			LastMenuUpdateTime = vw_GetTimeThread(0);
			NeedShowMenu = true;
			NeedHideMenu = false;
		}
	}


	vw_SetCameraMoveAroundPoint(sVECTOR3D(0,0,0), 0.0f, sVECTOR3D(0.0f, 0.0f, 0.0f));
	vw_CameraLookAt();




	// всегда первым рисуем скайбокс и "далекое" окружение
	StarSystemDraw(eDrawType::MENU);




	// рисуем название игры, чтобы звезды и корабли пролетали перед ним
	vw_Start2DMode(-1,1);
	// надпись AstroMenace
	sRECT SrcRect, DstRect;
	SrcRect(0,0,863,128 );
	int StartX = (GameConfig().InternalWidth - 863)/2;
	DstRect(StartX,10,StartX+863,10+128);

	if ((MenuStatus != eMenuStatus::WORKSHOP) &&
	    (MenuStatus != eMenuStatus::INTERFACE) &&
	    (MenuStatus != eMenuStatus::OPTIONS) &&
	    (MenuStatus != eMenuStatus::CONFCONTROL) &&
	    (MenuStatus != eMenuStatus::OPTIONS_ADVANCED)) {
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/astromenace.tga"), true, MenuContentTransp);
	}

	vw_End2DMode();



	// рисуем все 3д объекты
	DrawAllObject3D(eDrawType::MENU);



	// после полной прорисовки делаем обновление данных
	UpdateAllObject3D(vw_GetTimeThread(0));
	vw_UpdateAllParticleSystems(vw_GetTimeThread(0));




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работаем со скриптом, пока он есть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if ((Script != nullptr) &&
	    (!Script->Update(vw_GetTimeThread(0)))) {
		// удаляем скрипт
		delete Script;
		Script = nullptr;
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переходим в 2д режим для прорисовки всех 2д частей
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_Start2DMode(-1,1);

	switch(MenuStatus) {
	case eMenuStatus::MAIN_MENU:
		MainMenu();
		break;
	case eMenuStatus::TOP_SCORES:
		TopScoresMenu();
		break;
	case eMenuStatus::INTERFACE:
		InterfaceMenu(MenuContentTransp, &Button10Transp, &LastButton10UpdateTime);
		break;
	case eMenuStatus::OPTIONS:
		OptionsMenu(MenuContentTransp, &Button10Transp, &LastButton10UpdateTime, &Button11Transp, &LastButton11UpdateTime);
		break;
	case eMenuStatus::OPTIONS_ADVANCED:
		OptionsAdvMenu(MenuContentTransp, &Button10Transp, &LastButton10UpdateTime, &Button11Transp, &LastButton11UpdateTime);
		break;
	case eMenuStatus::INFORMATION:
		InformationMenu();
		break;
	case eMenuStatus::CREDITS:
		CreditsMenu();
		break;
	case eMenuStatus::CONFCONTROL:
		ConfControlMenu(MenuContentTransp, &Button10Transp, &LastButton10UpdateTime);
		break;
	case eMenuStatus::PROFILE:
		ProfileMenu();
		break;
	case eMenuStatus::DIFFICULTY:
		DifficultyMenu();
		break;
	case eMenuStatus::MISSION:
		MissionMenu();
		break;
	case eMenuStatus::WORKSHOP:
		WorkshopMenu();
		break;

	default:
		break;
	}

	vw_End2DMode();



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// последнее - вывод версии и копирайта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	vw_SetInternalResolutionStatus(false);
	vw_Start2DMode(-1,1);

	vw_SetFontSize(10);
	// Version
	int VSize = vw_TextWidth(vw_GetText("Version"));
	vw_DrawText(6, GameConfig().Height - 16, 0, 0, 1.0f, eRGBCOLOR::white, 0.99f, vw_GetText("Version"));
	vw_DrawText(16 + VSize, GameConfig().Height - 16, 0, 0, 1.0f, eRGBCOLOR::white, 0.99f, GAME_VERSION);
	// Copyright
	int CSize = vw_TextWidth("Copyright © 2007-2018, Viewizard");
	vw_DrawText(GameConfig().Width - 6 - CSize, GameConfig().Height - 16, 0, 0, 1.0f, eRGBCOLOR::white, 0.99f,
		    "Copyright © 2007-2018, Viewizard");
	ResetFontSize();

	vw_End2DMode();
	vw_SetInternalResolutionStatus(true);



	// если нужно - рисуем в окошке еще одном
	switch(MenuStatus) {
	case eMenuStatus::INFORMATION:
		InformationDrawObject();
		break;

	default:
		break;
	}




	// черное затемнение, если нужно
	if (NeedOnMenu) {
		MenuBlackTransp = 1.0f - 2.4f*(vw_GetTimeThread(0) - LastMenuOnOffUpdateTime);
		if (MenuBlackTransp <= 0.0f) {
			MenuBlackTransp = 0.0f;
			NeedOnMenu = false;
			LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
		}

		vw_Start2DMode(-1,1);

		SrcRect(0, 0, 2, 2);
		DstRect(0, 0, GameConfig().InternalWidth, 768);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, MenuBlackTransp);

		vw_End2DMode();
	}

	// черное затемнение, выключаем воркшоп (готовимся к переходу на игру)
	if (ComBuffer == eCommand::TURN_OFF_WORKSHOP_MENU) {
		MenuBlackTransp = 2.4f*(vw_GetTimeThread(0) - LastMenuOnOffUpdateTime);
		if (MenuBlackTransp >= 1.0f) {
			MenuBlackTransp = 1.0f;
			LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
			// переход на игру
			WorkshopDestroyData();
			ComBuffer = eCommand::SWITCH_TO_GAME;
		}

		vw_Start2DMode(-1,1);

		SrcRect(0, 0, 2, 2);
		DstRect(0, 0, GameConfig().InternalWidth, 768);
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, MenuBlackTransp);

		vw_End2DMode();
	}

}












//------------------------------------------------------------------------------------
// прорисовка основного меню
//------------------------------------------------------------------------------------
void MainMenu()
{

	int Prir = 100;
	int X = (GameConfig().InternalWidth - 384) / 2;
	int Y = 165;

	if (DrawButton384(X,Y, vw_GetText("START GAME"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime)) {
		// если текущего профиля нет - нужно перейти на выбор профилей, если есть - сразу идем на выбор миссий
		if (CurrentProfile < 0)
			ComBuffer = eCommand::SWITCH_TO_PROFILE;
		else
			ComBuffer = eCommand::SWITCH_TO_MISSION;
	}





	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("TOP SCORES"), MenuContentTransp, &Button2Transp, &LastButton2UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_TOP_SCORES;

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("OPTIONS"), MenuContentTransp, &Button3Transp, &LastButton3UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_OPTIONS;

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("INFORMATION"), MenuContentTransp, &Button4Transp, &LastButton4UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_INFORMATION;

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("CREDITS"), MenuContentTransp, &Button5Transp, &LastButton5UpdateTime)) {
		ComBuffer = eCommand::SWITCH_TO_CREDITS;
		StartMusicWithFade(eMusicTheme::CREDITS, 2000, 2000);
	}

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("QUIT"), MenuContentTransp, &Button6Transp, &LastButton6UpdateTime))
		SetCurrentDialogBox(eDialogBox::QuitFromGame);

}




