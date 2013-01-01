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


#ifndef PARTICLE_H
#define PARTICLE_H

#include "../Base.h"
#include "../Math/Math.h"



//-----------------------------------------------------------------------------
// Структура цвета
//-----------------------------------------------------------------------------
struct COLORVALUE3D
{
	float r;
	float g;
	float b;
	float a;
};



//-----------------------------------------------------------------------------
// Класс eParticle
//-----------------------------------------------------------------------------
struct eParticle
{
	// конструктор и деструктор
	eParticle();
	~eParticle();

	// обновление информации в частице
	bool Update(float TimeDelta, VECTOR3D ParentLocation = VECTOR3D(0.0f,0.0f,0.0f), bool Attractive = false, float AttractiveValue = 25.0f);
	// текущее место расположения частицы
	VECTOR3D		Location;
	// текущая скорость частицы
	VECTOR3D		Velocity;

	// texture number
	int			TextureNum;

	// текущий цвет частицы
	COLORVALUE3D	Color;
	// значение приращение цвета
	COLORVALUE3D	ColorDelta;

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
	// изменение прозрачности
	float		AlphaDelta;


	// сначало ув. альфу, потом уменьшаем
	bool	AlphaShowHide;
	// какой цикл - затухаем, или только появляемся
	bool	Show;

	// если нужно замедлять и остановить
	bool	NeedStop;

	// указатели на цепь частиц
	eParticle *Next;
	eParticle *Prev;
};


#endif // PARTICLE_H
