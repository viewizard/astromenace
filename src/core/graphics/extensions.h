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

#ifndef GraphicsExtensions_H
#define GraphicsExtensions_H

#include "opengl.h"

// GL_ARB_shader_objects
extern PFNGLDELETEOBJECTARBPROC _glDeleteObject;
extern PFNGLGETHANDLEARBPROC _glGetHandle;
extern PFNGLDETACHOBJECTARBPROC _glDetachObject;
extern PFNGLCREATESHADEROBJECTARBPROC _glCreateShaderObject;
extern PFNGLSHADERSOURCEARBPROC _glShaderSource;
extern PFNGLCOMPILESHADERARBPROC _glCompileShader;
extern PFNGLCREATEPROGRAMOBJECTARBPROC _glCreateProgramObject;
extern PFNGLATTACHOBJECTARBPROC _glAttachObject;
extern PFNGLLINKPROGRAMARBPROC _glLinkProgram;
extern PFNGLUSEPROGRAMOBJECTARBPROC _glUseProgramObject;
extern PFNGLVALIDATEPROGRAMARBPROC _glValidateProgram;
extern PFNGLUNIFORM1FARBPROC _glUniform1f;
extern PFNGLUNIFORM2FARBPROC _glUniform2f;
extern PFNGLUNIFORM3FARBPROC _glUniform3f;
extern PFNGLUNIFORM4FARBPROC _glUniform4f;
extern PFNGLUNIFORM1IARBPROC _glUniform1i;
extern PFNGLUNIFORM2IARBPROC _glUniform2i;
extern PFNGLUNIFORM3IARBPROC _glUniform3i;
extern PFNGLUNIFORM4IARBPROC _glUniform4i;
extern PFNGLUNIFORM1FVARBPROC _glUniform1fv;
extern PFNGLUNIFORM2FVARBPROC _glUniform2fv;
extern PFNGLUNIFORM3FVARBPROC _glUniform3fv;
extern PFNGLUNIFORM4FVARBPROC _glUniform4fv;
extern PFNGLUNIFORM1IVARBPROC _glUniform1iv;
extern PFNGLUNIFORM2IVARBPROC _glUniform2iv;
extern PFNGLUNIFORM3IVARBPROC _glUniform3iv;
extern PFNGLUNIFORM4IVARBPROC _glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVARBPROC _glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVARBPROC _glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVARBPROC _glUniformMatrix4fv;
extern PFNGLGETOBJECTPARAMETERFVARBPROC _glGetObjectParameterfv;
extern PFNGLGETOBJECTPARAMETERIVARBPROC _glGetObjectParameteriv;
extern PFNGLGETINFOLOGARBPROC _glGetInfoLog;
extern PFNGLGETATTACHEDOBJECTSARBPROC _glGetAttachedObjects;
extern PFNGLGETUNIFORMLOCATIONARBPROC _glGetUniformLocation;
extern PFNGLGETACTIVEUNIFORMARBPROC _glGetActiveUniform;
extern PFNGLGETUNIFORMFVARBPROC _glGetUniformfv;
extern PFNGLGETUNIFORMIVARBPROC _glGetUniformiv;
extern PFNGLGETSHADERSOURCEARBPROC _glGetShaderSource;

// GL_EXT_framebuffer_object
extern PFNGLGENRENDERBUFFERSEXTPROC _glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFEREXTPROC _glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC _glRenderbufferStorageMultisample;
extern PFNGLGENRENDERBUFFERSEXTPROC _glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFEREXTPROC _glBindFramebuffer;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC _glFramebufferRenderbuffer;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC _glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC _glFramebufferTexture2D;
extern PFNGLDELETERENDERBUFFERSEXTPROC _glDeleteRenderbuffers;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC _glDeleteFramebuffers;
extern PFNGLBLITFRAMEBUFFEREXTPROC _glBlitFramebuffer;
extern PFNGLISFRAMEBUFFEREXTPROC _glIsFramebuffer;
extern PFNGLGENERATEMIPMAPPROC _glGenerateMipmap;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC _glGetFramebufferAttachmentParameteriv;

// GL_NV_framebuffer_multisample_coverage
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC _glRenderbufferStorageMultisampleCoverageNV;


bool __Initialize_GL_ARB_shader_objects();
bool __Initialize_GL_EXT_framebuffer_object();
bool __Initialize_GL_NV_framebuffer_multisample_coverage();

#endif // GraphicsExtensions_H
