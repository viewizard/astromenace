/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text00.VW2D");
			break;
		// примитивная броня
		case 1:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text04.VW2D");
			break;
		// модернизированная броня
		case 2:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text05.VW2D");
			break;
		// и против иона
		case 3:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text07.VW2D");
			break;
		// и против иона+
		case 4:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text08.VW2D");
			break;
		// и против иона++
		case 5:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text09.VW2D");
			break;
		// и против плазмы
		case 6:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text10.VW2D");
			break;
		// универсальная
		case 7:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-text06.VW2D");
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
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum01.VW2D");
			break;
		// примитивная броня
		case 1:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum02.VW2D");
			break;
		// модернизированная броня
		case 2:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D");
			break;
		// и против иона
		case 3:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D");
			break;
		// и против иона+
		case 4:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D");
			break;
		// и против иона++
		case 5:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum02.VW2D");
			break;
		// и против плазмы
		case 6:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum04.VW2D");
			break;
		// универсальная
		case 7:
			return vw_FindTextureByName("DATA/MODELS/EARTHFIGHTER/sf-illum03.VW2D");
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















