/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: GroundObjectManager.cpp

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
void DrawAllGroundObject()
{

	CGroundObject *tmp = StartGroundObject;
	while (tmp!=0)
	{
		CGroundObject *tmp2 = tmp->Next;
		tmp->Draw();
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
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartGroundObject = 0;
	EndGroundObject = 0;
}
