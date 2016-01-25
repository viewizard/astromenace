/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#include "RendererInterface.h"


// VAO Extension Function Pointers
PFNGLGENVERTEXARRAYSPROC 		glGenVertexArraysARB = NULL;		// VAO Name Generation Procedure
PFNGLBINDVERTEXARRAYPROC 		glBindVertexArrayARB = NULL;		// VAO Bind Procedure
PFNGLDELETEVERTEXARRAYSPROC 	glDeleteVertexArraysARB = NULL;		// VAO Deletion Procedure
PFNGLISVERTEXARRAYPROC 			glIsVertexArrayARB = NULL;


void vw_SendVertices_DisableStatesAndPointers(int DataFormat, unsigned int *VBO, unsigned int *VAO);
GLuint *vw_SendVertices_EnableStatesAndPointers(int NumVertices, int DataFormat, void *Data, int Stride, unsigned int *VBO, unsigned int RangeStart, unsigned int *DataIndex, unsigned int *DataIndexVBO);



//------------------------------------------------------------------------------------
// Инициализация работы с VAO
//------------------------------------------------------------------------------------
bool vw_Internal_InitializationVAO()
{
	// Get Pointers To The GL Functions
	glGenVertexArraysARB = (PFNGLGENVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glGenVertexArrays");
	glBindVertexArrayARB = (PFNGLBINDVERTEXARRAYPROC) SDL_GL_GetProcAddress("glBindVertexArray");
	glDeleteVertexArraysARB = (PFNGLDELETEVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glDeleteVertexArrays");
	glIsVertexArrayARB = (PFNGLISVERTEXARRAYPROC) SDL_GL_GetProcAddress("glIsVertexArray");

	if (glGenVertexArraysARB == NULL || glBindVertexArrayARB == NULL ||
		glDeleteVertexArraysARB == NULL || glIsVertexArrayARB == NULL)
	{
		glGenVertexArraysARB 	= NULL;
		glBindVertexArrayARB 	= NULL;
		glDeleteVertexArraysARB = NULL;
		glIsVertexArrayARB 		= NULL;

		return false;
	}

	return true;
}




//------------------------------------------------------------------------------------
// Процедура генерации буферов
//------------------------------------------------------------------------------------
bool vw_BuildVAO(unsigned int *VAO, int NumVertices, int DataFormat, void *Data, int Stride, unsigned int *VBO,
						unsigned int RangeStart, unsigned int *DataIndex, unsigned int *DataIndexVBO)
{
	if (VAO == 0) return false;
	if (glGenVertexArraysARB == NULL) return false;
	if (glIsVertexArrayARB == NULL) return false;

	glGenVertexArraysARB(1, VAO);

	vw_BindVAO(*VAO);

	vw_SendVertices_EnableStatesAndPointers(NumVertices, DataFormat, Data, Stride, VBO,
						RangeStart, DataIndex, DataIndexVBO);

	vw_BindVAO(0);

	vw_SendVertices_DisableStatesAndPointers(DataFormat, VBO, 0);

	if (!glIsVertexArrayARB(*VAO)) return false;

	return true;
}




//------------------------------------------------------------------------------------
// Установка текущего буфера
//------------------------------------------------------------------------------------
void vw_BindVAO(unsigned int VAO)
{
	if (glBindVertexArrayARB == NULL) return;

	glBindVertexArrayARB(VAO);
}





//------------------------------------------------------------------------------------
// Процедура удаления буфера
//------------------------------------------------------------------------------------
void vw_DeleteVAO(unsigned int VAO)
{
	if (glIsVertexArrayARB == NULL) return;
	if (glDeleteVertexArraysARB == NULL) return;

	if (glIsVertexArrayARB(VAO)) glDeleteVertexArraysARB(1, &VAO);
}

