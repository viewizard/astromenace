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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

#include "object3d.h"
#include "../config/config.h"
#include "../game.h"
#include "../gfx/shadow_map.h"
#include "../script/script.h"

// флаг, показывать боксы или нет (1>AABB, 2>OBB, 3>HitBB)
int NeedShowBB = 0;

// FIXME should be fixed, don't allow global scope interaction for local variables
extern std::weak_ptr<cGLSL> GLSLShaderType1;
extern std::weak_ptr<cGLSL> GLSLShaderType2;
extern std::weak_ptr<cGLSL> GLSLShaderType3;


//-----------------------------------------------------------------------------
// Установка AABB, OBB и габаритов по геометрии объекта
//-----------------------------------------------------------------------------
void cObject3D::InitByDrawObjectList()
{
	// нет самой геометрии, работать не можем
	if (ObjectBlocks.empty())
		return;

	// создаем HitBB
	for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {
		float Matrix[9];
		vw_Matrix33CreateRotate(Matrix, ObjectBlocks[i].Rotation);

		float MinX = 10000.0f;
		float MaxX = -10000.0f;
		float MinY = 10000.0f;
		float MaxY = -10000.0f;
		float MinZ = 10000.0f;
		float MaxZ = -10000.0f;

		int tmpOffset;
		if (ObjectBlocks[i].IndexArray)
			tmpOffset = ObjectBlocks[i].IndexArray.get()[ObjectBlocks[i].RangeStart] * ObjectBlocks[i].VertexStride;
		else
			tmpOffset = ObjectBlocks[i].RangeStart * ObjectBlocks[i].VertexStride;

		sVECTOR3D tmp;
		tmp.x = ObjectBlocks[i].VertexArray.get()[tmpOffset + 0];
		tmp.y = ObjectBlocks[i].VertexArray.get()[tmpOffset + 1];
		tmp.z = ObjectBlocks[i].VertexArray.get()[tmpOffset + 2];
		vw_Matrix33CalcPoint(tmp, Matrix);
		MinX = MaxX = tmp.x;
		MinY = MaxY = tmp.y;
		MinZ = MaxZ = tmp.z;

		for (unsigned int j = 0; j < ObjectBlocks[i].VertexQuantity; j++) {
			int j2;
			if (ObjectBlocks[i].IndexArray)
				j2 = ObjectBlocks[i].IndexArray.get()[ObjectBlocks[i].RangeStart + j] * ObjectBlocks[i].VertexStride;
			else
				j2 = (ObjectBlocks[i].RangeStart + j) * ObjectBlocks[i].VertexStride;

			sVECTOR3D v;
			v.x = ObjectBlocks[i].VertexArray.get()[j2];
			v.y = ObjectBlocks[i].VertexArray.get()[j2 + 1];
			v.z = ObjectBlocks[i].VertexArray.get()[j2 + 2];
			vw_Matrix33CalcPoint(v, Matrix);
			if (MinX > v.x)
				MinX = v.x;
			if (MinY > v.y)
				MinY = v.y;
			if (MinZ > v.z)
				MinZ = v.z;
			if (MaxX < v.x)
				MaxX = v.x;
			if (MaxY < v.y)
				MaxY = v.y;
			if (MaxZ < v.z)
				MaxZ = v.z;
		}

		// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
		HitBB[i].Box[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		HitBB[i].Box[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		HitBB[i].Box[2] = sVECTOR3D(MinX, MaxY, MinZ);
		HitBB[i].Box[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		HitBB[i].Box[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		HitBB[i].Box[5] = sVECTOR3D(MinX, MinY, MaxZ);
		HitBB[i].Box[6] = sVECTOR3D(MinX, MinY, MinZ);
		HitBB[i].Box[7] = sVECTOR3D(MaxX, MinY, MinZ);

		// находим координаты центра HitBB относительно координат модели
		HitBB[i].Location.x = (MaxX + MinX) / 2.0f;
		HitBB[i].Location.y = (MaxY + MinY) / 2.0f;
		HitBB[i].Location.z = (MaxZ + MinZ) / 2.0f;

		// находим размеры коробки
		HitBB[i].Size.x = fabsf(MaxX - MinX);
		HitBB[i].Size.y = fabsf(MaxY - MinY);
		HitBB[i].Size.z = fabsf(MaxZ - MinZ);

		// находим квадрат радиуса
		HitBB[i].Radius2 = (HitBB[i].Size.x / 2.0f) * (HitBB[i].Size.x / 2.0f) +
				   (HitBB[i].Size.y / 2.0f) * (HitBB[i].Size.y / 2.0f) +
				   (HitBB[i].Size.z / 2.0f) * (HitBB[i].Size.z / 2.0f);

		// переносим данные HitBB чтобы центр стал геометрическим центром
		for (int k = 0; k < 8; k++) {
			HitBB[i].Box[k] -= HitBB[i].Location;
		}

		// учитываем положение самого объекта в моделе
		HitBB[i].Location += ObjectBlocks[i].Location;
	}

	// находим AABB, OBB... первоначально без учета текущего положения и поворота - их нет
	// 1-й HitBB всегда есть... фактически это OBB
	float MinX = HitBB[0].Box[6].x + HitBB[0].Location.x;
	float MaxX = HitBB[0].Box[0].x + HitBB[0].Location.x;
	float MinY = HitBB[0].Box[6].y + HitBB[0].Location.y;
	float MaxY = HitBB[0].Box[0].y + HitBB[0].Location.y;
	float MinZ = HitBB[0].Box[6].z + HitBB[0].Location.z;
	float MaxZ = HitBB[0].Box[0].z + HitBB[0].Location.z;

	// берем то по HitBB, причем со 2-го сразу, т.к. первый учли выше
	for (unsigned int i = 1; i < ObjectBlocks.size(); i++) {
		if (MinX > HitBB[i].Box[6].x + HitBB[i].Location.x)
			MinX = HitBB[i].Box[6].x + HitBB[i].Location.x;
		if (MaxX < HitBB[i].Box[0].x + HitBB[i].Location.x)
			MaxX = HitBB[i].Box[0].x + HitBB[i].Location.x;
		if (MinY > HitBB[i].Box[6].y + HitBB[i].Location.y)
			MinY = HitBB[i].Box[6].y + HitBB[i].Location.y;
		if (MaxY < HitBB[i].Box[0].y + HitBB[i].Location.y)
			MaxY = HitBB[i].Box[0].y + HitBB[i].Location.y;
		if (MinZ > HitBB[i].Box[6].z + HitBB[i].Location.z)
			MinZ = HitBB[i].Box[6].z + HitBB[i].Location.z;
		if (MaxZ < HitBB[i].Box[0].z + HitBB[i].Location.z)
			MaxZ = HitBB[i].Box[0].z + HitBB[i].Location.z;
	}

	// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
	OBB.Box[0] = AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
	OBB.Box[1] = AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
	OBB.Box[2] = AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
	OBB.Box[3] = AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
	OBB.Box[4] = AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
	OBB.Box[5] = AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
	OBB.Box[6] = AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
	OBB.Box[7] = AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);

	// находим координаты центра OBB относительно координат модели
	OBB.Location.x = (MaxX + MinX) / 2.0f;
	OBB.Location.y = (MaxY + MinY) / 2.0f;
	OBB.Location.z = (MaxZ + MinZ) / 2.0f;

	// переносим данные OBB чтобы центр стал центром
	for (int k = 0; k < 8; k++) {
		OBB.Box[k] -= OBB.Location;
	}

	// габариты
	Width = fabsf(MaxX - MinX);
	Height = fabsf(MaxY - MinY);
	Length = fabsf(MaxZ - MinZ);

	float Width2 = Width / 2.0f;
	float Length2 = Length / 2.0f;
	float Height2 = Height / 2.0f;
	// находим радиус
	Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);

	// находим "центр массы", в базовом режиме считаем всю геометрию
	int AllVertexCounted = 0;
	for (auto &tmpObjectBlock : ObjectBlocks) {
		for (unsigned int j = 0; j < tmpObjectBlock.VertexQuantity; j++) {
			AllVertexCounted++;

			unsigned int tmpIndex{0};
			if (tmpObjectBlock.IndexArray.get())
				tmpIndex = tmpObjectBlock.IndexArray.get()[j] * tmpObjectBlock.VertexStride;
			else
				tmpIndex = tmpObjectBlock.VertexStride * j;

			GeometryCenterLocation += tmpObjectBlock.Location +
						  sVECTOR3D{tmpObjectBlock.VertexArray.get()[tmpIndex],
							    tmpObjectBlock.VertexArray.get()[tmpIndex + 1],
							    tmpObjectBlock.VertexArray.get()[tmpIndex + 2]};
		}
	}
	if (AllVertexCounted > 0)
		GeometryCenterLocation = GeometryCenterLocation / AllVertexCounted;
}

//-----------------------------------------------------------------------------
// Установка положения 1 объекта модели
//-----------------------------------------------------------------------------
void cObject3D::SetObjectLocation(sVECTOR3D NewLocation, int ObjectNum)
{
	// пересчет HitBB
	if (!HitBB.empty()) {
		// делаем временную обратную матрицу модели
		float OldInvRotationMatTmp[9];
		memcpy(OldInvRotationMatTmp, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
		vw_Matrix33InverseRotate(OldInvRotationMatTmp);

		vw_Matrix33CalcPoint(HitBB[ObjectNum].Location, OldInvRotationMatTmp);
		HitBB[ObjectNum].Location -= ObjectBlocks[ObjectNum].Location - NewLocation;
		vw_Matrix33CalcPoint(HitBB[ObjectNum].Location, CurrentRotationMat);

		// нужно подкорректировать OBB и ABB
		float MinX = 10000.0f;
		float MaxX = -10000.0f;
		float MinY = 10000.0f;
		float MaxY = -10000.0f;
		float MinZ = 10000.0f;
		float MaxZ = -10000.0f;

		// проверяем данные
		for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {
			vw_Matrix33CalcPoint(HitBB[i].Location, OldInvRotationMatTmp);

			for (int j = 0; j < 8; j++) {
				vw_Matrix33CalcPoint(HitBB[i].Box[j], OldInvRotationMatTmp);

				if (MinX > HitBB[i].Box[j].x + HitBB[i].Location.x)
					MinX = HitBB[i].Box[j].x + HitBB[i].Location.x;
				if (MaxX < HitBB[i].Box[j].x + HitBB[i].Location.x)
					MaxX = HitBB[i].Box[j].x + HitBB[i].Location.x;
				if (MinY > HitBB[i].Box[j].y + HitBB[i].Location.y)
					MinY = HitBB[i].Box[j].y + HitBB[i].Location.y;
				if (MaxY < HitBB[i].Box[j].y + HitBB[i].Location.y)
					MaxY = HitBB[i].Box[j].y + HitBB[i].Location.y;
				if (MinZ > HitBB[i].Box[j].z + HitBB[i].Location.z)
					MinZ = HitBB[i].Box[j].z + HitBB[i].Location.z;
				if (MaxZ < HitBB[i].Box[j].z + HitBB[i].Location.z)
					MaxZ = HitBB[i].Box[j].z + HitBB[i].Location.z;

				vw_Matrix33CalcPoint(HitBB[i].Box[j], CurrentRotationMat);
			}
			vw_Matrix33CalcPoint(HitBB[i].Location, CurrentRotationMat);
		}

		// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
		OBB.Box[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		OBB.Box[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		OBB.Box[2] = sVECTOR3D(MinX, MaxY, MinZ);
		OBB.Box[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		OBB.Box[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		OBB.Box[5] = sVECTOR3D(MinX, MinY, MaxZ);
		OBB.Box[6] = sVECTOR3D(MinX, MinY, MinZ);
		OBB.Box[7] = sVECTOR3D(MaxX, MinY, MinZ);

		// габариты, пересчет именно тут, т.к. нужны данные OBB
		Width = fabsf(MaxX - MinX);
		Height = fabsf(MaxY - MinY);
		Length = fabsf(MaxZ - MinZ);

		float Width2 = Width / 2.0f;
		float Length2 = Length / 2.0f;
		float Height2 = Height / 2.0f;
		// находим радиус
		Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);

		// находим координаты центра OBB относительно координат модели
		OBB.Location.x = (MaxX + MinX) / 2.0f;
		OBB.Location.y = (MaxY + MinY) / 2.0f;
		OBB.Location.z = (MaxZ + MinZ) / 2.0f;

		// переносим данные OBB чтобы центр стал центром
		for (int k = 0; k < 8; k++) {
			OBB.Box[k] -= OBB.Location;
			vw_Matrix33CalcPoint(OBB.Box[k], CurrentRotationMat);
		}
		vw_Matrix33CalcPoint(OBB.Location, CurrentRotationMat);

		// по OBB находим AABB, это не совсем AABB (он будет больше), но зато быстро
		MinX = MaxX = OBB.Box[0].x + OBB.Location.x;
		MinY = MaxY = OBB.Box[0].y + OBB.Location.y;
		MinZ = MaxZ = OBB.Box[0].z + OBB.Location.z;
		// берем и проверяем
		for (int j = 0; j < 8; j++) {
			if (MinX > OBB.Box[j].x + OBB.Location.x)
				MinX = OBB.Box[j].x + OBB.Location.x;
			if (MinY > OBB.Box[j].y + OBB.Location.y)
				MinY = OBB.Box[j].y + OBB.Location.y;
			if (MinZ > OBB.Box[j].z + OBB.Location.z)
				MinZ = OBB.Box[j].z + OBB.Location.z;
			if (MaxX < OBB.Box[j].x + OBB.Location.x)
				MaxX = OBB.Box[j].x + OBB.Location.x;
			if (MaxY < OBB.Box[j].y + OBB.Location.y)
				MaxY = OBB.Box[j].y + OBB.Location.y;
			if (MaxZ < OBB.Box[j].z + OBB.Location.z)
				MaxZ = OBB.Box[j].z + OBB.Location.z;
		}
		AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
		AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
		AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
		AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);
	}

	// собственно меняем данные в геометрии
	ObjectBlocks[ObjectNum].Location = NewLocation;
}

//-----------------------------------------------------------------------------
// Установка углов поворота 1 объекта модели
//-----------------------------------------------------------------------------
void cObject3D::SetObjectRotation(sVECTOR3D NewRotation, int ObjectNum)
{
	// пересчет HitBB
	if (!HitBB.empty()) {
		// создаем матрицу поворота объекта
		float CurrentRotationMatTmp2[9];
		vw_Matrix33CreateRotate(CurrentRotationMatTmp2, NewRotation);

		float OldInvRotationMatTmp2[9];
		vw_Matrix33CreateRotate(OldInvRotationMatTmp2, ObjectBlocks[ObjectNum].Rotation);
		vw_Matrix33InverseRotate(OldInvRotationMatTmp2);

		// делаем временную обратную матрицу модели
		float OldInvRotationMatTmp[9];
		memcpy(OldInvRotationMatTmp, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
		vw_Matrix33InverseRotate(OldInvRotationMatTmp);

		// собственно меняем данные в геометрии
		vw_Matrix33CalcPoint(HitBB[ObjectNum].Location, OldInvRotationMatTmp);
		HitBB[ObjectNum].Location -= ObjectBlocks[ObjectNum].Location;
		vw_Matrix33CalcPoint(HitBB[ObjectNum].Location, OldInvRotationMatTmp2);
		vw_Matrix33CalcPoint(HitBB[ObjectNum].Location, CurrentRotationMatTmp2);
		HitBB[ObjectNum].Location += ObjectBlocks[ObjectNum].Location;
		vw_Matrix33CalcPoint(HitBB[ObjectNum].Location, CurrentRotationMat);
		for (int j = 0; j < 8; j++) {
			vw_Matrix33CalcPoint(HitBB[ObjectNum].Box[j], OldInvRotationMatTmp);
			vw_Matrix33CalcPoint(HitBB[ObjectNum].Box[j], OldInvRotationMatTmp2);
			vw_Matrix33CalcPoint(HitBB[ObjectNum].Box[j], CurrentRotationMatTmp2);
			vw_Matrix33CalcPoint(HitBB[ObjectNum].Box[j], CurrentRotationMat);
		}

		// нужно подкорректировать OBB и ABB
		float MinX = 10000.0f;
		float MaxX = -10000.0f;
		float MinY = 10000.0f;
		float MaxY = -10000.0f;
		float MinZ = 10000.0f;
		float MaxZ = -10000.0f;

		// проверяем данные
		for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {
			vw_Matrix33CalcPoint(HitBB[i].Location, OldInvRotationMatTmp);

			for (int j = 0; j < 8; j++) {
				vw_Matrix33CalcPoint(HitBB[i].Box[j], OldInvRotationMatTmp);

				if (MinX > HitBB[i].Box[j].x + HitBB[i].Location.x)
					MinX = HitBB[i].Box[j].x + HitBB[i].Location.x;
				if (MaxX < HitBB[i].Box[j].x + HitBB[i].Location.x)
					MaxX = HitBB[i].Box[j].x + HitBB[i].Location.x;
				if (MinY > HitBB[i].Box[j].y + HitBB[i].Location.y)
					MinY = HitBB[i].Box[j].y + HitBB[i].Location.y;
				if (MaxY < HitBB[i].Box[j].y + HitBB[i].Location.y)
					MaxY = HitBB[i].Box[j].y + HitBB[i].Location.y;
				if (MinZ > HitBB[i].Box[j].z + HitBB[i].Location.z)
					MinZ = HitBB[i].Box[j].z + HitBB[i].Location.z;
				if (MaxZ < HitBB[i].Box[j].z + HitBB[i].Location.z)
					MaxZ = HitBB[i].Box[j].z + HitBB[i].Location.z;

				vw_Matrix33CalcPoint(HitBB[i].Box[j], CurrentRotationMat);
			}
			vw_Matrix33CalcPoint(HitBB[i].Location, CurrentRotationMat);
		}

		// запоминаем только то, что нужно - float x, float y, float z, float sizeX, float sizeY, float sizeZ
		OBB.Box[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		OBB.Box[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		OBB.Box[2] = sVECTOR3D(MinX, MaxY, MinZ);
		OBB.Box[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		OBB.Box[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		OBB.Box[5] = sVECTOR3D(MinX, MinY, MaxZ);
		OBB.Box[6] = sVECTOR3D(MinX, MinY, MinZ);
		OBB.Box[7] = sVECTOR3D(MaxX, MinY, MinZ);

		// габариты, пересчет именно тут, т.к. нужны данные OBB
		Width = fabsf(MaxX - MinX);
		Height = fabsf(MaxY - MinY);
		Length = fabsf(MaxZ - MinZ);

		float Width2 = Width / 2.0f;
		float Length2 = Length / 2.0f;
		float Height2 = Height / 2.0f;
		// находим радиус
		Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);

		// находим координаты центра OBB относительно координат модели
		OBB.Location.x = (MaxX + MinX) / 2.0f;
		OBB.Location.y = (MaxY + MinY) / 2.0f;
		OBB.Location.z = (MaxZ + MinZ) / 2.0f;

		// переносим данные OBB чтобы центр стал центром
		for (int k = 0; k < 8; k++) {
			OBB.Box[k] -= OBB.Location;
			vw_Matrix33CalcPoint(OBB.Box[k], CurrentRotationMat);
		}
		vw_Matrix33CalcPoint(OBB.Location, CurrentRotationMat);

		// по OBB находим AABB, это не совсем AABB (он будет больше), но зато быстро
		MinX = MaxX = OBB.Box[0].x + OBB.Location.x;
		MinY = MaxY = OBB.Box[0].y + OBB.Location.y;
		MinZ = MaxZ = OBB.Box[0].z + OBB.Location.z;
		// берем и проверяем
		for (int j = 0; j < 8; j++) {
			if (MinX > OBB.Box[j].x + OBB.Location.x)
				MinX = OBB.Box[j].x + OBB.Location.x;
			if (MinY > OBB.Box[j].y + OBB.Location.y)
				MinY = OBB.Box[j].y + OBB.Location.y;
			if (MinZ > OBB.Box[j].z + OBB.Location.z)
				MinZ = OBB.Box[j].z + OBB.Location.z;
			if (MaxX < OBB.Box[j].x + OBB.Location.x)
				MaxX = OBB.Box[j].x + OBB.Location.x;
			if (MaxY < OBB.Box[j].y + OBB.Location.y)
				MaxY = OBB.Box[j].y + OBB.Location.y;
			if (MaxZ < OBB.Box[j].z + OBB.Location.z)
				MaxZ = OBB.Box[j].z + OBB.Location.z;
		}
		AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
		AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
		AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
		AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);
	}

	// собственно меняем данные в геометрии
	ObjectBlocks[ObjectNum].Rotation = NewRotation;
}

//-----------------------------------------------------------------------------
// Установка положения модели
//-----------------------------------------------------------------------------
void cObject3D::SetLocation(sVECTOR3D NewLocation)
{
	PrevLocation = Location;
	// новое положение объекта
	Location = NewLocation;
}

//-----------------------------------------------------------------------------
// Установка углов поворота модели
//-----------------------------------------------------------------------------
void cObject3D::SetRotation(sVECTOR3D NewRotation)
{
	// Записываем данные в Rotation
	OldRotationInv = sVECTOR3D(-Rotation.x, -Rotation.y, -Rotation.z);
	Rotation += NewRotation;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	// делаем инверсную старую матрицу
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Rotation);

	// По углам находим новые значения вектора Orientation
	vw_Matrix33CalcPoint(Orientation, OldInvRotationMat);
	vw_Matrix33CalcPoint(Orientation, CurrentRotationMat);

	// пересчет HitBB
	if (!HitBB.empty()) {
		for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {
			vw_Matrix33CalcPoint(HitBB[i].Location, OldInvRotationMat);
			vw_Matrix33CalcPoint(HitBB[i].Location, CurrentRotationMat);

			for (int j = 0; j < 8; j++) {
				vw_Matrix33CalcPoint(HitBB[i].Box[j], OldInvRotationMat);
				vw_Matrix33CalcPoint(HitBB[i].Box[j], CurrentRotationMat);
			}
		}
	}

	// Пересчитываем OBB, просто поворачиваем его как и модель
	vw_Matrix33CalcPoint(OBB.Location, OldInvRotationMat);
	vw_Matrix33CalcPoint(OBB.Location, CurrentRotationMat);
	for (int j = 0; j < 8; j++) {
		vw_Matrix33CalcPoint(OBB.Box[j], OldInvRotationMat);
		vw_Matrix33CalcPoint(OBB.Box[j], CurrentRotationMat);
	}

	// по OBB находим AABB, это не совсем AABB (он будет больше), но зато быстро
	float MinX = 10000.0f;
	float MaxX = -10000.0f;
	float MinY = 10000.0f;
	float MaxY = -10000.0f;
	float MinZ = 10000.0f;
	float MaxZ = -10000.0f;
	// берем и проверяем
	for (int j = 0; j < 8; j++) {
		if (MinX > OBB.Box[j].x + OBB.Location.x)
			MinX = OBB.Box[j].x + OBB.Location.x;
		if (MinY > OBB.Box[j].y + OBB.Location.y)
			MinY = OBB.Box[j].y + OBB.Location.y;
		if (MinZ > OBB.Box[j].z + OBB.Location.z)
			MinZ = OBB.Box[j].z + OBB.Location.z;
		if (MaxX < OBB.Box[j].x + OBB.Location.x)
			MaxX = OBB.Box[j].x + OBB.Location.x;
		if (MaxY < OBB.Box[j].y + OBB.Location.y)
			MaxY = OBB.Box[j].y + OBB.Location.y;
		if (MaxZ < OBB.Box[j].z + OBB.Location.z)
			MaxZ = OBB.Box[j].z + OBB.Location.z;
	}
	AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
	AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
	AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
	AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
	AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
	AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
	AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
	AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);

}

//-----------------------------------------------------------------------------
// Прорисовка одной линии
//-----------------------------------------------------------------------------
void DrawLine(sVECTOR3D Point1, sVECTOR3D Point2, float ColorR, float ColorG, float ColorB, float ColorA)
{
	// буфер для последовательности RI_LINES
	// войдет RI_3f_XYZ | RI_4f_COLOR
	float *tmpDATA = new float[2 * (3 + 4)];
	int k = 0;

	tmpDATA[k++] = Point1.x;		// X
	tmpDATA[k++] = Point1.y;		// Y
	tmpDATA[k++] = Point1.z;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = Point2.x;		// X
	tmpDATA[k++] = Point2.y;		// Y
	tmpDATA[k++] = Point2.z;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k] = ColorA;

	vw_Draw3D(ePrimitiveType::LINES, 2, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7 * sizeof(tmpDATA[0]));

	delete [] tmpDATA;
}

//-----------------------------------------------------------------------------
// Прорисовка сетки бокса
//-----------------------------------------------------------------------------
void DrawBoxLines(sVECTOR3D Point[8], sVECTOR3D LocalLocation, float ColorR, float ColorG, float ColorB, float ColorA)
{
	vw_PushMatrix();
	vw_Translate(LocalLocation);

	DrawLine(Point[0], Point[1], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[1], Point[2], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[2], Point[3], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[3], Point[0], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[4], Point[5], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[5], Point[6], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[6], Point[7], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[7], Point[4], ColorR, ColorG, ColorB, ColorA);

	DrawLine(Point[0], Point[4], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[1], Point[5], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[2], Point[6], ColorR, ColorG, ColorB, ColorA);
	DrawLine(Point[3], Point[7], ColorR, ColorG, ColorB, ColorA);

	vw_PopMatrix();
}

//-----------------------------------------------------------------------------
// Прорисовка объектa Object3D
//-----------------------------------------------------------------------------
void cObject3D::Draw(bool VertexOnlyPass, bool ShadowMap)
{
	// если нечего рисовать - выходим
	if (ObjectBlocks.empty())
		return;

	// если есть установка, нужно получить квадрат расстояния до камеры
	// прорисовка модели "полностью", "одним куском" (только то что без "составных" частей - колес, стволов и т.п.)
	bool NeedOnePieceDraw{false};
	if (PromptDrawDist2 >= 0.0f) {
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);
		float PromptDrawRealDist2 = (Location.x - CurrentCameraLocation.x) * (Location.x - CurrentCameraLocation.x) +
					    (Location.y - CurrentCameraLocation.y) * (Location.y - CurrentCameraLocation.y) +
					    (Location.z - CurrentCameraLocation.z) * (Location.z - CurrentCameraLocation.z);

		// получаем кол-во точечных источников, воздействующих на объект
		int LightsCount = vw_CalculateAllPointLightsAttenuation(Location, Radius * Radius, nullptr);

		// если дальше - смотрим сколько воздействует источников света
		if (PromptDrawRealDist2 > PromptDrawDist2) {
			// если больше заданного кол-ва
			if (LightsCount <= GameConfig().MaxPointLights)
				NeedOnePieceDraw = true;
			else
				NeedOnePieceDraw = false;

			// если это двигатели - не надо переходить
			if (InternalLights >= LightsCount)
				NeedOnePieceDraw = true;
		} else {
			// находимся близко, но нужно посмотреть, если кол-во источников ниже макс, надо перейти в упращенный режим
			if (LightsCount <= GameConfig().MaxPointLights)
				NeedOnePieceDraw = true;
		}
	}

	// fast rendering for shadows map generation, without textures, shaders, etc
	// make sure, we call this one _before_ any camera/frustum checks, since not visible
	// for us 3D model could also drop the shadow on visible for us part of scene
	if (VertexOnlyPass) {
		vw_PushMatrix();
		// переносим и двигаем уже по данным всей модели
		vw_Translate(Location);
		vw_Rotate(Rotation.z, 0.0f, 0.0f, 1.0f);
		vw_Rotate(Rotation.y, 0.0f, 1.0f, 0.0f);
		vw_Rotate(Rotation.x, 1.0f, 0.0f, 0.0f);

		if (NeedOnePieceDraw) {
			int GlobalVertexCount{0};
			for (auto &tmpObjectBlock : ObjectBlocks) {
				GlobalVertexCount += tmpObjectBlock.VertexQuantity;
			}

			// часть данных берем из 1-го объекта, т.к. они идентичны для всей модели
			vw_Draw3D(ePrimitiveType::TRIANGLES, GlobalVertexCount, RI_3f_XYZ, GlobalVertexArray.get(),
				  ObjectBlocks[0].VertexStride * sizeof(float), GlobalVBO, 0,
				  GlobalIndexArray.get(), GlobalIBO, GlobalVAO);
		} else {
			// установка текстур и подхотовка к прорисовке
			for (auto &tmpObjectBlock : ObjectBlocks) {
				vw_PushMatrix();

				// сдвигаем его в нужное место
				vw_Translate(tmpObjectBlock.Location);
				// поворачиваем объект
				vw_Rotate(tmpObjectBlock.Rotation.z, 0.0f, 0.0f, 1.0f);
				vw_Rotate(tmpObjectBlock.Rotation.y, 0.0f, 1.0f, 0.0f);
				vw_Rotate(tmpObjectBlock.Rotation.x, 1.0f, 0.0f, 0.0f);
				// если нужна дополнительная анимация геометрией
				if (tmpObjectBlock.NeedGeometryAnimation) {
					vw_Rotate(tmpObjectBlock.GeometryAnimation.z, 0.0f, 0.0f, 1.0f);
					vw_Rotate(tmpObjectBlock.GeometryAnimation.y, 0.0f, 1.0f, 0.0f);
					vw_Rotate(tmpObjectBlock.GeometryAnimation.x, 1.0f, 0.0f, 0.0f);
				}

				// работаем только с шейдером взрывов, т.к. он меняет положение и размеры треугольников
				if (tmpObjectBlock.ShaderType == 2) {
					if (auto sharedGLSL = GLSLShaderType2.lock()) {
						vw_UseShaderProgram(sharedGLSL);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 0);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), 0);
						vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 3), ObjectBlocks[0].ShaderData[0]);
						vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 4), ObjectBlocks[0].ShaderData[1]);
					}
				}

				vw_Draw3D(ePrimitiveType::TRIANGLES, tmpObjectBlock.VertexQuantity, RI_3f_XYZ, tmpObjectBlock.VertexArray.get(),
					  tmpObjectBlock.VertexStride * sizeof(float), tmpObjectBlock.VBO,
					  tmpObjectBlock.RangeStart, tmpObjectBlock.IndexArray.get(), tmpObjectBlock.IBO, tmpObjectBlock.VAO);

				if ((tmpObjectBlock.ShaderType == 2) &&
				    !GLSLShaderType2.expired())
					vw_StopShaderProgram();

				vw_PopMatrix();
			}
		}

		vw_PopMatrix();
		// уходим вообще из прорисовки
		return;
	}

	// оптимизация, если не в фруструме - соотв. и не видем его
	if (!vw_BoxInFrustum(Location + AABB[6], Location + AABB[0])) {
		// если показали а сейчас нет - установка флага
		if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::showed)
			DeleteAfterLeaveScene = eDeleteAfterLeaveScene::need_delete;

		return;
	}

	// показываем - нужно установить флаг
	if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::enabled)
		DeleteAfterLeaveScene = eDeleteAfterLeaveScene::showed;
	// уже включили обратный отсчет на удаление - нужно его выключить
	if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::wait_delay) {
		DeleteAfterLeaveScene = eDeleteAfterLeaveScene::showed;
		Lifetime = -1.0f;
	}

	// у модели может быть нормал меппинг только на отдельные объекты
	GLtexture CurrentNormalMap{0};
	// текущий шейдер, чтобы не ставить лишний раз
	std::weak_ptr<cGLSL> CurrentGLSL{};
	int NeedNormalMapping{0};
	// получаем матрицу, до всех преобразований
	float Matrix[16];
	vw_GetMatrix(eMatrixPname::MODELVIEW, Matrix);

	vw_PushMatrix();

	// переносим и двигаем уже по данным всей модели
	vw_Translate(Location);
	vw_Rotate(Rotation.z, 0.0f, 0.0f, 1.0f);
	vw_Rotate(Rotation.y, 0.0f, 1.0f, 0.0f);
	vw_Rotate(Rotation.x, 1.0f, 0.0f, 0.0f);

	// для корректной прорисовки на всех видеокартах атмосферы планеты ...
	bool N1 = false;
	for (auto &tmpObjectBlock : ObjectBlocks) {
		if (tmpObjectBlock.DrawType == eObjectDrawType::Blend)
			N1 = true;
	}

	// если используем тени - сразу находим режим сглаживания
	int PCFMode{0};
	if (ShadowMap)
		PCFMode = GameConfig().ShadowMap;

	// Устанавливаем данные для поверхности объекта
	vw_MaterialV(eMaterialParameter::DIFFUSE, Diffuse);
	vw_MaterialV(eMaterialParameter::AMBIENT, Ambient);
	vw_MaterialV(eMaterialParameter::SPECULAR, Specular);
	vw_MaterialV(eMaterialParameter::SHININESS, Power);

	if (!NeedCullFaces)
		vw_CullFace(eCullFace::NONE);
	// для частей базы надо включить прозрачность через альфатест
	if (NeedAlphaTest)
		vw_SetTextureAlphaTest(true, eCompareFunc::GREATER, 0.4f);

	// если надо рисовать одним проходом
	if (NeedOnePieceDraw) {
		vw_BindTexture(0, Texture[0]);

		// включаем вторую текстуру
		if (!TextureIllum.empty() && TextureIllum[0]) {
			// свечение
			vw_BindTexture(1, TextureIllum[0]);
			// для корректной прорисовки без шейдеров, ставим правильный режим смешивания
			vw_SetTextureEnvMode(eTextureEnvMode::COMBINE);
			vw_SetTextureBlendMode(eTextureCombinerName::COMBINE_RGB, eTextureCombinerOp::ADD);
		}

		// если у нас работают шейдеры и есть текстура нормал меппа - ставим ее
		NeedNormalMapping = 0;
		if (GameConfig().UseGLSL120 &&
		    !NormalMap.empty() && NormalMap[0]) {
			NeedNormalMapping = 1;
			CurrentNormalMap = NormalMap[0];
			vw_BindTexture(3, CurrentNormalMap);
		}

		int LightType1, LightType2;
		// включаем источники света
		vw_CheckAndActivateAllLights(LightType1, LightType2, Location, Radius*Radius, 2, GameConfig().MaxPointLights, Matrix);

		if (GameConfig().UseGLSL120 && (ObjectBlocks[0].ShaderType >= 0)) {
			std::weak_ptr<cGLSL> CurrentObject3DGLSL{};

			// небольшая корректировка, если 1-й шейдер (попиксельное освещение), но передали шадовмеп - ставим 3
			if ((ObjectBlocks[0].ShaderType == 1) && ShadowMap)
				ObjectBlocks[0].ShaderType = 3;
			// и на оборот, если стоит 3-й, но шадовмепа нет - ставим 1-й, просто попиксельное освещение
			if ((ObjectBlocks[0].ShaderType == 3) && !ShadowMap)
				ObjectBlocks[0].ShaderType = 1;

			// ставим нужный шейдер
			switch (ObjectBlocks[0].ShaderType) {
			case 1:
				CurrentObject3DGLSL = GLSLShaderType1;
				break;
			case 2:
				CurrentObject3DGLSL = GLSLShaderType2;
				break;
			case 3:
				CurrentObject3DGLSL = GLSLShaderType3;
				break;
			}

			if (CurrentGLSL.lock() != CurrentObject3DGLSL.lock()) {
				if (!CurrentGLSL.expired())
					vw_StopShaderProgram();

				CurrentGLSL = CurrentObject3DGLSL;

				if (!CurrentGLSL.expired())
					vw_UseShaderProgram(CurrentGLSL);
			}

			// данные ставим каждый раз, т.к. может что-то поменяться
			if (auto sharedGLSL = CurrentObject3DGLSL.lock()) {
				switch (ObjectBlocks[0].ShaderType) {
				case 1: // только попиксельное освещение
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), LightType1);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), LightType2);
					if (!TextureIllum.empty() && TextureIllum[0])
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 1);
					else
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 0);

					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 5), 3);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 6), NeedNormalMapping);
					break;

				case 2: // шейдеры взрывов
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), LightType1);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), LightType2);
					vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 3), ObjectBlocks[0].ShaderData[0]);
					vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 4), ObjectBlocks[0].ShaderData[1]);
					break;

				case 3: // шадов меп
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), LightType1);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), LightType2);
					if (!TextureIllum.empty() && TextureIllum[0])
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 1);
					else
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 0);

					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 5), 2);
					vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 6), ShadowMap_Get_xPixelOffset());
					vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 7), ShadowMap_Get_yPixelOffset());
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 8), 3);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 9), NeedNormalMapping);
					vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 10), PCFMode);
					break;
				}
			}
		}

		int GlobalVertexCount{0};
		for (auto &tmpObjectBlock : ObjectBlocks) {
			GlobalVertexCount += tmpObjectBlock.VertexQuantity;
		}

		// часть данных берем из 1-го объекта, т.к. они идентичны для всей модели
		vw_Draw3D(ePrimitiveType::TRIANGLES, GlobalVertexCount, ObjectBlocks[0].VertexFormat, GlobalVertexArray.get(),
			  ObjectBlocks[0].VertexStride * sizeof(float), GlobalVBO, 0,
			  GlobalIndexArray.get(), GlobalIBO, GlobalVAO);

		vw_DeActivateAllLights();
	} else {

		// текущая текстура, чтобы не переставлять по 10 раз одно и то же
		GLtexture CurrentTexture{0};

		// установка текстур и подхотовка к прорисовке
		for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {

			// небольшая проверка для конкретной части
			if (!HitBB.empty()) {
				sVECTOR3D Min, Max;
				Min.x = Max.x = HitBB[i].Box[0].x + HitBB[i].Location.x;
				Min.y = Max.y = HitBB[i].Box[0].y + HitBB[i].Location.y;
				Min.z = Max.z = HitBB[i].Box[0].z + HitBB[i].Location.z;
				// берем и проверяем
				for (int j = 0; j < 8; j++) {
					if (Min.x > HitBB[i].Box[j].x + HitBB[i].Location.x)
						Min.x = HitBB[i].Box[j].x + HitBB[i].Location.x;
					if (Min.y > HitBB[i].Box[j].y + HitBB[i].Location.y)
						Min.y = HitBB[i].Box[j].y + HitBB[i].Location.y;
					if (Min.z > HitBB[i].Box[j].z + HitBB[i].Location.z)
						Min.z = HitBB[i].Box[j].z + HitBB[i].Location.z;
					if (Max.x < HitBB[i].Box[j].x + HitBB[i].Location.x)
						Max.x = HitBB[i].Box[j].x + HitBB[i].Location.x;
					if (Max.y < HitBB[i].Box[j].y + HitBB[i].Location.y)
						Max.y = HitBB[i].Box[j].y + HitBB[i].Location.y;
					if (Max.z < HitBB[i].Box[j].z + HitBB[i].Location.z)
						Max.z = HitBB[i].Box[j].z + HitBB[i].Location.z;
				}

				if (!vw_BoxInFrustum(Location + Min, Location + Max))
					continue;
			}

			// работа с текстурами
			if (CurrentTexture != Texture[i]) {
				vw_BindTexture(0, Texture[i]);

				// если есть тайловая анимация - работаем с текстурной матрицей
				// ! исходим из того что активен всегда 0-вой стейдж текстуры т.к. у танков на траках нет илюминейшен текстуры
				if (ObjectBlocks[i].NeedTextureAnimation) {
					vw_MatrixMode(eMatrixMode::TEXTURE);
					vw_LoadIdentity();
					vw_Translate(ObjectBlocks[i].TextureAnimation);
					vw_MatrixMode(eMatrixMode::MODELVIEW);
				}

				// включаем вторую текстуру
				if ((TextureIllum.size() > static_cast<unsigned>(i)) && TextureIllum[i]) {
					// свечение
					vw_BindTexture(1, TextureIllum[i]);
					// для корректной прорисовки без шейдеров, ставим правильный режим смешивания
					vw_SetTextureEnvMode(eTextureEnvMode::COMBINE);
					vw_SetTextureBlendMode(eTextureCombinerName::COMBINE_RGB, eTextureCombinerOp::ADD);
				}

				// если у нас работают шейдеры и есть текстура нормал меппа - ставим ее
				NeedNormalMapping = 0;
				if (GameConfig().UseGLSL120 &&
				    (NormalMap.size() > static_cast<unsigned>(i))) {
					if (NormalMap[i]) {
						NeedNormalMapping = 1;
						CurrentNormalMap = NormalMap[i];
						vw_BindTexture(3, CurrentNormalMap);
					} else if (CurrentNormalMap) { // если нет, но был установлен - нужно сделать сброс установки
						vw_BindTexture(3, 0);
						CurrentNormalMap = 0;
					}
				}

				CurrentTexture = Texture[i];
			}

			vw_PushMatrix();

			// сдвигаем его в нужное место
			vw_Translate(ObjectBlocks[i].Location);
			// поворачиваем объект
			vw_Rotate(ObjectBlocks[i].Rotation.z, 0.0f, 0.0f, 1.0f);
			vw_Rotate(ObjectBlocks[i].Rotation.y, 0.0f, 1.0f, 0.0f);
			vw_Rotate(ObjectBlocks[i].Rotation.x, 1.0f, 0.0f, 0.0f);

			// если нужна дополнительная анимация геометрией
			if (ObjectBlocks[i].NeedGeometryAnimation) {
				vw_Rotate(ObjectBlocks[i].GeometryAnimation.z, 0.0f, 0.0f, 1.0f);
				vw_Rotate(ObjectBlocks[i].GeometryAnimation.y, 0.0f, 1.0f, 0.0f);
				vw_Rotate(ObjectBlocks[i].GeometryAnimation.x, 1.0f, 0.0f, 0.0f);
			}

			int LightType1, LightType2;
			// включаем источники света
			if (!HitBB.empty())
				vw_CheckAndActivateAllLights(LightType1, LightType2, Location + HitBB[i].Location, HitBB[i].Radius2, 2, GameConfig().MaxPointLights, Matrix);
			else
				vw_CheckAndActivateAllLights(LightType1, LightType2, Location, Radius * Radius, 2, GameConfig().MaxPointLights, Matrix);

			// если нужно рисовать прозрачным
			// для корректной прорисовки на всех видеокартах атмосферы планеты ...
			if (N1)
				vw_PolygonOffset(true, 2.0f, 2.0f);

			if (ObjectBlocks[i].DrawType == eObjectDrawType::Blend) {
				vw_SetTextureAlphaTest(true, eCompareFunc::GREATER, 0.01f);
				vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);
				vw_PolygonOffset(true, 1.0f, 1.0f);
			}

			if (GameConfig().UseGLSL120 &&
			    (ObjectBlocks[i].ShaderType >= 0)) {
				std::weak_ptr<cGLSL> CurrentObject3DGLSL{};

				// небольшая корректировка, если 1-й шейдер (попиксельное освещение), но передали шадовмеп - ставим 3
				if ((ObjectBlocks[i].ShaderType == 1) && ShadowMap)
					ObjectBlocks[i].ShaderType = 3;
				// и на оборот, если стоит 3-й, но шадовмепа нет - ставим 1-й, просто попиксельное освещение
				if ((ObjectBlocks[i].ShaderType == 3) && !ShadowMap)
					ObjectBlocks[i].ShaderType = 1;

				// ставим нужный шейдер
				switch (ObjectBlocks[i].ShaderType) {
				case 1:
					CurrentObject3DGLSL = GLSLShaderType1;
					break;
				case 2:
					CurrentObject3DGLSL = GLSLShaderType2;
					break;
				case 3:
					CurrentObject3DGLSL = GLSLShaderType3;
					break;
				}

				if (CurrentGLSL.lock() != CurrentObject3DGLSL.lock()) {
					if (!CurrentGLSL.expired())
						vw_StopShaderProgram();

					CurrentGLSL = CurrentObject3DGLSL;

					if (!CurrentGLSL.expired())
						vw_UseShaderProgram(CurrentGLSL);
				}

				// данные ставим каждый раз, т.к. может что-то поменяться
				if (auto sharedGLSL = CurrentObject3DGLSL.lock()) {
					switch (ObjectBlocks[i].ShaderType) {
					case 1: // только попиксельное освещение
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), LightType1);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), LightType2);
						if (!TextureIllum.empty() && TextureIllum[0])
							vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 1);
						else
							vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 0);

						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 5), 3);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 6), NeedNormalMapping);
						break;

					case 2: // шейдеры взрывов
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), LightType1);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), LightType2);
						vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 3), ObjectBlocks[0].ShaderData[0]);
						vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 4), ObjectBlocks[0].ShaderData[1]);
						break;

					case 3: // шадов меп
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 0), 0);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 1), 1);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 2), LightType1);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 3), LightType2);
						if (!TextureIllum.empty() && TextureIllum[0])
							vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 1);
						else
							vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 4), 0);

						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 5), 2);
						vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 6), ShadowMap_Get_xPixelOffset());
						vw_Uniform1f(vw_GetShaderUniformLocation(sharedGLSL, 7), ShadowMap_Get_yPixelOffset());
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 8), 3);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 9), NeedNormalMapping);
						vw_Uniform1i(vw_GetShaderUniformLocation(sharedGLSL, 10), PCFMode);
						break;
					}
				}
			}

			vw_Draw3D(ePrimitiveType::TRIANGLES, ObjectBlocks[i].VertexQuantity, ObjectBlocks[i].VertexFormat, ObjectBlocks[i].VertexArray.get(),
				  ObjectBlocks[i].VertexStride * sizeof(float), ObjectBlocks[i].VBO,
				  ObjectBlocks[i].RangeStart, ObjectBlocks[i].IndexArray.get(), ObjectBlocks[i].IBO, ObjectBlocks[i].VAO);

			if (ObjectBlocks[i].DrawType == eObjectDrawType::Blend) {
				vw_SetTextureAlphaTest(false, eCompareFunc::ALWAYS, 0);
				vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
				vw_PolygonOffset(false, 0.0f, 0.0f);
			}

			vw_DeActivateAllLights();

			// если меняли текстурную матрицу - обязательно восстанавливаем
			if (ObjectBlocks[i].NeedTextureAnimation) {
				vw_BindTexture(0, 0);
				vw_MatrixMode(eMatrixMode::TEXTURE);
				vw_LoadIdentity();
				vw_MatrixMode(eMatrixMode::MODELVIEW);
				CurrentTexture = 0;
			}

			vw_PopMatrix();
		}
	}

	// останавливаем шейдер, если был запущен
	if (GameConfig().UseGLSL120)
		vw_StopShaderProgram();

	// установка параметров текстур в исходное состояние
	if (CurrentNormalMap)
		vw_BindTexture(3, 0);
	vw_BindTexture(1, 0);
	vw_BindTexture(0, 0);
	if (NeedAlphaTest)
		vw_SetTextureAlphaTest(false, eCompareFunc::ALWAYS, 0);
	if (!NeedCullFaces)
		vw_CullFace(eCullFace::BACK);
	vw_PopMatrix();

