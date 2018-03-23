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

#include "../graphics/graphics.h"
#include "../math/math.h"

namespace {

float Frustum[6][4]{{0, 0, 0, 0},
		    {0, 0, 0, 0},
		    {0, 0, 0, 0},
		    {0, 0, 0, 0},
		    {0, 0, 0, 0},
		    {0, 0, 0, 0}};

enum ePlaneData {
	A = 0,		// The X value of the plane's normal.
	B = 1,		// The Y value of the plane's normal.
	C = 2,		// The Z value of the plane's normal.
	D = 3		// The distance the plane is from the origin.
};

enum eSide {
	RIGHT	= 0,	// The RIGHT side.
	LEFT	= 1,	// The LEFT side.
	BOTTOM	= 2,	// The BOTTOM side.
	TOP	= 3,	// The TOP side.
	BACK	= 4,	// The BACK side.
	FRONT	= 5	// The FRONT side.
};

} // unnamed namespace

/*
 * This normalizes a plane (A side) from a given frustum.
 */
static void NormalizePlane(int Side)
{
	// Here we calculate the magnitude of the normal to the plane (point A B C)
	// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
	// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
	float Magnitude{vw_sqrtf(Frustum[Side][A] * Frustum[Side][A] +
				 Frustum[Side][B] * Frustum[Side][B] +
				 Frustum[Side][C] * Frustum[Side][C])};

	// Then we divide the plane's values by it's magnitude.
	// This makes it easier to work with.
	Frustum[Side][A] /= Magnitude;
	Frustum[Side][B] /= Magnitude;
	Frustum[Side][C] /= Magnitude;
	Frustum[Side][D] /= Magnitude;
}

/*
 * Call this every time the camera moves to update the frustum
 */
void vw_CalculateFrustum()
{
	float proj[16]; // This will hold our projection matrix.
	float modl[16]; // This will hold our modelview matrix.
	float clip[16]; // This will hold the clipping planes.

	// Below, we pass in RI_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	vw_GetMatrix(RI_PROJECTION_MATRIX, proj);

	// By passing in RI_MODELVIEW_MATRIX, we can abstract our model view matrix.
	// This also stores it in an array of [16].
	vw_GetMatrix(RI_MODELVIEW_MATRIX, modl);

	// Now that we have our modelview and projection matrix, if we combine these 2 matrices,
	// it will give us our clipping planes.  To combine 2 matrices, we multiply them.
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	// Now we actually want to get the sides of the frustum.  To do this we take
	// the clipping planes we received above and extract the sides from them.
	Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
	Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
	Frustum[RIGHT][C] = clip[11] - clip[ 8];
	Frustum[RIGHT][D] = clip[15] - clip[12];
	NormalizePlane(RIGHT);

	Frustum[LEFT][A] = clip[ 3] + clip[ 0];
	Frustum[LEFT][B] = clip[ 7] + clip[ 4];
	Frustum[LEFT][C] = clip[11] + clip[ 8];
	Frustum[LEFT][D] = clip[15] + clip[12];
	NormalizePlane(LEFT);

	Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
	Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
	Frustum[BOTTOM][C] = clip[11] + clip[ 9];
	Frustum[BOTTOM][D] = clip[15] + clip[13];
	NormalizePlane(BOTTOM);

	Frustum[TOP][A] = clip[ 3] - clip[ 1];
	Frustum[TOP][B] = clip[ 7] - clip[ 5];
	Frustum[TOP][C] = clip[11] - clip[ 9];
	Frustum[TOP][D] = clip[15] - clip[13];
	NormalizePlane(TOP);

	Frustum[BACK][A] = clip[ 3] - clip[ 2];
	Frustum[BACK][B] = clip[ 7] - clip[ 6];
	Frustum[BACK][C] = clip[11] - clip[10];
	Frustum[BACK][D] = clip[15] - clip[14];
	NormalizePlane(BACK);

	Frustum[FRONT][A] = clip[ 3] + clip[ 2];
	Frustum[FRONT][B] = clip[ 7] + clip[ 6];
	Frustum[FRONT][C] = clip[11] + clip[10];
	Frustum[FRONT][D] = clip[15] + clip[14];
	NormalizePlane(FRONT);
}

/*
 * This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
 */
bool vw_SphereInFrustum(const sVECTOR3D &Point, float Radius)
{
	// go through all the sides of the frustum
	for (int i = 0; i < 6; i++) {
		// if the center of the sphere is farther away from the plane than the radius
		if (Frustum[i][A] * Point.x + Frustum[i][B] * Point.y + Frustum[i][C] * Point.z + Frustum[i][D] <= -Radius)
			// the distance was greater than the radius so the sphere is outside of the frustum
			return false;
	}
	// the sphere inside of the frustum
	return true;
}

/*
 * This checks if a box is in the frustum.
 */
bool vw_BoxInFrustum(const sVECTOR3D &MinPoint, const sVECTOR3D &MaxPoint)
{
	// Go through all of the corners of the box and check then again each plane
	// in the frustum.  If all of them are behind one of the planes, then it most
	// like is not in the frustum.
	// Note! This will sometimes say that a box is inside the frustum when it is not.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and should not effect the overall rendering speed.
	for (int i = 0; i < 6; i++) {
		if ((Frustum[i][A] * MinPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MinPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MinPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MinPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] <= 0) &&
		    (Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] <= 0))
			// if we get here, it is not in the frustum
			return false;

	}
	// return a true for the box being inside of the frustum
	return true;
}
