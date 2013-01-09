/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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


#include "Model3D.h"
#include "../RendererInterface/RendererInterface.h"



//-----------------------------------------------------------------------------
// Конструктор
//-----------------------------------------------------------------------------
eObjectBlock::eObjectBlock(void)
{
	VertexFormat = 0;
	VertexStride = 0;
	VertexCount = 0;
	DrawType = 0;
	RangeStart = 0;
	Location = Rotation = VECTOR3D(0.0f,0.0f,0.0f);

	NeedGeometryAnimation = NeedTextureAnimation = false;
	GeometryAnimation = TextureAnimation = VECTOR3D(0.0f,0.0f,0.0f);

	NeedDestroyDataInObjectBlock = false;
	VertexBuffer = 0;
	VBO = 0;
	IndexBuffer = 0;
	IBO = 0;
	VAO = 0;

	VertexBufferLimitedBySizeTriangles = 0;
	VertexBufferLimitedBySizeTrianglesCount = 0;

	ShaderType = 1;
	for (int i=0; i<16; i++) ShaderData[i] = 0;

}
//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
eObjectBlock::~eObjectBlock(void)
{
	// если нужно, удаляем с блоком
	if (NeedDestroyDataInObjectBlock)
	{
		if (VertexBuffer != 0)
		{
			delete [] VertexBuffer; VertexBuffer = 0;
		}
		if (VBO != 0)
		{
			vw_DeleteVBO(*VBO); delete VBO; VBO = 0;
		}
		if (IndexBuffer != 0)
		{
			delete [] IndexBuffer; IndexBuffer = 0;
		}
		if (IBO != 0)
		{
			vw_DeleteVBO(*IBO); delete IBO; IBO = 0;
		}
		if (VAO != 0)
		{
			vw_DeleteVAO(*VAO); delete VAO; VAO = 0;
		}

		// память для VertexBufferLimitedBySizeTriangles никогда не должена быть выделена в конкретном объекте,
		// только в моделе его удаляем только в деструкторе eModel3D, т.к. он там создается
	}
}







//-----------------------------------------------------------------------------
// Конструктор
//-----------------------------------------------------------------------------
eModel3D::eModel3D(void)
{
	Name = 0;
	DrawObjectList = 0;
	DrawObjectCount = 0;
	GlobalVertexBuffer = 0;
	GlobalVBO = 0;
	GlobalIndexBuffer = 0;
	GlobalIBO = 0;
	GlobalVAO = 0;
	Next = 0;
	Prev = 0;
	vw_AttachModel3D(this);
}


//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
eModel3D::~eModel3D(void)
{
	if (Name != 0) {delete [] Name; Name = 0;}

	if (DrawObjectList != 0)
	{
		for (int i=0; i<DrawObjectCount; i++)
		{
			// удаляем только если это не ссылка на вертекс буфер блока
			if ((DrawObjectList[i].VertexBufferLimitedBySizeTriangles != 0) && (DrawObjectList[i].VertexBufferLimitedBySizeTriangles != DrawObjectList[i].VertexBuffer))
			{
				delete [] DrawObjectList[i].VertexBufferLimitedBySizeTriangles;
				DrawObjectList[i].VertexBufferLimitedBySizeTriangles = 0;
			}
			if ((DrawObjectList[i].VertexBuffer != 0) && (DrawObjectList[i].VertexBuffer != GlobalVertexBuffer))
			{
				delete [] DrawObjectList[i].VertexBuffer;
				DrawObjectList[i].VertexBuffer = 0;
			}
			if ((DrawObjectList[i].IndexBuffer != 0) && (DrawObjectList[i].IndexBuffer != GlobalIndexBuffer))
			{
				delete [] DrawObjectList[i].IndexBuffer;
				DrawObjectList[i].IndexBuffer = 0;
			}
			if ((DrawObjectList[i].VBO != 0) && (DrawObjectList[i].VBO != GlobalVBO))
			{
				vw_DeleteVBO(*DrawObjectList[i].VBO);
				delete DrawObjectList[i].VBO; DrawObjectList[i].VBO = 0;
			}
			if ((DrawObjectList[i].IBO != 0) && (DrawObjectList[i].IBO != GlobalIBO))
			{
				vw_DeleteVBO(*DrawObjectList[i].IBO);
				delete DrawObjectList[i].IBO; DrawObjectList[i].IBO = 0;
			}
			if ((DrawObjectList[i].VAO != 0) && (DrawObjectList[i].VAO != GlobalVAO))
			{
				vw_DeleteVAO(*DrawObjectList[i].VAO);
				delete DrawObjectList[i].VAO; DrawObjectList[i].VAO = 0;
			}
		}
		delete [] DrawObjectList; DrawObjectList = 0;
	}
	if (GlobalVertexBuffer != 0)
	{
		delete [] GlobalVertexBuffer; GlobalVertexBuffer = 0;
	}
	if (GlobalVBO != 0)
	{
		vw_DeleteVBO(*GlobalVBO); delete GlobalVBO; GlobalVBO=0;
	}
	if (GlobalIndexBuffer != 0)
	{
		delete [] GlobalIndexBuffer; GlobalIndexBuffer = 0;
	}
	if (GlobalIBO != 0)
	{
		vw_DeleteVBO(*GlobalIBO); delete GlobalIBO; GlobalIBO=0;
	}
	if (GlobalVAO != 0)
	{
		vw_DeleteVAO(*GlobalVAO); delete GlobalVAO; GlobalVAO=0;
	}

	DrawObjectCount = 0;
	vw_DetachModel3D(this);
}





