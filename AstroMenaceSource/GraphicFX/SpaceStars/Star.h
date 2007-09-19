/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Star.h

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


#ifndef STAR_H
#define STAR_H


#include "../../Core/Core.h"
#include "../../Struct.h"






//-----------------------------------------------------------------------------
// Класс CStar
//-----------------------------------------------------------------------------
struct CStar
{
	// конструктор и деструктор
	CStar();
	~CStar();

	// обновление информации в частице
	bool Update(float TimeDelta);
	// переносим положение частицы на указаное растояние
	void Move(VECTOR3D NewLocationDelta);
	// текущее место расположения частицы
	VECTOR3D		Location;

	// прозрачность
	float		Alpha;
	// изменение прозрачности
	float		AlphaDelta;


	// какой цикл - затухаем, или только появляемся
	bool	Show;

	// указатели на цепь частиц
	CStar *Next;
	CStar *Prev;
};


#endif // STAR_H
