/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: EarthSpaceFighterArmour.cpp

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
#include "EarthSpaceFighter.h"








//-----------------------------------------------------------------------------
// HullResistance для брони
//-----------------------------------------------------------------------------
float GetHullResistance(int ArmourType)
{
	switch(ArmourType)
	{
		case 0:	return 1.0f;
		case 1:	return 1.0f;
		case 2:	return 1.0f;
		case 3:	return 1.0f;
		case 4:	return 1.0f;
		case 5:	return 1.0f;
		case 6:	return 1.0f;
		case 7:	return 4.0f;
	}

	return 0.0f;
}
//-----------------------------------------------------------------------------
// SystemsResistance для брони
//-----------------------------------------------------------------------------
float GetSystemsResistance(int ArmourType)
{
	switch(ArmourType)
	{
		case 0:	return 1.0f;
		case 1:	return 1.0f;
		case 2:	return 1.0f;
		case 3:	return 1.0f;
		case 4:	return 1.0f;
		case 5:	return 1.0f;
		case 6:	return 1.0f;
		case 7:	return 5.0f;
	}

	return 0.0f;
}
//-----------------------------------------------------------------------------
// Тестура для брони
//-----------------------------------------------------------------------------
eTexture *GetArmourTexture(int ArmourType)
{
	switch(ArmourType)
	{
		// фактически - брони нет, только стальной корпус
		case 0:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text00.tga");
			break;
		// примитивная броня
		case 1:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text04.tga");
			break;
		// модернизированная броня
		case 2:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text05.tga");
			break;
		// и против иона
		case 3:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text07.tga");
			break;
		// и против иона+
		case 4:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text08.tga");
			break;
		// и против иона++
		case 5:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text09.tga");
			break;
		// и против плазмы
		case 6:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text10.tga");
			break;
		// универсальная
		case 7:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text06.tga");
			break;
	}

	return 0;
}
eTexture *GetArmourIllumTexture(int ArmourType)
{
	switch(ArmourType)
	{
		// фактически - брони нет, только стальной корпус
		case 0:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum01.tga");
			break;
		// примитивная броня
		case 1:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum02.tga");
			break;
		// модернизированная броня
		case 2:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.tga");
			break;
		// и против иона
		case 3:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.tga");
			break;
		// и против иона+
		case 4:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.tga");
			break;
		// и против иона++
		case 5:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum02.tga");
			break;
		// и против плазмы
		case 6:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum04.tga");
			break;
		// универсальная
		case 7:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.tga");
			break;
	}

	return 0;
}



//-----------------------------------------------------------------------------
// Установка брони для кораблей землян
//-----------------------------------------------------------------------------
void SetEarthSpaceFighterArmour(CEarthSpaceFighter *SpaceShip, int ArmourType)
{
	SpaceShip->ResistanceHull = GetHullResistance(ArmourType);
	SpaceShip->ResistanceSystems = GetSystemsResistance(ArmourType);

	for (int i=0; i<SpaceShip->DrawObjectQuantity; i++)
	{
		SpaceShip->Texture[i] = GetArmourTexture(ArmourType);
		SpaceShip->TextureIllum[i] = GetArmourIllumTexture(ArmourType);
	}

}















