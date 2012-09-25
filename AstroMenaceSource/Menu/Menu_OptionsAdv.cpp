/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_OptionsAdv.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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
int Options_TexturesCompression;
int Options_UseGLSL;
int Options_MultiSampleType;

extern CSpaceStars *psSpaceStatic;

const char *ButtonQuality[3] =
{"3_High",
"3_Medium",
"3_Low"};


const char *ButtonTile[4] =
{"3_Disabled",
"3_1_Layer",
"3_2_Layers",
"3_3_Layers"};


const char *ButtonCompression[2] =
{"1_Off",
"1_On"};


const char *ButtonGLSL[2] =
{"1_Off",
"1_On"};


const char *ButtonPointLights[7] =
{"3_Disabled",
"3_1_Light",
"3_2_Lights",
"3_3_Lights",
"3_4_Lights",
"3_5_Lights",
"3_6_Lights"};






void OptionsAdvMenu()
{


	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);







	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;




	// качество испускание частиц... во сколько уменьшаем
	DrawFont(X1, Y1, 0, 0, 3, MenuContentTransp, GetText("3_Particles_GFX_Quality"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Setup.ParticlesPerSecQuality==3))
	{
		Setup.ParticlesPerSecQuality++;
		if (Setup.ParticlesPerSecQuality > 3) Setup.ParticlesPerSecQuality = 1;
		vw_SetParticleSystemStates(Setup.UseGLSL, Setup.ParticlesPerSecQuality*1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Setup.ParticlesPerSecQuality==1))
	{
		Setup.ParticlesPerSecQuality--;
		if (Setup.ParticlesPerSecQuality < 1) Setup.ParticlesPerSecQuality = 3;
		vw_SetParticleSystemStates(Setup.UseGLSL, Setup.ParticlesPerSecQuality*1.0f);
	}
	int Size = FontSize(GetText(ButtonQuality[Setup.ParticlesPerSecQuality-1]));
	int SizeI = (170-Size)/2;//High, Medium, Low
	DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText(ButtonQuality[Setup.ParticlesPerSecQuality-1]));






	// качество взрывов
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 3, MenuContentTransp, GetText("3_Explosions_GFX_Quality"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Setup.PartsExplosionQuality==2))
	{
		Setup.PartsExplosionQuality++;
		if (Setup.PartsExplosionQuality > 2) Setup.PartsExplosionQuality = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Setup.PartsExplosionQuality==0))
	{
		Setup.PartsExplosionQuality--;
		if (Setup.PartsExplosionQuality < 0) Setup.PartsExplosionQuality = 2;
	}
	Size = FontSize(GetText(ButtonQuality[Setup.PartsExplosionQuality]));
	SizeI = (170-Size)/2;//High, Medium, Low
	DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText(ButtonQuality[Setup.PartsExplosionQuality]));





	// качество прорисовки звезд на заднем плане
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 3, MenuContentTransp, GetText("3_Stars_Quantity"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Decrease"), MenuContentTransp, Setup.BackgroundStarsQuality==0))
	{
		Setup.BackgroundStarsQuality--;
		if (Setup.BackgroundStarsQuality<0) Setup.BackgroundStarsQuality = 0;

		if (psSpaceStatic!=0){delete psSpaceStatic; psSpaceStatic = 0;}
		psSpaceStatic = new CSpaceStars;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, Setup.BackgroundStarsQuality==10))
	{
		Setup.BackgroundStarsQuality++;
		if (Setup.BackgroundStarsQuality>10) Setup.BackgroundStarsQuality = 10;

		if (psSpaceStatic!=0){delete psSpaceStatic; psSpaceStatic = 0;}
		psSpaceStatic = new CSpaceStars;
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRest,0,0,16,32);
		SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.BackgroundStarsQuality>i)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, MenuContentTransp);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, MenuContentTransp);
	}






	// Тайловая анимация, слои
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 3, MenuContentTransp, GetText("3_Background_Tile_Animation"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Setup.BackgroundTileAnimation==0))
	{
		Setup.BackgroundTileAnimation--;
		if (Setup.BackgroundTileAnimation < 0) Setup.BackgroundTileAnimation = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Setup.BackgroundTileAnimation==3))
	{
		Setup.BackgroundTileAnimation++;
		if (Setup.BackgroundTileAnimation > 3) Setup.BackgroundTileAnimation = 0;
	}
	Size = FontSize(GetText(ButtonTile[Setup.BackgroundTileAnimation]));
	SizeI = (170-Size)/2;
	DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText(ButtonTile[Setup.BackgroundTileAnimation]));






	// Максимальное кол-во источников света на 1 объекта
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 4, MenuContentTransp, GetText("3_Point_Lights_per_Object"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Setup.MaxPointLights==0))
	{
		Setup.MaxPointLights--;
		if (Setup.MaxPointLights < 0) Setup.MaxPointLights = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Setup.MaxPointLights==6))
	{
		Setup.MaxPointLights++;
		if (Setup.MaxPointLights > 6) Setup.MaxPointLights = 6;
	}
	Size = FontSize(GetText(ButtonPointLights[Setup.MaxPointLights]));
	SizeI = (170-Size)/2;
	DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText(ButtonPointLights[Setup.MaxPointLights]));








	// анизотропия
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 4, MenuContentTransp, GetText("3_Anisotropy_Level"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, Setup.AnisotropyLevel==1))
	{
		Setup.AnisotropyLevel = (int)(Setup.AnisotropyLevel/2);
		if (Setup.AnisotropyLevel < 1) Setup.AnisotropyLevel = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, Setup.AnisotropyLevel==CAPS->MaxAnisotropyLevel))
	{
		Setup.AnisotropyLevel = Setup.AnisotropyLevel*2;
		if (Setup.AnisotropyLevel > CAPS->MaxAnisotropyLevel) Setup.AnisotropyLevel = CAPS->MaxAnisotropyLevel;
	}
	if (Setup.AnisotropyLevel>1)
	{
		Size = FontSize("x%i", Setup.AnisotropyLevel);
		SizeI = (170-Size)/2;
		DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "x%i",Setup.AnisotropyLevel);
	}
	else
	{
		if (CAPS->MaxAnisotropyLevel > 1)
		{
			Size = FontSize(GetText("1_Off"));
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText("1_Off"));
		}
		else
		{
			Size = FontSize(GetText("3_Not_available"));
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("3_Not_available"));
		}
	}




	//Options_MultiSampleType
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 4, MenuContentTransp, GetText("3_Multisample_Antialiasing"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, CAPS->MaxMultiSampleType==0 || Options_MultiSampleType==0))
	{
		if (Options_MultiSampleType == 0) Options_MultiSampleType = CAPS->MaxMultiSampleType;
		else
			if (Options_MultiSampleType == 2) Options_MultiSampleType = 0;
			else
				if (Options_MultiSampleType == 4) Options_MultiSampleType = 2;
				else
					if (Options_MultiSampleType == 8) Options_MultiSampleType = 4;
					else
						if (Options_MultiSampleType == 16) Options_MultiSampleType = 8;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, CAPS->MaxMultiSampleType==0 || Options_MultiSampleType==CAPS->MaxMultiSampleType))
	{
		if (Options_MultiSampleType == 8) Options_MultiSampleType = 16;
		else
			if (Options_MultiSampleType == 4) Options_MultiSampleType = 8;
			else
				if (Options_MultiSampleType == 2) Options_MultiSampleType = 4;
				else
					if (Options_MultiSampleType == 0) Options_MultiSampleType = 2;
					else
						if (Options_MultiSampleType == 16) Options_MultiSampleType = 0;

		if (Options_MultiSampleType < 0) Options_MultiSampleType = CAPS->MaxMultiSampleType;
	}
	if (Options_MultiSampleType == 0)
	{
		if (CAPS->MaxMultiSampleType == 0)
		{
			Size = FontSize(GetText("3_Not_available"));
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("3_Not_available"));
		}
		else
		{
			Size = FontSize(GetText("1_Off"));
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText("1_Off"));
		}
	}
	else
	{
		Size = FontSize("%ix",Options_MultiSampleType);
		SizeI = (170-Size)/2;//Off, 2x, 4x ...
		DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "%ix", Options_MultiSampleType);
	}




	// вкл-выкл компрессии текстур
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 4, MenuContentTransp, GetText("3_Textures_Compression"));
	if (DrawButton128_2(X1+458, Y1-6, GetText(ButtonCompression[Options_TexturesCompression]), MenuContentTransp, !CAPS->TexturesCompression))
	{
		Options_TexturesCompression++;
		if (Options_TexturesCompression > 1) Options_TexturesCompression = 0;
	}





	// вкл-выкл шейдеров, если они поддерживаются
	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 2, MenuContentTransp, GetText("3_OpenGL_Shading_Language"));
	int tmp = 0;
	if (Options_UseGLSL) tmp = 1;
	if (DrawButton128_2(X1+458, Y1-6, GetText(ButtonGLSL[tmp]), MenuContentTransp, !CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0))
	{
		Options_UseGLSL = !Options_UseGLSL;
	}
















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;

	X = Setup.iAspectRatioWidth/2 - 366;
	DrawButton200_2(X,Y+28, GetText("1_Advanced"), MenuContentTransp, true);

	X = Setup.iAspectRatioWidth/2 - 100;
	char Text[1024];
	sprintf(Text,GetText("1_Video_%s_Audio"),"&");
	if (DrawButton200_2(X,Y+28, Text, MenuContentTransp, false))
	{
		ComBuffer = OPTIONS;
	}

	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, GetText("1_Config_Controls"), MenuContentTransp, false))
	{
		ComBuffer = CONFCONTROL;
	}








	if (Options_TexturesCompression == Setup.TexturesCompression &&
		Options_MultiSampleType == Setup.MultiSampleType &&
		Options_UseGLSL == Setup.UseGLSL)
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
			if (Options_TexturesCompression != Setup.TexturesCompression ||
				Options_MultiSampleType != Setup.MultiSampleType ||
				Options_UseGLSL != Setup.UseGLSL)
			{
				CanQuit = false;
				Quit = true;
				NeedReCreate = true;
			}

			Setup.UseGLSL = Options_UseGLSL;
			Setup.TexturesCompression = Options_TexturesCompression;
			Setup.MultiSampleType = Options_MultiSampleType;
		}
	}
}









