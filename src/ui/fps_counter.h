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

// TODO move to SDL_GetTicks() usage

#ifndef UI_FPSCOUNTER_H
#define UI_FPSCOUNTER_H

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

class cFPS {
private:
	cFPS() = default;
	~cFPS() = default;

	float LastTime{0.0f};
	float CurrentFPS{0.0f};
	unsigned int CurrentFrame{0};
	sRGBCOLOR TextColor{eRGBCOLOR::white};

public:
	cFPS(cFPS const&) = delete;
	void operator = (cFPS const&) = delete;

	static cFPS &GetInstance()
	{
		static cFPS FPSCounter;
		return FPSCounter;
	}

	void Switch(float CurrentTime);
	void CheckKeyboard(float CurrentTime);
	void Draw();
	void Update(float CurrentTime);
};

} // astromenace namespace
} // viewizard namespace

#endif // UI_FPSCOUNTER_H
