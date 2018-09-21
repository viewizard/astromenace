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

#include "fps_counter.h"
#include "../config/config.h"
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Clear DrawString_, if appropriate FPS not calculated yet.
 */
void cFPS::ClearDrawString()
{
	DrawString_ = vw_GetText("fps") + std::string{" ..."};
}

/*
 * Reset fps counter.
 */
void cFPS::Reset()
{
	LastTick_ = SDL_GetTicks();
	CurrentFrame_ = 0;
	ClearDrawString();
}

/*
 * Switch fps counter show/hide status.
 */
void cFPS::Switch()
{
	ChangeGameConfig().ShowFPS = !GameConfig().ShowFPS;

	if (GameConfig().ShowFPS)
		Reset();
}

/*
 * Check keyboard.
 */
void cFPS::CheckKeyboard()
{
	if (!vw_GetKeyStatus(SDLK_F2))
		return;

	vw_SetKeyStatus(SDLK_F2, false);
	Switch();
}

/*
 * Draw fps counter.
 * Note, caller should setup 2D mode rendering first.
 */
void cFPS::Draw()
{
	if (!GameConfig().ShowFPS)
		return;

	vw_DrawText(6, 5, 0, 0, 1.0f, TextColor_, 1.0f, DrawString_.c_str());
}

/*
 * Update fps counter.
 */
void cFPS::Update()
{
	CheckKeyboard();

	if (!GameConfig().ShowFPS)
		return;

	uint32_t CurrentTick = SDL_GetTicks();
	if (LastTick_ < CurrentTick) {
		uint32_t TicksDelta = CurrentTick - LastTick_;
		constexpr uint32_t TicksInSecond{1000}; // connected to SDL_GetTicks()

		if (TicksDelta >= TicksInSecond) {
			// ostringstream is not so fast, but we use it one time per second
			std::ostringstream tmpStream;
			tmpStream << vw_GetText("fps") << " "
				  << std::fixed << std::setprecision(1)
				  << static_cast<float>(CurrentFrame_ * TicksInSecond) / TicksDelta;
			if (GameConfig().VSync)
				tmpStream << " (VSync - " << vw_GetText("On") << ")";

			DrawString_ = tmpStream.str();
			CurrentFrame_ = 0;
			LastTick_ = CurrentTick;
		}
	} else if (LastTick_ > CurrentTick) // game was restarted, or SDL was re-inited
		Reset();
	// if LastTick_ == CurrentTick - do nothing

	CurrentFrame_++;
}

} // astromenace namespace
} // viewizard namespace
