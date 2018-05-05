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


// временные данные для изменения и восстановления
int Options_TexturesAnisotropyLevel;
int Options_TexturesCompressionType;
int Options_UseGLSL120;
int Options_MSAA;
int Options_CSAA;
int Options_ShadowMap;

const char *ButtonQuality[3] = {
	"High",
	"Medium",
	"Low"
};

const char *ShadowButtonQuality[10] = {
	"Off",
	"8x8 PCF" // don't need translation
};

const char *ButtonTextCompression[3] = {
	"Off",
	"S3TC", // don't need translation
	"BPTC" // don't need translation
};


const char *ButtonPointLights[7] = {
	"Disabled",
	"1 Light",
	"2 Lights",
	"3 Lights",
	"4 Lights",
	"5 Lights",
	"6 Lights"
};


const char *ButtonTexturesQuality[3] = {
	"Low",
	"Medium",
	"High"
};






void OptionsAdvMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1, float *ButtonTransp2, float *LastButtonUpdateTime2)
{
	sRECT SrcRect{0, 0, 2, 2};
	sRECT DstRect{0, 0, static_cast<int>(Setup.InternalWidth), 768};
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f*ContentTransp);







	int X1 = Setup.InternalWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;




	// качество визуальных эффектов
	// VisualEffectsQuality is inverted (0 - all effects, 2 - minimum effects)
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, ContentTransp, vw_GetText("Visual Effects Quality"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, Setup.VisualEffectsQuality==2)) {
		Setup.VisualEffectsQuality++;
		if (Setup.VisualEffectsQuality > 2) Setup.VisualEffectsQuality = 0;

		vw_InitParticleSystems(Setup.UseGLSL120, Setup.VisualEffectsQuality+1.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, Setup.VisualEffectsQuality==0)) {
		Setup.VisualEffectsQuality--;
		if (Setup.VisualEffectsQuality < 0) Setup.VisualEffectsQuality = 2;

		vw_InitParticleSystems(Setup.UseGLSL120, Setup.VisualEffectsQuality+1.0f);
	}
	int Size = vw_FontSize(vw_GetText(ButtonQuality[Setup.VisualEffectsQuality]));
	int SizeI = (170-Size)/2;//High, Medium, Low
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ButtonQuality[Setup.VisualEffectsQuality]));







	// Максимальное кол-во источников света на 1 объекта
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, ContentTransp, vw_GetText("Point Lights per Object"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, Setup.MaxPointLights==0)) {
		Setup.MaxPointLights--;
		if (Setup.MaxPointLights < 0) Setup.MaxPointLights = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, Setup.MaxPointLights==6)) {
		Setup.MaxPointLights++;
		if (Setup.MaxPointLights > 6) Setup.MaxPointLights = 6;
	}
	Size = vw_FontSize(vw_GetText(ButtonPointLights[Setup.MaxPointLights]));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ButtonPointLights[Setup.MaxPointLights]));





	// анизотропия
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, ContentTransp, vw_GetText("Anisotropy Level"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, Options_TexturesAnisotropyLevel == 1)) {
		Options_TexturesAnisotropyLevel >>= 1;
		if (Options_TexturesAnisotropyLevel < 1)
			Options_TexturesAnisotropyLevel = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, Options_TexturesAnisotropyLevel == vw_GetDevCaps().MaxAnisotropyLevel)) {
		Options_TexturesAnisotropyLevel <<= 1;
		if (Options_TexturesAnisotropyLevel > vw_GetDevCaps().MaxAnisotropyLevel)
			Options_TexturesAnisotropyLevel = vw_GetDevCaps().MaxAnisotropyLevel;
	}
	if (Options_TexturesAnisotropyLevel > 1) {
		Size = vw_FontSize("x%i", Options_TexturesAnisotropyLevel);
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, "x%i", Options_TexturesAnisotropyLevel);
	} else {
		if (vw_GetDevCaps().MaxAnisotropyLevel > 1) {
			Size = vw_FontSize(vw_GetText("Off"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("Off"));
		} else {
			Size = vw_FontSize(vw_GetText("Not available"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("Not available"));
		}
	}




	// вкл-выкл компрессии текстур
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 0.0f,1.0f,0.0f, ContentTransp, vw_GetText("Textures Compression"));
	int MaxCompressionCount = 1;
	if (vw_GetDevCaps().ARB_texture_compression_bptc)
		MaxCompressionCount = 2;
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, !vw_GetDevCaps().EXT_texture_compression_s3tc || (Options_TexturesCompressionType <= 0))) {
		Options_TexturesCompressionType--;
		if (Options_TexturesCompressionType < 0) Options_TexturesCompressionType = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, !vw_GetDevCaps().EXT_texture_compression_s3tc || (Options_TexturesCompressionType >= MaxCompressionCount))) {
		Options_TexturesCompressionType++;
		if (Options_TexturesCompressionType > MaxCompressionCount) Options_TexturesCompressionType = MaxCompressionCount;
	}
	Size = vw_FontSize(vw_GetText(ButtonTextCompression[Options_TexturesCompressionType]));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ButtonTextCompression[Options_TexturesCompressionType]));





	//Options_MultiSampleType
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("Multisample Antialiasing"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, (Options_MSAA == 0) || vw_GetDevCaps().MultisampleCoverageModes.empty())) {
		// находим текущий режим
		int CurrentMode = 0;
		if (Options_MSAA == 0) CurrentMode = -1;
		else {
			for (unsigned int i = 0; i < vw_GetDevCaps().MultisampleCoverageModes.size(); i++) {
				if ((vw_GetDevCaps().MultisampleCoverageModes[i].ColorSamples == Options_MSAA) &&
				    (vw_GetDevCaps().MultisampleCoverageModes[i].CoverageSamples == Options_CSAA)) {
					CurrentMode = i;
					break;
				}
			}
		}

		CurrentMode --;
		if (CurrentMode < -1)
			CurrentMode = vw_GetDevCaps().MultisampleCoverageModes.size() - 1;

		// -1 - делаем "выключение" антиалиасинга
		if (CurrentMode == -1) {
			Options_MSAA = 0;
			Options_CSAA = 0;
		} else {
			Options_MSAA = vw_GetDevCaps().MultisampleCoverageModes[CurrentMode].ColorSamples;
			Options_CSAA = vw_GetDevCaps().MultisampleCoverageModes[CurrentMode].CoverageSamples;
		}
	}
	bool TestStateButton = false;
	if (!vw_GetDevCaps().MultisampleCoverageModes.empty())
		TestStateButton = (vw_GetDevCaps().MultisampleCoverageModes.back().ColorSamples == Options_MSAA) &&
				(vw_GetDevCaps().MultisampleCoverageModes.back().CoverageSamples == Options_CSAA);
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, TestStateButton || vw_GetDevCaps().MultisampleCoverageModes.empty())) {
		// находим текущий режим
		int CurrentMode = 0;
		if (Options_MSAA == 0) CurrentMode = -1;
		else {
			for (unsigned int i = 0; i < vw_GetDevCaps().MultisampleCoverageModes.size(); i++) {
				if ((vw_GetDevCaps().MultisampleCoverageModes[i].ColorSamples == Options_MSAA) &&
				    (vw_GetDevCaps().MultisampleCoverageModes[i].CoverageSamples == Options_CSAA)) {
					CurrentMode = i;
					break;
				}
			}
		}

		CurrentMode ++;
		if (CurrentMode > (int)vw_GetDevCaps().MultisampleCoverageModes.size() - 1)
			CurrentMode = -1;

		// -1 - делаем "выключение" антиалиасинга
		if (CurrentMode == -1) {
			Options_MSAA = 0;
			Options_CSAA = 0;
		} else {
			Options_MSAA = vw_GetDevCaps().MultisampleCoverageModes[CurrentMode].ColorSamples;
			Options_CSAA = vw_GetDevCaps().MultisampleCoverageModes[CurrentMode].CoverageSamples;
		}
	}
	if (Options_MSAA == 0) {
		if (vw_GetDevCaps().MultisampleCoverageModes.empty()) {
			Size = vw_FontSize(vw_GetText("Not available"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("Not available"));
		} else {
			Size = vw_FontSize(vw_GetText("Off"));
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("Off"));
		}
	} else {
		if ((Options_MSAA == Options_CSAA) || (Options_CSAA == 0)) {
			Size = vw_FontSize("%ix MS",Options_MSAA);
			SizeI = (170-Size)/2;//Off, 2x, 4x ...
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, "%ix MS", Options_MSAA);
		} else {
			Size = vw_FontSize("%ix CS/%ix MS",Options_CSAA,Options_MSAA);
			SizeI = (170-Size)/2;//Off, 2x, 4x ...
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, "%ix CS/%ix MS",Options_CSAA,Options_MSAA);
		}
	}




	// вкл-выкл шейдеров, если они поддерживаются
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,0.0f,0.0f, ContentTransp, vw_GetText("OpenGL Shading Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, !vw_GetDevCaps().OpenGL_2_0_supported || !vw_GetDevCaps().OpenGL_2_1_supported) ||
	    DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, !vw_GetDevCaps().OpenGL_2_0_supported || !vw_GetDevCaps().OpenGL_2_1_supported)) {
		Options_UseGLSL120 = !Options_UseGLSL120;
		// если выключены шейдеры - выключаем и тени
		if (!Options_UseGLSL120)
			Options_ShadowMap = 0;
		else
			Options_ShadowMap = Setup.ShadowMap;
	}
	if (vw_GetDevCaps().OpenGL_2_0_supported && vw_GetDevCaps().OpenGL_2_1_supported) {
		Size = vw_FontSize(Options_UseGLSL120 ? vw_GetText("On") : vw_GetText("Off"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, Options_UseGLSL120 ? vw_GetText("On") : vw_GetText("Off"));
	} else {
		Size = vw_FontSize(vw_GetText("Not available"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("Not available"));
	}





	// качество теней
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,0.0f,0.0f, ContentTransp, vw_GetText("Shadow Quality"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, (Options_ShadowMap == 0) || !vw_GetDevCaps().OpenGL_2_0_supported || !vw_GetDevCaps().OpenGL_2_1_supported ||
			    !vw_GetDevCaps().OpenGL_3_0_supported || !Options_UseGLSL120 || (vw_GetDevCaps().FramebufferObjectDepthSize < 24))) {
		Options_ShadowMap--;
		if (Options_ShadowMap < 0) Options_ShadowMap = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, (Options_ShadowMap == 1) || !vw_GetDevCaps().OpenGL_2_0_supported || !vw_GetDevCaps().OpenGL_2_1_supported ||
			    !vw_GetDevCaps().OpenGL_3_0_supported || !Options_UseGLSL120 || (vw_GetDevCaps().FramebufferObjectDepthSize < 24))) {
		Options_ShadowMap++;
		if (Options_ShadowMap > 1) Options_ShadowMap = 1;
	}
	if (vw_GetDevCaps().OpenGL_2_0_supported && vw_GetDevCaps().OpenGL_2_1_supported && vw_GetDevCaps().OpenGL_3_0_supported && Options_UseGLSL120 && (vw_GetDevCaps().FramebufferObjectDepthSize >= 24)) {
		Size = vw_FontSize(vw_GetText(ShadowButtonQuality[Options_ShadowMap]));
		float WScale = 0;
		if (Size > 170) {
			Size = 170;
			WScale = -170;
		}
		SizeI = (170-Size)/2;//High, Medium, Low
		vw_DrawFont(X1+438+SizeI, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ShadowButtonQuality[Options_ShadowMap]));
	} else {
		Size = vw_FontSize(vw_GetText("Not available"));
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("Not available"));
	}









	int Prir = 100;
	int X;
	int Y = 165+Prir*4;

	X = Setup.InternalWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, vw_GetText("Interface"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::INTERFACE);
			GameMenuStatus = eGameMenuStatus::INTERFACE;
		} else
			ComBuffer = eCommand::SWITCH_TO_INTERFACE;
	}

	X = Setup.InternalWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, vw_GetText("Video & Audio"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::OPTIONS);
			GameMenuStatus = eGameMenuStatus::OPTIONS;
		} else
			ComBuffer = eCommand::SWITCH_TO_OPTIONS;
	}

	X = Setup.InternalWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("Config Controls"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::CONFCONTROL);
			GameMenuStatus = eGameMenuStatus::CONFCONTROL;
		} else
			ComBuffer = eCommand::SWITCH_TO_CONFCONTROL;
	}








	if (Options_TexturesAnisotropyLevel == Setup.AnisotropyLevel &&
	    Options_TexturesCompressionType == Setup.TexturesCompressionType &&
	    Options_MSAA == Setup.MSAA &&
	    Options_CSAA == Setup.CSAA &&
	    Options_UseGLSL120 == Setup.UseGLSL120 &&
	    Options_ShadowMap == Setup.ShadowMap) {
		X = (Setup.InternalWidth - 384)/2;
		Y = Y+Prir;
		if (MenuStatus == eMenuStatus::GAME) {
			if (DrawButton384(X,Y, vw_GetText("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				GameMenuStatus = eGameMenuStatus::GAME_MENU;
		} else {
			if (DrawButton384(X,Y, vw_GetText("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
		}
	} else {
		X = Setup.InternalWidth/2 - 256 - 38;
		Y = Y+Prir;
		if (MenuStatus == eMenuStatus::GAME) {
			if (DrawButton256(X,Y, vw_GetText("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				GameMenuStatus = eGameMenuStatus::GAME_MENU;
		} else {
			if (DrawButton256(X,Y, vw_GetText("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
				ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
		}
		X = Setup.InternalWidth/2 + 38;
		if (DrawButton256(X,Y, vw_GetText("APPLY"), ContentTransp, ButtonTransp2, LastButtonUpdateTime2)) {
			// проверяем, нужно перегружать или нет
			if (Options_TexturesAnisotropyLevel != Setup.AnisotropyLevel ||
			    Options_TexturesCompressionType != Setup.TexturesCompressionType ||
			    Options_MSAA != Setup.MSAA ||
			    Options_CSAA != Setup.CSAA ||
			    Options_UseGLSL120 != Setup.UseGLSL120 ||
			    Options_ShadowMap != Setup.ShadowMap) {
				if (MenuStatus == eMenuStatus::GAME)
					SetCurrentDialogBox(eDialogBox::RestartOnAdvOptChanged);
				else {
					CanQuit = false;
					Quit = true;
					NeedReCreate = true;
					SaveOptionsAdvMenuTmpData();
				}
			}
		}
	}
}


void SaveOptionsAdvMenuTmpData()
{
	Setup.UseGLSL120 = Options_UseGLSL120;
	Setup.AnisotropyLevel = Options_TexturesAnisotropyLevel;
	Setup.TexturesCompressionType = Options_TexturesCompressionType;
	Setup.MSAA = Options_MSAA;
	Setup.CSAA = Options_CSAA;
	Setup.ShadowMap = Options_ShadowMap;
}






