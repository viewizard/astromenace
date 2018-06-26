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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../base.h"
#include "opengl.h"

// OpenGL 1.3 (only what we need or would need in future)
PFNGLACTIVETEXTUREPROC _glActiveTexture{nullptr};
PFNGLCLIENTACTIVETEXTUREPROC _glClientActiveTexture{nullptr};

// OpenGL 1.5 (only what we need or would need in future)
PFNGLBINDBUFFERPROC _glBindBuffer{nullptr};
PFNGLDELETEBUFFERSPROC _glDeleteBuffers{nullptr};
PFNGLGENBUFFERSPROC _glGenBuffers{nullptr};
PFNGLISBUFFERPROC _glIsBuffer{nullptr};
PFNGLBUFFERDATAPROC _glBufferData{nullptr};

// OpenGL 2.0 (only what we need or would need in future)
PFNGLATTACHSHADERPROC _glAttachShader{nullptr};
PFNGLBINDATTRIBLOCATIONPROC _glBindAttribLocation{nullptr};
PFNGLCOMPILESHADERPROC _glCompileShader{nullptr};
PFNGLCREATEPROGRAMPROC _glCreateProgram{nullptr};
PFNGLCREATESHADERPROC _glCreateShader{nullptr};
PFNGLDELETEPROGRAMPROC _glDeleteProgram{nullptr};
PFNGLDELETESHADERPROC _glDeleteShader{nullptr};
PFNGLDETACHSHADERPROC _glDetachShader{nullptr};
PFNGLGETACTIVEATTRIBPROC _glGetActiveAttrib{nullptr};
PFNGLGETACTIVEUNIFORMPROC _glGetActiveUniform{nullptr};
PFNGLGETATTACHEDSHADERSPROC _glGetAttachedShaders{nullptr};
PFNGLGETATTRIBLOCATIONPROC _glGetAttribLocation{nullptr};
PFNGLGETPROGRAMIVPROC _glGetProgramiv{nullptr};
PFNGLGETPROGRAMINFOLOGPROC _glGetProgramInfoLog{nullptr};
PFNGLGETSHADERIVPROC _glGetShaderiv{nullptr};
PFNGLGETSHADERINFOLOGPROC _glGetShaderInfoLog{nullptr};
PFNGLGETSHADERSOURCEPROC _glGetShaderSource{nullptr};
PFNGLGETUNIFORMLOCATIONPROC _glGetUniformLocation{nullptr};
PFNGLGETUNIFORMFVPROC _glGetUniformfv{nullptr};
PFNGLGETUNIFORMIVPROC _glGetUniformiv{nullptr};
PFNGLISPROGRAMPROC _glIsProgram{nullptr};
PFNGLISSHADERPROC _glIsShader{nullptr};
PFNGLLINKPROGRAMPROC _glLinkProgram{nullptr};
PFNGLSHADERSOURCEPROC _glShaderSource{nullptr};
PFNGLUSEPROGRAMPROC _glUseProgram{nullptr};
PFNGLUNIFORM1FPROC _glUniform1f{nullptr};
PFNGLUNIFORM2FPROC _glUniform2f{nullptr};
PFNGLUNIFORM3FPROC _glUniform3f{nullptr};
PFNGLUNIFORM4FPROC _glUniform4f{nullptr};
PFNGLUNIFORM1IPROC _glUniform1i{nullptr};
PFNGLUNIFORM2IPROC _glUniform2i{nullptr};
PFNGLUNIFORM3IPROC _glUniform3i{nullptr};
PFNGLUNIFORM4IPROC _glUniform4i{nullptr};
PFNGLUNIFORM1FVPROC _glUniform1fv{nullptr};
PFNGLUNIFORM2FVPROC _glUniform2fv{nullptr};
PFNGLUNIFORM3FVPROC _glUniform3fv{nullptr};
PFNGLUNIFORM4FVPROC _glUniform4fv{nullptr};
PFNGLUNIFORM1IVPROC _glUniform1iv{nullptr};
PFNGLUNIFORM2IVPROC _glUniform2iv{nullptr};
PFNGLUNIFORM3IVPROC _glUniform3iv{nullptr};
PFNGLUNIFORM4IVPROC _glUniform4iv{nullptr};
PFNGLVALIDATEPROGRAMPROC _glValidateProgram{nullptr};

