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


/// подключаем нужные файлы
#include "space_object.h"
#include "../../game.h"


// Указатели на начальный и конечный объект в списке
CSpaceObject *StartSpaceObject = nullptr;
CSpaceObject *EndSpaceObject = nullptr;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachSpaceObject(CSpaceObject* SpaceObject)
{
	if (SpaceObject == nullptr)
		return;

	if (EndSpaceObject == nullptr) {
		SpaceObject->Prev = nullptr;
		SpaceObject->Next = nullptr;
		StartSpaceObject = SpaceObject;
		EndSpaceObject = SpaceObject;
	} else {
		SpaceObject->Prev = EndSpaceObject;
		SpaceObject->Next = nullptr;
		EndSpaceObject->Next = SpaceObject;
		EndSpaceObject = SpaceObject;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachSpaceObject(CSpaceObject* SpaceObject)
{
	if (SpaceObject == nullptr)
		return;

	if (StartSpaceObject == SpaceObject)
		StartSpaceObject = SpaceObject->Next;
	if (EndSpaceObject == SpaceObject)
		EndSpaceObject = SpaceObject->Prev;


	if (SpaceObject->Next != nullptr)
		SpaceObject->Next->Prev = SpaceObject->Prev;
	else if (SpaceObject->Prev != nullptr)
		SpaceObject->Prev->Next = nullptr;

	if (SpaceObject->Prev != nullptr)
		SpaceObject->Prev->Next = SpaceObject->Next;
	else if (SpaceObject->Next != nullptr)
		SpaceObject->Next->Prev = nullptr;
}





//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceObject(float Time)
{
	CSpaceObject *tmp = StartSpaceObject;
	while (tmp != nullptr) {
		CSpaceObject *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllSpaceObject(bool VertexOnlyPass, unsigned int ShadowMap)
{

	CSpaceObject *tmp = StartSpaceObject;
	while (tmp != nullptr) {
		CSpaceObject *tmp2 = tmp->Next;

		// планеты и астероиды рисуем до тайловой анимации в игре!!!
		if ((tmp->ObjectType != 14) &&
		    !((tmp->ObjectType == 15) &&
		      (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
			tmp->Draw(VertexOnlyPass, ShadowMap);

		tmp = tmp2;
	}
}
int DrawAllSpaceObjectCount(int DrawOnlyType)
{
	int Count = 0;

	CSpaceObject *tmp = StartSpaceObject;
	while (tmp != nullptr) {
		CSpaceObject *tmp2 = tmp->Next;

		if (DrawOnlyType != -1) {
			// если нужно прорисовать только определенный тип
			if ((tmp->ObjectType == DrawOnlyType) &&
			    (tmp->ObjectType != 14 &&
			     !(tmp->ObjectType == 15 && (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20))))
				Count++;
		} else {
			// планеты и астероиды рисуем до тайловой анимации в игре!!!
			if (tmp->ObjectType != 14 &&
			    !(tmp->ObjectType == 15 &&
			      (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
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
	while (tmp != nullptr) {
		CSpaceObject *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartSpaceObject = nullptr;
	EndSpaceObject = nullptr;
}
