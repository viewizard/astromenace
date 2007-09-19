/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: EarthSpaceFighterWeapon.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


/// подключаем нужные файлы
#include "EarthSpaceFighter.h"





//-----------------------------------------------------------------------------
// Установка оружия на корабль
//-----------------------------------------------------------------------------
bool SetEarthSpaceFighterWeapon(CEarthSpaceFighter *SpaceShip, int WeaponSlot, int WeaponNum)
{

	// проверяем, можно ли вообще ставить в этот слот оружие
	if (SpaceShip->WeaponQuantity < WeaponSlot)
	{
		return false;
	}

	// удаляем оружие, если оно уже тут есть
	if (SpaceShip->Weapon[WeaponSlot-1] != 0){delete SpaceShip->Weapon[WeaponSlot-1]; SpaceShip->Weapon[WeaponSlot-1] = 0;}

	// создаем нужное оружие
	SpaceShip->Weapon[WeaponSlot-1] = new CWeapon;
	if (SpaceShip->Weapon[WeaponSlot-1] == 0) return false;
	SpaceShip->Weapon[WeaponSlot-1]->Create(WeaponNum);


	// проверяем уровень оружия и уровень слота
	if (SpaceShip->WeaponType[WeaponSlot-1] < SpaceShip->Weapon[WeaponSlot-1]->WeaponLevel)
	{
		delete SpaceShip->Weapon[WeaponSlot-1]; SpaceShip->Weapon[WeaponSlot-1] = 0;
		return false;
	}

	// если тут, значит все нормально - можно подключать
	SpaceShip->Weapon[WeaponSlot-1]->SetLocation(SpaceShip->WeaponLocation[WeaponSlot-1]);
	// передаем статус корабля, чтобы учесть что это игрока оружие
	SpaceShip->Weapon[WeaponSlot-1]->ObjectStatus = SpaceShip->ObjectStatus;

	return true;
}