#ifndef NDEBUG
	// debug info, line number in script file
	if (!ScriptLineNumberUTF32.empty())
		vw_DrawText3DUTF32(Location.x, Location.y + AABB[0].y, Location.z, ScriptLineNumberUTF32);
#endif // NDEBUG

	if (NeedShowBB >= 1)
		// AABB объекта
		DrawBoxLines(AABB, Location, 1.0f, 0.0f, 0.0f, 1.0f);
	if (NeedShowBB >= 2)
		// OBB объекта
		DrawBoxLines(OBB.Box, Location + OBB.Location, 0.0f, 1.0f, 0.0f, 1.0f);
	if ((NeedShowBB >= 3) && !HitBB.empty()) {
		for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {
			DrawBoxLines(HitBB[i].Box, Location + HitBB[i].Location, 0.0f, 0.0f, 1.0f, 1.0f);
		}
	}

	// выводим "жизнь", если нужно
	if (!ShowStrength ||
	    (StrengthStart <= 0.0f) ||
	    ((Strength == StrengthStart) && (ShieldStrength == ShieldStrengthStart) && !NeedShowStrengthNow))
		return;

	// раз тут, значит показали, и нужно теперь и дальше всегда показывать, даже если щит перезарядили
	NeedShowStrengthNow = true;

	// буфер для последовательности TRIANGLE_STRIP
	// войдет RI_3f_XYZ | RI_4f_COLOR
	float *tmpDATA = new float[4 * (3 + 4)];
	int k = 0;

	// рисуем заднюю часть - основу
	float SizeX = Width / 2.5f + 0.1f;
	float SizeY = 0.35f;
	float ColorR = 0.0f;
	float ColorG = 0.0f;
	float ColorB = 0.0f;
	float ColorA = 1.0f;

	tmpDATA[k++] = -SizeX;		// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;


	tmpDATA[k++] = -SizeX;		// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeX;		// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeX;		// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k] = ColorA;

	vw_PushMatrix();
	sVECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);
	// поднимаем
	vw_Translate(sVECTOR3D(Location.x, Location.y + AABB[0].y + SizeY * 2.0f, Location.z));
	// поворачиваем к камере
	vw_Rotate(-180+CurrentCameraRotation.y, 0.0f, 1.0f, 0.0f);
	vw_Rotate(-CurrentCameraRotation.x, 1.0f, 0.0f, 0.0f);

	vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7 * sizeof(tmpDATA[0]));

	// рисуем вывод кол-ва жизни
	k = 0;
	float SizeXStart = Width / 2.5f - (2.0f * Width / 2.5f) * Strength / StrengthStart;
	float SizeXEnd = Width / 2.5f;
	SizeY = 0.25f;
	ColorR = 1.0f;
	ColorG = 0.0f;
	ColorB = 0.0f;
	ColorA = 1.0f;

	tmpDATA[k++] = SizeXStart;	// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeXStart;	// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeXEnd;	// X
	tmpDATA[k++] = -SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k++] = ColorA;

	tmpDATA[k++] = SizeXEnd;	// X
	tmpDATA[k++] = SizeY;		// Y
	tmpDATA[k++] = 0.0f;		// Z
	tmpDATA[k++] = ColorR;
	tmpDATA[k++] = ColorG;
	tmpDATA[k++] = ColorB;
	tmpDATA[k] = ColorA;

	vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7 * sizeof(tmpDATA[0]));

	vw_PopMatrix();

	// выводим щит, если он есть
	if (ShieldStrengthStart > 0.0f) {
		k = 0;

		// рисуем заднюю часть - основу
		SizeX = Width / 2.5f + 0.1f;
		SizeY = 0.35f;
		ColorR = 0.0f;
		ColorG = 0.0f;
		ColorB = 0.0f;
		ColorA = 1.0f;

		tmpDATA[k++] = -SizeX;		// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;


		tmpDATA[k++] = -SizeX;		// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeX;		// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeX;		// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k] = ColorA;

		vw_PushMatrix();
		// поднимаем
		vw_Translate(sVECTOR3D(Location.x, Location.y + AABB[0].y + SizeY * 5.0f, Location.z));
		// поворачиваем к камере
		vw_Rotate(-180+CurrentCameraRotation.y, 0.0f, 1.0f, 0.0f);
		vw_Rotate(-CurrentCameraRotation.x, 1.0f, 0.0f, 0.0f);

		vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7 * sizeof(tmpDATA[0]));

		// рисуем вывод кол-ва жизни
		k=0;
		SizeXStart = Width / 2.5f - (2.0f * Width / 2.5f) * ShieldStrength / ShieldStrengthStart;
		SizeXEnd = Width / 2.5f;
		SizeY = 0.25f;
		ColorR = 0.1f;
		ColorG = 0.7f;
		ColorB = 1.0f;
		ColorA = 1.0f;

		tmpDATA[k++] = SizeXStart;	// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeXStart;	// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeXEnd;	// X
		tmpDATA[k++] = -SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k++] = ColorA;

		tmpDATA[k++] = SizeXEnd;	// X
		tmpDATA[k++] = SizeY;		// Y
		tmpDATA[k++] = 0.0f;		// Z
		tmpDATA[k++] = ColorR;
		tmpDATA[k++] = ColorG;
		tmpDATA[k++] = ColorB;
		tmpDATA[k] = ColorA;

		vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_4f_COLOR, tmpDATA, 7 * sizeof(tmpDATA[0]));

		vw_PopMatrix();
	}

	delete [] tmpDATA;

	vw_BindTexture(0, 0);
}

