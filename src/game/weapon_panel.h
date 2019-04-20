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

#ifndef GAME_WEAPONPANEL_H
#define GAME_WEAPONPANEL_H

#include "../core/base.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

class cSpaceShip;

// do not use specific numbers here, since we use incrementation and decrementation
// with cast to int, see WeaponPanelViewNext() and WeaponPanelViewPrev()
enum eWeaponPanelView : int {
	hide = 0, // should be the first one
	slim,
	flat,
	full // should be the last one
};


// Draw in-game weapon panels (part of HUD).
void DrawWeaponPanels(std::weak_ptr<cSpaceShip> &SpaceShip);
// Find next weapon panel view (cycled).
void WeaponPanelViewNext(eWeaponPanelView &Value);
// Find previous weapon panel view (cycled).
void WeaponPanelViewPrev(eWeaponPanelView &Value);

} // astromenace namespace
} // viewizard namespace

#endif // GAME_WEAPONPANEL_H
