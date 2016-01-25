/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "CollisionDetection.h"




//-----------------------------------------------------------------------------
// Проверка столкновений OBB-OBB
//-----------------------------------------------------------------------------
bool vw_OBBOBBCollision(VECTOR3D Object1OBB[8], VECTOR3D Object1OBBLocation, VECTOR3D Object1Location, float Object1RotationMatrix[9],
						VECTOR3D Object2OBB[8], VECTOR3D Object2OBBLocation, VECTOR3D Object2Location, float Object2RotationMatrix[9])
{
	// строим матрицу, чтобы развернуть точки
	float TMPInvObject1RotationMatrix[9];
	memcpy(TMPInvObject1RotationMatrix, Object1RotationMatrix, 9*sizeof(float));
	Matrix33InverseRotate(TMPInvObject1RotationMatrix);
	// размер первого бокса
	VECTOR3D a = (Object1OBB[0] - Object1OBB[6])^0.5f;
	Matrix33CalcPoint(&a, TMPInvObject1RotationMatrix);

	// строим инверсную матрицу для 2-го объекта
	float TMPInvObject2RotationMatrix[9];
	memcpy(TMPInvObject2RotationMatrix, Object2RotationMatrix, 9*sizeof(float));
	Matrix33InverseRotate(TMPInvObject2RotationMatrix);
	// размер второго бокса
	VECTOR3D b = (Object2OBB[0] - Object2OBB[6])^0.5f;
	Matrix33CalcPoint(&b, TMPInvObject2RotationMatrix);


	// получаем смещение в мировой системе координат
	VECTOR3D T = (Object2Location + Object2OBBLocation)
					 - (Object1Location + Object1OBBLocation);
	Matrix33CalcPoint(&T, TMPInvObject1RotationMatrix);

	// строем матрицу для переноса точек 2-го объекта в систему координат 1-го
	Matrix33Mult(TMPInvObject1RotationMatrix, Object2RotationMatrix);
	float R[3][3];
	R[0][0] = TMPInvObject1RotationMatrix[0];
	R[0][1] = TMPInvObject1RotationMatrix[3];
	R[0][2] = TMPInvObject1RotationMatrix[6];
	R[1][0] = TMPInvObject1RotationMatrix[1];
	R[1][1] = TMPInvObject1RotationMatrix[4];
	R[1][2] = TMPInvObject1RotationMatrix[7];
	R[2][0] = TMPInvObject1RotationMatrix[2];
	R[2][1] = TMPInvObject1RotationMatrix[5];
	R[2][2] = TMPInvObject1RotationMatrix[8];


	//делаем 15 проверок, т.к. у нас 15 разделяющих осей

	//1 (Ra)x
	if(fabsf(T.x) > a.x + b.x * fabsf(R[0][0]) + b.y * fabsf(R[0][1]) + b.z * fabsf(R[0][2]))
		return false;
	//2 (Ra)y
	if(fabsf(T.y) > a.y + b.x * fabsf(R[1][0]) + b.y * fabsf(R[1][1]) + b.z * fabsf(R[1][2]))
		return false;
	//3 (Ra)z
	if(fabsf(T.z) > a.z + b.x * fabsf(R[2][0]) + b.y * fabsf(R[2][1]) + b.z * fabsf(R[2][2]))
		return false;

	//4 (Rb)x
	if(fabsf(T.x*R[0][0] + T.y*R[1][0] + T.z*R[2][0]) >
		(b.x + a.x*fabsf(R[0][0]) + a.y * fabsf(R[1][0]) + a.z*fabsf(R[2][0])))
		return false;
	//5 (Rb)y
	if(fabsf(T.x*R[0][1] + T.y*R[1][1] + T.z*R[2][1]) >
		(b.y + a.x*fabsf(R[0][1]) + a.y * fabsf(R[1][1]) + a.z*fabsf(R[2][1])))
		return false;
	//6 (Rb)z
	if(fabsf(T.x*R[0][2] + T.y*R[1][2] + T.z*R[2][2]) >
		(b.z + a.x*fabsf(R[0][2]) + a.y * fabsf(R[1][2]) + a.z*fabsf(R[2][2])))
		return false;

	//7 (Ra)x X (Rb)x
	if(fabsf(T.z*R[1][0] - T.y*R[2][0]) > a.y*fabsf(R[2][0]) +
		a.z*fabsf(R[1][0]) + b.y*fabsf(R[0][2]) + b.z*fabsf(R[0][1]))
		return false;
	//8 (Ra)x X (Rb)y
	if(fabsf(T.z*R[1][1] - T.y*R[2][1]) > a.y*fabsf(R[2][1]) +
		a.z*fabsf(R[1][1]) + b.x*fabsf(R[0][2]) + b.z*fabsf(R[0][0]))
		return false;
	//9 (Ra)x X (Rb)z
	if(fabsf(T.z*R[1][2]-T.y*R[2][2]) > a.y*fabsf(R[2][2]) +
		a.z*fabsf(R[1][2]) + b.x*fabsf(R[0][1]) + b.y*fabsf(R[0][0]))
		return false;
	//10 (Ra)y X (Rb)x
	if(fabsf(T.x*R[2][0]-T.z*R[0][0]) > a.x*fabsf(R[2][0]) +
		a.z*fabsf(R[0][0]) + b.y*fabsf(R[1][2]) + b.z*fabsf(R[1][1]))
		return false;
	//11 (Ra)y X (Rb)y
	if(fabsf(T.x*R[2][1]-T.z*R[0][1]) > a.x*fabsf(R[2][1]) +
		a.z*fabsf(R[0][1]) + b.x*fabsf(R[1][2]) + b.z*fabsf(R[1][0]))
		return false;
	//12 (Ra)y X (Rb)z
	if(fabsf(T.x*R[2][2]-T.z*R[0][2]) > a.x*fabsf(R[2][2]) +
		a.z*fabsf(R[0][2]) + b.x*fabsf(R[1][1]) + b.y*fabsf(R[1][0]))
		return false;
	//13 (Ra)z X (Rb)x
	if(fabsf(T.y*R[0][0]-T.x*R[1][0]) > a.x*fabsf(R[1][0]) +
		a.y*fabsf(R[0][0]) + b.y*fabsf(R[2][2]) + b.z*fabsf(R[2][1]))
		return false;
	//14 (Ra)z X (Rb)y
	if(fabsf(T.y*R[0][1]-T.x*R[1][1]) > a.x*fabsf(R[1][1]) +
		a.y*fabsf(R[0][1]) + b.x*fabsf(R[2][2]) + b.z*fabsf(R[2][0]))
		return false;
	//15 (Ra)z X (Rb)z
	if(fabsf(T.y*R[0][2]-T.x*R[1][2]) > a.x*fabsf(R[1][2]) +
		a.y*fabsf(R[0][2]) + b.x*fabsf(R[2][1]) + b.y*fabsf(R[2][0]))
		return false;

	return true;
}
