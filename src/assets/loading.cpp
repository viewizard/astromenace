/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../core/core.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../main.h"
#include "audio.h"
#include "model3d.h"
#include "shader.h"
#include "texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Draw Viewizard logotype.
 */
static void DrawViewizardLogo(GLtexture ViewizardLogoTexture)
{
	constexpr uint32_t FadeTicks{2000}; // 2 sec.
	sRECT SrcRect{0, 0, 512, 512};
	sRECT DstRect{static_cast<int>((GameConfig().InternalWidth - 512) / 2),
		      static_cast<int>((GameConfig().InternalHeight - 512) / 2),
		      static_cast<int>((GameConfig().InternalWidth - 512) / 2) + 512,
		      static_cast<int>((GameConfig().InternalHeight - 512) / 2) + 512};
	float Transp{0.0f};
	float GreyColor{1.0f};
	uint32_t CurrentTick{0};

	auto CheckEvent = [] () {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_KEYDOWN:
			case SDL_JOYBUTTONDOWN:
				return true;
			default:
				break;
			}
		}
		return false;
	};

	auto DrawLogo = [&] () {
		vw_BeginRendering(RI_COLOR_BUFFER);
		vw_Start2DMode(-1, 1);

		vw_Draw2D(DstRect, SrcRect, ViewizardLogoTexture, true, Transp, 0.0f,
			  sRGBCOLOR{GreyColor, GreyColor, GreyColor});

		vw_End2DMode();
		vw_EndRendering();
	};

	auto Loop = [&] (uint32_t LogoFade, std::function<void ()> Calculations) {
		while (LogoFade > CurrentTick) {
			Calculations();
			DrawLogo();

			if (CheckEvent())
				return true;

			SDL_Delay(10); // we don't need high FPS here, ~100 FPS should be enough
			AudioLoop(); // important, update music buffers
			CurrentTick = SDL_GetTicks();
		}
		return false;
	};

	vw_SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// fade-in
	CurrentTick = SDL_GetTicks();
	uint32_t LogoFadeIn = CurrentTick + FadeTicks;
	if (!Loop(LogoFadeIn, [&] () {
		  Transp = 1.0f - static_cast<float>(LogoFadeIn - CurrentTick) / static_cast<float>(FadeTicks);})) {
		// fade-out
		Transp = 1.0f;
		CurrentTick = SDL_GetTicks();
		uint32_t LogoFadeOut = CurrentTick + FadeTicks;
		Loop(LogoFadeOut, [&] () {
			GreyColor = static_cast<float>(LogoFadeOut - CurrentTick) / static_cast<float>(FadeTicks);
			vw_SetClearColor(GreyColor, GreyColor, GreyColor, 1.0f);
		});
	}

	vw_SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/*
 * Draw assets load progress bar.
 */
static void DrawLoadProgress(unsigned int Current, unsigned int AllDrawLoading, uint32_t &LastDrawTick,
			     GLtexture Background, GLtexture ProgressBar, GLtexture ProgressBarBorder)
{
	if ((Current != AllDrawLoading) && // the last one (with 100%) must be rendered for sure
	    (LastDrawTick + 10 >= SDL_GetTicks())) // we don't need high FPS here, ~100 FPS should be enough
		return;

	vw_BeginRendering(RI_COLOR_BUFFER);
	vw_Start2DMode(-1, 1);

	int CenterdPositionY{96}; // "centered" position for background and progress bar, that located at bottom

	// background
	sRECT SrcRect{0, 0, 1024, 512};
	sRECT DstRect{0,
		      CenterdPositionY,
		      static_cast<int>(GameConfig().InternalWidth),
		      CenterdPositionY + 512};
	vw_Draw2D(DstRect, SrcRect, Background, false, 1.0f, 0.0f);

	// "LOADING" text
	vw_DrawTextUTF32(GameConfig().InternalWidth / 2 - vw_TextWidthUTF32(vw_GetTextUTF32("LOADING")) / 2,
			 GameConfig().InternalHeight - 16/*progress bar width*/ - 16/*font size*/ - CenterdPositionY,
			 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 1.0f, vw_GetTextUTF32("LOADING"));

	// progress bar border
	SrcRect(0, 0, 256, 16);
	DstRect((GameConfig().InternalWidth - 256) / 2,
		GameConfig().InternalHeight - CenterdPositionY,
		(GameConfig().InternalWidth - 256) / 2 + 256,
		GameConfig().InternalHeight - CenterdPositionY + 16);
	vw_Draw2D(DstRect, SrcRect, ProgressBarBorder, true, 1.0f, 0.0f);

	// progress bar
	SrcRect(0, 0, static_cast<int>(256.0f * Current / AllDrawLoading), 16);
	DstRect((GameConfig().InternalWidth - 256) / 2,
		GameConfig().InternalHeight - CenterdPositionY,
		(GameConfig().InternalWidth - 256) / 2 + SrcRect.right/*static_cast<int>(256.0f * Current / AllDrawLoading)*/,
		GameConfig().InternalHeight - CenterdPositionY + 16);
	vw_Draw2D(DstRect, SrcRect, ProgressBar, true, 1.0f, 0.0f);

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

	LastDrawTick = SDL_GetTicks();

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
	vw_SetTextureProp(sTextureFilter{eTextureBasicFilter::BILINEAR}, 1,
			  sTextureWrap{eTextureWrapMode::CLAMP_TO_EDGE},
			  false, eAlphaCreateMode::EQUAL, false);
	GLtexture ViewizardLogo = vw_LoadTexture("loading/viewizardlogo.tga");
	DrawViewizardLogo(ViewizardLogo);
	vw_ReleaseTexture(ViewizardLogo);

	// background with progress bar
	vw_SetTextureAlpha(0, 0, 0);
	vw_SetTextureProp(sTextureFilter{eTextureBasicFilter::BILINEAR}, 1,
			  sTextureWrap{eTextureWrapMode::CLAMP_TO_EDGE},
			  true, eAlphaCreateMode::GREYSC, false);
	GLtexture ProgressBar = vw_LoadTexture("loading/loading_line.tga");
	GLtexture ProgressBarBorder = vw_LoadTexture("loading/loading_back.tga");
	vw_SetTextureProp(sTextureFilter{eTextureBasicFilter::BILINEAR}, 1,
			  sTextureWrap{eTextureWrapMode::CLAMP_TO_EDGE},
			  false, eAlphaCreateMode::GREYSC, false);
	GLtexture Background = vw_LoadTexture("loading/loading0" + std::to_string(1 + vw_uRandNum(3)) + ".tga");

	uint32_t LastDrawTick = SDL_GetTicks();
	unsigned RealLoadedAssets{0};
	unsigned AllDrawLoading{GetAudioAssetsLoadValue() +
				GetModel3DAssetsLoadValue() +
				GetTextureAssetsLoadValue()};

	auto UpdateLoadStatus = [&] (unsigned AssetValue) {
		RealLoadedAssets += AssetValue;
		DrawLoadProgress(RealLoadedAssets, AllDrawLoading, LastDrawTick,
				 Background, ProgressBar, ProgressBarBorder);
		// important, update music buffers
		AudioLoop();
	};
	ForEachAudioAssetLoad(UpdateLoadStatus);
	ForEachModel3DAssetLoad(UpdateLoadStatus);
	ForEachTextureAssetLoad(UpdateLoadStatus);

	vw_ReleaseTexture(ProgressBar);
	vw_ReleaseTexture(ProgressBarBorder);
	vw_ReleaseTexture(Background);
}

} // astromenace namespace
} // viewizard namespace
