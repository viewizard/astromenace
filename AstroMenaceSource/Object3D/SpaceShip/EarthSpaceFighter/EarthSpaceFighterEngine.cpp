/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: EarthSpaceFighterEngine.cpp

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
// получение мощности одного двигателя
//-----------------------------------------------------------------------------
float GetEnginePower(int EngineType)
{
	switch(EngineType)
	{
		case 1:	return 14.0f;
		case 2:	return 17.0f;
		case 3:	return 19.0f;
		case 4:	return 22.0f;
	}

	return 0.0f;
}
//-----------------------------------------------------------------------------
// получение ускорения одного двигателя
//-----------------------------------------------------------------------------
float GetEngineAcceleration(int EngineType)
{
	switch(EngineType)
	{
		case 1:	return 15.0f;
		case 2:	return 19.0f;
		case 3:	return 24.0f;
		case 4:	return 28.0f;
	}

	return 0.0f;
}
//-----------------------------------------------------------------------------
// получение мощности маневровых двигателя
//-----------------------------------------------------------------------------
float GetEngineRotatePower(int EngineType)
{
	switch(EngineType)
	{
		case 1:	return 15.0f;
		case 2:	return 18.0f;
		case 3:	return 20.0f;
		case 4:	return 23.0f;
	}

	return 0.0f;
}






//-----------------------------------------------------------------------------
// Создание двигателя
//-----------------------------------------------------------------------------
void CreateSpaceShipEngine(eParticleSystem *ParticleSystem, int EngineType)
{
	switch(EngineType)
	{
		case 1:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 7.00f;
			ParticleSystem->SpeedOnCreation	   = 7.00f;
			ParticleSystem->SpeedVar   = 2.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.07f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 2:
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.70f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 7.00f;
			ParticleSystem->SpeedOnCreation	   = 7.00f;
			ParticleSystem->SpeedVar   = 2.00f;
			ParticleSystem->Theta      = 12.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.4f, 0.75f, 0.15f, 0.0f, 0.07f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 3:
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 0.30f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.20f;
			ParticleSystem->ColorEnd.g = 0.20f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 7.00f;
			ParticleSystem->SpeedOnCreation	   = 7.00f;
			ParticleSystem->SpeedVar   = 2.00f;
			ParticleSystem->Theta      = 3.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->ParticlesPerSec = 80;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.25f, 0.25f, 1.0f, 0.0f, 0.07f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 4:
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 12.00f;
			ParticleSystem->SpeedOnCreation	   = 12.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(0.6f,0.6f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 50.0f;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.07f);
			ParticleSystem->LightNeedDeviation = true;
			break;

	}
}



//-----------------------------------------------------------------------------
// Создание двигателя
//-----------------------------------------------------------------------------
void CreateRotateSpaceShipEngine(eParticleSystem *ParticleSystem, int EngineType)
{
	switch(EngineType)
	{
		case 1:
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 5.00f;
			ParticleSystem->SpeedVar   = 5.00f;
			ParticleSystem->Theta      = 40.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->IsSuppressed = true;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 2:
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.5f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 5.00f;
			ParticleSystem->SpeedVar   = 5.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->IsSuppressed = true;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.4f, 0.75f, 0.15f, 0.0f, 0.2f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 3:
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 0.30f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.20f;
			ParticleSystem->ColorEnd.g = 0.20f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.5f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 5.00f;
			ParticleSystem->SpeedVar   = 5.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->IsSuppressed = true;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.25f, 0.25f, 1.0f, 0.0f, 0.2f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 4:
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.60f;
			ParticleSystem->AlphaEnd   = 0.10f;
			ParticleSystem->SizeStart  = 0.5f;
			ParticleSystem->SizeVar    = 0.05f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 5.00f;
			ParticleSystem->SpeedVar   = 5.00f;
			ParticleSystem->Theta      = 30.00f;
			ParticleSystem->Life       = 0.50f;
			ParticleSystem->IsSuppressed = true;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.2f);
			ParticleSystem->LightNeedDeviation = true;
			break;

	}
}