//-----------------------------------------------------------------------------
// создание вертекс и индекс буферов для каждого блока модели
//-----------------------------------------------------------------------------
void eModel3D::CreateObjectsBuffers()
{
	for (int i=0; i<DrawObjectCount; i++)
	{
		// создаем вертексный буфер блока
		DrawObjectList[i].VertexBuffer = new float[DrawObjectList[i].VertexStride*DrawObjectList[i].VertexCount];
		for (int j=0; j<DrawObjectList[i].VertexCount; j++)
		{
			memcpy(DrawObjectList[i].VertexBuffer+DrawObjectList[i].VertexStride*j,
					GlobalVertexBuffer+GlobalIndexBuffer[DrawObjectList[i].RangeStart+j]*DrawObjectList[i].VertexStride,
					DrawObjectList[i].VertexStride*sizeof(float));
		}

		// создаем индексный буфер блока
		DrawObjectList[i].IndexBuffer = new unsigned int[DrawObjectList[i].VertexCount];
		for (int j=0; j<DrawObjectList[i].VertexCount; j++) DrawObjectList[i].IndexBuffer[j] = j;

		// т.к. у нас отдельные буферы, то начало идет с нуля теперь
		DrawObjectList[i].RangeStart = 0;
	}
}




//-----------------------------------------------------------------------------
// создаем все поддерживаемые буферы (VAO, VBO, IBO)
//-----------------------------------------------------------------------------
void eModel3D::CreateHardwareBuffers()
{
	// делаем общее VBO
	GlobalVBO = new unsigned int;
	if (!vw_BuildVBO(GlobalVertexCount, GlobalVertexBuffer, DrawObjectList[0].VertexStride, GlobalVBO))
	{
		delete GlobalVBO; GlobalVBO=0;
	}

	// делаем общий IBO
	GlobalIBO = new unsigned int;
	if (!vw_BuildIBO(GlobalIndexCount, GlobalIndexBuffer, GlobalIBO))
	{
		delete GlobalIBO; GlobalIBO=0;
	}

	GlobalVAO = new unsigned int;
	if (!vw_BuildVAO(GlobalVAO, GlobalIndexCount, DrawObjectList[0].VertexFormat, GlobalVertexBuffer,
						DrawObjectList[0].VertexStride*sizeof(float), GlobalVBO, 0,
						GlobalIndexBuffer, GlobalIBO))
	{
		delete GlobalVAO; GlobalVAO=0;
	}


	// создаем буферы для каждого объекта
	for (int i=0; i<DrawObjectCount; i++)
	{
		// делаем VBO
		DrawObjectList[i].VBO = new unsigned int;
		if (!vw_BuildVBO(DrawObjectList[i].VertexCount, DrawObjectList[i].VertexBuffer, DrawObjectList[i].VertexStride, DrawObjectList[i].VBO))
		{
			delete DrawObjectList[i].VBO; DrawObjectList[i].VBO=0;
		}

		// делаем IBO
		DrawObjectList[i].IBO = new unsigned int;
		if (!vw_BuildIBO(DrawObjectList[i].VertexCount, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IBO))
		{
			delete DrawObjectList[i].IBO; DrawObjectList[i].IBO=0;
		}

		// делаем VAO
		DrawObjectList[i].VAO = new unsigned int;
		if (!vw_BuildVAO(DrawObjectList[i].VAO, DrawObjectList[i].VertexCount, DrawObjectList[i].VertexFormat, DrawObjectList[i].VertexBuffer,
							DrawObjectList[i].VertexStride*sizeof(float), DrawObjectList[i].VBO,
							DrawObjectList[i].RangeStart, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IBO))
		{
			delete DrawObjectList[i].VAO; DrawObjectList[i].VAO=0;
		}
	}

}





