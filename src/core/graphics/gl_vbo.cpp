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


#include "graphics.h"


// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC 		glGenBuffersARB = nullptr;		// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC 		glBindBufferARB = nullptr;		// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC 		glBufferDataARB = nullptr;		// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC 	glDeleteBuffersARB = nullptr;	// VBO Deletion Procedure
PFNGLISBUFFERARBPROC 		glIsBufferARB = nullptr;






//------------------------------------------------------------------------------------
// Инициализация работы с VBO
//------------------------------------------------------------------------------------
bool vw_Internal_InitializationVBO()
{
	// Get Pointers To The GL Functions
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");
	glIsBufferARB = (PFNGLISBUFFERARBPROC) SDL_GL_GetProcAddress("glIsBufferARB");

	if (glGenBuffersARB == nullptr || glBindBufferARB == nullptr || glBufferDataARB == nullptr ||
	    glDeleteBuffersARB == nullptr || glIsBufferARB == nullptr) {
		glGenBuffersARB 	= nullptr;
		glBindBufferARB 	= nullptr;
		glBufferDataARB 	= nullptr;
		glDeleteBuffersARB 	= nullptr;
		glIsBufferARB 		= nullptr;

		return false;
	}

	return true;
}




//------------------------------------------------------------------------------------
// Процедура генерации буферов
//------------------------------------------------------------------------------------
bool vw_BuildVBO(int NumVertices, void *Data, int Stride, unsigned int *VBO)
{
	if ((Data == nullptr) || (VBO == nullptr) ||
	    (glGenBuffersARB == nullptr) ||
	    (glBindBufferARB == nullptr) ||
	    (glBufferDataARB == nullptr) ||
	    (glIsBufferARB == nullptr))
		return false;

	glGenBuffersARB(1, VBO);			// Get A Valid Name
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, *VBO);	// Bind The Buffer
	// Load The Data
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, NumVertices*Stride*sizeof(float), Data, GL_STATIC_DRAW_ARB);
	// убираем буфер
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	if (!glIsBufferARB(*VBO))
		return false;

	return true;
}


//------------------------------------------------------------------------------------
// Процедура генерации индекс буферов
//------------------------------------------------------------------------------------
bool vw_BuildIBO(int NumIndex, void *Data, unsigned int *IBO)
{
	if ((Data == nullptr) || (IBO == nullptr) ||
	    (glGenBuffersARB == nullptr) ||
	    (glBindBufferARB == nullptr) ||
	    (glBufferDataARB == nullptr) ||
	    (glIsBufferARB == nullptr))
		return false;

	glGenBuffersARB(1, IBO);				// Get A Valid Name
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, *IBO);	// Bind The Buffer
	// Load The Data
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NumIndex*sizeof(unsigned int), Data, GL_STATIC_DRAW_ARB);
	// убираем буфер
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	if (!glIsBufferARB(*IBO))
		return false;

	return true;
}


//------------------------------------------------------------------------------------
// Установка текущего буфера
//------------------------------------------------------------------------------------
void vw_BindVBO(int target, unsigned int VBO)
{
	if (glBindBufferARB == nullptr) return;

	switch (target) {
	case RI_ARRAY_BUFFER:
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBO);
		break;
	case RI_ELEMENT_ARRAY_BUFFER:
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, VBO);
		break;
	default:
		fprintf(stderr, "Error in vw_BindVBO function call, wrong target.\n");
		break;
	}
}





//------------------------------------------------------------------------------------
// Процедура удаления буфера
//------------------------------------------------------------------------------------
void vw_DeleteVBO(unsigned int VBO)
{
	if (glIsBufferARB == nullptr) return;
	if (glDeleteBuffersARB == nullptr) return;

	if (glIsBufferARB(VBO))	glDeleteBuffersARB(1, &VBO);
}

