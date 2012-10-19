/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceObjectManager.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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
void DrawAllSpaceObject(bool VertexOnlyPass, unsigned int ShadowMap, int DrawOnlyType)
{

	CSpaceObject *tmp = StartSpaceObject;
	while (tmp!=0)
	{
		CSpaceObject *tmp2 = tmp->Next;

		if (DrawOnlyType != -1)
		{
			// если нужно прорисовать только определенный тип
			if (tmp->ObjectType == DrawOnlyType)
			if (tmp->ObjectType != 14 && !(tmp->ObjectType == 15 && (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
				tmp->Draw(VertexOnlyPass, ShadowMap);
		}
		else
		{
			// планеты и астероиды рисуем до тайловой анимации в игре!!!
			if (tmp->ObjectType != 14 && !(tmp->ObjectType == 15 && (tmp->ObjectCreationType>10 && tmp->ObjectCreationType<20)))
				tmp->Draw(VertexOnlyPass, ShadowMap);
		}

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
