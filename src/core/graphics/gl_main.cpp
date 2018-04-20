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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE GL_MAX_TEXTURE_MAX_ANISOTROPY (since OpenGL 4.6)
//      could be used to replace GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"



//------------------------------------------------------------------------------------
// переменные...
//------------------------------------------------------------------------------------
sDevCaps OpenGL_DevCaps;

float fClearRedGL = 0.0f;
float fClearGreenGL = 0.0f;
float fClearBlueGL = 0.0f;
float fClearAlphaGL = 1.0f;

float fNearClipGL = 1.0f;
float fFarClipGL = 1000.0f;
float fAngleGL = 45.0f;

// aspect ratio
float ARWidthGL;
float ARHeightGL;
bool ARFLAGGL = false;





SDL_Window *window_SDL2 = nullptr;
SDL_Window *vw_GetSDL2Windows()
{
	return window_SDL2;
}

float CurrentGammaGL = 1.0f;
float CurrentContrastGL = 1.0f;
float CurrentBrightnessGL = 1.0f;

GLsizei ScreenWidthGL{0};
GLsizei ScreenHeightGL{0};

sFBO MainFBO; // основной FBO, для прорисовки со сглаживанием
sFBO ResolveFBO; // FBO для вывода основного




//------------------------------------------------------------------------------------
// получение поддерживаемых устройством расширений
//------------------------------------------------------------------------------------
bool ExtensionSupported( const char *Extension)
{
	char *extensions;
	extensions=(char *) glGetString(GL_EXTENSIONS);
	// если можем получить указатель, значит это расширение есть
	if (strstr(extensions, Extension) != nullptr) return true;
	return false;
}







