/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


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
#include "weapon.h"






//-----------------------------------------------------------------------------
// Установка нужных данных для вспышки возле ствола
//-----------------------------------------------------------------------------
void SetWeaponFire(cParticleSystem *ParticleSystem, int WeaponNum)
{
	switch (WeaponNum) {
	case 1:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
		break;
	case 2:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 2.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 40.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
		break;
	case 3:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.10f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.60f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 50.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.05f, 0.0f, 0.02f);
		break;
	case 4:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.7f, 0.15f, 0.0f, 0.02f);
		break;
	case 5:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 4.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 10.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 6:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.20f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 20.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 7:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.70f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 0.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.20f;
		ParticleSystem->SizeStart  = 0.50f;
		ParticleSystem->SizeEnd    = 0.20f;
		ParticleSystem->Speed      = 6.00f;
		ParticleSystem->SpeedVar   = 1.00f;
		ParticleSystem->Theta      = 10.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.5f, 0.35f, 0.0f, 0.02f);
		break;
	case 8:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.2f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.2f;
		ParticleSystem->Speed      = 4.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->CreationType       = 1;
		ParticleSystem->CreationSize       = sVECTOR3D(0.4f,0.4f,0.4f);
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 9:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.15f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.15f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->CreationType       = 1;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.3f,0.3f);
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;
	case 10:
		ParticleSystem->ColorStart.r = 0.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 1.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 1.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.30f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 0.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->CreationType       = 1;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.3f,0.3f);
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.02f);
		break;



	case 11:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.60f;
		ParticleSystem->SizeStart  = 0.1f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->IsAttractive = true;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType       = 1;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.2f,0.3f);
		break;
	case 12:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.60f;
		ParticleSystem->SizeStart  = 0.1f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.30f;
		ParticleSystem->IsAttractive = true;
		ParticleSystem->ParticlesPerSec = 20;
		ParticleSystem->CreationType       = 1;
		ParticleSystem->CreationSize       = sVECTOR3D(0.3f,0.2f,0.3f);
		break;
	case 13:
		ParticleSystem->ColorStart.r = 0.70f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 0.70f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.7f, 1.0f, 0.0f, 0.0f, 0.02f);
		break;
	case 14:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 1.00f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 1.00f;
		ParticleSystem->SizeStart  = 0.10f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.00f;
		ParticleSystem->Speed      = 1.00f;
		ParticleSystem->SpeedVar   = 0.00f;
		ParticleSystem->Theta      = 360.00f;
		ParticleSystem->Life       = 0.20f;
		ParticleSystem->ParticlesPerSec = 200;
		ParticleSystem->CreationType       = 1;
		ParticleSystem->CreationSize       = sVECTOR3D(0.2f,0.2f,0.8f);
		break;
	case 15:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.50f;
		ParticleSystem->ColorStart.b = 0.00f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.50f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->Light = vw_CreatPointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.5f, 0.0f, 0.0f, 0.02f);
		break;
	case 16:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;
	case 17:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;
	case 18:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;
	case 19:
		ParticleSystem->ColorStart.r = 1.00f;
		ParticleSystem->ColorStart.g = 0.70f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 1.00f;
		ParticleSystem->ColorEnd.g = 0.70f;
		ParticleSystem->ColorEnd.b = 0.00f;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd   = 0.10f;
		ParticleSystem->SizeStart  = 0.4f;
		ParticleSystem->SizeVar    = 0.05f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedVar   = 3.00f;
		ParticleSystem->Theta      = 30.00f;
		ParticleSystem->Life       = 0.40f;
		break;

	default:
		fprintf(stderr, "Error in SetWeaponFire function call, wrong WeaponNum.\n");
		break;
	}


	ParticleSystem->IsSuppressed = true;
	ParticleSystem->ParticlesPerSec = 50;
	ParticleSystem->Texture[0] = vw_FindTextureByName("gfx/flare1.tga");

}
