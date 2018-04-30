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
#include "../script_engine/script.h"
#include "../object3d/object3d.h"
#include "../gfx/star_system.h"


//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------

std::weak_ptr<cParticleSystem> psSpace{};

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
			      (int)((512.0f + 256.0f)/(Setup.InternalWidth / tmpViewportWidth)),
			      (int)(384.0f / (Setup.InternalHeight / tmpViewportHeight)));



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
	vw_ResizeScene(45.0f, Setup.InternalWidth / Setup.InternalHeight, 1.0f, 2000.0f);
	vw_SetCameraLocation(sVECTOR3D(-50,30,-50));
	vw_SetCameraMoveAroundPoint(sVECTOR3D(0,0,0), 0.0f, sVECTOR3D(0.0f, 0.0f, 0.0f));

	// since we use scripts with background animation in the same way as
	// games levels do, we should reset game camera first
	ResetGameCamera();

	if (Script != nullptr) {
		delete Script;
		Script = nullptr;
	}
	Script = new cScriptEngine;

	if (Setup.MenuScript > 2) Setup.MenuScript = 0;
	switch (Setup.MenuScript) {
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
	Setup.MenuScript ++;

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



	// активные частицы космоса
	psSpace = vw_CreateParticleSystem();
	if (auto sharedSpace = psSpace.lock()) {
		sharedSpace->ColorStart.r = 0.80f;
		sharedSpace->ColorStart.g = 0.80f;
		sharedSpace->ColorStart.b = 1.00f;
		sharedSpace->ColorEnd.r = 0.70f;
		sharedSpace->ColorEnd.g = 0.70f;
		sharedSpace->ColorEnd.b = 1.00f;
		sharedSpace->AlphaStart = 1.00f;
		sharedSpace->AlphaEnd   = 0.00f;
		sharedSpace->SizeStart = 0.10f;
		sharedSpace->SizeVar = 0.05f;
		sharedSpace->SizeEnd = 0.30f;
		sharedSpace->Speed = 4.00f;
		sharedSpace->SpeedVar = 8.00f;
		sharedSpace->Theta = 0.00f;
		sharedSpace->Life = 10.00f;
		sharedSpace->LifeVar = 0.00f;
		sharedSpace->CreationType = eParticleCreationType::Cube;
		sharedSpace->CreationSize = sVECTOR3D(2.0f, 50.0f, 30.0f);
		sharedSpace->ParticlesPerSec = 140;
		sharedSpace->Texture = vw_FindTextureByName("gfx/flare3.tga");
		sharedSpace->Direction = sVECTOR3D(1.0f, 0.0f, 0.0f);
		sharedSpace->CameraDistResize = 0.1f;
		sharedSpace->SetStartLocation(sVECTOR3D(-50, 10, -20));

		// немного "прокручиваем", чтобы сразу по появлению было заполнено
		float Time = sharedSpace->TimeLastUpdate;
		for (float i = Time; i < (Time + 20); i += 1.0f) {
			sharedSpace->Update(i);
		}
		sharedSpace->TimeLastUpdate = Time;
	}


	LastMenuUpdateTime = vw_GetTimeThread(0);
	NeedShowMenu = true;
	NeedHideMenu = false;
	LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
	MenuBlackTransp = 1.0f;
	NeedOnMenu = true;
	// подстраховка, если не укажем меню, перейдем в основное
	MenuStatus = eMenuStatus::MAIN_MENU;

	DrawGameCursor = true;

	StarsTileUpdateTime = vw_GetTimeThread(0);
	StarsTileUpdateTime2 = vw_GetTimeThread(0);
}









