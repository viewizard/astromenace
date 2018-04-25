/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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

// NOTE GL_ARB_vertex_program (since OpenGL 2.0)
//      glVertexAttribPointer(), glEnableVertexAttribArray(), glDisableVertexAttribArray()
//      could be used to replace gl*Pointer() + glEnableClientState()

// NOTE GL_EXT_draw_instanced (since OpenGL 3.1)
//      probably, we could render same type of objects with glDrawElementsInstanced()

// NOTE ARB_vertex_attrib_binding (since OpenGL 4.3)
//      specify the attribute format and the attribute data separately
//      glEnableVertexAttribArray(), glVertexAttribFormat(), glVertexAttribBinding(),
//      glDisableVertexAttribArray()

// NOTE ARB_direct_state_access (since OpenGL 4.5)
//      we could avoid glBindBuffer() or glBindVertexArray() call during VAO creation by
//      glEnableVertexArrayAttrib(), glVertexArrayAttribFormat(), glVertexArrayAttribBinding(),
//      glVertexArrayVertexBuffer(), glVertexArrayVertexBuffer(), glDisableVertexArrayAttrib()

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

/*
 * Setup states and pointers.
 * Also used for VAO generation.
 */
void __Draw3D_EnableStates(int DataFormat, GLvoid *VertexArray,
			   GLsizei stride, GLuint VertexBO, GLuint IndexBO)
{
	if (!VertexArray && !VertexBO)
		return;

	uint8_t *tmpPointer{nullptr};

	// VBO (GL_ARRAY_BUFFER) binding affects VAO state indirectly,
	// as the current GL_ARRAY_BUFFER binding is saved in VAO state
	// at the time of the gl***Pointer() call
	if (VertexBO && __GetDevCaps().VBOSupported)
		vw_BindBufferObject(eBufferObject::Vertex, VertexBO);
	else
		tmpPointer = static_cast<uint8_t *>(VertexArray);

	// IBO (GL_ELEMENT_ARRAY_BUFFER) binding is part of the VAO state
	if (IndexBO && __GetDevCaps().VBOSupported)
		vw_BindBufferObject(eBufferObject::Index, IndexBO);

	if ((DataFormat & RI_COORD) == RI_3f_XYZ) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, stride, tmpPointer);
		tmpPointer += 3 * sizeof(GLfloat);
	}

	if ((DataFormat & RI_COORD) == RI_2f_XY) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, stride, tmpPointer);
		tmpPointer += 2 * sizeof(GLfloat);
	}

	if ((DataFormat & RI_NORMAL) == RI_3f_NORMAL) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, stride, tmpPointer);
		tmpPointer += 3 * sizeof(GLfloat);
	}

	if ((DataFormat & RI_COLOR) == RI_4f_COLOR) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, stride, tmpPointer);
		tmpPointer += 4 * sizeof(GLfloat);
	}

	int TexturesCount = DataFormat & RI_TEX_COUNT;
	if (TexturesCount > 0) {
		for (int i = 0; i < TexturesCount; i++) {
			if (_glClientActiveTexture)
				_glClientActiveTexture(GL_TEXTURE0 + i);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, stride, tmpPointer);
			if ((DataFormat & RI_TEX_COORD_TYPE) == RI_SEPARATE_TEX_COORD)
				tmpPointer += 2 * sizeof(GLfloat);
		}
	}
}

/*
 * Disable states.
 */
void __Draw3D_DisableStates(int DataFormat, GLuint VertexBO, GLuint IndexBO)
{
	if (DataFormat & RI_NORMAL)
		glDisableClientState(GL_NORMAL_ARRAY);
	if (DataFormat & RI_COLOR)
		glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	int TexturesCount = DataFormat & RI_TEX_COUNT;
	for (int i = 0; i < TexturesCount; i++) {
		if (_glClientActiveTexture)
			_glClientActiveTexture(GL_TEXTURE0 + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (VertexBO && __GetDevCaps().VBOSupported)
		vw_BindBufferObject(eBufferObject::Vertex, 0);
	if (IndexBO && __GetDevCaps().VBOSupported)
		vw_BindBufferObject(eBufferObject::Index, 0);
}

/*
 * Draw 3D primitives.
 */
void vw_Draw3D(ePrimitiveType mode, GLsizei count, int DataFormat, GLvoid *VertexArray,
	       GLsizei Stride, GLuint VertexBO, unsigned int RangeStart,
	       unsigned int *IndexArray, GLuint IndexBO, GLuint VAO)
{
	if (!VertexArray && !VertexBO)
		return;

	if (VAO && __GetDevCaps().VAOSupported)
		vw_BindVAO(VAO);
	else
		__Draw3D_EnableStates(DataFormat, VertexArray, Stride, VertexBO, IndexBO);

	if (IndexArray || IndexBO) {
		// all IBO binding related code moved to __Draw3D_EnableStates(),
		// we should care only about index array pointer here
		GLuint *indices{nullptr};
		if (!IndexBO || !__GetDevCaps().VBOSupported)
			indices = IndexArray;
		glDrawElements(static_cast<GLenum>(mode), count, GL_UNSIGNED_INT, indices + RangeStart);
	} else
		glDrawArrays(static_cast<GLenum>(mode), RangeStart, count);

	if (VAO && __GetDevCaps().VAOSupported)
		vw_BindVAO(0);
	else
		__Draw3D_DisableStates(DataFormat, VertexBO, IndexBO);
}
