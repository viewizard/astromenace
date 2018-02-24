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


/// подключаем нужные файлы
#include "object3d.h"






//-----------------------------------------------------------------------------
// Проверка столкновений Sphere-Mesh
//-----------------------------------------------------------------------------
bool CheckMeshSphereCollisionDetection(CObject3D *Object1, CObject3D *Object2, VECTOR3D *NewLoc, int *Object1PieceNum)
{
	if (Object1->DrawObjectList == nullptr)
		return false;


	for (int j = 0; j < Object1->DrawObjectQuantity; j++)
		if (Object1->DrawObjectList[j].VertexCount != 0) {

			if (Object1->HitBB != nullptr) {
				float Distance2 = (Object1->Location.x + Object1->HitBBLocation[j].x - Object2->Location.x)*(Object1->Location.x + Object1->HitBBLocation[j].x - Object2->Location.x) +
						  (Object1->Location.y + Object1->HitBBLocation[j].y - Object2->Location.y)*(Object1->Location.y + Object1->HitBBLocation[j].y - Object2->Location.y) +
						  (Object1->Location.z + Object1->HitBBLocation[j].z - Object2->Location.z)*(Object1->Location.z + Object1->HitBBLocation[j].z - Object2->Location.z);

				// если очень далеко - берем следующий HitBB
				// но сначала делаем еще одну проверку
				if (Distance2 > Object1->HitBBRadius2[j] + Object2->Radius*Object2->Radius) {
					// средняя точка линии
					VECTOR3D mid = (Object2->Location + Object2->PrevLocation) / 2.0f;
					// направление линии
					VECTOR3D dir = Object2->Location - Object2->PrevLocation;
					// полудлина линии
					float hl = dir.Length()/2.0f;
					dir.Normalize();

					VECTOR3D T = Object1->Location + Object1->HitBBLocation[j] - mid;
					float r;

					// проверяем, является ли одна из осей X,Y,Z разделяющей
					if ( (fabs(T.x) > Object1->HitBB[j][0].x + hl*fabs(dir.x)) ||
					     (fabs(T.y) > Object1->HitBB[j][0].y + hl*fabs(dir.y)) ||
					     (fabs(T.z) > Object1->HitBB[j][0].z + hl*fabs(dir.z)) )
						continue;


					// проверяем X ^ dir
					r = Object1->HitBB[j][0].y*fabs(dir.z) + Object1->HitBB[j][0].z*fabs(dir.y);
					if ( fabs(T.y*dir.z - T.z*dir.y) > r )
						continue;

					// проверяем Y ^ dir
					r = Object1->HitBB[j][0].x*fabs(dir.z) + Object1->HitBB[j][0].z*fabs(dir.x);
					if ( fabs(T.z*dir.x - T.x*dir.z) > r )
						continue;

					// проверяем Z ^ dir
					r = Object1->HitBB[j][0].x*fabs(dir.y) + Object1->HitBB[j][0].y*fabs(dir.x);
					if ( fabs(T.x*dir.y - T.y*dir.x) > r )
						continue;
				}
			}



			// дальше работаем с геометрией

			if(vw_SphereMeshCollision(Object1->Location, &(Object1->DrawObjectList[j]), Object1->CurrentRotationMat,
						  Object2->Radius, Object2->Location, Object2->PrevLocation,
						  NewLoc)) {
				*Object1PieceNum = j;
				return true;
			}
		}

	return false;
}









