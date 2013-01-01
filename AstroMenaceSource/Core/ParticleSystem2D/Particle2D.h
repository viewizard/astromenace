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


#ifndef PARTICLE2D_H
#define PARTICLE2D_H

#include "../Base.h"
#include "../Math/Math.h"





//-----------------------------------------------------------------------------
// Структура цвета
//-----------------------------------------------------------------------------
struct COLORVALUE2D
{
	float r;
	float g;
	float b;
	float a;
};





//-----------------------------------------------------------------------------
// Класс eParticle2D
//-----------------------------------------------------------------------------
struct eParticle2D
{
	// конструктор и деструктор
	eParticle2D();
	~eParticle2D();

	// обновление информации в частице
	bool Update(float TimeDelta, VECTOR3D ParentLocation = VECTOR3D(0.0f,0.0f,0.0f), bool Attractive = false, float AttractiveValue = 25.0f);
	// текущее место расположения частицы
	VECTOR3D		Location;
	// текущая скорость частицы
	VECTOR3D		Velocity;

	// текущий цвет частицы
	COLORVALUE2D	Color;
	// значение приращение цвета
	COLORVALUE2D	ColorDelta;

	// время жизни частицы в секундах
	float		Age;

	// оставщееся время жизни частицы
	float		Lifetime;

	// размер частицы
	float		Size;
	// значение изменения размера
	float		SizeDelta;

	// прозрачность
	float		Alpha;

	// если нужно замедлять и остановить
	bool	NeedStop;

	// указатели на цепь частиц
	eParticle2D *Next;
	eParticle2D *Prev;
};


#endif // PARTICLE2D_H
