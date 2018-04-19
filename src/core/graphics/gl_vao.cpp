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

// NOTE ARB_vertex_attrib_binding (since OpenGL 4.3)
//      specify the attribute format and the attribute data separately

// NOTE ARB_direct_state_access (since OpenGL 4.5)
//      no longer need to call glBindBuffer() or glBindVertexArray()

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

/*
 * Build vertex array object.
 */
bool vw_BuildVAO(GLuint &VAO, int NumVertices, int DataFormat, void *VertexArray, int Stride, GLuint VertexBO,
		 unsigned int RangeStart, unsigned int *IndexArray, GLuint IndexBO)
{
	if (!_glGenVertexArrays ||
	    !_glIsVertexArray)
		return false;

	_glGenVertexArrays(1, &VAO);

	vw_BindVAO(VAO);
	__SendVertices_EnableStatesAndPointers(NumVertices, DataFormat, VertexArray, Stride, VertexBO,
					       RangeStart, IndexArray, IndexBO);

	vw_BindVAO(0);
	__SendVertices_DisableStatesAndPointers(DataFormat, VertexBO, 0);

	if (!_glIsVertexArray(VAO))
		return false;

	return true;
}

/*
 * Bind vertex array object.
 */
void vw_BindVAO(GLuint VAO)
{
	// don't call glIsVertexArray() here, for best speed
	if (!_glBindVertexArray)
		return;

	_glBindVertexArray(VAO);
}

/*
 * Delete vertex array object.
 */
void vw_DeleteVAO(GLuint &VAO)
{
	if (!_glIsVertexArray ||
	    !_glDeleteVertexArrays ||
	    !_glIsVertexArray(VAO))
		return;

	_glDeleteVertexArrays(1, &VAO);
	VAO = 0;
}
