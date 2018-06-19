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

// TODO revised shutdown/release resources code (implement hooks with dependencies list on init?)

// FIXME after switch to OpenGL 2.1 (? v1.5), remove goto statement from main()

#include "core/core.h"
#include "config/config.h"
#include "enum.h"
#include "build_config.h"
#include "ui/font.h"
#include "audio/audio.h"
#include "gfx/shadow_map.h"
#include "platform/platform.h"
#include "object3d/object3d.h"
#include "utils/fs2vfs.h"
#include "game.h" // FIXME "game.h" should be replaced by individual headers

namespace {

bool NeedQuitFromLoop{false};
bool NeedRecreateWindow{false};

} // unnamed namespace


/*
 * Quit from main loop.
 */
void QuitFromMainLoop()
{
	NeedQuitFromLoop = true;
}

/*
 * Recreate game's window.
 */
void RecreateGameWindow()
{
	NeedQuitFromLoop = true;
	NeedRecreateWindow = true;
}

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
 * Check config settings and OpenGL capabilities.
 */
static bool CheckOpenGLCapabilities(bool FirstStart)
{
	// hardware must support multtextures (OpenGL 1.3)
	if (!vw_GetDevCaps().OpenGL_1_3_supported) {
		std::cerr << __func__ << "(): " << "The Multi Textures feature not supported by hardware. Fatal error.\n";
#ifdef WIN32
		MessageBox(nullptr,
			   "OpenGL 1.3 required. Please, install the newest video drivers from your video card vendor.",
			   "Render system - Fatal Error",
			   MB_OK | MB_APPLMODAL | MB_ICONERROR);
#endif // WIN32
		return false;
	}

	if (FirstStart) {
		if (vw_GetDevCaps().OpenGL_3_0_supported) {
			ChangeGameConfig().UseGLSL120 = true;
			ChangeGameConfig().ShadowMap = 1;
			ChangeGameConfig().MSAA = 2;
			ChangeGameConfig().CSAA = 2;
			ChangeGameConfig().AnisotropyLevel = vw_GetDevCaps().MaxAnisotropyLevel;
			ChangeGameConfig().MaxPointLights = 4;
		}
		if (vw_GetDevCaps().OpenGL_4_2_supported) {
			ChangeGameConfig().ShadowMap = 1;
			ChangeGameConfig().MSAA = 4;
			ChangeGameConfig().CSAA = 4;
			ChangeGameConfig().MaxPointLights = 6;
		}
	}

	// NOTE in future, use std::clamp (since C++17)
	if (GameConfig().AnisotropyLevel > vw_GetDevCaps().MaxAnisotropyLevel)
		ChangeGameConfig().AnisotropyLevel = vw_GetDevCaps().MaxAnisotropyLevel;
	else if (GameConfig().AnisotropyLevel < 1)
		ChangeGameConfig().AnisotropyLevel = 1;

	// for shaders version 120, check OpenGL 2.0 and 2.1
	if (!vw_GetDevCaps().OpenGL_2_0_supported ||
	    !vw_GetDevCaps().OpenGL_2_1_supported)
		ChangeGameConfig().UseGLSL120 = false;

	// for shadowmap we need shaders (OpenGL 2.0 and 2.1) and fbo (OpenGL 3.0) support
	if (!vw_GetDevCaps().OpenGL_2_0_supported ||
	    !vw_GetDevCaps().OpenGL_2_1_supported ||
	    !vw_GetDevCaps().OpenGL_3_0_supported ||
	    (vw_GetDevCaps().MaxTextureWidth < 2048))
		ChangeGameConfig().ShadowMap = 0;

	// check MSAA/CSAA mode from cofig
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

	SaveXMLConfigFile();
	return true;
}

/*
 * Log info about game's and lib's versions.
 */
static void LogGameAndLibsVersion()
{
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
}

/*
 * Main loop.
 */