//------------------------------------------------------------------------------------
// инициализация окна приложения и получение возможностей железа
//------------------------------------------------------------------------------------
int vw_InitWindow(const char* Title, int Width, int Height, int *Bits, bool FullScreenFlag, int CurrentVideoModeX, int CurrentVideoModeY, int VSync)
{
	// самым первым делом - запоминаем все
	ScreenWidthGL = Width;
	ScreenHeightGL = Height;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем режим и делаем окно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Uint32 Flags = SDL_WINDOW_OPENGL;

	if (FullScreenFlag) Flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	window_SDL2 = SDL_CreateWindow(Title, CurrentVideoModeX, CurrentVideoModeY, Width, Height, Flags);
	if (window_SDL2 == nullptr) {
		std::cerr << __func__ << "(): " << "SDL Error: " << SDL_GetError() << "\n";
		std::cerr << __func__ << "(): " << "Can't set video mode " <<  Width << " x " << Height << "\n\n";
		return 1;
	}
	SDL_GL_CreateContext(window_SDL2);
	SDL_GL_SetSwapInterval(VSync);
	SDL_DisableScreenSaver();

	std::cout << "Set video mode: " << Width << " x " << Height << " x " << *Bits << "\n\n";


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем данные по возможностям железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	OpenGL_DevCaps.OpenGLmajorVersion = 1;
	OpenGL_DevCaps.OpenGLminorVersion = 0;
	OpenGL_DevCaps.MaxMultTextures = 0;
	OpenGL_DevCaps.MaxTextureWidth = 0;
	OpenGL_DevCaps.MaxTextureHeight = 0;
	OpenGL_DevCaps.MaxActiveLights = 0;
	OpenGL_DevCaps.MaxAnisotropyLevel = 0;
	OpenGL_DevCaps.MaxSamples = 0;
	OpenGL_DevCaps.MaxMultisampleCoverageModes = 0;
	OpenGL_DevCaps.TexturesCompression = false;
	OpenGL_DevCaps.TexturesCompressionBPTC = false;
	OpenGL_DevCaps.VBOSupported = false;
	OpenGL_DevCaps.VAOSupported = false;
	OpenGL_DevCaps.TextureNPOTSupported = false;
	OpenGL_DevCaps.GLSL100Supported = false;
	OpenGL_DevCaps.ShaderModel = 0;
	OpenGL_DevCaps.HardwareMipMapGeneration = false;
	OpenGL_DevCaps.TextureStorage = false;
	OpenGL_DevCaps.FramebufferObject = false;
	OpenGL_DevCaps.FramebufferObjectDepthSize = 0;
	OpenGL_DevCaps.OpenGL_1_3_supported = __Initialize_OpenGL_1_3();
	OpenGL_DevCaps.OpenGL_1_5_supported = __Initialize_OpenGL_1_5();
	OpenGL_DevCaps.OpenGL_2_0_supported = __Initialize_OpenGL_2_0();
	OpenGL_DevCaps.OpenGL_3_0_supported = __Initialize_OpenGL_3_0();
	OpenGL_DevCaps.OpenGL_4_2_supported = __Initialize_OpenGL_4_2();
	__Initialize_GL_NV_framebuffer_multisample_coverage();

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем возможности железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	std::cout << "Vendor     : " << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer   : " << glGetString(GL_RENDERER) << "\n";
	std::cout << "Version    : " << glGetString(GL_VERSION) << "\n";
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGL_DevCaps.OpenGLmajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGL_DevCaps.OpenGLminorVersion);
	// после GL_MAJOR_VERSION и GL_MINOR_VERSION сбрасываем ошибку, т.к. тут можем получить
	// 0x500 GL_INVALID_ENUM
	glGetError();
	std::cout << "OpenGL Version    : " << OpenGL_DevCaps.OpenGLmajorVersion << "." << OpenGL_DevCaps.OpenGLminorVersion << "\n\n";
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL_DevCaps.MaxTextureHeight);
	std::cout << "Max texture height: " << OpenGL_DevCaps.MaxTextureHeight << "\n";
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL_DevCaps.MaxTextureWidth);
	std::cout << "Max texture width: " << OpenGL_DevCaps.MaxTextureWidth << "\n";
	glGetIntegerv(GL_MAX_LIGHTS, &OpenGL_DevCaps.MaxActiveLights);
	std::cout << "Max lights: " << OpenGL_DevCaps.MaxActiveLights << "\n";


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// дальше проверяем обязательные части, то, без чего вообще работать не будем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверяем поддержку мультитекстуры
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &OpenGL_DevCaps.MaxMultTextures);
	std::cout << "Max multitexture supported: " << OpenGL_DevCaps.MaxMultTextures << " textures.\n";
	// shader-based GL 2.0 and above programs should use GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS only


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим остальные поддерживаемые функции
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверем поддержку анизотропной фильтрации
	if (ExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
		// получим максимально доступный угол анизотропии...
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &OpenGL_DevCaps.MaxAnisotropyLevel);
		std::cout << "Max anisotropy: " << OpenGL_DevCaps.MaxAnisotropyLevel << "\n";
	}

	// проверем поддержку VBO
	if (ExtensionSupported("GL_ARB_vertex_buffer_object")) {
		OpenGL_DevCaps.VBOSupported = true;
		std::cout << "Vertex Buffer support enabled.\n";
	}

	// проверем поддержку VAO
	if (ExtensionSupported("GL_ARB_vertex_array_object")) {
		OpenGL_DevCaps.VAOSupported = true;
		std::cout << "Vertex Array support enabled.\n";
	}

	// проверем поддержку non_power_of_two генерацию текстур
	if (ExtensionSupported("GL_ARB_texture_non_power_of_two"))
		OpenGL_DevCaps.TextureNPOTSupported = true;

	// проверяем, есть ли поддержка компрессии текстур
	if (ExtensionSupported("GL_ARB_texture_compression") && ExtensionSupported("GL_EXT_texture_compression_s3tc")) {
		OpenGL_DevCaps.TexturesCompression = true;
		std::cout << "Textures S3TC compression support enabled.\n";
	}
	if (ExtensionSupported("GL_ARB_texture_compression") && ExtensionSupported("GL_ARB_texture_compression_bptc")) {
		OpenGL_DevCaps.TexturesCompressionBPTC = true;
		std::cout << "Textures BPTC compression support enabled.\n";
	}

	// проверяем, есть ли поддержка SGIS_generate_mipmap (хардварная генерация мипмеп уровней)
	if (ExtensionSupported("SGIS_generate_mipmap")) {
		OpenGL_DevCaps.HardwareMipMapGeneration = true;
	}

	// проверяем, есть ли поддержка GL_ARB_framebuffer_object (GL_EXT_framebuffer_object+GL_EXT_framebuffer_multisample+GL_EXT_framebuffer_blit)
	if (ExtensionSupported("GL_ARB_framebuffer_object") ||
	    (ExtensionSupported("GL_EXT_framebuffer_blit") && ExtensionSupported("GL_EXT_framebuffer_multisample") && ExtensionSupported("GL_EXT_framebuffer_object"))) {
		OpenGL_DevCaps.FramebufferObject = true;
		std::cout << "Frame Buffer Object support enabled.\n";
	}

	// проверяем, есть ли поддержка GL_ARB_texture_storage или GL_EXT_texture_storage
	if (ExtensionSupported("GL_ARB_texture_storage") || ExtensionSupported("GL_EXT_texture_storage")) {
		OpenGL_DevCaps.TextureStorage = true;
		std::cout << "Texture Storage support enabled.\n";
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// шейдеры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверяем, есть ли поддержка шейдеров GLSL версии 1.00
	if (ExtensionSupported("GL_ARB_shader_objects") &&
	    ExtensionSupported("GL_ARB_vertex_shader") &&
	    ExtensionSupported("GL_ARB_fragment_shader") &&
	    ExtensionSupported("GL_ARB_shading_language_100")) {
		OpenGL_DevCaps.GLSL100Supported = true;
	}

	// определяем какую версию шейдеров поддерживаем в железе.
	// т.к. это может пригодиться для определения как работает GLSL,
	// ведь может работать даже с шейдерной моделью 2.0 (в обрезанном режиме), полному GLSL 1.0 нужна модель 3.0 или выше
	OpenGL_DevCaps.ShaderModel = 0.0f;
	// If a card supports GL_ARB_vertex_program and/or GL_ARB_vertex_shader, it supports vertex shader 1.1.
	// If a card supports GL_NV_texture_shader and GL_NV_register_combiners, it supports pixel shader 1.1.
	if ((ExtensionSupported("GL_ARB_vertex_program") || ExtensionSupported("GL_ARB_vertex_shader")) &&
	    ExtensionSupported("GL_NV_texture_shader") && ExtensionSupported("GL_NV_register_combiners")) {
		OpenGL_DevCaps.ShaderModel = 1.1f;
	}
	// If a card supports GL_ATI_fragment_shader or GL_ATI_text_fragment_shader it supports pixel shader 1.4.
	if (ExtensionSupported("GL_ATI_fragment_shader") || ExtensionSupported("GL_ATI_text_fragment_shader")) {
		OpenGL_DevCaps.ShaderModel = 1.4f;
	}
	// If a card supports GL_ARB_fragment_program and/or GL_ARB_fragment_shader it supports Shader Model 2.0.
	if (ExtensionSupported("GL_ARB_fragment_program") || ExtensionSupported("GL_ARB_fragment_shader")) {
		OpenGL_DevCaps.ShaderModel = 2.0f;
	}
	// If a card supports GL_NV_vertex_program3 or GL_ARB_shader_texture_lod/GL_ATI_shader_texture_lod it supports Shader Model 3.0.
	if (ExtensionSupported("GL_ARB_shader_texture_lod") || ExtensionSupported("GL_NV_vertex_program3") || ExtensionSupported("GL_ATI_shader_texture_lod")) {
		OpenGL_DevCaps.ShaderModel = 3.0f;
	}
	// If a card supports GL_EXT_gpu_shader4 it is a Shader Model 4.0 card. (Geometry shaders are implemented in GL_EXT_geometry_shader4)
	if (ExtensionSupported("GL_EXT_gpu_shader4")) {
		OpenGL_DevCaps.ShaderModel = 4.0f;
	}

	// проверяем, если версия опенжл выше 3.3, версия шейдеров им соответствует
	// (если мы не нашли более высокую через расширения ранее, ставим по версии опенжл)
	float OpenGLVersion = OpenGL_DevCaps.OpenGLmajorVersion;
	if (OpenGL_DevCaps.OpenGLminorVersion != 0) OpenGLVersion += OpenGL_DevCaps.OpenGLminorVersion/10.0f;
	if ((OpenGL_DevCaps.ShaderModel >= 3.0f) && (OpenGLVersion >= 3.3))
		if (OpenGL_DevCaps.ShaderModel < OpenGLVersion) OpenGL_DevCaps.ShaderModel = OpenGLVersion;

	// выводим эти данные
	if (OpenGL_DevCaps.ShaderModel == 0.0f)
		std::cout << "Shaders unsupported.\n";
	else
		std::cout << "Shader Model: " << OpenGL_DevCaps.ShaderModel << "\n";


	// если есть полная поддержка FBO, значит можем работать с семплами
	if (OpenGL_DevCaps.FramebufferObject) {
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &OpenGL_DevCaps.MaxSamples);
		std::cout << "Max Samples: " << OpenGL_DevCaps.MaxSamples << "\n";

		// дальше может и не быть GL_NV_framebuffer_multisample_coverage, потому делаем список сглаживаний
		int TestSample = 2;
		OpenGL_DevCaps.MaxMultisampleCoverageModes = 0;
		while (TestSample <= OpenGL_DevCaps.MaxSamples) {
			OpenGL_DevCaps.MultisampleCoverageModes[OpenGL_DevCaps.MaxMultisampleCoverageModes].ColorSamples = TestSample;
			OpenGL_DevCaps.MultisampleCoverageModes[OpenGL_DevCaps.MaxMultisampleCoverageModes].CoverageSamples = TestSample;
			OpenGL_DevCaps.MaxMultisampleCoverageModes++;
			TestSample = TestSample*2;
		}

		// проверяем, есть ли поддержка CSAA
		if (ExtensionSupported("GL_NV_framebuffer_multisample_coverage")) {
			glGetIntegerv( GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV, &OpenGL_DevCaps.MaxMultisampleCoverageModes);
			std::cout << "Max Multisample coverage modes: " << OpenGL_DevCaps.MaxMultisampleCoverageModes << "\n";
			int *coverageConfigs = nullptr;
			coverageConfigs = new int[OpenGL_DevCaps.MaxMultisampleCoverageModes * 2 + 4];
			glGetIntegerv( GL_MULTISAMPLE_COVERAGE_MODES_NV, coverageConfigs);

			// просматриваем все конфиги, печатаем их и делаем второй тест на MSAA
			int MaxMultiSampleTypeTest2 = -1;
			for (int kk = 0; kk < OpenGL_DevCaps.MaxMultisampleCoverageModes; kk++) {
				OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples = coverageConfigs[kk*2+1];
				OpenGL_DevCaps.MultisampleCoverageModes[kk].CoverageSamples = coverageConfigs[kk*2];

				if (OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples == OpenGL_DevCaps.MultisampleCoverageModes[kk].CoverageSamples) {
					// если ковередж и глубина/цвет одинаковые - это обычный MSAA
					std::cout << " - " << OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples << " MSAA\n";
					if (MaxMultiSampleTypeTest2 < OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples) MaxMultiSampleTypeTest2 = OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples;
				} else {
					// CSAA
					std::cout << " - " <<  OpenGL_DevCaps.MultisampleCoverageModes[kk].CoverageSamples << "/"
						  << OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples << " CSAA\n";
				}
			}
			// GL_MAX_SAMPLES_EXT может нести в себе общий макс. семпл, а не MSAA, если есть поддержка CSAA
			// смотрим если была доп проверка, и если в доп проверке значение меньше - берем его, оно более корректное
			if (OpenGL_DevCaps.MaxSamples > MaxMultiSampleTypeTest2) OpenGL_DevCaps.MaxSamples = MaxMultiSampleTypeTest2;

			delete [] coverageConfigs;
		}
	}

	// временно, потом переработать все проверки под версию GL
	if (!OpenGL_DevCaps.OpenGL_1_3_supported)
		OpenGL_DevCaps.MaxMultTextures = 1; // FIXME this should be revised

	if (!OpenGL_DevCaps.OpenGL_4_2_supported)
		OpenGL_DevCaps.TextureStorage = false; // FIXME this should be revised

	if (!OpenGL_DevCaps.OpenGL_2_0_supported)
		OpenGL_DevCaps.GLSL100Supported = false; // FIXME this should be revised

	if (!OpenGL_DevCaps.OpenGL_1_5_supported)
		OpenGL_DevCaps.VBOSupported = false; // FIXME this should be revised

	if (!OpenGL_DevCaps.OpenGL_3_0_supported) {
		OpenGL_DevCaps.VAOSupported = false; // FIXME this should be revised
		OpenGL_DevCaps.FramebufferObject = false; // FIXME this should be revised
	}

