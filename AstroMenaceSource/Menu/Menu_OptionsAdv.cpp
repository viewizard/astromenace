/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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
int Options_TexturesCompression;
int Options_UseGLSL;
int Options_MSAA;
int Options_CSAA;
int Options_ShadowMap;
int Options_TexturesQuality;

extern CSpaceStars *psSpaceStatic;

const char *ButtonQuality[3] =
{"3_High",
"3_Medium",
"3_Low"};

const char *ShadowButtonQuality[10] =
{"1_Off",
"3_Low",
"3_Low",
"3_Low",
"3_Medium",
"3_Medium",
"3_Medium",
"3_High",
"3_High",
"3_High"};

const char *ShadowButtonQualityBase[10] =
{"%s",
"%s, 2x2 PCF",
"%s, 4x4 PCF",
"%s, 8x8 PCF",
"%s, 2x2 PCF",
"%s, 4x4 PCF",
"%s, 8x8 PCF",
"%s, 2x2 PCF",
"%s, 4x4 PCF",
"%s, 8x8 PCF"};


const char *ButtonTile[3] =
{"3_Disabled",
"3_1_Layer",
"3_2_Layers"};


const char *ButtonTextFiltr[2] =
{"3_Bilinear",
"3_Trilinear"};


const char *ButtonPointLights[7] =
{"3_Disabled",
"3_1_Light",
"3_2_Lights",
"3_3_Lights",
"3_4_Lights",
"3_5_Lights",
"3_6_Lights"};


const char *ButtonTexturesQuality[3] =
{"3_Low",
"3_Medium",
"3_High"};






