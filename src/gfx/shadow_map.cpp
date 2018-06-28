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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO ShadowMap_StartRenderToFBO() should be fixed in order to automatically
//      calculate focus point, probably not a good idea setup gluLookAt() to point,
//      that based on camera focus point and 'magic' FocusPointCorrection

#include "../core/core.h"
#include "../config/config.h"
#include "shadow_map.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

std::shared_ptr<sFBO> ShadowMapFBO{};
float ShadowMap_LightProjectionMatrix[16];
float ShadowMap_LightModelViewMatrix[16];

float xPixelOffset{0};
float yPixelOffset{0};

std::weak_ptr<sFBO> CurrentSystemFBO{};
int ShadowMapViewPort_x, ShadowMapViewPort_y, ShadowMapViewPort_width, ShadowMapViewPort_height;

} // unnamed namespace


/*
 * X offset for shader.
 */
float ShadowMap_Get_xPixelOffset()
{
	return xPixelOffset;
}

/*
 * Y offset for shader.
 */
float ShadowMap_Get_yPixelOffset()
{
	return yPixelOffset;
}

/*
 * Initialization.
 */
bool ShadowMap_Init(int Width, int Height)
{
	if (ShadowMapFBO)
		ShadowMap_Release();

	xPixelOffset = 1.0f / static_cast<float>(Width);
	yPixelOffset = 1.0f / static_cast<float>(Height);

	ShadowMapFBO = vw_BuildFBO(Width, Height, false, true);
	if (ShadowMapFBO && (ShadowMapFBO->DepthSize >= 24)) // we need at least 24 bits
		return true;

	ShadowMap_Release();
	std::cout << "\nYou need at least 24 bits Depth Size for Shadow Mapping support.\n\n";
	return false;
}

/*
 * Release.
 */
void ShadowMap_Release()
{
	if (!ShadowMapFBO)
		return;

	ShadowMapFBO.reset();
}

/*
 * Setup shadow map with appropriate map's size.
 */
void ShadowMap_SizeSetup(eShadowMapSetup ShadowMapSetup)
{
	ShadowMap_Release();

	if (GameConfig().ShadowMap <= 0)
		return;

	int ShadowMapSize = vw_GetDevCaps().MaxTextureWidth;

	switch (ShadowMapSetup) {
	case eShadowMapSetup::Menu:
		// since we need "soft" shadows for less price, reduce shadow map size
		if (ShadowMapSize > 2048)
			ShadowMapSize = 2048;
		if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize / 2))
			ChangeGameConfig().ShadowMap = 0;
		break;

	case eShadowMapSetup::Game:
		// since we need "soft" shadows for less price, reduce shadow map size
		if (ShadowMapSize > 4096)
			ShadowMapSize = 4096;
		if (!ShadowMap_Init(ShadowMapSize, ShadowMapSize))
			ChangeGameConfig().ShadowMap = 0;
		break;
	}
}

/*
 * Start shadow map rendering (depth map).
 */
