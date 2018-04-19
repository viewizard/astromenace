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
#include "extensions.h"


/*
 * Build vertex buffer object.
 */
bool vw_BuildVertexBufferObject(int NumVertices, void *Data, int Stride, unsigned int &Buffer)
{
	if (!Data ||
	    !_glGenBuffers ||
	    !_glBindBuffer ||
	    !_glBufferData ||
	    !_glIsBuffer)
		return false;

	_glGenBuffers(1, &Buffer);
	_glBindBuffer(GL_ARRAY_BUFFER, Buffer);
	_glBufferData(GL_ARRAY_BUFFER, NumVertices * Stride * sizeof(float), Data, GL_STATIC_DRAW);
	_glBindBuffer(GL_ARRAY_BUFFER, 0); // disable buffer (bind buffer 0)

	if (!_glIsBuffer(Buffer))
		return false;

	return true;
}

/*
 * Build index buffer object.
 */
bool vw_BuildIndexBufferObject(int NumIndex, void *Data, unsigned int &Buffer)
{
	if (!Data ||
	    !_glGenBuffers ||
	    !_glBindBuffer ||
	    !_glBufferData ||
	    !_glIsBuffer)
		return false;

	_glGenBuffers(1, &Buffer);
	_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
	_glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndex * sizeof(unsigned int), Data, GL_STATIC_DRAW);
	_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // disable buffer (bind buffer 0)

	if (!_glIsBuffer(Buffer))
		return false;

	return true;
}

/*
 * Bind buffer object.
 */
void vw_BindBufferObject(int target, unsigned int Buffer)
{
	if (!_glBindBuffer)
		return;

	switch (target) {
	case RI_ARRAY_BUFFER:
		_glBindBuffer(GL_ARRAY_BUFFER, Buffer);
		break;
	case RI_ELEMENT_ARRAY_BUFFER:
		_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
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
	if (!_glIsBuffer ||
	    !_glDeleteBuffers ||
	    !_glIsBuffer(Buffer))
		return;

	_glDeleteBuffers(1, &Buffer);
	Buffer = 0;
}
