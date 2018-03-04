/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "earth_space_fighter.h"





//-----------------------------------------------------------------------------
// Установка оружия на корабль
//-----------------------------------------------------------------------------
bool SetEarthSpaceFighterWeapon(cEarthSpaceFighter *SpaceShip, int WeaponSlot, int WeaponNum)
{

	// проверяем, можно ли вообще ставить в этот слот оружие
	if (SpaceShip->WeaponQuantity < WeaponSlot) {
		return false;
	}

	// удаляем оружие, если оно уже тут есть
	if (SpaceShip->Weapon[WeaponSlot-1] != nullptr) {
		delete SpaceShip->Weapon[WeaponSlot-1];
		SpaceShip->Weapon[WeaponSlot-1] = nullptr;
	}

	// создаем нужное оружие
	SpaceShip->Weapon[WeaponSlot-1] = new cWeapon;
	if (SpaceShip->Weapon[WeaponSlot-1] == nullptr)
		return false;
	SpaceShip->Weapon[WeaponSlot-1]->Create(WeaponNum);


	// проверяем уровень оружия и уровень слота
	if (SpaceShip->WeaponType[WeaponSlot-1] < SpaceShip->Weapon[WeaponSlot-1]->WeaponLevel) {
		delete SpaceShip->Weapon[WeaponSlot-1];
		SpaceShip->Weapon[WeaponSlot-1] = nullptr;
		return false;
	}

	// если тут, значит все нормально - можно подключать
	SpaceShip->Weapon[WeaponSlot-1]->SetLocation(SpaceShip->WeaponLocation[WeaponSlot-1]);
	// передаем статус корабля, чтобы учесть что это игрока оружие
	SpaceShip->Weapon[WeaponSlot-1]->ObjectStatus = SpaceShip->ObjectStatus;

	return true;
}
