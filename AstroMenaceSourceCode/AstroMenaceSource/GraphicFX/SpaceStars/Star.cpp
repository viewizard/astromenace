/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Star.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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


#include "Star.h"





//-----------------------------------------------------------------------------
// начальная инициализация
//-----------------------------------------------------------------------------
CStar::CStar()
{
	Next = 0;
	Prev = 0;
	Show = true;
}


//-----------------------------------------------------------------------------
// деструктор частицы
//-----------------------------------------------------------------------------
CStar::~CStar()
{
	// тут ничего не делаем - все что нужно делаем в системе частиц
}


//-----------------------------------------------------------------------------
//	обновление информации частицы
//-----------------------------------------------------------------------------
bool CStar::Update(float TimeDelta)
{

	if (Show)
	{
		Alpha += AlphaDelta * TimeDelta;
		if (Alpha >= 1.0f) Show = false;
	}
	else
	{
		Alpha -= AlphaDelta * TimeDelta;
		if (Alpha <= 0.3f) Show = true;
	}
	Clamp( Alpha, 0.0f, 1.0f );


	// если пришли сюда - значит все хорошо и частица работает
	return true;
}


//-----------------------------------------------------------------------------
//	переносим положение частицы на указаное растояние
//-----------------------------------------------------------------------------
void CStar::Move(VECTOR3D NewLocationDelta)
{
	// просто делаем приращение
	Location += NewLocationDelta;
}
