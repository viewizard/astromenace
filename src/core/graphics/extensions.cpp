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
