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

// TODO translate comments

#include "core/core.h"
#include "config/config.h"
#include "platform/platform.h"
#include "ui/cursor.h"
#include "ui/fps_counter.h"
#include "gfx/star_system.h"
#include "command.h"
#include "game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, don't allow global scope interaction for local variables
extern float CurrentGameSpeedShowTime;


/*
 * Main loop.
 */
void Loop_Proc()
{
	CursorUpdate();

	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);

	switch (MenuStatus) {
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

	vw_Start2DMode(-1,1);

	DrawDialogBox();
	CursorDraw();
	cFPS::GetInstance().Draw();

	vw_End2DMode();
	vw_EndRendering();

	if (vw_GetKeyStatus(SDLK_ESCAPE)) {
		SetCurrentDialogBox(eDialogBox::QuitFromGame);
		vw_SetKeyStatus(SDLK_ESCAPE, false);
	}

	ProceedCommand();

	cFPS::GetInstance().Update();

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
		} else
			CurrentKeyboardSelectMenuElement = 0;
	}
	CurrentActiveMenuElement = 0;

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
	    (GameContentTransp <= 0.0f) &&
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