static void Loop()
{
	NeedQuitFromLoop = false;
	NeedRecreateWindow = false;
	bool NeedPause{false};

	while (!NeedQuitFromLoop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: // close window by ALT+F4 or click on window's 'close' button
				if (MenuStatus == eMenuStatus::GAME)
					SetCurrentDialogBox(eDialogBox::QuitNoSave);
				else
					SetCurrentDialogBox(eDialogBox::QuitFromGame);
				break;

			case SDL_MOUSEMOTION:
				vw_SetMousePos(event.motion.x, event.motion.y);
				// in case we have mouse movement, reset keyboard selected menu element
				CurrentKeyboardSelectMenuElement = 0;
				break;
			case SDL_MOUSEWHEEL:
				vw_ChangeWheelStatus(-event.wheel.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				vw_SetMouseButtonStatus(event.button.button, true);
				if (event.button.button == SDL_BUTTON_LEFT)
					vw_SetMouseLeftClick(true);
				if (event.button.button == SDL_BUTTON_RIGHT)
					vw_SetMouseRightClick(true);
				if ((event.button.clicks == 2) && // double click
				    (event.button.button == SDL_BUTTON_LEFT))
					vw_SetMouseLeftDoubleClick(true);
				break;
			case SDL_MOUSEBUTTONUP:
				vw_SetMouseButtonStatus(event.button.button, false);
				if (event.button.button == SDL_BUTTON_LEFT)
					vw_SetMouseLeftClick(false);
				if (event.button.button == SDL_BUTTON_RIGHT)
					vw_SetMouseRightClick(false);
				if ((event.button.clicks == 2) && // double click
				    (event.button.button == SDL_BUTTON_LEFT))
					vw_SetMouseLeftDoubleClick(false);
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
					NeedPause = true;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				case SDL_WINDOWEVENT_RESTORED:
					NeedPause = false;
					break;
				}
				break;

			case SDL_KEYUP:
				vw_KeyStatusUpdate(event.key.keysym.sym);
			break;

			case SDL_TEXTINPUT:
				// convert utf8 to utf32, that we need for taping
				vw_SetCurrentUnicodeChar(event.text.text);
#ifndef NDEBUG
				std::cout << "TextInput, Unicode: " << event.text.text << "\n";
#endif // NDEBUG
				break;

			default:
				break;
			}
		}

		if (!NeedPause) {
			JoystickEmulateMouseMovement(vw_GetTimeThread(0));
			Loop_Proc();
			AudioLoop();
		} else {
			// turn off music
			if (vw_IsAnyMusicPlaying())
				vw_ReleaseAllMusic();

			// pause, so, player don't lose anything
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

	LogGameAndLibsVersion();

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

	// should be called after vw_InitText(), since we need find language index numbers
	bool FirstStart = LoadXMLConfigFile(NeedResetConfig);

	if (!VideoConfig(FirstStart)) {
		vw_ReleaseText();
		vw_ShutdownVFS();
		SDL_Quit();
		return 1;
	}

	InitFont(GetFontMetadata(GameConfig().FontNumber).FontFileName); // should be called after LoadXMLConfigFile()

	vw_InitTimeThread(0);

	// should be called after vw_InitTimeThread(0)
	JoystickInit(vw_GetTimeThread(0));

	if (!vw_InitAudio())
		std::cerr << __func__ << "(): " << "Unable to open audio.\n\n";

	if (FirstStart) {
		unsigned DetectedLanguage = FindPreferredLanguageByLocale(); // should be called after vw_InitText()
		if (DetectedLanguage) {
			ChangeGameConfig().MenuLanguage = DetectedLanguage;
			ChangeGameConfig().VoiceLanguage = DetectedLanguage;
		}
		SetCurrentDialogBox(eDialogBox::ChoseLanguage);
	}
	vw_SetTextLanguage(GameConfig().MenuLanguage);

RecreateWindow:

	if (!vw_CreateWindow("AstroMenace", GameConfig().Width, GameConfig().Height,
			     GameConfig().Fullscreen, GameConfig().DisplayIndex) ||
	    !vw_CreateOpenGLContext(GameConfig().VSync) ||
	    !CheckOpenGLCapabilities(FirstStart)) {
		vw_DeleteOpenGLContext();
		vw_DestroyWindow();
		vw_ShutdownFont();
		vw_ReleaseText();
		vw_ShutdownAudio();
		vw_ShutdownVFS();
		JoystickClose();
		vw_ReleaseAllTimeThread();
		SDL_Quit();
		return 1;
	}

	vw_InitOpenGLStuff(GameConfig().Width, GameConfig().Height,
			   &ChangeGameConfig().MSAA, &ChangeGameConfig().CSAA);

	ConfigVirtualInternalResolution();

	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_DISABLE);

	LoadGameData(eLoading::MenuWithLogo);

	// Main loop.
	Loop();

	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_ENABLE);

	DestroyInfoObject();
	WorkshopDestroyData();

	ReleaseAllObject3D();
	vw_ReleaseAllModel3D();
	vw_ReleaseAllParticleSystems2D();
	vw_ReleaseAllParticleSystems();
	vw_ReleaseAllLights();

	vw_ReleaseAllFontChars();
	vw_ReleaseAllTextures();
	ShadowMap_Release();

	// gl stuff -> gl context -> window
	vw_ReleaseOpenGLStuff();
	vw_DeleteOpenGLContext();
	vw_DestroyWindow();

	// important, save config before game's window recreate
	SaveXMLConfigFile();

	if (NeedRecreateWindow) {
		// if we change options during game mission with game restart, care about dialogs reset
		InitDialogBoxes();
		vw_ResetMouseButtons();
		FirstStart = false;
		goto RecreateWindow;
	}

	vw_ShutdownFont();
	vw_ReleaseText();
	vw_ShutdownAudio();
	vw_ShutdownVFS();
	JoystickClose();
	vw_ReleaseAllTimeThread();
	SDL_Quit();
	return 0;
}
