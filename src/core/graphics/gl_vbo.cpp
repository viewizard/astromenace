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

PFNGLGENBUFFERSARBPROC glGenBuffersARB = nullptr;
PFNGLBINDBUFFERARBPROC glBindBufferARB = nullptr;
PFNGLBUFFERDATAARBPROC glBufferDataARB = nullptr;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = nullptr;
PFNGLISBUFFERARBPROC glIsBufferARB = nullptr;

/*
 * Internal initialization for buffer objects.
 */
bool Internal_InitializationBufferObjects()
{
	// get pointers to the GL functions
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");
	glIsBufferARB = (PFNGLISBUFFERARBPROC) SDL_GL_GetProcAddress("glIsBufferARB");

	if (!glGenBuffersARB || !glBindBufferARB || !glBufferDataARB ||
	    !glDeleteBuffersARB || !glIsBufferARB) {
		glGenBuffersARB = nullptr;
		glBindBufferARB = nullptr;
		glBufferDataARB = nullptr;
		glDeleteBuffersARB = nullptr;
		glIsBufferARB = nullptr;

		return false;
	}

	return true;
}

/*
 * Build vertex buffer object.
 */
bool vw_BuildVertexBufferObject(int NumVertices, void *Data, int Stride, unsigned int &Buffer)
{
	if (!Data || !glGenBuffersARB || !glBindBufferARB ||
	    !glBufferDataARB || !glIsBufferARB)
		return false;

	glGenBuffersARB(1, &Buffer);			// get a valid name
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, Buffer);	// bind the buffer
	// load the data
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, NumVertices * Stride * sizeof(float), Data, GL_STATIC_DRAW_ARB);
	// disable buffer (bind buffer 0)
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	if (!glIsBufferARB(Buffer))
		return false;

	return true;
}

/*
 * Build index buffer object.
 */
bool vw_BuildIndexBufferObject(int NumIndex, void *Data, unsigned int &Buffer)
{
	if (!Data || !glGenBuffersARB || !glBindBufferARB ||
	    !glBufferDataARB || !glIsBufferARB)
		return false;

	glGenBuffersARB(1, &Buffer);				// get a valid name
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, Buffer);	// bind the buffer
	// load the data
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, NumIndex * sizeof(unsigned int), Data, GL_STATIC_DRAW_ARB);
	// disable buffer (bind buffer 0)
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	if (!glIsBufferARB(Buffer))
		return false;

	return true;
}

/*
 * Bind buffer object.
 */
void vw_BindBufferObject(int target, unsigned int Buffer)
{
	if (!glBindBufferARB)
		return;

	switch (target) {
	case RI_ARRAY_BUFFER:
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, Buffer);
		break;
	case RI_ELEMENT_ARRAY_BUFFER:
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, Buffer);
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong target.\n";
		break;
	}
}

/*
 * Delete buffer object.
 */
void vw_DeleteBufferObject(unsigned int &Buffer)
{
	if (!glIsBufferARB || !glDeleteBuffersARB || !glIsBufferARB(Buffer))
		return;

	glDeleteBuffersARB(1, &Buffer);
	Buffer = 0;
}
