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

#include "../game.h"
#include "../config/config.h"


// временные данные для изменения и восстановления
int Options_Width;
int Options_Height;
int Options_BPP;
int Options_VSync;
int Options_iAspectRatioWidth;


const char *ButtonScreenModeTitle[2] = {
	"Screen Mode",
	"Window Size"
};


















void OptionsMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1, float *ButtonTransp2, float *LastButtonUpdateTime2)
{

	sRECT SrcRect, DstRect;
	SrcRect(0, 0, 2, 2);
	DstRect(0 ,0, GameConfig().InternalWidth, 768);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f * ContentTransp);


	int CurrentPos = 0;

	int X1 = GameConfig().InternalWidth / 2 - 375;
	int Y1 = 65;
	int Prir1 = 55;


	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Music Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, !GameConfig().Music_check || (GameConfig().MusicVolume == 0))) {
		ChangeGameConfig().MusicVolume--;
		if (GameConfig().MusicVolume < 0)
			ChangeGameConfig().MusicVolume = 0;
		vw_SetMusicGlobalVolume(GameConfig().MusicVolume / 10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, !GameConfig().Music_check || (GameConfig().MusicVolume == 10))) {
		ChangeGameConfig().MusicVolume++;
		if (GameConfig().MusicVolume > 10)
			ChangeGameConfig().MusicVolume = 10;
		vw_SetMusicGlobalVolume(GameConfig().MusicVolume / 10.0f);
	}
	if (!GameConfig().Music_check) {
		int SizeI = (170-vw_TextWidth(vw_GetText("Not available")))/2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::orange, ContentTransp, vw_GetText("Not available"));
	} else {
		for (int i = 0; i < 10; i++) {
			SrcRect(0 ,0, 16, 32);
			DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
			if (GameConfig().MusicVolume > i)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
		}
	}






	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Voice Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, !GameConfig().Sound_check || (GameConfig().VoiceVolume == 0), false)) {
		ChangeGameConfig().VoiceVolume--;
		if (GameConfig().VoiceVolume < 0)
			ChangeGameConfig().VoiceVolume = 0;
		Audio_SetVoiceGlobalVolume(GameConfig().VoiceVolume / 10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, !GameConfig().Sound_check || (GameConfig().VoiceVolume == 10), false)) {
		ChangeGameConfig().VoiceVolume++;
		if (GameConfig().VoiceVolume > 10)
			ChangeGameConfig().VoiceVolume = 10;
		Audio_SetVoiceGlobalVolume(GameConfig().VoiceVolume / 10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (!GameConfig().Sound_check) {
		int SizeI = (170 - vw_TextWidth(vw_GetText("Not available"))) / 2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::orange, ContentTransp, vw_GetText("Not available"));
	} else {
		for (int i = 0; i < 10; i++) {
			SrcRect(0, 0, 16, 32);
			DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
			if (GameConfig().VoiceVolume > i)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
		}
	}





	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Sound Effects Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Decrease"), ContentTransp, !GameConfig().Sound_check || (GameConfig().SoundVolume == 0))) {
		ChangeGameConfig().SoundVolume--;
		if (GameConfig().SoundVolume < 0)
			ChangeGameConfig().SoundVolume = 0;
		Audio_SetSound2DGlobalVolume(GameConfig().SoundVolume / 10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Increase"), ContentTransp, !GameConfig().Sound_check || (GameConfig().SoundVolume == 10))) {
		ChangeGameConfig().SoundVolume++;
		if (GameConfig().SoundVolume > 10)
			ChangeGameConfig().SoundVolume = 10;
		Audio_SetSound2DGlobalVolume(GameConfig().SoundVolume / 10.0f);
	}
	if (!GameConfig().Sound_check) {
		int SizeI = (170-vw_TextWidth(vw_GetText("Not available")))/2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::orange, ContentTransp, vw_GetText("Not available"));
	} else {
		for (int i=0; i<10; i++) {
			SrcRect(0,0,16,32);
			DstRect(X1+443+16*i, Y1-4, X1+443+16+16*i, Y1+32-4);
			if (GameConfig().SoundVolume > i)
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc.tga"), true, ContentTransp);
			else
				vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/perc_none.tga"), true, ContentTransp);
		}
	}



	// проверяем, есть ли вообще полноэкранные разрешения
	bool CanSwitchToFullScreen = false;
	for(int i=0; i<VideoModesNum; i++) {
		if ((VideoModes[i].BPP == 16) |
		    (VideoModes[i].BPP == 24)) {
			CanSwitchToFullScreen = true;
			break;
		}
	}


	Y1 += Prir1;
	if (Options_BPP != 0) CurrentPos = 0;
	else CurrentPos = 1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Full Screen"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, !CanSwitchToFullScreen) || DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, !CanSwitchToFullScreen)) {
		if (Options_BPP != 0) {
			Options_BPP = 0;
		} else {
			Options_BPP = CurrentVideoMode.BPP;

			// пробуем просто переключить BPP, проверяем есть ли такое разрешение
			bool NeedDetectResolution = true;
			for(int i=0; i<VideoModesNum; i++) {
				if ((VideoModes[i].W == Options_Width) &
				    (VideoModes[i].H == Options_Height) &
				    (VideoModes[i].BPP == Options_BPP)) {
					NeedDetectResolution = false;
					break;
				}
			}

			// находим первый полноэкранный режим в списке
			if (NeedDetectResolution)
				for(int i=0; i<VideoModesNum; i++) {
					if ((VideoModes[i].BPP == 16) |
					    (VideoModes[i].BPP == 24)) {
						Options_Width = VideoModes[i].W;
						Options_Height = VideoModes[i].H;
						Options_BPP = VideoModes[i].BPP;
						break;
					}
				}
		}
	}
	int Size = vw_TextWidth(Options_BPP ? vw_GetText("On") : vw_GetText("Off"));
	int SizeI = (170-Size)/2;
	vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, Options_BPP ? vw_GetText("On") : vw_GetText("Off"));




	// находим текущий номер режима в списке
	int CurrentListNum = 0;
	for(int i=0; i<VideoModesNum; i++) {
		if (VideoModes[i].W == Options_Width &&
		    VideoModes[i].H == Options_Height &&
		    VideoModes[i].BPP == Options_BPP) {
			CurrentListNum = i;
			break;
		}
	}



	Y1 += Prir1;
	if (Options_BPP != 0) CurrentPos = 0;
	else CurrentPos = 1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText(ButtonScreenModeTitle[CurrentPos]));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, false)) {
		CurrentListNum--;

		// ставим правильный
		bool check_next = true;
		while (check_next) {
			// вышли за предел
			if (CurrentListNum < 0) CurrentListNum = VideoModesNum-1;

			if ((VideoModes[CurrentListNum].BPP > 0 && Options_BPP > 0) ||
			    (VideoModes[CurrentListNum].BPP == 0 && Options_BPP == 0)) {
				check_next = false;
			} else {
				CurrentListNum--;
			}
		}
		Options_Width = VideoModes[CurrentListNum].W;
		Options_Height = VideoModes[CurrentListNum].H;
		Options_BPP = VideoModes[CurrentListNum].BPP;
		if ((Options_Width*1.0f)/(Options_Height*1.0f) < 1.4f)
			Options_iAspectRatioWidth = 1024;
		else
			Options_iAspectRatioWidth = 1228;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, false)) {
		CurrentListNum++;

		// ставим правильный
		bool check_next = true;
		while (check_next) {
			// вышли за предел
			if (CurrentListNum >= VideoModesNum) CurrentListNum = 0;

			if ((VideoModes[CurrentListNum].BPP > 0 && Options_BPP > 0) ||
			    (VideoModes[CurrentListNum].BPP == 0 && Options_BPP == 0)) {
				check_next = false;
			} else {
				CurrentListNum++;
			}
		}

		Options_Width = VideoModes[CurrentListNum].W;
		Options_Height = VideoModes[CurrentListNum].H;
		Options_BPP = VideoModes[CurrentListNum].BPP;
		if ((Options_Width*1.0f)/(Options_Height*1.0f) < 1.4f)
			Options_iAspectRatioWidth = 1024;
		else
			Options_iAspectRatioWidth = 1228;
	}

	std::string VideoModeTitle{std::to_string(VideoModes[CurrentListNum].W) + "x" +
				   std::to_string(VideoModes[CurrentListNum].H)};
	if (VideoModes[CurrentListNum].BPP != 0)
		VideoModeTitle += " " + std::to_string(VideoModes[CurrentListNum].BPP) + "bit";

	Size = vw_TextWidth(VideoModeTitle.c_str());
	SizeI = (170-Size)/2;
	vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, VideoModeTitle.c_str());



	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Aspect Ratio"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, false)) {
		if (Options_iAspectRatioWidth == 1228) Options_iAspectRatioWidth = 1024;
		else Options_iAspectRatioWidth = 1228;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, false)) {
		if (Options_iAspectRatioWidth == 1024) Options_iAspectRatioWidth = 1228;
		else Options_iAspectRatioWidth = 1024;
	}
	if (Options_iAspectRatioWidth == 1024) {
		Size = vw_TextWidth(vw_GetText("Standard"));
		SizeI = (170-Size)/2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Standard"));
	} else {
		Size = vw_TextWidth(vw_GetText("Widescreen"));
		SizeI = (170-Size)/2;
		vw_DrawText(X1+438+SizeI, Y1, 0, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Widescreen"));
	}






	Y1 += Prir1;
	vw_DrawText(X1, Y1, -280, 0, 1.0f, eRGBCOLOR::white, ContentTransp, vw_GetText("Sync to VBlank"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, false) || DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, false))
		if (NeedCheck == 0) {
			if (Options_VSync == 1) Options_VSync = 0;
			else Options_VSync = 1;
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
	     Options_BPP == GameConfig().BPP &&
	     Options_VSync == GameConfig().VSync &&
	     Options_iAspectRatioWidth == GameConfig().InternalWidth)) {
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
			// проверяем, нужно перегружать или нет
			if (Options_Width != GameConfig().Width ||
			    Options_Height != GameConfig().Height ||
			    Options_BPP != GameConfig().BPP ||
			    Options_VSync != GameConfig().VSync ||
			    Options_iAspectRatioWidth != GameConfig().InternalWidth) {
				if (MenuStatus == eMenuStatus::GAME)
					SetCurrentDialogBox(eDialogBox::RestartOnOptionsChanged);
				else {
					CanQuit = false;
					Quit = true;
					NeedReCreate = true;
					SaveOptionsMenuTmpData();
				}
			}
		}
	}
}


void SaveOptionsMenuTmpData()
{
	ChangeGameConfig().Width = Options_Width;
	ChangeGameConfig().Height = Options_Height;

	// пока своего управление нет- делаем автоматическую установку
	if (Options_iAspectRatioWidth == 1024) {
		ChangeGameConfig().InternalWidth = 1024.0f;
		ChangeGameConfig().InternalHeight = 768.0f;
	} else {
		ChangeGameConfig().InternalWidth = 1228.0f;
		ChangeGameConfig().InternalHeight = 768.0f;
	}

	ChangeGameConfig().BPP = Options_BPP;
	ChangeGameConfig().VSync = Options_VSync;
}






