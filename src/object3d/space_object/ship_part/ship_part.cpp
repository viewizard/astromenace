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

#include "ship_part.h"





//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
void cShipPart::Create(int ShipPartNum)
{
	ObjectType = eObjectType::ShipPart;
	InternalType = ShipPartNum;

	// тут все делаем сами...
	// т.к. это часть большого корабля


}

//-----------------------------------------------------------------------------
// Установка AABB, OBB и габаритов по геометрии объекта
//-----------------------------------------------------------------------------
void cShipPart::InitByDrawObjectList()
{
	if (ObjectBlocks.empty())
		return;

	int AllVertexCounted{0};

	// HitBB calculation
	HitBB.resize(ObjectBlocks.size());
	for (unsigned int i = 0; i < ObjectBlocks.size(); i++) {
		float Matrix[9];
		vw_Matrix33CreateRotate(Matrix, ObjectBlocks[i].Rotation);

		// first vertex's data as initial data for calculation
		int tmpOffset;
		if (ObjectBlocks[i].IndexArray)
			tmpOffset = ObjectBlocks[i].IndexArray.get()[ObjectBlocks[i].RangeStart] *
				    ObjectBlocks[i].VertexStride;
		else
			tmpOffset = ObjectBlocks[i].RangeStart * ObjectBlocks[i].VertexStride;

		sVECTOR3D tmpVertex;
		tmpVertex.x = ObjectBlocks[i].VertexArray.get()[tmpOffset];
		tmpVertex.y = ObjectBlocks[i].VertexArray.get()[tmpOffset + 1];
		tmpVertex.z = ObjectBlocks[i].VertexArray.get()[tmpOffset + 2];
		vw_Matrix33CalcPoint(tmpVertex, Matrix);
		float MinX = tmpVertex.x;
		float MaxX = tmpVertex.x;
		float MinY = tmpVertex.y;
		float MaxY = tmpVertex.y;
		float MinZ = tmpVertex.z;
		float MaxZ = tmpVertex.z;

		GeometryCenter += ObjectBlocks[i].Location + tmpVertex;

		// starts from second vertex, check all vertices
		for (unsigned int j = 1; j < ObjectBlocks[i].VertexQuantity; j++) {
			if (ObjectBlocks[i].IndexArray)
				tmpOffset = ObjectBlocks[i].IndexArray.get()[ObjectBlocks[i].RangeStart + j] *
					    ObjectBlocks[i].VertexStride;
			else
				tmpOffset = (ObjectBlocks[i].RangeStart + j) * ObjectBlocks[i].VertexStride;

			tmpVertex.x = ObjectBlocks[i].VertexArray.get()[tmpOffset];
			tmpVertex.y = ObjectBlocks[i].VertexArray.get()[tmpOffset + 1];
			tmpVertex.z = ObjectBlocks[i].VertexArray.get()[tmpOffset + 2];
			vw_Matrix33CalcPoint(tmpVertex, Matrix);
			if (MinX > tmpVertex.x)
				MinX = tmpVertex.x;
			if (MinY > tmpVertex.y)
				MinY = tmpVertex.y;
			if (MinZ > tmpVertex.z)
				MinZ = tmpVertex.z;
			if (MaxX < tmpVertex.x)
				MaxX = tmpVertex.x;
			if (MaxY < tmpVertex.y)
				MaxY = tmpVertex.y;
			if (MaxZ < tmpVertex.z)
				MaxZ = tmpVertex.z;

			GeometryCenter += ObjectBlocks[i].Location + tmpVertex;
		}

		// store data to the HitBB
		HitBB[i].Box[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		HitBB[i].Box[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		HitBB[i].Box[2] = sVECTOR3D(MinX, MaxY, MinZ);
		HitBB[i].Box[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		HitBB[i].Box[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		HitBB[i].Box[5] = sVECTOR3D(MinX, MinY, MaxZ);
		HitBB[i].Box[6] = sVECTOR3D(MinX, MinY, MinZ);
		HitBB[i].Box[7] = sVECTOR3D(MaxX, MinY, MinZ);

		// calculate HitBB geometry center
		HitBB[i].Location.x = (MaxX + MinX) / 2.0f;
		HitBB[i].Location.y = (MaxY + MinY) / 2.0f;
		HitBB[i].Location.z = (MaxZ + MinZ) / 2.0f;

		// calculate HitBB size
		HitBB[i].Size.x = fabsf(MaxX - MinX);
		HitBB[i].Size.y = fabsf(MaxY - MinY);
		HitBB[i].Size.z = fabsf(MaxZ - MinZ);

		// calculate HitBB radius square
		HitBB[i].Radius2 = (HitBB[i].Size.x / 2.0f) * (HitBB[i].Size.x / 2.0f) +
				   (HitBB[i].Size.y / 2.0f) * (HitBB[i].Size.y / 2.0f) +
				   (HitBB[i].Size.z / 2.0f) * (HitBB[i].Size.z / 2.0f);

		// calculate HitBB points accordingly to HitBB geometry center location
		for (int k = 0; k < 8; k++) {
			HitBB[i].Box[k] -= HitBB[i].Location;
		}

		// correct HitBB geometry center location accordingly to object location
		HitBB[i].Location += ObjectBlocks[i].Location;

		AllVertexCounted += ObjectBlocks[i].VertexQuantity;
	}

	// calculate 3D model's geometry center
	if (AllVertexCounted > 0)
		GeometryCenter = GeometryCenter / AllVertexCounted;

	// first HitBB's data as initial data for calculation
	float MinX = HitBB[0].Box[6].x + HitBB[0].Location.x;
	float MaxX = HitBB[0].Box[0].x + HitBB[0].Location.x;
	float MinY = HitBB[0].Box[6].y + HitBB[0].Location.y;
	float MaxY = HitBB[0].Box[0].y + HitBB[0].Location.y;
	float MinZ = HitBB[0].Box[6].z + HitBB[0].Location.z;
	float MaxZ = HitBB[0].Box[0].z + HitBB[0].Location.z;

	// starts from second HitBB, check all HitBBs
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

	// store data to OBB and AABB, since on this stage they are identical
	OBB.Box[0] = AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
	OBB.Box[1] = AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
	OBB.Box[2] = AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
	OBB.Box[3] = AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
	OBB.Box[4] = AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
	OBB.Box[5] = AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
	OBB.Box[6] = AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
	OBB.Box[7] = AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);

	// calculate OBB geometry center
	OBB.Location.x = (MaxX + MinX) / 2.0f;
	OBB.Location.y = (MaxY + MinY) / 2.0f;
	OBB.Location.z = (MaxZ + MinZ) / 2.0f;

	// calculate OBB points accordingly to OBB geometry center location
	for (int k = 0; k < 8; k++) {
		OBB.Box[k] -= OBB.Location;
	}

	// calculate 3D model's size
	Width = fabsf(MaxX - MinX);
	Height = fabsf(MaxY - MinY);
	Length = fabsf(MaxZ - MinZ);

	// calculate 3D model's radius square
	float Width2 = Width / 2.0f;
	float Length2 = Length / 2.0f;
	float Height2 = Height / 2.0f;
	Radius = vw_sqrtf(Width2 * Width2 + Length2 * Length2 + Height2 * Height2);
}
