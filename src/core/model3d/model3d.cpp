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

// TODO translate comments

#include "../graphics/graphics.h"
#include "../math/math.h"
#include "../vfs/vfs.h"
#include "model3d.h"

namespace {

cModel3D *StartModel3D = nullptr;
cModel3D *EndModel3D = nullptr;

} // unnamed namespace


//-----------------------------------------------------------------------------
// Нахождение геометрии, или ее загрузка
//-----------------------------------------------------------------------------
cModel3D *vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal)
{
	if (FileName.empty())
		return nullptr;

	// сначала пытаемся найти уже сущ.
	cModel3D *tmp = StartModel3D;
	while (tmp) {
		cModel3D *tmp2 = tmp->Next;
		if (tmp->Name == FileName)
			return tmp;
		tmp = tmp2;
	}

	// если ничего нет, значит нужно загрузить
	cModel3D *Model = new cModel3D;

	// check extension
	if (vw_CheckFileExtension(FileName, ".vw3d")) {
		if (!Model->ReadVW3D(FileName)) {
			std::cout << "Can't load file ... " << FileName << "\n";
			delete Model;
			return nullptr;
		}
	} else {
		std::cerr << __func__ << "(): " << "Format not supported " << FileName << "\n";
		delete Model;
		return nullptr;
	}

	// пересоздаем буфер вертексов, для работы с нормал меппингом в шейдерах, добавляем тангент и бинормаль
	if (NeedTangentAndBinormal)
		Model->CreateTangentAndBinormal();
	// создаем вертексные и индексные буферы для каждого блока
	Model->CreateObjectsBuffers();
	// создаем все поддерживаемые буферы (VAO, VBO, IBO)
	Model->CreateHardwareBuffers();

	// делаем спец буфер для разрушаемых объектов
	// (!) используем фиксированную последовательность RI_3f_XYZ | RI_3f_NORMAL | RI_2f_TEX
	// с которой работают взрывы в игре, не делаем универсальную (нет необходимости)
	Model->CreateVertexBufferLimitedBySizeTriangles(TriangleSizeLimit);

	std::cout << "Loaded ... " << FileName << "\n";

	return Model;
}

//-----------------------------------------------------------------------------
//	Присоеденяем Model3D к списку
//-----------------------------------------------------------------------------
static void AttachModel3D(cModel3D *NewModel3D)
{
	if (NewModel3D == nullptr)
		return;

	if (EndModel3D == nullptr) {
		NewModel3D->Prev = nullptr;
		NewModel3D->Next = nullptr;
		StartModel3D = NewModel3D;
		EndModel3D = NewModel3D;
	} else {
		NewModel3D->Prev = EndModel3D;
		NewModel3D->Next = nullptr;
		EndModel3D->Next = NewModel3D;
		EndModel3D = NewModel3D;
	}
}

