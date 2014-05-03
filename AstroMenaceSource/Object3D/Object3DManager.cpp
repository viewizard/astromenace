/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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


/// подключаем нужные файлы
#include "Object3D.h"
#include "../Game.h"



//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllObject3D()
{
	// корабли
	ReleaseAllSpaceShip();
	// оружие
	ReleaseAllWeapon();
	// наземные объекты
	ReleaseAllGroundObject();
	// снаряды
	ReleaseAllProjectile();
	// космические объекты
	ReleaseAllSpaceObject();
	// взрывы
	ReleaseAllExplosion();
}






//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllObject3D(int DrawType)
{
	// ставим всегда меньше или равно!
	vw_DepthTest(true, RI_LESSEQUAL);


	bool ShadowMap = false;

	if (Setup.ShadowMap > 0)
	{
		switch (DrawType)
		{
			// меню
			case 1: ShadowMap_StartRenderToFBO(VECTOR3D(50,-5,-120), 120.0f, 500.0f); break;
			// игра
			case 2: ShadowMap_StartRenderToFBO(VECTOR3D(0,0,160), 600.0f, 800.0f); break;
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


	// космические объекты
	DrawAllSpaceObject(false, ShadowMap);
	// корабли
	DrawAllSpaceShip(false, ShadowMap);
	// оружие
	DrawAllWeapon(false, ShadowMap);
	// наземные объекты
	DrawAllGroundObject(false, ShadowMap);
	// снаряды
	DrawAllProjectile(false, ShadowMap);


	if (Setup.ShadowMap > 0)
	{
		ShadowMap_EndFinalRender();
	}

	// взрывы
	DrawAllExplosion(false);


	// эффекты - самые последние в прорисовке!
	vw_DrawAllParticleSystems();







	// второй слой тайловой анимации "пыли"
	StarSystemDrawSecondLayer(DrawType);

	// эмуляция гаммы, фактически это простой пост эффект, всегда самый последний в прорисовке
	if( Setup.Brightness != 5 )
	{

		float *buff = 0;
		// RI_2f_XY | RI_1_TEX
		buff = new float[4*4]; if (buff == 0) return;

		int k=0;

		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;

		buff[k++] = 0.0f;
		buff[k++] = Setup.fAspectRatioHeight;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;

		buff[k++] = Setup.fAspectRatioWidth;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;

		buff[k++] = Setup.fAspectRatioWidth;
		buff[k++] = Setup.fAspectRatioHeight;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;


		eTexture *TileTexture = vw_FindTextureByName("DATA/MENU/whitepoint.tga");
		vw_SetTexture(0, TileTexture);

		float BrightnessF = 1.0f + (Setup.Brightness - 5)/5.0f;

		if( BrightnessF > 1.0f )
		{
			vw_SetTextureBlend(true, RI_BLEND_DESTCOLOR, RI_BLEND_ONE);
			vw_SetColor(BrightnessF-1.0f, BrightnessF-1.0f, BrightnessF-1.0f, 1.0f);
		}
		else
		{
			vw_SetTextureBlend(true, RI_BLEND_ZERO, RI_BLEND_SRCCOLOR);
			vw_SetColor(BrightnessF, BrightnessF, BrightnessF, 1.0f);
		}

		vw_Start2DMode(-1,1);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, buff, 4*sizeof(float));
		vw_End2DMode();

		vw_SetTextureBlend(false, 0, 0);
		vw_BindTexture(0, 0);
		if (buff != 0){delete [] buff; buff = 0;}
	}
}









//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllObject3D(float Time)
{
	// проверяем корабли
	UpdateAllSpaceShip(Time);
	// проверяем все оружие
	UpdateAllWeapon(Time);
	// наземные объекты
	UpdateAllGroundObject(Time);
	// снаряды
	UpdateAllProjectile(Time);
	// космические объекты
	UpdateAllSpaceObject(Time);
	// взрывы
	UpdateAllExplosion(Time);
}







