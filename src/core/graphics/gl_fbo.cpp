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

// TODO probably, we need VBO+VAO here, since we don't change arrays,
//      no reason re-send them all the time

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glNamedRenderbufferStorageMultisampleEXT()
//      could be used to replace glRenderbufferStorageMultisample()

// NOTE GL_EXT_direct_state_access (part of extension, not standardized yet)
//      glNamedRenderbufferStorageMultisampleCoverageEXT()
//      could be used to replace glRenderbufferStorageMultisampleCoverageNV()

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

namespace viewizard {

namespace {

// Current FBO, null if FrameBuffer.
std::weak_ptr<sFBO> CurrentFBO{};

} // unnamed namespace


/*
 * Build FBO.
 */
std::shared_ptr<sFBO> vw_BuildFBO(GLsizei Width, GLsizei Height, bool NeedColor, bool NeedDepth, GLsizei MSAA, GLsizei *CSAA)
{
	if (!_glGenRenderbuffers ||
	    !_glBindRenderbuffer ||
	    !_glRenderbufferStorageMultisample ||
	    !_glGenFramebuffers ||
	    !_glBindFramebuffer ||
	    !_glFramebufferRenderbuffer ||
	    !_glCheckFramebufferStatus)
		return std::shared_ptr<sFBO> {};

	// if hardware don't support coverage, switch to MSAA
	if (!_glRenderbufferStorageMultisampleCoverageNV && CSAA)
		*CSAA = MSAA;

	GLsizei InternalCSAA = MSAA;
	if (CSAA)
		InternalCSAA = *CSAA;

	std::shared_ptr<sFBO> FBO{new sFBO, [](sFBO *p) {delete p;}};

	FBO->Width = Width;
	FBO->Height = Height;
	FBO->ColorBuffer = 0;
	FBO->DepthBuffer = 0;
	FBO->ColorTexture = 0;
	FBO->DepthTexture = 0;
	FBO->FrameBufferObject = 0;
	FBO->DepthSize = 0;

	std::cout << "Frame Buffer Object creation start with parameters ";
	std::cout << Width << " " << Height << " " << NeedColor << " "
		  << NeedDepth << " " << MSAA << " " << InternalCSAA << "\n";

	_glGenFramebuffers(1, &FBO->FrameBufferObject);
	_glBindFramebuffer(GL_FRAMEBUFFER, FBO->FrameBufferObject);


	if (NeedColor) {
		// if we need multisamples, use FBO
		if (MSAA >= 2) {
			_glGenRenderbuffers(1, &FBO->ColorBuffer);
			_glBindRenderbuffer(GL_RENDERBUFFER, FBO->ColorBuffer);
			if ((InternalCSAA == MSAA) || (InternalCSAA == 0))
				_glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_RGBA,
								  FBO->Width.i(), FBO->Height.i());
			else
				_glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, InternalCSAA, MSAA,
									    GL_RGBA, FBO->Width.i(), FBO->Height.i());
			_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO->ColorBuffer);
			if(_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << __func__ << "(): " << "Can't create FRAMEBUFFER.\n\n";
				return std::shared_ptr<sFBO> {};
			}
		} else { // if we don't need multisamples, switch to texture
			glGenTextures(1, &FBO->ColorTexture);
			glBindTexture(GL_TEXTURE_2D, FBO->ColorTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FBO->Width.i(), FBO->Height.i(),
				     0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBO->ColorTexture, 0);
			if(_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << __func__ << "(): " << "Can't create FRAMEBUFFER.\n\n";
				return std::shared_ptr<sFBO> {};
			}
		}
	}

	if (NeedDepth) {
		// // if we need multisamples, use FBO
		if (MSAA >= 2) {
			_glGenRenderbuffers(1, &FBO->DepthBuffer);
			_glBindRenderbuffer(GL_RENDERBUFFER, FBO->DepthBuffer);
			if ((InternalCSAA == MSAA) || (InternalCSAA == 0))
				_glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA, GL_DEPTH_COMPONENT,
								  FBO->Width.i(), FBO->Height.i());
			else
				_glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, InternalCSAA, MSAA,
									    GL_DEPTH_COMPONENT, FBO->Width.i(), FBO->Height.i());
			_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBO->DepthBuffer);
			_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							       GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &FBO->DepthSize);
			if(_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << __func__ << "(): " << "Can't create FRAMEBUFFER.\n\n";
				return std::shared_ptr<sFBO> {};
			}
		} else { // if we don't need multisamples, switch to texture
			glGenTextures(1, &FBO->DepthTexture);
			glBindTexture(GL_TEXTURE_2D, FBO->DepthTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, FBO->Width.i(), FBO->Height.i(), 0,
				     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
			_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBO->DepthTexture, 0);
			_glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							       GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &FBO->DepthSize);
			if(_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << __func__ << "(): " << "Can't create FRAMEBUFFER.\n\n";
				return std::shared_ptr<sFBO> {};
			}
		}
	}

	_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::cout << "Frame Buffer Object created. Depth Size: " << FBO->DepthSize << "\n";

	// store new maximum depth size
	if (__GetDevCaps().FramebufferObjectDepthSize < FBO->DepthSize)
		__GetDevCaps().FramebufferObjectDepthSize = FBO->DepthSize;

	return FBO;
}