// OpenGL 2.1 (only what we need or would need in future)
PFNGLUNIFORMMATRIX2X3FVPROC _glUniformMatrix2x3fv{nullptr};
PFNGLUNIFORMMATRIX3X2FVPROC _glUniformMatrix3x2fv{nullptr};
PFNGLUNIFORMMATRIX2X4FVPROC _glUniformMatrix2x4fv{nullptr};
PFNGLUNIFORMMATRIX4X2FVPROC _glUniformMatrix4x2fv{nullptr};
PFNGLUNIFORMMATRIX3X4FVPROC _glUniformMatrix3x4fv{nullptr};
PFNGLUNIFORMMATRIX4X3FVPROC _glUniformMatrix4x3fv{nullptr};

// OpenGL 3.0 (only what we need or would need in future)
PFNGLBINDRENDERBUFFERPROC _glBindRenderbuffer{nullptr};
PFNGLDELETERENDERBUFFERSPROC _glDeleteRenderbuffers{nullptr};
PFNGLGENRENDERBUFFERSPROC _glGenRenderbuffers{nullptr};
PFNGLISFRAMEBUFFERPROC _glIsFramebuffer{nullptr};
PFNGLBINDFRAMEBUFFERPROC _glBindFramebuffer{nullptr};
PFNGLDELETEFRAMEBUFFERSPROC _glDeleteFramebuffers{nullptr};
PFNGLGENFRAMEBUFFERSPROC _glGenFramebuffers{nullptr};
PFNGLCHECKFRAMEBUFFERSTATUSPROC _glCheckFramebufferStatus{nullptr};
PFNGLFRAMEBUFFERTEXTURE2DPROC _glFramebufferTexture2D{nullptr};
PFNGLFRAMEBUFFERRENDERBUFFERPROC _glFramebufferRenderbuffer{nullptr};
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC _glGetFramebufferAttachmentParameteriv{nullptr};
PFNGLGENERATEMIPMAPPROC _glGenerateMipmap{nullptr};
PFNGLBLITFRAMEBUFFERPROC _glBlitFramebuffer{nullptr};
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC _glRenderbufferStorageMultisample{nullptr};
PFNGLBINDVERTEXARRAYPROC _glBindVertexArray{nullptr};
PFNGLDELETEVERTEXARRAYSPROC _glDeleteVertexArrays{nullptr};
PFNGLGENVERTEXARRAYSPROC _glGenVertexArrays{nullptr};
PFNGLISVERTEXARRAYPROC _glIsVertexArray{nullptr};

// OpenGL 4.2 (only what we need or would need in future)
PFNGLTEXSTORAGE2DPROC _glTexStorage2D{nullptr};

// GL_NV_framebuffer_multisample_coverage
PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC _glRenderbufferStorageMultisampleCoverageNV{nullptr};


/*
 * OpenGL 1.3 initialization (only what we need or would need in future).
 */
