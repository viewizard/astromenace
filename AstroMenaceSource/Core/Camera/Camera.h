/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#ifndef CAMERA_H
#define CAMERA_H


#include "../Base.h"
#include "../Math/Math.h"


// работа с положением камеры
void vw_SetCameraLocation(VECTOR3D NewLocation);
void vw_IncCameraLocation(VECTOR3D IncLocation);
VECTOR3D vw_GetCameraLocation(VECTOR3D *CurrentLocation);
VECTOR3D vw_GetCameraFocusPoint();
// работа с поворотом камеры
void vw_SetCameraRotation(VECTOR3D NewRotation);
VECTOR3D vw_GetCameraRotation(VECTOR3D *CurrentRotation);
// перемещение камеры
void vw_SetCameraMove(VECTOR3D NewRotation, float ChangeDistance, VECTOR3D Point);
// перемещение относительно точки
void vw_SetCameraMoveAroundPoint(VECTOR3D Point, float ChangeDistance, VECTOR3D ChangeRotation);
// установка болтания камеры
void vw_SetCameraDeviation(VECTOR3D NewCameraDeviation);
// установка камеры
void vw_CameraLookAt(void);




// This normalizes a plane (A side) from a given frustum.
void vw_NormalizePlane(float frustum[6][4], int side);
// Call this every time the camera moves to update the frustum
void vw_CalculateFrustum(void);
// This takes a 3D point and returns TRUE if it's inside of the frustum
bool vw_PointInFrustum(VECTOR3D Point);
// This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
bool vw_SphereInFrustum(VECTOR3D Point, float radius);
// This takes the center and half the length of the cube.
bool vw_CubeInFrustum(VECTOR3D CenterPoint, float size);
// This checks if a box is in the frustum
bool vw_BoxInFrustum(VECTOR3D MinPoint, VECTOR3D MaxPoint);








#endif // CAMERA_H

