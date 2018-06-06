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

// TODO translate comments

// TODO check fullscreen modes with SDL_GetDisplayBounds() and windowed modes with SDL_GetDisplayUsableBounds()

// TODO probably, we could provide option for screen number, where game's window should
//      be created, see ScreenNumber

#include "game.h"
#include "config/config.h"
#include "ui/font.h"
#include "gfx/shadow_map.h"
#include "platform/platform.h"
#include "object3d/object3d.h"
#include <sys/stat.h> // stat


#if defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <pwd.h>
#endif // unix




//------------------------------------------------------------------------------------
// общие состояния и статусы
//------------------------------------------------------------------------------------
// текущий статус (текущее меню)
eMenuStatus MenuStatus;
// защелка на выход, когда нужно перегрузить, а когда просто поменять режим
bool Quit = false;
bool NeedReCreate = false;
// выходим или нет
bool CanQuit = true;
// для выбора в списке режимов разрешения
sVideoModes *VideoModes = nullptr;
int VideoModesNum = 0;
// текущие параметры десктопа
sVideoModes CurrentVideoMode;




//------------------------------------------------------------------------------------
// управление
//------------------------------------------------------------------------------------
// состояние кнопок мышки
bool SDL_MouseCurrentStatus[8];



//------------------------------------------------------------------------------------
// камера
//------------------------------------------------------------------------------------
// ближайшая точка, основная точка отсчета локальной системы координат игры
sVECTOR3D GamePoint(0.0f, 0.0f, 0.0f);
// направление движения камеры
sVECTOR3D GameCameraMovement(0.0f, 0.0f, 1.0f);