#ifndef NDEBUG
	// print all supported OpenGL extensions (one per line)
	if (glGetString(GL_EXTENSIONS) != nullptr) {
		std::string extensions{(char *)glGetString(GL_EXTENSIONS)};
		if (!extensions.empty()) {
			std::replace(extensions.begin(), extensions.end(), ' ', '\n'); // replace all ' ' to '\n'
			std::cout << "Supported OpenGL extensions:\n" << extensions.c_str() << "\n";
		}
	}
#endif // NDEBUG

	std::cout << "\n";

	return 0;
}






void vw_InitOpenGL(int Width, int Height, int *MSAA, int *CSAA)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка параметров прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_ResizeScene(fAngleGL, (Width*1.0f)/(Height*1.0f), fNearClipGL, fFarClipGL);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vw_SetClearColor(fClearRedGL, fClearGreenGL, fClearBlueGL, fClearAlphaGL);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_TEXTURE_2D);							//Enable two dimensional texture mapping
	glEnable(GL_DEPTH_TEST);							//Enable depth testing
	glShadeModel(GL_SMOOTH);							//Enable smooth shading (so you can't see the individual polygons of a primitive, best shown when drawing a sphere)
	glClearDepth(1.0);									//Depth buffer setup
	glClearStencil(0);
	glDepthFunc(GL_LEQUAL);								//The type of depth testing to do (LEQUAL==less than or equal to)
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//The nicest perspective look
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подключаем расширения
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (OpenGL_DevCaps.FramebufferObject) {
		// инициализируем буферы, если поддерживаем работу с ними - через них всегда рисуем
		if (OpenGL_DevCaps.FramebufferObject) {
			if ((!vw_BuildFBO(&MainFBO, Width, Height, true, true, *MSAA, CSAA)) &
			    (!vw_BuildFBO(&ResolveFBO, Width, Height, true, false))) {
				vw_DeleteFBO(&MainFBO);
				vw_DeleteFBO(&ResolveFBO);
				OpenGL_DevCaps.FramebufferObject = false;
			}
		}
	}


	// если с FBO работать не получилось
	if (!OpenGL_DevCaps.FramebufferObject) {
		//выключаем антиалиасинг
		*MSAA = OpenGL_DevCaps.MaxSamples = 0;

		// сбрасываем в нули структуры буферов (на всякий случай, т.к. не было инициализаций)
		MainFBO.ColorBuffer = 0;
		MainFBO.DepthBuffer = 0;
		MainFBO.ColorTexture = 0;
		MainFBO.DepthTexture = 0;
		MainFBO.FrameBufferObject = 0;
		ResolveFBO.ColorBuffer = 0;
		ResolveFBO.DepthBuffer = 0;
		ResolveFBO.ColorTexture = 0;
		ResolveFBO.DepthTexture = 0;
		ResolveFBO.FrameBufferObject = 0;
	}

}






