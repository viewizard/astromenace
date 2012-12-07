/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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
#include "SpaceShip.h"



// Указатели на начальный и конечный объект в списке
CSpaceShip *StartSpaceShip = 0;
CSpaceShip *EndSpaceShip = 0;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachSpaceShip(CSpaceShip* SpaceShip)
{
	if (SpaceShip == 0) return;

	// первый в списке...
	if (EndSpaceShip == 0)
	{
		SpaceShip->Prev = 0;
		SpaceShip->Next = 0;
		StartSpaceShip = SpaceShip;
		EndSpaceShip = SpaceShip;
	}
	else // продолжаем заполнение...
	{
		SpaceShip->Prev = EndSpaceShip;
		SpaceShip->Next = 0;
		EndSpaceShip->Next = SpaceShip;
		EndSpaceShip = SpaceShip;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachSpaceShip(CSpaceShip* SpaceShip)
{
	if (SpaceShip == 0) return;

	// переустанавливаем указатели...
	if (StartSpaceShip == SpaceShip) StartSpaceShip = SpaceShip->Next;
	if (EndSpaceShip == SpaceShip) EndSpaceShip = SpaceShip->Prev;


	if (SpaceShip->Next != 0) SpaceShip->Next->Prev = SpaceShip->Prev;
		else if (SpaceShip->Prev != 0) SpaceShip->Prev->Next = 0;
	if (SpaceShip->Prev != 0) SpaceShip->Prev->Next = SpaceShip->Next;
		else if (SpaceShip->Next != 0) SpaceShip->Next->Prev = 0;
}







//-----------------------------------------------------------------------------
// Находим нужный объекты по ID
//-----------------------------------------------------------------------------
CSpaceShip *FindSpaceShipByID(int ID)
{
	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmp2 = tmp->Next;
		if (tmp->ID == ID) return tmp;
		tmp = tmp2;
	}
	return 0;
}






//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceShip(float Time)
{
	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmp2 = tmp->Next;
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
void DrawAllSpaceShip(bool VertexOnlyPass, unsigned int ShadowMap, int DrawOnlyType)
{

	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmp2 = tmp->Next;

		if (DrawOnlyType != -1)
		{
			// если нужно прорисовать только определенный тип
			if (tmp->ObjectType == DrawOnlyType)
				tmp->Draw(VertexOnlyPass, ShadowMap);
		}
		else
			tmp->Draw(VertexOnlyPass, ShadowMap);

		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceShip()
{
	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmp2 = tmp->Next;
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartSpaceShip = 0;
	EndSpaceShip = 0;
}
