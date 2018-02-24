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

#ifndef PARTICLE2D_H
#define PARTICLE2D_H

#include "../base.h"
#include "../math/math.h"

//-----------------------------------------------------------------------------
// Структура цвета
//-----------------------------------------------------------------------------
struct COLORVALUE2D {
	float r;
	float g;
	float b;
	float a;
};


//-----------------------------------------------------------------------------
// Класс eParticle2D
//-----------------------------------------------------------------------------
struct eParticle2D {
	// конструктор и деструктор
	eParticle2D() {};
	~eParticle2D() {};

	// обновление информации в частице
	bool Update(float TimeDelta, VECTOR3D ParentLocation = VECTOR3D(0.0f,0.0f,0.0f), bool Attractive = false, float AttractiveValue = 25.0f);
	// текущее место расположения частицы
	VECTOR3D	Location{0.0f, 0.0f, 0.0f};
	// текущая скорость частицы
	VECTOR3D	Velocity{0.0f, 0.0f, 0.0f};

	// текущий цвет частицы
	COLORVALUE2D	Color{1.0f, 0.0f, 0.0f, 0.5f};
	// значение приращение цвета
	COLORVALUE2D	ColorDelta{0.0f, 0.0f, 0.0f, 0.0f};

	// время жизни частицы в секундах
	float		Age{0.0f};

	// оставщееся время жизни частицы
	float		Lifetime{0.0f};

	// размер частицы
	float		Size{1.0f};
	// значение изменения размера
	float		SizeDelta{0.0f};

	// прозрачность
	float		Alpha{1.0f};

	// если нужно замедлять и остановить
	bool		NeedStop{false};

	// указатели на цепь частиц
	eParticle2D	*Next{nullptr};
	eParticle2D	*Prev{nullptr};
};

#endif // PARTICLE2D_H
