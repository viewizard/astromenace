/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "../Game.h"


// временные данные для изменения и восстановления
int Options_Width;
int Options_Height;
int Options_BPP;
int Options_VSync;
int Options_iAspectRatioWidth;


const char *ButtonScreenModeTitle[2] =
{"3_Screen_Mode",
"3_Window_Size"};


















void OptionsMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1, float *ButtonTransp2, float *LastButtonUpdateTime2)
{

	RECT SrcRect, DstRect;
	SetRect(&SrcRect,0,0,2,2);
	SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*ContentTransp);


	int CurrentPos = 0;

	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;


	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Music_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), ContentTransp, !Setup.Music_check || Setup.MusicSw==0))
	{
		Setup.MusicSw--;
		if (Setup.MusicSw<0) Setup.MusicSw = 0;
		vw_SetMusicMainVolume(Setup.MusicSw/10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), ContentTransp, !Setup.Music_check || Setup.MusicSw==10))
	{
		Setup.MusicSw++;
		if (Setup.MusicSw>10) Setup.MusicSw = 10;
		vw_SetMusicMainVolume(Setup.MusicSw/10.0f);
	}
	if (!Setup.Music_check)
	{
		int SizeI = (170-vw_FontSize(vw_GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRect,0,0,16,32);
			SetRect(&DstRect,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.MusicSw>i)
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc.tga"), true, ContentTransp);
			else
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, ContentTransp);
		}
	}






	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Voice_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), ContentTransp, !Setup.Sound_check || Setup.VoiceSw==0, false))
	{
		Setup.VoiceSw--;
		if (Setup.VoiceSw<0) Setup.VoiceSw = 0;
		Audio_SetVoiceMainVolume(Setup.VoiceSw/10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), ContentTransp, !Setup.Sound_check || Setup.VoiceSw==10, false))
	{
		Setup.VoiceSw++;
		if (Setup.VoiceSw>10) Setup.VoiceSw = 10;
		Audio_SetVoiceMainVolume(Setup.VoiceSw/10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (!Setup.Sound_check)
	{
		int SizeI = (170-vw_FontSize(vw_GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRect,0,0,16,32);
			SetRect(&DstRect,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.VoiceSw>i)
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc.tga"), true, ContentTransp);
			else
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, ContentTransp);
		}
	}





	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Sound_Effects_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), ContentTransp, !Setup.Sound_check || Setup.SoundSw==0))
	{
		Setup.SoundSw--;
		if (Setup.SoundSw<0) Setup.SoundSw = 0;
		Audio_SetSound2DMainVolume(Setup.SoundSw/10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), ContentTransp, !Setup.Sound_check || Setup.SoundSw==10))
	{
		Setup.SoundSw++;
		if (Setup.SoundSw>10) Setup.SoundSw = 10;
		Audio_SetSound2DMainVolume(Setup.SoundSw/10.0f);
	}
	if (!Setup.Sound_check)
	{
		int SizeI = (170-vw_FontSize(vw_GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRect,0,0,16,32);
			SetRect(&DstRect,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.SoundSw>i)
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc.tga"), true, ContentTransp);
			else
				vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, ContentTransp);
		}
	}



	// проверяем, есть ли вообще полноэкранные разрешения
	bool CanSwitchToFullScreen = false;
	for(int i=0; i<VideoModesNum; i++)
	{
		if ((VideoModes[i].BPP == 16) |
			(VideoModes[i].BPP == 24))
		{
			CanSwitchToFullScreen = true;
			break;
		}
	}


	Y1 += Prir1;
	if (Options_BPP != 0) CurrentPos = 0;
	else CurrentPos = 1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Full_Screen"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, !CanSwitchToFullScreen) || DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, !CanSwitchToFullScreen))
	{
		if (Options_BPP != 0)
		{
			Options_BPP = 0;
		}
		else
		{
			Options_BPP = CurrentVideoMode.BPP;

			// пробуем просто переключить BPP, проверяем есть ли такое разрешение
			bool NeedDetectResolution = true;
			for(int i=0; i<VideoModesNum; i++)
			{
				if ((VideoModes[i].W == Options_Width) &
					(VideoModes[i].H == Options_Height) &
					(VideoModes[i].BPP == Options_BPP))
				{
					NeedDetectResolution = false;
					break;
				}
			}

			// находим первый полноэкранный режим в списке
			if (NeedDetectResolution)
			for(int i=0; i<VideoModesNum; i++)
			{
				if ((VideoModes[i].BPP == 16) |
					(VideoModes[i].BPP == 24))
				{
					Options_Width = VideoModes[i].W;
					Options_Height = VideoModes[i].H;
					Options_BPP = VideoModes[i].BPP;
					break;
				}
			}
		}
	}
	int Size = vw_FontSize(Options_BPP ? vw_GetText("1_On") : vw_GetText("1_Off"));
	int SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, Options_BPP ? vw_GetText("1_On") : vw_GetText("1_Off"));




	// находим текущий номер режима в списке
	int CurrentListNum = 0;
	for(int i=0; i<VideoModesNum; i++)
	{
		if (VideoModes[i].W == Options_Width &&
			VideoModes[i].H == Options_Height &&
			VideoModes[i].BPP == Options_BPP)
		{
			CurrentListNum = i;
			break;
		}
	}



	Y1 += Prir1;
	if (Options_BPP != 0) CurrentPos = 0;
	else CurrentPos = 1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ButtonScreenModeTitle[CurrentPos]));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, false))
	{
		CurrentListNum--;

		// ставим правильный
		bool check_next = true;
		while (check_next)
		{
			// вышли за предел
			if (CurrentListNum < 0) CurrentListNum = VideoModesNum-1;

			if ((VideoModes[CurrentListNum].BPP > 0 && Options_BPP > 0) ||
				(VideoModes[CurrentListNum].BPP == 0 && Options_BPP == 0))
			{
					check_next = false;
			}
			else
			{
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
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, false))
	{
		CurrentListNum++;

		// ставим правильный
		bool check_next = true;
		while (check_next)
		{
			// вышли за предел
			if (CurrentListNum >= VideoModesNum) CurrentListNum = 0;

			if ((VideoModes[CurrentListNum].BPP > 0 && Options_BPP > 0) ||
				(VideoModes[CurrentListNum].BPP == 0 && Options_BPP == 0))
			{
					check_next = false;
			}
			else
			{
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

	char VideoModeTitle[32];
	if (VideoModes[CurrentListNum].BPP != 0)
		sprintf(VideoModeTitle, "%ix%i %ibit", VideoModes[CurrentListNum].W, VideoModes[CurrentListNum].H, VideoModes[CurrentListNum].BPP);
	else
		sprintf(VideoModeTitle, "%ix%i", VideoModes[CurrentListNum].W, VideoModes[CurrentListNum].H);

	Size = vw_FontSize(VideoModeTitle);
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, VideoModeTitle);



	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Aspect_Ration"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, false))
	{
		if (Options_iAspectRatioWidth == 1228) Options_iAspectRatioWidth = 1024;
		else Options_iAspectRatioWidth = 1228;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, false))
	{
		if (Options_iAspectRatioWidth == 1024) Options_iAspectRatioWidth = 1228;
		else Options_iAspectRatioWidth = 1024;
	}
	if (Options_iAspectRatioWidth == 1024)
	{
		Size = vw_FontSize(vw_GetText("3_Standard"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Standard"));
	}
	else
	{
		Size = vw_FontSize(vw_GetText("3_Widescreen"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Widescreen"));
	}






	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_VSync"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, false) || DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, false))
	if (NeedCheck == 0)
	{
		if (Options_VSync == 1) Options_VSync = 0;
		else Options_VSync = 1;
	}
	Size = vw_FontSize(Options_VSync ? vw_GetText("1_On") : vw_GetText("1_Off"));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, Options_VSync ? vw_GetText("1_On") : vw_GetText("1_Off"));







	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Brightness"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), ContentTransp, Setup.Brightness<=1))
	{
		Setup.Brightness -= 1;
		if (Setup.Brightness <= 1) Setup.Brightness = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), ContentTransp, Setup.Brightness>=10))
	{
		Setup.Brightness += 1;
		if (Setup.Brightness >= 10) Setup.Brightness = 10;
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRect,0,0,16,32);
		SetRect(&DstRect,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.Brightness>i)
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc.tga"), true, ContentTransp);
		else
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, ContentTransp);
	}


















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Advanced"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(OPTIONS_ADVANCED);
			GameMenuStatus = 3;
		}
		else ComBuffer = OPTIONS_ADVANCED;
	}


	X = Setup.iAspectRatioWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Interface"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(INTERFACE);
			GameMenuStatus = 4;
		}
		else ComBuffer = INTERFACE;
	}


	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Config_Controls"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(CONFCONTROL);
			GameMenuStatus = 5;
		}
		else ComBuffer = CONFCONTROL;
	}







	if ((Options_Width == Setup.Width &&
		Options_Height == Setup.Height &&
		Options_BPP == Setup.BPP &&
		Options_VSync == Setup.VSync &&
		Options_iAspectRatioWidth == Setup.iAspectRatioWidth))
	{
		X = (Setup.iAspectRatioWidth - 384)/2;
		Y = Y+Prir;
		if (GameStatus == GAME)
		{
			if (DrawButton384(X,Y, vw_GetText("1_GAME_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
			{
				GameMenuStatus = 1;
			}
		}
		else
		{
			if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
			{
				ComBuffer = MAIN_MENU;
			}
		}
	}
	else
	{
		X = Setup.iAspectRatioWidth/2 - 256 - 38;
		Y = Y+Prir;
		if (GameStatus == GAME)
		{
			if (DrawButton256(X,Y, vw_GetText("1_GAME_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
			{
				GameMenuStatus = 1;
			}
		}
		else
		{
			if (DrawButton256(X,Y, vw_GetText("1_MAIN_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
			{
				ComBuffer = MAIN_MENU;
			}
		}
		X = Setup.iAspectRatioWidth/2 + 38;
		if (DrawButton256(X,Y, vw_GetText("1_APPLY"), ContentTransp, ButtonTransp2, LastButtonUpdateTime2))
		{
			// проверяем, нужно перегружать или нет
			if (Options_Width != Setup.Width ||
				Options_Height != Setup.Height ||
				Options_BPP != Setup.BPP ||
				Options_VSync != Setup.VSync ||
				Options_iAspectRatioWidth != Setup.iAspectRatioWidth)
			{
				if (GameStatus == GAME) SetCurrentDialogBox(17);
				else
				{
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
	Setup.Width = Options_Width;
	Setup.Height = Options_Height;

	// пока своего управление нет- делаем автоматическую установку
	if (Options_iAspectRatioWidth == 1024)
	{
		Setup.fAspectRatioWidth = 1024.0f;
		Setup.fAspectRatioHeight = 768.0f;
		Setup.iAspectRatioWidth = 1024;
		Setup.iAspectRatioHeight = 768;
	}
	else
	{
		Setup.fAspectRatioWidth = 1228.0f;
		Setup.fAspectRatioHeight = 768.0f;
		Setup.iAspectRatioWidth = 1228;
		Setup.iAspectRatioHeight = 768;
	}

	Setup.BPP = Options_BPP;
	Setup.VSync = Options_VSync;
}






