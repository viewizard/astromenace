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

#include "collision_detection.h"

/* TODO translate comments */

/*
 * Check, is point belong triangle.
 */
static bool PointInTriangle(VECTOR3D point, VECTOR3D pa, VECTOR3D pb, VECTOR3D pc)
{
	float TotalAngle = 0.0f;

	VECTOR3D V1 = point - pa;
	VECTOR3D V2 = point - pb;
	VECTOR3D V3 = point - pc;

	V1.NormalizeHi();
	V2.NormalizeHi();
	V3.NormalizeHi();

	TotalAngle += acosf(V1*V2);
	TotalAngle += acosf(V2*V3);
	TotalAngle += acosf(V3*V1);

	if (fabsf(TotalAngle - 2*3.14159265f) <= 0.005f) return true;

	return false;
}

/*
 * AABB-AABB collision detection.
 */
bool vw_AABBAABBCollision(VECTOR3D Object1AABB[8], VECTOR3D Object1Location,
			  VECTOR3D Object2AABB[8], VECTOR3D Object2Location)
{
	/* check projection's collisions */
	if (fabsf(Object1Location.x - Object2Location.x) > fabsf(Object1AABB[0].x + Object2AABB[0].x))
		return false;
	if (fabsf(Object1Location.y - Object2Location.y) > fabsf(Object1AABB[0].y + Object2AABB[0].y))
		return false;
	if (fabsf(Object1Location.z - Object2Location.z) > fabsf(Object1AABB[0].z + Object2AABB[0].z))
		return false;

	return true;
}

/*
 * OBB-OBB collision detection.
 */
bool vw_OBBOBBCollision(VECTOR3D Object1OBB[8], VECTOR3D Object1OBBLocation, VECTOR3D Object1Location, float Object1RotationMatrix[9],
			VECTOR3D Object2OBB[8], VECTOR3D Object2OBBLocation, VECTOR3D Object2Location, float Object2RotationMatrix[9])
{
	/* calcuate rotation matrix */
	float TMPInvObject1RotationMatrix[9];
	memcpy(TMPInvObject1RotationMatrix, Object1RotationMatrix, 9*sizeof(float));
	Matrix33InverseRotate(TMPInvObject1RotationMatrix);
	/* calcuate first box size */
	VECTOR3D a = (Object1OBB[0] - Object1OBB[6])^0.5f;
	Matrix33CalcPoint(&a, TMPInvObject1RotationMatrix);
	/* calcuate inverse rotation matrix */
	float TMPInvObject2RotationMatrix[9];
	memcpy(TMPInvObject2RotationMatrix, Object2RotationMatrix, 9*sizeof(float));
	Matrix33InverseRotate(TMPInvObject2RotationMatrix);
	/* calcuate second box size */
	VECTOR3D b = (Object2OBB[0] - Object2OBB[6])^0.5f;
	Matrix33CalcPoint(&b, TMPInvObject2RotationMatrix);
	/* calcuate offset in global coordinate systems */
	VECTOR3D T = (Object2Location + Object2OBBLocation) -
		     (Object1Location + Object1OBBLocation);
	Matrix33CalcPoint(&T, TMPInvObject1RotationMatrix);
	/* calcuate transformation matrix */
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
	if(fabsf(T.z*R[1][0] - T.y*R[2][0]) >
	    a.y*fabsf(R[2][0]) + a.z*fabsf(R[1][0]) + b.y*fabsf(R[0][2]) + b.z*fabsf(R[0][1]))
		return false;
	//8 (Ra)x X (Rb)y
	if(fabsf(T.z*R[1][1] - T.y*R[2][1]) >
	    a.y*fabsf(R[2][1]) + a.z*fabsf(R[1][1]) + b.x*fabsf(R[0][2]) + b.z*fabsf(R[0][0]))
		return false;
	//9 (Ra)x X (Rb)z
	if(fabsf(T.z*R[1][2]-T.y*R[2][2]) >
	    a.y*fabsf(R[2][2]) + a.z*fabsf(R[1][2]) + b.x*fabsf(R[0][1]) + b.y*fabsf(R[0][0]))
		return false;
	//10 (Ra)y X (Rb)x
	if(fabsf(T.x*R[2][0]-T.z*R[0][0]) >
	    a.x*fabsf(R[2][0]) + a.z*fabsf(R[0][0]) + b.y*fabsf(R[1][2]) + b.z*fabsf(R[1][1]))
		return false;
	//11 (Ra)y X (Rb)y
	if(fabsf(T.x*R[2][1]-T.z*R[0][1]) >
	    a.x*fabsf(R[2][1]) + a.z*fabsf(R[0][1]) + b.x*fabsf(R[1][2]) + b.z*fabsf(R[1][0]))
		return false;
	//12 (Ra)y X (Rb)z
	if(fabsf(T.x*R[2][2]-T.z*R[0][2]) >
	    a.x*fabsf(R[2][2]) + a.z*fabsf(R[0][2]) + b.x*fabsf(R[1][1]) + b.y*fabsf(R[1][0]))
		return false;
	//13 (Ra)z X (Rb)x
	if(fabsf(T.y*R[0][0]-T.x*R[1][0]) >
	    a.x*fabsf(R[1][0]) + a.y*fabsf(R[0][0]) + b.y*fabsf(R[2][2]) + b.z*fabsf(R[2][1]))
		return false;
	//14 (Ra)z X (Rb)y
	if(fabsf(T.y*R[0][1]-T.x*R[1][1]) >
	    a.x*fabsf(R[1][1]) + a.y*fabsf(R[0][1]) + b.x*fabsf(R[2][2]) + b.z*fabsf(R[2][0]))
		return false;
	//15 (Ra)z X (Rb)z
	if(fabsf(T.y*R[0][2]-T.x*R[1][2]) >
	    a.x*fabsf(R[1][2]) + a.y*fabsf(R[0][2]) + b.x*fabsf(R[2][1]) + b.y*fabsf(R[2][0]))
		return false;

	return true;
}

