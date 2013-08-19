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
#include "GroundObject.h"



// Указатели на начальный и конечный объект в списке
CGroundObject *StartGroundObject = 0;
CGroundObject *EndGroundObject = 0;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachGroundObject(CGroundObject* GroundObject)
{
	if (GroundObject == 0) return;

	// первый в списке...
	if (EndGroundObject == 0)
	{
		GroundObject->Prev = 0;
		GroundObject->Next = 0;
		StartGroundObject = GroundObject;
		EndGroundObject = GroundObject;
	}
	else // продолжаем заполнение...
	{
		GroundObject->Prev = EndGroundObject;
		GroundObject->Next = 0;
		EndGroundObject->Next = GroundObject;
		EndGroundObject = GroundObject;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachGroundObject(CGroundObject* GroundObject)
{
	if (GroundObject == 0) return;

	// переустанавливаем указатели...
	if (StartGroundObject == GroundObject) StartGroundObject = GroundObject->Next;
	if (EndGroundObject == GroundObject) EndGroundObject = GroundObject->Prev;


	if (GroundObject->Next != 0) GroundObject->Next->Prev = GroundObject->Prev;
		else if (GroundObject->Prev != 0) GroundObject->Prev->Next = 0;
	if (GroundObject->Prev != 0) GroundObject->Prev->Next = GroundObject->Next;
		else if (GroundObject->Next != 0) GroundObject->Next->Prev = 0;
}





//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllGroundObject(float Time)
{
	CGroundObject *tmp = StartGroundObject;
	while (tmp!=0)
	{
		CGroundObject *tmp2 = tmp->Next;
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
void DrawAllGroundObject(bool VertexOnlyPass, unsigned int ShadowMap)
{

	CGroundObject *tmp = StartGroundObject;
	while (tmp!=0)
	{
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
	while (tmp!=0)
	{
		CGroundObject *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartGroundObject = 0;
	EndGroundObject = 0;
}
