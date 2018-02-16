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
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/


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
