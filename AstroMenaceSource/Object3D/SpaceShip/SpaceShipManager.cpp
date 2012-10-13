/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceShipManager.cpp

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
void DrawAllSpaceShip(bool VertexOnlyPass)
{

	CSpaceShip *tmp = StartSpaceShip;
	while (tmp!=0)
	{
		CSpaceShip *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass);
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
