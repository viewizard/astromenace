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

// NOTE SDL2 could be used for OpenGL context setup. "request" OpenGL context version:
//      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

// NOTE glGetStringi() for GL_EXTENSIONS (since OpenGL 3.0)
//      glGetString() usage with GL_EXTENSIONS is deprecated
//
//      int NumberOfExtensions;
//      glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
//      for(int i = 0; i < NumberOfExtensions; i++) {
//      	const GLubyte *one_string = glGetStringi(GL_EXTENSIONS, i);
//      }

// NOTE GL_MAX_TEXTURE_MAX_ANISOTROPY (since OpenGL 4.6)
//      could be used to replace GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

namespace {

// hardware device capabilities
sDevCaps DevCaps{};

// for 2D we need fixed internal resolution, since window's size could be
// different and we should guaranty, that 2D looks same for all window's sizes,
// it is important to understand, that we don't use this values directly,
// but only for 'mapping' internal window coordinates into real window coordinates
float InternalWidth{0.0f};
float InternalHeight{0.0f};
bool InternalResolution{false};

// pointer to main window structure
SDL_Window *SDLWindow{nullptr};
// pointer to OpenGL context
SDL_GLContext GLContext{nullptr};

// main FBO
std::shared_ptr<sFBO> MainFBO{};
// resolve FBO (for blit main FBO with multisample)
std::shared_ptr<sFBO> ResolveFBO{};

} // unnamed namespace


/*
 * Get SDL window handle.
 */
SDL_Window *vw_GetSDLWindow()
{
	assert(SDLWindow);

	return SDLWindow;
}

/*
 * Check supported OpenGL extension.
 */
static bool ExtensionSupported(const char *Extension)
{
	char *extensions;
	extensions = (char *) glGetString(GL_EXTENSIONS); // WARNING fix conversion
	if (strstr(extensions, Extension) != nullptr)
		return true;
	return false;
}

/*
 * Create window.
 */
bool vw_CreateWindow(const char *Title, int Width, int Height, bool Fullscreen, int DisplayIndex)
{
	Uint32 Flags{SDL_WINDOW_OPENGL};

	if (Fullscreen)
		Flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	SDLWindow = SDL_CreateWindow(Title,
				     SDL_WINDOWPOS_CENTERED_DISPLAY(DisplayIndex),
				     SDL_WINDOWPOS_CENTERED_DISPLAY(DisplayIndex),
				     Width, Height, Flags);
	if (!SDLWindow) {
		std::cerr << __func__ << "(): " << "SDL_CreateWindow() failed: " << SDL_GetError() << "\n";
		std::cerr << __func__ << "(): " << "Can't set video mode " <<  Width << " x " << Height << "\n\n";
		return false;
	}

	if (Fullscreen)
		std::cout << "Fullscreen mode: ";
	else
		std::cout << "Windowed mode: ";
	std::cout << Width << " x " << Height << "\n\n";

	SDL_DisableScreenSaver();

	return true;
}

/*
 * Destroy window.
 */
void vw_DestroyWindow()
{
	if (!SDLWindow)
		return;

	SDL_DestroyWindow(SDLWindow);
	SDLWindow = nullptr;
}

/*
 * Create OpenGL context.
 */