//------------------------------------------------------------------------------------
// получение возможностей железа...
//------------------------------------------------------------------------------------
const sDevCaps *vw_GetDevCaps()
{
	return &OpenGL_DevCaps;
}

/*
 * Internal access to DevCaps, with write access.
 */
sDevCaps &__GetDevCaps()
{
	return OpenGL_DevCaps;
}










//------------------------------------------------------------------------------------
// Завершение работы с OpenGL'м
//------------------------------------------------------------------------------------
void vw_ShutdownRenderer()
{
	vw_ReleaseAllShaders();

	vw_DeleteFBO(&MainFBO);
	vw_DeleteFBO(&ResolveFBO);
}






//------------------------------------------------------------------------------------
// Изменение области вывода...
//------------------------------------------------------------------------------------
void vw_ResizeScene(float nfAngle, float AR, float nfNearClip, float nfFarClip)
{
	fAngleGL = nfAngle;

	if(nfNearClip==0 && nfFarClip==0)
		;
	else {
		fNearClipGL = nfNearClip;
		fFarClipGL = nfFarClip;
	}


	glMatrixMode(GL_PROJECTION);								//Select the projection matrix
	glLoadIdentity();											//Reset the projection matrix

	gluPerspective(fAngleGL, AR, fNearClipGL, fFarClipGL);


	glMatrixMode(GL_MODELVIEW);									//Select the modelview matrix
	glLoadIdentity();											//Reset The modelview matrix
}







