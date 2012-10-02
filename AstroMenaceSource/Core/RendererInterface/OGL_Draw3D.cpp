/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_Draw3D.cpp

	Copyright (c) 2003-2007 Michael Kurinnoy, Viewizard
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


#include "RendererInterface.h"



extern	int tmpPrimCountGL;
extern	PFNGLCLIENTACTIVETEXTUREPROC	glClientActiveTexture13;







//------------------------------------------------------------------------------------
// данный для индекс буфера
//------------------------------------------------------------------------------------
int VertexIndexCount = 0;
GLuint *VertexIndex = 0;
GLuint *IndexVBO = 0;

//------------------------------------------------------------------------------------
// Инициализация данных индекс буфера
//------------------------------------------------------------------------------------
void vw_Internal_InitializationIndexBufferData()
{
	VertexIndexCount = 0;
	VertexIndex = 0;
	IndexVBO = 0;
}
//------------------------------------------------------------------------------------
// Чистка памяти данных индекс буфера
//------------------------------------------------------------------------------------
void vw_Internal_ReleaseIndexBufferData()
{
	if (VertexIndex != 0){delete [] VertexIndex; VertexIndex = 0;}
	VertexIndexCount = 0;
	if (IndexVBO != 0){vw_DeleteVBO(*IndexVBO); delete IndexVBO; IndexVBO=0;}
}