/*
 * Sphere-Sphere collision detection.
 */
bool vw_SphereSphereCollision(float Object1Radius, VECTOR3D Object1Location,
			      float Object2Radius, VECTOR3D Object2Location,
			      VECTOR3D Object2PrevLocation)
{
	bool Result = false;

	VECTOR3D Object1m2Location = Object1Location - Object2Location;
	float Object1p1Radius = Object1Radius + Object2Radius;

	// быстрая проверка на кубы... чтобы не делать много умножений
	if (fabsf(Object1m2Location.x) <= Object1p1Radius) Result = true;
	if (fabsf(Object1m2Location.y) <= Object1p1Radius) Result = true;
	if (fabsf(Object1m2Location.z) <= Object1p1Radius) Result = true;

	// делаем просчет по радиусам... т.е. уже работаем со сферами
	if (Result) {
		// квадрат расстояния между объектами
		float Dist2 = Object1m2Location.x*Object1m2Location.x +
			      Object1m2Location.y*Object1m2Location.y +
			      Object1m2Location.z*Object1m2Location.z;

		// квадрат минимального расстояния
		float NeedDist2 = Object1p1Radius*Object1p1Radius;

		// если расстояние меньше или равно - значит есть столкновение по радиусам
		if (Dist2 <= NeedDist2) return true;
	}

	// проверяем на расстояние от точки до прямой...
	// т.е. находим, на каком расстоянии проходит прямая
	// если она "пронизывает" радиус первого объекта - нужна обработка
	// чтобы перехватить быстро летящих
	if (!Result) {
		// прямая
		VECTOR3D Ray = Object2Location - Object2PrevLocation;
		Ray.Normalize();

		// находим ближайшую точку на прямой
		float t = Ray.x*Object1Location.x +
			  Ray.y*Object1Location.y +
			  Ray.z*Object1Location.z - (Ray.x*Object2PrevLocation.x +
						     Ray.y*Object2PrevLocation.y +
						     Ray.z*Object2PrevLocation.z);

		// точка на прямой
		VECTOR3D IntercPoint = Object2PrevLocation^t;

		// если не на отрезке точка - там тут делать нечего
		if ((Object2PrevLocation - IntercPoint)*(Object2Location - IntercPoint) >= 0.0f)
			return false;

		// проверяем расстояние
		float NewDist2 = (IntercPoint.x - Object1Location.x)*(IntercPoint.x - Object1Location.x)+
				 (IntercPoint.y - Object1Location.y)*(IntercPoint.y - Object1Location.y)+
				 (IntercPoint.z - Object1Location.z)*(IntercPoint.z - Object1Location.z);

		// если расстояние меньше или равно - значит пролетит рядом
		if (NewDist2 <= Object1Radius*Object1Radius) return true;
	}

	// объекты очень далеко
	return false;
}

