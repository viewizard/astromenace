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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

/*
Should be for graphic's internal use only.

Rules for functions and variables naming:
"__" prefix for functions;
"_" prefix for variables.
*/

#ifndef CORE_GRAPHICS_EXTENSION_H
#define CORE_GRAPHICS_EXTENSION_H

#include "opengl.h"

namespace viewizard {

// OpenGL 1.3 (only what we need or would need in future)
extern PFNGLACTIVETEXTUREPROC pfn_glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC pfn_glClientActiveTexture;

// OpenGL 1.5 (only what we need or would need in future)
extern PFNGLBINDBUFFERPROC pfn_glBindBuffer;
extern PFNGLDELETEBUFFERSPROC pfn_glDeleteBuffers;
extern PFNGLGENBUFFERSPROC pfn_glGenBuffers;
extern PFNGLISBUFFERPROC pfn_glIsBuffer;
extern PFNGLBUFFERDATAPROC pfn_glBufferData;

// OpenGL 2.0 (only what we need or would need in future)
extern PFNGLATTACHSHADERPROC pfn_glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC pfn_glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC pfn_glCompileShader;
extern PFNGLCREATEPROGRAMPROC pfn_glCreateProgram;
extern PFNGLCREATESHADERPROC pfn_glCreateShader;
extern PFNGLDELETEPROGRAMPROC pfn_glDeleteProgram;
extern PFNGLDELETESHADERPROC pfn_glDeleteShader;
extern PFNGLDETACHSHADERPROC pfn_glDetachShader;
extern PFNGLGETACTIVEATTRIBPROC pfn_glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC pfn_glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC pfn_glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC pfn_glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC pfn_glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC pfn_glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC pfn_glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC pfn_glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC pfn_glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC pfn_glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC pfn_glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC pfn_glGetUniformiv;
extern PFNGLISPROGRAMPROC pfn_glIsProgram;
extern PFNGLISSHADERPROC pfn_glIsShader;
extern PFNGLLINKPROGRAMPROC pfn_glLinkProgram;
extern PFNGLSHADERSOURCEPROC pfn_glShaderSource;
extern PFNGLUSEPROGRAMPROC pfn_glUseProgram;
extern PFNGLUNIFORM1FPROC pfn_glUniform1f;
extern PFNGLUNIFORM2FPROC pfn_glUniform2f;
extern PFNGLUNIFORM3FPROC pfn_glUniform3f;
extern PFNGLUNIFORM4FPROC pfn_glUniform4f;
extern PFNGLUNIFORM1IPROC pfn_glUniform1i;
extern PFNGLUNIFORM2IPROC pfn_glUniform2i;
extern PFNGLUNIFORM3IPROC pfn_glUniform3i;
extern PFNGLUNIFORM4IPROC pfn_glUniform4i;
extern PFNGLUNIFORM1FVPROC pfn_glUniform1fv;
extern PFNGLUNIFORM2FVPROC pfn_glUniform2fv;
extern PFNGLUNIFORM3FVPROC pfn_glUniform3fv;
extern PFNGLUNIFORM4FVPROC pfn_glUniform4fv;
extern PFNGLUNIFORM1IVPROC pfn_glUniform1iv;
extern PFNGLUNIFORM2IVPROC pfn_glUniform2iv;
extern PFNGLUNIFORM3IVPROC pfn_glUniform3iv;
extern PFNGLUNIFORM4IVPROC pfn_glUniform4iv;
extern PFNGLVALIDATEPROGRAMPROC pfn_glValidateProgram;

// OpenGL 2.1 (only what we need or would need in future)
extern PFNGLUNIFORMMATRIX2X3FVPROC pfn_glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC pfn_glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC pfn_glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC pfn_glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC pfn_glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC pfn_glUniformMatrix4x3fv;

// OpenGL 3.0 (only what we need or would need in future)
extern PFNGLBINDRENDERBUFFERPROC pfn_glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC pfn_glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC pfn_glGenRenderbuffers;
extern PFNGLISFRAMEBUFFERPROC pfn_glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC pfn_glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC pfn_glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC pfn_glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC pfn_glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC pfn_glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC pfn_glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC pfn_glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC pfn_glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC pfn_glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC pfn_glRenderbufferStorageMultisample;
extern PFNGLBINDVERTEXARRAYPROC pfn_glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC pfn_glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC pfn_glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC pfn_glIsVertexArray;

// OpenGL 4.2 (only what we need or would need in future)
extern PFNGLTEXSTORAGE2DPROC pfn_glTexStorage2D;

// GL_NV_framebuffer_multisample_coverage
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC pfn_glRenderbufferStorageMultisampleCoverageNV;


bool Initialize_OpenGL_1_3();
bool Initialize_OpenGL_1_5();
bool Initialize_OpenGL_2_0();
bool Initialize_OpenGL_2_1();
bool Initialize_OpenGL_3_0();
bool Initialize_OpenGL_4_2();
bool Initialize_GL_NV_framebuffer_multisample_coverage();

} // viewizard namespace

#endif // CORE_GRAPHICS_EXTENSION_H
