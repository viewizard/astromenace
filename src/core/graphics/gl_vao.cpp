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

// NOTE ARB_vertex_attrib_binding (since OpenGL 4.3)
//      specify the attribute format and the attribute data separately
// NOTE ARB_direct_state_access (since OpenGL 4.5)
//      no longer need to call glBindBuffer() or glBindVertexArray()

#include "graphics.h"

PFNGLGENVERTEXARRAYSPROC 	glGenVertexArraysARB = nullptr;		// VAO Name Generation Procedure
PFNGLBINDVERTEXARRAYPROC 	glBindVertexArrayARB = nullptr;		// VAO Bind Procedure
PFNGLDELETEVERTEXARRAYSPROC	glDeleteVertexArraysARB = nullptr;	// VAO Deletion Procedure
PFNGLISVERTEXARRAYPROC 		glIsVertexArrayARB = nullptr;

GLuint *vw_SendVertices_EnableStatesAndPointers(int NumVertices, int DataFormat, void *Data, int Stride, unsigned int VertexBO,
						unsigned int RangeStart, unsigned int *IndexArray, unsigned int IndexBO);
void vw_SendVertices_DisableStatesAndPointers(int DataFormat, unsigned int VBO, unsigned int *VAO);


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

	if (glGenVertexArraysARB == nullptr || glBindVertexArrayARB == nullptr ||
	    glDeleteVertexArraysARB == nullptr || glIsVertexArrayARB == nullptr) {
		glGenVertexArraysARB 	= nullptr;
		glBindVertexArrayARB 	= nullptr;
		glDeleteVertexArraysARB = nullptr;
		glIsVertexArrayARB 		= nullptr;

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------
// Процедура генерации буферов
//------------------------------------------------------------------------------------
bool vw_BuildVAO(unsigned int *VAO, int NumVertices, int DataFormat, void *Data, int Stride, unsigned int VBO,
		 unsigned int RangeStart, unsigned int *DataIndex, unsigned int DataIndexVBO)
{
	if (!VAO ||
	    !glGenVertexArraysARB ||
	    !glIsVertexArrayARB)
		return false;

	glGenVertexArraysARB(1, VAO);

	vw_BindVAO(*VAO);
	vw_SendVertices_EnableStatesAndPointers(NumVertices, DataFormat, Data, Stride, VBO,
						RangeStart, DataIndex, DataIndexVBO);

	vw_BindVAO(0);
	vw_SendVertices_DisableStatesAndPointers(DataFormat, VBO, nullptr);

	if (!glIsVertexArrayARB(*VAO))
		return false;

	return true;
}

//------------------------------------------------------------------------------------
// Установка текущего буфера
//------------------------------------------------------------------------------------
void vw_BindVAO(unsigned int VAO)
{
	if (!glBindVertexArrayARB)
		return;

	glBindVertexArrayARB(VAO);
}

//------------------------------------------------------------------------------------
// Процедура удаления буфера
//------------------------------------------------------------------------------------
void vw_DeleteVAO(unsigned int VAO)
{
	if (!glIsVertexArrayARB ||
	    !glDeleteVertexArraysARB ||
	    !glIsVertexArrayARB(VAO))
		return;

	glDeleteVertexArraysARB(1, &VAO);
}
