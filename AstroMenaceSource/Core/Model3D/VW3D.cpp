/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: VW3D.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
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


#include "Model3D.h"
#include "../RendererInterface/RendererInterface.h"






//-----------------------------------------------------------------------------
// загрузка "родного" формата
//-----------------------------------------------------------------------------
void eModel3D::ReadVW3D(const char *nName)
{
	eFILE *file = 0;
	file = vw_fopen(nName);
	if (file == 0) return;

	size_t SizeB = strlen(nName)+1;
	Name = new char[SizeB];
	strcpy(Name, nName);

	// "пропускаем" заголовок "VW3D"
	file->fread(&DrawObjectCount, 4, 1);

	// читаем, сколько объектов
	file->fread(&DrawObjectCount, sizeof(int), 1);

	DrawObjectList = new eObjectBlock[DrawObjectCount];

	unsigned int GlobalRangeStart = 0;


	// для каждого объекта
	for (int i=0; i<DrawObjectCount; i++)
	{
		DrawObjectList[i].RangeStart = GlobalRangeStart;

		// FVF_Format
		file->fread(&(DrawObjectList[i].FVF_Format),sizeof(int),1);
		// Stride
		file->fread(&(DrawObjectList[i].Stride),sizeof(int),1);
		// VertexCount на самом деле, это кол-во индексов на объект
		file->fread(&(DrawObjectList[i].VertexCount),sizeof(int),1);
		GlobalRangeStart += DrawObjectList[i].VertexCount;

		// Location
		file->fread(&(DrawObjectList[i].Location),sizeof(float)*3,1);
		// Rotation
		file->fread(&(DrawObjectList[i].Rotation),sizeof(float)*3,1);

		// рисуем нормально, не прозрачным
		DrawObjectList[i].DrawType = 0;

		// вертексный буфер
		DrawObjectList[i].NeedDestroyDataInObjectBlock = false;
		DrawObjectList[i].VertexBuffer = 0;
		DrawObjectList[i].VertexBufferVBO = 0;
		// индексный буфер
		DrawObjectList[i].IndexBuffer = 0;
		DrawObjectList[i].IndexBufferVBO = 0;
		// vao
		DrawObjectList[i].VAO = 0;
	}

	// получаем сколько всего вертексов
	unsigned int VCount = 0;
	file->fread(&VCount,sizeof(unsigned int),1);

	// собственно данные (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	GlobalVertexBuffer = new float[VCount*DrawObjectList[0].Stride];
	file->fread(GlobalVertexBuffer,	VCount*DrawObjectList[0].Stride*sizeof(float),1);

	// индекс буфер
	GlobalIndexBuffer = new unsigned int[GlobalRangeStart];
	file->fread(GlobalIndexBuffer, GlobalRangeStart*sizeof(unsigned int),1);


	// делаем общее VBO
	GlobalVertexBufferVBO = new unsigned int;
	if (!vw_BuildVBO(VCount, GlobalVertexBuffer, DrawObjectList[0].Stride, GlobalVertexBufferVBO))
	{
		delete GlobalVertexBufferVBO; GlobalVertexBufferVBO=0;
	}

	// делаем общий индекс VBO
	GlobalIndexBufferVBO = new unsigned int;
	if (!vw_BuildIndexVBO(GlobalRangeStart, GlobalIndexBuffer, GlobalIndexBufferVBO))
	{
		delete GlobalIndexBufferVBO; GlobalIndexBufferVBO=0;
	}

	GlobalVAO = new unsigned int;
	if (!vw_BuildVAO(GlobalVAO, GlobalRangeStart, DrawObjectList[0].FVF_Format, GlobalVertexBuffer,
						DrawObjectList[0].Stride*sizeof(float), GlobalVertexBufferVBO, 0,
						GlobalIndexBuffer, GlobalIndexBufferVBO))
	{
		delete GlobalVAO; GlobalVAO=0;
	}


	// устанавливаем правильные указатели на массивы
	for (int i=0; i<DrawObjectCount; i++)
	{
		// создаем вертексный буфер блока
		DrawObjectList[i].VertexBuffer = new float[DrawObjectList[i].Stride*DrawObjectList[i].VertexCount];

		for (int j=0; j<DrawObjectList[i].VertexCount; j++)
		{
			memcpy(DrawObjectList[i].VertexBuffer+DrawObjectList[i].Stride*j,
					GlobalVertexBuffer+GlobalIndexBuffer[DrawObjectList[i].RangeStart+j]*DrawObjectList[i].Stride,
					DrawObjectList[i].Stride*sizeof(float));
		}

		// создаем индексный буфер блока
		DrawObjectList[i].IndexBuffer = new unsigned int[DrawObjectList[i].VertexCount];
		for (int j=0; j<DrawObjectList[i].VertexCount; j++) DrawObjectList[i].IndexBuffer[j] = j;

		// т.к. у нас отдельные буферы, то начало идет с нуля теперь
		DrawObjectList[i].RangeStart = 0;


		// делаем VBO
		DrawObjectList[i].VertexBufferVBO = new unsigned int;
		if (!vw_BuildVBO(DrawObjectList[i].VertexCount, DrawObjectList[i].VertexBuffer, DrawObjectList[i].Stride, DrawObjectList[i].VertexBufferVBO))
		{
			delete DrawObjectList[i].VertexBufferVBO; DrawObjectList[i].VertexBufferVBO=0;
		}

		// делаем индекс VBO
		DrawObjectList[i].IndexBufferVBO = new unsigned int;
		if (!vw_BuildIndexVBO(DrawObjectList[i].VertexCount, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IndexBufferVBO))
		{
			delete DrawObjectList[i].IndexBufferVBO; DrawObjectList[i].IndexBufferVBO=0;
		}

		// делаем VAO
		DrawObjectList[i].VAO = new unsigned int;
		if (!vw_BuildVAO(DrawObjectList[i].VAO, DrawObjectList[i].VertexCount, DrawObjectList[i].FVF_Format, DrawObjectList[i].VertexBuffer,
							DrawObjectList[i].Stride*sizeof(float), DrawObjectList[i].VertexBufferVBO,
							DrawObjectList[i].RangeStart, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IndexBufferVBO))
		{
			delete DrawObjectList[i].VAO; DrawObjectList[i].VAO=0;
		}

	}


	vw_fclose(file);
}