//-----------------------------------------------------------------------------
//	Удаляем Model3D из списка
//-----------------------------------------------------------------------------
static void DetachModel3D(cModel3D *OldModel3D)
{
	if (OldModel3D == nullptr)
		return;

	if (StartModel3D == OldModel3D)
		StartModel3D = OldModel3D->Next;
	if (EndModel3D == OldModel3D)
		EndModel3D = OldModel3D->Prev;

	if (OldModel3D->Next != nullptr)
		OldModel3D->Next->Prev = OldModel3D->Prev;
	else if (OldModel3D->Prev != nullptr)
		OldModel3D->Prev->Next = nullptr;

	if (OldModel3D->Prev != nullptr)
		OldModel3D->Prev->Next = OldModel3D->Next;
	else if (OldModel3D->Next != nullptr)
		OldModel3D->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
//	Удаляем все Model3D в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllModel3D()
{
	cModel3D *tmp = StartModel3D;
	while (tmp) {
		cModel3D *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartModel3D = nullptr;
	EndModel3D = nullptr;
}

//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
sObjectBlock::~sObjectBlock(void)
{
	// если нужно, удаляем с блоком
	if (NeedDestroyDataInObjectBlock) {
		if (VertexArray)
			delete [] VertexArray;
		if (VBO)
			vw_DeleteBufferObject(VBO);
		if (IndexArray)
			delete [] IndexArray;
		if (IBO)
			vw_DeleteBufferObject(IBO);
		if (VAO)
			vw_DeleteVAO(VAO);

		// память для VertexBufferLimitedBySizeTriangles никогда не должена быть выделена в конкретном объекте,
		// только в моделе его удаляем только в деструкторе eModel3D, т.к. он там создается
	}
}

//-----------------------------------------------------------------------------
// Конструктор
//-----------------------------------------------------------------------------
cModel3D::cModel3D(void)
{
	AttachModel3D(this);
}

//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cModel3D::~cModel3D(void)
{
	if (DrawObjectList) {
		for (int i = 0; i < DrawObjectCount; i++) {
			// удаляем только если это не ссылка на вертекс буфер блока
			if (DrawObjectList[i].VertexBufferLimitedBySizeTriangles &&
			    (DrawObjectList[i].VertexBufferLimitedBySizeTriangles != DrawObjectList[i].VertexArray))
				delete [] DrawObjectList[i].VertexBufferLimitedBySizeTriangles;
			if (DrawObjectList[i].VertexArray &&
			    (DrawObjectList[i].VertexArray != GlobalVertexArray))
				delete [] DrawObjectList[i].VertexArray;
			if (DrawObjectList[i].IndexArray &&
			    (DrawObjectList[i].IndexArray != GlobalIndexArray))
				delete [] DrawObjectList[i].IndexArray;
			if (DrawObjectList[i].VBO && (DrawObjectList[i].VBO != GlobalVBO))
				vw_DeleteBufferObject(DrawObjectList[i].VBO);
			if (DrawObjectList[i].IBO && (DrawObjectList[i].IBO != GlobalIBO))
				vw_DeleteBufferObject(DrawObjectList[i].IBO);
			if (DrawObjectList[i].VAO && (DrawObjectList[i].VAO != GlobalVAO))
				vw_DeleteVAO(DrawObjectList[i].VAO);
		}
		delete [] DrawObjectList;
	}
	if (GlobalVertexArray)
		delete [] GlobalVertexArray;
	if (GlobalVBO)
		vw_DeleteBufferObject(GlobalVBO);
	if (GlobalIndexArray)
		delete [] GlobalIndexArray;
	if (GlobalIBO)
		vw_DeleteBufferObject(GlobalIBO);
	if (GlobalVAO)
		vw_DeleteVAO(GlobalVAO);

	DrawObjectCount = 0;
	DetachModel3D(this);
}

//-----------------------------------------------------------------------------
// создание вертекс и индекс буферов для каждого блока модели
//-----------------------------------------------------------------------------
void cModel3D::CreateObjectsBuffers()
{
	for (int i = 0; i < DrawObjectCount; i++) {
		// создаем вертексный буфер блока
		DrawObjectList[i].VertexArray = new float[DrawObjectList[i].VertexStride * DrawObjectList[i].VertexCount];
		for (int j = 0; j < DrawObjectList[i].VertexCount; j++) {
			memcpy(DrawObjectList[i].VertexArray + DrawObjectList[i].VertexStride * j,
			       GlobalVertexArray + GlobalIndexArray[DrawObjectList[i].RangeStart + j] * DrawObjectList[i].VertexStride,
			       DrawObjectList[i].VertexStride * sizeof(DrawObjectList[i].VertexArray[0]));
		}

		// создаем индексный буфер блока
		DrawObjectList[i].IndexArray = new unsigned int[DrawObjectList[i].VertexCount];
		for (int j = 0; j < DrawObjectList[i].VertexCount; j++) {
			DrawObjectList[i].IndexArray[j] = j;
		}

		// т.к. у нас отдельные буферы, то начало идет с нуля теперь
		DrawObjectList[i].RangeStart = 0;
	}
}

//-----------------------------------------------------------------------------
// создаем все поддерживаемые буферы (VAO, VBO, IBO)
//-----------------------------------------------------------------------------
void cModel3D::CreateHardwareBuffers()
{
	// делаем общее VBO
	if (!vw_BuildVertexBufferObject(GlobalVertexCount, GlobalVertexArray, DrawObjectList[0].VertexStride, GlobalVBO))
		GlobalVBO = 0;

	// делаем общий IBO
	if (!vw_BuildIndexBufferObject(GlobalIndexCount, GlobalIndexArray, GlobalIBO))
		GlobalIBO = 0;

	if (!vw_BuildVAO(GlobalVAO, GlobalIndexCount, DrawObjectList[0].VertexFormat, GlobalVertexArray,
			 DrawObjectList[0].VertexStride * sizeof(float), GlobalVBO, 0, GlobalIndexArray, GlobalIBO))
		GlobalVAO = 0;

	// создаем буферы для каждого объекта
	for (int i = 0; i < DrawObjectCount; i++) {
		// делаем VBO
		if (!vw_BuildVertexBufferObject(DrawObjectList[i].VertexCount, DrawObjectList[i].VertexArray,
						DrawObjectList[i].VertexStride, DrawObjectList[i].VBO))
			DrawObjectList[i].VBO = 0;

		// делаем IBO
		if (!vw_BuildIndexBufferObject(DrawObjectList[i].VertexCount, DrawObjectList[i].IndexArray,
					       DrawObjectList[i].IBO))
			DrawObjectList[i].IBO = 0;

		// делаем VAO
		if (!vw_BuildVAO(DrawObjectList[i].VAO, DrawObjectList[i].VertexCount, DrawObjectList[i].VertexFormat,
				 DrawObjectList[i].VertexArray, DrawObjectList[i].VertexStride * sizeof(DrawObjectList[i].VertexArray[0]),
				 DrawObjectList[i].VBO, DrawObjectList[i].RangeStart, DrawObjectList[i].IndexArray,
				 DrawObjectList[i].IBO))
			DrawObjectList[i].VAO = 0;
	}
}

//-----------------------------------------------------------------------------
// рекурсивно просчитываем кол-во VertexBufferLimitedBySizeTriangles
//-----------------------------------------------------------------------------
static int RecursiveBufferLimitedBySizeTrianglesCalculate(float (&Point1)[8], float (&Point2)[8], float (&Point3)[8],
							  int Stride, float *VertexBuffer, int *CurrentPosition,
							  float TriangleSizeLimit)
{
	// подсчитываем длину сторон, если они меньше чем необходимый минимум - выходим с 1
	float Dist1 = sqrtf((Point1[0] - Point2[0]) * (Point1[0] - Point2[0]) +
			    (Point1[1] - Point2[1]) * (Point1[1] - Point2[1]) +
			    (Point1[2] - Point2[2]) * (Point1[2] - Point2[2]));
	float Dist2 = sqrtf((Point2[0] - Point3[0]) * (Point2[0] - Point3[0]) +
			    (Point2[1] - Point3[1]) * (Point2[1] - Point3[1]) +
			    (Point2[2] - Point3[2]) * (Point2[2] - Point3[2]));
	float Dist3 = sqrtf((Point3[0] - Point1[0]) * (Point3[0] - Point1[0]) +
			    (Point3[1] - Point1[1]) * (Point3[1] - Point1[1]) +
			    (Point3[2] - Point1[2]) * (Point3[2] - Point1[2]));

	if ((Dist1 <= TriangleSizeLimit) && (Dist2 <= TriangleSizeLimit) && (Dist3 <= TriangleSizeLimit)) {
		// добавляем в новую последовательность треугольник
		if (VertexBuffer && CurrentPosition) {
			memcpy(VertexBuffer + (*CurrentPosition), Point1, sizeof(Point1[0]) * 8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer + (*CurrentPosition), Point2, sizeof(Point2[0]) * 8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer + (*CurrentPosition), Point3, sizeof(Point3[0]) * 8);
			*CurrentPosition += Stride;
		}
		return 1;
	}

	// одна из сторон больше, ищем наибольшую, делим ее пополам и идем дальше в рекурсию
	if ((Dist1 > Dist2) && (Dist1 > Dist3)) {
		float Point_A[8]= {(Point1[0] + Point2[0]) / 2.0f,
				   (Point1[1] + Point2[1]) / 2.0f,
				   (Point1[2] + Point2[2]) / 2.0f,
				   (Point1[3] + Point2[3]) / 2.0f,
				   (Point1[4] + Point2[4]) / 2.0f,
				   (Point1[5] + Point2[5]) / 2.0f,
				   (Point1[6] + Point2[6]) / 2.0f,
				   (Point1[7] + Point2[7]) / 2.0f};

		return RecursiveBufferLimitedBySizeTrianglesCalculate(Point1, Point_A, Point3,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit) +
		       RecursiveBufferLimitedBySizeTrianglesCalculate(Point_A, Point2, Point3,
				       Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
	} else if ((Dist2 > Dist1) && (Dist2 > Dist3)) {
		float Point_A[8]= {(Point2[0] + Point3[0]) / 2.0f,
				   (Point2[1] + Point3[1]) / 2.0f,
				   (Point2[2] + Point3[2]) / 2.0f,
				   (Point2[3] + Point3[3]) / 2.0f,
				   (Point2[4] + Point3[4]) / 2.0f,
				   (Point2[5] + Point3[5]) / 2.0f,
				   (Point2[6] + Point3[6]) / 2.0f,
				   (Point2[7] + Point3[7]) / 2.0f};

		return RecursiveBufferLimitedBySizeTrianglesCalculate(Point1, Point2, Point_A,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit) +
		       RecursiveBufferLimitedBySizeTrianglesCalculate(Point1, Point_A, Point3,
				       Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
	} else {
		float Point_A[8]= {(Point3[0] + Point1[0]) / 2.0f,
				   (Point3[1] + Point1[1]) / 2.0f,
				   (Point3[2] + Point1[2]) / 2.0f,
				   (Point3[3] + Point1[3]) / 2.0f,
				   (Point3[4] + Point1[4]) / 2.0f,
				   (Point3[5] + Point1[5]) / 2.0f,
				   (Point3[6] + Point1[6]) / 2.0f,
				   (Point3[7] + Point1[7]) / 2.0f};

		return RecursiveBufferLimitedBySizeTrianglesCalculate(Point1, Point2, Point_A,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit) +
		       RecursiveBufferLimitedBySizeTrianglesCalculate(Point_A, Point2, Point3,
				       Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
	}
}

//-----------------------------------------------------------------------------
// рекурсивно создаем VertexBufferLimitedBySizeTriangles
//-----------------------------------------------------------------------------
void RecursiveBufferLimitedBySizeTrianglesGenerate(float Point1[8], float Point2[8], float Point3[8],
		int Stride, float *VertexBuffer, int *CurrentPosition, float TriangleSizeLimit)
{
	// подсчитываем длину сторон, если они меньше чем необходимый минимум - выходим с 1
	float Dist1 = sqrtf((Point1[0] - Point2[0]) * (Point1[0] - Point2[0]) +
			    (Point1[1] - Point2[1]) * (Point1[1] - Point2[1]) +
			    (Point1[2] - Point2[2]) * (Point1[2] - Point2[2]));
	float Dist2 = sqrtf((Point2[0] - Point3[0]) * (Point2[0] - Point3[0]) +
			    (Point2[1] - Point3[1]) * (Point2[1] - Point3[1]) +
			    (Point2[2] - Point3[2]) * (Point2[2] - Point3[2]));
	float Dist3 = sqrtf((Point3[0] - Point1[0]) * (Point3[0] - Point1[0]) +
			    (Point3[1] - Point1[1]) * (Point3[1] - Point1[1]) +
			    (Point3[2] - Point1[2]) * (Point3[2] - Point1[2]));

	if ((Dist1 <= TriangleSizeLimit) && (Dist2 <= TriangleSizeLimit) && (Dist3 <= TriangleSizeLimit)) {
		// добавляем в новую последовательность треугольник
		if (VertexBuffer && CurrentPosition) {
			memcpy(VertexBuffer + (*CurrentPosition), Point1, sizeof(Point1[0]) * 8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer + (*CurrentPosition), Point2, sizeof(Point2[0]) * 8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer + (*CurrentPosition), Point3, sizeof(Point3[0]) * 8);
			*CurrentPosition += Stride;
		}

		return;
	}

	// одна из сторон больше, ищем наибольшую, делим ее пополам и идем дальше в рекурсию
	if ((Dist1 > Dist2) && (Dist1 > Dist3)) {
		float Point_A[8]= {(Point1[0] + Point2[0]) / 2.0f,
				   (Point1[1] + Point2[1]) / 2.0f,
				   (Point1[2] + Point2[2]) / 2.0f,
				   (Point1[3] + Point2[3]) / 2.0f,
				   (Point1[4] + Point2[4]) / 2.0f,
				   (Point1[5] + Point2[5]) / 2.0f,
				   (Point1[6] + Point2[6]) / 2.0f,
				   (Point1[7] + Point2[7]) / 2.0f};

		RecursiveBufferLimitedBySizeTrianglesGenerate(Point1, Point_A, Point3,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
		RecursiveBufferLimitedBySizeTrianglesGenerate(Point_A, Point2, Point3,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
		return;
	} else if ((Dist2 > Dist1) && (Dist2 > Dist3)) {
		float Point_A[8]= {(Point2[0] + Point3[0]) / 2.0f,
				   (Point2[1] + Point3[1]) / 2.0f,
				   (Point2[2] + Point3[2]) / 2.0f,
				   (Point2[3] + Point3[3]) / 2.0f,
				   (Point2[4] + Point3[4]) / 2.0f,
				   (Point2[5] + Point3[5]) / 2.0f,
				   (Point2[6] + Point3[6]) / 2.0f,
				   (Point2[7] + Point3[7]) / 2.0f};

		RecursiveBufferLimitedBySizeTrianglesGenerate(Point1, Point2, Point_A,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
		RecursiveBufferLimitedBySizeTrianglesGenerate(Point1, Point_A, Point3,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
		return;
	} else {
		float Point_A[8]= {(Point3[0] + Point1[0]) / 2.0f,
				   (Point3[1] + Point1[1]) / 2.0f,
				   (Point3[2] + Point1[2]) / 2.0f,
				   (Point3[3] + Point1[3]) / 2.0f,
				   (Point3[4] + Point1[4]) / 2.0f,
				   (Point3[5] + Point1[5]) / 2.0f,
				   (Point3[6] + Point1[6]) / 2.0f,
				   (Point3[7] + Point1[7]) / 2.0f};

		RecursiveBufferLimitedBySizeTrianglesGenerate(Point1, Point2, Point_A,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
		RecursiveBufferLimitedBySizeTrianglesGenerate(Point_A, Point2, Point3,
				Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
		return;
	}
}

void cModel3D::CreateVertexBufferLimitedBySizeTriangles(float TriangleSizeLimit)
{
	// если преобразования делать не требуется - устанавливаем ссылку на обуфер блока и выходим
	if (TriangleSizeLimit <= 0.0f) {
		for (int i = 0; i < DrawObjectCount; i++) {
			DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount = DrawObjectList[i].VertexCount;
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles = DrawObjectList[i].VertexArray;
		}
		return;
	}

	// первый вызов - считаем кол-во точек, для выделения памяти на новый буфер
	for (int i = 0; i < DrawObjectCount; i++) {
		DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount = 0;

		// перебираем по треугольникам (3 точки)
		for (int j = 0; j < DrawObjectList[i].VertexCount - 2; j += 3) {
			unsigned int tmpOffset0 = DrawObjectList[i].VertexStride * j;		// j
			unsigned int tmpOffset1 = tmpOffset0 + DrawObjectList[i].VertexStride;	// j + 1
			unsigned int tmpOffset2 = tmpOffset1 + DrawObjectList[i].VertexStride;	// j + 2

			float Point1[8] = {DrawObjectList[i].VertexArray[tmpOffset0],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 1],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 2],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 3],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 4],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 5],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 6],
					   DrawObjectList[i].VertexArray[tmpOffset0 + 7]};

			float Point2[8] = {DrawObjectList[i].VertexArray[tmpOffset1],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 1],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 2],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 3],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 4],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 5],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 6],
					   DrawObjectList[i].VertexArray[tmpOffset1 + 7]};

			float Point3[8] = {DrawObjectList[i].VertexArray[tmpOffset2],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 1],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 2],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 3],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 4],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 5],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 6],
					   DrawObjectList[i].VertexArray[tmpOffset2 + 7]};

			// идем на рекурсивную функцию считать кол-во треугольников
			DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount +=
				RecursiveBufferLimitedBySizeTrianglesCalculate(Point1, Point2, Point3, 0,
									       nullptr, nullptr, TriangleSizeLimit) * 3;
		}
	}

	// второй проход
	for (int i = 0; i < DrawObjectCount; i++) {
		// если кол-во получилось как в текущем вертекс буфере - корректировок не требуется, просто копируем данные
		if (DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount == DrawObjectList[i].VertexCount)
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles = DrawObjectList[i].VertexArray;
		else {
			// выделяем память
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles =
					new float[DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount *
						  DrawObjectList[i].VertexStride];
			int CurrentPosition = 0;

			// перебираем по треугольникам (3 точки)
			for (int j = 0; j < DrawObjectList[i].VertexCount - 2; j += 3) {
				unsigned int tmpOffset0 = DrawObjectList[i].VertexStride * j;		// j
				unsigned int tmpOffset1 = tmpOffset0 + DrawObjectList[i].VertexStride;	// j + 1
				unsigned int tmpOffset2 = tmpOffset1 + DrawObjectList[i].VertexStride;	// j + 2

				float Point1[8] = {DrawObjectList[i].VertexArray[tmpOffset0],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 1],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 2],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 3],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 4],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 5],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 6],
						   DrawObjectList[i].VertexArray[tmpOffset0 + 7]};

				float Point2[8] = {DrawObjectList[i].VertexArray[tmpOffset1],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 1],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 2],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 3],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 4],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 5],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 6],
						   DrawObjectList[i].VertexArray[tmpOffset1 + 7]};

				float Point3[8] = {DrawObjectList[i].VertexArray[tmpOffset2],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 1],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 2],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 3],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 4],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 5],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 6],
						   DrawObjectList[i].VertexArray[tmpOffset2 + 7]};

				// идем на рекурсивную функцию
				RecursiveBufferLimitedBySizeTrianglesGenerate(Point1, Point2, Point3,
					DrawObjectList[i].VertexStride, DrawObjectList[i].VertexBufferLimitedBySizeTriangles,
					&CurrentPosition, TriangleSizeLimit);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// пересоздаем буфер вертексов, для работы с нормал меппингом в шейдерах, добавляем тангент и бинормаль
