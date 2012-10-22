/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_Main.cpp

	Copyright (c) 2003-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "RendererInterface.h"




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
PFNGLACTIVETEXTUREARBPROC		glActiveTexture_ARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTexture_ARB = NULL;
// GL_ARB_texture_storage (OpenGL 4.2)
PFNGLTEXSTORAGE2DPROC 			glTexStorage2DEXT = NULL;




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
// инициализация Occlusion Queries
bool vw_Internal_InitializationOcclusionQueries();
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
	if (strstr(extensions, Extension) != NULL) return true;
	return false;
}






//------------------------------------------------------------------------------------
// установка окна на середину
//------------------------------------------------------------------------------------
void CenterWindow(int CurrentVideoModeX, int CurrentVideoModeY, int CurrentVideoModeW, int CurrentVideoModeH)
{
	SDL_SysWMinfo info;

	SDL_VERSION(&info.version);
	if ( SDL_GetWMInfo(&info) > 0 )
	{
		#ifdef __unix
            if ( info.subsystem == SDL_SYSWM_X11 )
			{
				SDL_Surface *GameScreen = SDL_GetVideoSurface();
                info.info.x11.lock_func();
                int x = (CurrentVideoModeW - GameScreen->w)/2 + CurrentVideoModeX;
                int y = (CurrentVideoModeH - GameScreen->h)/2 + CurrentVideoModeY;
                XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
                info.info.x11.unlock_func();
            }
        #endif // unix
        #ifdef WIN32
            {
				RECT rc;
				HWND hwnd = info.window;
				GetWindowRect(hwnd, &rc);
				int x = (CurrentVideoModeW - (rc.right-rc.left))/2 + CurrentVideoModeX;
				int y = (CurrentVideoModeH - (rc.bottom-rc.top))/2 + CurrentVideoModeY;
				SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }
        #endif // WIN32
    }

	printf("SDL version: %i.%i.%i\n", info.version.major, info.version.minor, info.version.patch);
}