//-----------------------------------------------------------------------------
// запись "родного" формата на диск
//-----------------------------------------------------------------------------
void eModel3D::WriteVW3D(const char *FileName)
{
	// небольшие проверки
	if ((GlobalVertexBuffer == 0) | (GlobalIndexBuffer == 0) | (DrawObjectList == 0))
	{
        fprintf(stderr, "Can't create %s file for empty Model3D.\n", FileName);
        return;
	}


	SDL_RWops *FileVW3D;
	FileVW3D = SDL_RWFromFile(FileName, "wb");
	// если не можем создать файл на запись - уходим
    if (FileVW3D == NULL)
    {
        fprintf(stderr, "Can't create %s file on disk.\n", FileName);
        return;
    }

	// маркер файла 4 байта
	char tmp1[5] = "VW3D";
	SDL_RWwrite(FileVW3D, tmp1, 4, 1);

	// общее кол-во объектов в моделе - 4 байта (int)
	SDL_RWwrite(FileVW3D, &DrawObjectCount, sizeof(int), 1);

	unsigned int RealVertexCount = 0;
	unsigned int RealIndexCount = 0;

	// для каждого объекта в моделе
	for (int i=0; i<DrawObjectCount; i++)
	{
		// FVF_Format
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].FVF_Format, sizeof(int), 1);
		// Stride
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Stride, sizeof(int), 1);
		// VertexCount
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexCount, sizeof(int), 1);

		// Location
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Location, sizeof(float)*3, 1);
		// Rotation
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Rotation, sizeof(float)*3, 1);

		// вычисляем кол-во вертексов в моделе (один вертекс может использоваться несколько раз в индекс буфере)
		// т.е. нам надо просмотреть индекс буфер и найти максимальный номер вертекса.
		for (int j=0; j<DrawObjectList[i].VertexCount; j++)
		{
			if (RealVertexCount < GlobalIndexBuffer[RealIndexCount+j]) RealVertexCount = GlobalIndexBuffer[RealIndexCount+j];
		}
		// считаем сколько у нас всего вертексов на прорисовке, чтобы знать размер индексного буфера
		RealIndexCount += DrawObjectList[i].VertexCount;
	}

	// надо прибавить 1, мы ведь считали вертексы с нуля
	RealVertexCount++;
	// записываем реальное кол-во вертексов в общем вертекс буфере, мы их посчитали заранее
	SDL_RWwrite(FileVW3D, &RealVertexCount, sizeof(unsigned int), 1);

	// данные, вертексы (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	SDL_RWwrite(FileVW3D, GlobalVertexBuffer, DrawObjectList[0].Stride*RealVertexCount*sizeof(float), 1);

	// данные, индексный буфер
	SDL_RWwrite(FileVW3D, GlobalIndexBuffer, RealIndexCount*sizeof(unsigned int), 1);

	// закрываем файл
	SDL_RWclose(FileVW3D);

	printf("VW3D Write: %s\n", FileName);
}
