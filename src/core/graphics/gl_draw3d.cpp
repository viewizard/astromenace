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

// TODO move from new/delete to std::unique_ptr

// NOTE GL_ARB_vertex_program (since OpenGL 2.0)
//      glVertexAttribPointer(), glEnableVertexAttribArray() + shader
//      could be used to replace gl*Pointer() + glEnableClientState()

// NOTE GL_EXT_draw_instanced (since OpenGL 3.1)
//      probably, we could render same type of objects with glDrawElementsInstanced()

// NOTE ARB_vertex_attrib_binding (since OpenGL 4.3)
//      specify the attribute format and the attribute data separately
//      glEnableVertexAttribArray(), glVertexAttribFormat(), glVertexAttribBinding()

// NOTE ARB_direct_state_access (since OpenGL 4.5)
//      we could avoid glBindBuffer() or glBindVertexArray() call during VAO creation by
//      glEnableVertexArrayAttrib(), glVertexArrayAttribFormat(), glVertexArrayAttribBinding(),
//      glVertexArrayVertexBuffer(), glVertexArrayVertexBuffer()

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

namespace {

// local index array count
unsigned int LocalIndexArrayCount{0};
// local index array
GLuint *LocalIndexArray{nullptr};
// local index buffer object
GLuint LocalIndexBO{0};

} // unnamed namespace


/*
 * Initialization.
 */
void __InitializationLocalIndexData()
{
	LocalIndexArrayCount = 0;
	LocalIndexArray = nullptr;
	LocalIndexBO = 0;
}

/*
 * Release memory.
 */
void __ReleaseLocalIndexData()
{
	if (LocalIndexArray) {
		delete [] LocalIndexArray;
		LocalIndexArray = nullptr;
	}
	LocalIndexArrayCount = 0;
	if (LocalIndexBO)
		vw_DeleteBufferObject(LocalIndexBO);
}

/*
 * Setup states and pointers.
 */
GLuint *__SendVertices_EnableStatesAndPointers(GLsizei count, int DataFormat, const GLvoid *VertexArray,
					       GLsizei stride, GLuint VertexBO, unsigned int RangeStart,
					       unsigned int *IndexArray, GLuint IndexBO)
{
	if (!VertexArray && !VertexBO)
		return nullptr;

	bool NeedVBO = __GetDevCaps().VBOSupported;
	if (!VertexBO)
		NeedVBO = false;

	uint8_t *tmpPointer{nullptr};

	if (NeedVBO)
		vw_BindBufferObject(eBufferObject::Vertex, VertexBO);
	else
		tmpPointer = (uint8_t*)VertexArray;

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

	// indices - fourth parameter for glDrawElements()
	GLuint *indices{nullptr};
	if (IndexBO || IndexArray) {
		// by default, work with IndexArray
		indices = IndexArray;
		// if IBO provided and supported, setup IBO
		if (IndexBO && __GetDevCaps().VBOSupported) {
			vw_BindBufferObject(eBufferObject::Index, IndexBO);
			indices = nullptr;
		}
		// care about RangeStart, since we could start from range
		indices = indices + RangeStart;

		return indices;
	}

	// re-create local index array, if need
	if (LocalIndexArrayCount < (unsigned int)(count + RangeStart)) {
		LocalIndexArrayCount = count + RangeStart;
		if (LocalIndexArray)
			delete [] LocalIndexArray;

		LocalIndexArray = new GLuint[LocalIndexArrayCount];
		for (unsigned int i = 0; i < LocalIndexArrayCount; i++) {
			LocalIndexArray[i] = i;
		}

		// if buffer objects supported, create IBO
		if (__GetDevCaps().VBOSupported) {
			if (LocalIndexBO)
				vw_DeleteBufferObject(LocalIndexBO);
			if (!vw_BuildBufferObject(eBufferObject::Index,
						  LocalIndexArrayCount * sizeof(unsigned), LocalIndexArray, LocalIndexBO))
				LocalIndexBO = 0;
		}
	}

	// by default, work with LocalIndexArray
	indices = LocalIndexArray;
	// if IBO provided and supported, setup IBO
	if (LocalIndexBO && __GetDevCaps().VBOSupported) {
		vw_BindBufferObject(eBufferObject::Index, LocalIndexBO);
		indices = nullptr;
	}
	// care about RangeStart, since we could start from range
	indices = indices + RangeStart;

	return indices;
}

/*
 * Disable states.
 */
void __SendVertices_DisableStatesAndPointers(int DataFormat)
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

	if (__GetDevCaps().VBOSupported) {
		vw_BindBufferObject(eBufferObject::Index, 0);
		vw_BindBufferObject(eBufferObject::Vertex, 0);
	}
}

/*
 * Send (draw) vertices.
 */
void vw_SendVertices(ePrimitiveType mode, GLsizei count, int DataFormat, const GLvoid *VertexArray,
		     GLsizei Stride, GLuint VertexBO, unsigned int RangeStart,
		     unsigned int *IndexArray, GLuint IndexBO, GLuint VAO)
{
	if (!VertexArray && !VertexBO)
		return;

	// in case of VAO, we don't need provide indices, VAO will care about pointers
	GLuint *indices{nullptr};
	if (VAO && __GetDevCaps().VAOSupported)
		vw_BindVAO(VAO);
	else
		indices = __SendVertices_EnableStatesAndPointers(count, DataFormat, VertexArray, Stride,
								 VertexBO, RangeStart, IndexArray, IndexBO);

	glDrawElements(static_cast<GLenum>(mode), count, GL_UNSIGNED_INT, indices);

	if (VAO && __GetDevCaps().VAOSupported)
		vw_BindVAO(0);
	else
		__SendVertices_DisableStatesAndPointers(DataFormat);
}
