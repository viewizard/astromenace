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


bool __Initialize_GL_EXT_framebuffer_object();
bool __Initialize_GL_NV_framebuffer_multisample_coverage();

#endif // GraphicsExtensions_H
