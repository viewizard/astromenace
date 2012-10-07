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

	// пропускаем заголовок
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
		DrawObjectList[i].VertexBufferDestrType = 0;
		DrawObjectList[i].VertexBuffer = 0;
		DrawObjectList[i].VertexBufferVBO = 0;
		// индексный буфер
		DrawObjectList[i].IndexBuffer = 0;
		DrawObjectList[i].IndexBufferVBO = 0;
	}

	// получаем сколько всего вертексов
	int VCount = 0;
	file->fread(&VCount,sizeof(int),1);

	// собственно данные
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
		DrawObjectList[i].VertexBuffer = GlobalVertexBuffer;
		DrawObjectList[i].VertexBufferVBO = GlobalVertexBufferVBO;
		DrawObjectList[i].IndexBuffer = GlobalIndexBuffer;
		DrawObjectList[i].IndexBufferVBO = GlobalIndexBufferVBO;
		DrawObjectList[i].VAO = 0;

		// не ставим генерацию vao тут! т.к. иначе не сможем работать с изменением геометрии и текстурных координат "на лету" - гусеницы, колеса, пушки и т.п.
		// если надо будет - найти изменения геометрии и делать VAO уже после изменений, с правильным vbo
/*		DrawObjectList[i].VAO = new unsigned int;
		if (!vw_BuildVAO(DrawObjectList[i].VAO, DrawObjectList[i].VertexCount, DrawObjectList[i].FVF_Format, DrawObjectList[i].VertexBuffer,
							DrawObjectList[i].Stride*sizeof(float), DrawObjectList[i].VertexBufferVBO,
							DrawObjectList[i].RangeStart, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IndexBufferVBO))
		{
			delete GlobalVAO; GlobalVAO=0;
		}*/



	}


	vw_fclose(file);
}