//-----------------------------------------------------------------------------
// Установка системы двигателей
//-----------------------------------------------------------------------------
void SetEarthSpaceFighterEngine(CEarthSpaceFighter *SpaceShip, int EngineType)
{
	// если нужен сброс установки двигателя
	if (EngineType == 0)
	{
		for (int i=0; i<SpaceShip->EngineQuantity; i++)
		if (SpaceShip->Engine[i] != 0){delete SpaceShip->Engine[i]; SpaceShip->Engine[i] = 0;}

		for (int i=0; i<SpaceShip->EngineLeftQuantity; i++)
		if (SpaceShip->EngineLeft[i] != 0){delete SpaceShip->EngineLeft[i]; SpaceShip->EngineLeft[i] = 0;}

		for (int i=0; i<SpaceShip->EngineRightQuantity; i++)
		if (SpaceShip->EngineRight[i] != 0){delete SpaceShip->EngineRight[i]; SpaceShip->EngineRight[i] = 0;}

		SpaceShip->MaxSpeed = 0.0f;
		SpaceShip->MaxAcceler = 0.0f;
		SpaceShip->MaxSpeedRotate = 0.0f;
		return;
	}


	for (int i=0; i<SpaceShip->EngineQuantity; i++)
	{
		if (SpaceShip->Engine[i] != 0){delete SpaceShip->Engine[i]; SpaceShip->Engine[i] = 0;}
		SpaceShip->Engine[i] = new eParticleSystem;

		CreateSpaceShipEngine(SpaceShip->Engine[i], EngineType);
		SpaceShip->Engine[i]->SetStartLocation(SpaceShip->EngineLocation[i]);
		SpaceShip->Engine[i]->Direction = VECTOR3D(0.0f, 0.0f, -1.0f);
	}


	for (int i=0; i<SpaceShip->EngineLeftQuantity; i++)
	{
		if (SpaceShip->EngineLeft[i] != 0){delete SpaceShip->EngineLeft[i]; SpaceShip->EngineLeft[i] = 0;}
		SpaceShip->EngineLeft[i] = new eParticleSystem;

		CreateRotateSpaceShipEngine(SpaceShip->EngineLeft[i], EngineType);
		SpaceShip->EngineLeft[i]->SetStartLocation(SpaceShip->EngineLeftLocation[i]);
		SpaceShip->EngineLeft[i]->Direction = VECTOR3D(1.0f, 0.0f, 0.6f);
		SpaceShip->EngineLeft[i]->IsSuppressed = true;
	}


	for (int i=0; i<SpaceShip->EngineRightQuantity; i++)
	{
		if (SpaceShip->EngineRight[i] == 0){delete SpaceShip->EngineRight[i]; SpaceShip->EngineRight[i] = 0;}
		SpaceShip->EngineRight[i] = new eParticleSystem;

		CreateRotateSpaceShipEngine(SpaceShip->EngineRight[i], EngineType);
		SpaceShip->EngineRight[i]->SetStartLocation(SpaceShip->EngineRightLocation[i]);
		SpaceShip->EngineRight[i]->Direction = VECTOR3D(-1.0f, 0.0f, 0.6f);
		SpaceShip->EngineRight[i]->IsSuppressed = true;
	}


    // параметрами игрока управляем в другом месте!!!! пользуй поиск
	SpaceShip->MaxSpeed = GetEnginePower(EngineType)*SpaceShip->EngineQuantity - SpaceShip->Weight/1000.0f;
	SpaceShip->MaxAcceler = GetEngineAcceleration(EngineType)*SpaceShip->EngineQuantity - SpaceShip->Weight/1000.0f;
	SpaceShip->MaxSpeedRotate = GetEngineRotatePower(EngineType)*SpaceShip->EngineQuantity - SpaceShip->Weight/1000.0f;


	if (SpaceShip->MaxSpeed <= 0.5f) SpaceShip->MaxSpeed = 0.5f;
	if (SpaceShip->MaxAcceler <= 0.5f) SpaceShip->MaxAcceler = 0.5f;
	if (SpaceShip->MaxSpeedRotate <= 0.5f) SpaceShip->MaxSpeedRotate = 0.5f;

}




