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

#include "../base.h"
#include "opengl.h"

// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC _glDeleteObject{nullptr};
PFNGLGETHANDLEARBPROC _glGetHandle{nullptr};
PFNGLDETACHOBJECTARBPROC _glDetachObject{nullptr};
PFNGLCREATESHADEROBJECTARBPROC _glCreateShaderObject{nullptr};
PFNGLSHADERSOURCEARBPROC _glShaderSource{nullptr};
PFNGLCOMPILESHADERARBPROC _glCompileShader{nullptr};
PFNGLCREATEPROGRAMOBJECTARBPROC _glCreateProgramObject{nullptr};
PFNGLATTACHOBJECTARBPROC _glAttachObject{nullptr};
PFNGLLINKPROGRAMARBPROC _glLinkProgram{nullptr};
PFNGLUSEPROGRAMOBJECTARBPROC _glUseProgramObject{nullptr};
PFNGLVALIDATEPROGRAMARBPROC _glValidateProgram{nullptr};
PFNGLUNIFORM1FARBPROC _glUniform1f{nullptr};
PFNGLUNIFORM2FARBPROC _glUniform2f{nullptr};
PFNGLUNIFORM3FARBPROC _glUniform3f{nullptr};
PFNGLUNIFORM4FARBPROC _glUniform4f{nullptr};
PFNGLUNIFORM1IARBPROC _glUniform1i{nullptr};
PFNGLUNIFORM2IARBPROC _glUniform2i{nullptr};
PFNGLUNIFORM3IARBPROC _glUniform3i{nullptr};
PFNGLUNIFORM4IARBPROC _glUniform4i{nullptr};
PFNGLUNIFORM1FVARBPROC _glUniform1fv{nullptr};
PFNGLUNIFORM2FVARBPROC _glUniform2fv{nullptr};
PFNGLUNIFORM3FVARBPROC _glUniform3fv{nullptr};
PFNGLUNIFORM4FVARBPROC _glUniform4fv{nullptr};
PFNGLUNIFORM1IVARBPROC _glUniform1iv{nullptr};
PFNGLUNIFORM2IVARBPROC _glUniform2iv{nullptr};
PFNGLUNIFORM3IVARBPROC _glUniform3iv{nullptr};
PFNGLUNIFORM4IVARBPROC _glUniform4iv{nullptr};
PFNGLUNIFORMMATRIX2FVARBPROC _glUniformMatrix2fv{nullptr};
PFNGLUNIFORMMATRIX3FVARBPROC _glUniformMatrix3fv{nullptr};
PFNGLUNIFORMMATRIX4FVARBPROC _glUniformMatrix4fv{nullptr};
PFNGLGETOBJECTPARAMETERFVARBPROC _glGetObjectParameterfv{nullptr};
PFNGLGETOBJECTPARAMETERIVARBPROC _glGetObjectParameteriv{nullptr};
PFNGLGETINFOLOGARBPROC _glGetInfoLog{nullptr};
PFNGLGETATTACHEDOBJECTSARBPROC _glGetAttachedObjects{nullptr};
PFNGLGETUNIFORMLOCATIONARBPROC _glGetUniformLocation{nullptr};
PFNGLGETACTIVEUNIFORMARBPROC _glGetActiveUniform{nullptr};
PFNGLGETUNIFORMFVARBPROC _glGetUniformfv{nullptr};
PFNGLGETUNIFORMIVARBPROC _glGetUniformiv{nullptr};
PFNGLGETSHADERSOURCEARBPROC _glGetShaderSource{nullptr};

