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
PFNGLACTIVETEXTUREPROC			glActiveTexture13	= NULL;
PFNGLCLIENTACTIVETEXTUREPROC	glClientActiveTexture13 = NULL;



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
// инициализация индекс буфера
bool vw_Internal_InitializationIndexBufferData();
// чистка данных индекс буфера
void vw_Internal_ReleaseIndexBufferData();







//------------------------------------------------------------------------------------
// проверка режимов антиалиасинга
//------------------------------------------------------------------------------------
void vw_TestAAModes(int Width, int Height)
{

		int AA[4] = {2, 4, 8, 16};
		int AAcount = 4;//sizeof (AA)/ sizeof (int);

		for (int i=AAcount-1; i!=0; i--)
		{
			SDL_Init(SDL_INIT_VIDEO);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, AA[i]);
			if (SDL_SetVideoMode(Width, Height, 0, SDL_OPENGL) != NULL)
			{
				int Buffers=0;
				int Samples=0;
				glGetIntegerv(GL_SAMPLE_BUFFERS_ARB, & Buffers);
				glGetIntegerv(GL_SAMPLES_ARB, & Samples);

				SDL_Surface *GameScreen = SDL_GetVideoSurface();
				if (GameScreen != NULL)
					SDL_FreeSurface(GameScreen);

				SDL_Quit();
				if (!Buffers || !Samples)
				{

				}
				else
				{
					OpenGL_DevCaps.MaxMultiSampleType = Samples;
					return;
				}
			}
			else
			{
				SDL_Surface *GameScreen = SDL_GetVideoSurface();
				if (GameScreen != NULL)
					SDL_FreeSurface(GameScreen);

				SDL_Quit();
			}
		}



}










