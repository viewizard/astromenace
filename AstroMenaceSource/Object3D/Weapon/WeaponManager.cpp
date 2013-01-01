/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


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
void DrawAllWeapon(bool VertexOnlyPass, unsigned int ShadowMap)
{

	CWeapon *tmp = StartWeapon;
	while (tmp!=0)
	{
		CWeapon *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
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

