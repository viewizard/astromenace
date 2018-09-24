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

// TODO this is temporary code, should be revised to be a part of new 'menu' window class hierarchy instead of singleton

// TODO change sources, use GameTimeThread instead of 'magic' number "1"

#ifndef UI_GAMESPEED_H
#define UI_GAMESPEED_H

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// note, we use 0 time thread for menu and other stuff
constexpr int GameTimeThread{1};

class cGameSpeed {
private:
	cGameSpeed() = default;
	~cGameSpeed() = default;

	void CheckKeyboard();

	float RemainingDrawTime_{0.0f};
	uint32_t LastUpdateTick_{0};
	std::u32string DrawStringUTF32_{};

public:
	cGameSpeed(cGameSpeed const&) = delete;
	void operator = (cGameSpeed const&) = delete;

	static cGameSpeed &GetInstance()
	{
		static cGameSpeed Instance;
		return Instance;
	}

	void InitGameSpeed();
	void SetThreadSpeed(float Speed);
	void SetSpeed(float Speed);
	void Draw();
	void Update();
};

} // astromenace namespace
} // viewizard namespace

#endif // UI_GAMESPEED_H
