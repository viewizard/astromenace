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


#include "graphics.h"




//------------------------------------------------------------------------------------
// переменные...
//------------------------------------------------------------------------------------
eDevCaps OpenGL_DevCaps;

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

// переменные для просчета кол-ва прорисовываемых примитивов...
int tmpPrimCountGL=0;
int PrimCountGL=0;




// multitexture (OpenGL 1.3)
PFNGLACTIVETEXTUREARBPROC		glActiveTexture_ARB = nullptr;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTexture_ARB = nullptr;
// GL_ARB_texture_storage (OpenGL 4.2)
PFNGLTEXSTORAGE2DPROC 			glTexStorage2DEXT = nullptr;


SDL_Window *window_SDL2 = nullptr;
SDL_Window *vw_GetSDL2Windows()
{
	return window_SDL2;
};

float CurrentGammaGL = 1.0f;
float CurrentContrastGL = 1.0f;
float CurrentBrightnessGL = 1.0f;

float fScreenWidthGL = 0.0f;
float fScreenHeightGL = 0.0f;


// состояние устройства (гамма)
Uint16 UserDisplayRamp[256*3];
int UserDisplayRampStatus = -1; // не использовать, была ошибка при получении значения

// инициализация GLSL
bool vw_Internal_InitializationGLSL();
// инициализация VBO
bool vw_Internal_InitializationVBO();
// инициализация VAO
bool vw_Internal_InitializationVAO();
// индекс буфера
bool vw_Internal_InitializationIndexBufferData();
void vw_Internal_ReleaseIndexBufferData();
// FBO
bool vw_Internal_InitializationFBO();
eFBO MainFBO; // основной FBO, для прорисовки со сглаживанием
eFBO ResolveFBO; // FBO для вывода основного




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
	fScreenWidthGL = Width*1.0f;
	fScreenHeightGL = Height*1.0f;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем режим и делаем окно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Uint32 Flags = SDL_WINDOW_OPENGL;

	if (FullScreenFlag) Flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	window_SDL2 = SDL_CreateWindow(Title, CurrentVideoModeX, CurrentVideoModeY, Width, Height, Flags);
	if (window_SDL2 == nullptr) {
		fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
		fprintf(stderr, "Can't set video mode %i x %i\n\n", Width, Height);
		return 1;
	}
	SDL_GL_CreateContext(window_SDL2);
	SDL_GL_SetSwapInterval(VSync);
	SDL_DisableScreenSaver();

	UserDisplayRampStatus = SDL_GetWindowGammaRamp(window_SDL2, UserDisplayRamp, UserDisplayRamp+256, UserDisplayRamp+512);

	printf("Set video mode: %i x %i x %i\n\n", Width, Height, *Bits);


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


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем возможности железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	printf("Vendor     : %s\n", glGetString(GL_VENDOR));
	printf("Renderer   : %s\n", glGetString(GL_RENDERER));
	printf("Version    : %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGL_DevCaps.OpenGLmajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGL_DevCaps.OpenGLminorVersion);
	// после GL_MAJOR_VERSION и GL_MINOR_VERSION сбрасываем ошибку, т.к. тут можем получить
	// 0x500 GL_INVALID_ENUM
	glGetError();
	printf("OpenGL Version    : %i.%i\n", OpenGL_DevCaps.OpenGLmajorVersion, OpenGL_DevCaps.OpenGLminorVersion);
	printf("\n");
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL_DevCaps.MaxTextureHeight);
	printf("Max texture height: %i \n", OpenGL_DevCaps.MaxTextureHeight);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL_DevCaps.MaxTextureWidth);
	printf("Max texture width: %i \n", OpenGL_DevCaps.MaxTextureWidth);
	glGetIntegerv(GL_MAX_LIGHTS, &OpenGL_DevCaps.MaxActiveLights);
	printf("Max lights: %i \n", OpenGL_DevCaps.MaxActiveLights);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// дальше проверяем обязательные части, то, без чего вообще работать не будем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверяем поддержку мультитекстуры
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &OpenGL_DevCaps.MaxMultTextures);
	printf("Max multitexture supported: %i textures.\n", OpenGL_DevCaps.MaxMultTextures);
	// shader-based GL 2.0 and above programs should use GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS only


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим остальные поддерживаемые функции
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверем поддержку анизотропной фильтрации
	if (ExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
		// получим максимально доступный угол анизотропии...
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&OpenGL_DevCaps.MaxAnisotropyLevel);
		printf("Max anisotropy: %i\n", OpenGL_DevCaps.MaxAnisotropyLevel);
	}

	// проверем поддержку VBO
