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


#ifndef CoreMath_H
#define CoreMath_H


#include "../base.h"



inline void itoa10(int val, char* res)
{
	// http://www.cplusplus.com/ref/cstdio/sprintf.html
	// если нужны не 10-ки
	sprintf(res,"%i",val);
}


// Get cosine
double	vw_dcos(int Angle);
// Get sine
double	vw_dsin(int Angle);
// быстрый sqrtf
float	vw_sqrtf(float x);



// преобразуем утф8 в утф32, как результат возвращаем указатель на след утф8 символ
const char* utf8_to_utf32(const char* utf8, unsigned* utf32);




/* Random number generator */

/* Generate random float between 0.0f and 1.0f. */
float	vw_fRand();
#define vw_Randf1	vw_fRand()
/* Generate random float between 0.0f and Max. */
float	vw_fRandNum(float Max);
/* Generate random integer between 0.0f and Max. */
int	vw_iRandNum(int Max);
/* Generate random float between -1.0f and 1.0f. */
#define vw_Randf0	(vw_fRand() - vw_fRand())





// макрос, проверяет значение с учетом допустимых пределов
#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max);




// Нахождение максимального из 3
float Max3(float a1,float a2,float a3);
// Нахождение минимального из 3
float Min3(float a1,float a2,float a3);



// вектор в 3д пространстве
struct VECTOR3D {
	float x,y,z;

	VECTOR3D() {};
	VECTOR3D(float nX, float nY, float nZ)
	{
		x = nX;
		y = nY;
		z = nZ;
	};
	void Set(float nX, float nY, float nZ)
	{
		x = nX;
		y = nY;
		z = nZ;
	};
	~VECTOR3D() {};
	// Equal
	bool operator == (const VECTOR3D &A)
	{
		return (A.x == x &&
			A.y == y &&
			A.z == z);
	};
	// Not equal
	bool operator != (const VECTOR3D &A)
	{
		return (A.x != x ||
			A.y != y ||
			A.z != z);
	};
	// Multiply
	friend float operator * (const VECTOR3D &A, const VECTOR3D &B)
	{
		return A.x * B.x + A.y * B.y + A.z * B.z;
	};

	VECTOR3D operator ^ (const float C)
	{
		return VECTOR3D(x*C,y*C, z*C);
	};

	VECTOR3D operator / (const float C)
	{
		return VECTOR3D(x/C,y/C, z/C);
	};

	void operator *= (float C)
	{
		x *= C;
		y *= C;
		z *= C;
	};

	// Add
	VECTOR3D operator + (const VECTOR3D &A)
	{
		return VECTOR3D(x + A.x, y + A.y, z + A.z);
	};
	void operator += (const VECTOR3D &A)
	{
		x += A.x;
		y += A.y;
		z += A.z;
	};
	// Subtract
	VECTOR3D operator - (const VECTOR3D &A)
	{
		return VECTOR3D(x - A.x, y - A.y, z - A.z);
	};
	void operator -= (const VECTOR3D &A)
	{
		x -= A.x;
		y -= A.y;
		z -= A.z;
	};


	float Length();
	void Normalize();
	void NormalizeHi();
	void Multiply(VECTOR3D A);
};




// вращение точки на углы
void RotatePoint(VECTOR3D *Point, VECTOR3D Angle);
void RotatePointInv(VECTOR3D *Point, VECTOR3D Angle);
// получение данных плоскости по 3 -м точкам
void GetPlaneABCD(float *A, float *B, float *C, float *D, VECTOR3D Point1, VECTOR3D Point2, VECTOR3D Point3);






// матрица 4на4 (float Matrix[16])

// начальная установка
void Matrix44Identity(float Matrix44[16]);
// перемножение
void Matrix44Mult(float DstMatrix44[16], float SrcMatrix44[16]);
// перенос в точку Location
void Matrix44Translate(float Matrix44[16], VECTOR3D Location);
// создаем матрицу поворота на углы Angle
void Matrix44CreateRotate(float Matrix44[16], VECTOR3D Angle);
// Получение обратной матрицы поворота
void Matrix44InverseRotate(float Matrix44[16]);
// Получаем точку по матрице трансформаций
void Matrix44CalcPoint(VECTOR3D *Point, float Matrix44[16]);


// матрица 3на3 (float Matrix[9])

// начальная установка
void Matrix33Identity(float Matrix33[9]);
// перемножение
void Matrix33Mult(float DstMatrix33[9], float SrcMatrix33[9]);
// создаем матрицу поворота на углы Angle
void Matrix33CreateRotate(float Matrix33[9], VECTOR3D Angle);
// Получение обратной матрицы поворота
void Matrix33InverseRotate(float Matrix33[9]);
// Получаем точку по матрице трансформаций
void Matrix33CalcPoint(VECTOR3D *Point, float Matrix33[9]);




#endif // CoreMath_H

