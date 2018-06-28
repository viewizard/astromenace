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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "core/core.h"
#include "enum.h"
#include "config/config.h"
#include "platform/platform.h"
#include "object3d/object3d.h"
#include "ui/cursor.h"
#include "gfx/star_system.h"
#include "assets/audio.h"
#include "assets/texture.h"
#include "game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// командный буфер
eCommand ComBuffer{eCommand::DO_NOTHING};

// для просчета фпс
float LastSecond;
#define UPDATE_SPEED_MS 1.0f
float eFPS = 0;
unsigned int eCurrentFrames = 0;

extern float CurrentGameSpeedShowTime;


//------------------------------------------------------------------------------------
// зацикленная процедура работы программы...
//------------------------------------------------------------------------------------
void Loop_Proc()
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работа с курсором
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CursorUpdate();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// начало прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// рисуем то, что нужно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(MenuStatus) {
	case eMenuStatus::MAIN_MENU:
	case eMenuStatus::TOP_SCORES:
	case eMenuStatus::INTERFACE:
	case eMenuStatus::OPTIONS:
	case eMenuStatus::INFORMATION:
	case eMenuStatus::CREDITS:
	case eMenuStatus::CONFCONTROL:
	case eMenuStatus::OPTIONS_ADVANCED:
	case eMenuStatus::PROFILE:
	case eMenuStatus::DIFFICULTY:
	case eMenuStatus::MISSION:
	case eMenuStatus::WORKSHOP:
		DrawMenu();
		break;

	case eMenuStatus::GAME:
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
	CursorDraw();





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// вывод вспомогательной информации
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (GameConfig().ShowFPS) {
		// фпс
		if (GameConfig().VSync == 0)
			vw_DrawText(6,5, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.99f, "%s %.1f", vw_GetText("fps"), eFPS);
		else
			vw_DrawText(6,5, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.99f, "%s %.1f (VSync - %s)", vw_GetText("fps"), eFPS, vw_GetText("On"));
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_End2DMode();
	vw_EndRendering();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выход по нажатию на Esc
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (vw_GetKeyStatus(SDLK_ESCAPE)) {
		SetCurrentDialogBox(eDialogBox::QuitFromGame);
		vw_SetKeyStatus(SDLK_ESCAPE, false);
	}


	auto PrepareToSwitchStatus = [] () {
		SaveXMLConfigFile();
		ReleaseAllObject3D();
		vw_ReleaseAllParticleSystems();
		vw_ReleaseAllLights();
		StarSystemRelease();
		vw_StopAllSoundsIfAllowed();
	};

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выполняем то, что есть в буфере команд, если там что-то есть
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ComBuffer != eCommand::DO_NOTHING) {

		switch(ComBuffer) {
		case eCommand::SWITCH_TO_MAIN_MENU:
			SetMenu(eMenuStatus::MAIN_MENU);
			break;
		case eCommand::SWITCH_TO_TOP_SCORES:
			SetMenu(eMenuStatus::TOP_SCORES);
			break;
		case eCommand::SWITCH_TO_INTERFACE:
			SetMenu(eMenuStatus::INTERFACE);
			break;
		case eCommand::SWITCH_TO_OPTIONS:
			SetMenu(eMenuStatus::OPTIONS);
			break;
		case eCommand::SWITCH_TO_OPTIONS_ADVANCED:
			SetMenu(eMenuStatus::OPTIONS_ADVANCED);
			break;
		case eCommand::SWITCH_TO_INFORMATION:
			SetMenu(eMenuStatus::INFORMATION);
			break;
		case eCommand::SWITCH_TO_CREDITS:
			SetMenu(eMenuStatus::CREDITS);
			break;
		case eCommand::SWITCH_TO_CONFCONTROL:
			SetMenu(eMenuStatus::CONFCONTROL);
			break;
		case eCommand::SWITCH_TO_PROFILE:
			SetMenu(eMenuStatus::PROFILE);
			break;
		case eCommand::SWITCH_TO_DIFFICULTY:
			SetMenu(eMenuStatus::DIFFICULTY);
			break;
		case eCommand::SWITCH_TO_MISSION:
			SetMenu(eMenuStatus::MISSION);
			break;
		case eCommand::SWITCH_TO_WORKSHOP:
			SetMenu(eMenuStatus::WORKSHOP);
			break;
		case eCommand::SWITCH_FROM_MENU_TO_GAME: // also used for mission restart
			PrepareToSwitchStatus();
			InitGame();
			PlayMusicTheme(eMusicTheme::GAME, 2000, 2000);
			PlayVoicePhrase(eVoicePhrase::PrepareForAction, 1.0f);
			break;
		case eCommand::SWITCH_FROM_GAME_TO_MISSION_MENU:
			PrepareToSwitchStatus();
			InitMenu(eMenuStatus::MISSION);
			PlayMusicTheme(eMusicTheme::MENU, 2000, 2000);
			// FIXME code duplication, see SetMenu()
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
		case eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU:
			PrepareToSwitchStatus();
			InitMenu(eMenuStatus::MAIN_MENU);
			PlayMusicTheme(eMusicTheme::MENU, 2000, 2000);
			break;
		case eCommand::SWITCH_FROM_GAME_TO_CREDITS:
			PrepareToSwitchStatus();
			InitMenu(eMenuStatus::CREDITS);
			PlayMusicTheme(eMusicTheme::CREDITS, 2000, 2000);
			// FIXME code duplication, see SetMenu()
			InitCreditsMenu(vw_GetTimeThread(0));
			break;

		default:
			std::cerr << __func__ << "(): " << "ComBuffer = " << (int)ComBuffer << " ... error!\n";
			break;

		}

		ComBuffer = eCommand::DO_NOTHING;
	}




	// самым последним - просчет фпс
	float FPSTime = vw_GetTimeThread(0);
	if (FPSTime - LastSecond > UPDATE_SPEED_MS) {
		eFPS = eCurrentFrames * (FPSTime - LastSecond);
		eCurrentFrames = 0;
		LastSecond = FPSTime;
	} else if (FPSTime - LastSecond < 0) // game was restarted, vw_GetTimeThread() re-initialized
		LastSecond = FPSTime;
	++eCurrentFrames;




	// после обхода всех активных элементов меню, надо подкорректировать состояние выбора через клавиатуру (если оно было)
	if (vw_GetKeyStatus(SDLK_TAB)) {
		CurrentKeyboardSelectMenuElement++;
		vw_SetKeyStatus(SDLK_TAB, false);
	}

	// если не в игре, используем и кнопки курсора
	if ((MenuStatus != eMenuStatus::GAME) || ((MenuStatus == eMenuStatus::GAME) &&
	    (isDialogBoxDrawing() || (GameContentTransp >= 0.99f)))) {
		if (vw_GetKeyStatus(SDLK_RIGHT) || vw_GetKeyStatus(SDLK_DOWN)) {
			CurrentKeyboardSelectMenuElement++;
			vw_SetKeyStatus(SDLK_RIGHT, false);
			vw_SetKeyStatus(SDLK_DOWN, false);
		}
		if (vw_GetKeyStatus(SDLK_LEFT) || vw_GetKeyStatus(SDLK_UP)) {
			CurrentKeyboardSelectMenuElement--;
			vw_SetKeyStatus(SDLK_LEFT, false);
			vw_SetKeyStatus(SDLK_UP, false);
			if (CurrentKeyboardSelectMenuElement < 1)
				CurrentKeyboardSelectMenuElement = CurrentActiveMenuElement;
		}
	}

	if (CurrentKeyboardSelectMenuElement > 0) {
		// если у нас вообще есть активные элементы, ставим на первый
		if (CurrentActiveMenuElement > 0) {
			if (CurrentKeyboardSelectMenuElement > CurrentActiveMenuElement)
				CurrentKeyboardSelectMenuElement = 1;
		} else CurrentKeyboardSelectMenuElement = 0;
	}
	CurrentActiveMenuElement = 0;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// дополнительные функции, недокументированные возможности
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// выключение fps и примитивов
	if (vw_GetKeyStatus(SDLK_F2)) {
		ChangeGameConfig().ShowFPS = !GameConfig().ShowFPS;
		vw_SetKeyStatus(SDLK_F2, false);
	}

	// делаем на рабочем столе бмп скриншоты
	if (vw_GetKeyStatus(SDLK_PRINTSCREEN) || vw_GetKeyStatus(SDLK_F12)) {
		std::time_t RawTime = std::time(nullptr);
		std::array<char, 128> tmpBuffer;
		std::strftime(tmpBuffer.data(), tmpBuffer.size(), "AstroMenaceScreenshot%Y-%m-%d_%H-%M-%S.bmp", std::localtime(&RawTime));

		vw_Screenshot(GameConfig().Width, GameConfig().Height, GetDesktopPath() + std::string{tmpBuffer.data()});
		vw_SetKeyStatus(SDLK_PRINTSCREEN, false);
		vw_SetKeyStatus(SDLK_F12, false);
	}

	// управление скоростью игры, только в самой игре!
	if ((MenuStatus == eMenuStatus::GAME) &&
	    (GameContentTransp<=0.0f) &&
	    !GameMissionCompleteStatus) {
		if (vw_GetKeyStatus(SDLK_F5)) {
			ChangeGameConfig().GameSpeed -= 0.1f;
			if (GameConfig().GameSpeed < 0.1f)
				ChangeGameConfig().GameSpeed = 0.1f;
			CurrentGameSpeedShowTime = 2.0f;
			vw_SetTimeThreadSpeed(1, GameConfig().GameSpeed);
			vw_SetKeyStatus(SDLK_F5, false);
		}
		if (vw_GetKeyStatus(SDLK_F6)) {
			ChangeGameConfig().GameSpeed = 1.5f;
			vw_SetTimeThreadSpeed(1, GameConfig().GameSpeed);
			CurrentGameSpeedShowTime = 2.0f;
			vw_SetKeyStatus(SDLK_F6, false);
		}
		if (vw_GetKeyStatus(SDLK_F7)) {
			ChangeGameConfig().GameSpeed += 0.1f;
			if (GameConfig().GameSpeed > 3.0f)
				ChangeGameConfig().GameSpeed = 3.0f;
			CurrentGameSpeedShowTime = 2.0f;
			vw_SetTimeThreadSpeed(1, GameConfig().GameSpeed);
			vw_SetKeyStatus(SDLK_F7, false);
		}
	}

	if (MenuStatus == eMenuStatus::GAME) {
		// изменение вывода состояния вооружения
		if (vw_GetKeyStatus(SDLK_F8)) {
			ChangeGameConfig().GameWeaponInfoType++;
			if (GameConfig().GameWeaponInfoType > 4)
				ChangeGameConfig().GameWeaponInfoType = 1;
			vw_SetKeyStatus(SDLK_F8, false);
		}
		// изменение типов стрельбы
		if (vw_GetKeyStatus(SDLK_F9)) {
			ChangeGameConfig().Profile[CurrentProfile].PrimaryWeaponFireMode++;
			if (GameConfig().Profile[CurrentProfile].PrimaryWeaponFireMode > 2)
				ChangeGameConfig().Profile[CurrentProfile].PrimaryWeaponFireMode = 1;
			vw_SetKeyStatus(SDLK_F9, false);
		}
		if (vw_GetKeyStatus(SDLK_F10)) {
			ChangeGameConfig().Profile[CurrentProfile].SecondaryWeaponFireMode++;
			if (GameConfig().Profile[CurrentProfile].SecondaryWeaponFireMode > 2)
				ChangeGameConfig().Profile[CurrentProfile].SecondaryWeaponFireMode = 1;
			vw_SetKeyStatus(SDLK_F10, false);
		}

	}

}

} // astromenace namespace
} // viewizard namespace