bool __Initialize_OpenGL_1_3()
{
	_glActiveTexture = (PFNGLACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glActiveTexture");
	_glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glClientActiveTexture");

	if (!_glActiveTexture ||
	    !_glClientActiveTexture) {
		_glActiveTexture = nullptr;
		_glClientActiveTexture = nullptr;

		return false;
	}

	return true;
}

/*
 * OpenGL 1.5 initialization (only what we need or would need in future).
 */
bool __Initialize_OpenGL_1_5()
{
	// get pointers to the GL functions
	_glBindBuffer = (PFNGLBINDBUFFERPROC) SDL_GL_GetProcAddress("glBindBuffer");
	_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteBuffers");
	_glGenBuffers = (PFNGLGENBUFFERSPROC) SDL_GL_GetProcAddress("glGenBuffers");
	_glIsBuffer = (PFNGLISBUFFERPROC) SDL_GL_GetProcAddress("glIsBuffer");
	_glBufferData = (PFNGLBUFFERDATAPROC) SDL_GL_GetProcAddress("glBufferData");

	if (!_glBindBuffer ||
	    !_glDeleteBuffers ||
	    !_glGenBuffers ||
	    !_glIsBuffer ||
	    !_glBufferData) {
		_glBindBuffer = nullptr;
		_glDeleteBuffers = nullptr;
		_glGenBuffers = nullptr;
		_glIsBuffer = nullptr;
		_glBufferData = nullptr;

		return false;
	}

	return true;
}

/*
 * OpenGL 2.0 initialization (only what we need or would need in future).
 */
bool __Initialize_OpenGL_2_0()
{
	_glAttachShader = (PFNGLATTACHSHADERPROC) SDL_GL_GetProcAddress("glAttachShader");
	_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glBindAttribLocation");
	_glCompileShader = (PFNGLCOMPILESHADERPROC) SDL_GL_GetProcAddress("glCompileShader");
	_glCreateProgram = (PFNGLCREATEPROGRAMPROC) SDL_GL_GetProcAddress("glCreateProgram");
	_glCreateShader = (PFNGLCREATESHADERPROC) SDL_GL_GetProcAddress("glCreateShader");
	_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SDL_GL_GetProcAddress("glDeleteProgram");
	_glDeleteShader = (PFNGLDELETESHADERPROC) SDL_GL_GetProcAddress("glDeleteShader");
	_glDetachShader = (PFNGLDETACHSHADERPROC) SDL_GL_GetProcAddress("glDetachShader");
	_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) SDL_GL_GetProcAddress("glGetActiveAttrib");
	_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) SDL_GL_GetProcAddress("glGetActiveUniform");
	_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) SDL_GL_GetProcAddress("glGetAttachedShaders");
	_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glGetAttribLocation");
	_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SDL_GL_GetProcAddress("glGetProgramiv");
	_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SDL_GL_GetProcAddress("glGetProgramInfoLog");
	_glGetShaderiv = (PFNGLGETSHADERIVPROC) SDL_GL_GetProcAddress("glGetShaderiv");
	_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SDL_GL_GetProcAddress("glGetShaderInfoLog");
	_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) SDL_GL_GetProcAddress("glGetShaderSource");
	_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SDL_GL_GetProcAddress("glGetUniformLocation");
	_glGetUniformfv = (PFNGLGETUNIFORMFVPROC) SDL_GL_GetProcAddress("glGetUniformfv");
	_glGetUniformiv = (PFNGLGETUNIFORMIVPROC) SDL_GL_GetProcAddress("glGetUniformiv");
	_glIsProgram = (PFNGLISPROGRAMPROC) SDL_GL_GetProcAddress("glIsProgram");
	_glIsShader = (PFNGLISSHADERPROC) SDL_GL_GetProcAddress("glIsShader");
	_glLinkProgram = (PFNGLLINKPROGRAMPROC) SDL_GL_GetProcAddress("glLinkProgram");
	_glShaderSource = (PFNGLSHADERSOURCEPROC) SDL_GL_GetProcAddress("glShaderSource");
	_glUseProgram = (PFNGLUSEPROGRAMPROC) SDL_GL_GetProcAddress("glUseProgram");
	_glUniform1f = (PFNGLUNIFORM1FPROC) SDL_GL_GetProcAddress("glUniform1f");
	_glUniform2f = (PFNGLUNIFORM2FPROC) SDL_GL_GetProcAddress("glUniform2f");
	_glUniform3f = (PFNGLUNIFORM3FPROC) SDL_GL_GetProcAddress("glUniform3f");
	_glUniform4f = (PFNGLUNIFORM4FPROC) SDL_GL_GetProcAddress("glUniform4f");
	_glUniform1i = (PFNGLUNIFORM1IPROC) SDL_GL_GetProcAddress("glUniform1i");
	_glUniform2i = (PFNGLUNIFORM2IPROC) SDL_GL_GetProcAddress("glUniform2i");
	_glUniform3i = (PFNGLUNIFORM3IPROC) SDL_GL_GetProcAddress("glUniform3i");
	_glUniform4i = (PFNGLUNIFORM4IPROC) SDL_GL_GetProcAddress("glUniform4i");
	_glUniform1fv = (PFNGLUNIFORM1FVPROC) SDL_GL_GetProcAddress("glUniform1fv");
	_glUniform2fv = (PFNGLUNIFORM2FVPROC) SDL_GL_GetProcAddress("glUniform2fv");
	_glUniform3fv = (PFNGLUNIFORM3FVPROC) SDL_GL_GetProcAddress("glUniform3fv");
	_glUniform4fv = (PFNGLUNIFORM4FVPROC) SDL_GL_GetProcAddress("glUniform4fv");
	_glUniform1iv = (PFNGLUNIFORM1IVPROC) SDL_GL_GetProcAddress("glUniform1iv");
	_glUniform2iv = (PFNGLUNIFORM2IVPROC) SDL_GL_GetProcAddress("glUniform2iv");
	_glUniform3iv = (PFNGLUNIFORM3IVPROC) SDL_GL_GetProcAddress("glUniform3iv");
	_glUniform4iv = (PFNGLUNIFORM4IVPROC) SDL_GL_GetProcAddress("glUniform4iv");
	_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) SDL_GL_GetProcAddress("glValidateProgram");

	if (!_glAttachShader ||
	    !_glBindAttribLocation ||
	    !_glCompileShader ||
	    !_glCreateProgram ||
	    !_glCreateShader ||
	    !_glDeleteProgram ||
	    !_glDeleteShader ||
	    !_glDetachShader ||
	    !_glGetActiveAttrib ||
	    !_glGetActiveUniform ||
	    !_glGetAttachedShaders ||
	    !_glGetAttribLocation ||
	    !_glGetProgramiv ||
	    !_glGetProgramInfoLog ||
	    !_glGetShaderiv ||
	    !_glGetShaderInfoLog ||
	    !_glGetShaderSource ||
	    !_glGetUniformLocation ||
	    !_glGetUniformfv ||
	    !_glGetUniformiv ||
	    !_glIsProgram ||
	    !_glIsShader ||
	    !_glLinkProgram ||
	    !_glShaderSource ||
	    !_glUseProgram ||
	    !_glUniform1f ||
	    !_glUniform2f ||
	    !_glUniform3f ||
	    !_glUniform4f ||
	    !_glUniform1i ||
	    !_glUniform2i ||
	    !_glUniform3i ||
	    !_glUniform4i ||
	    !_glUniform1fv ||
	    !_glUniform2fv ||
	    !_glUniform3fv ||
	    !_glUniform4fv ||
	    !_glUniform1iv ||
	    !_glUniform2iv ||
	    !_glUniform3iv ||
	    !_glUniform4iv ||
	    !_glValidateProgram) {
		_glAttachShader = nullptr;
		_glBindAttribLocation = nullptr;
		_glCompileShader = nullptr;
		_glCreateProgram = nullptr;
		_glCreateShader = nullptr;
		_glDeleteProgram = nullptr;
		_glDeleteShader = nullptr;
		_glDetachShader = nullptr;
		_glGetActiveAttrib = nullptr;
		_glGetActiveUniform = nullptr;
		_glGetAttachedShaders = nullptr;
		_glGetAttribLocation = nullptr;
		_glGetProgramiv = nullptr;
		_glGetProgramInfoLog = nullptr;
		_glGetShaderiv = nullptr;
		_glGetShaderInfoLog = nullptr;
		_glGetShaderSource = nullptr;
		_glGetUniformLocation = nullptr;
		_glGetUniformfv = nullptr;
		_glGetUniformiv = nullptr;
		_glIsProgram = nullptr;
		_glIsShader = nullptr;
		_glLinkProgram = nullptr;
		_glShaderSource = nullptr;
		_glUseProgram = nullptr;
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
		_glValidateProgram = nullptr;

		return false;
	}

	return true;
}

