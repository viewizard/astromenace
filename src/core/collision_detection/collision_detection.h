/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef CORE_COLLISIONDETECTION_COLLISIONDETECTION_H
#define CORE_COLLISIONDETECTION_COLLISIONDETECTION_H

#include "../base.h"
#include "../model3d/model3d.h"

namespace viewizard {

struct sChunk3D;

// AABB-AABB collision detection.
bool vw_AABBAABBCollision(const bounding_box &Object1AABB, const sVECTOR3D &Object1Location,
			  const bounding_box &Object2AABB, const sVECTOR3D &Object2Location);
// OBB-OBB collision detection.
bool vw_OBBOBBCollision(const bounding_box &Object1OBB, const sVECTOR3D &Object1OBBLocation,
			const sVECTOR3D &Object1Location, const float (&Object1RotationMatrix)[9],
			const bounding_box &Object2OBB, const sVECTOR3D &Object2OBBLocation,
			const sVECTOR3D &Object2Location, const float (&Object2RotationMatrix)[9]);
// Sphere-Sphere collision detection.
bool vw_SphereSphereCollision(float Object1Radius, const sVECTOR3D &Object1Location,
			      float Object2Radius, const sVECTOR3D &Object2Location,
			      const sVECTOR3D &Object2PrevLocation);
// Sphere-AABB collision detection.
bool vw_SphereAABBCollision(const bounding_box &Object1AABB, const sVECTOR3D &Object1Location,
			    float Object2Radius, const sVECTOR3D &Object2Location, const sVECTOR3D &Object2PrevLocation);
// Sphere-OBB collision detection.
bool vw_SphereOBBCollision(const bounding_box &Object1OBB, const sVECTOR3D &Object1OBBLocation,
			   const sVECTOR3D &Object1Location, const float (&Object1RotationMatrix)[9],
			   float Object2Radius, const sVECTOR3D &Object2Location, const sVECTOR3D &Object2PrevLocation);
// Sphere-Mesh collision detection.
bool vw_SphereMeshCollision(const sVECTOR3D &Object1Location, const sChunk3D &Object1Chunks,
			    const float (&Object1RotationMatrix)[9], float Object2Radius, const sVECTOR3D &Object2Location,
			    const sVECTOR3D &Object2PrevLocation, sVECTOR3D &CollisionLocation);

} // viewizard namespace

#endif // CORE_COLLISIONDETECTION_COLLISIONDETECTION_H