//------------------------------------------------------------------------------------
// Изменение области вывода...
//------------------------------------------------------------------------------------
void vw_ChangeSize(int nWidth, int nHeight)
{

	vw_ResizeScene(fAngleGL, (nWidth*1.0f)/(nHeight*1.0f), fNearClipGL, fFarClipGL);

	// скорее всего сдвинули немного изображение, из-за разницы в осях с иксами
	int buff[4];
	glGetIntegerv(GL_VIEWPORT, buff);
	// перемещаем его вверх
	glViewport(buff[0], nHeight - buff[3], (GLsizei)buff[2], (GLsizei)buff[3]);

	ScreenWidthGL = nWidth;
	ScreenHeightGL = nHeight;
}




//------------------------------------------------------------------------------------
// очистка буфера
//------------------------------------------------------------------------------------
void vw_Clear(int mask)
{
	GLbitfield  glmask = 0;

	if (mask & 0x1000) glmask = glmask | GL_COLOR_BUFFER_BIT;
	if (mask & 0x0100) glmask = glmask | GL_DEPTH_BUFFER_BIT;
	if (mask & 0x0010) glmask = glmask | GL_ACCUM_BUFFER_BIT;
	if (mask & 0x0001) glmask = glmask | GL_STENCIL_BUFFER_BIT;

	glClear(glmask);
}