/*
 * Sphere-AABB collision detection.
 */
bool vw_SphereAABBCollision(VECTOR3D Object1AABB[8], VECTOR3D Object1Location,
			    float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation)
{
	bool Result = true;

	// проверяем на расстояние...
	// немного не точно на углах т.к. фактически проверяем AABB с кубом, но зато очень быстро
	if (fabsf(Object1Location.x - Object2Location.x) > Object1AABB[0].x + Object2Radius)
		Result = false;
	if (fabsf(Object1Location.y - Object2Location.y) > Object1AABB[0].y + Object2Radius)
		Result = false;
	if (fabsf(Object1Location.z - Object2Location.z) > Object1AABB[0].z + Object2Radius)
		Result = false;

	// проверим прямую, чтобы перехватить быстро летящих...
	if (!Result) {
		// средняя точка линии
		VECTOR3D mid = (Object2Location + Object2PrevLocation) / 2.0f;
		// направление линии
		VECTOR3D dir = Object2Location - Object2PrevLocation;
		// полудлина линии
		float hl = dir.Length() / 2.0f;
		dir.Normalize();

		VECTOR3D T = Object1Location - mid;
		float r;

		// проверяем, является ли одна из осей X,Y,Z разделяющей
		if ( (fabs(T.x) > Object1AABB[0].x + hl*fabs(dir.x)) ||
		     (fabs(T.y) > Object1AABB[0].y + hl*fabs(dir.y)) ||
		     (fabs(T.z) > Object1AABB[0].z + hl*fabs(dir.z)) )
			return false;

		// проверяем X ^ dir
		r = Object1AABB[0].y*fabs(dir.z) + Object1AABB[0].z*fabs(dir.y);
		if (fabs(T.y*dir.z - T.z*dir.y) > r)
			return false;

		// проверяем Y ^ dir
		r = Object1AABB[0].x*fabs(dir.z) + Object1AABB[0].z*fabs(dir.x);
		if (fabs(T.z*dir.x - T.x*dir.z) > r)
			return false;

		// проверяем Z ^ dir
		r = Object1AABB[0].x*fabs(dir.y) + Object1AABB[0].y*fabs(dir.x);
		if (fabs(T.x*dir.y - T.y*dir.x) > r)
			return false;

		return true;
	}

	// если сдесь - значит попали в AABB
	return Result;
}

/*
 * Sphere-OBB collision detection.
 */
