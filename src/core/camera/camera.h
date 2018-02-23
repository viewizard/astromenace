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

#ifndef CAMERA_H
#define CAMERA_H

#include "../base.h"
#include "../math/math.h"

/* Camera related function. */

/* Set camera location. */
void vw_SetCameraLocation(const VECTOR3D &NewLocation);
/* Increment camera location by vector. */
void vw_IncCameraLocation(const VECTOR3D &IncLocation);
/* Get camera location. */
VECTOR3D vw_GetCameraLocation(VECTOR3D *CurrentLocation);
/* Get camera rotation angles. */
VECTOR3D vw_GetCameraRotation(VECTOR3D *CurrentRotation);
/* Get camera focus point (anchor). */
VECTOR3D vw_GetCameraFocusPoint();
/* Move camera by direction. */
void vw_SetCameraMove(const VECTOR3D &NewRotation, float ChangeDistance, const VECTOR3D &Point);
/* Move camera around point (anchor). */
void vw_SetCameraMoveAroundPoint(const VECTOR3D &Point, float ChangeDistance, const VECTOR3D &ChangeRotation);
/* Camera deviation setup (need for camera shake effect). */
void vw_SetCameraDeviation(const VECTOR3D &NewCameraDeviation);
/* Camera setup. */
void vw_CameraLookAt();

/* Frustum related function. */

/* Call this every time the camera moves to update the frustum. */
void vw_CalculateFrustum();
/* This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum. */
bool vw_SphereInFrustum(const VECTOR3D &Point, float Radius);
/* This checks if a box is in the frustum. */
bool vw_BoxInFrustum(const VECTOR3D &MinPoint, const VECTOR3D &MaxPoint);

#endif // CAMERA_H