//-----------------------------------------------------------------------------
// Проверка столкновений HitBB-HitBB
//-----------------------------------------------------------------------------
bool CheckHitBBHitBBCollisionDetection(CObject3D *Object1, CObject3D *Object2, int *Object1PieceNum, int *Object2PieceNum)
{
	// проверяем HitBB, находим номера пересекающихся
	for (int i=0; i<Object1->DrawObjectQuantity; i++)
		if (Object1->DrawObjectList[i].VertexCount != 0) {

			for (int j=0; j<Object2->DrawObjectQuantity; j++)
				if (Object2->DrawObjectList[j].VertexCount != 0) {

					// находим расстояние между HitBB-ми
					float Distance2 = (Object1->Location.x + Object1->HitBBLocation[i].x - Object2->Location.x - Object2->HitBBLocation[j].x)*
							  (Object1->Location.x + Object1->HitBBLocation[i].x - Object2->Location.x - Object2->HitBBLocation[j].x) +
							  (Object1->Location.y + Object1->HitBBLocation[i].y - Object2->Location.y - Object2->HitBBLocation[j].y)*
							  (Object1->Location.y + Object1->HitBBLocation[i].y - Object2->Location.y - Object2->HitBBLocation[j].y) +
							  (Object1->Location.z + Object1->HitBBLocation[i].z - Object2->Location.z - Object2->HitBBLocation[j].z)*
							  (Object1->Location.z + Object1->HitBBLocation[i].z - Object2->Location.z - Object2->HitBBLocation[j].z);


					// если очень далеко - берем следующий HitBB
					if (Distance2 > Object2->HitBBRadius2[j] + Object1->HitBBRadius2[i]) continue;

					// если достаточно близко - проверяем как OBB


					// строим матрицу, чтобы развернуть точки
					float TMPOldInvRotationMat[9];
					memcpy(TMPOldInvRotationMat, Object1->CurrentRotationMat, 9*sizeof(float));
					vw_Matrix33InverseRotate(TMPOldInvRotationMat);

					float matB[9];
					memcpy(matB, Object2->CurrentRotationMat, 9*sizeof(float));
					vw_Matrix33Mult(matB, TMPOldInvRotationMat);



					VECTOR3D vPosB = (Object2->Location + Object2->HitBBLocation[j])
							 - (Object1->Location + Object1->HitBBLocation[i]);
					vw_Matrix33CalcPoint(&vPosB, TMPOldInvRotationMat);


					VECTOR3D XAxis(matB[0],matB[3],matB[6]);
					VECTOR3D YAxis(matB[1],matB[4],matB[7]);
					VECTOR3D ZAxis(matB[2],matB[5],matB[8]);


					VECTOR3D Obj1_data(Object1->HitBBSize[i].x/2.0f, Object1->HitBBSize[i].y/2.0f, Object1->HitBBSize[i].z/2.0f);
					VECTOR3D Obj2_data(Object2->HitBBSize[j].x/2.0f, Object2->HitBBSize[j].y/2.0f, Object2->HitBBSize[j].z/2.0f);



					//делаем 15 проверок, т.к. у нас 15 разделяющих осей

					//1 (Ra)x
					if(fabsf(vPosB.x) > Obj1_data.x + Obj2_data.x * fabsf(XAxis.x) + Obj2_data.y * fabsf(XAxis.y) + Obj2_data.z * fabsf(XAxis.z))
						continue;

					//2 (Ra)y
					if(fabsf(vPosB.y) > Obj1_data.y + Obj2_data.x * fabsf(YAxis.x) + Obj2_data.y * fabsf(YAxis.y) + Obj2_data.z * fabsf(YAxis.z))
						continue;

					//3 (Ra)z
					if(fabsf(vPosB.z) > Obj1_data.z + Obj2_data.x * fabsf(ZAxis.x) + Obj2_data.y * fabsf(ZAxis.y) + Obj2_data.z * fabsf(ZAxis.z))
						continue;


					//4 (Rb)x
					if(fabsf(vPosB.x*XAxis.x+vPosB.y*YAxis.x+vPosB.z*ZAxis.x) >
					    (Obj2_data.x+Obj1_data.x*fabsf(XAxis.x) + Obj1_data.y * fabsf(YAxis.x) + Obj1_data.z*fabsf(ZAxis.x)))
						continue;

					//5 (Rb)y
					if(fabsf(vPosB.x*XAxis.y+vPosB.y*YAxis.y+vPosB.z*ZAxis.y) >
					    (Obj2_data.y+Obj1_data.x*fabsf(XAxis.y) + Obj1_data.y * fabsf(YAxis.y) + Obj1_data.z*fabsf(ZAxis.y)))
						continue;

					//6 (Rb)z
					if(fabsf(vPosB.x*XAxis.z+vPosB.y*YAxis.z+vPosB.z*ZAxis.z) >
					    (Obj2_data.z+Obj1_data.x*fabsf(XAxis.z) + Obj1_data.y * fabsf(YAxis.z) + Obj1_data.z*fabsf(ZAxis.z)))
						continue;

					//7 (Ra)x X (Rb)x
					if(fabsf(vPosB.z*YAxis.x-vPosB.y*ZAxis.x) > Obj1_data.y*fabsf(ZAxis.x) +
					    Obj1_data.z*fabsf(YAxis.x) + Obj2_data.y*fabsf(XAxis.z) + Obj2_data.z*fabsf(XAxis.y))
						continue;

					//8 (Ra)x X (Rb)y
					if(fabsf(vPosB.z*YAxis.y-vPosB.y*ZAxis.y) > Obj1_data.y*fabsf(ZAxis.y) +
					    Obj1_data.z*fabsf(YAxis.y) + Obj2_data.x*fabsf(XAxis.z) + Obj2_data.z*fabsf(XAxis.x))
						continue;

					//9 (Ra)x X (Rb)z
					if(fabsf(vPosB.z*YAxis.z-vPosB.y*ZAxis.z) > Obj1_data.y*fabsf(ZAxis.z) +
					    Obj1_data.z*fabsf(YAxis.z) + Obj2_data.x*fabsf(XAxis.y) + Obj2_data.y*fabsf(XAxis.x))
						continue;

					//10 (Ra)y X (Rb)x
					if(fabsf(vPosB.x*ZAxis.x-vPosB.z*XAxis.x) > Obj1_data.x*fabsf(ZAxis.x) +
					    Obj1_data.z*fabsf(XAxis.x) + Obj2_data.y*fabsf(YAxis.z) + Obj2_data.z*fabsf(YAxis.y))
						continue;

					//11 (Ra)y X (Rb)y
					if(fabsf(vPosB.x*ZAxis.y-vPosB.z*XAxis.y) > Obj1_data.x*fabsf(ZAxis.y) +
					    Obj1_data.z*fabsf(XAxis.y) + Obj2_data.x*fabsf(YAxis.z) + Obj2_data.z*fabsf(YAxis.x))
						continue;

					//12 (Ra)y X (Rb)z
					if(fabsf(vPosB.x*ZAxis.z-vPosB.z*XAxis.z) > Obj1_data.x*fabsf(ZAxis.z) +
					    Obj1_data.z*fabsf(XAxis.z) + Obj2_data.x*fabsf(YAxis.y) + Obj2_data.y*fabsf(YAxis.x))
						continue;

					//13 (Ra)z X (Rb)x
					if(fabsf(vPosB.y*XAxis.x-vPosB.x*YAxis.x) > Obj1_data.x*fabsf(YAxis.x) +
					    Obj1_data.y*fabsf(XAxis.x) + Obj2_data.y*fabsf(ZAxis.z) + Obj2_data.z*fabsf(ZAxis.y))
						continue;

					//14 (Ra)z X (Rb)y
					if(fabsf(vPosB.y*XAxis.y-vPosB.x*YAxis.y) > Obj1_data.x*fabsf(YAxis.y) +
					    Obj1_data.y*fabsf(XAxis.y) + Obj2_data.x*fabsf(ZAxis.z) + Obj2_data.z*fabsf(ZAxis.x))
						continue;

					//15 (Ra)z X (Rb)z
					if(fabsf(vPosB.y*XAxis.z-vPosB.x*YAxis.z) > Obj1_data.x*fabsf(YAxis.z) +
					    Obj1_data.y*fabsf(XAxis.z) + Obj2_data.x*fabsf(ZAxis.y) + Obj2_data.y*fabsf(ZAxis.x))
						continue;


					// если тут, нашли пересекающиеся!

					*Object1PieceNum = i;
					*Object2PieceNum = j;

					return true;
				}


		}


	return false;
}






