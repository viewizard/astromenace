/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
#include "AlienSpaceFighter.h"






//-----------------------------------------------------------------------------
// Создание двигателя
//-----------------------------------------------------------------------------
void SetAlienSpaceFighterEngine(eParticleSystem *ParticleSystem, int EngineType)
{

	ParticleSystem->Texture[0] = vw_FindTextureByName("DATA/GFX/flare1.tga");
	ParticleSystem->Direction = VECTOR3D(0.0f, 0.0f, -1.0f);

	switch(EngineType)
	{
		case 1:
			// большой двигатель
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
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
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.1f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 2:
			// малый двигатель
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
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
			ParticleSystem->CreationSize = VECTOR3D(0.4f,0.7f,0.1f);
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.3f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 3:
			// кабина
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.50f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 3.0f;
			ParticleSystem->SpeedOnCreation	   = -1.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.0f;
			ParticleSystem->ParticlesPerSec = 15;
			ParticleSystem->IsAttractive = true;
			break;
		case 4:
			// кабина - высокая, двойная
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 0.50f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.30f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 3.0f;
			ParticleSystem->SpeedOnCreation	   = -1.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.0f;
			ParticleSystem->ParticlesPerSec = 20;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(0.1f,0.9f,0.1f);
			break;
		case 5:
			// основа для очень большого двигателя
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.40f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.70f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.50f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 7.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.50f;
			ParticleSystem->ParticlesPerSec = 40;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.0f, 0.55f, 1.0f, 0.0f, 0.1f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 6:
			// малый двигатель 2
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
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
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.85f, 1.0f, 0.0f, 0.3f);
			ParticleSystem->LightNeedDeviation = true;
			break;
		case 7:
			// свечение частей
			ParticleSystem->ColorStart.r = 0.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.70f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.20f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 8.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 0.00f;
			ParticleSystem->Life       = 0.30f;
			ParticleSystem->ParticlesPerSec = 20;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(0.1f,0.2f,0.1f);
			ParticleSystem->IsAttractive = true;
			break;

		default:
			fprintf(stderr, "Error in SetAlienSpaceFighterEngine function call, wrong EngineType.\n");
			break;
	}

}

