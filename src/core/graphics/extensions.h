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

/*
Should be for graphic's internal use only.

Rules for functions and variables naming:
"__" prefix for functions;
"_" prefix for variables.
*/

#ifndef GraphicsExtensions_H
#define GraphicsExtensions_H

#include "opengl.h"

// OpenGL 2.0 (only what we need or would need in future)
extern PFNGLATTACHSHADERPROC _glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC _glCompileShader;
extern PFNGLCREATEPROGRAMPROC _glCreateProgram;
extern PFNGLCREATESHADERPROC _glCreateShader;
extern PFNGLDELETEPROGRAMPROC _glDeleteProgram;
extern PFNGLDELETESHADERPROC _glDeleteShader;
extern PFNGLDETACHSHADERPROC _glDetachShader;
extern PFNGLGETACTIVEATTRIBPROC _glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC _glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC _glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC _glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC _glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC _glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC _glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC _glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC _glGetUniformiv;
extern PFNGLISPROGRAMPROC _glIsProgram;
extern PFNGLISSHADERPROC _glIsShader;
extern PFNGLLINKPROGRAMPROC _glLinkProgram;
extern PFNGLSHADERSOURCEPROC _glShaderSource;
extern PFNGLUSEPROGRAMPROC _glUseProgram;
extern PFNGLUNIFORM1FPROC _glUniform1f;
extern PFNGLUNIFORM2FPROC _glUniform2f;
extern PFNGLUNIFORM3FPROC _glUniform3f;
extern PFNGLUNIFORM4FPROC _glUniform4f;
extern PFNGLUNIFORM1IPROC _glUniform1i;
extern PFNGLUNIFORM2IPROC _glUniform2i;
extern PFNGLUNIFORM3IPROC _glUniform3i;
extern PFNGLUNIFORM4IPROC _glUniform4i;
extern PFNGLUNIFORM1FVPROC _glUniform1fv;
extern PFNGLUNIFORM2FVPROC _glUniform2fv;
extern PFNGLUNIFORM3FVPROC _glUniform3fv;
extern PFNGLUNIFORM4FVPROC _glUniform4fv;
extern PFNGLUNIFORM1IVPROC _glUniform1iv;
extern PFNGLUNIFORM2IVPROC _glUniform2iv;
extern PFNGLUNIFORM3IVPROC _glUniform3iv;
extern PFNGLUNIFORM4IVPROC _glUniform4iv;
extern PFNGLVALIDATEPROGRAMPROC _glValidateProgram;

// OpenGL 3.0 (only what we need or would need in future)
extern PFNGLBINDRENDERBUFFERPROC _glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC _glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC _glGenRenderbuffers;
extern PFNGLISFRAMEBUFFERPROC _glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC _glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC _glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC _glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC _glRenderbufferStorageMultisample;

// GL_NV_framebuffer_multisample_coverage
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC _glRenderbufferStorageMultisampleCoverageNV;


bool __Initialize_OpenGL_2_0();
bool __Initialize_OpenGL_3_0();
bool __Initialize_GL_NV_framebuffer_multisample_coverage();

#endif // GraphicsExtensions_H
