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
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/


/// подключаем нужные файлы
#include "Explosion.h"





//-----------------------------------------------------------------------------
// Создание графического эффекта
//-----------------------------------------------------------------------------
void SetExplosionGFX(eParticleSystem *ParticleSystem, int GFXNum)
{

	ParticleSystem->Texture[0] = vw_FindTextureByName("DATA/GFX/flare1.tga");


	switch(GFXNum)
	{
		case 0:	// разлет снаряда
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.20f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.05f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 1.0f;
			ParticleSystem->LifeVar	 = vw_Randf0/10.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 10;
			ParticleSystem->NeedStop = true;
			break;
		case 1:	// малый взрыв, астероид
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.05f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 2.0f;
			ParticleSystem->LifeVar	 = vw_Randf0/10.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->NeedStop = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.005f);
			break;
		case 2:	// средний взрыв, пришельцы
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 0.10f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 5.0f;
			ParticleSystem->SpeedVar   = 1.5f*vw_Randf0;
			ParticleSystem->Life       = 3.0f;
			ParticleSystem->LifeVar	 = vw_Randf0/10.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->NeedStop = true;
			break;
		case 3:	// средний взрыв, пришельцы
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 0.10f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.80f;
			ParticleSystem->SizeVar    = 0.40f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 5.0f;
			ParticleSystem->SpeedVar   = 1.5f*vw_Randf0;
			ParticleSystem->Life       = 2.0f;
			ParticleSystem->LifeVar	 = vw_Randf0/10.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 20;
			ParticleSystem->CreationType = 1;
			ParticleSystem->CreationSize = VECTOR3D(2.8f,2.8f,2.8f);
			ParticleSystem->NeedStop = true;
			break;
		case 4:	// средний взрыв
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 0.10f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 2.3f;
			ParticleSystem->LifeVar	 = vw_Randf0;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->NeedStop = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.05f, 0.85f, 1.0f, 0.0f, 0.002f);
			break;
		case 5:	// средний взрыв, земляне
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.05f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 3.0f;
			ParticleSystem->LifeVar	 = vw_Randf0/10.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->NeedStop = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.003f);
			break;
		case 6:	// средний взрыв, земляне
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 2.3f;
			ParticleSystem->LifeVar	 = vw_Randf0;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->NeedStop = true;
			break;
		case 7: // nuke
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 3.00f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.10f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 300;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(2.0f,0.3f,2.0f);
			ParticleSystem->DeadZone = 1.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.15f, 0.35f, 1.0f, 0.0f, 0.0001f);
			break;
		case 8: // torpedo
			ParticleSystem->ColorStart.r = 0.70f;
			ParticleSystem->ColorStart.g = 1.00f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.30f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(3.0f,0.3f,3.0f);
			ParticleSystem->DeadZone = 0.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.35f, 1.0f, 0.15f, 0.0f, 0.0005f);
			break;
		case 9: // nuke
			ParticleSystem->ColorStart.r = 0.30f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 2.00f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.30f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 100;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(3.0f,0.3f,3.0f);
			ParticleSystem->DeadZone = 0.9f;
			ParticleSystem->AlphaShowHide = true;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			break;
		case 10: // missile, swamp
			ParticleSystem->ColorStart.r = 1.00f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 0.30f;
			ParticleSystem->ColorEnd.r = 1.00f;
			ParticleSystem->ColorEnd.g = 0.00f;
			ParticleSystem->ColorEnd.b = 0.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.10f;
			ParticleSystem->SizeEnd    = 0.30f;
			ParticleSystem->Speed      = 0.00f;
			ParticleSystem->SpeedOnCreation	   = -1.00f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->Life       = 1.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->CreationType = 2;
			ParticleSystem->CreationSize = VECTOR3D(3.0f,0.3f,3.0f);
			ParticleSystem->DeadZone = 0.9f;
			ParticleSystem->AlphaShowHide = false;
			ParticleSystem->IsAttractive = true;
			ParticleSystem->AttractiveValue = -2.5f;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.35f, 0.15f, 0.0f, 0.005f);
			break;


		case 11:	// взрыв осколка босса, как средний взрыв, земляне
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 0.10f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 0.30f;
			ParticleSystem->SizeVar    = 0.20f;
			ParticleSystem->SizeEnd    = 0.05f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 3.0f;
			ParticleSystem->LifeVar	 = vw_Randf0/10.0f;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->NeedStop = true;
			ParticleSystem->Light = vw_CreatPointLight(VECTOR3D(0.0f,0.0f,0.0f), 0.05f, 0.85f, 1.0f, 0.0f, 0.001f);
			break;
		case 12:	// взрыв осколка босса, как средний взрыв, земляне
			ParticleSystem->TimeLastUpdate = 1;// чтобы сделать один проход
			ParticleSystem->ColorStart.r = 0.10f;
			ParticleSystem->ColorStart.g = 0.70f;
			ParticleSystem->ColorStart.b = 1.00f;
			ParticleSystem->ColorEnd.r = 0.00f;
			ParticleSystem->ColorEnd.g = 1.00f;
			ParticleSystem->ColorEnd.b = 1.00f;
			ParticleSystem->AlphaStart = 1.00f;
			ParticleSystem->AlphaEnd   = 0.00f;
			ParticleSystem->SizeStart  = 1.00f;
			ParticleSystem->SizeVar    = 0.50f;
			ParticleSystem->SizeEnd    = 0.20f;
			ParticleSystem->Speed      = 2.0f;
			ParticleSystem->SpeedVar   = 2.0f*vw_Randf0;
			ParticleSystem->Life       = 2.3f;
			ParticleSystem->LifeVar	 = vw_Randf0;
			ParticleSystem->Theta      = 360.00f;
			ParticleSystem->ParticlesPerSec = 50;
			ParticleSystem->NeedStop = true;
			break;

		default:
			fprintf(stderr, "Error in SetExplosionGFX function call, wrong GFXNum.\n");
			break;
	}
}

