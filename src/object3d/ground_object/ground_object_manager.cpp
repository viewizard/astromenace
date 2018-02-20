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
#include "ground_object.h"



// Указатели на начальный и конечный объект в списке
CGroundObject *StartGroundObject = nullptr;
CGroundObject *EndGroundObject = nullptr;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachGroundObject(CGroundObject* GroundObject)
{
	if (GroundObject == nullptr)
		return;

	// первый в списке...
	if (EndGroundObject == nullptr) {
		GroundObject->Prev = nullptr;
		GroundObject->Next = nullptr;
		StartGroundObject = GroundObject;
		EndGroundObject = GroundObject;
	} else { // продолжаем заполнение...
		GroundObject->Prev = EndGroundObject;
		GroundObject->Next = nullptr;
		EndGroundObject->Next = GroundObject;
		EndGroundObject = GroundObject;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachGroundObject(CGroundObject* GroundObject)
{
	if (GroundObject == nullptr) return;

	// переустанавливаем указатели...
	if (StartGroundObject == GroundObject)
		StartGroundObject = GroundObject->Next;
	if (EndGroundObject == GroundObject)
		EndGroundObject = GroundObject->Prev;

	if (GroundObject->Next != nullptr)
		GroundObject->Next->Prev = GroundObject->Prev;
	else if (GroundObject->Prev != nullptr)
		GroundObject->Prev->Next = nullptr;

	if (GroundObject->Prev != nullptr)
		GroundObject->Prev->Next = GroundObject->Next;
	else if (GroundObject->Next != nullptr)
		GroundObject->Next->Prev = nullptr;
}





//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllGroundObject(float Time)
{
	CGroundObject *tmp = StartGroundObject;
	while (tmp != nullptr) {
		CGroundObject *tmp2 = tmp->Next;
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
void DrawAllGroundObject(bool VertexOnlyPass, unsigned int ShadowMap)
{

	CGroundObject *tmp = StartGroundObject;
	while (tmp != nullptr) {
		CGroundObject *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllGroundObject()
{
	CGroundObject *tmp = StartGroundObject;
	while (tmp != nullptr) {
		CGroundObject *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartGroundObject = nullptr;
	EndGroundObject = nullptr;
}
