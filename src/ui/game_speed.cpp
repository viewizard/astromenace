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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "game_speed.h"
#include "font.h"
#include "../config/config.h"
#include "../enum.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

constexpr float DefaultSpeed{1.5f}; // FIXME should be the same as sGameConfig::GameSpeed default value
constexpr float SpeedChangeStep{0.1f};
constexpr float MinSpeed{0.1f};
constexpr float MaxSpeed{3.0f};
constexpr float ShowNotificationSeconds{2.0f};


/*
 * Init/re-init time thread and setup game speed.
 */
void cGameSpeed::InitGameSpeed()
{
	vw_InitTimeThread(GameTimeThread);
	SetSpeed(GameConfig().GameSpeed);
}

/*
 * Set time thread speed.
 */
void cGameSpeed::SetThreadSpeed(float Speed)
{
	vw_SetTimeThreadSpeed(GameTimeThread, Speed);
}

/*
 * Set game speed.
 */
void cGameSpeed::SetSpeed(float Speed)
{
	vw_Clamp(Speed, MinSpeed, MaxSpeed);

	ChangeGameConfig().GameSpeed = Speed;
	SetThreadSpeed(Speed);

	RemainingDrawTime_ = ShowNotificationSeconds;
	LastUpdateTick_ = SDL_GetTicks();
}

/*
 * Check keyboard.
 */
void cGameSpeed::CheckKeyboard()
{
	if ((MenuStatus != eMenuStatus::GAME) ||
	    (GameContentTransp > 0.0f) ||
	    GameMissionCompleteStatus)
		return;

	if (vw_GetKeyStatus(SDLK_F5)) {
		SetSpeed(GameConfig().GameSpeed - SpeedChangeStep);
		vw_SetKeyStatus(SDLK_F5, false);
	}
	if (vw_GetKeyStatus(SDLK_F6)) {
		SetSpeed(DefaultSpeed);
		vw_SetKeyStatus(SDLK_F6, false);
	}
	if (vw_GetKeyStatus(SDLK_F7)) {
		SetSpeed(GameConfig().GameSpeed + SpeedChangeStep);
		vw_SetKeyStatus(SDLK_F7, false);
	}
}

/*
 * Update game speed status.
 */
void cGameSpeed::Update()
{
	CheckKeyboard();

	if (RemainingDrawTime_ <= 0.0f)
		return;

	uint32_t CurrentTick = SDL_GetTicks();
	if (LastUpdateTick_ < CurrentTick) {
		constexpr uint32_t TicksInSecond{1000}; // connected to SDL_GetTicks()
		RemainingDrawTime_ -= static_cast<float>(CurrentTick - LastUpdateTick_) / TicksInSecond;
	}
	LastUpdateTick_ = CurrentTick;
}

/*
 * Draw current game speed.
 * Note, caller should setup 2D mode rendering first.
 */
void cGameSpeed::Draw()
{
	if (RemainingDrawTime_ <= 0.0f)
		return;

	vw_SetFontSize(20);

	int tmpTextSize = GameConfig().InternalWidth -
			  vw_TextWidth("%s x%1.1f", vw_GetText("Game Speed:"), GameConfig().GameSpeed);
	vw_DrawText(tmpTextSize / 2, 80, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white},
		    (RemainingDrawTime_ > 1.0f) ? 1.0f : RemainingDrawTime_,
		    "%s x%1.1f", vw_GetText("Game Speed:"), GameConfig().GameSpeed);

	ResetFontSize();
}

} // astromenace namespace
} // viewizard namespace
