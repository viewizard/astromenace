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

#include "earth_space_fighter.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {


//-----------------------------------------------------------------------------
// Установка оружия на корабль
//-----------------------------------------------------------------------------
bool SetEarthSpaceFighterWeapon(cEarthSpaceFighter *SpaceShip, int WeaponSlot, int WeaponNum)
{

	// проверяем, можно ли вообще ставить в этот слот оружие
	if (static_cast<int>(SpaceShip->WeaponSlots.size()) < WeaponSlot) {
		return false;
	}

	// удаляем оружие, если оно уже тут есть
	if (SpaceShip->WeaponSlots[WeaponSlot-1].Weapon) {
		ReleaseWeapon(SpaceShip->WeaponSlots[WeaponSlot-1].Weapon);
		SpaceShip->WeaponSlots[WeaponSlot-1].Weapon = nullptr;
	}

	// создаем нужное оружие
	SpaceShip->WeaponSlots[WeaponSlot-1].Weapon = CreateWeapon(WeaponNum);

	// проверяем уровень оружия и уровень слота
	if (SpaceShip->WeaponSlots[WeaponSlot-1].Type < SpaceShip->WeaponSlots[WeaponSlot-1].Weapon->WeaponLevel) {
		ReleaseWeapon(SpaceShip->WeaponSlots[WeaponSlot-1].Weapon);
		SpaceShip->WeaponSlots[WeaponSlot-1].Weapon = nullptr;
		return false;
	}

	// если тут, значит все нормально - можно подключать
	SpaceShip->WeaponSlots[WeaponSlot-1].Weapon->SetLocation(SpaceShip->WeaponSlots[WeaponSlot-1].Location);
	// передаем статус корабля, чтобы учесть что это игрока оружие
	SpaceShip->WeaponSlots[WeaponSlot-1].Weapon->ObjectStatus = SpaceShip->ObjectStatus;

	return true;
}

} // astromenace namespace
} // viewizard namespace
