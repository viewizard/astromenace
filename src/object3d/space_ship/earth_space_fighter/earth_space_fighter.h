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

#ifndef OBJECT3D_SPACESHIP_EARTHSPACEFIGHTER_EARTHSPACEFIGHTER_H
#define OBJECT3D_SPACESHIP_EARTHSPACEFIGHTER_EARTHSPACEFIGHTER_H

#include "../space_ship.h"

namespace viewizard {

/*
 * All earth fighters.
 */
class cEarthSpaceFighter : public cSpaceShip
{
public:
	explicit cEarthSpaceFighter(int SpaceShipNum);
};

// Установка системы двигателей
void SetEarthSpaceFighterEngine(cEarthSpaceFighter *SpaceShip, int EngineType);
// Установка брони для кораблей землян
void SetEarthSpaceFighterArmour(cEarthSpaceFighter *SpaceShip, int ArmourType);
// Установка оружия на корабль
bool SetEarthSpaceFighterWeapon(cEarthSpaceFighter *SpaceShip, int WeaponSlot, int WeaponNum);
// Получаем возможный поворот орудия в данном слоте
void GetShipWeaponSlotAngle(int ShipNum, int SlotNum, float *Min, float *Max);

} // viewizard namespace

#endif // OBJECT3D_SPACESHIP_EARTHSPACEFIGHTER_EARTHSPACEFIGHTER_H
