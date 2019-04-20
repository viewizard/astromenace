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

#ifndef GFX_SHADOWMAP_H
#define GFX_SHADOWMAP_H

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eShadowMapSetup {
	Menu,
	Game
};

// Initialization.
bool ShadowMap_Init(int Width, int Height);
// Release.
void ShadowMap_Release();
// Setup shadow map with appropriate map's size.
void ShadowMap_SizeSetup(eShadowMapSetup ShadowMapSetup);
// Start shadow map rendering (depth map).
void ShadowMap_StartRenderToFBO(sVECTOR3D FocusPointCorrection, float Distance, float fFarClip);
// End shadow map rendering (depth map).
void ShadowMap_EndRenderToFBO();
// Start scene rendering with shadows (color).
void ShadowMap_StartFinalRender();
// End scene rendering with shadows (color).
void ShadowMap_EndFinalRender();
// X offset for shader.
float ShadowMap_Get_xPixelOffset();
// Y offset for shader.
float ShadowMap_Get_yPixelOffset();

} // astromenace namespace
} // viewizard namespace

#endif // GFX_SHADOWMAP_H