// GL_EXT_framebuffer_object
PFNGLGENRENDERBUFFERSEXTPROC _glGenRenderbuffers{nullptr};
PFNGLBINDRENDERBUFFEREXTPROC _glBindRenderbuffer{nullptr};
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC _glRenderbufferStorageMultisample{nullptr};
PFNGLGENRENDERBUFFERSEXTPROC _glGenFramebuffers{nullptr};
PFNGLBINDFRAMEBUFFEREXTPROC _glBindFramebuffer{nullptr};
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC _glFramebufferRenderbuffer{nullptr};
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC _glCheckFramebufferStatus{nullptr};
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC _glFramebufferTexture2D{nullptr};
PFNGLDELETERENDERBUFFERSEXTPROC _glDeleteRenderbuffers{nullptr};
PFNGLDELETEFRAMEBUFFERSEXTPROC _glDeleteFramebuffers{nullptr};
PFNGLBLITFRAMEBUFFEREXTPROC _glBlitFramebuffer{nullptr};
PFNGLISFRAMEBUFFEREXTPROC _glIsFramebuffer{nullptr};
PFNGLGENERATEMIPMAPPROC _glGenerateMipmap{nullptr};
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC _glGetFramebufferAttachmentParameteriv{nullptr};

// GL_NV_framebuffer_multisample_coverage
PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC _glRenderbufferStorageMultisampleCoverageNV{nullptr};


/*
 * GL_ARB_shader_objects initialization.
 */
