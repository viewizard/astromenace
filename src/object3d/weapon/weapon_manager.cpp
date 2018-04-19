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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "weapon.h"



// Указатели на начальный и конечный объект в списке
cWeapon *StartWeapon = nullptr;
cWeapon *EndWeapon = nullptr;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachWeapon(cWeapon *Weapon)
{
	if (Weapon == nullptr)
		return;

	// первый в списке...
	if (EndWeapon == nullptr) {
		Weapon->Prev = nullptr;
		Weapon->Next = nullptr;
		StartWeapon = Weapon;
		EndWeapon = Weapon;
	} else { // продолжаем заполнение...
		Weapon->Prev = EndWeapon;
		Weapon->Next = nullptr;
		EndWeapon->Next = Weapon;
		EndWeapon = Weapon;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachWeapon(cWeapon *Weapon)
{
	if (Weapon == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartWeapon == Weapon) StartWeapon = Weapon->Next;
	if (EndWeapon == Weapon) EndWeapon = Weapon->Prev;


	if (Weapon->Next != nullptr)
		Weapon->Next->Prev = Weapon->Prev;
	else if (Weapon->Prev != nullptr)
		Weapon->Prev->Next = nullptr;
	if (Weapon->Prev != nullptr)
		Weapon->Prev->Next = Weapon->Next;
	else if (Weapon->Next != nullptr)
		Weapon->Next->Prev = nullptr;
}







//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllWeapon(float Time)
{
	cWeapon *tmp = StartWeapon;
	while (tmp != nullptr) {
		cWeapon *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllWeapon(bool VertexOnlyPass, unsigned int ShadowMap)
{

	cWeapon *tmp = StartWeapon;
	while (tmp != nullptr) {
		cWeapon *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllWeapon()
{
	cWeapon *tmp = StartWeapon;
	while (tmp != nullptr) {
		cWeapon *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}
}