//------------------------------------------------------------------------------------
// основная процедура...
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	bool NeedShowSystemCursor{false};
	bool NeedResetConfig{false};
	bool NeedPack{false};

	// don't use getopt_long() here, since it could be not available (MSVC)
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--help")) {
			std::cout << "AstroMenace launch options:\n\n"
				  << "--dir=/folder - folder with gamedata.vfs file;\n"
				  << "--rawdata=/folder - folder with raw data for gamedata.vfs;\n"
				  << "--pack - pack data to gamedata.vfs file;\n"
				  << "--mouse - launch the game without system cursor hiding;\n"
				  << "--reset-config - reset all settings except Pilots Profiles;\n"
				  << "--help - info about all game launch options.\n";
			return 0;
		}

		if (!strncmp(argv[i], "--dir", strlen("--dir")))
			SetDataPathByParameter(argv[i], "--dir=");

		if (!strncmp(argv[i], "--rawdata", strlen("--rawdata")))
			SetRawDataPathByParameter(argv[i], "--rawdata=");

		if (!strcmp(argv[i], "--pack"))
			NeedPack = true;

		if (!strcmp(argv[i], "--mouse"))
			NeedShowSystemCursor = true;

		if (!strcmp(argv[i], "--reset-config"))
			NeedResetConfig = true;
	}

	// should be initialized before any interaction with SDL functions
	// for predictable SDL functions work on all platforms
	Uint32 SDL_Init_Flags = SDL_INIT_TIMER |
				SDL_INIT_EVENTS |
				SDL_INIT_JOYSTICK |
				SDL_INIT_VIDEO;

	// no reason init all around for vfs creation only
	if (NeedPack)
		SDL_Init_Flags = SDL_INIT_TIMER | SDL_INIT_EVENTS;

	if (SDL_Init(SDL_Init_Flags) != 0) {
		std::cerr << __func__ << "(): " << "Couldn't init SDL: " << SDL_GetError() << "\n";
		return 1;
	}

	std::cout << "AstroMenace " << GAME_VERSION << "\n";
	std::cout << "VFS version " << GAME_VFS_BUILD << "\n\n";

	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	std::cout << "Compiled against SDL version "
		  << static_cast<int>(compiled.major) << "."
		  << static_cast<int>(compiled.minor) << "."
		  << static_cast<int>(compiled.patch)
		  << "\n";
	std::cout << "Linking against SDL version "
		  << static_cast<int>(linked.major) << "."
		  << static_cast<int>(linked.minor) << "."
		  << static_cast<int>(linked.patch)
		  << "\n";

	if (NeedPack) {
		int rc = ConvertFS2VFS(GetRawDataPath(), GetDataPath() + "gamedata.vfs");
		SDL_Quit();
		return rc;
	}

	if (vw_OpenVFS(GetDataPath() + "gamedata.vfs", GAME_VFS_BUILD) != 0) {
		std::cerr << __func__ << "(): " << "gamedata.vfs file not found or corrupted.\n";
		SDL_Quit();
		return 1;
	}

	if (vw_InitText("lang/text.csv", ';', '\n') != 0) {
		std::cout << "lang/text.csv file not found or corrupted.\n";
		// if file not loaded - it's ok, we will work with English only
	}



	// работа с файлом данных... передаем базовый режим окна (обязательно после инициализации языков!)
	bool FirstStart = LoadXMLConfigFile(NeedResetConfig);

	if (GameConfig().FontNumber > FontQuantity)
		ChangeGameConfig().FontNumber = 0;

	// иним фонт
	InitFont(FontList[GameConfig().FontNumber].FontFileName);

	vw_InitTimeThread(0);

	JoystickInit(vw_GetTimeThread(0)); // should be after vw_InitTimeThread(0)

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка звука, всегда до LoadGameData
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!vw_InitAudio())
		std::cerr << __func__ << "(): " << "Unable to open audio.\n\n";





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем текущее разрешение экрана
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// предварительно ставим по умолчанию
	CurrentVideoMode.W = 1024;
	CurrentVideoMode.H = 768;
	CurrentVideoMode.BPP = 16;
	// пытаемся получить данные
	int ScreenNumber = 0; // should be less than SDL_GetNumVideoDisplays()
	SDL_DisplayMode CurrentDisplayMode;
	SDL_GetDesktopDisplayMode(ScreenNumber, &CurrentDisplayMode);
	CurrentVideoMode.BPP = SDL_BITSPERPIXEL(CurrentDisplayMode.format);
	CurrentVideoMode.W = CurrentDisplayMode.w;
	CurrentVideoMode.H = CurrentDisplayMode.h;
	std::cout << "Current Video Mode: "
		  << CurrentVideoMode.W << "x" << CurrentVideoMode.H << " "
		  << CurrentVideoMode.BPP << "bit \n";

	std::cout << "Screen count: " << SDL_GetNumVideoDisplays() << "\n";
	for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
		SDL_DisplayMode testDisplayMode;
		SDL_GetDesktopDisplayMode(i, &testDisplayMode);
		std::cout << "Screen #" << i << ": " << testDisplayMode.w << " x " << testDisplayMode.h << "\n";
	}





	// детектим и составляем перечень всех возможных разрешений самостоятельно
	sVideoModes AllSupportedModes[] = {
		{640, 480, -1},
		{768, 480, -1},
		{800, 480, -1},
		{800, 600, -1},
		{832, 624, -1},
		{854, 480, -1},
		{960, 540, -1},
		{960, 544, -1},
		{960, 640, -1},
		{960, 720, -1},
		{1024, 576, -1},
		{1024, 600, -1},
		{1024, 640, -1},
		{1024, 768, -1},
		{1024, 800, -1},
		{1120, 832, -1},
		{1152, 720, -1},
		{1152, 768, -1},
		{1152, 864, -1},
		{1152, 900, -1},
		{1280, 720, -1},
		{1280, 768, -1},
		{1280, 800, -1},
		{1280, 854, -1},
		{1280, 960, -1},
		{1280, 1024, -1},
		{1366, 768, -1},
		{1400, 1050, -1},
		{1440, 900, -1},
		{1440, 960, -1},
		{1440, 1024, -1},
		{1440, 1080, -1},
		{1600, 900, -1},
		{1600, 1024, -1},
		{1600, 1200, -1},
		{1680, 1050, -1},
		{1792, 1344, -1},
		{1856, 1392, -1},
		{1800, 1440, -1},
		{1920, 1080, -1},
		{1920, 1200, -1},
		{1920, 1400, -1},
		{1920, 1440, -1},
		{2048, 1280, -1},
		{2048, 1536, -1},
		{2304, 1440, -1},
		{2304, 1728, -1},
		{2560, 1440, -1},
		{2560, 1600, -1},
		{2560, 1920, -1},
		{2560, 2048, -1},
		{2800, 2100, -1},
		{2880, 1800, -1},
		{3200, 2048, -1},
		{3200, 2400, -1},
		{3840, 2160, -1},
		{3840, 2400, -1},
		{4096, 2304, -1},
		{4096, 3072, -1},
		{5120, 3200, -1},
		{5120, 4096, -1},
		{6400, 4096, -1},
		{6400, 4800, -1},
		{7680, 4320, -1},
		{7680, 4800, -1},
	};
