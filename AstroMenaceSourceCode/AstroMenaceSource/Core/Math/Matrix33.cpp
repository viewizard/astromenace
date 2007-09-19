/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Matrix33.cpp

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


#include "Math.h"




//------------------------------------------------------------------------------------
// начальная установка
//------------------------------------------------------------------------------------
void Matrix33Identity(float Matrix33[9])
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





//------------------------------------------------------------------------------------
// Матрица, перемножение
//------------------------------------------------------------------------------------
void Matrix33Mult(float DstMatrix33[9], float SrcMatrix33[9])
{
	float tmp[9];
	memcpy(tmp, DstMatrix33, 9*sizeof(float));

	DstMatrix33[0] = SrcMatrix33[0]*tmp[0] + SrcMatrix33[1]*tmp[3] + SrcMatrix33[2]*tmp[6];
	DstMatrix33[1] = SrcMatrix33[0]*tmp[1] + SrcMatrix33[1]*tmp[4] + SrcMatrix33[2]*tmp[7];
	DstMatrix33[2] = SrcMatrix33[0]*tmp[2] + SrcMatrix33[1]*tmp[5] + SrcMatrix33[2]*tmp[8];

	DstMatrix33[3] = SrcMatrix33[3]*tmp[0] + SrcMatrix33[4]*tmp[3] + SrcMatrix33[5]*tmp[6];
	DstMatrix33[4] = SrcMatrix33[3]*tmp[1] + SrcMatrix33[4]*tmp[4] + SrcMatrix33[5]*tmp[7];
	DstMatrix33[5] = SrcMatrix33[3]*tmp[2] + SrcMatrix33[4]*tmp[5] + SrcMatrix33[5]*tmp[8];

	DstMatrix33[6] = SrcMatrix33[6]*tmp[0] + SrcMatrix33[7]*tmp[3] + SrcMatrix33[8]*tmp[6];
	DstMatrix33[7] = SrcMatrix33[6]*tmp[1] + SrcMatrix33[7]*tmp[4] + SrcMatrix33[8]*tmp[7];
	DstMatrix33[8] = SrcMatrix33[6]*tmp[2] + SrcMatrix33[7]*tmp[5] + SrcMatrix33[8]*tmp[8];
}










//------------------------------------------------------------------------------------
// Матрица, создаем матрицу поворота на углы Angle
//------------------------------------------------------------------------------------
void Matrix33CreateRotate(float Matrix33[9], VECTOR3D Angle)
{
	const float p180 = 0.0174532925f;

	// если угол только один - сюда идем
	if (Angle.z != 0.0f && Angle.x == 0.0f && Angle.y == 0.0f)
	{
		float a = Angle.z*p180;
		float c = cosf(a);
		float s = sinf(a);
		// ставим в нужные элементы
		Matrix33[0] = c;
		Matrix33[1] = -s;
		Matrix33[3] = s;
		Matrix33[4] = c;
		// делаем инициализацю
		Matrix33[2] = Matrix33[5] = Matrix33[6] = Matrix33[7] = 0.0f;
		Matrix33[8] = 1.0f;
		// выходим
		return;
	}

	if (Angle.y != 0.0f && Angle.x == 0.0f && Angle.z == 0.0f)
	{
		float a = Angle.y*p180;
		float c = cosf(a);
		float s = sinf(a);
		// ставим в нужные элементы
		Matrix33[0] = c;
		Matrix33[2] = s;
		Matrix33[6] = -s;
		Matrix33[8] = c;
		// делаем инициализацю
		Matrix33[1] = Matrix33[3] = Matrix33[5] = Matrix33[7] = 0.0f;
		Matrix33[4] = 1.0f;
		// выходим
		return;
	}

	if (Angle.x != 0.0f && Angle.y == 0.0f && Angle.z == 0.0f)
	{
		float a = Angle.x*p180;
		float c = cosf(a);
		float s = sinf(a);
		// ставим в нужные элементы
		Matrix33[4] = c;
		Matrix33[5] = -s;
		Matrix33[7] = s;
		Matrix33[8] = c;
		// делаем инициализацю
		Matrix33[1] = Matrix33[2] = Matrix33[3] = Matrix33[6] = 0.0f;
		Matrix33[0] = 1.0f;
		// выходим
		return;
	}


	// если 2 или более углов

	float a = Angle.x*p180;
	float A = cosf(a);
	float B = sinf(a);
	a = Angle.y*p180;
	float C = cosf(a);
	float D = sinf(a);
	a = Angle.z*p180;
	float E = cosf(a);
	float F = sinf(a);
	float AD = A * D;
	float BD = B * D;
	// эти формулы получили после оптимизации верхних 3-х преобразований
    Matrix33[0]  =   C * E;
    Matrix33[1]  =  -C * F;
    Matrix33[2]  =   D;
    Matrix33[3]  =  BD * E + A * F;
    Matrix33[4]  = -BD * F + A * E;
    Matrix33[5]  =  -B * C;
    Matrix33[6]  = -AD * E + B * F;
    Matrix33[7]  =  AD * F + B * E;
    Matrix33[8] =   A * C;
}








//------------------------------------------------------------------------------------
// Получение обратной матрицы поворота
//------------------------------------------------------------------------------------
void Matrix33InverseRotate(float Matrix33[9])
{
	float tmp[9];
	memcpy(tmp, Matrix33, 9*sizeof(float));

	// транспонируем
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








//------------------------------------------------------------------------------------
// Получаем точку по матрице трансформаций
//------------------------------------------------------------------------------------
void Matrix33CalcPoint(VECTOR3D *Point, float Matrix33[9])
{
	VECTOR3D TmpPoint = *Point;
	Point->x = Matrix33[0]*TmpPoint.x + Matrix33[3]*TmpPoint.y + Matrix33[6]*TmpPoint.z;
	Point->y = Matrix33[1]*TmpPoint.x + Matrix33[4]*TmpPoint.y + Matrix33[7]*TmpPoint.z;
	Point->z = Matrix33[2]*TmpPoint.x + Matrix33[5]*TmpPoint.y + Matrix33[8]*TmpPoint.z;
}