//-----------------------------------------------------------------------------
void cModel3D::CreateTangentAndBinormal()
{
	// пересоздаем глобальный вертексный буфер, (!) работаем с фиксированным типом,
	// на входе у нас всегда RI_3f_XYZ | RI_3f_NORMAL | RI_2f_TEX
	int New_VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX | RI_SEPARATE_TEX_COORD;
	int New_VertexStride = 3 + 3 + 6;
	float *New_VertexBuffer = new float[New_VertexStride * GlobalIndexCount];

	for (unsigned int j = 0; j < GlobalIndexCount; j++) {
		memcpy(New_VertexBuffer + New_VertexStride * j,
		       GlobalVertexArray + GlobalIndexArray[j] * DrawObjectList[0].VertexStride,
		       DrawObjectList[0].VertexStride * sizeof(float));
	}

	// удаляем указатели на старый буфер
	delete [] GlobalVertexArray;
	// присваиваем новые значения
	GlobalVertexArray = New_VertexBuffer;
	GlobalVertexCount = GlobalIndexCount;

	// создаем тангенты и бинормали, сохраняем их в 2 и 3 текстурных координатах
	for (unsigned int j = 0; j < GlobalVertexCount - 2; j += 3) {
		unsigned int tmpOffset0 = New_VertexStride * j;			// j
		unsigned int tmpOffset1 = tmpOffset0 + New_VertexStride;	// j + 1
		unsigned int tmpOffset2 = tmpOffset1 + New_VertexStride;	// j + 2

		sVECTOR3D Point1(GlobalVertexArray[tmpOffset0],
				 GlobalVertexArray[tmpOffset0 + 1],
				 GlobalVertexArray[tmpOffset0 + 2]);

		sVECTOR3D Point2(GlobalVertexArray[tmpOffset1],
				 GlobalVertexArray[tmpOffset1 + 1],
				 GlobalVertexArray[tmpOffset1 + 2]);

		sVECTOR3D Point3(GlobalVertexArray[tmpOffset2],
				 GlobalVertexArray[tmpOffset2 + 1],
				 GlobalVertexArray[tmpOffset2 + 2]);

		// находим 2 вектора, образующих плоскость
		sVECTOR3D PlaneVector1 = Point2 - Point1;
		sVECTOR3D PlaneVector2 = Point3 - Point1;
		// находим нормаль плоскости
		PlaneVector1.Multiply(PlaneVector2);
		PlaneVector1.NormalizeHi();
		sVECTOR3D PlaneNormal = PlaneVector1;

		// нормаль получили (нужна будет для проверки зеркалирования), можем идти дальше
		// делаем просчет тангента и бинормали
		PlaneVector1 = Point1 - Point2;
		PlaneVector2 = Point3 - Point2;

		float delta_U_0 = GlobalVertexArray[tmpOffset0 + 6] -
				  GlobalVertexArray[tmpOffset1 + 6];
		float delta_U_1 = GlobalVertexArray[tmpOffset2 + 6] -
				  GlobalVertexArray[tmpOffset1 + 6];

		float delta_V_0 = GlobalVertexArray[tmpOffset0 + 7] -
				  GlobalVertexArray[tmpOffset1 + 7];
		float delta_V_1 = GlobalVertexArray[tmpOffset2 + 7] -
				  GlobalVertexArray[tmpOffset1 + 7];

		sVECTOR3D Tangent = ((PlaneVector1 ^ delta_V_1) - (PlaneVector2 ^ delta_V_0));
		Tangent.NormalizeHi();
		float TangentW = 1.0f;
		sVECTOR3D BiNormal = ((PlaneVector1 ^ delta_U_1) - (PlaneVector2 ^ delta_U_0));
		BiNormal.NormalizeHi();

		// проверка на зеркалирование нормал мепа
		sVECTOR3D TBCross = Tangent;
		TBCross.Multiply(BiNormal);
		if( (TBCross * PlaneNormal) < 0 ) {
			// вот тут, нормал меп "перевернут"
			// надо это учесть
			Tangent = ((PlaneVector1 ^ (-delta_V_1)) - (PlaneVector2 ^ (-delta_V_0)));
			Tangent.NormalizeHi();
			TangentW = -1.0f;
		}

		// тангент
		GlobalVertexArray[tmpOffset0 + 8] =
				GlobalVertexArray[tmpOffset1 + 8] = GlobalVertexArray[tmpOffset2 + 8] = Tangent.x;
		GlobalVertexArray[tmpOffset0 + 9] =
				GlobalVertexArray[tmpOffset1 + 9] = GlobalVertexArray[tmpOffset2 + 9] = Tangent.y;
		GlobalVertexArray[tmpOffset0 + 10] =
				GlobalVertexArray[tmpOffset1 + 10] = GlobalVertexArray[tmpOffset2 + 10] = Tangent.z;
		GlobalVertexArray[tmpOffset0 + 11] =
				GlobalVertexArray[tmpOffset1 + 11] = GlobalVertexArray[tmpOffset2 + 11] = TangentW;
	}

	// меняем данные в глобальном индекс буфере, они теперь идут по порядку
	for (unsigned int i = 0; i < GlobalIndexCount; i++) {
		GlobalIndexArray[i] = i;
	}

	// меняем указатели на вертексный буфер, шаг и формат
	for (int i = 0; i < DrawObjectCount; i++) {
		DrawObjectList[i].VertexArray = GlobalVertexArray;
		DrawObjectList[i].VertexFormat = New_VertexFormat;
		DrawObjectList[i].VertexStride = New_VertexStride;
	}
}

