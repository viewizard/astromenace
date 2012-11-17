/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: AlienSpaceMotherShipEngine.cpp

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
#include "AlienSpaceMotherShip.h"






//-----------------------------------------------------------------------------
// Создание двигателя
//-----------------------------------------------------------------------------
void SetAlienSpaceMotherShipEngine(eParticleSystem *ParticleSystem, int EngineType)
{

	ParticleSystem->Texture = vw_FindTextureByName("DATA/GFX/flare1.tga");
	ParticleSystem->Direction = VECTOR3D(0.0f, 0.0f, -1.0f);

	switch(EngineType)
	{
		// красно-желтые
		case 1:
			// большой двигатель
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 1.00f;
			ParticleSystem->Speed      = 15.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(2.4f,2.4f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.65f, 0.15f, 0.0f, 0.05f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 2:
			// средний двигатель
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.80f;
			ParticleSystem->Speed      = 10.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(1.2f,1.2f,0.1f);
			ParticleSystem->IsAttractive = true;
			break;
		case 3:
			// внутренности
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 400;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(15.0f,15.0f,3.0f);
			ParticleSystem->DeadZone = 14.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			break;
		case 4:
			// внутренности2
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 80;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(15.0f,15.0f,3.0f);
			ParticleSystem->DeadZone = 14.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 15.0f;
			break;
		case 5:
			// внутренности3
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(9.0f,9.0f,2.0f);
			ParticleSystem->DeadZone = 8.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			break;
		case 6:
			// внутренности4
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.30f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(9.0f,9.0f,2.0f);
			ParticleSystem->DeadZone = 8.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 15.0f;
			break;




		// бело-синие


		case 9:
			// малый двигатель 2
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.20f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.00f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 6.00f;
			ParticleSystem->SpeedOnCreation	   = 6.00f;
			ParticleSystem->Theta      = 20.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(0.5f,0.5f,0.1f);
			ParticleSystem->IsAttractive = true;
			break;
		case 10:
			// двигатель
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.50f;
			ParticleSystem->Speed      = 10.00f;
			ParticleSystem->SpeedOnCreation	   = 10.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(0.8f,0.8f,0.8f);
			ParticleSystem->IsAttractive = true;
			break;
		case 11:
			// большой двигатель
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.50f;
			ParticleSystem->Speed      = 10.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(1.4f,1.4f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 13.0f;
			break;
		case 12:
			// большой двигатель
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.50f;
			ParticleSystem->Speed      = 15.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(2.4f,2.4f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.03f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 13:
			// внутренности2
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.50f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 0.70f;
			ParticleSystem->ParticlesPerSec = 200;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(14.0f,5.0f,10.0f);
			ParticleSystem->DeadZone = 4.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 15.0f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.01f);
			ParticleSystem->LightNeedDeviation = true;
			break;




		// фиолетовое


		case 15:
			// большой двигатель
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.80f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.60f;
			ParticleSystem->Speed      = 20.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.50f;
			ParticleSystem->ParticlesPerSec = 150;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(5.0f,3.0f,3.0f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 30.0f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.01f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 16:
			// средний двигатель
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.80f;
			ParticleSystem->Speed      = 10.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(1.2f,1.2f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.05f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 17:
			// внутренности
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 400;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(14.0f,1.0f,14.0f);
			ParticleSystem->DeadZone = 13.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			break;
		case 18:
			// внутренности2
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 80;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(14.0f,1.0f,14.0f);
			ParticleSystem->DeadZone = 13.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = 20.0f;
			break;
		case 19:
			// внутренности
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.90f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 10.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.01f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 20:
			// большой двигатель
			ParticleSystem->ColorStart.r = 0.50f;
			ParticleSystem->ColorStart.g = 0.50f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.50f;
			ParticleSystem->ColorEnd.g = 0.50f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.80f;
			ParticleSystem->SizeStart  = 0.40f;
			ParticleSystem->SizeVar    = 0.40f;
			ParticleSystem->SizeEnd    = 0.50f;
			ParticleSystem->Speed      = 11.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(2.0f,2.0f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.01f);
			ParticleSystem->LightNeedDeviation = true;
			break;
	}



}
