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


#include "Camera.h"
#include "../RendererInterface/RendererInterface.h"


//-----------------------------------------------------------------------------
// локальные/защищенные переменные
//-----------------------------------------------------------------------------

// видимый объем пространства
float Frustum[6][4];

enum PlaneData
{
	A = 0,				// The X value of the plane's normal
	B = 1,				// The Y value of the plane's normal
	C = 2,				// The Z value of the plane's normal
	D = 3				// The distance the plane is from the origin
};
enum Side
{
	RIGHT	= 0,		// The RIGHT side
	LEFT	= 1,		// The LEFT	 side
	BOTTOM	= 2,		// The BOTTOM side
	TOP		= 3,		// The TOP side
	BACK	= 4,		// The BACK	side
	FRONT	= 5			// The FRONT side
};






//-----------------------------------------------------------------------------
// This normalizes a plane (A side) from a given frustum.
//-----------------------------------------------------------------------------
void vw_NormalizePlane(float frustum[6][4], int side)
{
	// Here we calculate the magnitude of the normal to the plane (point A B C)
	// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
	// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
	float magnitude = vw_sqrtf( frustum[side][A] * frustum[side][A] +
								   frustum[side][B] * frustum[side][B] +
								   frustum[side][C] * frustum[side][C] );

	// Then we divide the plane's values by it's magnitude.
	// This makes it easier to work with.
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude;
}



//-----------------------------------------------------------------------------
// Call this every time the camera moves to update the frustum
//-----------------------------------------------------------------------------
void vw_CalculateFrustum(void)
{
	float   proj[16];								// This will hold our projection matrix
	float   modl[16];								// This will hold our modelview matrix
	float   clip[16];								// This will hold the clipping planes

	// glGetFloatv() is used to extract information about our OpenGL world.
	// Below, we pass in GL_PROJECTION_MATRIX to abstract our projection matrix.
	// It then stores the matrix into an array of [16].
	vw_GetMatrix(RI_PROJECTION_MATRIX, proj);


	// By passing in GL_MODELVIEW_MATRIX, we can abstract our model view matrix.
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

	// This will extract the RIGHT side of the frustum
	Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
	Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
	Frustum[RIGHT][C] = clip[11] - clip[ 8];
	Frustum[RIGHT][D] = clip[15] - clip[12];
	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.
	// Normalize the RIGHT side
	vw_NormalizePlane(Frustum, RIGHT);

	// This will extract the LEFT side of the frustum
	Frustum[LEFT][A] = clip[ 3] + clip[ 0];
	Frustum[LEFT][B] = clip[ 7] + clip[ 4];
	Frustum[LEFT][C] = clip[11] + clip[ 8];
	Frustum[LEFT][D] = clip[15] + clip[12];
	// Normalize the LEFT side
	vw_NormalizePlane(Frustum, LEFT);

	// This will extract the BOTTOM side of the frustum
	Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
	Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
	Frustum[BOTTOM][C] = clip[11] + clip[ 9];
	Frustum[BOTTOM][D] = clip[15] + clip[13];
	// Normalize the BOTTOM side
	vw_NormalizePlane(Frustum, BOTTOM);

	// This will extract the TOP side of the frustum
	Frustum[TOP][A] = clip[ 3] - clip[ 1];
	Frustum[TOP][B] = clip[ 7] - clip[ 5];
	Frustum[TOP][C] = clip[11] - clip[ 9];
	Frustum[TOP][D] = clip[15] - clip[13];
	// Normalize the TOP side
	vw_NormalizePlane(Frustum, TOP);

	// This will extract the BACK side of the frustum
	Frustum[BACK][A] = clip[ 3] - clip[ 2];
	Frustum[BACK][B] = clip[ 7] - clip[ 6];
	Frustum[BACK][C] = clip[11] - clip[10];
	Frustum[BACK][D] = clip[15] - clip[14];
	// Normalize the BACK side
	vw_NormalizePlane(Frustum, BACK);

	// This will extract the FRONT side of the frustum
	Frustum[FRONT][A] = clip[ 3] + clip[ 2];
	Frustum[FRONT][B] = clip[ 7] + clip[ 6];
	Frustum[FRONT][C] = clip[11] + clip[10];
	Frustum[FRONT][D] = clip[15] + clip[14];
	// Normalize the FRONT side
	vw_NormalizePlane(Frustum, FRONT);
}



//-----------------------------------------------------------------------------
// This takes a 3D point and returns TRUE if it's inside of the frustum
//-----------------------------------------------------------------------------
bool vw_PointInFrustum(VECTOR3D Point)
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if(Frustum[i][A] * Point.x + Frustum[i][B] * Point.y + Frustum[i][C] * Point.z + Frustum[i][D] <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}



//-----------------------------------------------------------------------------
// This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
//-----------------------------------------------------------------------------
bool vw_SphereInFrustum(VECTOR3D Point, float radius)
{
	// Go through all the sides of the frustum
	for(int i = 0; i < 6; i++ )
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( Frustum[i][A] * Point.x + Frustum[i][B] * Point.y + Frustum[i][C] * Point.z + Frustum[i][D] <= -radius )
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}

	// The sphere was inside of the frustum!
	return true;
}



//-----------------------------------------------------------------------------
// This takes the center and half the length of the cube.
//-----------------------------------------------------------------------------
bool vw_CubeInFrustum(VECTOR3D CenterPoint, float size)
{
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in front
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in front of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	// поставить доп проверку...

	for(int i = 0; i < 6; i++ )
	{
		if(Frustum[i][A] * (CenterPoint.x - size) + Frustum[i][B] * (CenterPoint.y - size) + Frustum[i][C] * (CenterPoint.z - size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x + size) + Frustum[i][B] * (CenterPoint.y - size) + Frustum[i][C] * (CenterPoint.z - size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x - size) + Frustum[i][B] * (CenterPoint.y + size) + Frustum[i][C] * (CenterPoint.z - size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x + size) + Frustum[i][B] * (CenterPoint.y + size) + Frustum[i][C] * (CenterPoint.z - size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x - size) + Frustum[i][B] * (CenterPoint.y - size) + Frustum[i][C] * (CenterPoint.z + size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x + size) + Frustum[i][B] * (CenterPoint.y - size) + Frustum[i][C] * (CenterPoint.z + size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x - size) + Frustum[i][B] * (CenterPoint.y + size) + Frustum[i][C] * (CenterPoint.z + size) + Frustum[i][D] > 0)
		   continue;
		if(Frustum[i][A] * (CenterPoint.x + size) + Frustum[i][B] * (CenterPoint.y + size) + Frustum[i][C] * (CenterPoint.z + size) + Frustum[i][D] > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}



//-----------------------------------------------------------------------------
// This checks if a box is in the frustum
//-----------------------------------------------------------------------------
bool vw_BoxInFrustum(VECTOR3D MinPoint, VECTOR3D MaxPoint)
{
	// Go through all of the corners of the box and check then again each plane
	// in the frustum.  If all of them are behind one of the planes, then it most
	// like is not in the frustum.

	// *Note* - This will sometimes say that a box is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	// поставить доп проверку...
	for(int i = 0; i < 6; i++ )
	{
		if(Frustum[i][A] * MinPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MinPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MinPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MinPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MinPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MinPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] > 0)  continue;
		if(Frustum[i][A] * MaxPoint.x + Frustum[i][B] * MaxPoint.y + Frustum[i][C] * MaxPoint.z + Frustum[i][D] > 0)  continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	// Return a true for the box being inside of the frustum
	return true;
}
