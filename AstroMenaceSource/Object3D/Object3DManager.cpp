/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Object3DManager.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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

	// т.к. мы в игре не на все объекты отбрасываем тени, смотрим, есть ли вообще эти объекты, надо ли вообще работать с шадовмепом
	int ObjectsWithShadow = 1;
	if (Setup.ShadowMap > 0)
		if (DrawType == 2)
			ObjectsWithShadow = DrawAllSpaceObjectCount(13) + DrawAllSpaceObjectCount(15);


	bool ShadowMap = false;

	if ((Setup.ShadowMap > 0) & (ObjectsWithShadow > 0))
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
		// от больших объектов в т.ч. частей баз, только в меню отбрасываем тень, в игре проблемы с точностью z буфера
		if (DrawType == 1) DrawAllSpaceObject(true, 0);
		else DrawAllSpaceObject(true, 0, 8); // делаем тень только от частей взорванных объектов

		ShadowMap_EndRenderToFBO();

		// работаем с 3-м стейджем текстур (первые два у нас заняты)
		ShadowMap = true;
		ShadowMap_StartFinalRender();
	}


	// космические объекты
	DrawAllSpaceObject(false, ShadowMap, 13); // части базы
	DrawAllSpaceObject(false, ShadowMap, 15); // большие астероиды


	// если в игре - то не рисуем тени на мелкие объекты, чтобы избежать z-файтинга
	if (DrawType == 2) ShadowMap = false;


	// космические объекты
	DrawAllSpaceObject(false, ShadowMap, 7); // мелкие астероиды
	DrawAllSpaceObject(false, ShadowMap, 8); // части разруженных моделей
	// корабли
	DrawAllSpaceShip(false, ShadowMap);
	// оружие
	DrawAllWeapon(false, ShadowMap);
	// наземные объекты
	DrawAllGroundObject(false, ShadowMap);
	// снаряды
	DrawAllProjectile(false, ShadowMap);


	if ((Setup.ShadowMap > 0) & (ObjectsWithShadow > 0))
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
	if( Setup.Gamma != 5 )
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

		float GammaF = 1.0f + (Setup.Gamma - 5)/5.0f;

		if( GammaF > 1.0f )
		{
			vw_SetTextureBlend(true, RI_BLEND_DESTCOLOR, RI_BLEND_ONE);
			vw_SetColor(GammaF-1.0f, GammaF-1.0f, GammaF-1.0f, 1.0f);
		}
		else
		{
			vw_SetTextureBlend(true, RI_BLEND_ZERO, RI_BLEND_SRCCOLOR);
			vw_SetColor(GammaF, GammaF, GammaF, 1.0f);
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







