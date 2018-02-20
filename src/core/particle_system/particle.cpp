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


#include "particle.h"





//-----------------------------------------------------------------------------
// начальная инициализация
//-----------------------------------------------------------------------------
eParticle::eParticle()
{
	TextureNum = 0;
	Age = 0.0f;
	Lifetime = 0.0f;
	Size = 1.0f;
	SizeDelta = 0.0f;
	Alpha = 1.0f;
	AlphaDelta = 0.0f;
	Color.r = 1.0f;
	Color.g = 0.0f;
	Color.b = 0.0f;
	Color.a = 0.5f;
	ColorDelta.a = ColorDelta.b = ColorDelta.g = ColorDelta.r = 0.0f;
	Next = nullptr;
	Prev = nullptr;
	Show = true;
	AlphaShowHide = false;
	NeedStop = false;
}


//-----------------------------------------------------------------------------
// деструктор частицы
//-----------------------------------------------------------------------------
eParticle::~eParticle()
{
	// тут ничего не делаем - все что нужно делаем в системе частиц
}


//-----------------------------------------------------------------------------
//	обновление информации частицы
//-----------------------------------------------------------------------------
bool eParticle::Update(float TimeDelta, VECTOR3D ParentLocation, bool Attractive, float AttractiveValue)
{

	// Если частица уже мертва, ее нужно отключить - передаем в систему эти данные
	if ( Age + TimeDelta >= Lifetime ) {
		Age = -1.0f;
		return false;
	} else {

		// увеличиваем возраст частицы
		Age += TimeDelta;

		// перемещаем частицу на нужное значение
		Location += Velocity ^ TimeDelta;

		if (NeedStop)
			Velocity -= Velocity^TimeDelta;


		// если есть притяжение системы, просчитываем воздействие
		if ( Attractive ) {
			VECTOR3D AttractLocation = ParentLocation;

			// рассчитывае вектор взаимодействия между частицей и точкой притяжения
			VECTOR3D AttractDir = AttractLocation - Location;

			// если нужно использовать притяжения, считаем перемещение
			if (NeedStop)
				AttractiveValue -= AttractiveValue*TimeDelta;

			AttractDir.Normalize();
			Velocity += AttractDir ^ (AttractiveValue * TimeDelta);

		}

		// просчитываем текущий цвет частицы
		Color.r += ColorDelta.r * TimeDelta;
		Color.g += ColorDelta.g * TimeDelta;
		Color.b += ColorDelta.b * TimeDelta;

		// просчитываем текущую прозрачность
		if (!AlphaShowHide) {
			Alpha += AlphaDelta * TimeDelta;
		} else {
			if (Show) {
				Alpha += AlphaDelta * TimeDelta;
				if (Alpha >= 1.0f) {
					Alpha = 1.0f;
					Show = false;
				}
			} else
				Alpha -= AlphaDelta * TimeDelta;

			Clamp( Alpha, 0.0f, 1.0f );
		}


		// текущий размер частицы
		Size += SizeDelta * TimeDelta;


	}

	// если пришли сюда - значит все хорошо и частица работает
	return true;

}
