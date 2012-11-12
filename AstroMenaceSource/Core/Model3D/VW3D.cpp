/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: VW3D.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

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
bool eModel3D::ReadVW3D(const char *FileName)
{
	eFILE *file = 0;
	file = vw_fopen(FileName);
	if (file == 0) return false;

	size_t SizeB = strlen(FileName)+1;
	Name = new char[SizeB];
	strcpy(Name, FileName);

	// "пропускаем" заголовок "VW3D"
	file->fread(&DrawObjectCount, 4, 1);

	// читаем, сколько объектов
	file->fread(&DrawObjectCount, sizeof(int), 1);

	DrawObjectList = new eObjectBlock[DrawObjectCount];

	GlobalIndexCount = 0;


	// для каждого объекта
	for (int i=0; i<DrawObjectCount; i++)
	{
		DrawObjectList[i].RangeStart = GlobalIndexCount;

		// FVF_Format
		file->fread(&(DrawObjectList[i].FVF_Format),sizeof(int),1);
		// Stride
		file->fread(&(DrawObjectList[i].Stride),sizeof(int),1);
		// VertexCount на самом деле, это кол-во индексов на объект
		file->fread(&(DrawObjectList[i].VertexCount),sizeof(int),1);
		GlobalIndexCount += DrawObjectList[i].VertexCount;

		// Location
		file->fread(&(DrawObjectList[i].Location),sizeof(float)*3,1);
		// Rotation
		file->fread(&(DrawObjectList[i].Rotation),sizeof(float)*3,1);

		// рисуем нормально, не прозрачным
		DrawObjectList[i].DrawType = 0;

		// вертексный буфер
		DrawObjectList[i].NeedDestroyDataInObjectBlock = false;
		DrawObjectList[i].VertexBuffer = 0;
		DrawObjectList[i].VBO = 0;
		// индексный буфер
		DrawObjectList[i].IndexBuffer = 0;
		DrawObjectList[i].IBO = 0;
		// vao
		DrawObjectList[i].VAO = 0;
	}

	// получаем сколько всего вертексов
	file->fread(&GlobalVertexCount,sizeof(unsigned int),1);

	// собственно данные (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	GlobalVertexBuffer = new float[GlobalVertexCount*DrawObjectList[0].Stride];
	file->fread(GlobalVertexBuffer,	GlobalVertexCount*DrawObjectList[0].Stride*sizeof(float),1);

	// индекс буфер
	GlobalIndexBuffer = new unsigned int[GlobalIndexCount];
	file->fread(GlobalIndexBuffer, GlobalIndexCount*sizeof(unsigned int),1);

	// т.к. наши объекты используют глобальные буферы, надо поставить указатели
	for (int i=0; i<DrawObjectCount; i++)
	{
		DrawObjectList[i].VertexBuffer = GlobalVertexBuffer;
		DrawObjectList[i].IndexBuffer = GlobalIndexBuffer;
	}

	vw_fclose(file);

	return true;
}






//-----------------------------------------------------------------------------
// запись "родного" формата на диск
//-----------------------------------------------------------------------------
bool eModel3D::WriteVW3D(const char *FileName)
{
	// небольшие проверки
	if ((GlobalVertexBuffer == 0) || (GlobalIndexBuffer == 0) || (DrawObjectList == 0))
	{
        fprintf(stderr, "Can't create %s file for empty Model3D.\n", FileName);
        return false;
	}


	SDL_RWops *FileVW3D;
	FileVW3D = SDL_RWFromFile(FileName, "wb");
	// если не можем создать файл на запись - уходим
    if (FileVW3D == NULL)
    {
        fprintf(stderr, "Can't create %s file on disk.\n", FileName);
        return false;
    }

	// маркер файла 4 байта
	char tmp1[5] = "VW3D";
	SDL_RWwrite(FileVW3D, tmp1, 4, 1);

	// общее кол-во объектов в моделе - 4 байта (int)
	SDL_RWwrite(FileVW3D, &DrawObjectCount, sizeof(int), 1);

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
	}

	// записываем реальное кол-во вертексов в общем вертекс буфере, мы их посчитали заранее
	SDL_RWwrite(FileVW3D, &GlobalVertexCount, sizeof(unsigned int), 1);

	// данные, вертексы (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	SDL_RWwrite(FileVW3D, GlobalVertexBuffer, DrawObjectList[0].Stride*GlobalVertexCount*sizeof(float), 1);

	// данные, индексный буфер
	SDL_RWwrite(FileVW3D, GlobalIndexBuffer, GlobalIndexCount*sizeof(unsigned int), 1);

	// закрываем файл
	SDL_RWclose(FileVW3D);

	printf("VW3D Write: %s\n", FileName);
	return true;
}
