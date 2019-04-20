/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE glBufferSubData (since OpenGL 1.5)
//      When replacing the entire data store, consider using glBufferSubData
//      rather than completely recreating the data store with glBufferData.
//      This avoids the cost of reallocating the data store.

// NOTE glBufferStorage() (since OpenGL 4.4)
//      creates and initializes a buffer object's immutable data store

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glNamedBufferStorage(), glNamedBufferSubData()

#include "graphics.h"
#include "extensions.h"

namespace viewizard {

/*
 * Build buffer object (size in bytes).
 */
bool vw_BuildBufferObject(eBufferObject target, GLsizeiptr size, const GLvoid *data,
			  GLuint &buffer, eBufferObjectUsage usage)
{
	// Don't check 'data' parameter, since nullptr is appropriate value for glBufferData().
	if (!data ||
	    !pfn_glGenBuffers ||
	    !pfn_glBindBuffer ||
	    !pfn_glBufferData ||
	    !pfn_glIsBuffer)
		return false;

	pfn_glGenBuffers(1, &buffer);
	pfn_glBindBuffer(static_cast<GLenum>(target), buffer);
	pfn_glBufferData(static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
	pfn_glBindBuffer(static_cast<GLenum>(target), 0); // disable buffer (bind buffer 0)

	if (!pfn_glIsBuffer(buffer))
		return false;

	return true;
}

/*
 * Bind buffer object.
 */
void vw_BindBufferObject(eBufferObject target, GLuint buffer)
{
	if (!pfn_glBindBuffer)
		return;

	pfn_glBindBuffer(static_cast<GLenum>(target), buffer);
}

/*
 * Delete buffer object.
 */
void vw_DeleteBufferObject(GLuint &buffer)
{
	if (!pfn_glIsBuffer ||
	    !pfn_glDeleteBuffers ||
	    !pfn_glIsBuffer(buffer))
		return;

	pfn_glDeleteBuffers(1, &buffer);
	buffer = 0;
}

} // viewizard namespace
