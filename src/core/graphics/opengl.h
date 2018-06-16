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

// NOTE glu.h should be removed after OpenGL 3.1 core profile switch,
//      all glu functionality should be replaced:
//      gluLookAt(), gluBuild2DMipmaps() and gluPerspective()

#ifndef CORE_GRAPHICS_OPENGL_H
#define CORE_GRAPHICS_OPENGL_H

#if defined(__APPLE__) && defined(__MACH__)
	#define GL_GLEXT_LEGACY
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#define __glext_h_ // don't let gl.h include glext.h
	#include <GL/gl.h>
	#include <GL/glu.h>
	#undef __glext_h_
#endif

// GL version can be defined by system gl.h header
// undef them, since we need function's typedefs
#undef GL_VERSION_1_3
#undef GL_VERSION_1_4
#undef GL_VERSION_1_5
#undef GL_VERSION_2_0
#undef GL_VERSION_2_1
#undef GL_VERSION_3_0
#undef GL_VERSION_3_1
#undef GL_VERSION_3_2
#undef GL_VERSION_3_3
#undef GL_VERSION_4_0
#undef GL_VERSION_4_1
#undef GL_VERSION_4_2
#undef GL_VERSION_4_3
#undef GL_VERSION_4_4
#undef GL_VERSION_4_5
#undef GL_VERSION_4_6

#include "glext.h" // provide glext.h version we need

// alias for OpenGL variable types, in order to make code more clear
using GLtexture = GLuint;
using GLrenderbuffer = GLuint;
using GLframebuffer = GLuint;

#endif // CORE_GRAPHICS_OPENGL_H
