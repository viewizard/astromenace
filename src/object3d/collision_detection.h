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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef OBJECT3D_COLLISIONDETECTION_H
#define OBJECT3D_COLLISIONDETECTION_H

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// Mesh-Sphere collision detection.
bool CheckMeshSphereCollisionDetection(const cObject3D &Object1, const cObject3D &Object2,
				       sVECTOR3D &NewLoc, int &Object1PieceNum);
// HitBB-HitBB collision detection.
bool CheckHitBBHitBBCollisionDetection(const cObject3D &Object1, const cObject3D &Object2,
				       int &Object1PieceNum, int &Object2PieceNum);
// HitBB-OBB collision detection.
bool CheckHitBBOBBCollisionDetection(const cObject3D &Object1, const cObject3D &Object2, int &Object1PieceNum);
// HitBB-Mesh collision detection.
bool CheckHitBBMeshCollisionDetection(const cObject3D &Object1, const cObject3D &Object2, int &Object1PieceNum);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_COLLISIONDETECTION_H
