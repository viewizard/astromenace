/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: ProjectileManager.cpp

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
#include "Projectile.h"



// Указатели на начальный и конечный объект в списке
CProjectile *StartProjectile = 0;
CProjectile *EndProjectile = 0;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachProjectile(CProjectile* Projectile)
{
	if (Projectile == 0) return;

	// первый в списке...
	if (EndProjectile == 0)
	{
		Projectile->Prev = 0;
		Projectile->Next = 0;
		StartProjectile = Projectile;
		EndProjectile = Projectile;
	}
	else // продолжаем заполнение...
	{
		Projectile->Prev = EndProjectile;
		Projectile->Next = 0;
		EndProjectile->Next = Projectile;
		EndProjectile = Projectile;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachProjectile(CProjectile* Projectile)
{
	if (Projectile == 0) return;

	// переустанавливаем указатели...
	if (StartProjectile == Projectile) StartProjectile = Projectile->Next;
	if (EndProjectile == Projectile) EndProjectile = Projectile->Prev;


	if (Projectile->Next != 0) Projectile->Next->Prev = Projectile->Prev;
		else if (Projectile->Prev != 0) Projectile->Prev->Next = 0;
	if (Projectile->Prev != 0) Projectile->Prev->Next = Projectile->Next;
		else if (Projectile->Next != 0) Projectile->Next->Prev = 0;
}







//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllProjectile(float Time)
{
	CProjectile *tmp = StartProjectile;
	while (tmp!=0)
	{
		CProjectile *tmp2 = tmp->Next;
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
void DrawAllProjectile(bool VertexOnlyPass, unsigned int ShadowMap)
{

	CProjectile *tmp = StartProjectile;
	while (tmp!=0)
	{
		CProjectile *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllProjectile()
{
	CProjectile *tmp = StartProjectile;
	while (tmp!=0)
	{
		CProjectile *tmp2 = tmp->Next;
		delete tmp; tmp = 0;
		tmp = tmp2;
	}
}

