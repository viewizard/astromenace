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

#ifndef CORE_MATH_MATH_H
#define CORE_MATH_MATH_H

#include "../base.h"

struct sVECTOR3D {
	float x{0.0f}, y{0.0f}, z{0.0f};

	sVECTOR3D() = default;
	explicit sVECTOR3D(float _x, float _y, float _z) :
		x{_x},
		y{_y},
		z{_z}
	{}
	void Set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	// Equal
	bool operator == (const sVECTOR3D &A) const
	{
		return ((A.x == x) &&
			(A.y == y) &&
			(A.z == z));
	}
	// Not equal
	bool operator != (const sVECTOR3D &A) const
	{
		return ((A.x != x) ||
			(A.y != y) ||
			(A.z != z));
	}
	// Multiply
	friend float operator * (const sVECTOR3D &A, const sVECTOR3D &B)
	{
		return A.x * B.x + A.y * B.y + A.z * B.z;
	}

	sVECTOR3D operator ^ (const float C) const
	{
		return sVECTOR3D{x * C, y * C, z * C};
	}

	sVECTOR3D operator / (const float C) const
	{
		return sVECTOR3D{x / C, y / C, z / C};
	}

	void operator () (const float _x, const float _y, const float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	void operator *= (float C)
	{
		x *= C;
		y *= C;
		z *= C;
	}

	// Add
	sVECTOR3D operator + (const sVECTOR3D &A) const
	{
		return sVECTOR3D{x + A.x, y + A.y, z + A.z};
	}
	void operator += (const sVECTOR3D &A)
	{
		x += A.x;
		y += A.y;
		z += A.z;
	}
	// Subtract
	sVECTOR3D operator - (const sVECTOR3D &A) const
	{
		return sVECTOR3D{x - A.x, y - A.y, z - A.z};
	}
	void operator -= (const sVECTOR3D &A)
	{
		x -= A.x;
		y -= A.y;
		z -= A.z;
	}

	float Length() const;
	void Normalize();
	void NormalizeHi();
	void Multiply(const sVECTOR3D &A);
};

/*
 * Misc functions.
 */

// Note, we use left-top as starting point (upper left is origin), this is why sRECT
// keeps together pairs "left-top" and "right-bottom".
struct sRECT {
	int left{0}, top{0}, right{0}, bottom{0};

	sRECT() = default;
	explicit sRECT(const int _left, const int _top, const int _right, const int _bottom) :
		left{_left},
		top{_top},
		right{_right},
		bottom{_bottom}
	{}
	void operator () (const int _left, const int _top, const int _right, const int _bottom)
	{
		left = _left;
		top = _top;
		right = _right;
		bottom = _bottom;
	}
};

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template <class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args) :
		Facet(std::forward<Args>(args)...)
	{}
	~deletable_facet()
	{}
};
// Convert utf8 to utf32: ConvertUTF8.from_bytes(utf8)
// Convert utf32 to utf8: ConvertUTF8.to_bytes(utf32)
extern std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> ConvertUTF8;

// Fast cosine function.
double vw_dcos(int Angle);
// Fast sine function.
double vw_dsin(int Angle);
// Fast sqrtf function.
float vw_sqrtf(float x);

// Reference to low if value is less than low, reference to high if high is less than value,
// otherwise reference to value.
// NOTE in future, move to std::clamp (since C++17) implementation
template <typename T>
inline void vw_Clamp(T &value, T low, T high)
{
	value = (value < low) ? low : ((value > high) ? high : value);
}

// Calculate point rotation.
void vw_RotatePoint(sVECTOR3D &Point, const sVECTOR3D &Angle);
// Calculate point inverse rotation.
void vw_RotatePointInv(sVECTOR3D &Point, const sVECTOR3D &Angle);
// Calculates the plane equation given three points.
void vw_GetPlaneABCD(float &A, float &B, float &C, float &D,
		     const sVECTOR3D &Point1, const sVECTOR3D &Point2, const sVECTOR3D &Point3);

// Check file extension (for extensions with ASCII characters).
inline bool vw_CheckFileExtension(const std::string &FileName, const std::string &Extension)
{
	if (Extension.size() > FileName.size())
		return false;

	auto predicate = [] (unsigned char a, unsigned char b) {
		return (std::tolower(a) == std::tolower(b));
	};

	std::string FileExt = FileName.substr(FileName.size() - Extension.size());

	return std::equal(FileExt.begin(), FileExt.end(), Extension.begin(), predicate);
}

/*
 * Random number generator.
 */

// Generate random float in range [0.0f, 1.0f).
float vw_fRand();
// Generate random float in range [0.0f, Max).
float vw_fRandNum(float Max);
// Generate random integer in range [0, Max].
int vw_iRandNum(int Max);
// Generate random float in range (-1.0f, 1.0f).
#define vw_Randf0 (vw_fRand() - vw_fRand())

/*
 * 4x4 matrix, float Matrix[16]:
 *
 *  Matrix[0]  Matrix[1]  Matrix[2]  Matrix[3]
 *  Matrix[4]  Matrix[5]  Matrix[6]  Matrix[7]
 *  Matrix[8]  Matrix[9]  Matrix[10] Matrix[11]
 *  Matrix[12] Matrix[13] Matrix[14] Matrix[15]
 *
 */

// Setup matrix identity.
void vw_Matrix44Identity(float (&Matrix44)[16]);
// Matrix multiplication.
void vw_Matrix44Mult(float (&DstMatrix44)[16], const float (&SrcMatrix44)[16]);
// Calculate translation matrix by new location point.
void vw_Matrix44Translate(float (&Matrix44)[16], const sVECTOR3D &Location);
// Create rotation matrix.
void vw_Matrix44CreateRotate(float (&Matrix44)[16], const sVECTOR3D &Angle);
// Create inverted rotation matrix.
void vw_Matrix44InverseRotate(float (&Matrix44)[16]);
// Calculate point position by transformation matrix.
void vw_Matrix44CalcPoint(sVECTOR3D &Point, const float (&Matrix44)[16]);

/*
 * 3x3 matrix, float Matrix[9]:
 *
 *  Matrix[0] Matrix[1] Matrix[2]
 *  Matrix[3] Matrix[4] Matrix[5]
 *  Matrix[6] Matrix[7] Matrix[8]
 *
 */

// Setup matrix identity.
void vw_Matrix33Identity(float (&Matrix33)[9]);
// Matrix multiplication.
void vw_Matrix33Mult(float (&DstMatrix33)[9], const float (&SrcMatrix33)[9]);
// Create rotation matrix.
void vw_Matrix33CreateRotate(float (&Matrix33)[9], const sVECTOR3D &Angle);
// Create inverted rotation matrix.
void vw_Matrix33InverseRotate(float (&Matrix33)[9]);
// Calculate point position by transformation matrix.
void vw_Matrix33CalcPoint(sVECTOR3D &Point, const float (&Matrix33)[9]);

#endif // CORE_MATH_MATH_H