/*
 * OpenGL 2.1 initialization (only what we need or would need in future).
 */
bool __Initialize_OpenGL_2_1()
{
	_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) SDL_GL_GetProcAddress("glUniformMatrix2x3fv");
	_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) SDL_GL_GetProcAddress("glUniformMatrix3x2fv");
	_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix2x4fv");
	_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4x2fv");
	_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix3x4fv");
	_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4x3fv");

	if (!_glUniformMatrix2x3fv ||
	    !_glUniformMatrix3x2fv ||
	    !_glUniformMatrix2x4fv ||
	    !_glUniformMatrix4x2fv ||
	    !_glUniformMatrix3x4fv ||
	    !_glUniformMatrix4x3fv) {
		_glUniformMatrix2x3fv = nullptr;
		_glUniformMatrix3x2fv = nullptr;
		_glUniformMatrix2x4fv = nullptr;
		_glUniformMatrix4x2fv = nullptr;
		_glUniformMatrix3x4fv = nullptr;
		_glUniformMatrix4x3fv = nullptr;

		return false;
	}

	return true;
}

/*
 * OpenGL 3.0 initialization (only what we need or would need in future).
 */
bool __Initialize_OpenGL_3_0()
{
	_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) SDL_GL_GetProcAddress("glBindRenderbuffer");
	_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffers");
	_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) SDL_GL_GetProcAddress("glGenRenderbuffers");
	_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glIsFramebuffer");
	_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glBindFramebuffer");
	_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteFramebuffers");
	_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) SDL_GL_GetProcAddress("glGenFramebuffers");
	_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatus");
	_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) SDL_GL_GetProcAddress("glFramebufferTexture2D");
	_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) SDL_GL_GetProcAddress("glFramebufferRenderbuffer");
	_glGetFramebufferAttachmentParameteriv =
		(PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv");
	_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) SDL_GL_GetProcAddress("glGenerateMipmap");
	_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glBlitFramebuffer");
	_glRenderbufferStorageMultisample =
		(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisample");
	_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) SDL_GL_GetProcAddress("glBindVertexArray");
	_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glDeleteVertexArrays");
	_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glGenVertexArrays");
	_glIsVertexArray = (PFNGLISVERTEXARRAYPROC) SDL_GL_GetProcAddress("glIsVertexArray");

	if (!_glBindRenderbuffer ||
	    !_glDeleteRenderbuffers ||
	    !_glGenRenderbuffers ||
	    !_glIsFramebuffer ||
	    !_glBindFramebuffer ||
	    !_glDeleteFramebuffers ||
	    !_glGenFramebuffers ||
	    !_glCheckFramebufferStatus ||
	    !_glFramebufferTexture2D ||
	    !_glFramebufferRenderbuffer ||
	    !_glGetFramebufferAttachmentParameteriv ||
	    !_glGenerateMipmap ||
	    !_glBlitFramebuffer ||
	    !_glRenderbufferStorageMultisample ||
	    !_glBindVertexArray ||
	    !_glDeleteVertexArrays ||
	    !_glGenVertexArrays ||
	    !_glIsVertexArray) {
		_glBindRenderbuffer = nullptr;
		_glDeleteRenderbuffers = nullptr;
		_glGenRenderbuffers = nullptr;
		_glIsFramebuffer = nullptr;
		_glBindFramebuffer = nullptr;
		_glDeleteFramebuffers = nullptr;
		_glGenFramebuffers = nullptr;
		_glCheckFramebufferStatus = nullptr;
		_glFramebufferTexture2D = nullptr;
		_glFramebufferRenderbuffer = nullptr;
		_glGetFramebufferAttachmentParameteriv = nullptr;
		_glGenerateMipmap = nullptr;
		_glBlitFramebuffer = nullptr;
		_glRenderbufferStorageMultisample = nullptr;
		_glBindVertexArray = nullptr;
		_glDeleteVertexArrays = nullptr;
		_glGenVertexArrays = nullptr;
		_glIsVertexArray = nullptr;

		return false;
	}

	return true;
}

/*
 * OpenGL 4.2 initialization (only what we need or would need in future).
 */
bool __Initialize_OpenGL_4_2()
{
	_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) SDL_GL_GetProcAddress("glTexStorage2D");

	return _glTexStorage2D;
}

/*
 * GL_NV_framebuffer_multisample_coverage initialization.
 */
bool __Initialize_GL_NV_framebuffer_multisample_coverage()
{
	_glRenderbufferStorageMultisampleCoverageNV =
		(PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC)
			SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleCoverageNV");

	return _glRenderbufferStorageMultisampleCoverageNV;
}