//------------------------------------------------------------------------------------
// инициализация окна приложения и получение возможностей железа
//------------------------------------------------------------------------------------
int vw_InitWindow(const char* Title, int Width, int Height, int *Bits, BOOL FullScreenFlag, int CurrentVideoModeX, int CurrentVideoModeY, int CurrentVideoModeW, int CurrentVideoModeH)
{
	// самым первым делом - запоминаем все
	UserDisplayRampStatus = SDL_GetGammaRamp(UserDisplayRamp, UserDisplayRamp+256, UserDisplayRamp+512);

	fScreenWidthGL = Width*1.0f;
	fScreenHeightGL = Height*1.0f;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем режим и делаем окно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Uint32 Flags = SDL_OPENGL;

	// если иним в первый раз, тут ноль и нужно взять что-то подходящее
	if (*Bits == 0) *Bits = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	if (*Bits < 8) *Bits = 8;
	int WBits = *Bits;

	if (FullScreenFlag)
	{
		Flags |= SDL_FULLSCREEN;
	}
	else
	{	// для окна не нужно ставить глубину принудительно
		WBits = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
		Flags |= SDL_ANYFORMAT; // чтобы для оконного взял лучший режим сам
	}

	// ставим двойную буферизацию (теоретически, и так ее должно брать)
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


	// создаем окно
	if (SDL_SetVideoMode(Width, Height, WBits, Flags)  == NULL)
	{
		fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
		fprintf(stderr, "Can't set video mode %i x %i x %i\n\n", Width, Height, WBits);
		return 1;
	}



	// центровка
	if (!FullScreenFlag) CenterWindow(CurrentVideoModeX, CurrentVideoModeY, CurrentVideoModeW, CurrentVideoModeH);
	// ставим название класса окна
	SDL_WM_SetCaption(Title, 0);


	// если полноэкранный режим, смотрим сколько действительно поставили bpp и запоминаем
	if (FullScreenFlag) *Bits = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

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
	OpenGL_DevCaps.VBOSupported = false;
	OpenGL_DevCaps.VAOSupported = false;
	OpenGL_DevCaps.TextureNPOTSupported = false;
	OpenGL_DevCaps.GLSL100Supported = false;
	OpenGL_DevCaps.ShaderModel = 0;
	OpenGL_DevCaps.OcclusionQuerySupported = false;
	OpenGL_DevCaps.HardwareMipMapGeneration = false;
	OpenGL_DevCaps.TextureStorage = false;
	OpenGL_DevCaps.FramebufferObject = false;


	printf("Video card hardware test.\n\n");

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем возможности железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	printf("Vendor     : %s\n", glGetString(GL_VENDOR));
	printf("Renderer   : %s\n", glGetString(GL_RENDERER));
	printf("Version    : %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGL_DevCaps.OpenGLmajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGL_DevCaps.OpenGLminorVersion);
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
	if (ExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
		// получим максимально доступный угол анизотропии...
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&OpenGL_DevCaps.MaxAnisotropyLevel);
		printf("Max anisotropy: %i\n", OpenGL_DevCaps.MaxAnisotropyLevel);
	}

	// проверем поддержку VBO
	if (ExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		OpenGL_DevCaps.VBOSupported = true;
		printf("Vertex Buffer support enabled.\n");
	}

	// проверем поддержку VAO
	if (ExtensionSupported("GL_ARB_vertex_array_object"))
	{
		OpenGL_DevCaps.VAOSupported = true;
		printf("Vertex Array support enabled.\n");
	}

	// проверем поддержку non_power_of_two генерацию текстур
	if (ExtensionSupported("GL_ARB_texture_non_power_of_two"))
		OpenGL_DevCaps.TextureNPOTSupported = true;

	// проверяем, есть ли поддержка компрессии текстур
	if (ExtensionSupported("GL_ARB_texture_compression") && ExtensionSupported("GL_EXT_texture_compression_s3tc"))
		OpenGL_DevCaps.TexturesCompression = true;

	// проверяем, есть ли поддержка OcclusionQuery
	if (ExtensionSupported("GL_ARB_occlusion_query"))
	{
		OpenGL_DevCaps.OcclusionQuerySupported = true;
		printf("Occlusion Query support enabled.\n");
	}

	// проверяем, есть ли поддержка SGIS_generate_mipmap (хардварная генерация мипмеп уровней)
	if (ExtensionSupported("SGIS_generate_mipmap"))
	{
		OpenGL_DevCaps.HardwareMipMapGeneration = true;
	}

	// проверяем, есть ли поддержка GL_ARB_framebuffer_object (GL_EXT_framebuffer_object+GL_EXT_framebuffer_multisample+GL_EXT_framebuffer_blit)
	if (ExtensionSupported("GL_ARB_framebuffer_object") |
		(ExtensionSupported("GL_EXT_framebuffer_blit") & ExtensionSupported("GL_EXT_framebuffer_multisample") & ExtensionSupported("GL_EXT_framebuffer_object")))
	{
		OpenGL_DevCaps.FramebufferObject = true;
		printf("Frame Buffer Object support enabled.\n");
	}

	// проверяем, есть ли поддержка GL_ARB_texture_storage или GL_EXT_texture_storage
	if (ExtensionSupported("GL_ARB_texture_storage") | ExtensionSupported("GL_EXT_texture_storage"))
	{
		OpenGL_DevCaps.TextureStorage = true;
		printf("Texture Storage support enabled.\n");
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// шейдеры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверяем, есть ли поддержка шейдеров GLSL версии 1.00
	if (ExtensionSupported("GL_ARB_shader_objects") &&
		ExtensionSupported("GL_ARB_vertex_shader") &&
		ExtensionSupported("GL_ARB_fragment_shader") &&
		ExtensionSupported("GL_ARB_shading_language_100"))
	{
		OpenGL_DevCaps.GLSL100Supported = true;
	}

	// определяем какую версию шейдеров поддерживаем в железе.
	// т.к. это может пригодиться для определения как работает GLSL,
	// ведь может работать даже с шейдерной моделью 2.0 (в обрезанном режиме), полному GLSL 1.0 нужна модель 3.0 или выше
	OpenGL_DevCaps.ShaderModel = 0.0f;
	// If a card supports GL_ARB_vertex_program and/or GL_ARB_vertex_shader, it supports vertex shader 1.1.
	// If a card supports GL_NV_texture_shader and GL_NV_register_combiners, it supports pixel shader 1.1.
	if ((ExtensionSupported("GL_ARB_vertex_program") || ExtensionSupported("GL_ARB_vertex_shader")) &&
			ExtensionSupported("GL_NV_texture_shader") && ExtensionSupported("GL_NV_register_combiners"))
	{
		OpenGL_DevCaps.ShaderModel = 1.1f;
	}
	// If a card supports GL_ATI_fragment_shader or GL_ATI_text_fragment_shader it supports pixel shader 1.4.
	if (ExtensionSupported("GL_ATI_fragment_shader") || ExtensionSupported("GL_ATI_text_fragment_shader"))
	{
		OpenGL_DevCaps.ShaderModel = 1.4f;
	}
	// If a card supports GL_ARB_fragment_program and/or GL_ARB_fragment_shader it supports Shader Model 2.0.
	if (ExtensionSupported("GL_ARB_fragment_program") || ExtensionSupported("GL_ARB_fragment_shader"))
	{
		OpenGL_DevCaps.ShaderModel = 2.0f;
	}
	// If a card supports GL_NV_vertex_program3 or GL_ATI_shader_texture_lod it it supports Shader Model 3.0.
	if (ExtensionSupported("GL_NV_vertex_program3") || ExtensionSupported("GL_ATI_shader_texture_lod"))
	{
		OpenGL_DevCaps.ShaderModel = 3.0f;
	}
	// If a card supports GL_EXT_gpu_shader4 it is a Shader Model 4.0 card. (Geometry shaders are implemented in GL_EXT_geometry_shader4)
	if (ExtensionSupported("GL_EXT_gpu_shader4"))
	{
		OpenGL_DevCaps.ShaderModel = 4.0f;
	}

	// проверяем, если версия опенжл выше 3.3, версия шейдеров им соответствует
	// (если мы не нашли более высокую через расширения ранее, ставим по версии опенжл)
	float OpenGLVersion = OpenGL_DevCaps.OpenGLmajorVersion;
	if (OpenGL_DevCaps.OpenGLminorVersion != 0) OpenGLVersion += OpenGL_DevCaps.OpenGLminorVersion/10.0f;
	if ((OpenGL_DevCaps.ShaderModel >= 3.0f) & (OpenGLVersion >= 3.3))
		if (OpenGL_DevCaps.ShaderModel < OpenGLVersion) OpenGL_DevCaps.ShaderModel = OpenGLVersion;

	// выводим эти данные
	if (OpenGL_DevCaps.ShaderModel == 0.0f) printf("Shaders unsupported.\n");
	else printf("Shader Model: %.1f\n", OpenGL_DevCaps.ShaderModel);


	// если есть полная поддержка FBO, значит можем работать с семплами
	if (OpenGL_DevCaps.FramebufferObject)
	{
		glGetIntegerv(GL_MAX_SAMPLES_EXT, &OpenGL_DevCaps.MaxSamples);
		printf("Max Samples: %i\n", OpenGL_DevCaps.MaxSamples);

		// дальше может и не быть GL_NV_framebuffer_multisample_coverage, потому делаем список сглаживаний
		int TestSample = 2;
		OpenGL_DevCaps.MaxMultisampleCoverageModes = 0;
		while (TestSample <= OpenGL_DevCaps.MaxSamples)
		{
			OpenGL_DevCaps.MultisampleCoverageModes[OpenGL_DevCaps.MaxMultisampleCoverageModes].ColorSamples = TestSample;
			OpenGL_DevCaps.MultisampleCoverageModes[OpenGL_DevCaps.MaxMultisampleCoverageModes].CoverageSamples = TestSample;
			OpenGL_DevCaps.MaxMultisampleCoverageModes++;
			TestSample = TestSample*2;
		}

		// проверяем, есть ли поддержка CSAA
		if (ExtensionSupported("GL_NV_framebuffer_multisample_coverage"))
		{
			glGetIntegerv( GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV, &OpenGL_DevCaps.MaxMultisampleCoverageModes);
			printf("Max Multisample coverage modes: %i\n", OpenGL_DevCaps.MaxMultisampleCoverageModes);
			int *coverageConfigs = 0;
			coverageConfigs = new int[OpenGL_DevCaps.MaxMultisampleCoverageModes * 2 + 4];
			glGetIntegerv( GL_MULTISAMPLE_COVERAGE_MODES_NV, coverageConfigs);

			// просматриваем все конфиги, печатаем их и делаем второй тест на MSAA
			int MaxMultiSampleTypeTest2 = -1;
			for (int kk = 0; kk < OpenGL_DevCaps.MaxMultisampleCoverageModes; kk++)
			{
				OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples = coverageConfigs[kk*2+1];
				OpenGL_DevCaps.MultisampleCoverageModes[kk].CoverageSamples = coverageConfigs[kk*2];

				if (OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples == OpenGL_DevCaps.MultisampleCoverageModes[kk].CoverageSamples)
				{
					// если ковередж и глубина/цвет одинаковые - это обычный MSAA
					printf( " - %d MSAA\n", OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples);
					if (MaxMultiSampleTypeTest2 < OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples) MaxMultiSampleTypeTest2 = OpenGL_DevCaps.MultisampleCoverageModes[kk].ColorSamples;
				}
				else
				{
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
	// получаем и выводим все поддерживаемые расширения
	char *extensions_tmp;
	size_t len;
	extensions_tmp = (char *)glGetString(GL_EXTENSIONS);
	if (extensions_tmp != 0)
	{
		char *extensions = 0;
		len = strlen(extensions_tmp);
		extensions = new char[len+1];
		if (extensions != 0)
		{
			strcpy(extensions, extensions_tmp);
			for (unsigned int i=0; i<len; i++) // меняем разделитель
				if (extensions[i]==' ') extensions[i]='\n';

			printf("Supported OpenGL extensions:\n%s\n", extensions);
			delete [] extensions;
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

	if (OpenGL_DevCaps.MaxMultTextures > 1)
	{
		glActiveTexture_ARB = (PFNGLACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glActiveTexture");
		glClientActiveTexture_ARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glClientActiveTexture");
		if (glActiveTexture_ARB == NULL || glClientActiveTexture_ARB == NULL)
		{
			OpenGL_DevCaps.MaxMultTextures = 1;
			fprintf(stderr, "Can't get proc address for glActiveTexture or glClientActiveTexture.\n\n");
		}
	}

	if (OpenGL_DevCaps.TextureStorage)
	{
		glTexStorage2DEXT = (PFNGLTEXSTORAGE2DPROC) SDL_GL_GetProcAddress("glTexStorage2D");
		if (glTexStorage2DEXT == NULL) glTexStorage2DEXT = (PFNGLTEXSTORAGE2DPROC) SDL_GL_GetProcAddress("glTexStorage2DEXT");
		if (glTexStorage2DEXT == NULL)
		{
			OpenGL_DevCaps.TextureStorage = false;
			fprintf(stderr, "Can't get proc address for glTexStorage2DEXT.\n\n");
		}
	}

	// инициализация индекс буфера
	vw_Internal_InitializationIndexBufferData();
	// иним шейдеры
	if (OpenGL_DevCaps.GLSL100Supported) OpenGL_DevCaps.GLSL100Supported = vw_Internal_InitializationGLSL();
	// иним оклюжен
	if (OpenGL_DevCaps.OcclusionQuerySupported) OpenGL_DevCaps.OcclusionQuerySupported = vw_Internal_InitializationOcclusionQueries();
	// иним вбо
	if (OpenGL_DevCaps.VBOSupported) OpenGL_DevCaps.VBOSupported = vw_Internal_InitializationVBO();
	// иним вaо
	if (OpenGL_DevCaps.VAOSupported) OpenGL_DevCaps.VAOSupported = vw_Internal_InitializationVAO();
	// инициализируем FBO
	if (OpenGL_DevCaps.FramebufferObject)
	{
		OpenGL_DevCaps.FramebufferObject = vw_Internal_InitializationFBO();

		// инициализируем буферы, если поддерживаем работу с ними - через них всегда рисуем
		if (OpenGL_DevCaps.FramebufferObject)
		{
			if (!vw_BuildFBO(&MainFBO, Width, Height, true, true, *MSAA, CSAA) &
				!vw_BuildFBO(&ResolveFBO, Width, Height, true, false))
			{
				vw_DeleteFBO(&MainFBO);
				vw_DeleteFBO(&ResolveFBO);
				OpenGL_DevCaps.FramebufferObject = false;
			}
		}
	}


	// если с FBO работать не получилось
	if (!OpenGL_DevCaps.FramebufferObject)
	{
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




	// выключаем вертикальную синхронизацию в винде, она тут не нужна, а нам нужно как можно больше фпс
#ifdef WIN32
	typedef void (APIENTRY * WGLSWAPINTERVALEXT) (int);
	WGLSWAPINTERVALEXT wglSwapIntervalEXT = (WGLSWAPINTERVALEXT)
	wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
	   wglSwapIntervalEXT(0); // disable vertical synchronisation
#endif // WIN32


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
	if (UserDisplayRampStatus != -1)
	{
		SDL_SetGammaRamp(UserDisplayRamp, UserDisplayRamp+256, UserDisplayRamp+512);
	}
	else
	{
		vw_SetGammaRamp(1.0f,1.0f,1.0f);
	}

	vw_ReleaseAllShaders();

    SDL_Surface *GameScreen = SDL_GetVideoSurface();
	if (GameScreen != NULL)
		SDL_FreeSurface(GameScreen);


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
	else
	{
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
	if (MainFBO.ColorTexture != 0)
	{
		// если у нас буфер простой - достаточно просто прорисовать его текстуру
		vw_DrawColorFBO(&MainFBO, 0);
	}
	else
	{
		// если буфер с мультисемплами, надо сначало сделать блит в простой буфер
		vw_BlitFBO(&MainFBO, &ResolveFBO);
		vw_DrawColorFBO(&ResolveFBO, 0);
	}

	SDL_GL_SwapBuffers();

	PrimCountGL = tmpPrimCountGL;
}








//------------------------------------------------------------------------------------
// Установка Aspect Ratio
//------------------------------------------------------------------------------------
void vw_SetAspectRatio(float nWidth, float nHeight, bool Value)
{
	if (Value)
	{
		ARWidthGL = nWidth;
		ARHeightGL = nHeight;
		ARFLAGGL = true;
	}
	else
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

	Uint16 *ramp = 0;
	ramp = new Uint16[256*3]; if (ramp == 0) return;

	float angle = CurrentContrastGL;
	float offset = (CurrentBrightnessGL-1)*256;
	for (int i = 0; i < 256; i++)
	{
		float k = i/256.0f;
		k = (float)pow(k, 1.f/CurrentGammaGL);
		k = k*256;
		float value = k*angle*256+offset*256;
		if (value > 65535)	value = 65535;
		if (value < 0)		value = 0;

		ramp[i]		= (Uint16) value;
		ramp[i+256]	= (Uint16) value;
		ramp[i+512]	= (Uint16) value;
	}

	SDL_SetGammaRamp(ramp, ramp+256, ramp+512);

	if (ramp != 0){delete [] ramp; ramp = 0;}
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
	if (x != 0) *x = buff[0];
	if (y != 0) *y = buff[1];
	if (width != 0) *width = buff[2];
	if (height != 0) *height = buff[3];
	float buff2[2];
	glGetFloatv(GL_DEPTH_RANGE, buff2);
	if (znear != 0) *znear = buff2[0];
	if (zfar != 0) *zfar = buff2[1];
}







//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_PolygonMode(int mode)
{
	switch (mode)
	{
		case RI_POINT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case RI_LINE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case RI_FILL:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
	}
}




//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_CullFace(int face)
{
	glEnable(GL_CULL_FACE);

	switch (face)
	{
		case RI_BACK:
			glCullFace(GL_BACK);
			break;
		case RI_FRONT:
			glCullFace(GL_FRONT);
			break;
		case RI_NONE:
			glDisable(GL_CULL_FACE);
			break;
	}
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
// Управление буфером глубины
//------------------------------------------------------------------------------------
void vw_DepthTest(bool mode, int funct)
{
	if (mode)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		// и сразу выходим, там тут делать нечего
		return;
	}

	if (funct>=1 && funct<=8)
	{
		GLenum fun = GL_NEVER;
		switch(funct)
		{
			case 1: fun = GL_NEVER; break;
			case 2: fun = GL_LESS; break;
			case 3: fun = GL_EQUAL; break;
			case 4: fun = GL_LEQUAL; break;
			case 5: fun = GL_GREATER; break;
			case 6: fun = GL_NOTEQUAL; break;
			case 7: fun = GL_GEQUAL; break;
			case 8: fun = GL_ALWAYS; break;
		}
		glDepthFunc(fun);
	}
}
