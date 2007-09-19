/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Particle2D.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#include "Particle2D.h"





//-----------------------------------------------------------------------------
// начальная инициализация
//-----------------------------------------------------------------------------
eParticle2D::eParticle2D()
{
	Age = 0.0f;
	Size = 1.0f;
	Color.r = 1.0f;
	Color.g = 0.0f;
	Color.b = 0.0f;
	Color.a = 0.5f;
	Next = 0;
	Prev = 0;
	NeedStop = false;
}


//-----------------------------------------------------------------------------
// деструктор частицы
//-----------------------------------------------------------------------------
eParticle2D::~eParticle2D()
{
	// тут ничего не делаем - все что нужно делаем в системе частиц
}


//-----------------------------------------------------------------------------
//	обновление информации частицы
//-----------------------------------------------------------------------------
bool eParticle2D::Update(float TimeDelta, VECTOR3D ParentLocation, bool Attractive, float AttractiveValue)
{

	// Если частица уже мертва, ее нужно отключить - передаем в систему эти данные
	if ( Age + TimeDelta >= Lifetime )
	{
		Age = -1.0f;
		return false;
	}
	else
	{

		// увеличиваем возраст частицы
		Age += TimeDelta;

		// перемещаем частицу на нужное значение
		Location += Velocity ^ TimeDelta;

		if (NeedStop)
			Velocity -= Velocity^TimeDelta;


		// если есть притяжение системы, просчитываем воздействие
		if ( Attractive )
		{
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

		// текущий размер частицы
		Size += SizeDelta * TimeDelta;
	}

	// если пришли сюда - значит все хорошо и частица работает
	return true;

}