bool __Initialize_GL_ARB_shader_objects()
{
	_glDeleteObject = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
	_glGetHandle = (PFNGLGETHANDLEARBPROC) SDL_GL_GetProcAddress("glGetHandleARB");
	_glDetachObject = (PFNGLDETACHOBJECTARBPROC) SDL_GL_GetProcAddress("glDetachObjectARB");
	_glCreateShaderObject = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
	_glShaderSource = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
	_glCompileShader = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
	_glCreateProgramObject = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
	_glAttachObject = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
	_glLinkProgram = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
	_glUseProgramObject = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
	_glValidateProgram = (PFNGLVALIDATEPROGRAMARBPROC) SDL_GL_GetProcAddress("glValidateProgramARB");
	_glUniform1f = (PFNGLUNIFORM1FARBPROC) SDL_GL_GetProcAddress("glUniform1fARB");
	_glUniform2f = (PFNGLUNIFORM2FARBPROC) SDL_GL_GetProcAddress("glUniform2fARB");
	_glUniform3f = (PFNGLUNIFORM3FARBPROC) SDL_GL_GetProcAddress("glUniform3fARB");
	_glUniform4f = (PFNGLUNIFORM4FARBPROC) SDL_GL_GetProcAddress("glUniform4fARB");
	_glUniform1i = (PFNGLUNIFORM1IARBPROC) SDL_GL_GetProcAddress("glUniform1iARB");
	_glUniform2i = (PFNGLUNIFORM2IARBPROC) SDL_GL_GetProcAddress("glUniform2iARB");
	_glUniform3i = (PFNGLUNIFORM3IARBPROC) SDL_GL_GetProcAddress("glUniform3iARB");
	_glUniform4i = (PFNGLUNIFORM4IARBPROC) SDL_GL_GetProcAddress("glUniform4iARB");
	_glUniform1fv = (PFNGLUNIFORM1FVARBPROC) SDL_GL_GetProcAddress("glUniform1fvARB");
	_glUniform2fv = (PFNGLUNIFORM2FVARBPROC) SDL_GL_GetProcAddress("glUniform2fvARB");
	_glUniform3fv = (PFNGLUNIFORM3FVARBPROC) SDL_GL_GetProcAddress("glUniform3fvARB");
	_glUniform4fv = (PFNGLUNIFORM4FVARBPROC) SDL_GL_GetProcAddress("glUniform4fvARB");
	_glUniform1iv = (PFNGLUNIFORM1IVARBPROC) SDL_GL_GetProcAddress("glUniform1ivARB");
	_glUniform2iv = (PFNGLUNIFORM2IVARBPROC) SDL_GL_GetProcAddress("glUniform2ivARB");
	_glUniform3iv = (PFNGLUNIFORM3IVARBPROC) SDL_GL_GetProcAddress("glUniform3ivARB");
	_glUniform4iv = (PFNGLUNIFORM4IVARBPROC) SDL_GL_GetProcAddress("glUniform4ivARB");
	_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVARBPROC) SDL_GL_GetProcAddress("glUniformMatrix2fvARB");
	_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVARBPROC) SDL_GL_GetProcAddress("glUniformMatrix3fvARB");
	_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVARBPROC) SDL_GL_GetProcAddress("glUniformMatrix4fvARB");
	_glGetObjectParameterfv = (PFNGLGETOBJECTPARAMETERFVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterfvARB");
	_glGetObjectParameteriv = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
	_glGetInfoLog = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
	_glGetAttachedObjects = (PFNGLGETATTACHEDOBJECTSARBPROC) SDL_GL_GetProcAddress("glGetAttachedObjectsARB");
	_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
	_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMARBPROC) SDL_GL_GetProcAddress("glGetActiveUniformARB");
	_glGetUniformfv = (PFNGLGETUNIFORMFVARBPROC) SDL_GL_GetProcAddress("glGetUniformfvARB");
	_glGetUniformiv = (PFNGLGETUNIFORMIVARBPROC) SDL_GL_GetProcAddress("glGetUniformivARB");
	_glGetShaderSource = (PFNGLGETSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glGetShaderSourceARB");

	if (!_glDeleteObject || !_glGetHandle || !_glDetachObject || !_glCreateShaderObject ||
	    !_glShaderSource || !_glCompileShader || !_glCreateProgramObject || !_glAttachObject ||
	    !_glLinkProgram || !_glUseProgramObject || !_glValidateProgram || !_glUniform1f ||
	    !_glUniform2f || !_glUniform3f || !_glUniform4f || !_glUniform1i ||
	    !_glUniform2i || !_glUniform3i || !_glUniform4i || !_glUniform1fv ||
	    !_glUniform2fv || !_glUniform3fv || !_glUniform4fv || !_glUniform1iv ||
	    !_glUniform2iv || !_glUniform3iv || !_glUniform4iv || !_glUniformMatrix2fv ||
	    !_glUniformMatrix3fv || !_glUniformMatrix4fv || !_glGetObjectParameterfv ||
	    !_glGetObjectParameteriv || !_glGetInfoLog || !_glGetAttachedObjects ||
	    !_glGetUniformLocation || !_glGetActiveUniform || !_glGetUniformfv || !_glGetUniformiv ||
	    !_glGetShaderSource) {
		_glDeleteObject = nullptr;
		_glGetHandle = nullptr;
		_glDetachObject = nullptr;
		_glCreateShaderObject = nullptr;
		_glShaderSource = nullptr;
		_glCompileShader = nullptr;
		_glCreateProgramObject = nullptr;
		_glAttachObject = nullptr;
		_glLinkProgram = nullptr;
		_glUseProgramObject = nullptr;
		_glValidateProgram = nullptr;
		_glUniform1f = nullptr;
		_glUniform2f = nullptr;
		_glUniform3f = nullptr;
		_glUniform4f = nullptr;
		_glUniform1i = nullptr;
		_glUniform2i = nullptr;
		_glUniform3i = nullptr;
		_glUniform4i = nullptr;
		_glUniform1fv = nullptr;
		_glUniform2fv = nullptr;
		_glUniform3fv = nullptr;
		_glUniform4fv = nullptr;
		_glUniform1iv = nullptr;
		_glUniform2iv = nullptr;
		_glUniform3iv = nullptr;
		_glUniform4iv = nullptr;
		_glUniformMatrix2fv = nullptr;
		_glUniformMatrix3fv = nullptr;
		_glUniformMatrix4fv = nullptr;
		_glGetObjectParameterfv = nullptr;
		_glGetObjectParameteriv = nullptr;
		_glGetInfoLog = nullptr;
		_glGetAttachedObjects = nullptr;
		_glGetUniformLocation = nullptr;
		_glGetActiveUniform = nullptr;
		_glGetUniformfv = nullptr;
		_glGetUniformiv = nullptr;
		_glGetShaderSource = nullptr;

		return false;
	}

	return true;
}

