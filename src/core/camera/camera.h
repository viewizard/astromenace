/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

*****************************************************************************/

#ifndef CORE_CAMERA_CAMERA_H
#define CORE_CAMERA_CAMERA_H

#include "../base.h"

namespace viewizard {

struct sVECTOR3D;

/* Camera related function. */

// Set camera location.
void vw_SetCameraLocation(const sVECTOR3D &NewLocation);
// Increment camera location by vector.
void vw_IncCameraLocation(const sVECTOR3D &IncLocation);
// Get camera location.
sVECTOR3D vw_GetCameraLocation(sVECTOR3D *CurrentLocation);
// Get camera rotation angles.
sVECTOR3D vw_GetCameraRotation(sVECTOR3D *CurrentRotation);
// Get camera focus point (anchor).
sVECTOR3D vw_GetCameraFocusPoint();
// Move camera by direction.
void vw_SetCameraMove(const sVECTOR3D &NewRotation, float ChangeDistance, const sVECTOR3D &Point);
// Move camera around point (anchor).
void vw_SetCameraMoveAroundPoint(const sVECTOR3D &Point, float ChangeDistance, const sVECTOR3D &ChangeRotation);
// Camera deviation setup (need for camera shake effect).
void vw_SetCameraDeviation(const sVECTOR3D &NewCameraDeviation);
// Camera setup.
void vw_CameraLookAt();

/* Frustum related function. */

// Call this every time the camera moves to update the frustum.
void vw_CalculateFrustum();
// This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum.
bool vw_SphereInFrustum(const sVECTOR3D &Point, float Radius);
// This checks if a box is in the frustum.
bool vw_BoxInFrustum(const sVECTOR3D &MinPoint, const sVECTOR3D &MaxPoint);

} // viewizard namespace

#endif // CORE_CAMERA_CAMERA_H
