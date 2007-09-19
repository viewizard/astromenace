/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: CollisionDetection.h

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


#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H


#include "../Base.h"
#include "../Math/Math.h"
#include "../Model3D/Model3D.h"



// Важно! Расположение точек в OBB и AABB в момент инициализации
//
//	VECTOR3D OBB[8], AABB[8];
//
//	OBB[0] = AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
//	OBB[1] = AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
//	OBB[2] = AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
//	OBB[3] = AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
//	OBB[4] = AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
//	OBB[5] = AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
//	OBB[6] = AABB[6] = VECTOR3D(MinX, MinY, MinZ);
//	OBB[7] = AABB[7] = VECTOR3D(MaxX, MinY, MinZ);



// проверка на попадание точки в треугольник
bool vw_PointInTriangle(VECTOR3D point, VECTOR3D pa, VECTOR3D pb, VECTOR3D pc);



// Проверка столкновения Sphere-Sphere
bool vw_SphereSphereCollision(float Object1Radius, VECTOR3D Object1Location, VECTOR3D Object1PrevLocation,
							float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation);
// Проверка столкновений Sphere-AABB
bool vw_SphereAABBCollision(VECTOR3D Object1AABB[8], VECTOR3D Object1Location,
							float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation);
// Проверка столкновений Sphere-OBB
bool vw_SphereOBBCollision(VECTOR3D Object1OBB[8], VECTOR3D Object1OBBLocation, VECTOR3D Object1Location, float Object1RotationMatrix[9],
							float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation);
// Проверка столкновений Sphere-Mesh
bool vw_SphereMeshCollision(VECTOR3D Object1Location, eObjectBlock *Object1DrawObjectList, float Object1RotationMatrix[9],
							float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation,
							VECTOR3D *CollisionLocation);



// Проверка столкновений AABB-AABB
bool vw_AABBAABBCollision(VECTOR3D Object1AABB[8], VECTOR3D Object1Location, VECTOR3D Object2AABB[8], VECTOR3D Object2Location);


// Проверка столкновений OBB-OBB
bool vw_OBBOBBCollision(VECTOR3D Object1OBB[8], VECTOR3D Object1OBBLocation, VECTOR3D Object1Location, float Object1RotationMatrix[9],
						VECTOR3D Object2OBB[8], VECTOR3D Object2OBBLocation, VECTOR3D Object2Location, float Object2RotationMatrix[9]);




#endif // COLLISIONDETECTION_H