#define AllSupportedModesCount sizeof(AllSupportedModes)/sizeof(AllSupportedModes[0])

	// если списка еще нет - создаем его
	if (VideoModes == nullptr) {
		VideoModesNum = 0;

		for(unsigned int i = 0; i < AllSupportedModesCount; i++)
			if ((AllSupportedModes[i].W <= CurrentVideoMode.W) &&
			    (AllSupportedModes[i].H <= CurrentVideoMode.H)) {
				// если мы подходим по размерам экрана для оконного режима - просто добавляем его
				VideoModesNum++;

				// пропускаем разрешение с одинаковыми параметрами и разной частотой
				int SkipPrevH = -1;
				int SkipPrevW = -1;
				// добавляем к полноэкранным режимам, если входит в список
				for (int j = 0; j < SDL_GetNumDisplayModes(ScreenNumber); j++) {
					SDL_DisplayMode testDisplayMode;
					SDL_GetDisplayMode(ScreenNumber, j, &testDisplayMode);
					if (SkipPrevH == -1) {
						SkipPrevH = testDisplayMode.h;
						SkipPrevW = testDisplayMode.w;
						if ((testDisplayMode.w == AllSupportedModes[i].W) &&
						    (testDisplayMode.h == AllSupportedModes[i].H))
							VideoModesNum++;
					} else {
						if ((SkipPrevH != testDisplayMode.h) ||
						    (SkipPrevW != testDisplayMode.w)) {
							if ((testDisplayMode.w == AllSupportedModes[i].W) &&
							    (testDisplayMode.h == AllSupportedModes[i].H))
								VideoModesNum++;
							SkipPrevH = testDisplayMode.h;
							SkipPrevW = testDisplayMode.w;
						}
					}
				}
			}


		VideoModes = new sVideoModes[VideoModesNum];

		int k=0;
		for(unsigned int i = 0; i < AllSupportedModesCount; i++)
			if ((AllSupportedModes[i].W <= CurrentVideoMode.W) &&
			    (AllSupportedModes[i].H <= CurrentVideoMode.H)) {
				// если мы подходим по размерам экрана для оконного режима - просто добавляем его
				VideoModes[k].W = AllSupportedModes[i].W;
				VideoModes[k].H = AllSupportedModes[i].H;
				VideoModes[k].BPP = 0;
				k++;

				// пропускаем разрешение с одинаковыми параметрами и разной частотой
				int SkipPrevH = -1;
				int SkipPrevW = -1;
				// добавляем к полноэкранным режимам, если входит в список
				for (int j = 0; j < SDL_GetNumDisplayModes(ScreenNumber); j++) {
					SDL_DisplayMode testDisplayMode;
					SDL_GetDisplayMode(ScreenNumber, j, &testDisplayMode);
					if (SkipPrevH == -1) {
						SkipPrevH = testDisplayMode.h;
						SkipPrevW = testDisplayMode.w;
						if ((testDisplayMode.w == AllSupportedModes[i].W) &&
						    (testDisplayMode.h == AllSupportedModes[i].H)) {
							VideoModes[k].W = AllSupportedModes[i].W;
							VideoModes[k].H = AllSupportedModes[i].H;
							VideoModes[k].BPP = CurrentVideoMode.BPP;
							k++;
						}
					} else {
						if ((SkipPrevH != testDisplayMode.h) ||
						    (SkipPrevW != testDisplayMode.w)) {
							if ((testDisplayMode.w == AllSupportedModes[i].W) &&
							    (testDisplayMode.h == AllSupportedModes[i].H)) {
								VideoModes[k].W = AllSupportedModes[i].W;
								VideoModes[k].H = AllSupportedModes[i].H;
								VideoModes[k].BPP = CurrentVideoMode.BPP;
								k++;
							}
							SkipPrevH = testDisplayMode.h;
							SkipPrevW = testDisplayMode.w;
						}
					}
				}
			}
		// выводим список поддерживаемых разрешений
		std::cout << "\nSupported resolutions list:\n";
		for(int i = 0; i < VideoModesNum; i++) {
			std::cout << VideoModes[i].W << " " << VideoModes[i].H << " " << VideoModes[i].BPP << "bit \n";
		}
		std::cout << "\n";

	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем и корректируем текущие настройки окна
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (FirstStart) {
		ChangeGameConfig().Width = CurrentVideoMode.W;
		ChangeGameConfig().Height = CurrentVideoMode.H;
		ChangeGameConfig().BPP = CurrentVideoMode.BPP;
	}
	// если загруженные параметры, больше чем максимальные, ставим максимальные (если Xinerama, например)
	if ((VideoModes[VideoModesNum-1].W < GameConfig().Width) ||
	    (VideoModes[VideoModesNum-1].H < GameConfig().Height)) {
		ChangeGameConfig().Width = VideoModes[VideoModesNum-1].W;
		ChangeGameConfig().Height = VideoModes[VideoModesNum-1].H;
		ChangeGameConfig().BPP = CurrentVideoMode.BPP;
	}
	// делаем проверку по листу разрешений экрана, если входит - все ок, если нет - ставим оконный режим принудительно
	bool NeedResetToWindowedMode = true;
	for(int i = 0; i < VideoModesNum; i++) {
		if ((VideoModes[i].W == GameConfig().Width) &&
		    (VideoModes[i].H == GameConfig().Height) &&
		    (VideoModes[i].BPP == GameConfig().BPP)) {
			NeedResetToWindowedMode = false;
			break;
		}
	}
	if (NeedResetToWindowedMode)
		ChangeGameConfig().BPP = 0;








	// устанавливаем язык текста
	vw_SetTextLanguage(GameConfig().MenuLanguage);






ReCreate:

	// if we change options during game mission with game restart, care about dialogs reset
	InitDialogBoxes();





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// создаем окно и базовые опенжл контекст
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool Fullscreen = (GameConfig().BPP != 0);

	if (!vw_CreateWindow("AstroMenace", GameConfig().Width, GameConfig().Height,
			     &ChangeGameConfig().BPP, Fullscreen, ScreenNumber)) {
		// не можем создать окно или включить полноэкранный режим - ставим минимальный оконный режим
		if ((640 != GameConfig().Width) ||
		    (480 != GameConfig().Height) ||
		    (0 != GameConfig().BPP)) {
			ChangeGameConfig().Width = 640;
			ChangeGameConfig().Height = 480;
			ChangeGameConfig().BPP = 0;
			ChangeGameConfig().InternalWidth = 1024.0f;
			ChangeGameConfig().InternalHeight = 768.0f;
			ChangeGameConfig().MSAA = 0;
			ChangeGameConfig().CSAA = 0;
			SaveXMLConfigFile();
			FirstStart = false;
			goto ReCreate;
		}

		std::cerr << __func__ << "(): " << "Wrong resolution. Fatal error.\n";
		if (VideoModes != nullptr) {
			delete [] VideoModes;
			VideoModes = nullptr;
		}
		vw_ShutdownFont();
		vw_ReleaseText();
		vw_ShutdownAudio();
		vw_ShutdownVFS();
		JoystickClose();
		vw_ReleaseAllTimeThread();
		SDL_Quit();
		return 1;
	}
	if (!Fullscreen)
		ChangeGameConfig().BPP = 0;


	if (!vw_CreateOpenGLContext(GameConfig().VSync)) {
		if (VideoModes != nullptr) {
			delete [] VideoModes;
			VideoModes = nullptr;
		}
		vw_ShutdownFont();
		vw_ReleaseText();
		vw_ShutdownAudio();
		vw_ShutdownVFS();
		JoystickClose();
		vw_ReleaseAllTimeThread();
		SDL_Quit();
		return 1;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем возможности железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// проверка поддержки шейдеров
	// т.к. у нас шейдеры версии 120, проверяем версию OpenGL 2.0 и 2.1
	if (GameConfig().UseGLSL120 &&
	    (!vw_GetDevCaps().OpenGL_2_0_supported || !vw_GetDevCaps().OpenGL_2_1_supported))
		ChangeGameConfig().UseGLSL120 = false;

	// анализ системы только если это первый запуск
	if (FirstStart) {
		// если железо поддерживает OpenGL 3.0
		if (vw_GetDevCaps().OpenGL_3_0_supported) {
			// 100% держит наши шейдеры
			ChangeGameConfig().UseGLSL120 = true;
			ChangeGameConfig().ShadowMap = 1;
			// немного больше ставим другие опции
			ChangeGameConfig().MSAA = 2;
			ChangeGameConfig().CSAA = 2;
			ChangeGameConfig().AnisotropyLevel = vw_GetDevCaps().MaxAnisotropyLevel;
			ChangeGameConfig().MaxPointLights = 4;
		}
		// если железо поддерживает OpenGL 4.2
		if (vw_GetDevCaps().OpenGL_4_2_supported) {
			// немного больше ставим другие опции
			ChangeGameConfig().ShadowMap = 1;
			ChangeGameConfig().MSAA = 4;
			ChangeGameConfig().CSAA = 4;
			ChangeGameConfig().MaxPointLights = 6;
		}

		// устанавливаем соотношение сторон по установленному разрешению экрана
		if ((GameConfig().Width * 1.0f) / (GameConfig().Height * 1.0f) < 1.4f) {
			ChangeGameConfig().InternalWidth = 1024.0f;
			ChangeGameConfig().InternalHeight = 768.0f;
		} else {
			ChangeGameConfig().InternalWidth = 1228.0f;
			ChangeGameConfig().InternalHeight = 768.0f;
		}
	}

	// если не поддерживает железо фбо или шейдеры, выключаем шадовмеп
	if (!vw_GetDevCaps().OpenGL_2_0_supported ||
	    !vw_GetDevCaps().OpenGL_2_1_supported ||
	    !vw_GetDevCaps().OpenGL_3_0_supported ||
	    (vw_GetDevCaps().MaxTextureWidth < 2048))
		ChangeGameConfig().ShadowMap = 0;

	// check MSAA/CSAA mode
	bool FoundAAMode{false};
	for (auto &Sample : vw_GetDevCaps().MultisampleCoverageModes) {
		if ((GameConfig().MSAA == Sample.ColorSamples) &&
		    (GameConfig().CSAA == Sample.CoverageSamples)) {
			FoundAAMode = true;
			break;
		}
	}
	if (!FoundAAMode) {
		ChangeGameConfig().MSAA = 0;
		ChangeGameConfig().CSAA = 0;
	}







	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершаем инициализацию
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_InitOpenGLStuff(GameConfig().Width, GameConfig().Height,
			   &ChangeGameConfig().MSAA, &ChangeGameConfig().CSAA);


	// вторичная работа с настройками
	// в процессе инициализации opengl контекста мы подключаем указатели на функции, и данные могут измениться


	// hardware must support multtextures
	if (!vw_GetDevCaps().OpenGL_1_3_supported) {
		std::cerr << __func__ << "(): " << "The Multi Textures feature not supported by hardware. Fatal error.\n";
#ifdef WIN32
		MessageBox(nullptr, "OpenGL 1.3 required. Please, install the newest video drivers from your video card vendor.",
			   "Render system - Fatal Error", MB_OK | MB_APPLMODAL | MB_ICONERROR);
#endif // WIN32
		if (VideoModes != nullptr) {
			delete [] VideoModes;
			VideoModes = nullptr;
		}
		vw_ShutdownFont();
		vw_ReleaseText();
		vw_ShutdownAudio();
		vw_ShutdownVFS();
		JoystickClose();
		vw_ReleaseAllTimeThread();
		SDL_Quit();
		return 1;
	}

	// сохраняем данные во время первого старта сразу после инициализации "железа"
	// в случае некорректного завершения игры, файл настроек будет содержать актуальные параметры
	if (FirstStart)
		SaveXMLConfigFile();






	if ((static_cast<float>(GameConfig().Width) / static_cast<float>(GameConfig().Height)) < 1.4f) {
		ChangeGameConfig().InternalWidth = 1024.0f;
		ChangeGameConfig().InternalHeight = 768.0f;
	} else {
		ChangeGameConfig().InternalWidth = 1228.0f;
		ChangeGameConfig().InternalHeight = 768.0f;
	}

	// setup internal game resolution
	vw_SetInternalResolution(GameConfig().InternalWidth, GameConfig().InternalHeight, true);





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// скрываем системный курсор
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_DISABLE);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загрузка текстур согласно моделе загрузки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	LoadGameData(eLoading::MenuWithLogo);
	// загрузка списка миссий
	MissionsListInit(); // должен загружаться только 1 раз при запуске




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка процедуры... основного цикла и переход на нее
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CanQuit = true;


	// первоначальная установка курсора
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	vw_SetMousePos(mouse_x, mouse_y);

	// первоначальный сброс кнопок мышки
	for (int i = 0; i < 8; i++) {
		SDL_MouseCurrentStatus[i] = false;
	}


	// TODO check std::locale first
	// show dialog with language setup during first game launch
	if (FirstStart)
		SetCurrentDialogBox(eDialogBox::ChoseLanguage);



loop:


	Quit = false;
	NeedReCreate = false;
	bool NeedLoop = true;


	while(!Quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			// если нажали закрыть окно
			case SDL_QUIT:
				Quit = true;
				goto GotoQuit;

			// работаем с движением мышки
			case SDL_MOUSEMOTION:
				vw_SetMousePos(event.motion.x, event.motion.y);
				// если есть перемещение мышкой - сразу убираем управление клавиатурой
				CurrentKeyboardSelectMenuElement = 0;
				break;
			case SDL_MOUSEWHEEL:
				vw_ChangeWheelStatus(-1*event.wheel.y);
				break;
			// обрабатываем кнопки мыши
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					vw_SetMouseLeftClick(true);
				}
				if (event.button.button ==  SDL_BUTTON_RIGHT)
					vw_SetMouseRightClick(true);
				if (event.button.button < 8) // на всякий случай небольшая проверка
					SDL_MouseCurrentStatus[event.button.button] = true;
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button ==  SDL_BUTTON_LEFT)
					vw_SetMouseLeftClick(false);
				if (event.button.button ==  SDL_BUTTON_RIGHT)
					vw_SetMouseRightClick(false);
				if (event.button.button < 8) // на всякий случай небольшая проверка
					SDL_MouseCurrentStatus[event.button.button] = false;
				break;

			case SDL_JOYBUTTONDOWN:
				// only events from opened joystick could be here, no checks are needed
				vw_SetMouseLeftClick(true);
				SetJoystickButton(event.jbutton.button, true);
				break;
			case SDL_JOYBUTTONUP:
				// only events from opened joystick could be here, no checks are needed
				vw_SetMouseLeftClick(false);
				SetJoystickButton(event.jbutton.button, false);
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_FOCUS_LOST:
				case SDL_WINDOWEVENT_MINIMIZED:
					NeedLoop = false;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				case SDL_WINDOWEVENT_RESTORED:
					NeedLoop = true;
					break;
				}
				break;

			case SDL_KEYUP:
				vw_KeyStatusUpdate(event.key.keysym.sym);
			break;

			case SDL_TEXTINPUT:
				// устанавливаем текущий юникод нажатой клавиши
				vw_SetCurrentUnicodeChar(event.text.text);