/*
 * Bind FBO.
 */
void vw_BindFBO(std::shared_ptr<sFBO> &FBO)
{
	if (!_glBindFramebuffer ||
	    (FBO && !FBO->FrameBufferObject))
		return;

	// if current have multisamples, disable it first
	if (auto sharedCurrentFBO = CurrentFBO.lock())
		if (sharedCurrentFBO->ColorBuffer || sharedCurrentFBO->DepthBuffer)
			glDisable(GL_MULTISAMPLE);

	if (FBO) {
		if (FBO->ColorBuffer || FBO->DepthBuffer)
			glEnable(GL_MULTISAMPLE);
		_glBindFramebuffer(GL_FRAMEBUFFER, FBO->FrameBufferObject);
	} else
		_glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CurrentFBO = FBO;
}

/*
 * Get current FBO (null if FrameBuffer).
 */
std::weak_ptr<sFBO> &vw_GetCurrentFBO()
{
	return CurrentFBO;
}

/*
 * Blit color part of source FBO to target FBO (need this one in order to work with multisamples).
 */
void vw_BlitFBO(std::shared_ptr<sFBO> &SourceFBO, std::shared_ptr<sFBO> &TargetFBO)
{
	if (!SourceFBO || !TargetFBO ||
	    !_glBindFramebuffer || !_glBlitFramebuffer ||
	    !SourceFBO->FrameBufferObject || !TargetFBO->FrameBufferObject)
		return;

	_glBindFramebuffer(GL_READ_FRAMEBUFFER, SourceFBO->FrameBufferObject);
	_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, TargetFBO->FrameBufferObject);
	_glBlitFramebuffer(0, 0, SourceFBO->Width.i(), SourceFBO->Height.i(),
			   0, 0, TargetFBO->Width.i(), TargetFBO->Height.i(),
			   GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

/*
 * Draw source FBO (color texture) to target FBO (if null, to FrameBuffer).
 */
void vw_DrawColorFBO(std::shared_ptr<sFBO> &SourceFBO, std::shared_ptr<sFBO> &TargetFBO)
{
	if (!SourceFBO || !SourceFBO->ColorTexture)
		return;

	vw_BindFBO(TargetFBO);
	glViewport(0, 0, SourceFBO->Width.i(), SourceFBO->Height.i());

	glPushAttrib(GL_ENABLE_BIT);
	// disable useless flags
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);	// select the projection matrix
	glPushMatrix();			// store the projection matrix
	glLoadIdentity();		// reset the projection matrix

	glOrtho(0, SourceFBO->Width.i(), 0, SourceFBO->Height.i(), -1, 1);

	glMatrixMode(GL_MODELVIEW);	// select the modelview matrix
	glPushMatrix();
	glLoadIdentity();

	// RI_2f_XY | RI_1_TEX
	//		     X				Y			U	V
	float DrawBuffer[16]{0.0f,			0.0f,			0.0f,	0.0f,
			     0.0f,			SourceFBO->Height.f(),	0.0f,	1.0f,
			     SourceFBO->Width.f(),	0.0f,			1.0f,	0.0f,
			     SourceFBO->Width.f(),	SourceFBO->Height.f(),	1.0f,	1.0f};

	vw_BindTexture(0, SourceFBO->ColorTexture);

	vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, DrawBuffer, 4 * sizeof(DrawBuffer[0]));

	vw_BindTexture(0, 0);

	glMatrixMode(GL_PROJECTION);	// select the projection matrix
	glPopMatrix();			// restore the old projection matrix

	glMatrixMode(GL_MODELVIEW);	// select the modelview matrix
	glPopMatrix();

	glPopAttrib();
}

/*
 * Destructor.
 */
sFBO::~sFBO()
{
	if (ColorTexture)
		glDeleteTextures(1, &ColorTexture);

	if (DepthTexture)
		glDeleteTextures(1, &DepthTexture);

	if (_glDeleteRenderbuffers) {
		if (ColorBuffer)
			_glDeleteRenderbuffers(1, &ColorBuffer);
		if (DepthBuffer)
			_glDeleteRenderbuffers(1, &DepthBuffer);
	}

	if (_glDeleteFramebuffers &&
	    FrameBufferObject)
		_glDeleteFramebuffers(1, &FrameBufferObject);
}

} // viewizard namespace
