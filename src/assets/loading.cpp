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

#include "../core/core.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../main.h"
#include "audio.h"
#include "model3d.h"
#include "shader.h"
#include "texture.h"


//------------------------------------------------------------------------------------
// процедура прорисовки логотипа
//------------------------------------------------------------------------------------
static void DrawViewizardLogo(GLtexture ViewizardLogoTexture)
{
	int ShowLogoTime{6000}; // сколько нужно показывать логотип
	int ShowLogoLife{ShowLogoTime}; // сколько осталось показывать
	uint32_t ShowLogoPrevTime = SDL_GetTicks();

	vw_SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	while (ShowLogoLife > 0) {
		sRECT SrcRect{1, 1, 511, 511};
		int StartX{static_cast<int>((GameConfig().InternalWidth - 510) / 2)};
		int EndX{StartX + 510};
		sRECT DstRect{StartX, 128 + 1, EndX, 640 - 2};
		float Transp{1.0f};
		float GreyColor{1.0f};

		// плавно делаем появление
		if (ShowLogoLife > ShowLogoTime / 2.0f)
			Transp = (ShowLogoTime / 2.0f - (ShowLogoLife - ShowLogoTime / 2.0f)) / (ShowLogoTime / 2.0f);
		else {
			Transp = 1.0f;
			// относительно быстро исчезаем
			if (ShowLogoLife < ShowLogoTime / 4.0f) {
				GreyColor = ShowLogoLife / (ShowLogoTime / 4.0f);
				vw_SetClearColor(GreyColor, GreyColor, GreyColor, 1.0f);
			}
		}
		Transp -= 0.01f; // чтобы всегда был немного прозрачным
		if (Transp < 0.0f)
			Transp = 0.0f;

		// рисуем
		vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
		vw_Start2DMode(-1, 1);

		vw_Draw2D(DstRect, SrcRect, ViewizardLogoTexture, true, Transp, 0.0f,
			  sRGBCOLOR{GreyColor, GreyColor, GreyColor});

		vw_End2DMode();
		vw_EndRendering();

		// проверка времени
		ShowLogoLife -= SDL_GetTicks() - ShowLogoPrevTime;
		ShowLogoPrevTime = SDL_GetTicks();
		if (ShowLogoLife <= 0)
			ShowLogoLife = 0;

		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_KEYDOWN:
			case SDL_JOYBUTTONDOWN:
				ShowLogoLife = 0;
				break;
			default:
				break;
			}
		}

		SDL_Delay(2);

		// important, update music buffers
		AudioLoop();
	}

	vw_SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------------
// процедура прорисовки процента загрузки данных
//------------------------------------------------------------------------------------
static void DrawLoading(unsigned int Current, unsigned int AllDrawLoading, uint32_t &LastDrawTime,
			GLtexture LoadBackground, GLtexture StatusLine, GLtexture StatusBack)
{
	if ((Current != AllDrawLoading) && // the last one (with 100%) must be rendered
	    (LastDrawTime + 10 >= SDL_GetTicks())) // 100 per second - is good enough frame rate, no need more
		return;

	vw_BeginRendering(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);
	vw_Start2DMode(-1, 1);

	sRECT SrcRect{0, 0, 1024, 512};
	sRECT DstRect{0, 64+32, static_cast<int>(GameConfig().InternalWidth), 64+32+512};
	vw_Draw2D(DstRect, SrcRect, LoadBackground, false, 1.0f, 0.0f);

	vw_DrawText(GameConfig().InternalWidth / 2 - vw_TextWidth(vw_GetText("LOADING")) / 2,
		    768-128, 0, 0, 1.0f, eRGBCOLOR::white, 1.0f, vw_GetText("LOADING"));

	SrcRect(0, 0, 256, 32);
	int StartX = (GameConfig().InternalWidth - 256) / 2;
	DstRect(StartX, 768-64-8-32, StartX + SrcRect.right - SrcRect.left, 768-64-8-32 + SrcRect.bottom - SrcRect.top);
	vw_Draw2D(DstRect, SrcRect, StatusBack, true, 1.0f, 0.0f);

	int loaded = (int)(256.0f * Current / AllDrawLoading);
	SrcRect(0, 0, loaded, 16);
	DstRect(StartX, 768-64-1-32, StartX + SrcRect.right - SrcRect.left, 768-64-1-32 + SrcRect.bottom - SrcRect.top);
	vw_Draw2D(DstRect, SrcRect, StatusLine, true, 1.0f, 0.0f);

	vw_End2DMode();
	vw_EndRendering();

	// important, care about system events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			QuitFromMainLoop();
			break;
		default:
			break;
		}
	}

	LastDrawTime = SDL_GetTicks();

	// small delay for last call with 100%
	if (Current == AllDrawLoading)
		SDL_Delay(100);
}

/*
 * Load all game assets.
 */
void LoadAllGameAssets()
{
	// fill screen with white at start, since shaders load could take some time
	vw_SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BeginRendering(RI_COLOR_BUFFER);
	vw_EndRendering();

	// NOTE shaders should be load first (since OpenGL 3.1),
	// since in OpenGL 3.1+ Fixed Function Pipeline was removed
	if (GameConfig().UseGLSL120)
		ChangeGameConfig().UseGLSL120 = ForEachShaderAssetLoad(AudioLoop);

	// Viewizard logo
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1, eTextureWrapMode::CLAMP_TO_EDGE,
			  false, eAlphaCreateMode::EQUAL, false);
	GLtexture ViewizardLogo = vw_LoadTexture("loading/viewizardlogo.tga");
	DrawViewizardLogo(ViewizardLogo);
	vw_ReleaseTexture(ViewizardLogo);

	// background with status bar
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1, eTextureWrapMode::CLAMP_TO_EDGE,
			  true, eAlphaCreateMode::GREYSC, false);
	GLtexture StatusLine = vw_LoadTexture("loading/loading_line.tga");
	GLtexture StatusBack = vw_LoadTexture("loading/loading_back.tga");
	vw_SetTextureProp(eTextureBasicFilter::BILINEAR, 1, eTextureWrapMode::CLAMP_TO_EDGE,
			  false, eAlphaCreateMode::GREYSC, false);
	GLtexture LoadBackground = vw_LoadTexture("loading/loading0" + std::to_string(1 + vw_iRandNum(3)) + ".tga");

	uint32_t LastDrawTime = SDL_GetTicks();
	unsigned RealLoadedAssets{0};
	unsigned AllDrawLoading{GetAudioAssetsLoadValue() +
				GetModel3DAssetsLoadValue() +
				GetTextureAssetsLoadValue()};

	auto UpdateLoadStatus = [&] (unsigned AssetValue) {
		RealLoadedAssets += AssetValue;
		DrawLoading(RealLoadedAssets, AllDrawLoading, LastDrawTime,
			    LoadBackground, StatusLine, StatusBack);
		// important, update music buffers
		AudioLoop();
	};
	ForEachAudioAssetLoad(UpdateLoadStatus);
	ForEachModel3DAssetLoad(UpdateLoadStatus);
	ForEachTextureAssetLoad(UpdateLoadStatus);

	vw_ReleaseTexture(StatusLine);
	vw_ReleaseTexture(StatusBack);
	vw_ReleaseTexture(LoadBackground);
}
