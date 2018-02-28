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

#ifndef STAR_H
#define STAR_H

#include "../../core/core.h"
#include "../../struct.h"


//-----------------------------------------------------------------------------
// Класс CStar
//-----------------------------------------------------------------------------
struct sStar {
	// конструктор и деструктор
	sStar() {};
	~sStar() {};

	// обновление информации в частице
	bool	Update(float TimeDelta);
	// переносим положение частицы на указаное растояние
	void	Move(sVECTOR3D NewLocationDelta);
	// текущее место расположения частицы
	sVECTOR3D	Location{0.0f, 0.0f, 0.0f};

	// прозрачность
	float	Alpha{1.0f};
	// изменение прозрачности
	float	AlphaDelta{0.0f};

	// какой цикл - затухаем, или только появляемся
	bool	Show{true};

	// указатели на цепь частиц
	sStar	*Next{nullptr};
	sStar	*Prev{nullptr};
};

#endif // STAR_H