/*
 * GL_EXT_framebuffer_object initialization.
 */
bool __Initialize_GL_EXT_framebuffer_object()
{
	_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenRenderbuffers");
	_glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindRenderbuffer");
	_glRenderbufferStorageMultisample =
		(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisample");
	_glGenFramebuffers = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenFramebuffers");
	_glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindFramebuffer");
	_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glFramebufferRenderbuffer");
	_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatus");
	_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture2D");
	_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffers");
	_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteFramebuffers");
	_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBlitFramebuffer");
	_glIsFramebuffer = (PFNGLISFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glIsFramebuffer");
	_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) SDL_GL_GetProcAddress("glGenerateMipmap");
	_glGetFramebufferAttachmentParameteriv =
		(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv");

	if (!_glGenRenderbuffers || !_glBindRenderbuffer ||
	    !_glRenderbufferStorageMultisample || !_glGenFramebuffers ||
	    !_glBindFramebuffer || !_glFramebufferRenderbuffer ||
	    !_glCheckFramebufferStatus || !_glFramebufferTexture2D ||
	    !_glDeleteRenderbuffers || !_glDeleteFramebuffers ||
	    !_glBlitFramebuffer || !_glIsFramebuffer ||
	    !_glGenerateMipmap || !_glGetFramebufferAttachmentParameteriv) {
		// GL_EXT_framebuffer_object + GL_EXT_framebuffer_multisample + GL_EXT_framebuffer_blit
		_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenRenderbuffersEXT");
		_glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindRenderbufferEXT");
		_glRenderbufferStorageMultisample =
			(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleEXT");
		_glGenFramebuffers = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenFramebuffersEXT");
		_glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindFramebufferEXT");
		_glFramebufferRenderbuffer =
			(PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glFramebufferRenderbufferEXT");
		_glCheckFramebufferStatus =
			(PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
		_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
		_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffersEXT");
		_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
		_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBlitFramebufferEXT");
		_glIsFramebuffer = (PFNGLISFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glIsFramebufferEXT");
		_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) SDL_GL_GetProcAddress("glGenerateMipmapEXT");
		_glGetFramebufferAttachmentParameteriv =
			(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)
				SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	}

	if (!_glGenRenderbuffers || !_glBindRenderbuffer ||
	    !_glRenderbufferStorageMultisample || !_glGenFramebuffers ||
	    !_glBindFramebuffer || !_glFramebufferRenderbuffer ||
	    !_glCheckFramebufferStatus || !_glFramebufferTexture2D ||
	    !_glDeleteRenderbuffers || !_glDeleteFramebuffers ||
	    !_glBlitFramebuffer || !_glIsFramebuffer ||
	    !_glGenerateMipmap || !_glGetFramebufferAttachmentParameteriv) {
		_glGenRenderbuffers = nullptr;
		_glBindRenderbuffer = nullptr;
		_glRenderbufferStorageMultisample = nullptr;
		_glGenFramebuffers = nullptr;
		_glBindFramebuffer = nullptr;
		_glFramebufferRenderbuffer = nullptr;
		_glCheckFramebufferStatus = nullptr;
		_glFramebufferTexture2D = nullptr;
		_glDeleteRenderbuffers = nullptr;
		_glDeleteFramebuffers = nullptr;
		_glBlitFramebuffer = nullptr;
		_glIsFramebuffer = nullptr;
		_glGenerateMipmap = nullptr;
		_glGetFramebufferAttachmentParameteriv= nullptr;

		return false;
	}

	return true;
}

/*
 * GL_NV_framebuffer_multisample_coverage initialization.
 */
bool __Initialize_GL_NV_framebuffer_multisample_coverage()
{
	// optional, GL_NV_framebuffer_multisample_coverage
	_glRenderbufferStorageMultisampleCoverageNV =
		(PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC)
			SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleCoverageNV");

	if (!_glRenderbufferStorageMultisampleCoverageNV)
		return false;

	return true;
}
