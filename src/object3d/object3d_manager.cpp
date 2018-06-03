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

#include "../game.h"
#include "../config/config.h"
#include "space_ship/space_ship.h"
#include "ground_object/ground_object.h"
#include "space_object/space_object.h"
#include "object3d.h"
#include "../gfx/star_system.h"
#include "../gfx/shadow_map.h"


//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllObject3D()
{
	ReleaseAllSpaceShip();
	ReleaseAllWeapon();
	ReleaseAllGroundObject();
	ReleaseAllProjectile();
	ReleaseAllSpaceObject();
	ReleaseAllExplosion();
}

//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllObject3D(eDrawType DrawType)
{
	// ставим всегда меньше или равно!
	vw_DepthTest(true, eCompareFunc::LEQUAL);

	bool ShadowMap{false};

	if (GameConfig().ShadowMap > 0) {
		switch (DrawType) {
		// меню
		case eDrawType::MENU:
			ShadowMap_StartRenderToFBO(sVECTOR3D(50,-5,-120), 120.0f, 500.0f);
			break;
		// игра
		case eDrawType::GAME:
			ShadowMap_StartRenderToFBO(sVECTOR3D(0,0,160), 600.0f, 800.0f);
			break;
		}

		DrawAllSpaceShip(true, 0);
		DrawAllWeapon(true, 0);
		DrawAllGroundObject(true, 0);
		DrawAllProjectile(true, 0);
		DrawAllExplosion(true);
		DrawAllSpaceObject(true, 0);

		ShadowMap_EndRenderToFBO();

		// работаем с 3-м стейджем текстур (первые два у нас заняты)
		ShadowMap = true;
		ShadowMap_StartFinalRender();
	}

	DrawAllSpaceObject(false, ShadowMap);
	DrawAllSpaceShip(false, ShadowMap);
	DrawAllWeapon(false, ShadowMap);
	DrawAllGroundObject(false, ShadowMap);
	DrawAllProjectile(false, ShadowMap);

	if (GameConfig().ShadowMap > 0)
		ShadowMap_EndFinalRender();

	// взрывы
	DrawAllExplosion(false);

	// эффекты - самые последние в прорисовке!
	vw_DrawAllParticleSystems();

	// второй слой тайловой анимации "пыли"
	StarSystemDrawThirdLayer(DrawType);

	// эмуляция гаммы, фактически это простой пост эффект, всегда самый последний в прорисовке
	if(GameConfig().Brightness != 5) {
		float *buff = new float[4*4]; // RI_2f_XY | RI_1_TEX
		int k=0;

		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;

		buff[k++] = 0.0f;
		buff[k++] = GameConfig().InternalHeight;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;

		buff[k++] = GameConfig().InternalWidth;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;

		buff[k++] = GameConfig().InternalWidth;
		buff[k++] = GameConfig().InternalHeight;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;

		GLtexture TileTexture = vw_FindTextureByName("menu/whitepoint.tga");
		vw_BindTexture(0, TileTexture);

		float BrightnessF = 1.0f + (GameConfig().Brightness - 5) / 5.0f;

		if( BrightnessF > 1.0f) {
			vw_SetTextureBlend(true, eTextureBlendFactor::DST_COLOR, eTextureBlendFactor::ONE);
			vw_SetColor(BrightnessF-1.0f, BrightnessF-1.0f, BrightnessF-1.0f, 1.0f);
		} else {
			vw_SetTextureBlend(true, eTextureBlendFactor::ZERO, eTextureBlendFactor::SRC_COLOR);
			vw_SetColor(BrightnessF, BrightnessF, BrightnessF, 1.0f);
		}

		vw_Start2DMode(-1,1);
		vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, buff, 4 * sizeof(buff[0]));
		vw_End2DMode();

		vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
		vw_BindTexture(0, 0);
		if (buff)
			delete [] buff;
	}
}

//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllObject3D(float Time)
{
	UpdateAllSpaceShip(Time);
	UpdateAllWeapon(Time);
	UpdateAllGroundObject(Time);
	UpdateAllProjectile(Time);
	UpdateAllSpaceObject(Time);
	UpdateAllExplosion(Time);
}