void ShadowMap_StartRenderToFBO(sVECTOR3D FocusPointCorrection, float Distance, float fFarClip)
{
	if (!ShadowMapFBO ||
	    !ShadowMapFBO->DepthTexture)
		return;

	vw_PushMatrix(); // push MODELVIEW matrix

	vw_GetViewport(&ShadowMapViewPort_x, &ShadowMapViewPort_y, &ShadowMapViewPort_width, &ShadowMapViewPort_height);
	vw_SetViewport(0, 0, ShadowMapFBO->Width.i(), ShadowMapFBO->Height.i(), eOrigin::bottom_left);
	vw_DepthRange(0.005f, 1.0f);

	CurrentSystemFBO = vw_GetCurrentFBO();
	vw_BindFBO(ShadowMapFBO);

	vw_Clear(RI_DEPTH_BUFFER);
	vw_SetColorMask(false, false, false, false);

	vw_MatrixMode(eMatrixMode::PROJECTION);
	vw_PushMatrix();
	vw_MatrixMode(eMatrixMode::MODELVIEW);

	vw_ResizeScene(45.0f, ShadowMapFBO->Width.f() / ShadowMapFBO->Height.f(), 1.0f, fFarClip);
	vw_GetMatrix(eMatrixPname::PROJECTION, ShadowMap_LightProjectionMatrix);

	// get directional light
	std::weak_ptr<cLight> CurrentDirectLight;
	sVECTOR3D LightPosition{0.0f, 0.0f, 0.0f};
	if (vw_GetMainDirectLight(CurrentDirectLight))
		if (auto sharedCurrentDirectLight = CurrentDirectLight.lock())
			LightPosition = sharedCurrentDirectLight->Direction;
	LightPosition.Normalize();
	LightPosition = LightPosition ^ -Distance;

	sVECTOR3D CurrentCameraFocusPoint = vw_GetCameraFocusPoint();
	CurrentCameraFocusPoint += FocusPointCorrection;
	// for directional light, we should move eyes point
	LightPosition += CurrentCameraFocusPoint;

	gluLookAt(LightPosition.x, LightPosition.y, LightPosition.z,
		  CurrentCameraFocusPoint.x, CurrentCameraFocusPoint.y, CurrentCameraFocusPoint.z,
		  0.0f, 1.0f, 0.0f);

	vw_GetMatrix(eMatrixPname::MODELVIEW, ShadowMap_LightModelViewMatrix);
	vw_CullFace(eCullFace::FRONT);
	vw_PolygonOffset(true, 2.0f, 2.0f);
}

/*
 * End shadow map rendering (depth map).
 */
void ShadowMap_EndRenderToFBO()
{
	if (!ShadowMapFBO ||
	    !ShadowMapFBO->DepthTexture)
		return;

	vw_PolygonOffset(false, 0.0f, 0.0f);
	vw_CullFace(eCullFace::BACK);

	auto sharedCurrentSystemFBO = CurrentSystemFBO.lock();
	// don't use 'if' here on lock() call, nullptr object is appropriate too
	vw_BindFBO(sharedCurrentSystemFBO);

	vw_SetColorMask(true, true, true, true);
	vw_SetViewport(ShadowMapViewPort_x, ShadowMapViewPort_y,
		       ShadowMapViewPort_width, ShadowMapViewPort_height,
		       eOrigin::bottom_left);
	vw_DepthRange(0.0f, 1.0f);

	vw_MatrixMode(eMatrixMode::PROJECTION);
	vw_PopMatrix();
	vw_MatrixMode(eMatrixMode::MODELVIEW);
	vw_PopMatrix();
}

/*
 * Start scene rendering with shadows.
 */
void ShadowMap_StartFinalRender()
{
	if (!ShadowMapFBO ||
	    !ShadowMapFBO->DepthTexture)
		 return;

	vw_BindTexture(2, ShadowMapFBO->DepthTexture);
	vw_SetTextureCompare(eTextureCompareMode::REF_TO_TEXTURE, eCompareFunc::LEQUAL);
	vw_SetTextureDepthMode(eTextureDepthMode::INTENSITY);
	vw_SetTextureFiltering(sTextureFilter{eTextureBasicFilter::BILINEAR});

	vw_MatrixMode(eMatrixMode::TEXTURE);
	vw_LoadIdentity();
	vw_Translate(sVECTOR3D(0.5f, 0.5f, 0.5f)); // remap from [-1,1]^2 to [0,1]^2
	vw_Scale(0.5f, 0.5f, 0.5f);

	vw_MultMatrix(ShadowMap_LightProjectionMatrix);
	vw_MultMatrix(ShadowMap_LightModelViewMatrix);

	float CurrentInvModelView[16];
	vw_GetMatrix(eMatrixPname::MODELVIEW, CurrentInvModelView);
	vw_Matrix44InverseRotate(CurrentInvModelView);
	vw_MultMatrix(CurrentInvModelView);

	vw_MatrixMode(eMatrixMode::MODELVIEW);
}

/*
 * End scene rendering with shadows.
 */
void ShadowMap_EndFinalRender()
{
	if (!ShadowMapFBO ||
	    !ShadowMapFBO->DepthTexture)
		return;

	vw_BindTexture(2, 0);
}

} // astromenace namespace
} // viewizard namespace
