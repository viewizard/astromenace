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

	NeedDestroyDataInObjectBlock = false;
	VertexBuffer = 0;
	VertexBufferVBO = 0;
	IndexBuffer = 0;
	IndexBufferVBO = 0;
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
		if (VertexBufferVBO != 0)
		{
			vw_DeleteVBO(*VertexBufferVBO); delete VertexBufferVBO; VertexBufferVBO = 0;
		}
		if (IndexBuffer != 0)
		{
			delete [] IndexBuffer; IndexBuffer = 0;
		}
		if (VertexBufferVBO != 0)
		{
			vw_DeleteVBO(*IndexBufferVBO); delete IndexBufferVBO; IndexBufferVBO = 0;
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
	GlobalVertexBufferVBO = 0;
	GlobalIndexBuffer = 0;
	GlobalIndexBufferVBO = 0;
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
			if (DrawObjectList[i].VertexBuffer != 0)
			{
				delete [] DrawObjectList[i].VertexBuffer;
				DrawObjectList[i].VertexBuffer = 0;
			}
			if (DrawObjectList[i].IndexBuffer != 0)
			{
				delete [] DrawObjectList[i].IndexBuffer;
				DrawObjectList[i].IndexBuffer = 0;
			}
			if (DrawObjectList[i].VertexBufferVBO != 0)
			{
				vw_DeleteVBO(*DrawObjectList[i].VertexBufferVBO);
				delete DrawObjectList[i].VertexBufferVBO; DrawObjectList[i].VertexBufferVBO = 0;
			}
			if (DrawObjectList[i].IndexBufferVBO != 0)
			{
				vw_DeleteVBO(*DrawObjectList[i].IndexBufferVBO);
				delete DrawObjectList[i].IndexBufferVBO; DrawObjectList[i].IndexBufferVBO = 0;
			}
			if (DrawObjectList[i].VAO != 0)
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
	if (GlobalVertexBufferVBO != 0)
	{
		vw_DeleteVBO(*GlobalVertexBufferVBO); delete GlobalVertexBufferVBO; GlobalVertexBufferVBO=0;
	}
	if (GlobalIndexBuffer != 0)
	{
		delete [] GlobalIndexBuffer; GlobalIndexBuffer = 0;
	}
	if (GlobalIndexBufferVBO != 0)
	{
		vw_DeleteVBO(*GlobalIndexBufferVBO); delete GlobalIndexBufferVBO; GlobalIndexBufferVBO=0;
	}
	if (GlobalVAO != 0)
	{
		vw_DeleteVAO(*GlobalVAO); delete GlobalVAO; GlobalVAO=0;
	}

	DrawObjectCount = 0;
	vw_DetachModel3D(this);
}