#ifdef use_Vertex_Buffer_Objects
	if (ExtensionSupported("GL_ARB_vertex_buffer_object")) {
		OpenGL_DevCaps.VBOSupported = true;
		printf("Vertex Buffer support enabled.\n");
	}
#endif

	// проверем поддержку VAO
#ifdef use_Vertex_Array_Objects
	if (ExtensionSupported("GL_ARB_vertex_array_object")) {
		OpenGL_DevCaps.VAOSupported = true;
		printf("Vertex Array support enabled.\n");
	}
#endif

	// проверем поддержку non_power_of_two генерацию текстур
#ifdef use_NPOT_Texture
	if (ExtensionSupported("GL_ARB_texture_non_power_of_two"))
		OpenGL_DevCaps.TextureNPOTSupported = true;
#endif

	// проверяем, есть ли поддержка компрессии текстур
	if (ExtensionSupported("GL_ARB_texture_compression") && ExtensionSupported("GL_EXT_texture_compression_s3tc")) {
		OpenGL_DevCaps.TexturesCompression = true;
		printf("Textures S3TC compression support enabled.\n");
	}
#ifdef use_BPTC_Texture_Compression
	if (ExtensionSupported("GL_ARB_texture_compression") && ExtensionSupported("GL_ARB_texture_compression_bptc")) {
		OpenGL_DevCaps.TexturesCompressionBPTC = true;
		printf("Textures BPTC compression support enabled.\n");
	}
#endif

	// проверяем, есть ли поддержка SGIS_generate_mipmap (хардварная генерация мипмеп уровней)
#ifdef use_Automatic_Mipmap_Generation
	if (ExtensionSupported("SGIS_generate_mipmap")) {
		OpenGL_DevCaps.HardwareMipMapGeneration = true;
	}
#endif

	// проверяем, есть ли поддержка GL_ARB_framebuffer_object (GL_EXT_framebuffer_object+GL_EXT_framebuffer_multisample+GL_EXT_framebuffer_blit)
#ifdef use_Framebuffer_Objects
	if (ExtensionSupported("GL_ARB_framebuffer_object") ||
	    (ExtensionSupported("GL_EXT_framebuffer_blit") && ExtensionSupported("GL_EXT_framebuffer_multisample") && ExtensionSupported("GL_EXT_framebuffer_object"))) {
		OpenGL_DevCaps.FramebufferObject = true;
		printf("Frame Buffer Object support enabled.\n");
	}
#endif

	// проверяем, есть ли поддержка GL_ARB_texture_storage или GL_EXT_texture_storage
#ifdef use_Texture_Storage
	if (ExtensionSupported("GL_ARB_texture_storage") || ExtensionSupported("GL_EXT_texture_storage")) {
		OpenGL_DevCaps.TextureStorage = true;
		printf("Texture Storage support enabled.\n");
	}
#endif


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// шейдеры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверяем, есть ли поддержка шейдеров GLSL версии 1.00
#ifdef use_Shading_Language
	if (ExtensionSupported("GL_ARB_shader_objects") &&
	    ExtensionSupported("GL_ARB_vertex_shader") &&
	    ExtensionSupported("GL_ARB_fragment_shader") &&
	    ExtensionSupported("GL_ARB_shading_language_100")) {
		OpenGL_DevCaps.GLSL100Supported = true;
	}
