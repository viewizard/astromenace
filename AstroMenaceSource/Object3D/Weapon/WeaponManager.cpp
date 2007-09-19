/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: WeaponManager.cpp

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
#include "Weapon.h"



// Указатели на начальный и конечный объект в списке
CWeapon *StartWeapon = 0;
CWeapon *EndWeapon = 0;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachWeapon(CWeapon* Weapon)
{
	if (Weapon == 0) return;

	// первый в списке...
	if (EndWeapon == 0)
	{
		Weapon->Prev = 0;
		Weapon->Next = 0;
		StartWeapon = Weapon;
		EndWeapon = Weapon;
	}
	else // продолжаем заполнение...
	{
		Weapon->Prev = EndWeapon;
		Weapon->Next = 0;
		EndWeapon->Next = Weapon;
		EndWeapon = Weapon;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachWeapon(CWeapon* Weapon)
{
	if (Weapon == 0) return;

	// переустанавливаем указатели...
	if (StartWeapon == Weapon) StartWeapon = Weapon->Next;
	if (EndWeapon == Weapon) EndWeapon = Weapon->Prev;


	if (Weapon->Next != 0) Weapon->Next->Prev = Weapon->Prev;
		else if (Weapon->Prev != 0) Weapon->Prev->Next = 0;
	if (Weapon->Prev != 0) Weapon->Prev->Next = Weapon->Next;
		else if (Weapon->Next != 0) Weapon->Next->Prev = 0;
}







//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllWeapon(float Time)
{
	CWeapon *tmp = StartWeapon;
	while (tmp!=0)
	{
		CWeapon *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
		{
			// если его нужно уничтожить - делаем это
			delete tmp; tmp = 0;
		}
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllWeapon()
{

	CWeapon *tmp = StartWeapon;
	while (tmp!=0)
	{
		CWeapon *tmp2 = tmp->Next;
		tmp->Draw();
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllWeapon()
{
	CWeapon *tmp = StartWeapon;
	while (tmp!=0)
	{
		CWeapon *tmp2 = tmp->Next;
		delete tmp; tmp = 0;
		tmp = tmp2;
	}
}

