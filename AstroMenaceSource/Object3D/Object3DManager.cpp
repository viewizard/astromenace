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
void DrawAllObject3D(int ShadowMapInitType)
{
	// ставим всегда меньше или равно!
	vw_DepthTest(true, RI_LESSEQUAL);

	// т.к. мы в игре не на все объекты отбрасываем тени, смотрим, есть ли вообще эти объекты, надо ли вообще работать с шадовмепом
	int ObjectsWithShadow = 1;
	if (Setup.ShadowMap > 0)
		if (ShadowMapInitType == 2)
			ObjectsWithShadow = DrawAllSpaceObjectCount(13) + DrawAllSpaceObjectCount(15);


	unsigned int ShadowMapStage = 0;

	if ((Setup.ShadowMap > 0) & (ObjectsWithShadow > 0))
	{
		switch (ShadowMapInitType)
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
		// от больших объектов в т.ч. частей баз, только в меню отбрасываем тень, в игре проблемы с точностью z буфера
		if (ShadowMapInitType == 1) DrawAllSpaceObject(true, 0);
		else DrawAllSpaceObject(true, 0, 8); // делаем тень только от частей взорванных объектов

		ShadowMap_EndRenderToFBO();

		// работаем с 3-м стейджем текстур (первые два у нас заняты)
		ShadowMapStage = 2;
		ShadowMap_StartFinalRender(ShadowMapStage);
	}


	// космические объекты
	DrawAllSpaceObject(false, ShadowMapStage, 13); // части базы
	DrawAllSpaceObject(false, ShadowMapStage, 15); // большие астероиды


	// если в игре - то не рисуем тени на мелкие объекты, чтобы избежать z-файтинга
	if (ShadowMapInitType == 2) ShadowMapStage = 0;


	// космические объекты
	DrawAllSpaceObject(false, ShadowMapStage, 7); // мелкие астероиды
	DrawAllSpaceObject(false, ShadowMapStage, 8); // части разруженных моделей
	// корабли
	DrawAllSpaceShip(false, ShadowMapStage);
	// оружие
	DrawAllWeapon(false, ShadowMapStage);
	// наземные объекты
	DrawAllGroundObject(false, ShadowMapStage);
	// снаряды
	DrawAllProjectile(false, ShadowMapStage);


	if ((Setup.ShadowMap > 0) & (ObjectsWithShadow > 0))
	{
		ShadowMap_EndFinalRender(ShadowMapStage);
	}

	// взрывы
	DrawAllExplosion(false);
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