void OptionsAdvMenu()
{


	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);







	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;




	// качество визуальных эффектов
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_Visual_Effects_Quality"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Setup.VisualEffectsQuality==2))
	{
		Setup.VisualEffectsQuality++;
		if (Setup.VisualEffectsQuality > 2) Setup.VisualEffectsQuality = 0;

		vw_InitParticleSystems(Setup.UseGLSL, Setup.VisualEffectsQuality+1.0f);

		if (psSpaceStatic!=0){delete psSpaceStatic; psSpaceStatic = 0;}
		psSpaceStatic = new CSpaceStars;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Setup.VisualEffectsQuality==0))
	{
		Setup.VisualEffectsQuality--;
		if (Setup.VisualEffectsQuality < 0) Setup.VisualEffectsQuality = 2;

		vw_InitParticleSystems(Setup.UseGLSL, Setup.VisualEffectsQuality+1.0f);

		if (psSpaceStatic!=0){delete psSpaceStatic; psSpaceStatic = 0;}
		psSpaceStatic = new CSpaceStars;
	}
	int Size = vw_FontSize(vw_GetText(ButtonQuality[Setup.VisualEffectsQuality]));
	int SizeI = (170-Size)/2;//High, Medium, Low
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(ButtonQuality[Setup.VisualEffectsQuality]));







	// Максимальное кол-во источников света на 1 объекта
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_Point_Lights_per_Object"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Setup.MaxPointLights==0))
	{
		Setup.MaxPointLights--;
		if (Setup.MaxPointLights < 0) Setup.MaxPointLights = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Setup.MaxPointLights==6))
	{
		Setup.MaxPointLights++;
		if (Setup.MaxPointLights > 6) Setup.MaxPointLights = 6;
	}
	Size = vw_FontSize(vw_GetText(ButtonPointLights[Setup.MaxPointLights]));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(ButtonPointLights[Setup.MaxPointLights]));







	// качество текстур
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_Textures_Quality"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Options_TexturesQuality==1))
	{
		Options_TexturesQuality--;
		if (Options_TexturesQuality < 1) Options_TexturesQuality = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Options_TexturesQuality==3))
	{
		Options_TexturesQuality++;
		if (Options_TexturesQuality > 3) Options_TexturesQuality = 1;
	}
	Size = vw_FontSize(vw_GetText(ButtonTexturesQuality[Options_TexturesQuality-1]));
	SizeI = (170-Size)/2;//High, Medium, Low
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(ButtonTexturesQuality[Options_TexturesQuality-1]));







	// тип фильтрации текстуры
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_Texture_Filtering_Mode"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Setup.TextureFilteringMode==1))
	{
		Setup.TextureFilteringMode--;
		if (Setup.TextureFilteringMode < 1) Setup.TextureFilteringMode = 2;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Setup.TextureFilteringMode==2))
	{
		Setup.TextureFilteringMode++;
		if (Setup.TextureFilteringMode > 2) Setup.TextureFilteringMode = 1;
	}
	Size = vw_FontSize(vw_GetText(ButtonTextFiltr[Setup.TextureFilteringMode-1]));
	SizeI = (170-Size)/2;//Bilinear, Trilinear
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(ButtonTextFiltr[Setup.TextureFilteringMode-1]));







	// анизотропия
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_Anisotropy_Level"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Setup.AnisotropyLevel==1))
	{
		Setup.AnisotropyLevel = (int)(Setup.AnisotropyLevel/2);
		if (Setup.AnisotropyLevel < 1) Setup.AnisotropyLevel = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Setup.AnisotropyLevel==CAPS->MaxAnisotropyLevel))
	{
		Setup.AnisotropyLevel = Setup.AnisotropyLevel*2;
		if (Setup.AnisotropyLevel > CAPS->MaxAnisotropyLevel) Setup.AnisotropyLevel = CAPS->MaxAnisotropyLevel;
	}
	if (Setup.AnisotropyLevel>1)
	{
		Size = vw_FontSize("x%i", Setup.AnisotropyLevel);
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "x%i",Setup.AnisotropyLevel);
	}
	else
	{
		if (CAPS->MaxAnisotropyLevel > 1)
		{
			Size = vw_FontSize(vw_GetText("1_Off"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("1_Off"));
		}
		else
		{
			Size = vw_FontSize(vw_GetText("3_Not_available"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, vw_GetText("3_Not_available"));
		}
	}




	// вкл-выкл компрессии текстур
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_Textures_Compression"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, !CAPS->TexturesCompression) || DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, !CAPS->TexturesCompression))
	{
		Options_TexturesCompression++;
		if (Options_TexturesCompression > 1) Options_TexturesCompression = 0;
	}
	Size = vw_FontSize(Options_TexturesCompression ? vw_GetText("1_On") : vw_GetText("1_Off"));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Options_TexturesCompression ? vw_GetText("1_On") : vw_GetText("1_Off"));





	//Options_MultiSampleType
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, vw_GetText("3_Multisample_Antialiasing"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, (Options_MSAA == 0) || (CAPS->MaxSamples == 0)))
	{
		// находим текущий режим
		int CurrentMode = 0;
		if (Options_MSAA == 0) CurrentMode = -1;
		else
		{
			for (int i=0;i<CAPS->MaxMultisampleCoverageModes; i++)
			{
				if ((CAPS->MultisampleCoverageModes[i].ColorSamples == Options_MSAA) &
					(CAPS->MultisampleCoverageModes[i].CoverageSamples == Options_CSAA))
					{
						CurrentMode = i;
						break;
					}
			}
		}

		CurrentMode --;
		if (CurrentMode < -1) CurrentMode = CAPS->MaxMultisampleCoverageModes-1;

		// -1 - делаем "выключение" антиалиасинга
		if (CurrentMode == -1)
		{
			Options_MSAA = 0;
			Options_CSAA = 0;
		}
		else
		{
			Options_MSAA = CAPS->MultisampleCoverageModes[CurrentMode].ColorSamples;
			Options_CSAA = CAPS->MultisampleCoverageModes[CurrentMode].CoverageSamples;
		}
	}
	bool TestStateButton = false;
	if (CAPS->MaxMultisampleCoverageModes > 0) TestStateButton = (CAPS->MultisampleCoverageModes[CAPS->MaxMultisampleCoverageModes-1].ColorSamples == Options_MSAA) &
												(CAPS->MultisampleCoverageModes[CAPS->MaxMultisampleCoverageModes-1].CoverageSamples == Options_CSAA);
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, TestStateButton || (CAPS->MaxSamples == 0)))
	{
		// находим текущий режим
		int CurrentMode = 0;
		if (Options_MSAA == 0) CurrentMode = -1;
		else
		{
			for (int i=0;i<CAPS->MaxMultisampleCoverageModes; i++)
			{
				if ((CAPS->MultisampleCoverageModes[i].ColorSamples == Options_MSAA) &
					(CAPS->MultisampleCoverageModes[i].CoverageSamples == Options_CSAA))
					{
						CurrentMode = i;
						break;
					}
			}
		}

		CurrentMode ++;
		if (CurrentMode > CAPS->MaxMultisampleCoverageModes-1) CurrentMode = -1;

		// -1 - делаем "выключение" антиалиасинга
		if (CurrentMode == -1)
		{
			Options_MSAA = 0;
			Options_CSAA = 0;
		}
		else
		{
			Options_MSAA = CAPS->MultisampleCoverageModes[CurrentMode].ColorSamples;
			Options_CSAA = CAPS->MultisampleCoverageModes[CurrentMode].CoverageSamples;
		}
	}
	if (Options_MSAA == 0)
	{
		if (CAPS->MaxSamples == 0)
		{
			Size = vw_FontSize(vw_GetText("3_Not_available"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, vw_GetText("3_Not_available"));
		}
		else
		{
			Size = vw_FontSize(vw_GetText("1_Off"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("1_Off"));
		}
	}
	else
	{	if ((Options_MSAA == Options_CSAA) || (Options_CSAA == 0))
		{
			Size = vw_FontSize("%ix MS",Options_MSAA);
			SizeI = (170-Size)/2;//Off, 2x, 4x ...
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%ix MS", Options_MSAA);
		}
		else
		{
			Size = vw_FontSize("%ix CS/%ix MS",Options_CSAA,Options_MSAA);
			SizeI = (170-Size)/2;//Off, 2x, 4x ...
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%ix CS/%ix MS",Options_CSAA,Options_MSAA);
		}
	}




	// вкл-выкл шейдеров, если они поддерживаются
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,0.0f,0.0f, MenuContentTransp, vw_GetText("3_OpenGL_Shading_Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, !CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0) || DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, !CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0))
	{
		Options_UseGLSL = !Options_UseGLSL;
		// если выключены шейдеры - выключаем и тени
		if (!Options_UseGLSL) Options_ShadowMap = 0;
		else Options_ShadowMap = Setup.ShadowMap;
	}
	if (CAPS->GLSL100Supported && (CAPS->ShaderModel >= 3.0))
	{
		Size = vw_FontSize(Options_UseGLSL ? vw_GetText("1_On") : vw_GetText("1_Off"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Options_UseGLSL ? vw_GetText("1_On") : vw_GetText("1_Off"));
	}
	else
	{
		Size = vw_FontSize(vw_GetText("3_Not_available"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, vw_GetText("3_Not_available"));
	}





	// качество теней
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,0.0f,0.0f, MenuContentTransp, vw_GetText("3_Shadow_Quality"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Options_ShadowMap==0 || !CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0 || !CAPS->FramebufferObject || !Options_UseGLSL || (CAPS->FramebufferObjectDepthSize < 24)))
	{
		Options_ShadowMap--;
		if (Options_ShadowMap < 0) Options_ShadowMap = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Options_ShadowMap==9 || !CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0 || !CAPS->FramebufferObject || !Options_UseGLSL || (CAPS->FramebufferObjectDepthSize < 24)))
	{
		Options_ShadowMap++;
		if (Options_ShadowMap > 9) Options_ShadowMap = 9;
	}
	if (CAPS->GLSL100Supported && (CAPS->ShaderModel >= 3.0) && CAPS->FramebufferObject && Options_UseGLSL && (CAPS->FramebufferObjectDepthSize >= 24))
	{
		Size = vw_FontSize(ShadowButtonQualityBase[Options_ShadowMap], vw_GetText(ShadowButtonQuality[Options_ShadowMap]));
		float WScale = 0;
		if (Size > 170)
		{
			Size = 170;
			WScale = -170;
		}
		SizeI = (170-Size)/2;//High, Medium, Low
		vw_DrawFont(X1+438+SizeI, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, ShadowButtonQualityBase[Options_ShadowMap], vw_GetText(ShadowButtonQuality[Options_ShadowMap]));
	}
	else
	{
		Size = vw_FontSize(vw_GetText("3_Not_available"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, MenuContentTransp, vw_GetText("3_Not_available"));
	}









	int Prir = 100;
	int X;
	int Y = 165+Prir*4;

	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Interface"), MenuContentTransp, false))
	{
		ComBuffer = INTERFACE;
	}

	X = Setup.iAspectRatioWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Video_&_Audio"), MenuContentTransp, false))
	{
		ComBuffer = OPTIONS;
	}

	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Config_Controls"), MenuContentTransp, false))
	{
		ComBuffer = CONFCONTROL;
	}








	if (Options_TexturesCompression == Setup.TexturesCompression &&
		Options_MSAA == Setup.MSAA &&
		Options_CSAA == Setup.CSAA &&
		Options_UseGLSL == Setup.UseGLSL &&
		Options_TexturesQuality == Setup.TexturesQuality &&
		Options_ShadowMap == Setup.ShadowMap)
	{
		X = (Setup.iAspectRatioWidth - 384)/2;
		Y = Y+Prir;
		if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
	}
	else
	{
		X = Setup.iAspectRatioWidth/2 - 256 - 38;
		Y = Y+Prir;
		if (DrawButton256(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
		X = Setup.iAspectRatioWidth/2 + 38;
		if (DrawButton256(X,Y, vw_GetText("1_APPLY"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime))
		{
			// проверяем, нужно перегружать или нет
			if (Options_TexturesCompression != Setup.TexturesCompression ||
				Options_MSAA != Setup.MSAA ||
				Options_CSAA != Setup.CSAA ||
				Options_UseGLSL != Setup.UseGLSL ||
				Options_TexturesQuality != Setup.TexturesQuality ||
				Options_ShadowMap != Setup.ShadowMap)
			{
				CanQuit = false;
				Quit = true;
				NeedReCreate = true;
			}

			Setup.UseGLSL = Options_UseGLSL;
			Setup.TexturesCompression = Options_TexturesCompression;
			Setup.MSAA = Options_MSAA;
			Setup.CSAA = Options_CSAA;
			Setup.ShadowMap = Options_ShadowMap;
			Setup.TexturesQuality = Options_TexturesQuality;
		}
	}
}









