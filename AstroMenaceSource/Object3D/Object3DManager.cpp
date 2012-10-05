/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Object3DManager.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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
extern GameSetup Setup;
extern eDevCaps CAPS;



//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceShip();
void ReleaseAllWeapon();
void ReleaseAllGroundObject();
void ReleaseAllProjectile();
void ReleaseAllSpaceObject();
void ReleaseAllExplosion();

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
void DrawAllSpaceShip();
void DrawAllWeapon();
void DrawAllGroundObject();
void DrawAllProjectile();
void DrawAllSpaceObject();
void DrawAllExplosion();

void DrawAllObject3D()
{
	// ставим всегда меньше или равно!
	vw_DepthTest(true, RI_LESSEQUAL);


	// корабли
	DrawAllSpaceShip();
	// оружие
	DrawAllWeapon();
	// наземные объекты
	DrawAllGroundObject();
	// снаряды
	DrawAllProjectile();
	// космические объекты
	DrawAllSpaceObject();
	// взрывы
	DrawAllExplosion();
}









//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceShip(float Time);
void UpdateAllWeapon(float Time);
void UpdateAllGroundObject(float Time);
void UpdateAllProjectile(float Time);
void UpdateAllSpaceObject(float Time);
void UpdateAllExplosion(float Time);

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







