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



/*
 * Initial setup on first start and video configuration check.
 */
static bool VideoConfig(bool FirstStart)
{
	// prevent out of range index usage
	if (GameConfig().DisplayIndex) {
		int tmpDisplaysCount = SDL_GetNumVideoDisplays();
		if (tmpDisplaysCount >= 1) {
			if (GameConfig().DisplayIndex >= tmpDisplaysCount)
				ChangeGameConfig().DisplayIndex = 0; // fallback to first display
		} else
			std::cerr << __func__ << "(): " << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << "\n";

		ChangeDisplayIndex(GameConfig().DisplayIndex);
	}

	// check both arrays (with corrected display index), we need at least one non empty array
	if (DetectFullscreenSize().empty() &&
	    DetectWindowSizeArray().empty()) {
		std::cerr << __func__ << "(): " << "display does not support any appropriate screen resolutions.\n";
		return false;
	}

	if (FirstStart) {
		if (!DetectFullscreenSize().empty()) {
			ChangeGameConfig().Width = DetectFullscreenSize().back().Width;
			ChangeGameConfig().Height = DetectFullscreenSize().back().Height;
			ChangeGameConfig().Fullscreen = true;
		} else { // we don't check DetectWindowSizeArray(), since we check it above
			ChangeGameConfig().Width = DetectWindowSizeArray().back().Width;
			ChangeGameConfig().Height = DetectWindowSizeArray().back().Height;
			ChangeGameConfig().Fullscreen = false;
		}
		return true;
	}

	// check config's mode, note, we need check only one array here, since we know,
	// if one is empty, second is not empty (we check this above)
	if (GameConfig().Fullscreen &&
	    DetectFullscreenSize().empty())
		ChangeGameConfig().Fullscreen = false;
	else if (!GameConfig().Fullscreen &&
		 DetectWindowSizeArray().empty())
		ChangeGameConfig().Fullscreen = true;

	// in case of Fullscreen we don't really care about saved Width and Height,
	// since we need current display size for sure
	if (GameConfig().Fullscreen) {
		ChangeGameConfig().Width = DetectFullscreenSize().back().Width;
		ChangeGameConfig().Height = DetectFullscreenSize().back().Height;
	} else if (std::find(DetectWindowSizeArray().cbegin(),
			     DetectWindowSizeArray().cend(),
			     sViewSize{GameConfig().Width, GameConfig().Height}) == DetectWindowSizeArray().cend()) {
		ChangeGameConfig().Width = DetectWindowSizeArray().back().Width;
		ChangeGameConfig().Height = DetectWindowSizeArray().back().Height;
	}

	return true;
}

/*
 * Main.
 */
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
				  << "--reset-config - reset all settings except Pilot Profiles;\n"
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

	std::cout << "AstroMenace " << GAME_VERSION << "\n";
	std::cout << "VFS version " << GAME_VFS_BUILD << "\n\n";

	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	// This function may be called safely at any time, even before SDL_Init().
	// https://wiki.libsdl.org/SDL_GetVersion
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

	// the file I/O are initialized by default (https://wiki.libsdl.org/SDL_Init)
	// since VFS use only file I/O, we are safe to call this one before SDL_Init()
	if (NeedPack)
		return ConvertFS2VFS(GetRawDataPath(), GetDataPath() + "gamedata.vfs");

	// subsystems, that should be initialized before any interactions
	// for predictable work on all platforms
	Uint32 SDL_Init_Flags = SDL_INIT_TIMER |
				SDL_INIT_EVENTS |
				SDL_INIT_JOYSTICK |
				SDL_INIT_VIDEO;

	if (SDL_Init(SDL_Init_Flags) != 0) {
		std::cerr << __func__ << "(): " << "Couldn't init SDL: " << SDL_GetError() << "\n";
		return 1;
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

	// should be called after vw_InitText(), since we need check language index numbers
	bool FirstStart = LoadXMLConfigFile(NeedResetConfig);

	if (!VideoConfig(FirstStart)) {
		vw_ReleaseText();
		vw_ShutdownVFS();
		SDL_Quit();
		return 1;
	}

	if (GameConfig().FontNumber > FontQuantity)
		ChangeGameConfig().FontNumber = 0;

	InitFont(FontList[GameConfig().FontNumber].FontFileName);

	vw_InitTimeThread(0);

	// should be called after vw_InitTimeThread(0)
	JoystickInit(vw_GetTimeThread(0));

	if (!vw_InitAudio())
		std::cerr << __func__ << "(): " << "Unable to open audio.\n\n";

	// TODO check user preferred locale first
	vw_SetTextLanguage(GameConfig().MenuLanguage);

ReCreateWindow:

	// if we change options during game mission with game restart, care about dialogs reset
	InitDialogBoxes();

	if (!vw_CreateWindow("AstroMenace", GameConfig().Width, GameConfig().Height,
			     GameConfig().Fullscreen, GameConfig().DisplayIndex)) {
		vw_ShutdownFont();
		vw_ReleaseText();
		vw_ShutdownAudio();
		vw_ShutdownVFS();
		JoystickClose();
		vw_ReleaseAllTimeThread();
		SDL_Quit();
		return 1;
	}

	if (!vw_CreateOpenGLContext(GameConfig().VSync)) {
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





	// setup internal game resolution
	if (StandardAspectRation({GameConfig().Width, GameConfig().Height}))
		ChangeGameConfig().InternalWidth = 1024.0f;
	else
		ChangeGameConfig().InternalWidth = 1228.0f;
	ChangeGameConfig().InternalHeight = 768.0f;
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
	MissionListInit(); // должен загружаться только 1 раз при запуске



	// первоначальная установка курсора
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	vw_SetMousePos(mouse_x, mouse_y);

	// первоначальный сброс кнопок мышки
	for (int i = 0; i < 8; i++) {
		SDL_MouseCurrentStatus[i] = false;
	}


	// show dialog with language setup during first game launch
	if (FirstStart)
		SetCurrentDialogBox(eDialogBox::ChoseLanguage);


	Quit = false;
	NeedReCreate = false;
	bool NeedLoop{true};

	while (!Quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			// если нажали закрыть окно
			case SDL_QUIT:
				if (MenuStatus == eMenuStatus::GAME)
					SetCurrentDialogBox(eDialogBox::QuitNoSave);
				else
					SetCurrentDialogBox(eDialogBox::QuitFromGame);
				break;

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
		goto ReCreateWindow;
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
