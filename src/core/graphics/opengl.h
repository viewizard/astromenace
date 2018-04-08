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

#ifndef GraphicsOpenGL_H
#define GraphicsOpenGL_H

#if defined(__APPLE__) && defined(__MACH__)
#define __glext_h_ // don't let gl.h include glext.h
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#undef __glext_h_
#else
#define __glext_h_ // don't let gl.h include glext.h
#include <GL/gl.h>
#include <GL/glu.h>
#undef __glext_h_
#endif

#include "glext.h" // provide glext.h version we need

// alias for OpenGL textures name (ID) in order to make code more clear
using GLtexture = GLuint;

#endif // GraphicsOpenGL_H