#endif

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
	if (OpenGL_DevCaps.ShaderModel == 0.0f) printf("Shaders unsupported.\n");
	else printf("Shader Model: %.1f\n", OpenGL_DevCaps.ShaderModel);


	// если есть полная поддержка FBO, значит можем работать с семплами
	if (OpenGL_DevCaps.FramebufferObject) {
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &OpenGL_DevCaps.MaxSamples);
		printf("Max Samples: %i\n", OpenGL_DevCaps.MaxSamples);

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
			printf("Max Multisample coverage modes: %i\n", OpenGL_DevCaps.MaxMultisampleCoverageModes);
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
					printf( " - %d MSAA\n", OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples);
					if (MaxMultiSampleTypeTest2 < OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples) MaxMultiSampleTypeTest2 = OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples;
				} else {
					// CSAA
					printf( " - %d/%d CSAA\n", OpenGL_DevCaps.MultisampleCoverageModes[kk].CoverageSamples, OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples);
				}
			}
			// GL_MAX_SAMPLES_EXT может нести в себе общий макс. семпл, а не MSAA, если есть поддержка CSAA
			// смотрим если была доп проверка, и если в доп проверке значение меньше - берем его, оно более корректное
			if (OpenGL_DevCaps.MaxSamples > MaxMultiSampleTypeTest2) OpenGL_DevCaps.MaxSamples = MaxMultiSampleTypeTest2;

			delete [] coverageConfigs;
		}
	}



#ifdef gamedebug
	// print all supported OpenGL extensions (one per line)
	if (glGetString(GL_EXTENSIONS) != nullptr) {
		std::string extensions{(char *)glGetString(GL_EXTENSIONS)};
		if (!extensions.empty()) {
			std::replace(extensions.begin(), extensions.end(), ' ', '\n'); // replace all ' ' to '\n'
			printf("Supported OpenGL extensions:\n%s\n", extensions.c_str());
		}
	}