//-----------------------------------------------------------------------------
// рекурсивно создаем VertexBufferLimitedBySizeTriangles
//-----------------------------------------------------------------------------
int RecursiveBufferLimitedBySizeTriangles(float Point1[8], float Point2[8], float Point3[8], int Stride, float *VertexBuffer, int *CurrentPosition, float TriangleSizeLimit)
{
	// подсчитываем длину сторон, если они меньше чем необходимый минимум - выходим с 1
	float Dist1 = sqrtf((Point1[0]-Point2[0])*(Point1[0]-Point2[0]) + (Point1[1]-Point2[1])*(Point1[1]-Point2[1]) + (Point1[2]-Point2[2])*(Point1[2]-Point2[2]));
	float Dist2 = sqrtf((Point2[0]-Point3[0])*(Point2[0]-Point3[0]) + (Point2[1]-Point3[1])*(Point2[1]-Point3[1]) + (Point2[2]-Point3[2])*(Point2[2]-Point3[2]));
	float Dist3 = sqrtf((Point3[0]-Point1[0])*(Point3[0]-Point1[0]) + (Point3[1]-Point1[1])*(Point3[1]-Point1[1]) + (Point3[2]-Point1[2])*(Point3[2]-Point1[2]));

	if ((Dist1<=TriangleSizeLimit) && (Dist2<=TriangleSizeLimit) && (Dist3<=TriangleSizeLimit))
	{
		// добавляем в новую последовательность треугольник
		if ((VertexBuffer != 0) && (CurrentPosition != 0))
		{
			memcpy(VertexBuffer+(*CurrentPosition), Point1, sizeof(float)*8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer+(*CurrentPosition), Point2, sizeof(float)*8);
			*CurrentPosition += Stride;
			memcpy(VertexBuffer+(*CurrentPosition), Point3, sizeof(float)*8);
			*CurrentPosition += Stride;
		}

		return 1;
	}

	// одна из сторон больше, ищем наибольшую, делим ее пополам и идем дальше в рекурсию

	if ((Dist1 > Dist2) && (Dist1 > Dist3))
	{
		float Point_A[8]={(Point1[0]+Point2[0])/2.0f,
							(Point1[1]+Point2[1])/2.0f,
							(Point1[2]+Point2[2])/2.0f,
							(Point1[3]+Point2[3])/2.0f,
							(Point1[4]+Point2[4])/2.0f,
							(Point1[5]+Point2[5])/2.0f,
							(Point1[6]+Point2[6])/2.0f,
							(Point1[7]+Point2[7])/2.0f};

		return RecursiveBufferLimitedBySizeTriangles(Point1, Point_A, Point3, Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit)
				+ RecursiveBufferLimitedBySizeTriangles(Point_A, Point2, Point3, Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
	}
	else
	if ((Dist2 > Dist1) && (Dist2 > Dist3))
	{
		float Point_A[8]={(Point2[0]+Point3[0])/2.0f,
							(Point2[1]+Point3[1])/2.0f,
							(Point2[2]+Point3[2])/2.0f,
							(Point2[3]+Point3[3])/2.0f,
							(Point2[4]+Point3[4])/2.0f,
							(Point2[5]+Point3[5])/2.0f,
							(Point2[6]+Point3[6])/2.0f,
							(Point2[7]+Point3[7])/2.0f};

		return RecursiveBufferLimitedBySizeTriangles(Point1, Point2, Point_A, Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit)
				+ RecursiveBufferLimitedBySizeTriangles(Point1, Point_A, Point3, Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
	}
	else
	{
		float Point_A[8]={(Point3[0]+Point1[0])/2.0f,
							(Point3[1]+Point1[1])/2.0f,
							(Point3[2]+Point1[2])/2.0f,
							(Point3[3]+Point1[3])/2.0f,
							(Point3[4]+Point1[4])/2.0f,
							(Point3[5]+Point1[5])/2.0f,
							(Point3[6]+Point1[6])/2.0f,
							(Point3[7]+Point1[7])/2.0f};

		return RecursiveBufferLimitedBySizeTriangles(Point1, Point2, Point_A, Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit)
				+ RecursiveBufferLimitedBySizeTriangles(Point_A, Point2, Point3, Stride, VertexBuffer, CurrentPosition, TriangleSizeLimit);
	}
}
void eModel3D::CreateVertexBufferLimitedBySizeTriangles(float TriangleSizeLimit)
{
	// если преобразования делать не требуется - устанавливаем ссылку на обуфер блока и выходим
	if (TriangleSizeLimit <= 0.0f)
	{
		for (int i=0; i<DrawObjectCount; i++)
		{
			DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount = DrawObjectList[i].VertexCount;
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles = DrawObjectList[i].VertexBuffer;
		}

		return;
	}

	// первый вызов - считаем кол-во точек, для выделения памяти на новый буфер
	for (int i=0; i<DrawObjectCount; i++)
	{
		DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount = 0;

		// перебираем по треугольникам (3 точки)
		for (int j=0; j<DrawObjectList[i].VertexCount-2; j+=3)
		{
			float Point1[8] ={DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+1],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+2],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+3],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+4],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+5],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+6],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+7]};

			float Point2[8] ={DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+1],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+2],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+3],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+4],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+5],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+6],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+7]};

			float Point3[8] ={DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+1],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+2],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+3],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+4],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+5],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+6],
							DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+7]};

			// идем на рекурсивную функцию считать кол-во треугольников
			DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount += RecursiveBufferLimitedBySizeTriangles(Point1, Point2, Point3, 0, 0, 0, TriangleSizeLimit)*3;
		}
	}


	// второй проход
	for (int i=0; i<DrawObjectCount; i++)
	{
		// если кол-во получилось как в текущем вертекс буфере - корректировок не требуется, просто копируем данные
		if (DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount == DrawObjectList[i].VertexCount)
		{
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles = DrawObjectList[i].VertexBuffer;
		}
		else
		{
			// выделяем память
			DrawObjectList[i].VertexBufferLimitedBySizeTriangles = new float[DrawObjectList[i].VertexBufferLimitedBySizeTrianglesCount*DrawObjectList[i].VertexStride];
			int CurrentPosition = 0;
			int FakeCalculation = 0;

			// перебираем по треугольникам (3 точки)
			for (int j=0; j<DrawObjectList[i].VertexCount-2; j+=3)
			{
				float Point1[8] ={DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+1],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+2],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+3],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+4],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+5],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+6],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*j+7]};

				float Point2[8] ={DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+1],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+2],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+3],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+4],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+5],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+6],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+1)+7]};

				float Point3[8] ={DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+1],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+2],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+3],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+4],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+5],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+6],
								DrawObjectList[i].VertexBuffer[DrawObjectList[i].VertexStride*(j+2)+7]};

				// идем на рекурсивную функцию
				FakeCalculation += RecursiveBufferLimitedBySizeTriangles(Point1, Point2, Point3, DrawObjectList[i].VertexStride, DrawObjectList[i].VertexBufferLimitedBySizeTriangles, &CurrentPosition, TriangleSizeLimit)*3;
			}
		}
	}
}