//-----------------------------------------------------------------------------
// Проверка столкновений HitBB-OBB
//-----------------------------------------------------------------------------
bool CheckHitBBOBBCollisionDetection(CObject3D *Object1, CObject3D *Object2, int *Object1PieceNum)
{
	// проверяем HitBB, находим номера пересекающихся
	for (int i=0; i<Object1->DrawObjectQuantity; i++)
		if (Object1->DrawObjectList[i].VertexCount != 0) {

			// строим матрицу, чтобы развернуть точки
			float TMPOldInvRotationMat[9];
			memcpy(TMPOldInvRotationMat, Object2->CurrentRotationMat, 9*sizeof(float));
			vw_Matrix33InverseRotate(TMPOldInvRotationMat);

			float matB[9];
			memcpy(matB, Object1->CurrentRotationMat, 9*sizeof(float));
			vw_Matrix33Mult(matB, TMPOldInvRotationMat);


			VECTOR3D vPosB = (Object1->Location + Object1->HitBBLocation[i])-(Object2->Location + Object2->OBBLocation);
			vw_Matrix33CalcPoint(&vPosB, TMPOldInvRotationMat);


			VECTOR3D XAxis(matB[0],matB[3],matB[6]);
			VECTOR3D YAxis(matB[1],matB[4],matB[7]);
			VECTOR3D ZAxis(matB[2],matB[5],matB[8]);


			VECTOR3D Obj2_data(Object1->HitBBSize[i].x/2.0f, Object1->HitBBSize[i].y/2.0f, Object1->HitBBSize[i].z/2.0f);
			VECTOR3D Obj1_data(Object2->Width/2.0f, Object2->Height/2.0f, Object2->Length/2.0f);



			//делаем 15 проверок, т.к. у нас 15 разделяющих осей

			//1 (Ra)x
			if(fabsf(vPosB.x) > Obj1_data.x + Obj2_data.x * fabsf(XAxis.x) + Obj2_data.y * fabsf(XAxis.y) + Obj2_data.z * fabsf(XAxis.z))
				continue;

			//2 (Ra)y
			if(fabsf(vPosB.y) > Obj1_data.y + Obj2_data.x * fabsf(YAxis.x) + Obj2_data.y * fabsf(YAxis.y) + Obj2_data.z * fabsf(YAxis.z))
				continue;

			//3 (Ra)z
			if(fabsf(vPosB.z) > Obj1_data.z + Obj2_data.x * fabsf(ZAxis.x) + Obj2_data.y * fabsf(ZAxis.y) + Obj2_data.z * fabsf(ZAxis.z))
				continue;

			//4 (Rb)x
			if(fabsf(vPosB.x*XAxis.x+vPosB.y*YAxis.x+vPosB.z*ZAxis.x) >
			    (Obj2_data.x+Obj1_data.x*fabsf(XAxis.x) + Obj1_data.y * fabsf(YAxis.x) + Obj1_data.z*fabsf(ZAxis.x)))
				continue;

			//5 (Rb)y
			if(fabsf(vPosB.x*XAxis.y+vPosB.y*YAxis.y+vPosB.z*ZAxis.y) >
			    (Obj2_data.y+Obj1_data.x*fabsf(XAxis.y) + Obj1_data.y * fabsf(YAxis.y) + Obj1_data.z*fabsf(ZAxis.y)))
				continue;

			//6 (Rb)z
			if(fabsf(vPosB.x*XAxis.z+vPosB.y*YAxis.z+vPosB.z*ZAxis.z) >
			    (Obj2_data.z+Obj1_data.x*fabsf(XAxis.z) + Obj1_data.y * fabsf(YAxis.z) + Obj1_data.z*fabsf(ZAxis.z)))
				continue;

			//7 (Ra)x X (Rb)x
			if(fabsf(vPosB.z*YAxis.x-vPosB.y*ZAxis.x) > Obj1_data.y*fabsf(ZAxis.x) +
			    Obj1_data.z*fabsf(YAxis.x) + Obj2_data.y*fabsf(XAxis.z) + Obj2_data.z*fabsf(XAxis.y))
				continue;

			//8 (Ra)x X (Rb)y
			if(fabsf(vPosB.z*YAxis.y-vPosB.y*ZAxis.y) > Obj1_data.y*fabsf(ZAxis.y) +
			    Obj1_data.z*fabsf(YAxis.y) + Obj2_data.x*fabsf(XAxis.z) + Obj2_data.z*fabsf(XAxis.x))
				continue;

			//9 (Ra)x X (Rb)z
			if(fabsf(vPosB.z*YAxis.z-vPosB.y*ZAxis.z) > Obj1_data.y*fabsf(ZAxis.z) +
			    Obj1_data.z*fabsf(YAxis.z) + Obj2_data.x*fabsf(XAxis.y) + Obj2_data.y*fabsf(XAxis.x))
				continue;

			//10 (Ra)y X (Rb)x
			if(fabsf(vPosB.x*ZAxis.x-vPosB.z*XAxis.x) > Obj1_data.x*fabsf(ZAxis.x) +
			    Obj1_data.z*fabsf(XAxis.x) + Obj2_data.y*fabsf(YAxis.z) + Obj2_data.z*fabsf(YAxis.y))
				continue;

			//11 (Ra)y X (Rb)y
			if(fabsf(vPosB.x*ZAxis.y-vPosB.z*XAxis.y) > Obj1_data.x*fabsf(ZAxis.y) +
			    Obj1_data.z*fabsf(XAxis.y) + Obj2_data.x*fabsf(YAxis.z) + Obj2_data.z*fabsf(YAxis.x))
				continue;

			//12 (Ra)y X (Rb)z
			if(fabsf(vPosB.x*ZAxis.z-vPosB.z*XAxis.z) > Obj1_data.x*fabsf(ZAxis.z) +
			    Obj1_data.z*fabsf(XAxis.z) + Obj2_data.x*fabsf(YAxis.y) + Obj2_data.y*fabsf(YAxis.x))
				continue;

			//13 (Ra)z X (Rb)x
			if(fabsf(vPosB.y*XAxis.x-vPosB.x*YAxis.x) > Obj1_data.x*fabsf(YAxis.x) +
			    Obj1_data.y*fabsf(XAxis.x) + Obj2_data.y*fabsf(ZAxis.z) + Obj2_data.z*fabsf(ZAxis.y))
				continue;

			//14 (Ra)z X (Rb)y
			if(fabsf(vPosB.y*XAxis.y-vPosB.x*YAxis.y) > Obj1_data.x*fabsf(YAxis.y) +
			    Obj1_data.y*fabsf(XAxis.y) + Obj2_data.x*fabsf(ZAxis.z) + Obj2_data.z*fabsf(ZAxis.x))
				continue;

			//15 (Ra)z X (Rb)z
			if(fabsf(vPosB.y*XAxis.z-vPosB.x*YAxis.z) > Obj1_data.x*fabsf(YAxis.z) +
			    Obj1_data.y*fabsf(XAxis.z) + Obj2_data.x*fabsf(ZAxis.y) + Obj2_data.y*fabsf(ZAxis.x))
				continue;

			// если тут, нашли пересекающиеся!
			*Object1PieceNum = i;
			return true;
		}

	return false;
}

















