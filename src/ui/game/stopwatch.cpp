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

#include "stopwatch.h"
#include "../../ui/game_speed.h"
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Reset stopwatch.
 */
void cStopwatch::Reset(bool Enable)
{
	Enabled_ = Enable;
	if (!Enabled_)
		return;

	LastTick_ = SDL_GetTicks();
	InitialTime_ = vw_GetTimeThread(GameTimeThread);
	DrawString_.clear();
}

/*
 * Draw stopwatch.
 * Note, caller should setup 2D mode rendering first.
 */
void cStopwatch::Draw()
{
	if (!Enabled_)
		return;

	vw_DrawText(6,45, 0, 0, 1.0f, TextColor_, 1.0f, DrawString_.c_str());
}

/*
 * Update stopwatch.
 */
void cStopwatch::Update()
{
	if (!Enabled_)
		return;

	uint32_t CurrentTick = SDL_GetTicks();
	constexpr uint32_t UpdateInterval{100}; // 100 milliseconds (ticks), connected to SDL_GetTicks()

	if (CurrentTick < (LastTick_ + UpdateInterval))
		return;

	LastTick_ = CurrentTick;

	// ostringstream is not so fast, but we use it one time per 100 milliseconds
	std::ostringstream tmpStream;
	tmpStream << "mission time: " << std::fixed << std::setprecision(1)
		  << vw_GetTimeThread(GameTimeThread) - InitialTime_;
	DrawString_ = tmpStream.str();
}

} // astromenace namespace
} // viewizard namespace
