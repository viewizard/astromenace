/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: AABB.cpp

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


#include "CollisionDetection.h"





//-----------------------------------------------------------------------------
// Проверка столкновений AABB-AABB
//-----------------------------------------------------------------------------
bool vw_AABBAABBCollision(VECTOR3D Object1AABB[8], VECTOR3D Object1Location, VECTOR3D Object2AABB[8], VECTOR3D Object2Location)
{
	// если проекции пересекаются...
	if (fabsf(Object1Location.x - Object2Location.x) > fabsf(Object1AABB[0].x + Object2AABB[0].x))
		return false;
	if (fabsf(Object1Location.y - Object2Location.y) > fabsf(Object1AABB[0].y + Object2AABB[0].y))
		return false;
	if (fabsf(Object1Location.z - Object2Location.z) > fabsf(Object1AABB[0].z + Object2AABB[0].z))
		return false;

	// если дошли сюда - значит наложение коробок есть
	return true;
}