//------------------------------------------------------------------------------------
// начало прорисовки
//------------------------------------------------------------------------------------
void vw_BeginRendering(int mask)
{
	// если нужно, переключаемся на FBO
	vw_BindFBO(&MainFBO);

	vw_Clear(mask);

	glMatrixMode(GL_MODELVIEW);		//Select the modelview matrix
	glLoadIdentity();				//Reset The modelview matrix
}







//------------------------------------------------------------------------------------
// завершение прорисовки
//------------------------------------------------------------------------------------
void vw_EndRendering()
{
	// завершаем прорисовку, и переключаемся на основной буфер, если работали с FBO
	if (MainFBO.ColorTexture != 0) {
		// если у нас буфер простой - достаточно просто прорисовать его текстуру
		vw_DrawColorFBO(&MainFBO, nullptr);
	} else {
		// если буфер с мультисемплами, надо сначало сделать блит в простой буфер
		vw_BlitFBO(&MainFBO, &ResolveFBO);
		vw_DrawColorFBO(&ResolveFBO, nullptr);
	}

	SDL_GL_SwapWindow(window_SDL2);
}








//------------------------------------------------------------------------------------
// Установка Aspect Ratio
//------------------------------------------------------------------------------------
void vw_SetAspectRatio(float nWidth, float nHeight, bool Value)
{
	if (Value) {
		ARWidthGL = nWidth;
		ARHeightGL = nHeight;
		ARFLAGGL = true;
	} else
		ARFLAGGL=false;
}








