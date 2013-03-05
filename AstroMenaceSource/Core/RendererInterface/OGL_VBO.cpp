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


#include "RendererInterface.h"


// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC 		glGenBuffersARB = NULL;		// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC 		glBindBufferARB = NULL;		// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC 		glBufferDataARB = NULL;		// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC 	glDeleteBuffersARB = NULL;	// VBO Deletion Procedure
PFNGLISBUFFERARBPROC 		glIsBufferARB = NULL;






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

	if (glGenBuffersARB == NULL || glBindBufferARB == NULL || glBufferDataARB == NULL ||
		glDeleteBuffersARB == NULL || glIsBufferARB == NULL)
	{
		glGenBuffersARB 	= NULL;
		glBindBufferARB 	= NULL;
		glBufferDataARB 	= NULL;
		glDeleteBuffersARB 	= NULL;
		glIsBufferARB 		= NULL;

		return false;
	}

	return true;
}




//------------------------------------------------------------------------------------
// Процедура генерации буферов
//------------------------------------------------------------------------------------
bool vw_BuildVBO(int NumVertices, void *Data, int Stride, unsigned int *VBO)
{
	if (Data == 0) return false;
	if (VBO == 0) return false;
	if (glGenBuffersARB == NULL) return false;
	if (glBindBufferARB == NULL) return false;
	if (glBufferDataARB == NULL) return false;
	if (glIsBufferARB == NULL) return false;

	glGenBuffersARB( 1, VBO );						// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, *VBO );	// Bind The Buffer
	// Load The Data
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, NumVertices*Stride*sizeof(float), Data, GL_STATIC_DRAW_ARB );
	// убираем буфер
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

	if (!glIsBufferARB(*VBO)) return false;

	return true;
}


//------------------------------------------------------------------------------------
// Процедура генерации индекс буферов
//------------------------------------------------------------------------------------
bool vw_BuildIBO(int NumIndex, void *Data, unsigned int *IBO)
{
	if (Data == 0) return false;
	if (IBO == 0) return false;
	if (glGenBuffersARB == NULL) return false;
	if (glBindBufferARB == NULL) return false;
	if (glBufferDataARB == NULL) return false;
	if (glIsBufferARB == NULL) return false;

	glGenBuffersARB( 1, IBO );								// Get A Valid Name
	glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, *IBO );	// Bind The Buffer
	// Load The Data
	glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, NumIndex*sizeof(unsigned int), Data, GL_STATIC_DRAW_ARB );
	// убираем буфер
	glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );

	if (!glIsBufferARB(*IBO)) return false;

	return true;
}


//------------------------------------------------------------------------------------
// Установка текущего буфера
//------------------------------------------------------------------------------------
void vw_BindVBO(int target, unsigned int VBO)
{
	if (glBindBufferARB == NULL) return;

	switch (target)
	{
		case RI_ARRAY_BUFFER: 			glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBO); break;
		case RI_ELEMENT_ARRAY_BUFFER: 	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, VBO); break;
		default: fprintf(stderr, "Error in vw_BindVBO function call, wrong target.\n"); break;
	}
}





//------------------------------------------------------------------------------------
// Процедура удаления буфера
//------------------------------------------------------------------------------------
void vw_DeleteVBO(unsigned int VBO)
{
	if (glIsBufferARB == NULL) return;
	if (glDeleteBuffersARB == NULL) return;

	if (glIsBufferARB(VBO))	glDeleteBuffersARB(1, &VBO);
}

