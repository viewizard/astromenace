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

// TODO Width, Height, Fullscreen and VSync should be altered without window/OpenGL context recreation

// TODO (?) DisplayIndex (display number) should be allowed to configure in menu

// TODO SDL_GL_SetSwapInterval() allow now -1 for adaptive vsync

// TODO translate comments

#include "../game.h"
#include "../config/config.h"
#include "../platform/platform.h"


// FIXME should be fixed, don't allow global scope interaction for local variables
// временные данные для изменения и восстановления
int Options_Width;
int Options_Height;
int Options_Fullscreen;
int Options_VSync;



















void OptionsMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1, float *ButtonTransp2, float *LastButtonUpdateTime2)
{

	sRECT SrcRect, DstRect;
	SrcRect(0, 0, 2, 2);
	DstRect(0 ,0, GameConfig().InternalWidth, 768);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f * ContentTransp);

	int X1 = GameConfig().InternalWidth / 2 - 375;
	int Y1 = 65;
	int Prir1 = 55;


	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Music Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, !vw_GetAudioStatus() || (GameConfig().MusicVolume == 0))) {
		ChangeGameConfig().MusicVolume--;
		if (GameConfig().MusicVolume < 0)
			ChangeGameConfig().MusicVolume = 0;
		vw_SetMusicGlobalVolume(GameConfig().MusicVolume / 10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, !vw_GetAudioStatus() || (GameConfig().MusicVolume == 10))) {
		ChangeGameConfig().MusicVolume++;
		if (GameConfig().MusicVolume > 10)
			ChangeGameConfig().MusicVolume = 10;
		vw_SetMusicGlobalVolume(GameConfig().MusicVolume / 10.0f);
	}
	if (!vw_GetAudioStatus()) {
		int SizeI = (170-vw_TextWidth(vw_GetText("Not available")))/2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::orange, ContentTransp, vw_GetText("Not available"));
	} else {
		for (int i = 0; i < 10; i++) {
			SrcRect(0 ,0, 16, 32);
			DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
			if (vw_GetAudioStatus() &&
			    vw_MouseOverRect(DstRect)) {
				CurrentCursorStatus = 1;
				if (vw_GetMouseLeftClick(true)) {
					ChangeGameConfig().MusicVolume = i + 1;
					vw_SetMusicGlobalVolume(GameConfig().MusicVolume / 10.0f);
					Audio_PlaySound2D(2, 1.0f);
				}
			}
			if (GameConfig().MusicVolume > i)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
		}
	}






	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Voice Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, !vw_GetAudioStatus() || (GameConfig().VoiceVolume == 0), false)) {
		ChangeGameConfig().VoiceVolume--;
		if (GameConfig().VoiceVolume < 0)
			ChangeGameConfig().VoiceVolume = 0;
		Audio_SetVoiceGlobalVolume(GameConfig().VoiceVolume / 10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, !vw_GetAudioStatus() || (GameConfig().VoiceVolume == 10), false)) {
		ChangeGameConfig().VoiceVolume++;
		if (GameConfig().VoiceVolume > 10)
			ChangeGameConfig().VoiceVolume = 10;
		Audio_SetVoiceGlobalVolume(GameConfig().VoiceVolume / 10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (!vw_GetAudioStatus()) {
		int SizeI = (170 - vw_TextWidth(vw_GetText("Not available"))) / 2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::orange, ContentTransp, vw_GetText("Not available"));
	} else {
		for (int i = 0; i < 10; i++) {
			SrcRect(0, 0, 16, 32);
			DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
			if (vw_GetAudioStatus() &&
			    vw_MouseOverRect(DstRect)) {
				CurrentCursorStatus = 1;
				if (vw_GetMouseLeftClick(true)) {
					ChangeGameConfig().VoiceVolume = i + 1;
					Audio_SetVoiceGlobalVolume(GameConfig().VoiceVolume / 10.0f);
					// play voice instead of 'click' here
					Audio_PlayVoice(1, 1.0f);
				}
			}
			if (GameConfig().VoiceVolume > i)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
		}
	}





	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Sound Effects Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, !vw_GetAudioStatus() || (GameConfig().SoundVolume == 0))) {
		ChangeGameConfig().SoundVolume--;
		if (GameConfig().SoundVolume < 0)
			ChangeGameConfig().SoundVolume = 0;
		Audio_SetSound2DGlobalVolume(GameConfig().SoundVolume / 10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, !vw_GetAudioStatus() || (GameConfig().SoundVolume == 10))) {
		ChangeGameConfig().SoundVolume++;
		if (GameConfig().SoundVolume > 10)
			ChangeGameConfig().SoundVolume = 10;
		Audio_SetSound2DGlobalVolume(GameConfig().SoundVolume / 10.0f);
	}
	if (!vw_GetAudioStatus()) {
		int SizeI = (170-vw_TextWidth(vw_GetText("Not available")))/2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::orange, ContentTransp, vw_GetText("Not available"));
	} else {
		for (int i=0; i<10; i++) {
			SrcRect(0,0,16,32);
			DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
			if (vw_GetAudioStatus() &&
			    vw_MouseOverRect(DstRect)) {
				CurrentCursorStatus = 1;
				if (vw_GetMouseLeftClick(true)) {
					ChangeGameConfig().SoundVolume = i + 1;
					Audio_SetSound2DGlobalVolume(GameConfig().SoundVolume / 10.0f);
					Audio_PlaySound2D(2, 1.0f);
				}
			}
			if (GameConfig().SoundVolume > i)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
		}
	}



	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Full Screen"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Off"), ContentTransp, DetectWindowSizeArray().empty() || !Options_Fullscreen) ||
	    DrawButton128_2(X1+616, Y1-6, vw_GetText("On"), ContentTransp, DetectFullscreenSize().empty() || Options_Fullscreen)) {
		Options_Fullscreen = !Options_Fullscreen;
		if (Options_Fullscreen) {
			Options_Width = DetectFullscreenSize().back().Width;
			Options_Height = DetectFullscreenSize().back().Height;
		} else {
			// if current mode is windowed, restore to current window size
			if (!GameConfig().Fullscreen) {
				Options_Width = GameConfig().Width;
				Options_Height = GameConfig().Height;
			} else {
				Options_Width = DetectWindowSizeArray().back().Width;
				Options_Height = DetectWindowSizeArray().back().Height;
			}
		}
	}
	int Size = vw_TextWidth(Options_Fullscreen ? vw_GetText("On") : vw_GetText("Off"));
	int SizeI = (170-Size)/2;
	vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, Options_Fullscreen ? vw_GetText("On") : vw_GetText("Off"));




	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Window Size"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, DetectWindowSizeArray().empty() || Options_Fullscreen)) {
		auto iter = std::find(DetectWindowSizeArray().cbegin(),
				      DetectWindowSizeArray().cend(),
				      sViewSize{Options_Width, Options_Height});
		// we don't check iter, since we check array and this window size before, so,
		// we know for sure, that the iter is not equal DetectWindowSizeArray().cend()
		if (iter == DetectWindowSizeArray().cbegin())
			iter = DetectWindowSizeArray().cend();
		--iter;

		Options_Width = iter->Width;
		Options_Height = iter->Height;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, DetectWindowSizeArray().empty() || Options_Fullscreen)) {
		auto iter = std::find(DetectWindowSizeArray().cbegin(),
				      DetectWindowSizeArray().cend(),
				      sViewSize{Options_Width, Options_Height});
		// we don't check iter, since we check array and this window size before, so,
		// we know for sure, that the iter is not equal DetectWindowSizeArray().cend()
		++iter;
		if (iter == DetectWindowSizeArray().cend())
			iter = DetectWindowSizeArray().cbegin();

		Options_Width = iter->Width;
		Options_Height = iter->Height;
	}

	std::string VideoModeTitle{std::to_string(Options_Width) + "x" + std::to_string(Options_Height)};
	Size = vw_TextWidth(VideoModeTitle.c_str());
	SizeI = (170-Size)/2;
	vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, VideoModeTitle.c_str());







	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Sync to VBlank"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Off"), ContentTransp, !Options_VSync) ||
	    DrawButton128_2(X1+616, Y1-6, vw_GetText("On"), ContentTransp, Options_VSync))
		if (NeedCheck == 0) {
			if (Options_VSync == 1)
				Options_VSync = 0;
			else
				Options_VSync = 1;
		}
	Size = vw_TextWidth(Options_VSync ? vw_GetText("On") : vw_GetText("Off"));
	SizeI = (170-Size)/2;
	vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, Options_VSync ? vw_GetText("On") : vw_GetText("Off"));







	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Brightness"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, (GameConfig().Brightness <= 1))) {
		ChangeGameConfig().Brightness--;
		if (GameConfig().Brightness <= 1)
			ChangeGameConfig().Brightness = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, (GameConfig().Brightness >= 10))) {
		ChangeGameConfig().Brightness++;
		if (GameConfig().Brightness >= 10)
			ChangeGameConfig().Brightness = 10;
	}
	for (int i = 0; i < 10; i++) {
		SrcRect(0, 0, 16, 32);
		DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
		if (vw_MouseOverRect(DstRect)) {
			CurrentCursorStatus = 1;
			if (vw_GetMouseLeftClick(true)) {
				ChangeGameConfig().Brightness = i + 1;
				Audio_PlaySound2D(2, 1.0f);
			}
		}
		if (GameConfig().Brightness > i)
			vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
		else
			vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
	}


















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = GameConfig().InternalWidth / 2 - 366;
	if (DrawButton200_2(X, Y+28, vw_GetText("Advanced"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::OPTIONS_ADVANCED);
			GameMenuStatus = eGameMenuStatus::OPTIONS_ADVANCED;
		} else
			ComBuffer = eCommand::SWITCH_TO_OPTIONS_ADVANCED;
	}


	X = GameConfig().InternalWidth / 2 - 100;
	if (DrawButton200_2(X, Y+28, vw_GetText("Interface"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::INTERFACE);
			GameMenuStatus = eGameMenuStatus::INTERFACE;
		} else
			ComBuffer = eCommand::SWITCH_TO_INTERFACE;
	}


	X = GameConfig().InternalWidth / 2 + 166;
	if (DrawButton200_2(X, Y+28, vw_GetText("Config Controls"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::CONFCONTROL);
			GameMenuStatus = eGameMenuStatus::CONFCONTROL;
		} else
			ComBuffer = eCommand::SWITCH_TO_CONFCONTROL;
	}







	if ((Options_Width == GameConfig().Width &&
	     Options_Height == GameConfig().Height &&
	     Options_Fullscreen == GameConfig().Fullscreen &&
	     Options_VSync == GameConfig().VSync)) {
		X = (GameConfig().InternalWidth - 384) / 2;
		Y = Y + Prir;
		if (MenuStatus == eMenuStatus::GAME) {
			if (DrawButton384(X,Y, vw_GetText("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				GameMenuStatus = eGameMenuStatus::GAME_MENU;
		} else {
			if (DrawButton384(X,Y, vw_GetText("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
		}
	} else {
		X = GameConfig().InternalWidth / 2 - 256 - 38;
		Y = Y+Prir;
		if (MenuStatus == eMenuStatus::GAME) {
			if (DrawButton256(X,Y, vw_GetText("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				GameMenuStatus = eGameMenuStatus::GAME_MENU;
		} else {
			if (DrawButton256(X,Y, vw_GetText("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
		}
		X = GameConfig().InternalWidth / 2 + 38;
		if (DrawButton256(X,Y, vw_GetText("APPLY"), ContentTransp, ButtonTransp2, LastButtonUpdateTime2)) {
			if (GameConfig().VSync != Options_VSync) {
				if (SDL_GL_SetSwapInterval(Options_VSync) == -1) {
					std::cerr << __func__ << "(): " << "SDL_GL_SetSwapInterval() failed: "
						  << SDL_GetError() << "\n";
					Options_VSync = GameConfig().VSync;
				} else
					ChangeGameConfig().VSync = Options_VSync;
			}
			// should be before size changes, since we could fallback to previous size
			if (GameConfig().Fullscreen != Options_Fullscreen) {
				Uint32 Flags{0};
				if (Options_Fullscreen)
					Flags = SDL_WINDOW_FULLSCREEN_DESKTOP;

				if (SDL_SetWindowFullscreen(vw_GetSDLWindow(), Flags) != 0) {
					std::cerr << __func__ << "(): " << "SDL_SetWindowFullscreen() failed: "
						  << SDL_GetError() << "\n";
					Options_Fullscreen = GameConfig().Fullscreen;
					Options_Width = GameConfig().Width;
					Options_Height = GameConfig().Height;
				} else
					ChangeGameConfig().Fullscreen = Options_Fullscreen;

				// note, since size will be changed for sure, we don't need
				// do anything here, code below will do all we need
			}
			if ((GameConfig().Width != Options_Width) ||
			    (GameConfig().Height != Options_Height)) {
				ChangeGameConfig().Width = Options_Width;
				ChangeGameConfig().Height = Options_Height;

				SDL_SetWindowSize(vw_GetSDLWindow(),
						  GameConfig().Width,
						  GameConfig().Height);

				SDL_SetWindowPosition(vw_GetSDLWindow(),
						      SDL_WINDOWPOS_CENTERED_DISPLAY(GameConfig().DisplayIndex),
						      SDL_WINDOWPOS_CENTERED_DISPLAY(GameConfig().DisplayIndex));

				vw_SetViewport(0, 0, GameConfig().Width, GameConfig().Height);
				vw_InitOpenGLStuff(GameConfig().Width, GameConfig().Height,
						   &ChangeGameConfig().MSAA, &ChangeGameConfig().CSAA);

				if (StandardAspectRation({GameConfig().Width, GameConfig().Height}))
					ChangeGameConfig().InternalWidth = 1024.0f;
				else
					ChangeGameConfig().InternalWidth = 1228.0f;
				ChangeGameConfig().InternalHeight = 768.0f;
				vw_SetInternalResolution(GameConfig().InternalWidth, GameConfig().InternalHeight, true);
			}
		}
	}
}