bool vw_SphereOBBCollision(VECTOR3D Object1OBB[8], VECTOR3D Object1OBBLocation,
			   VECTOR3D Object1Location, float Object1RotationMatrix[9],
			   float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation)
{
	// параметры OBB
	VECTOR3D TMPMax = Object1OBB[0];
	VECTOR3D TMPMin = Object1OBB[6];
	// точки
	VECTOR3D TMPPoint1 = Object2Location - (Object1Location + Object1OBBLocation);
	VECTOR3D TMPPoint2 = Object2PrevLocation - (Object1Location + Object1OBBLocation);

	// строим матрицу, чтобы развернуть точки
	float TMPInvRotationMatrix[9];
	memcpy(TMPInvRotationMatrix, Object1RotationMatrix, 9*sizeof(float));
	Matrix33InverseRotate(TMPInvRotationMatrix);
	// перемещаем в общие координаты
	Matrix33CalcPoint(&TMPMax, TMPInvRotationMatrix);
	Matrix33CalcPoint(&TMPMin, TMPInvRotationMatrix);
	Matrix33CalcPoint(&TMPPoint1, TMPInvRotationMatrix);
	Matrix33CalcPoint(&TMPPoint2, TMPInvRotationMatrix);

	// и как в Sphere-AABB
	bool Result = true;

	// если максимальная точка сферы меньше минимума - не входит
	if (TMPPoint1.x + Object2Radius < TMPMin.x) Result = false;
	if (TMPPoint1.y + Object2Radius < TMPMin.y) Result = false;
	if (TMPPoint1.z + Object2Radius < TMPMin.z) Result = false;
	// если минимальная точка сферы больше максимальной AABB - тоже не входит
	if (TMPPoint1.x - Object2Radius > TMPMax.x) Result = false;
	if (TMPPoint1.y - Object2Radius > TMPMax.y) Result = false;
	if (TMPPoint1.z - Object2Radius > TMPMax.z) Result = false;

	// проверим прямую, чтобы перехватить быстро летящих...
	if (!Result) {
		// средняя точка линии
		VECTOR3D mid = (Object2Location + Object2PrevLocation) / 2.0f;
		// направление линии
		VECTOR3D dir = Object2Location - Object2PrevLocation;
		// полудлина линии
		float hl = dir.Length() / 2.0f;
		dir.Normalize();

		VECTOR3D T = Object1Location - mid;

		// проверяем, является ли одна из осей X,Y,Z разделяющей
		if ( (fabs(T.x) > TMPMax.x + hl*fabs(dir.x)) ||
		     (fabs(T.y) > TMPMax.y + hl*fabs(dir.y)) ||
		     (fabs(T.z) > TMPMax.z + hl*fabs(dir.z)) )
			return false;

		// проверяем X ^ dir
		float r = TMPMax.y*fabs(dir.z) + TMPMax.z*fabs(dir.y);
		if ( fabs(T.y*dir.z - T.z*dir.y) > r )
			return false;

		// проверяем Y ^ dir
		r = TMPMax.x*fabs(dir.z) + TMPMax.z*fabs(dir.x);
		if ( fabs(T.z*dir.x - T.x*dir.z) > r )
			return false;

		// проверяем Z ^ dir
		r = TMPMax.x*fabs(dir.y) + TMPMax.y*fabs(dir.x);
		if ( fabs(T.x*dir.y - T.y*dir.x) > r )
			return false;

		return true;
	}

	return Result;
}

/*
 * Sphere-Mesh collision detection.
 */