bool vw_CreateOpenGLContext(int VSync)
{
	if (!SDLWindow) {
		std::cerr << __func__ << "(): " << "Can't create OpenGL context, create window first.\n";
		return false;
	}

	GLContext = SDL_GL_CreateContext(SDLWindow);

	if (!GLContext) {
		std::cerr << __func__ << "(): " << "SDL_GL_CreateContext() failed: " << SDL_GetError() << "\n";
		std::cerr << __func__ << "(): " << "Can't create OpenGL context.\n";
		return false;
	}

	if (SDL_GL_SetSwapInterval(VSync) == -1)
		std::cerr << __func__ << "(): " << "SDL_GL_SetSwapInterval() failed: " << SDL_GetError() << "\n";

	DevCaps.OpenGLmajorVersion = 1;
	DevCaps.OpenGLminorVersion = 0;
	DevCaps.MaxTextureWidth = 0;
	DevCaps.MaxTextureHeight = 0;
	DevCaps.MaxActiveLights = 0;
	DevCaps.MaxAnisotropyLevel = 0;
	DevCaps.FramebufferObjectDepthSize = 0;

	DevCaps.OpenGL_1_3_supported = __Initialize_OpenGL_1_3();
	DevCaps.OpenGL_1_5_supported = __Initialize_OpenGL_1_5();
	DevCaps.OpenGL_2_0_supported = __Initialize_OpenGL_2_0();
	DevCaps.OpenGL_2_1_supported = __Initialize_OpenGL_2_1();
	DevCaps.OpenGL_3_0_supported = __Initialize_OpenGL_3_0();
	DevCaps.OpenGL_4_2_supported = __Initialize_OpenGL_4_2();
	__Initialize_GL_NV_framebuffer_multisample_coverage(); // we don't have it in DevCaps, this is 1 function check only

	DevCaps.EXT_texture_compression_s3tc = ExtensionSupported("GL_EXT_texture_compression_s3tc");
	DevCaps.ARB_texture_compression_bptc = ExtensionSupported("GL_ARB_texture_compression_bptc");
	DevCaps.ARB_texture_non_power_of_two = ExtensionSupported("GL_ARB_texture_non_power_of_two");
	DevCaps.SGIS_generate_mipmap = ExtensionSupported("GL_SGIS_generate_mipmap");

	if (ExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &DevCaps.MaxAnisotropyLevel);
		std::cout << "Max anisotropy: " << DevCaps.MaxAnisotropyLevel << "\n";
	}

	std::cout << "Vendor     : " << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer   : " << glGetString(GL_RENDERER) << "\n";
	std::cout << "Version    : " << glGetString(GL_VERSION) << "\n";
	glGetIntegerv(GL_MAJOR_VERSION, &DevCaps.OpenGLmajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &DevCaps.OpenGLminorVersion);
	glGetError(); // reset errors
	std::cout << "OpenGL Version    : " << DevCaps.OpenGLmajorVersion << "." << DevCaps.OpenGLminorVersion << "\n\n";
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &DevCaps.MaxTextureHeight);
	std::cout << "Max texture height: " << DevCaps.MaxTextureHeight << "\n";
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &DevCaps.MaxTextureWidth);
	std::cout << "Max texture width: " << DevCaps.MaxTextureWidth << "\n";
	glGetIntegerv(GL_MAX_LIGHTS, &DevCaps.MaxActiveLights);
	std::cout << "Max lights: " << DevCaps.MaxActiveLights << "\n";

	// since we support FBO, should check supported samples
	if (DevCaps.OpenGL_3_0_supported) {
		int MaxSamples{0};
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &MaxSamples);
		std::cout << "Max Samples: " << MaxSamples << "\n";

		// MSAA
		for (int i = 2; i <= MaxSamples; i *= 2) {
			DevCaps.MultisampleCoverageModes.push_back(sCoverageModes{i, i});
		}

		// CSAA
		if (ExtensionSupported("GL_NV_framebuffer_multisample_coverage")) {
			int NumModes{0};
			glGetIntegerv(GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV, &NumModes);
			std::cout << "Coverage modes: " << NumModes << "\n";
			std::vector<int> modes(NumModes * 2);
			glGetIntegerv(GL_MULTISAMPLE_COVERAGE_MODES_NV, modes.data());
			// fill MultisampleCoverageModes with MSAA/CSAA modes
			DevCaps.MultisampleCoverageModes.clear();
			for (int i = 0; i < (NumModes * 2); i += 2) {
				DevCaps.MultisampleCoverageModes.push_back(sCoverageModes{modes[i + 1], modes[i]});
			}
		}
	}

#ifndef NDEBUG
	// print all supported OpenGL extensions (one per line)
	if (glGetString(GL_EXTENSIONS) != nullptr) {
		std::string extensions{(char *)glGetString(GL_EXTENSIONS)}; // WARNING fix conversion
		if (!extensions.empty()) {
			std::replace(extensions.begin(), extensions.end(), ' ', '\n'); // replace all ' ' to '\n'
			std::cout << "Supported OpenGL extensions:\n" << extensions.c_str() << "\n";
		}
	}
#endif // NDEBUG

	std::cout << "\n";

	return true;
}

/*
 * Delete OpenGL context.
 */
void vw_DeleteOpenGLContext()
{
	if (!GLContext)
		return;

	SDL_GL_DeleteContext(GLContext);
	GLContext = nullptr;
}

/*
 * Initialize (or reinitialize) and setup OpenGL related stuff.
 */
void vw_InitOpenGLStuff(int Width, int Height, int *MSAA, int *CSAA)
{
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);
	glClearStencil(0);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	if (DevCaps.OpenGL_3_0_supported) {
		MainFBO = vw_BuildFBO(Width, Height, true, true, *MSAA, CSAA);
		ResolveFBO = vw_BuildFBO(Width, Height, true, false);
		if (!MainFBO || !ResolveFBO) {
			*MSAA = 0;
			MainFBO.reset();
			ResolveFBO.reset();
			DevCaps.FramebufferObjectDepthSize = 0;
		}
	} else {
		*MSAA = 0;
		MainFBO.reset();
		ResolveFBO.reset();
		DevCaps.FramebufferObjectDepthSize = 0;
	}
}

/*
 * Release OpenGL related stuff.
 */
void vw_ReleaseOpenGLStuff()
{
	vw_ReleaseAllShaders();

	MainFBO.reset();
	ResolveFBO.reset();
}

/*
 * Get device capability.
 */
const sDevCaps &vw_GetDevCaps()
{
	return DevCaps;
}

