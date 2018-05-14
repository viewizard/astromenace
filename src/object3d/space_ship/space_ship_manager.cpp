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

#include "space_ship.h"



// Указатели на начальный и конечный объект в списке
cSpaceShip *StartSpaceShip = nullptr;
cSpaceShip *EndSpaceShip = nullptr;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachSpaceShip(cSpaceShip* SpaceShip)
{
	if (SpaceShip == nullptr)
		return;

	// первый в списке...
	if (EndSpaceShip == nullptr) {
		SpaceShip->Prev = nullptr;
		SpaceShip->Next = nullptr;
		StartSpaceShip = SpaceShip;
		EndSpaceShip = SpaceShip;
	} else { // продолжаем заполнение...
		SpaceShip->Prev = EndSpaceShip;
		SpaceShip->Next = nullptr;
		EndSpaceShip->Next = SpaceShip;
		EndSpaceShip = SpaceShip;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachSpaceShip(cSpaceShip* SpaceShip)
{
	if (SpaceShip == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartSpaceShip == SpaceShip)
		StartSpaceShip = SpaceShip->Next;
	if (EndSpaceShip == SpaceShip)
		EndSpaceShip = SpaceShip->Prev;

	if (SpaceShip->Next != nullptr)
		SpaceShip->Next->Prev = SpaceShip->Prev;
	else if (SpaceShip->Prev != nullptr)
		SpaceShip->Prev->Next = nullptr;

	if (SpaceShip->Prev != nullptr)
		SpaceShip->Prev->Next = SpaceShip->Next;
	else if (SpaceShip->Next != nullptr)
		SpaceShip->Next->Prev = nullptr;
}







//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceShip(float Time)
{
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp != nullptr) {
		cSpaceShip *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			// если его нужно уничтожить - делаем это
			delete tmp;
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllSpaceShip(bool VertexOnlyPass, unsigned int ShadowMap)
{
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp != nullptr) {
		cSpaceShip *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}
}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceShip()
{
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp != nullptr) {
		cSpaceShip *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartSpaceShip = nullptr;
	EndSpaceShip = nullptr;
}