#ifndef NDEBUG
				std::cout << "TextInput, Unicode: " << event.text.text << "\n";
#endif // NDEBUG
				break;
			default:
				break;
			}
		}

		if (NeedLoop) {
			JoystickEmulateMouseMovement(vw_GetTimeThread(0));
			Loop_Proc();
		} else {
			// выключаем музыку
			if (vw_IsAnyMusicPlaying())
				vw_ReleaseAllMusic();

			// если в игре, ставим паузу, т.е. открываем меню мгновенно
			if ((MenuStatus == eMenuStatus::GAME) && (GameContentTransp < 1.0f)) {
				NeedShowGameMenu = true;
				NeedHideGameMenu = false;
				GameContentTransp = 1.0f;
				vw_SetTimeThreadSpeed(1, 0.0f);
				DrawGameCursor = true;
			}

			vw_PauseTimeThreads();
			SDL_WaitEvent(nullptr);
			vw_ResumeTimeThreads();
		}
	}
GotoQuit:

	// если не выходим...
	if (!NeedReCreate && !CanQuit && Quit) {
		SetCurrentDialogBox(eDialogBox::QuitFromGame);
		goto loop;
	}



	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_ENABLE);

	DestroyInfoObject();
	WorkshopDestroyData();

	ReleaseAllObject3D();
	vw_ReleaseAllModel3D();
	vw_ReleaseAllParticleSystems2D();
	vw_ReleaseAllParticleSystems();
	vw_ReleaseAllLights();

	vw_ReleaseAllFontChars(); /* call before vw_ReleaseAllTextures() */
	vw_ReleaseAllTextures();
	ShadowMap_Release();

	// gl stuff -> gl context -> window
	vw_ReleaseOpenGLStuff();
	vw_DeleteOpenGLContext();
	vw_DestroyWindow();

	// сохраняем настройки игры
	SaveXMLConfigFile();

	// если нужно перезагрузить игру с новыми параметрами
	if (NeedReCreate) {
		FirstStart = false;

		// пересоздаем окно
		goto ReCreate;
	}


	if (VideoModes != nullptr) {
		delete [] VideoModes;
		VideoModes = nullptr;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение, освобождение памяти...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_ShutdownFont();
	vw_ReleaseText();
	vw_ShutdownAudio();
	vw_ShutdownVFS();
	JoystickClose();
	vw_ReleaseAllTimeThread();
	SDL_Quit();

	return 0;
}
