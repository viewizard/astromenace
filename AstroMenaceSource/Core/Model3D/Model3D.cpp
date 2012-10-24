/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Model3D.cpp

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
// Конструктор
//-----------------------------------------------------------------------------
eObjectBlock::eObjectBlock(void)
{
	FVF_Format = 0;
	Stride = 0;
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
			if ((DrawObjectList[i].VertexBuffer != 0) & (DrawObjectList[i].VertexBuffer != GlobalVertexBuffer))
			{
				delete [] DrawObjectList[i].VertexBuffer;
				DrawObjectList[i].VertexBuffer = 0;
			}
			if ((DrawObjectList[i].IndexBuffer != 0) & (DrawObjectList[i].IndexBuffer != GlobalIndexBuffer))
			{
				delete [] DrawObjectList[i].IndexBuffer;
				DrawObjectList[i].IndexBuffer = 0;
			}
			if ((DrawObjectList[i].VBO != 0) & (DrawObjectList[i].VBO != GlobalVBO))
			{
				vw_DeleteVBO(*DrawObjectList[i].VBO);
				delete DrawObjectList[i].VBO; DrawObjectList[i].VBO = 0;
			}
			if ((DrawObjectList[i].IBO != 0) & (DrawObjectList[i].IBO != GlobalIBO))
			{
				vw_DeleteVBO(*DrawObjectList[i].IBO);
				delete DrawObjectList[i].IBO; DrawObjectList[i].IBO = 0;
			}
			if ((DrawObjectList[i].VAO != 0) & (DrawObjectList[i].VAO != GlobalVAO))
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
	}
}




//-----------------------------------------------------------------------------
// создаем все поддерживаемые буферы (VAO, VBO, IBO)
//-----------------------------------------------------------------------------
void eModel3D::CreateHardwareBuffers()
{
	// делаем общее VBO
	GlobalVBO = new unsigned int;
	if (!vw_BuildVBO(GlobalVertexCount, GlobalVertexBuffer, DrawObjectList[0].Stride, GlobalVBO))
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
	if (!vw_BuildVAO(GlobalVAO, GlobalIndexCount, DrawObjectList[0].FVF_Format, GlobalVertexBuffer,
						DrawObjectList[0].Stride*sizeof(float), GlobalVBO, 0,
						GlobalIndexBuffer, GlobalIBO))
	{
		delete GlobalVAO; GlobalVAO=0;
	}


	// создаем буферы для каждого объекта
	for (int i=0; i<DrawObjectCount; i++)
	{
		// делаем VBO
		DrawObjectList[i].VBO = new unsigned int;
		if (!vw_BuildVBO(DrawObjectList[i].VertexCount, DrawObjectList[i].VertexBuffer, DrawObjectList[i].Stride, DrawObjectList[i].VBO))
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
		if (!vw_BuildVAO(DrawObjectList[i].VAO, DrawObjectList[i].VertexCount, DrawObjectList[i].FVF_Format, DrawObjectList[i].VertexBuffer,
							DrawObjectList[i].Stride*sizeof(float), DrawObjectList[i].VBO,
							DrawObjectList[i].RangeStart, DrawObjectList[i].IndexBuffer, DrawObjectList[i].IBO))
		{
			delete DrawObjectList[i].VAO; DrawObjectList[i].VAO=0;
		}
	}

}