//-----------------------------------------------------------------------------
// пересоздаем буфер вертексов, для работы с нормал меппингом в шейдерах, добавляем тангент и бинормаль
//-----------------------------------------------------------------------------
void eModel3D::CreateTangentAndBinormal()
{
	// пересоздаем глобальный вертексный буфер, (!) работаем с фиксированным типом,
	// на входе у нас всегда RI_3f_XYZ | RI_3f_NORMAL | RI_2f_TEX
	int	New_VertexFormat = RI_3f_XYZ | RI_3f_NORMAL | RI_3_TEX | RI_2f_TEX | RI_SEPARATE_TEX_COORD;
	int	New_VertexStride = 3+3+6;
	float *New_VertexBuffer = new float[New_VertexStride*GlobalIndexCount];


	for (unsigned int j=0; j<GlobalIndexCount; j++)
	{
		memcpy(New_VertexBuffer+New_VertexStride*j,
				GlobalVertexBuffer+GlobalIndexBuffer[j]*DrawObjectList[0].VertexStride,
				DrawObjectList[0].VertexStride*sizeof(float));
	}

	// удаляем указатели на старый буфер
	delete [] GlobalVertexBuffer;
	// присваиваем новые значения
	GlobalVertexBuffer = New_VertexBuffer;
	GlobalVertexCount = GlobalIndexCount;





	// создаем тангенты и бинормали, сохраняем их в 2 и 3 текстурных координатах
	for (unsigned int j=0; j<GlobalVertexCount-2; j+=3)
	{

		VECTOR3D Point1(GlobalVertexBuffer[New_VertexStride*j],
						GlobalVertexBuffer[New_VertexStride*j+1],
						GlobalVertexBuffer[New_VertexStride*j+2]);

		VECTOR3D Point2(GlobalVertexBuffer[New_VertexStride*(j+1)],
						GlobalVertexBuffer[New_VertexStride*(j+1)+1],
						GlobalVertexBuffer[New_VertexStride*(j+1)+2]);

		VECTOR3D Point3(GlobalVertexBuffer[New_VertexStride*(j+2)],
						GlobalVertexBuffer[New_VertexStride*(j+2)+1],
						GlobalVertexBuffer[New_VertexStride*(j+2)+2]);

		// находим 2 вектора, образующих плоскость
		VECTOR3D PlaneVector1 = Point2 - Point1;
		VECTOR3D PlaneVector2 = Point3 - Point1;
		// находим нормаль плоскости
		PlaneVector1.Multiply(PlaneVector2);
		PlaneVector1.NormalizeHi();
		VECTOR3D PlaneNormal = PlaneVector1;


		// нормаль получили (нужна будет для проверки зеркалирования), можем идти дальше
		// делаем просчет тангента и бинормали

		PlaneVector1 = Point1 - Point2;
		PlaneVector2 = Point3 - Point2;

		float delta_U_0 = GlobalVertexBuffer[New_VertexStride*j+6] - GlobalVertexBuffer[New_VertexStride*(j+1)+6];
		float delta_U_1 = GlobalVertexBuffer[New_VertexStride*(j+2)+6] - GlobalVertexBuffer[New_VertexStride*(j+1)+6];

		float delta_V_0 = GlobalVertexBuffer[New_VertexStride*j+7] - GlobalVertexBuffer[New_VertexStride*(j+1)+7];
		float delta_V_1 = GlobalVertexBuffer[New_VertexStride*(j+2)+7] - GlobalVertexBuffer[New_VertexStride*(j+1)+7];

		VECTOR3D Tangent = ((PlaneVector1 ^ delta_V_1) - (PlaneVector2 ^ delta_V_0));
		Tangent.NormalizeHi();
		float TangentW = 1.0f;
		VECTOR3D BiNormal = ((PlaneVector1 ^ delta_U_1) - (PlaneVector2 ^ delta_U_0));
		BiNormal.NormalizeHi();


		// проверка на зеркалирование нормал мепа
		VECTOR3D TBCross = Tangent;
		TBCross.Multiply(BiNormal);
		if( (TBCross * PlaneNormal) < 0 )
		{
			// вот тут, нормал меп "перевернут"
			// надо это учесть
			Tangent = ((PlaneVector1 ^ (-delta_V_1)) - (PlaneVector2 ^ (-delta_V_0)));
			Tangent.NormalizeHi();
			TangentW = -1.0f;
		}

		// тангент
		GlobalVertexBuffer[New_VertexStride*j+8] = GlobalVertexBuffer[New_VertexStride*(j+1)+8] = GlobalVertexBuffer[New_VertexStride*(j+2)+8] = Tangent.x;
		GlobalVertexBuffer[New_VertexStride*j+9] = GlobalVertexBuffer[New_VertexStride*(j+1)+9] = GlobalVertexBuffer[New_VertexStride*(j+2)+9] = Tangent.y;
		GlobalVertexBuffer[New_VertexStride*j+10] = GlobalVertexBuffer[New_VertexStride*(j+1)+10] = GlobalVertexBuffer[New_VertexStride*(j+2)+10] = Tangent.z;
		GlobalVertexBuffer[New_VertexStride*j+11] = GlobalVertexBuffer[New_VertexStride*(j+1)+11] = GlobalVertexBuffer[New_VertexStride*(j+2)+11] = TangentW;
	}





	// меняем данные в глобальном индекс буфере, они теперь идут по порядку
	for (unsigned int i=0; i<GlobalIndexCount; i++) GlobalIndexBuffer[i] = i;


	// меняем указатели на вертексный буфер, шаг и формат
	for (int i=0; i<DrawObjectCount; i++)
	{
		DrawObjectList[i].VertexBuffer = GlobalVertexBuffer;
		DrawObjectList[i].VertexFormat = New_VertexFormat;
		DrawObjectList[i].VertexStride = New_VertexStride;
	}

}
