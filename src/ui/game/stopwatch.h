/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

// TODO this is temporary code, should be revised to be a part of new 'menu' window class hierarchy instead of singleton

#ifndef UI_GAME_STOPWATCH_H
#define UI_GAME_STOPWATCH_H

#include "../../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

class cStopwatch {
private:
	cStopwatch() = default;
	~cStopwatch() = default;

	bool Enabled_{false};
	uint32_t LastTick_{0};
	float InitialTime_{0.0f};
	std::u32string DrawStringUTF32_{};
	sRGBCOLOR TextColor_{eRGBCOLOR::white};

public:
	cStopwatch(cStopwatch const&) = delete;
	void operator = (cStopwatch const&) = delete;

	static cStopwatch &GetInstance()
	{
		static cStopwatch Instance;
		return Instance;
	}

	void Reset(bool Enable);
	void Draw();
	void Update();
};

} // astromenace namespace
} // viewizard namespace

#endif // UI_GAME_STOPWATCH_H