//-----------------------------------------------------------------------------
// загрузка "родного" формата
//-----------------------------------------------------------------------------
bool cModel3D::ReadVW3D(const std::string &FileName)
{
	if (FileName.empty())
		return false;

	std::unique_ptr<sFILE> File = vw_fopen(FileName);
	if (!File)
		return false;

	Name = FileName;

	// check "VW3D" sign
	char Sign[4];
	File->fread(&Sign, 4, 1);
	// Sign don't contain null-terminated string, strncmp() should be used
	if (strncmp(Sign, "VW3D", 4) != 0)
		return false;

	// читаем, сколько объектов
	File->fread(&DrawObjectCount, sizeof(DrawObjectCount), 1);

	DrawObjectList = new sObjectBlock[DrawObjectCount];

	GlobalIndexCount = 0;

	// для каждого объекта
	for (int i = 0; i < DrawObjectCount; i++) {
		DrawObjectList[i].RangeStart = GlobalIndexCount;

		// VertexFormat
		File->fread(&(DrawObjectList[i].VertexFormat), sizeof(DrawObjectList[0].VertexFormat), 1);
		// VertexStride
		File->fread(&(DrawObjectList[i].VertexStride), sizeof(DrawObjectList[0].VertexStride), 1);
		// VertexCount на самом деле, это кол-во индексов на объект
		File->fread(&(DrawObjectList[i].VertexCount), sizeof(DrawObjectList[0].VertexCount), 1);
		GlobalIndexCount += DrawObjectList[i].VertexCount;

		// Location
		File->fread(&(DrawObjectList[i].Location), sizeof(DrawObjectList[0].Location.x) * 3, 1);
		// Rotation
		File->fread(&(DrawObjectList[i].Rotation), sizeof(DrawObjectList[0].Rotation.x) * 3, 1);

		// рисуем нормально, не прозрачным
		DrawObjectList[i].DrawType = 0;

		// вертексный буфер
		DrawObjectList[i].NeedDestroyDataInObjectBlock = false;
		DrawObjectList[i].VertexArray = nullptr;
		DrawObjectList[i].VBO = 0;
		// индексный буфер
		DrawObjectList[i].IndexArray = nullptr;
		DrawObjectList[i].IBO = 0;
		// vao
		DrawObjectList[i].VAO = 0;
	}

	// получаем сколько всего вертексов
	File->fread(&GlobalVertexCount, sizeof(GlobalVertexCount), 1);

	// собственно данные (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	GlobalVertexArray = new float[GlobalVertexCount * DrawObjectList[0].VertexStride];
	File->fread(GlobalVertexArray,	GlobalVertexCount * DrawObjectList[0].VertexStride * sizeof(GlobalVertexArray[0]), 1);

	// индекс буфер
	GlobalIndexArray = new unsigned int[GlobalIndexCount];
	File->fread(GlobalIndexArray, GlobalIndexCount * sizeof(GlobalIndexArray[0]), 1);

	// т.к. наши объекты используют глобальные буферы, надо поставить указатели
	for (int i = 0; i < DrawObjectCount; i++) {
		DrawObjectList[i].VertexArray = GlobalVertexArray;
		DrawObjectList[i].IndexArray = GlobalIndexArray;
	}

	vw_fclose(File);

	return true;
}