//------------------------------------------------------------------------------------
// установка окна на середину
//------------------------------------------------------------------------------------
void CenterWindow()
{
    SDL_Surface *GameScreen = SDL_GetVideoSurface();
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version);
    if ( SDL_GetWMInfo(&info) > 0 )
	{
		#ifdef __unix
            if ( info.subsystem == SDL_SYSWM_X11 )
			{
                info.info.x11.lock_func();
                int w = DisplayWidth(info.info.x11.display,
                                 DefaultScreen(info.info.x11.display));
                int h = DisplayHeight(info.info.x11.display,
                                 DefaultScreen(info.info.x11.display));

				// пока нет возможности определить screens Xineramа-TwinView нормально через SDL, используем
				// костыль, который будет работать только если мониторы связаны в ширину (наиболее распространенный случай)
				if ( (float)w / (float)h >= 2.0f) w = w/2;

                int x = (w - GameScreen->w)/2;
                int y = (h - GameScreen->h)/2;
                XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
                info.info.x11.unlock_func();
            }
        #endif // unix
        #ifdef WIN32
            {
				RECT rc;
				HWND hwnd = info.window;
				int w=GetSystemMetrics(SM_CXSCREEN);
				int h=GetSystemMetrics(SM_CYSCREEN);
				GetWindowRect(hwnd, &rc);
				int x = (w - (rc.right-rc.left))/2;
				int y = (h - (rc.bottom-rc.top))/2;
				SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
            }
        #endif // WIN32
    }

	printf("SDL version: %i.%i.%i\n", info.version.major, info.version.minor, info.version.patch);
}





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
// инициализация Open_GL
//------------------------------------------------------------------------------------
int vw_InitRenderer(const char* Title, int Width, int Height, int *Bits, BOOL FullScreenFlag, int *FSAA)
{
	// самым первым делом - запоминаем все
	UserDisplayRampStatus = SDL_GetGammaRamp(UserDisplayRamp, UserDisplayRamp+256, UserDisplayRamp+512);

	if (*FSAA > OpenGL_DevCaps.MaxMultiSampleType) *FSAA = OpenGL_DevCaps.MaxMultiSampleType;


	fScreenWidthGL = Width*1.0f;
	fScreenHeightGL = Height*1.0f;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем режим и делаем окно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Uint32 Flags = SDL_OPENGL;


	// пока только для виндовса, только там у нас версия выше 1.2.10
#ifdef WIN32
	// требуем чтобы 100% было все в акселерации, иначе на шейдерах может перейти в софтовый режим
	Flags |= SDL_GL_ACCELERATED_VISUAL;
#endif

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


	// пытаемся поставить FSAA, тут все равно что сейчас ставим... потом разберемся
	if (*FSAA > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, *FSAA);
	}




	// создаем окно
	if (SDL_SetVideoMode(Width, Height, WBits, Flags)  == NULL)
	{
		fprintf(stderr, "Can't set video mode %i x %i x %i\n", Width, Height, WBits);
		return 1;
	}



	// центровка
	if (!FullScreenFlag) CenterWindow();
	// ставим название класса окна
	SDL_WM_SetCaption(Title, 0);


	// если полноэкранный режим, смотрим сколько действительно поставили bpp и запоминаем
	if (FullScreenFlag) *Bits = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

	printf("Set video mode: %i x %i x %i\n", Width, Height, *Bits);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка параметров прорисовки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (OpenGL_DevCaps.MaxMultiSampleType > 0) glEnable(GL_MULTISAMPLE);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vw_SetClearColor(fClearRedGL, fClearGreenGL, fClearBlueGL, fClearAlphaGL);
	glClearStencil(0);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_TEXTURE_2D);							//Enable two dimensional texture mapping
	glEnable(GL_DEPTH_TEST);							//Enable depth testing
	glShadeModel(GL_SMOOTH);							//Enable smooth shading (so you can't see the individual polygons of a primitive, best shown when drawing a sphere)
	glClearDepth(1.0);									//Depth buffer setup
	glDepthFunc(GL_LEQUAL);								//The type of depth testing to do (LEQUAL==less than or equal to)
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//The nicest perspective look
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);


	vw_ResizeScene(fAngleGL, (Width*1.0f)/(Height*1.0f), fNearClipGL, fFarClipGL);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем возможности железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	OpenGL_DevCaps.VidMemTotal = SDL_GetVideoInfo()->video_mem;
	printf("\n");
	printf("Vendor     : %s\n", glGetString(GL_VENDOR));
	printf("Renderer   : %s\n", glGetString(GL_RENDERER));
	printf("Version    : %s\n", glGetString(GL_VERSION));
	printf("Video RAM  : %i\n", OpenGL_DevCaps.VidMemTotal);
	printf("\n");
	OpenGL_DevCaps.MaxTextureHeight = 256;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL_DevCaps.MaxTextureHeight);
	printf("Max texture height: %i \n", OpenGL_DevCaps.MaxTextureHeight);
	OpenGL_DevCaps.MaxTextureWidth = 256;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL_DevCaps.MaxTextureWidth);
	printf("Max texture width: %i \n", OpenGL_DevCaps.MaxTextureWidth);
	glGetIntegerv(GL_MAX_LIGHTS, &OpenGL_DevCaps.MaxActiveLights);
	printf("Max lights: %i \n", OpenGL_DevCaps.MaxActiveLights);
	printf("Set FSAA: %i \n", *FSAA);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// дальше проверяем обязательные части, то, без чего вообще работать не будем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// Берем расширения OpenGL 1.3

	// проверяем поддержку мультитекстуры
	OpenGL_DevCaps.MaxMultTextures = 1;
	glActiveTexture13 = (PFNGLACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glActiveTexture");
	glClientActiveTexture13 = (PFNGLCLIENTACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glClientActiveTexture");
	if (glActiveTexture13 == NULL || glClientActiveTexture13 == NULL)
	{
		fprintf(stderr, "Can't get proc address for glActiveTexture or glClientActiveTexture.\n");
		return 2;
	}
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &OpenGL_DevCaps.MaxMultTextures);
	printf("Max multitexture supported: %i textures.\n", OpenGL_DevCaps.MaxMultTextures);





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// смотрим остальные поддерживаемые функции
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// по умолчанию, внутренний менеджер выключен
	OpenGL_DevCaps.ForceTexturesPriorManager = false;

	// проверем поддержку анизотропной фильтрации
	OpenGL_DevCaps.MaxAnisotropyLevel = 1;
	if (ExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
		// получим максимально доступный угол анизотропии...
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&OpenGL_DevCaps.MaxAnisotropyLevel);
		printf("Max anisotropy: %i\n", OpenGL_DevCaps.MaxAnisotropyLevel);
	}

	// проверем поддержку VBO
	OpenGL_DevCaps.VBOSupported = false;
	if (ExtensionSupported("GL_ARB_vertex_buffer_object"))
	{
		OpenGL_DevCaps.VBOSupported = vw_Internal_InitializationVBO();
	}

	// проверем поддержку non_power_of_two генерацию текстур
	OpenGL_DevCaps.TextureNPOTSupported = false;
	if (ExtensionSupported("GL_ARB_texture_non_power_of_two"))
		OpenGL_DevCaps.TextureNPOTSupported = true;

	// проверяем, есть ли поддержка компрессии текстур
	OpenGL_DevCaps.TexturesCompression = false;
	if (ExtensionSupported("GL_ARB_texture_compression") && ExtensionSupported("GL_EXT_texture_compression_s3tc"))
		OpenGL_DevCaps.TexturesCompression = true;

	// проверяем, есть ли поддержка OcclusionQuery
	OpenGL_DevCaps.OcclusionQuerySupported = false;
	if (ExtensionSupported("GL_ARB_occlusion_query"))
	{
		OpenGL_DevCaps.OcclusionQuerySupported = vw_Internal_InitializationOcclusionQueries();
	}

	// проверяем, есть ли поддержка SGIS_generate_mipmap (хардварная генерация мипмеп уровней)
	OpenGL_DevCaps.HardwareMipMapGeneration = false;
	if (ExtensionSupported("SGIS_generate_mipmap"))
	{
		OpenGL_DevCaps.HardwareMipMapGeneration = true;
	}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// шейдеры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверяем, есть ли поддержка шейдеров GLSL версии 1.00
	OpenGL_DevCaps.GLSL100Supported = false;
	if (ExtensionSupported("GL_ARB_shader_objects") &&
		ExtensionSupported("GL_ARB_vertex_shader") &&
		ExtensionSupported("GL_ARB_fragment_shader") &&
		ExtensionSupported("GL_ARB_shading_language_100"))
	{
		OpenGL_DevCaps.GLSL100Supported = vw_Internal_InitializationGLSL();
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
	// выводим эти данные
	if (OpenGL_DevCaps.ShaderModel == 0.0f) printf("Shaders unsupported.\n");
	else printf("Shader Model: %.1f\n", OpenGL_DevCaps.ShaderModel);
	printf("\n");








	// получаем и выводим все поддерживаемые расширения
	char *extensions_tmp;
	int i;
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
			for (i=0; i<len; i++) // меняем разделитель
				if (extensions[i]==' ') extensions[i]='\n';

			printf("Supported OpenGL extensions:\n%s\n", extensions);
			delete [] extensions;
		}
	}
	printf("\n");


	// инициализация индекс буфера
	vw_Internal_InitializationIndexBufferData();



	// выключаем вертикальную синхронизацию в винде, она тут не нужна, а нам нужно как можно
	// больше фпс
