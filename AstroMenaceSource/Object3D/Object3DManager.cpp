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

bool NeedShaderSetup = true;
eGLSL 	*GLSLShaderType1[2][7];
eGLSL 	*GLSLShaderType2[2][7];
eGLSL 	*GLSLShaderType3[2][7];


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

	// сбрасываем флаг, чтобы оптом опять установить правильные указатели
	NeedShaderSetup = true;
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
	// перед первой прорисовкой обязательно найти и задать шейдеры
	if (NeedShaderSetup)
	{
		NeedShaderSetup = false;

		GLSLShaderType1[0][0] = vw_FindShaderByName("ObjectLight10");
		GLSLShaderType1[0][1] = vw_FindShaderByName("ObjectLight11");
		GLSLShaderType1[0][2] = vw_FindShaderByName("ObjectLight12");
		GLSLShaderType1[0][3] = vw_FindShaderByName("ObjectLight13");
		GLSLShaderType1[0][4] = vw_FindShaderByName("ObjectLight14");
		GLSLShaderType1[0][5] = vw_FindShaderByName("ObjectLight15");
		GLSLShaderType1[0][6] = vw_FindShaderByName("ObjectLight16");
		GLSLShaderType1[1][0] = vw_FindShaderByName("ObjectLight20");
		GLSLShaderType1[1][1] = vw_FindShaderByName("ObjectLight21");
		GLSLShaderType1[1][2] = vw_FindShaderByName("ObjectLight22");
		GLSLShaderType1[1][3] = vw_FindShaderByName("ObjectLight23");
		GLSLShaderType1[1][4] = vw_FindShaderByName("ObjectLight24");
		GLSLShaderType1[1][5] = vw_FindShaderByName("ObjectLight25");
		GLSLShaderType1[1][6] = vw_FindShaderByName("ObjectLight26");

		GLSLShaderType2[0][0] = vw_FindShaderByName("Explosion10");
		GLSLShaderType2[0][1] = vw_FindShaderByName("Explosion11");
		GLSLShaderType2[0][2] = vw_FindShaderByName("Explosion12");
		GLSLShaderType2[0][3] = vw_FindShaderByName("Explosion13");
		GLSLShaderType2[0][4] = vw_FindShaderByName("Explosion14");
		GLSLShaderType2[0][5] = vw_FindShaderByName("Explosion15");
		GLSLShaderType2[0][6] = vw_FindShaderByName("Explosion16");
		GLSLShaderType2[1][0] = vw_FindShaderByName("Explosion20");
		GLSLShaderType2[1][1] = vw_FindShaderByName("Explosion21");
		GLSLShaderType2[1][2] = vw_FindShaderByName("Explosion22");
		GLSLShaderType2[1][3] = vw_FindShaderByName("Explosion23");
		GLSLShaderType2[1][4] = vw_FindShaderByName("Explosion24");
		GLSLShaderType2[1][5] = vw_FindShaderByName("Explosion25");
		GLSLShaderType2[1][6] = vw_FindShaderByName("Explosion26");

		GLSLShaderType3[0][0] = vw_FindShaderByName("Track10");
		GLSLShaderType3[0][1] = vw_FindShaderByName("Track11");
		GLSLShaderType3[0][2] = vw_FindShaderByName("Track12");
		GLSLShaderType3[0][3] = vw_FindShaderByName("Track13");
		GLSLShaderType3[0][4] = vw_FindShaderByName("Track14");
		GLSLShaderType3[0][5] = vw_FindShaderByName("Track15");
		GLSLShaderType3[0][6] = vw_FindShaderByName("Track16");
		GLSLShaderType3[1][0] = vw_FindShaderByName("Track20");
		GLSLShaderType3[1][1] = vw_FindShaderByName("Track21");
		GLSLShaderType3[1][2] = vw_FindShaderByName("Track22");
		GLSLShaderType3[1][3] = vw_FindShaderByName("Track23");
		GLSLShaderType3[1][4] = vw_FindShaderByName("Track24");
		GLSLShaderType3[1][5] = vw_FindShaderByName("Track25");
		GLSLShaderType3[1][6] = vw_FindShaderByName("Track26");
	}


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







