/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: ExplosionManager.cpp

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
#include "Explosion.h"



// Указатели на начальный и конечный объект в списке
CExplosion *StartExplosion = 0;
CExplosion *EndExplosion = 0;




//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachExplosion(CExplosion* Explosion)
{
	if (Explosion == 0) return;

	// первый в списке...
	if (EndExplosion == 0)
	{
		Explosion->Prev = 0;
		Explosion->Next = 0;
		StartExplosion = Explosion;
		EndExplosion = Explosion;
	}
	else // продолжаем заполнение...
	{
		Explosion->Prev = EndExplosion;
		Explosion->Next = 0;
		EndExplosion->Next = Explosion;
		EndExplosion = Explosion;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachExplosion(CExplosion* Explosion)
{
	if (Explosion == 0) return;

	// переустанавливаем указатели...
	if (StartExplosion == Explosion) StartExplosion = Explosion->Next;
	if (EndExplosion == Explosion) EndExplosion = Explosion->Prev;


	if (Explosion->Next != 0) Explosion->Next->Prev = Explosion->Prev;
		else if (Explosion->Prev != 0) Explosion->Prev->Next = 0;
	if (Explosion->Prev != 0) Explosion->Prev->Next = Explosion->Next;
		else if (Explosion->Next != 0) Explosion->Next->Prev = 0;
}





//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllExplosion(float Time)
{
	CExplosion *tmp = StartExplosion;
	while (tmp!=0)
	{
		CExplosion *tmp2 = tmp->Next;
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
void DrawAllExplosion()
{

	CExplosion *tmp = StartExplosion;
	while (tmp!=0)
	{
		CExplosion *tmp2 = tmp->Next;
		tmp->Draw();
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllExplosion()
{
	CExplosion *tmp = StartExplosion;
	while (tmp!=0)
	{
		CExplosion *tmp2 = tmp->Next;
		delete tmp; tmp = 0;
		tmp = tmp2;
	}

	StartExplosion = 0;
	EndExplosion = 0;
}
