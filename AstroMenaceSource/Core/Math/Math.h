/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Math.h

	Copyright (c) 2001-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#ifndef CoreMath_H
#define CoreMath_H


#include "../Base.h"



// Get cosine
double	vw_dcos(int Angle);
// Get sine
double	vw_dsin(int Angle);
// быстрый sqrtf
float	vw_sqrtf(float x);



// быстрое сравнение строк (по аналогии с strcmp)
int		vw_strcmp(const char *a, const char *b);


// преобразуем утф8 в утф32, как результат возвращаем указатель на след утф8 символ
const char* utf8_to_utf32(const char* utf8, unsigned* utf32);


// rand
float	vw_Rand();
float   vw_RandNum(float Max);
int		vw_iRandNum(int Max);
// RANDOM_NUM возвращает значение -1.0f - 1.0f, стремится к нулю (!)
#define vw_Randf0      ((vw_Rand()-vw_Rand())/RAND_MAX)
// RANDOM_NUM2 возвращает значение 0.0f - 1.0f
#define vw_Randf1		(vw_Rand()/RAND_MAX)	//(fmodf(vw_Rand(),100.0f)/100.0f)
// Полезный макрос, гарантирует что в пределах
#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max);



// Нахождение максимального их 3
float Max3(float a1,float a2,float a3);
// Нахождение минимального их 3
float Min3(float a1,float a2,float a3);



// вектор в 3д пространстве
struct VECTOR3D
{
	float x,y,z;

	VECTOR3D(){};
	VECTOR3D(float nX, float nY, float nZ) { x = nX; y = nY; z = nZ; };
	~VECTOR3D(){};
	// Equal
	bool operator == (const VECTOR3D &A)
	{ return (A.x == x &&
			A.y == y &&
			A.z == z); };
	// Not equal
	bool operator != (const VECTOR3D &A)
	{ return (A.x != x ||
			A.y != y ||
			A.z != z); };
	// Multiply
	friend float operator * (const VECTOR3D &A, const VECTOR3D &B)
	{ return A.x * B.x + A.y * B.y + A.z * B.z; };

	VECTOR3D operator ^ (const float C)
	{ return VECTOR3D(x*C,y*C, z*C); };

	VECTOR3D operator / (const float C)
	{ return VECTOR3D(x/C,y/C, z/C); };

	void operator *= (float C)
	{ x *= C; y *= C; z *= C; };

	// Add
	VECTOR3D operator + (const VECTOR3D &A)
	{ return VECTOR3D(x + A.x, y + A.y, z + A.z); };
	void operator += (const VECTOR3D &A)
	{ x += A.x; y += A.y; z += A.z; };
	// Subtract
	VECTOR3D operator - (const VECTOR3D &A)
	{ return VECTOR3D(x - A.x, y - A.y, z - A.z); };
	void operator -= (const VECTOR3D &A)
	{ x -= A.x; y -= A.y; z -= A.z; };


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