#endif // gamedebug

	printf("\n");

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

	if (OpenGL_DevCaps.MaxMultTextures > 1) {
		glActiveTexture_ARB = (PFNGLACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glActiveTexture");
		glClientActiveTexture_ARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glClientActiveTexture");
		if (glActiveTexture_ARB == nullptr || glClientActiveTexture_ARB == nullptr) {
			OpenGL_DevCaps.MaxMultTextures = 1;
			fprintf(stderr, "Can't get proc address for glActiveTexture or glClientActiveTexture.\n\n");
		}
	}

	if (OpenGL_DevCaps.TextureStorage) {
		glTexStorage2DEXT = (PFNGLTEXSTORAGE2DPROC) SDL_GL_GetProcAddress("glTexStorage2D");
		if (glTexStorage2DEXT == nullptr) glTexStorage2DEXT = (PFNGLTEXSTORAGE2DPROC) SDL_GL_GetProcAddress("glTexStorage2DEXT");
		if (glTexStorage2DEXT == nullptr) {
			OpenGL_DevCaps.TextureStorage = false;
			fprintf(stderr, "Can't get proc address for glTexStorage2DEXT.\n\n");
		}
	}

	// инициализация индекс буфера
	vw_Internal_InitializationIndexBufferData();
	// иним шейдеры
	if (OpenGL_DevCaps.GLSL100Supported) OpenGL_DevCaps.GLSL100Supported = vw_Internal_InitializationGLSL();
	// иним вбо
	if (OpenGL_DevCaps.VBOSupported) OpenGL_DevCaps.VBOSupported = vw_Internal_InitializationVBO();
	// иним вaо
	if (OpenGL_DevCaps.VAOSupported) OpenGL_DevCaps.VAOSupported = vw_Internal_InitializationVAO();
	// инициализируем FBO
	if (OpenGL_DevCaps.FramebufferObject) {
		OpenGL_DevCaps.FramebufferObject = vw_Internal_InitializationFBO();

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
eDevCaps *vw_GetDevCaps()
{
	return &OpenGL_DevCaps;
}





//------------------------------------------------------------------------------------
// получение кол-ва прорисованных примитивов...
//------------------------------------------------------------------------------------
int vw_GetPrimCount(void)
{
	return PrimCountGL;
}






//------------------------------------------------------------------------------------
// Завершение работы с OpenGL'м
//------------------------------------------------------------------------------------
void vw_ShutdownRenderer()
{
	// возвращаем гамму и все такое...
	if (UserDisplayRampStatus != -1) {
		SDL_SetWindowGammaRamp(window_SDL2, UserDisplayRamp, UserDisplayRamp+256, UserDisplayRamp+512);
	} else {
		vw_SetGammaRamp(1.0f,1.0f,1.0f);
	}

	vw_ReleaseAllShaders();

	vw_Internal_ReleaseIndexBufferData();
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

	fScreenWidthGL = nWidth*1.0f;
	fScreenHeightGL = nHeight*1.0f;
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

	tmpPrimCountGL = 0;
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

	PrimCountGL = tmpPrimCountGL;
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
	*ARWidth = ARWidthGL;
	*ARHeight = ARHeightGL;
	return ARFLAGGL;
}











//------------------------------------------------------------------------------------
// установка гаммы...
//------------------------------------------------------------------------------------
void vw_SetGammaRamp(float Gamma, float Contrast, float Brightness)
{
	CurrentGammaGL = Gamma;
	CurrentContrastGL = Contrast;
	CurrentBrightnessGL = Brightness;

	Uint16 *ramp = new Uint16[256*3];

	float angle = CurrentContrastGL;
	float offset = (CurrentBrightnessGL-1)*256;
	for (int i = 0; i < 256; i++) {
		float k = i/256.0f;
		k = (float)pow(k, 1.f/CurrentGammaGL);
		k = k*256;
		float value = k*angle*256+offset*256;
		if (value > 65535)
			value = 65535;
		else if (value < 0)
			value = 0;

		ramp[i] = (Uint16) value;
		ramp[i+256] = (Uint16) value;
		ramp[i+512] = (Uint16) value;
	}

	SDL_SetWindowGammaRamp(window_SDL2, ramp, ramp+256, ramp+512);

	if (ramp != nullptr)
		delete [] ramp;
}






//------------------------------------------------------------------------------------
// получение гаммы...
//------------------------------------------------------------------------------------
void vw_GetGammaRamp(float *Gamma, float *Contrast, float *Brightness)
{
	*Gamma = CurrentGammaGL;
	*Contrast = CurrentContrastGL;
	*Brightness = CurrentBrightnessGL;
}








//------------------------------------------------------------------------------------
// Установка параметров вьюпорта
//------------------------------------------------------------------------------------
void vw_SetViewport(int x, int y, int width, int height, float znear, float zfar, int Corner)
{
	if (Corner == RI_UL_CORNER)
		y = (int)(fScreenHeightGL - y - height);

	glViewport(x, y, (GLsizei)width, (GLsizei)height);
	glDepthRange(znear, zfar);
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
void vw_PolygonMode(int mode)
{
	switch (mode) {
	case RI_POINT:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case RI_LINE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case RI_FILL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	default:
		fprintf(stderr, "Error in vw_PolygonMode function call, wrong mode.\n");
		break;
	}
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
		fprintf(stderr, "Error in vw_CullFace function call, wrong face.\n");
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
void vw_DepthTest(bool mode, int funct)
{
	if (mode) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
		// и сразу выходим, там тут делать нечего
		return;
	}

	if (funct>=1 && funct<=8) {
		GLenum fun = GL_NEVER;
		switch(funct) {
		case 1:
			fun = GL_NEVER;
			break;
		case 2:
			fun = GL_LESS;
			break;
		case 3:
			fun = GL_EQUAL;
			break;
		case 4:
			fun = GL_LEQUAL;
			break;
		case 5:
			fun = GL_GREATER;
			break;
		case 6:
			fun = GL_NOTEQUAL;
			break;
		case 7:
			fun = GL_GEQUAL;
			break;
		case 8:
			fun = GL_ALWAYS;
			break;
		default:
			fprintf(stderr, "Error in vw_DepthTest function call, wrong funct.\n");
			return;
		}
		glDepthFunc(fun);
	}
}