//------------------------------------------------------------------------------------
// Процедура передачи последовательности вертексов для прорисовки
//------------------------------------------------------------------------------------
void vw_SendVertices(int PrimitiveType, int NumVertices, int DataFormat, void *Data, int Stride, unsigned int *VBO, unsigned int RangeStart, unsigned int *DataIndex, unsigned int *DataIndexVBO)
{
	// если ничего не передали
	if (Data == 0 && VBO == 0) return;

	// флаг нужно ли с вбо делать
	eDevCaps *OpenGL_DevCaps = vw_GetDevCaps();
	bool NeedVBO = OpenGL_DevCaps->VBOSupported;
	if (VBO == 0) NeedVBO = false;

	// еще одна проверка, есть ли что рисовать
	if (Data == 0 && !NeedVBO) return;


	// обязательно в байты, т.к. делаем смещение в байтах!
	BYTE *TMP = (BYTE *)Data;

	// чтобы знать сколько отступать, кол-во ед. элементов, в нашем случае float
	intptr_t AddStride = 0;
	// кол-во текстур
	int TextQ = DataFormat & 0x000000F;
	// длина блока
	int TextSize = 2;
	int TextCoordType = 1; // float
	switch (DataFormat & 0x0F00000)
	{
		case 0x0100000: TextSize = 1;	break;
		case 0x0200000: TextSize = 2;	break;
		case 0x0300000: TextSize = 3;	break;
		case 0x0400000: TextSize = 4;	break;
		// short
		case 0x0500000: TextSize = 1; TextCoordType = 2;	break;
		case 0x0600000: TextSize = 2; TextCoordType = 2;	break;
		case 0x0700000: TextSize = 3; TextCoordType = 2;	break;
		case 0x0800000: TextSize = 4; TextCoordType = 2;	break;
	}


	if (NeedVBO) vw_BuindVBO(RI_ARRAY_BUFFER, *VBO);


	// делаем установку поинтеров + ставим смещения для прорисовки
	if ((DataFormat & 0x000F000) == RI_3f_XYZ)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (NeedVBO)
			glVertexPointer(3, GL_FLOAT, Stride, (intptr_t *)(AddStride));
		else
			glVertexPointer(3, GL_FLOAT, Stride, TMP + AddStride);
		AddStride += 3*sizeof(GLfloat);
	}
	if ((DataFormat & 0x000F000) == RI_3i_XYZ)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (NeedVBO)
			glVertexPointer(3, GL_INT, Stride, (BYTE *)(AddStride));
		else
			glVertexPointer(3, GL_INT, Stride, TMP + AddStride);
		AddStride += 3*sizeof(GLint);
	}
	if ((DataFormat & 0x000F000) == RI_3s_XYZ)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (NeedVBO)
			glVertexPointer(3, GL_SHORT, Stride, (BYTE *)(AddStride));
		else
			glVertexPointer(3, GL_SHORT, Stride, TMP + AddStride);
		AddStride += 3*sizeof(GLshort);
	}

	if ((DataFormat & 0x000F000) == RI_2f_XYZ)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (NeedVBO)
			glVertexPointer(2, GL_FLOAT, Stride, (BYTE *)(AddStride));
		else
			glVertexPointer(2, GL_FLOAT, Stride, TMP + AddStride);
		AddStride += 2*sizeof(GLfloat);
	}
	if ((DataFormat & 0x000F000) == RI_2s_XYZ)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (NeedVBO)
			glVertexPointer(2, GL_SHORT, Stride, (BYTE *)(AddStride));
		else
			glVertexPointer(2, GL_SHORT, Stride, TMP + AddStride);
		AddStride += 2*sizeof(GLshort);
	}


	if ((DataFormat & 0x0000F00) == RI_3f_NORMAL)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		if (NeedVBO)
			glNormalPointer(GL_FLOAT, Stride, (BYTE *)(AddStride));
		else
			glNormalPointer(GL_FLOAT, Stride, TMP + AddStride);
		AddStride += 3*sizeof(GLfloat);
	}


	if ((DataFormat & 0x00000F0) == RI_4f_COLOR)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		if (NeedVBO)
			glColorPointer(4, GL_FLOAT, Stride, (BYTE *)(AddStride));
		else
			glColorPointer(4, GL_FLOAT, Stride, TMP + AddStride);
		AddStride += 4*sizeof(GLfloat);
	}
	if ((DataFormat & 0x00000F0) == RI_4ub_COLOR)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		if (NeedVBO)
			glColorPointer(4, GL_UNSIGNED_BYTE, Stride, (BYTE *)(AddStride));
		else
			glColorPointer(4, GL_UNSIGNED_BYTE, Stride, TMP + AddStride);
		AddStride += 4*sizeof(GLubyte);
	}


	if (TextQ > 0)// текстурные коорд. есть...
	{
		for (int i=0; i<TextQ; i++)
		{
			glClientActiveTexture13(GL_TEXTURE0+i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			switch (TextCoordType)
			{
				case 1:
				{
					if (NeedVBO)
						glTexCoordPointer(TextSize, GL_FLOAT, Stride, (BYTE *)(AddStride));
					else
						glTexCoordPointer(TextSize, GL_FLOAT, Stride, TMP + AddStride);
					if ((DataFormat & 0xF000000) == RI_SEPARATE_TEX_COORD) AddStride += TextSize*sizeof(GLfloat);
				}
				break;
				case 2:
				{
					if (NeedVBO)
						glTexCoordPointer(TextSize, GL_SHORT, Stride, (BYTE *)(AddStride));
					else
						glTexCoordPointer(TextSize, GL_SHORT, Stride, TMP + AddStride);
					if ((DataFormat & 0xF000000) == RI_SEPARATE_TEX_COORD) AddStride += TextSize*sizeof(GLshort);
				}
				break;
			}
		}
	}




	// указатель на смещение (в случае вбо) или на массив индексов
	GLuint *VertexIndexPointer = 0;
	// если нет своего, ставим общей массив индексов
	if (DataIndexVBO == 0 && DataIndex == 0)
	{
		// собираем если нужно массив индексов
		if (VertexIndexCount < NumVertices+RangeStart)
		{
			if (VertexIndex != 0){delete [] VertexIndex; VertexIndex = 0;}
			VertexIndexCount = 0;

			VertexIndex = new GLuint[NumVertices+RangeStart]; if (VertexIndex == 0) return;

			VertexIndexCount = NumVertices+RangeStart;
			for (unsigned int i=0; i<NumVertices+RangeStart; i++) VertexIndex[i] = i;

			// если держим VBO, все это один раз сразу запихиваем в видео память
			if (OpenGL_DevCaps->VBOSupported)
			{
				// прежде всего удаляем старый буфер, если он был
				if (IndexVBO != 0){vw_DeleteVBO(*IndexVBO); delete IndexVBO; IndexVBO=0;}
				// создаем новый
				IndexVBO = new GLuint;
				if (!vw_BuildIndexVBO(VertexIndexCount, VertexIndex, IndexVBO))
				{
					delete IndexVBO; IndexVBO=0;
				}
			}
		}

		VertexIndexPointer = VertexIndex+RangeStart;

		// собственно включаем индекс-вбо
		if (OpenGL_DevCaps->VBOSupported)
		if (IndexVBO != 0)
		{
			vw_BuindVBO(RI_ELEMENT_ARRAY_BUFFER, *IndexVBO);
			VertexIndexPointer = NULL;
			VertexIndexPointer = VertexIndexPointer+RangeStart;
		}
	}
	else
	{	// если массив или вбо индексов передали, просто подключаем их
		VertexIndexPointer = DataIndex+RangeStart;

		// собственно включаем индекс-вбо
		if (OpenGL_DevCaps->VBOSupported)
		if (DataIndexVBO != 0)
		{
			vw_BuindVBO(RI_ELEMENT_ARRAY_BUFFER, *DataIndexVBO);
			VertexIndexPointer = NULL;
			VertexIndexPointer = VertexIndexPointer+RangeStart;
		}
	}




// 1) Нельзя использовать short индексы (глючит в линуксе на картах нвидия, проверял на 97.55 драйвере)
// 2) Нельзя рисовать через glBegin-glEnd и glDrawArray - проблемы в линуксе у ati драйверов (на glDrawArray вообще сегфолтит)
// 3) С glDrawRangeElements могут быть неприятные сюрпризы на маках (интел+интел видео), сегфолты даже если учесть все ограничения (вертекс и индекс)


	// рисуем
	switch(PrimitiveType)
	{
		case RI_POINTS:
			glDrawElements(GL_POINTS,NumVertices,GL_UNSIGNED_INT,VertexIndexPointer);
			tmpPrimCountGL += NumVertices;
			break;

		case RI_LINES:
			glDrawElements(GL_LINES,NumVertices,GL_UNSIGNED_INT,VertexIndexPointer);
			tmpPrimCountGL += NumVertices/2;
			break;

		case RI_TRIANGLES:
			glDrawElements(GL_TRIANGLES,NumVertices,GL_UNSIGNED_INT,VertexIndexPointer);
			tmpPrimCountGL += NumVertices/3;
			break;

		case RI_TRIANGLE_STRIP:
			glDrawElements(GL_TRIANGLE_STRIP,NumVertices,GL_UNSIGNED_INT,VertexIndexPointer);
			tmpPrimCountGL += NumVertices-2;
			break;

		case RI_TRIANGLE_FAN:
			glDrawElements(GL_TRIANGLE_FAN,NumVertices,GL_UNSIGNED_INT,VertexIndexPointer);
			tmpPrimCountGL += NumVertices-2;
			break;

		case RI_QUADS:
			glDrawElements(GL_QUADS,NumVertices,GL_UNSIGNED_INT,VertexIndexPointer);
			tmpPrimCountGL += NumVertices/4;
			break;
	}




	if ((DataFormat & 0x0000F00) != 0) glDisableClientState(GL_NORMAL_ARRAY);
	if ((DataFormat & 0x00000F0) != 0) glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);


	for (int i=TextQ-1; i>=0; i--)
	{
		glClientActiveTexture13(GL_TEXTURE0+i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}


	// сбрасываем индексный и вертексный буфера, если они были установлены
	if (IndexVBO != 0) vw_BuindVBO(RI_ELEMENT_ARRAY_BUFFER, 0);
	if (NeedVBO) vw_BuindVBO(RI_ARRAY_BUFFER, 0);

}
