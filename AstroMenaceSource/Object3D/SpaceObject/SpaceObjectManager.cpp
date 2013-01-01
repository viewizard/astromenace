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
#include "SpaceObject.h"
#include "../../Game.h"


// Указатели на начальный и конечный объект в списке
CSpaceObject *StartSpaceObject = 0;
CSpaceObject *EndSpaceObject = 0;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachSpaceObject(CSpaceObject* SpaceObject)
{
	if (SpaceObject == 0) return;

	// первый в списке...
	if (EndSpaceObject == 0)
	{
		SpaceObject->Prev = 0;
		SpaceObject->Next = 0;
		StartSpaceObject = SpaceObject;
		EndSpaceObject = SpaceObject;
	}
	else // продолжаем заполнение...
	{
		SpaceObject->Prev = EndSpaceObject;
		SpaceObject->Next = 0;
		EndSpaceObject->Next = SpaceObject;
		EndSpaceObject = SpaceObject;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachSpaceObject(CSpaceObject* SpaceObject)
{
	if (SpaceObject == 0) return;

	// переустанавливаем указатели...
	if (StartSpaceObject == SpaceObject) StartSpaceObject = SpaceObject->Next;
	if (EndSpaceObject == SpaceObject) EndSpaceObject = SpaceObject->Prev;


	if (SpaceObject->Next != 0) SpaceObject->Next->Prev = SpaceObject->Prev;
		else if (SpaceObject->Prev != 0) SpaceObject->Prev->Next = 0;
	if (SpaceObject->Prev != 0) SpaceObject->Prev->Next = SpaceObject->Next;
		else if (SpaceObject->Next != 0) SpaceObject->Next->Prev = 0;
}





//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceObject(float Time)
{
	CSpaceObject *tmp = StartSpaceObject;
	while (tmp!=0)
	{
		CSpaceObject *tmp2 = tmp->Next;
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
void DrawAllSpaceObject(bool VertexOnlyPass, unsigned int ShadowMap)
{

	CSpaceObject *tmp = StartSpaceObject;
	while (tmp!=0)
	{
		CSpaceObject *tmp2 = tmp->Next;

		// планеты и астероиды рисуем до тайловой анимации в игре!!!
		if (tmp->ObjectType != 14 && !(tmp->ObjectType == 15 && (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
			tmp->Draw(VertexOnlyPass, ShadowMap);

		tmp = tmp2;
	}

}
int DrawAllSpaceObjectCount(int DrawOnlyType)
{
	int Count = 0;

	CSpaceObject *tmp = StartSpaceObject;
	while (tmp!=0)
	{
		CSpaceObject *tmp2 = tmp->Next;

		if (DrawOnlyType != -1)
		{
			// если нужно прорисовать только определенный тип
			if (tmp->ObjectType == DrawOnlyType)
			if (tmp->ObjectType != 14 && !(tmp->ObjectType == 15 && (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
				Count++;
		}
		else
		{
			// планеты и астероиды рисуем до тайловой анимации в игре!!!
			if (tmp->ObjectType != 14 && !(tmp->ObjectType == 15 && (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
				Count++;
		}

		tmp = tmp2;
	}

	return Count;
}




//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceObject()
{
	CSpaceObject *tmp = StartSpaceObject;
	while (tmp!=0)
	{
		CSpaceObject *tmp2 = tmp->Next;
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartSpaceObject = 0;
	EndSpaceObject = 0;
}
