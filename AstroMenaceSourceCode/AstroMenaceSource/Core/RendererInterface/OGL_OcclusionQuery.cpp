/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_OcclusionQuery.cpp

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


// функции для использования Occlusion Queries
PFNGLGENQUERIESARBPROC        glGenQueriesARB        = NULL;
PFNGLDELETEQUERIESARBPROC     glDeleteQueriesARB     = NULL;
PFNGLISQUERYARBPROC           glIsQueryARB           = NULL;
PFNGLBEGINQUERYARBPROC        glBeginQueryARB        = NULL;
PFNGLENDQUERYARBPROC          glEndQueryARB          = NULL;
PFNGLGETQUERYIVARBPROC        glGetQueryivARB        = NULL;
PFNGLGETQUERYOBJECTIVARBPROC  glGetQueryObjectivARB  = NULL;
PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB = NULL;




//------------------------------------------------------------------------------------
// Инициализации работы с Occlusion Queries
//------------------------------------------------------------------------------------
bool vw_Internal_InitializationOcclusionQueries()
{
	// Get Pointers To The GL Functions
	glGenQueriesARB        = (PFNGLGENQUERIESARBPROC) SDL_GL_GetProcAddress("glGenQueriesARB");
	glDeleteQueriesARB     = (PFNGLDELETEQUERIESARBPROC) SDL_GL_GetProcAddress("glDeleteQueriesARB");
	glIsQueryARB           = (PFNGLISQUERYARBPROC) SDL_GL_GetProcAddress("glIsQueryARB");
	glBeginQueryARB        = (PFNGLBEGINQUERYARBPROC) SDL_GL_GetProcAddress("glBeginQueryARB");
	glEndQueryARB          = (PFNGLENDQUERYARBPROC) SDL_GL_GetProcAddress("glEndQueryARB");
	glGetQueryivARB        = (PFNGLGETQUERYIVARBPROC) SDL_GL_GetProcAddress("glGetQueryivARB");
	glGetQueryObjectivARB  = (PFNGLGETQUERYOBJECTIVARBPROC) SDL_GL_GetProcAddress("glGetQueryObjectivARB");
	glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC) SDL_GL_GetProcAddress("glGetQueryObjectuivARB");

	// если хоть с одной функцией проблемы - не работаем вообще
	if (glGenQueriesARB == NULL || glDeleteQueriesARB == NULL || glIsQueryARB == NULL ||
		glBeginQueryARB == NULL || glEndQueryARB == NULL || glGetQueryivARB == NULL ||
		glGetQueryObjectivARB == NULL || glGetQueryObjectuivARB)
	{
		glGenQueriesARB        = NULL;
		glDeleteQueriesARB     = NULL;
		glIsQueryARB           = NULL;
		glBeginQueryARB        = NULL;
		glEndQueryARB          = NULL;
		glGetQueryivARB        = NULL;
		glGetQueryObjectivARB  = NULL;
		glGetQueryObjectuivARB = NULL;
		return false;
	}

	return true;
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GenQueries(int n, unsigned int *ids)
{
	if (glGenQueriesARB == NULL) return;
	glGenQueriesARB(n, ids);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_DeleteQueries(int n, const unsigned int *ids)
{
	if (glDeleteQueriesARB == NULL) return;
	glDeleteQueriesARB(n, ids);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
bool vw_IsQuery(unsigned int id)
{
	if (glIsQueryARB == NULL) return false;
	return glIsQueryARB(id);
}


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_BeginQuery(unsigned int id)
{
	if (glBeginQueryARB == NULL) return;
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, id);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_EndQuery()
{
	if (glEndQueryARB == NULL) return;
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GetQueryiv(unsigned int target, unsigned int pname, int *params)
{
	if (glGetQueryivARB == NULL) return;
	glGetQueryivARB(target, pname, params);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GetQueryObjectiv(unsigned int id, unsigned int pname, int *params)
{
	if (glGetQueryObjectivARB == NULL) return;
	glGetQueryObjectivARB(id, pname, params);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GetQueryObjectuiv(unsigned int id, unsigned int pname, unsigned int *params)
{
	if (glGetQueryObjectuivARB == NULL) return;
	glGetQueryObjectuivARB(id, pname, params);
}
