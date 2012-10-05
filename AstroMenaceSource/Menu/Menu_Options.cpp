/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Options.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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


#include "../Game.h"


// временные данные для изменения и восстановления
int Options_Width;
int Options_Height;
int Options_BPP;
int Options_TexturesQuality;
int Options_iAspectRatioWidth;


const char *ButtonFullScreen[2] =
{"1_On",
"1_Off"};


const char *ButtonScreenModeTitle[2] =
{"3_Screen_Mode",
"3_Window_Size"};

const char *ButtonTextFiltr[2] =
{"3_Bilinear",
"3_Trilinear"};

const char *ButtonTexturesQuality[3] =
{"3_Low",
"3_Medium",
"3_High"};



















void OptionsMenu()
{

	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);


	int CurrentPos = 0;

	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;


	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Music_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Decrease"), MenuContentTransp, !Setup.Music_check || Setup.MusicSw==0))
	{
		Setup.MusicSw--;
		if (Setup.MusicSw<0) Setup.MusicSw = 0;
		vw_SetMusicMainVolume(Setup.MusicSw/10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, !Setup.Music_check || Setup.MusicSw==10))
	{
		Setup.MusicSw++;
		if (Setup.MusicSw>10) Setup.MusicSw = 10;
		vw_SetMusicMainVolume(Setup.MusicSw/10.0f);
	}
	if (!Setup.Music_check)
	{
		int SizeI = (170-vw_FontSize(GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRest,0,0,16,32);
			SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.MusicSw>i)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, MenuContentTransp);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, MenuContentTransp);
		}
	}






	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Voice_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Decrease"), MenuContentTransp, !Setup.Sound_check || Setup.VoiceSw==0, false))
	{
		Setup.VoiceSw--;
		if (Setup.VoiceSw<0) Setup.VoiceSw = 0;
		Audio_SetVoiceMainVolume(Setup.VoiceSw/10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, !Setup.Sound_check || Setup.VoiceSw==10, false))
	{
		Setup.VoiceSw++;
		if (Setup.VoiceSw>10) Setup.VoiceSw = 10;
		Audio_SetVoiceMainVolume(Setup.VoiceSw/10.0f);
		Audio_PlayVoice(1, 1.0f);
	}
	if (!Setup.Sound_check)
	{
		int SizeI = (170-vw_FontSize(GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRest,0,0,16,32);
			SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.VoiceSw>i)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, MenuContentTransp);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, MenuContentTransp);
		}
	}





	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Sound_Effects_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Decrease"), MenuContentTransp, !Setup.Sound_check || Setup.SoundSw==0))
	{
		Setup.SoundSw--;
		if (Setup.SoundSw<0) Setup.SoundSw = 0;
		Audio_SetSound2DMainVolume(Setup.SoundSw/10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, !Setup.Sound_check || Setup.SoundSw==10))
	{
		Setup.SoundSw++;
		if (Setup.SoundSw>10) Setup.SoundSw = 10;
		Audio_SetSound2DMainVolume(Setup.SoundSw/10.0f);
	}
	if (!Setup.Sound_check)
	{
		int SizeI = (170-vw_FontSize(GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRest,0,0,16,32);
			SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.SoundSw>i)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, MenuContentTransp);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, MenuContentTransp);
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
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Full_Screen"));
	if (DrawButton128_2(X1+458, Y1-6, GetText(ButtonFullScreen[CurrentPos]), MenuContentTransp, !CanSwitchToFullScreen))
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



	// находим текущий номер режима в списке
	int CurrentListNum = 0;
	for(int i=0; i<VideoModesNum; i++)
	{
		if (VideoModes[i].W == Options_Width &&
			VideoModes[i].H == Options_Height &&
			VideoModes[i].BPP == Options_BPP)
		{
			CurrentListNum = i;
		}
	}



	Y1 += Prir1;
	if (Options_BPP != 0) CurrentPos = 0;
	else CurrentPos = 1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText(ButtonScreenModeTitle[CurrentPos]));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, false))
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
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, false))
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

	int Size = vw_FontSize(VideoModeTitle);
	int SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, VideoModeTitle);



	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Aspect_Ration"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, false))
	{
		if (Options_iAspectRatioWidth == 1228) Options_iAspectRatioWidth = 1024;
		else Options_iAspectRatioWidth = 1228;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, false))
	{
		if (Options_iAspectRatioWidth == 1024) Options_iAspectRatioWidth = 1228;
		else Options_iAspectRatioWidth = 1024;
	}
	if (Options_iAspectRatioWidth == 1024)
	{
		Size = vw_FontSize(GetText("3_Standard"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Standard"));
	}
	else
	{
		Size = vw_FontSize(GetText("3_Widescreen"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Widescreen"));
	}







	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Gamma"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Decrease"), MenuContentTransp, Setup.BPP == 0 || Setup.Gamma==0))
	{
		Setup.Gamma -= 1;
		if (Setup.Gamma < 0) Setup.Gamma = 0;
		if (Setup.BPP != 0)
			vw_SetGammaRamp(1.0f + ((Setup.Gamma-2)/10.0f), 1.0f, 1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, Setup.BPP == 0 || Setup.Gamma==10))
	{
		Setup.Gamma += 1;
		if (Setup.Gamma > 10) Setup.Gamma = 10;
		if (Setup.BPP != 0)
			vw_SetGammaRamp(1.0f + ((Setup.Gamma-2)/10.0f), 1.0f, 1.0f);
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRest,0,0,16,32);
		SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.Gamma>i)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, MenuContentTransp);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, MenuContentTransp);
	}






	// тип фильтрации текстуры
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Texture_Filtering_Mode"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Setup.TextureFilteringMode==1))
	{
		Setup.TextureFilteringMode--;
		if (Setup.TextureFilteringMode < 1) Setup.TextureFilteringMode = 2;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Setup.TextureFilteringMode==2))
	{
		Setup.TextureFilteringMode++;
		if (Setup.TextureFilteringMode > 2) Setup.TextureFilteringMode = 1;
	}
	Size = vw_FontSize(GetText(ButtonTextFiltr[Setup.TextureFilteringMode-1]));
	SizeI = (170-Size)/2;//Bilinear, Trilinear
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText(ButtonTextFiltr[Setup.TextureFilteringMode-1]));





	// качество текстур
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Textures_Quality"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Options_TexturesQuality==1))
	{
		Options_TexturesQuality--;
		if (Options_TexturesQuality < 1) Options_TexturesQuality = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Options_TexturesQuality==3))
	{
		Options_TexturesQuality++;
		if (Options_TexturesQuality > 3) Options_TexturesQuality = 1;
	}
	Size = vw_FontSize(GetText(ButtonTexturesQuality[Options_TexturesQuality-1]));
	SizeI = (170-Size)/2;//High, Medium, Low
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText(ButtonTexturesQuality[Options_TexturesQuality-1]));








	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, GetText("1_Advanced"), MenuContentTransp, false))
	{
		ComBuffer = OPTIONS_ADVANCED;
	}


	X = Setup.iAspectRatioWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, GetText("1_Interface"), MenuContentTransp, false))
	{
		ComBuffer = INTERFACE;
	}


	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, GetText("1_Config_Controls"), MenuContentTransp, false))
	{
		ComBuffer = CONFCONTROL;
	}







	if ((Options_Width == Setup.Width &&
		Options_Height == Setup.Height &&
		Options_BPP == Setup.BPP &&
		Options_TexturesQuality == Setup.TexturesQuality &&
		Options_iAspectRatioWidth == Setup.iAspectRatioWidth))
	{
		X = (Setup.iAspectRatioWidth - 384)/2;
		Y = Y+Prir;
		if (DrawButton384(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
	}
	else
	{
		X = Setup.iAspectRatioWidth/2 - 256 - 38;
		Y = Y+Prir;
		if (DrawButton256(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
		X = Setup.iAspectRatioWidth/2 + 38;
		if (DrawButton256(X,Y, GetText("1_APPLY"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime))
		{
			// проверяем, нужно перегружать или нет
			if (Options_Width != Setup.Width ||
				Options_Height != Setup.Height ||
				Options_BPP != Setup.BPP ||
				Options_TexturesQuality != Setup.TexturesQuality ||
				Options_iAspectRatioWidth != Setup.iAspectRatioWidth)
			{
				CanQuit = false;
				Quit = true;
				NeedReCreate = true;
			}

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
			Setup.TexturesQuality = Options_TexturesQuality;
		}
	}


}









