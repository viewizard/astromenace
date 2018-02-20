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
#include "explosion.h"



// Указатели на начальный и конечный объект в списке
CExplosion *StartExplosion = nullptr;
CExplosion *EndExplosion = nullptr;




//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void AttachExplosion(CExplosion* Explosion)
{
	if (Explosion == nullptr)
		return;

	if (EndExplosion == nullptr) {
		Explosion->Prev = nullptr;
		Explosion->Next = nullptr;
		StartExplosion = Explosion;
		EndExplosion = Explosion;
	} else {
		Explosion->Prev = EndExplosion;
		Explosion->Next = nullptr;
		EndExplosion->Next = Explosion;
		EndExplosion = Explosion;
	}
}





//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void DetachExplosion(CExplosion* Explosion)
{
	if (Explosion == nullptr)
		return;

	if (StartExplosion == Explosion)
		StartExplosion = Explosion->Next;
	if (EndExplosion == Explosion)
		EndExplosion = Explosion->Prev;

	if (Explosion->Next != nullptr)
		Explosion->Next->Prev = Explosion->Prev;
	else if (Explosion->Prev != nullptr)
		Explosion->Prev->Next = nullptr;

	if (Explosion->Prev != nullptr)
		Explosion->Prev->Next = Explosion->Next;
	else if (Explosion->Next != nullptr)
		Explosion->Next->Prev = nullptr;
}





//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllExplosion(float Time)
{
	CExplosion *tmp = StartExplosion;
	while (tmp != nullptr) {
		CExplosion *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllExplosion(bool VertexOnlyPass)
{

	CExplosion *tmp = StartExplosion;
	while (tmp != nullptr) {
		CExplosion *tmp2 = tmp->Next;
		tmp->Draw(VertexOnlyPass);
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllExplosion()
{
	CExplosion *tmp = StartExplosion;
	while (tmp != nullptr) {
		CExplosion *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartExplosion = nullptr;
	EndExplosion = nullptr;
}
