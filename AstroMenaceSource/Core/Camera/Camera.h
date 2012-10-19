/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Camera.h

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

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