//-----------------------------------------------------------------------------
// Проверка столкновений геометрия больших астероидов -HitBB
//-----------------------------------------------------------------------------
bool CheckHitBBMeshCollisionDetection(CObject3D *Object1, CObject3D *Object2, int *Object1PieceNum)
{
	// делаем матрицу перемещения точки, для геометрии
	float TransMat[16];
	TransMat[0] = Object2->CurrentRotationMat[0];
	TransMat[1] = Object2->CurrentRotationMat[1];
	TransMat[2] = Object2->CurrentRotationMat[2];
	TransMat[3] = 0.0f;
	TransMat[4] = Object2->CurrentRotationMat[3];
	TransMat[5] = Object2->CurrentRotationMat[4];
	TransMat[6] = Object2->CurrentRotationMat[5];
	TransMat[7] = 0.0f;
	TransMat[8] = Object2->CurrentRotationMat[6];
	TransMat[9] = Object2->CurrentRotationMat[7];
	TransMat[10] = Object2->CurrentRotationMat[8];
	TransMat[11] = 0.0f;
	TransMat[12] = Object2->Location.x;
	TransMat[13] = Object2->Location.y;
	TransMat[14] = Object2->Location.z;
	TransMat[15] = 1.0f;



	// проверяем HitBB, находим номера пересекающихся
	for (int i=0; i<Object1->DrawObjectQuantity; i++)
		if (Object1->DrawObjectList[i].VertexCount != 0) {

			// параметры HitBB
			VECTOR3D TMPMax = Object1->HitBB[i][0];
			VECTOR3D TMPMin = Object1->HitBB[i][6];

			// строим матрицу, чтобы развернуть точки
			float TMPOldInvRotationMat[9];
			memcpy(TMPOldInvRotationMat, Object1->CurrentRotationMat, 9*sizeof(float));
			vw_Matrix33InverseRotate(TMPOldInvRotationMat);

			// перемещаем в общие координаты, теперь это как-бы крайние точки аабб
			vw_Matrix33CalcPoint(&TMPMax, TMPOldInvRotationMat);
			vw_Matrix33CalcPoint(&TMPMin, TMPOldInvRotationMat);


			VECTOR3D Center = Object1->HitBBLocation[i] + Object1->Location;
			vw_Matrix33CalcPoint(&Center, TMPOldInvRotationMat);


			VECTOR3D TMPMax2;
			VECTOR3D TMPMin2;
			VECTOR3D Point1;
			VECTOR3D Point2;
			VECTOR3D Point3;


			// проверяем все треугольники объекта
			for (int j=0; j<Object2->DrawObjectQuantity; j++)
				if (Object2->DrawObjectList[j].VertexCount != 0) {

					// дальше работаем с геометрией


					// находим точку локального положения объекта в моделе
					VECTOR3D LocalLocation(Object2->DrawObjectList[j].Location);
					vw_Matrix33CalcPoint(&LocalLocation, Object2->CurrentRotationMat);

					// делаем временную матрицу для объекта, т.к. портить основную нельзя
					float ObjTransMat[16];
					memcpy(ObjTransMat, TransMat, 16*sizeof(float));

					// если нужно - создаем матрицу, иначе - копируем ее
					if (Object2->DrawObjectList[j].Rotation.x != 0.0f ||
					    Object2->DrawObjectList[j].Rotation.y != 0.0f ||
					    Object2->DrawObjectList[j].Rotation.z != 0.0f) {
						float TransMatTMP[16];
						vw_Matrix44Identity(TransMatTMP);

						vw_Matrix44CreateRotate(TransMatTMP, Object2->DrawObjectList[j].Rotation);

						vw_Matrix44Translate(TransMatTMP, LocalLocation);
						// и умножаем на основную матрицу, со сведениями по всему объекту
						vw_Matrix44Mult(ObjTransMat, TransMatTMP);
					} else {
						vw_Matrix44Translate(ObjTransMat, LocalLocation);
					}






					for (int k = 0; k < Object2->DrawObjectList[j].VertexCount; k+=3) {

						int j2;
						if (Object2->DrawObjectList[j].IndexBuffer != nullptr)
							j2 = Object2->DrawObjectList[j].IndexBuffer[Object2->DrawObjectList[j].RangeStart+k]*Object2->DrawObjectList[j].VertexStride;
						else
							j2 = (Object2->DrawObjectList[j].RangeStart+k)*Object2->DrawObjectList[j].VertexStride;

						// находим точки триугольника
						Point1.x = Object2->DrawObjectList[j].VertexBuffer[j2];
						Point1.y = Object2->DrawObjectList[j].VertexBuffer[j2+1];
						Point1.z = Object2->DrawObjectList[j].VertexBuffer[j2+2];
						vw_Matrix44CalcPoint(&Point1, ObjTransMat);

						if (Object2->DrawObjectList[j].IndexBuffer != nullptr)
							j2 = Object2->DrawObjectList[j].IndexBuffer[Object2->DrawObjectList[j].RangeStart+k+1]*Object2->DrawObjectList[j].VertexStride;
						else
							j2 = (Object2->DrawObjectList[j].RangeStart+k+1)*Object2->DrawObjectList[j].VertexStride;

						Point2.x = Object2->DrawObjectList[j].VertexBuffer[j2];
						Point2.y = Object2->DrawObjectList[j].VertexBuffer[j2+1];
						Point2.z = Object2->DrawObjectList[j].VertexBuffer[j2+2];
						vw_Matrix44CalcPoint(&Point2, ObjTransMat);

						if (Object2->DrawObjectList[j].IndexBuffer != nullptr)
							j2 = Object2->DrawObjectList[j].IndexBuffer[Object2->DrawObjectList[j].RangeStart+k+2]*Object2->DrawObjectList[j].VertexStride;
						else
							j2 = (Object2->DrawObjectList[j].RangeStart+k+2)*Object2->DrawObjectList[j].VertexStride;

						Point3.x = Object2->DrawObjectList[j].VertexBuffer[j2];
						Point3.y = Object2->DrawObjectList[j].VertexBuffer[j2+1];
						Point3.z = Object2->DrawObjectList[j].VertexBuffer[j2+2];
						vw_Matrix44CalcPoint(&Point3, ObjTransMat);



						// преобразуем все точки в систему первого объекта

						vw_Matrix33CalcPoint(&Point1, TMPOldInvRotationMat);
						vw_Matrix33CalcPoint(&Point2, TMPOldInvRotationMat);
						vw_Matrix33CalcPoint(&Point3, TMPOldInvRotationMat);


						// сразу ставим номер
						*Object1PieceNum = i;


						// сначала проверка по точкам...
						if (fabsf(Center.x - Point1.x) <= TMPMax.x)
							if (fabsf(Center.y - Point1.y) <= TMPMax.y)
								if (fabsf(Center.z - Point1.z) <= TMPMax.z)
									return true;
						if (fabsf(Center.x - Point2.x) <= TMPMax.x)
							if (fabsf(Center.y - Point2.y) <= TMPMax.y)
								if (fabsf(Center.z - Point2.z) <= TMPMax.z)
									return true;
						if (fabsf(Center.x - Point3.x) <= TMPMax.x)
							if (fabsf(Center.y - Point3.y) <= TMPMax.y)
								if (fabsf(Center.z - Point3.z) <= TMPMax.z)
									return true;




						// делаем боксы, и проверяем по ним


						// делаем аабб
						TMPMax2.x = std::max({Point1.x, Point2.x, Point3.x});
						TMPMin2.x = std::min({Point1.x, Point2.x, Point3.x});

						TMPMax2.y = std::max({Point1.y, Point2.y, Point3.y});
						TMPMin2.y = std::min({Point1.y, Point2.y, Point3.y});

						TMPMax2.z = std::max({Point1.z, Point2.z, Point3.z});
						TMPMin2.z = std::min({Point1.z, Point2.z, Point3.z});


						VECTOR3D Center2((TMPMax2.x+TMPMin2.x)/2.0f,(TMPMax2.y+TMPMin2.y)/2.0f,(TMPMax2.z+TMPMin2.z)/2.0f);

						TMPMax2.x = TMPMax2.x - Center2.x;
						TMPMax2.y = TMPMax2.y - Center2.y;
						TMPMax2.z = TMPMax2.z - Center2.z;



						if (fabsf(Center.x - Center2.x) > TMPMax.x + TMPMax2.x)
							continue;
						if (fabsf(Center.y - Center2.y) > TMPMax.y + TMPMax2.y)
							continue;
						if (fabsf(Center.z - Center2.z) > TMPMax.z + TMPMax2.z)
							continue;


						return true;

					}
				}

		}

	return false;
}