//-----------------------------------------------------------------------------
// Обновление данных объектa Object3D
//-----------------------------------------------------------------------------
bool cObject3D::Update(float Time)
{
	// если еще ни разу не обновляли - просто запоминаем время
	if (TimeLastUpdate == -1.0f) {
		TimeLastUpdate = Time;
		return true;
	}

	// нужно удалить объект - он вышел из зоны видемости
	if (DeleteAfterLeaveScene == eDeleteAfterLeaveScene::need_delete) {
		Lifetime = DeleteAfterLeaveSceneDelay;
		DeleteAfterLeaveScene = eDeleteAfterLeaveScene::wait_delay;
	}

	// находим дельту по времени, и запоминаем время вызова
	TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f)
		return true;

	TimeLastUpdate = Time;

	// проверяем, сколько объекту жить, если нужно...-1.0f  - проверка не нужна
	if (Lifetime > -1.0f) {
		// считаем, сколько осталось жить
		Lifetime -= TimeDelta;
		// если уже ничего не осталось - его нужно уничтожить
		if (Lifetime <= 0.0f)
			return false;
	}

	// проверка, зарядка щита если он есть
	if ((ShieldStrengthStart > 0.0f) &&
	    (ShieldStrength < ShieldStrengthStart)) {
		ShieldStrength += ShieldRecharge * TimeDelta;
		if (ShieldStrength > ShieldStrengthStart)
			ShieldStrength = ShieldStrengthStart;
	}

	// if we have TimeSheet with actions and this is not a cycled entry
	if (!TimeSheetList.empty() &&
	    (TimeSheetList.front().Time > -1.0f)) {
		TimeSheetList.front().Time -= TimeDelta;
		// if this entry is out of time, remove it
		if (TimeSheetList.front().Time <= 0.0f) {
			// correct time delta
			if (TimeSheetList.front().Time < 0.0f)
				TimeDelta += TimeSheetList.front().Time;
			TimeSheetList.pop_front();
		}
	}
	// should be unpacked
	if (!TimeSheetList.empty() &&
	    (TimeSheetList.front().AI_Mode != 0)) {
		InterAIMode(TimeSheetList);
		// since we already unpack this entry, remove it
		TimeSheetList.pop_front();
	}

	return true;
}
