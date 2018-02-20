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
void vw_SetCameraLocation(VECTOR3D NewLocation);
/* Increment camera location by vector. */
void vw_IncCameraLocation(VECTOR3D IncLocation);
/* Get camera location. */
VECTOR3D vw_GetCameraLocation(VECTOR3D *CurrentLocation);
/* Get camera focus point (anchor). */
VECTOR3D vw_GetCameraFocusPoint();
/* Set camera rotation angles. */
void vw_SetCameraRotation(VECTOR3D NewRotation);
/* Get camera rotation angles. */
VECTOR3D vw_GetCameraRotation(VECTOR3D *CurrentRotation);
/* Move camera by direction. */
void vw_SetCameraMove(VECTOR3D NewRotation, float ChangeDistance, VECTOR3D Point);
/* Move camera around point (anchor). */
void vw_SetCameraMoveAroundPoint(VECTOR3D Point, float ChangeDistance, VECTOR3D ChangeRotation);
/* Camera deviation setup (need for camera shake effect). */
void vw_SetCameraDeviation(VECTOR3D NewCameraDeviation);
/* Camera setup. */
void vw_CameraLookAt();

/* Frustum related function. */

/* Call this every time the camera moves to update the frustum. */
void vw_CalculateFrustum();
/* This takes a 3D point and returns TRUE if it's inside of the frustum. */
bool vw_PointInFrustum(VECTOR3D Point);
/* This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum. */
bool vw_SphereInFrustum(VECTOR3D Point, float Radius);
/* This takes the center and half the length of the cube. */
bool vw_CubeInFrustum(VECTOR3D CenterPoint, float Size);
/* This checks if a box is in the frustum. */
bool vw_BoxInFrustum(VECTOR3D MinPoint, VECTOR3D MaxPoint);

#endif // CAMERA_H
