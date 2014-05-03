/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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