bool vw_SphereMeshCollision(VECTOR3D Object1Location, eObjectBlock *Object1DrawObjectList, float Object1RotationMatrix[9],
			    float Object2Radius, VECTOR3D Object2Location, VECTOR3D Object2PrevLocation,
			    VECTOR3D *CollisionLocation)
{
	// если не передали геометрию - делать тут нечего
	if (Object1DrawObjectList == nullptr) return false;

	// делаем матрицу перемещения точек, для геометрии
	float TransMat[16];
	TransMat[0] = Object1RotationMatrix[0];
	TransMat[1] = Object1RotationMatrix[1];
	TransMat[2] = Object1RotationMatrix[2];
	TransMat[3] = 0.0f;
	TransMat[4] = Object1RotationMatrix[3];
	TransMat[5] = Object1RotationMatrix[4];
	TransMat[6] = Object1RotationMatrix[5];
	TransMat[7] = 0.0f;
	TransMat[8] = Object1RotationMatrix[6];
	TransMat[9] = Object1RotationMatrix[7];
	TransMat[10] = Object1RotationMatrix[8];
	TransMat[11] = 0.0f;
	TransMat[12] = Object1Location.x;
	TransMat[13] = Object1Location.y;
	TransMat[14] = Object1Location.z;
	TransMat[15] = 1.0f;

	// находим точку локального положения объекта в моделе
	VECTOR3D LocalLocation(Object1DrawObjectList->Location);
	Matrix33CalcPoint(&LocalLocation, Object1RotationMatrix);

	// если нужно - создаем матрицу, иначе - копируем ее
	if (Object1DrawObjectList->Rotation.x != 0.0f ||
	    Object1DrawObjectList->Rotation.y != 0.0f ||
	    Object1DrawObjectList->Rotation.z != 0.0f) {
		float TransMatTMP[16];
		Matrix44Identity(TransMatTMP);
		Matrix44CreateRotate(TransMatTMP, Object1DrawObjectList->Rotation);
		Matrix44Translate(TransMatTMP, LocalLocation);
		// и умножаем на основную матрицу, со сведениями по всему объекту
		Matrix44Mult(TransMat, TransMatTMP);
	} else {
		Matrix44Translate(TransMat, LocalLocation);
	}

	// проверяем все треугольники объекта
	for (int i = 0; i < Object1DrawObjectList->VertexCount; i += 3) {
		// находим 3 точки треугольника (с учетом индекс буфера)
		int j2;
		if (Object1DrawObjectList->IndexBuffer != nullptr)
			j2 = Object1DrawObjectList->IndexBuffer[Object1DrawObjectList->RangeStart+i]*Object1DrawObjectList->VertexStride;
		else
			j2 = (Object1DrawObjectList->RangeStart+i)*Object1DrawObjectList->VertexStride;

		// находим точки триугольника
		VECTOR3D Point1;
		Point1.x = Object1DrawObjectList->VertexBuffer[j2];
		Point1.y = Object1DrawObjectList->VertexBuffer[j2 + 1];
		Point1.z = Object1DrawObjectList->VertexBuffer[j2 + 2];
		Matrix44CalcPoint(&Point1, TransMat);

		if (Object1DrawObjectList->IndexBuffer != nullptr)
			j2 = Object1DrawObjectList->IndexBuffer[Object1DrawObjectList->RangeStart + i + 1]*Object1DrawObjectList->VertexStride;
		else
			j2 = (Object1DrawObjectList->RangeStart + i + 1)*Object1DrawObjectList->VertexStride;

		VECTOR3D Point2;
		Point2.x = Object1DrawObjectList->VertexBuffer[j2];
		Point2.y = Object1DrawObjectList->VertexBuffer[j2 + 1];
		Point2.z = Object1DrawObjectList->VertexBuffer[j2 + 2];
		Matrix44CalcPoint(&Point2, TransMat);

		if (Object1DrawObjectList->IndexBuffer != nullptr)
			j2 = Object1DrawObjectList->IndexBuffer[Object1DrawObjectList->RangeStart + i + 2]*Object1DrawObjectList->VertexStride;
		else
			j2 = (Object1DrawObjectList->RangeStart + i + 2)*Object1DrawObjectList->VertexStride;

		VECTOR3D Point3;
		Point3.x = Object1DrawObjectList->VertexBuffer[j2];
		Point3.y = Object1DrawObjectList->VertexBuffer[j2 + 1];
		Point3.z = Object1DrawObjectList->VertexBuffer[j2 + 2];
		Matrix44CalcPoint(&Point3, TransMat);

		// находим 2 вектора, образующих плоскость
		VECTOR3D PlaneVector1 = Point2 - Point1;
		VECTOR3D PlaneVector2 = Point3 - Point1;

		// находим нормаль плоскости
		VECTOR3D NormalVector = PlaneVector1;
		NormalVector.Multiply(PlaneVector2);
		NormalVector.Normalize();

		// проверка, сферы
		//	- в сфере или нет?, т.е. расстояние от плоскости до центра сферы
		//	- по нормале находим точку на плоскости
		//	- точка принадлежит треугольнику?

		// находим расстояние от точки до плоскости
		float Distance = (Object2Location - Point1)*NormalVector;

		// если точка дальше радиуса - нам тут делать нечего, берем следующую точку
		if (fabsf(Distance) <= Object2Radius) {
			// находим реальную точку пересечения с плоскостью
			VECTOR3D IntercPoint = Object2Location - (NormalVector^Distance);

			// передаем точку и флаг успешной коллизии
			if (PointInTriangle(IntercPoint, Point1, Point2, Point3)) {
				*CollisionLocation = IntercPoint;
				return true;
			}
		}

		// проверка, сферы
		// если от точек до текущего положения расстояние меньше

		float Object2Radius2 = Object2Radius*Object2Radius;

		// находим расстояние
		VECTOR3D DistancePoint1 = Object2Location - Point1;
		float Distance2Point1 = DistancePoint1.x*DistancePoint1.x +
					DistancePoint1.y*DistancePoint1.y +
					DistancePoint1.z*DistancePoint1.z;
		if (Distance2Point1 <= Object2Radius2) {
			*CollisionLocation = Point1;
			return true;
		}

		// находим расстояние
		VECTOR3D DistancePoint2 = Object2Location - Point2;
		float Distance2Point2 = DistancePoint2.x*DistancePoint2.x +
					DistancePoint2.y*DistancePoint2.y +
					DistancePoint2.z*DistancePoint2.z;
		if (Distance2Point2 <= Object2Radius2) {
			*CollisionLocation = Point2;
			return true;
		}

		// находим расстояние
		VECTOR3D DistancePoint3 = Object2Location - Point3;
		float Distance2Point3 = DistancePoint3.x*DistancePoint3.x +
					DistancePoint3.y*DistancePoint3.y +
					DistancePoint3.z*DistancePoint3.z;
		if (Distance2Point3 <= Object2Radius2) {
			*CollisionLocation = Point3;
			return true;
		}

		// проверка луч, старое положение - новое положение объекта
		//	- пересекает плоскость (обе точки с одной стороны? знак, или ноль)
		//	- точка пересечения плоскости
		//	- принадлежит треугольнику?

		// проверка, если это фронт-часть, нормально... иначе - берем следующую
		VECTOR3D vDir1 = Point1 - Object2PrevLocation;
		float d1 = vDir1*NormalVector;
		if (d1 > 0.001f)
			continue;

		// находим расстояние от плоскости до точки
		float originDistance = NormalVector*Point1;

		// пересечение прямой с плоскостью
		VECTOR3D vLineDir = Object2Location - Object2PrevLocation;
		// может и не нужно этого делать... т.к. все и так хорошо работает в нашем случае
		//vLineDir.Normalize();

		float Numerator = - (NormalVector.x * Object2PrevLocation.x +		// Use the plane equation with the normal and the line
				     NormalVector.y * Object2PrevLocation.y +
				     NormalVector.z * Object2PrevLocation.z - originDistance);

		float Denominator = NormalVector*vLineDir;
		if( Denominator == 0.0) continue;

		float dist = Numerator / Denominator;

		// зная расстояние, находим точку пересечения с плоскостью
		VECTOR3D IntercPoint = Object2PrevLocation + (vLineDir ^ dist);

		// проверяем, на отрезке или нет (до этого работали с прямой/лучем)
		if ((Object2PrevLocation - IntercPoint)*(Object2Location - IntercPoint) >= 0.0f) continue;

		// передаем точку и флаг успешной коллизии
		if (PointInTriangle(IntercPoint, Point1, Point2, Point3)) {
			*CollisionLocation = IntercPoint;
			return true;
		}
	}

	return false;
}