//-----------------------------------------------------------------------------
// запись "родного" формата на диск
//-----------------------------------------------------------------------------
bool cModel3D::WriteVW3D(const std::string &FileName)
{
	// небольшие проверки
	if (!GlobalVertexArray || !GlobalIndexArray || !DrawObjectList) {
		std::cerr << __func__ << "(): " << "Can't create " << FileName << " file for empty Model3D.\n";
		return false;
	}

	SDL_RWops *FileVW3D;
	FileVW3D = SDL_RWFromFile(FileName.c_str(), "wb");
	// если не можем создать файл на запись - уходим
	if (!FileVW3D) {
		std::cerr << __func__ << "(): " << "Can't create " << FileName << " file on disk.\n";
		return false;
	}

	// маркер файла 4 байта
	constexpr char Sign[4]{'V','W','3','D'};
	SDL_RWwrite(FileVW3D, Sign, 4, 1);

	// общее кол-во объектов в моделе - 4 байта (int)
	SDL_RWwrite(FileVW3D, &DrawObjectCount, sizeof(DrawObjectCount), 1);

	// для каждого объекта в моделе
	for (int i = 0; i < DrawObjectCount; i++) {
		// VertexFormat
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexFormat, sizeof(DrawObjectList[0].VertexFormat), 1);
		// VertexStride
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexStride, sizeof(DrawObjectList[0].VertexStride), 1);
		// VertexCount
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexCount, sizeof(DrawObjectList[0].VertexCount), 1);

		// Location
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Location, sizeof(DrawObjectList[0].Location.x) * 3, 1);
		// Rotation
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Rotation, sizeof(DrawObjectList[0].Rotation.x) * 3, 1);
	}

	// записываем реальное кол-во вертексов в общем вертекс буфере, мы их посчитали заранее
	SDL_RWwrite(FileVW3D, &GlobalVertexCount, sizeof(GlobalVertexCount), 1);

	// данные, вертексы (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	SDL_RWwrite(FileVW3D, GlobalVertexArray,
		    DrawObjectList[0].VertexStride * GlobalVertexCount * sizeof(GlobalVertexArray[0]), 1);

	// данные, индексный буфер
	SDL_RWwrite(FileVW3D, GlobalIndexArray, GlobalIndexCount * sizeof(GlobalIndexArray[0]), 1);

	// закрываем файл
	SDL_RWclose(FileVW3D);

	std::cout << "VW3D Write: " << FileName << "\n";
	return true;
}