/*
 * Internal access to DevCaps, with write access.
 */
sDevCaps &__GetDevCaps()
{
	return DevCaps;
}

/*
 * Resize scene.
 */
void vw_ResizeScene(float FieldOfViewAngle, float AspectRatio, float zNearClip, float zFarClip)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(FieldOfViewAngle, AspectRatio, zNearClip, zFarClip);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
 * Clear buffers.
 */
void vw_Clear(int mask)
{
	GLbitfield glmask{0};

	if (mask & 0x1000)
		glmask = glmask | GL_COLOR_BUFFER_BIT;
	if (mask & 0x0100)
		glmask = glmask | GL_DEPTH_BUFFER_BIT;
	if (mask & 0x0010)
		glmask = glmask | GL_ACCUM_BUFFER_BIT;
	if (mask & 0x0001)
		glmask = glmask | GL_STENCIL_BUFFER_BIT;

	glClear(glmask);
}

/*
 * Begin rendering.
 */
void vw_BeginRendering(int mask)
{
	vw_BindFBO(MainFBO);

	vw_Clear(mask);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
 * End rendering.
 */
void vw_EndRendering()
{
	if (MainFBO) {
		std::shared_ptr<sFBO> tmpEmptyFBO{};

		if (MainFBO->ColorTexture)
			vw_DrawColorFBO(MainFBO, tmpEmptyFBO);
		else {
			// if we use multisamples, should blit to color buffer first
			vw_BlitFBO(MainFBO, ResolveFBO);
			vw_DrawColorFBO(ResolveFBO, tmpEmptyFBO);
		}
	}

	assert(SDLWindow);

	SDL_GL_SwapWindow(SDLWindow);
}

/*
 * Set fixed internal resolution size and status.
 */
void vw_SetInternalResolution(float Width, float Height, bool Status)
{
	InternalResolution = Status;

	if (Status) {
		InternalWidth = Width;
		InternalHeight = Height;
	}
}

/*
 * Set fixed internal resolution status.
 */
void vw_SetInternalResolutionStatus(bool Status)
{
	InternalResolution = Status;
}

/*
 * Get fixed internal resolution.
 */
bool vw_GetInternalResolution(float *Width, float *Height)
{
	if (Width)
		*Width = InternalWidth;
	if (Height)
		*Height = InternalHeight;

	return InternalResolution;
}

/*
 * Set depth range.
 */
void vw_DepthRange(GLdouble zNear, GLdouble zFar)
{
	glDepthRange(zNear, zFar);
}

/*
 * Set viewport data.
 */
void vw_SetViewport(GLint x, GLint y, GLsizei width, GLsizei height, eOrigin Origin)
{
	assert(SDLWindow);

	if (Origin == eOrigin::upper_left) {
		int SDLWindowWidth, SDLWindowHeight;
		SDL_GetWindowSize(SDLWindow, &SDLWindowWidth, &SDLWindowHeight);
		y = SDLWindowHeight - y - height;
	}

	glViewport(x, y, width, height);
}

/*
 * Get viewport data.
 */
void vw_GetViewport(float *x, float *y, float *width, float *height)
{
	GLfloat buff[4];
	glGetFloatv(GL_VIEWPORT, buff);

	if (x)
		*x = buff[0];
	if (y)
		*y = buff[1];
	if (width)
		*width = buff[2];
	if (height)
		*height = buff[3];
}

/*
 * Get viewport data.
 */
void vw_GetViewport(int *x, int *y, int *width, int *height)
{
	GLint buff[4];
	glGetIntegerv(GL_VIEWPORT, buff);

	if (x)
		*x = buff[0];
	if (y)
		*y = buff[1];
	if (width)
		*width = buff[2];
	if (height)
		*height = buff[3];
}

/*
 * Set what facets can be culled.
 */
void vw_CullFace(eCullFace mode)
{
	if (mode == eCullFace::NONE) {
		glDisable(GL_CULL_FACE);
		return;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(static_cast<GLenum>(mode));
}

/*
 * Set the scale and units used to calculate depth values.
 */
void vw_PolygonOffset(bool status, GLfloat factor, GLfloat units)
{
	if (status)
		glEnable(GL_POLYGON_OFFSET_FILL);
	else
		glDisable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(factor, units);
}

/*
 * Specify clear values for the color buffers.
 */
void vw_SetClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	glClearColor(red, green, blue, alpha);
}

/*
 * Set color.
 */
void vw_SetColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	glColor4f(red, green, blue, alpha);
}

/*
 * Specifies whether the individual color components in the frame buffer can or cannot be written.
 */
void vw_SetColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	glColorMask(red, green, blue, alpha);
}

/*
 * Set depth buffer.
 */
void vw_DepthTest(bool mode, eCompareFunc func)
{
	if (mode) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(static_cast<GLenum>(func));
	} else
		glDisable(GL_DEPTH_TEST);
}
