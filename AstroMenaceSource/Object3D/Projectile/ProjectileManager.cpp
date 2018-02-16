/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
		delete tmp;
		tmp = tmp2;
	}
}

