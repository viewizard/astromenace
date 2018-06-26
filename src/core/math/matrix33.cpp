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

#include "math.h"

/*
 * Setup matrix identity.
 */
void vw_Matrix33Identity(float (&Matrix33)[9])
{
	Matrix33[0] = 1.0f;
	Matrix33[1] = 0.0f;
	Matrix33[2] = 0.0f;

	Matrix33[3] = 0.0f;
	Matrix33[4] = 1.0f;
	Matrix33[5] = 0.0f;

	Matrix33[6] = 0.0f;
	Matrix33[7] = 0.0f;
	Matrix33[8] = 1.0f;
}

/*
 * Matrix multiplication.
 */
void vw_Matrix33Mult(float (&DstMatrix33)[9], const float (&SrcMatrix33)[9])
{
	float tmp[9]{DstMatrix33[0], DstMatrix33[1], DstMatrix33[2],
		     DstMatrix33[3], DstMatrix33[4], DstMatrix33[5],
		     DstMatrix33[6], DstMatrix33[7], DstMatrix33[8]};

	DstMatrix33[0] = SrcMatrix33[0] * tmp[0] + SrcMatrix33[1] * tmp[3] + SrcMatrix33[2] * tmp[6];
	DstMatrix33[1] = SrcMatrix33[0] * tmp[1] + SrcMatrix33[1] * tmp[4] + SrcMatrix33[2] * tmp[7];
	DstMatrix33[2] = SrcMatrix33[0] * tmp[2] + SrcMatrix33[1] * tmp[5] + SrcMatrix33[2] * tmp[8];

	DstMatrix33[3] = SrcMatrix33[3] * tmp[0] + SrcMatrix33[4] * tmp[3] + SrcMatrix33[5] * tmp[6];
	DstMatrix33[4] = SrcMatrix33[3] * tmp[1] + SrcMatrix33[4] * tmp[4] + SrcMatrix33[5] * tmp[7];
	DstMatrix33[5] = SrcMatrix33[3] * tmp[2] + SrcMatrix33[4] * tmp[5] + SrcMatrix33[5] * tmp[8];

	DstMatrix33[6] = SrcMatrix33[6] * tmp[0] + SrcMatrix33[7] * tmp[3] + SrcMatrix33[8] * tmp[6];
	DstMatrix33[7] = SrcMatrix33[6] * tmp[1] + SrcMatrix33[7] * tmp[4] + SrcMatrix33[8] * tmp[7];
	DstMatrix33[8] = SrcMatrix33[6] * tmp[2] + SrcMatrix33[7] * tmp[5] + SrcMatrix33[8] * tmp[8];
}

/*
 * Create rotation matrix.
 */
void vw_Matrix33CreateRotate(float (&Matrix33)[9], const sVECTOR3D &Angle)
{
	constexpr float DegToRadFactor = 0.0174532925f; // conversion factor to convert degrees to radians

	if ((Angle.z != 0.0f) && (Angle.x == 0.0f) && (Angle.y == 0.0f)) {
		float a = -Angle.z * DegToRadFactor;
		float c = cosf(a);
		float s = sinf(a);
		Matrix33[0] = c;
		Matrix33[1] = -s;
		Matrix33[3] = s;
		Matrix33[4] = c;
		Matrix33[2] = Matrix33[5] = Matrix33[6] = Matrix33[7] = 0.0f;
		Matrix33[8] = 1.0f;
	} else if ((Angle.y != 0.0f) && (Angle.x == 0.0f) && (Angle.z == 0.0f)) {
		float a = -Angle.y * DegToRadFactor;
		float c = cosf(a);
		float s = sinf(a);
		Matrix33[0] = c;
		Matrix33[2] = s;
		Matrix33[6] = -s;
		Matrix33[8] = c;
		Matrix33[1] = Matrix33[3] = Matrix33[5] = Matrix33[7] = 0.0f;
		Matrix33[4] = 1.0f;
	} else if ((Angle.x != 0.0f) && (Angle.y == 0.0f) && (Angle.z == 0.0f)) {
		float a = -Angle.x * DegToRadFactor;
		float c = cosf(a);
		float s = sinf(a);
		Matrix33[4] = c;
		Matrix33[5] = -s;
		Matrix33[7] = s;
		Matrix33[8] = c;
		Matrix33[1] = Matrix33[2] = Matrix33[3] = Matrix33[6] = 0.0f;
		Matrix33[0] = 1.0f;
	} else {
		// if we need 2 or more angles
		float a = -Angle.x * DegToRadFactor;
		float A = cosf(a);
		float B = sinf(a);
		a = -Angle.y * DegToRadFactor;
		float C = cosf(a);
		float D = sinf(a);
		a = -Angle.z * DegToRadFactor;
		float E = cosf(a);
		float F = sinf(a);
		float AD = A * D;
		float BD = B * D;
		Matrix33[0]  =   C * E;
		Matrix33[1]  =  -C * F;
		Matrix33[2]  =   D;
		Matrix33[3]  =  BD * E + A * F;
		Matrix33[4]  = -BD * F + A * E;
		Matrix33[5]  =  -B * C;
		Matrix33[6]  = -AD * E + B * F;
		Matrix33[7]  =  AD * F + B * E;
		Matrix33[8] =    A * C;
	}
}

/*
 * Create inverted rotation matrix.
 */
void vw_Matrix33InverseRotate(float (&Matrix33)[9])
{
	float tmp[9]{Matrix33[0], Matrix33[1], Matrix33[2],
		     Matrix33[3], Matrix33[4], Matrix33[5],
		     Matrix33[6], Matrix33[7], Matrix33[8]};

	Matrix33[0] = tmp[0];
	Matrix33[1] = tmp[3];
	Matrix33[2] = tmp[6];

	Matrix33[3] = tmp[1];
	Matrix33[4] = tmp[4];
	Matrix33[5] = tmp[7];

	Matrix33[6] = tmp[2];
	Matrix33[7] = tmp[5];
	Matrix33[8] = tmp[8];
}

/*
 * Calculate point position by transformation matrix.
 */
void vw_Matrix33CalcPoint(sVECTOR3D &Point, const float (&Matrix33)[9])
{
	Point(Matrix33[0] * Point.x + Matrix33[3] * Point.y + Matrix33[6] * Point.z,
	      Matrix33[1] * Point.x + Matrix33[4] * Point.y + Matrix33[7] * Point.z,
	      Matrix33[2] * Point.x + Matrix33[5] * Point.y + Matrix33[8] * Point.z);
}
