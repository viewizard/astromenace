/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_VAO.cpp

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