#ifdef WIN32
	typedef void (APIENTRY * WGLSWAPINTERVALEXT) (int);
	WGLSWAPINTERVALEXT wglSwapIntervalEXT = (WGLSWAPINTERVALEXT)
	wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
	   wglSwapIntervalEXT(0); // disable vertical synchronisation
#endif // WIN32


	return 0;
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
// начало прорисовки
//------------------------------------------------------------------------------------
void vw_BeginRendering()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);		//Select the modelview matrix
	glLoadIdentity();				//Reset The modelview matrix

	tmpPrimCountGL = 0;
}







//------------------------------------------------------------------------------------
// завершение прорисовки
//------------------------------------------------------------------------------------
void vw_EndRendering()
{
	SDL_GL_SwapBuffers();

	PrimCountGL = tmpPrimCountGL;

	// проверка приоритета, включаем если нужно, обычно это уже есть в драйверах
	// в висте очень сильно снижает быстродействие
	// видимо конфликтует с менеджером приоритетов текстур в драйверах
	if (OpenGL_DevCaps.ForceTexturesPriorManager) vw_CheckTexturesPrior();
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
		if (!glIsEnabled(GL_DEPTH_TEST)) glEnable(GL_DEPTH_TEST);
	}
	else
	{
		if (glIsEnabled(GL_DEPTH_TEST)) glDisable(GL_DEPTH_TEST);
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
