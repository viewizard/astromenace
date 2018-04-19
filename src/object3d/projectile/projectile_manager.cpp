/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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

#include "projectile.h"



// Указатели на начальный и конечный объект в списке
cProjectile *StartProjectile = nullptr;
cProjectile *EndProjectile = nullptr;





//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachProjectile(cProjectile* Projectile)
{
	if (Projectile == nullptr)
		return;

	// первый в списке...
	if (EndProjectile == nullptr) {
		Projectile->Prev = nullptr;
		Projectile->Next = nullptr;
		StartProjectile = Projectile;
		EndProjectile = Projectile;
	} else { // продолжаем заполнение...
		Projectile->Prev = EndProjectile;
		Projectile->Next = nullptr;
		EndProjectile->Next = Projectile;
		EndProjectile = Projectile;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachProjectile(cProjectile* Projectile)
{
	if (Projectile == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartProjectile == Projectile)
		StartProjectile = Projectile->Next;
	if (EndProjectile == Projectile)
		EndProjectile = Projectile->Prev;

	if (Projectile->Next != nullptr)
		Projectile->Next->Prev = Projectile->Prev;
	else if (Projectile->Prev != nullptr)
		Projectile->Prev->Next = nullptr;

	if (Projectile->Prev != nullptr)
		Projectile->Prev->Next = Projectile->Next;
	else if (Projectile->Next != nullptr)
		Projectile->Next->Prev = nullptr;
}







//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllProjectile(float Time)
{
	cProjectile *tmp = StartProjectile;
	while (tmp != nullptr) {
		cProjectile *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllProjectile(bool VertexOnlyPass, unsigned int ShadowMap)
{

	cProjectile *tmp = StartProjectile;
	while (tmp != nullptr) {
		cProjectile *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass, ShadowMap);
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllProjectile()
{
	cProjectile *tmp = StartProjectile;
	while (tmp != nullptr) {
		cProjectile *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}
}

