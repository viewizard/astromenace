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

// TODO revise ApplyBrightness(), probably, should be moved to gfx, since this is 'post effect'

// TODO don't call GetPreloadedTextureAsset() all the time, use cached texture instead

#include "../game.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "space_ship/space_ship.h"
#include "ground_object/ground_object.h"
#include "space_object/space_object.h"
#include "projectile/projectile.h"
#include "object3d.h"
#include "explosion/explosion.h"
#include "../gfx/star_system.h"
#include "../gfx/shadow_map.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

float DrawBuffer[16]; // RI_2f_XY | RI_2f_TEX = (2 + 2) * 4 vertices = 16
unsigned int DrawBufferCurrentPosition{0};

} // unnamed namespace

/*
 * Add data to local draw buffer.
 */
static inline void AddToDrawBuffer(float CoordX, float CoordY, float TextureU, float TextureV)
{
	DrawBuffer[DrawBufferCurrentPosition++] = CoordX;
	DrawBuffer[DrawBufferCurrentPosition++] = CoordY;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureU;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureV;
}

/*
 * Brightness.
 */
static void ApplyBrightness()
{
	if (GameConfig().Brightness == 5)
		return;

	// 'reset' buffer
	DrawBufferCurrentPosition = 0;

	// TRIANGLE_STRIP (2 triangles)
	AddToDrawBuffer(0.0f, 0.0f, 1.0f, 0.0f);
	AddToDrawBuffer(0.0f, GameConfig().InternalHeight, 1.0f, 1.0f);
	AddToDrawBuffer(GameConfig().InternalWidth, 0.0f, 0.0f, 0.0f);
	AddToDrawBuffer(GameConfig().InternalWidth, GameConfig().InternalHeight, 0.0f, 1.0f);

	GLtexture TileTexture = GetPreloadedTextureAsset("menu/whitepoint.tga");
	vw_BindTexture(0, TileTexture);

	float BrightnessF = 1.0f + (GameConfig().Brightness - 5) / 5.0f;

	if (BrightnessF > 1.0f) {
		vw_SetTextureBlend(true, eTextureBlendFactor::DST_COLOR, eTextureBlendFactor::ONE);
		vw_SetColor(BrightnessF - 1.0f, BrightnessF - 1.0f, BrightnessF - 1.0f, 1.0f);
	} else {
		vw_SetTextureBlend(true, eTextureBlendFactor::ZERO, eTextureBlendFactor::SRC_COLOR);
		vw_SetColor(BrightnessF, BrightnessF, BrightnessF, 1.0f);
	}

	vw_Start2DMode(-1,1);
	vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, DrawBuffer, 4 * sizeof(DrawBuffer[0]));
	vw_End2DMode();

	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);
}

/*
 * Draw all oblect3d.
 */
void DrawAllObject3D(eDrawType DrawType)
{
	vw_DepthTest(true, eCompareFunc::LEQUAL);

	bool ShadowMap{false};

	if (GameConfig().ShadowMap > 0) {
		switch (DrawType) {
		case eDrawType::MENU:
			ShadowMap_StartRenderToFBO(sVECTOR3D{50.0f, -5.0f, -120.0f}, 120.0f, 500.0f);
			break;
		case eDrawType::GAME:
			ShadowMap_StartRenderToFBO(sVECTOR3D{0.0f, 0.0f, 160.0f}, 600.0f, 800.0f);
			break;
		}

		DrawAllSpaceShips(true, 0);
		DrawAllWeapons(true, 0);
		DrawAllGroundObjects(true, 0);
		DrawAllProjectiles(true, 0);
		DrawAllExplosions(true);
		DrawAllSpaceObjects(true, 0);

		ShadowMap_EndRenderToFBO();

		ShadowMap = true;
		ShadowMap_StartFinalRender();
	}

	DrawAllSpaceObjects(false, ShadowMap);
	DrawAllSpaceShips(false, ShadowMap);
	DrawAllWeapons(false, ShadowMap);
	DrawAllGroundObjects(false, ShadowMap);
	DrawAllProjectiles(false, ShadowMap);

	if (GameConfig().ShadowMap > 0)
		ShadowMap_EndFinalRender();

	DrawAllExplosions(false);

	vw_DrawAllParticleSystems();

	StarSystemDrawThirdLayer(DrawType);

	ApplyBrightness();
}

/*
 * Update all oblect3d.
 */
void UpdateAllObject3D(float Time)
{
	UpdateAllSpaceShip(Time);
	UpdateAllGroundObjects(Time);
	// make sure this called after SpaceShip and GroundObject, since we need
	// release all weapon with lazy release before collision detection
	UpdateAllWeapon(Time);
	UpdateAllProjectile(Time);
	UpdateAllSpaceObject(Time);
	UpdateAllExplosion(Time);
}

/*
 * Release all oblect3d.
 */
void ReleaseAllObject3D()
{
	ReleaseAllSpaceShips();
	ReleaseAllGroundObjects();
	ReleaseAllWeapons();
	ReleaseAllProjectiles();
	ReleaseAllSpaceObjects();
	ReleaseAllExplosions();
}

} // astromenace namespace
} // viewizard namespace
