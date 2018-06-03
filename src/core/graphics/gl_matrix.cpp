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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE the matrix stack and related matrix manipulation functions deprecated in OpenGL 3.1 core profile
//      https://www.khronos.org/registry/OpenGL/specs/gl/glspec30.pdf
//      E.1. PROFILES AND DEPRECATED FEATURES OF OPENGL 3.0

/*
We don't check pointers status, since we don't work with pointers
but only provide them to OpenGL functions, let OpenGL check them.
*/

#include "../math/math.h"
#include "graphics.h"


/*
 * Replace the current matrix with the identity matrix.
 */
void vw_LoadIdentity()
{
	glLoadIdentity();
}

/*
 * Produce a translation by sVECTOR3D.
 */
void vw_Translate(sVECTOR3D Location)
{
	glTranslatef(Location.x, Location.y, Location.z);
}

/*
 * Produce a rotation of angle degrees around the vector x, y and z.
 */
void vw_Rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	glRotatef(angle, x, y, z);
}

/*
 * Produce a nonuniform scaling along the x, y, and z axes.
 */
void vw_Scale(GLfloat x, GLfloat y, GLfloat z)
{
	glScalef(x, y, z);
}

/*
 * Get matrix from the top of the matrix stack.
 */
void vw_GetMatrix(eMatrixPname pname, GLfloat *params)
{
	glGetFloatv(static_cast<GLenum>(pname), params);
}

/*
 * Replace the current matrix with an arbitrary matrix.
 */
void vw_SetMatrix(const GLfloat *matrix)
{
	glLoadMatrixf(matrix);
}

/*
 * Sets the current matrix mode.
 */
void vw_MatrixMode(eMatrixMode mode)
{
	glMatrixMode(static_cast<GLenum>(mode));
}

/*
 * Multiply the current matrix by an arbitrary matrix.
 */
void vw_MultMatrix(const GLfloat *matrix)
{
	glMultMatrixf(matrix);
}

/*
 * Push the current matrix stack.
 */
void vw_PushMatrix()
{
	glPushMatrix();
}

/*
 * Pop the current matrix stack.
 */
void vw_PopMatrix()
{
	glPopMatrix();
}
