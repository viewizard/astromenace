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

#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "../base.h"

struct sObjectBlock;

// AABB-AABB collision detection.
bool vw_AABBAABBCollision(const sVECTOR3D Object1AABB[8], const sVECTOR3D &Object1Location,
			  const sVECTOR3D Object2AABB[8], const sVECTOR3D &Object2Location);
// OBB-OBB collision detection.
bool vw_OBBOBBCollision(sVECTOR3D Object1OBB[8], sVECTOR3D Object1OBBLocation, sVECTOR3D Object1Location,
			float Object1RotationMatrix[9], sVECTOR3D Object2OBB[8], sVECTOR3D Object2OBBLocation,
			sVECTOR3D Object2Location, float Object2RotationMatrix[9]);
// Sphere-Sphere collision detection.
bool vw_SphereSphereCollision(float Object1Radius, const sVECTOR3D &Object1Location,
			      float Object2Radius, const sVECTOR3D &Object2Location,
			      const sVECTOR3D &Object2PrevLocation);
// Sphere-AABB collision detection.
bool vw_SphereAABBCollision(sVECTOR3D Object1AABB[8], sVECTOR3D Object1Location,
			    float Object2Radius, sVECTOR3D Object2Location, sVECTOR3D Object2PrevLocation);
// Sphere-OBB collision detection.
bool vw_SphereOBBCollision(sVECTOR3D Object1OBB[8], sVECTOR3D Object1OBBLocation,
			   sVECTOR3D Object1Location, float Object1RotationMatrix[9],
			   float Object2Radius, sVECTOR3D Object2Location, sVECTOR3D Object2PrevLocation);
// Sphere-Mesh collision detection.
bool vw_SphereMeshCollision(sVECTOR3D Object1Location, sObjectBlock *Object1DrawObjectList, float Object1RotationMatrix[9],
			    float Object2Radius, sVECTOR3D Object2Location, sVECTOR3D Object2PrevLocation,
			    sVECTOR3D *CollisionLocation);

#endif // COLLISIONDETECTION_H
