/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_VBO.cpp

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

