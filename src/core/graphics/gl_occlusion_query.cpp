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


// функции для использования Occlusion Queries
PFNGLGENQUERIESARBPROC        glGenQueriesARB        = nullptr;
PFNGLDELETEQUERIESARBPROC     glDeleteQueriesARB     = nullptr;
PFNGLISQUERYARBPROC           glIsQueryARB           = nullptr;
PFNGLBEGINQUERYARBPROC        glBeginQueryARB        = nullptr;
PFNGLENDQUERYARBPROC          glEndQueryARB          = nullptr;
PFNGLGETQUERYIVARBPROC        glGetQueryivARB        = nullptr;
PFNGLGETQUERYOBJECTIVARBPROC  glGetQueryObjectivARB  = nullptr;
PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB = nullptr;




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
	if (glGenQueriesARB == nullptr || glDeleteQueriesARB == nullptr || glIsQueryARB == nullptr ||
	    glBeginQueryARB == nullptr || glEndQueryARB == nullptr || glGetQueryivARB == nullptr ||
	    glGetQueryObjectivARB == nullptr || glGetQueryObjectuivARB) {
		glGenQueriesARB        = nullptr;
		glDeleteQueriesARB     = nullptr;
		glIsQueryARB           = nullptr;
		glBeginQueryARB        = nullptr;
		glEndQueryARB          = nullptr;
		glGetQueryivARB        = nullptr;
		glGetQueryObjectivARB  = nullptr;
		glGetQueryObjectuivARB = nullptr;
		return false;
	}

	return true;
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GenQueries(int n, unsigned int *ids)
{
	if (glGenQueriesARB == nullptr) return;
	glGenQueriesARB(n, ids);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_DeleteQueries(int n, const unsigned int *ids)
{
	if (glDeleteQueriesARB == nullptr) return;
	glDeleteQueriesARB(n, ids);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
bool vw_IsQuery(unsigned int id)
{
	if (glIsQueryARB == nullptr) return false;
	return glIsQueryARB(id);
}


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_BeginQuery(unsigned int id)
{
	if (glBeginQueryARB == nullptr) return;
	glBeginQueryARB(GL_SAMPLES_PASSED_ARB, id);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_EndQuery()
{
	if (glEndQueryARB == nullptr) return;
	glEndQueryARB(GL_SAMPLES_PASSED_ARB);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GetQueryiv(unsigned int target, unsigned int pname, int *params)
{
	if (glGetQueryivARB == nullptr) return;
	glGetQueryivARB(target, pname, params);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GetQueryObjectiv(unsigned int id, unsigned int pname, int *params)
{
	if (glGetQueryObjectivARB == nullptr) return;
	glGetQueryObjectivARB(id, pname, params);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_GetQueryObjectuiv(unsigned int id, unsigned int pname, unsigned int *params)
{
	if (glGetQueryObjectuivARB == nullptr) return;
	glGetQueryObjectuivARB(id, pname, params);
}