//------------------------------------------------------------------------------------
// Получение данных aspect ratio
//------------------------------------------------------------------------------------
bool vw_GetAspectWH(float *ARWidth, float *ARHeight)
{
	if (ARWidth)
		*ARWidth = ARWidthGL;
	if (ARHeight)
		*ARHeight = ARHeightGL;
	return ARFLAGGL;
}









//------------------------------------------------------------------------------------
// Установка параметров вьюпорта
//------------------------------------------------------------------------------------
void vw_SetViewport(GLint x, GLint y, GLsizei width, GLsizei height, GLdouble near, GLdouble far, eOrigin Origin)
{
	if (Origin == eOrigin::upper_left)
		y = (GLint)(ScreenHeightGL - y - height);

	glViewport(x, y, width, height);
	glDepthRange(near, far);
}


//------------------------------------------------------------------------------------
// Получение параметров вьюпорта
//------------------------------------------------------------------------------------
void vw_GetViewport(int *x, int *y, int *width, int *height, float *znear, float *zfar)
{
	int buff[4];
	glGetIntegerv(GL_VIEWPORT, buff);

	if (x != nullptr)
		*x = buff[0];
	if (y != nullptr)
		*y = buff[1];
	if (width != nullptr)
		*width = buff[2];
	if (height != nullptr)
		*height = buff[3];

	float buff2[2];
	glGetFloatv(GL_DEPTH_RANGE, buff2);

	if (znear != nullptr)
		*znear = buff2[0];
	if (zfar != nullptr)
		*zfar = buff2[1];
}






//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_CullFace(int face)
{
	switch (face) {
	case RI_BACK:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	case RI_FRONT:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		break;
	case RI_NONE:
		glDisable(GL_CULL_FACE);
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong face.\n";
		break;
	}
}






//------------------------------------------------------------------------------------
// Polygon Offset
//------------------------------------------------------------------------------------
void vw_PolygonOffset(bool enable, float factor, float units)
{
	if (enable)	glEnable(GL_POLYGON_OFFSET_FILL);
	else glDisable(GL_POLYGON_OFFSET_FILL);

	glPolygonOffset(factor, units);
}







//------------------------------------------------------------------------------------
// Установка цвета очистки буфера
//------------------------------------------------------------------------------------
void vw_SetClearColor(float nRed, float nGreen, float nBlue, float nAlpha)
{
	fClearRedGL = nRed;
	fClearGreenGL = nGreen;
	fClearBlueGL = nBlue;
	fClearAlphaGL = nAlpha;
	glClearColor(nRed, nGreen, nBlue, nAlpha);
}





//------------------------------------------------------------------------------------
// Установка цвета
//------------------------------------------------------------------------------------
void vw_SetColor(float nRed, float nGreen, float nBlue, float nAlpha)
{
	glColor4f(nRed, nGreen, nBlue, nAlpha);
}




//------------------------------------------------------------------------------------
// Установка маски цвета
//------------------------------------------------------------------------------------
void vw_SetColorMask(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red ? GL_TRUE : GL_FALSE, green ? GL_TRUE : GL_FALSE, blue ? GL_TRUE : GL_FALSE, alpha ? GL_TRUE : GL_FALSE);
}




//------------------------------------------------------------------------------------
// Управление буфером глубины
//------------------------------------------------------------------------------------
void vw_DepthTest(bool mode, eCompareFunc func)
{
	if (mode) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
		// и сразу выходим, там тут делать нечего
		return;
	}

	glDepthFunc(static_cast<GLenum>(func));
}