//------------------------------------------------------------------------------------
// установка меню
//------------------------------------------------------------------------------------
void SetOptionsMenu(eMenuStatus Menu)
{
	switch (Menu) {
	case eMenuStatus::OPTIONS:
		Options_Width = Setup.Width;
		Options_Height = Setup.Height;
		Options_BPP = Setup.BPP;
		Options_VSync = Setup.VSync;
		Options_iAspectRatioWidth = Setup.InternalWidth;
		break;

	case eMenuStatus::OPTIONS_ADVANCED:
		Options_TexturesCompressionType = Setup.TexturesCompressionType;
		Options_UseGLSL120 = Setup.UseGLSL120;
		Options_MSAA = Setup.MSAA;
		Options_CSAA = Setup.CSAA;
		Options_TexturesAnisotropyLevel = Setup.AnisotropyLevel;
		Options_TexturesQuality = Setup.TexturesQuality;
		Options_ShadowMap = Setup.ShadowMap;
		break;

	case eMenuStatus::CONFCONTROL:
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
		for (int i=0; i<10; i++) {
			strcpy(GameName[i], Setup.TopScores[i].Name);
			GameScore[i] = Setup.TopScores[i].Score;
		}

		// проверяем профайлы
		for (int j=0; j<5; j++)
			if (Setup.Profile[j].Used) {
				AddTopScores(Setup.Profile[j].Experience, Setup.Profile[j].Name, true);
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
			if (MenuStatus == eMenuStatus::PROFILE)
				if (Setup.NeedShowHint[0]) SetCurrentDialogBox(eDialogBox::ProfileTipsAndTricks);
			if (MenuStatus == eMenuStatus::WORKSHOP) {
				if (CurrentWorkshop == 1)
					if (Setup.NeedShowHint[1]) SetCurrentDialogBox(eDialogBox::ShipyardTipsAndTricks);
				if (CurrentWorkshop == 2)
					if (Setup.NeedShowHint[2]) SetCurrentDialogBox(eDialogBox::SystemsTipsAndTricks);
				if (CurrentWorkshop == 3)
					if (Setup.NeedShowHint[3]) SetCurrentDialogBox(eDialogBox::WeaponryTipsAndTricks);
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
	StarSystemDraw(1);




	// рисуем название игры, чтобы звезды и корабли пролетали перед ним
	vw_Start2DMode(-1,1);
	// надпись AstroMenace
	sRECT SrcRect, DstRect;
	SrcRect(0,0,863,128 );
	int StartX = (Setup.InternalWidth - 863)/2;
	DstRect(StartX,10,StartX+863,10+128);

	if ((MenuStatus != eMenuStatus::WORKSHOP) &&
	    (MenuStatus != eMenuStatus::INFORMATION) &&
	    (MenuStatus != eMenuStatus::INTERFACE) &&
	    (MenuStatus != eMenuStatus::OPTIONS) &&
	    (MenuStatus != eMenuStatus::CONFCONTROL) &&
	    (MenuStatus != eMenuStatus::OPTIONS_ADVANCED)) {
		vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/astromenace.tga"), true, MenuContentTransp);
	}

	vw_End2DMode();



	// рисуем все 3д объекты
	DrawAllObject3D(1);



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




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// последнее - вывод версии и копирайта
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// Version
	vw_DrawFont(6, 740, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s %s %s %i", vw_GetText("11_Version"), GAME_VERSION, vw_GetText("11_build"), GAME_BUILD);

	// Copyright
	int CSize = vw_FontSize("%s © 2007-2018, Viewizard", vw_GetText("11_Copyright"));
	vw_DrawFont(Setup.InternalWidth-7-CSize, 740, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.99f, "%s © 2007-2018, Viewizard", vw_GetText("11_Copyright"));




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_End2DMode();


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

		SrcRect(0,0,2,2);
		DstRect(0,0,Setup.InternalWidth,768);
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

		SrcRect(0,0,2,2);
		DstRect(0,0,Setup.InternalWidth,768);
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
	int X = (Setup.InternalWidth - 384)/2;
	int Y = 165;

	if (DrawButton384(X,Y, vw_GetText("1_START_GAME"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime)) {
		// если текущего профиля нет - нужно перейти на выбор профилей, если есть - сразу идем на выбор миссий
		if (CurrentProfile < 0)
			ComBuffer = eCommand::SWITCH_TO_PROFILE;
		else
			ComBuffer = eCommand::SWITCH_TO_MISSION;
	}





	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("1_TOP_SCORES"), MenuContentTransp, &Button2Transp, &LastButton2UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_TOP_SCORES;

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("1_OPTIONS"), MenuContentTransp, &Button3Transp, &LastButton3UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_OPTIONS;

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("1_INFORMATION"), MenuContentTransp, &Button4Transp, &LastButton4UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_INFORMATION;

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("1_CREDITS"), MenuContentTransp, &Button5Transp, &LastButton5UpdateTime)) {
		ComBuffer = eCommand::SWITCH_TO_CREDITS;
		StartMusicWithFade(eMusicTheme::CREDITS, 2000, 2000);
	}

	Y = Y+Prir;
	if (DrawButton384(X,Y, vw_GetText("1_QUIT"), MenuContentTransp, &Button6Transp, &LastButton6UpdateTime))
		SetCurrentDialogBox(eDialogBox::QuitFromGame);

}




