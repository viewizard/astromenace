/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO add flare weapons (probably, we need some alien-specific visual effects)

#include "alien_space_mothership.h"
#include "../../../config/config.h"
#include "../../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

struct sAlienSpaceMotherShipData {
	unsigned int WeaponQuantity;
	unsigned int BossWeaponQuantity;
	unsigned int EngineQuantity;
	float Strength;
	float ShieldStrength;
	const char *Name;
	const char *Texture;
	const char *TextureIllum;
};

const sAlienSpaceMotherShipData PresetAlienSpaceMotherShipData[] = {
	{14,	0,	10,	3000,	1500,	"models/alienmothership/alm-01.vw3d", "models/alienmothership/alm-text04.vw2d", "models/alienmothership/alm-illum04.vw2d"},
	{8,	8,	10,	4000,	3000,	"models/alienmothership/alm-02.vw3d", "models/alienmothership/alm-text04.vw2d", "models/alienmothership/alm-illum04.vw2d"},
	{8,	1,	8,	5000,	3300,	"models/alienmothership/alm-03.vw3d", "models/alienmothership/alm-text02.vw2d", "models/alienmothership/alm-illum02.vw2d"},
	{8,	8,	12,	6000,	3500,	"models/alienmothership/alm-04.vw3d", "models/alienmothership/alm-text02.vw2d", "models/alienmothership/alm-illum02.vw2d"},
	{8,	6,	19,	7000,	3800,	"models/alienmothership/alm-05.vw3d", "models/alienmothership/alm-text08.vw2d", "models/alienmothership/alm-illum08.vw2d"},
	{12,	6,	15,	8000,	4000,	"models/alienmothership/alm-06.vw3d", "models/alienmothership/alm-text08.vw2d", "models/alienmothership/alm-illum08.vw2d"},
	{5,	2,	6,	9000,	4300,	"models/alienmothership/alm-07.vw3d", "models/alienmothership/alm-text03.vw2d", "models/alienmothership/alm-illum03.vw2d"},
	{4,	6,	10,	10000,	4500,	"models/alienmothership/alm-08.vw3d", "models/alienmothership/alm-text03.vw2d", "models/alienmothership/alm-illum03.vw2d"}
};
#define PresetAlienSpaceMotherShipDataCount sizeof(PresetAlienSpaceMotherShipData)/sizeof(PresetAlienSpaceMotherShipData[0])



//-----------------------------------------------------------------------------
// Создание двигателя
//-----------------------------------------------------------------------------
static void SetAlienSpaceMotherShipEngine(std::shared_ptr<cParticleSystem> &ParticleSystem, int EngineType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
	ParticleSystem->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);

	switch(EngineType) {
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(2.4f,2.4f,0.1f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 1.0f, 0.65f, 0.15f, 0.0f, 0.05f);
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(1.2f,1.2f,0.1f);
		ParticleSystem->IsMagnet = true;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(15.0f,15.0f,3.0f);
		ParticleSystem->DeadZone = 14.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(15.0f,15.0f,3.0f);
		ParticleSystem->DeadZone = 14.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 15.0f;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(9.0f,9.0f,2.0f);
		ParticleSystem->DeadZone = 8.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(9.0f,9.0f,2.0f);
		ParticleSystem->DeadZone = 8.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 15.0f;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(0.5f,0.5f,0.1f);
		ParticleSystem->IsMagnet = true;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(0.8f,0.8f,0.8f);
		ParticleSystem->IsMagnet = true;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(1.4f,1.4f,0.1f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 13.0f;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(2.4f,2.4f,0.1f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.03f);
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(14.0f,5.0f,10.0f);
		ParticleSystem->DeadZone = 4.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 15.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.35f, 0.75f, 1.0f, 0.0f, 0.01f);
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(5.0f,3.0f,3.0f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 30.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.01f);
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(1.2f,1.2f,0.1f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.05f);
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(14.0f,1.0f,14.0f);
		ParticleSystem->DeadZone = 13.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = -2.5f;
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(14.0f,1.0f,14.0f);
		ParticleSystem->DeadZone = 13.9f;
		ParticleSystem->AlphaShowHide = true;
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 20.0f;
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
		ParticleSystem->IsMagnet = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.01f);
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
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(2.0f,2.0f,0.1f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f,0.0f,0.0f), 0.5f, 0.5f, 1.0f, 0.0f, 0.01f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	default:
		std::cerr << __func__ << "(): " << "wrong EngineType.\n";
		break;
	}
}

//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cAlienSpaceMotherShip::cAlienSpaceMotherShip(int SpaceShipNum)
{
	if ((SpaceShipNum <= 0) || ((unsigned int)SpaceShipNum > PresetAlienSpaceMotherShipDataCount)) {
		std::cerr << __func__ << "(): "
			  << "Could not init cAlienSpaceMotherShip object with Number " << SpaceShipNum << "\n";
		return;
	}

	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::AlienMotherShip;
	InternalType = SpaceShipNum;
	PromptDrawDist2 = 20000.0f;

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 20.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 80.0f;
	Strength = StrengthStart = PresetAlienSpaceMotherShipData[SpaceShipNum-1].Strength/GameEnemyArmorPenalty;
	ShieldStrength = ShieldStrengthStart = PresetAlienSpaceMotherShipData[SpaceShipNum-1].ShieldStrength/GameEnemyArmorPenalty;
	ShieldRecharge = ShieldStrengthStart/15.0f;

	WeaponSlots.resize(PresetAlienSpaceMotherShipData[SpaceShipNum-1].WeaponQuantity);
	BossWeaponSlots.resize(PresetAlienSpaceMotherShipData[SpaceShipNum-1].BossWeaponQuantity);


	LoadObjectData(PresetAlienSpaceMotherShipData[SpaceShipNum-1].Name, *this);

	// всегда только эти текстуры
	for (unsigned int i = 0; i < Chunks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetAlienSpaceMotherShipData[SpaceShipNum - 1].Texture);
		TextureIllum[i] = GetPreloadedTextureAsset(PresetAlienSpaceMotherShipData[SpaceShipNum - 1].TextureIllum);
		// если шейдеры выключены - вернет ноль (не загружаем текстуры нормал мепов если нет шейдеров)
		// в LoadObjectData указываем Setup.UseGLSL
		NormalMap[i] = GetPreloadedTextureAsset("models/normalmap/alien_mothership_nm.tga");
	}


	// начальные установки для двигателей
	Engines.resize(PresetAlienSpaceMotherShipData[SpaceShipNum - 1].EngineQuantity);
	EnginesLocation.resize(Engines.size());










	// перебираем и ставим нужные данные
	switch (SpaceShipNum) {
	case 1:
		// оружие
		WeaponSlots[0](CreateWeapon(102), sVECTOR3D(-1.6f, 1.0f, 19.0f));
		WeaponSlots[1](CreateWeapon(102), sVECTOR3D(1.6f, 1.0f, 19.0f));
		WeaponSlots[2](CreateWeapon(102), sVECTOR3D(-1.6f, -1.0f, 19.0f));
		WeaponSlots[3](CreateWeapon(102), sVECTOR3D(1.6f, -1.0f, 19.0f));

		WeaponSlots[4](CreateWeapon(103), sVECTOR3D(-1.6f, 1.5f, 18.0f));
		WeaponSlots[5](CreateWeapon(103), sVECTOR3D(1.6f, 1.5f, 18.0f));
		WeaponSlots[6](CreateWeapon(103), sVECTOR3D(-1.6f, -1.5f, 18.0f));
		WeaponSlots[7](CreateWeapon(103), sVECTOR3D(1.6f, -1.5f, 18.0f));

		WeaponSlots[8](CreateWeapon(104), sVECTOR3D(-1.6f, 1.0f, 19.0f));
		WeaponSlots[9](CreateWeapon(104), sVECTOR3D(1.6f, 1.0f, 19.0f));
		WeaponSlots[10](CreateWeapon(104), sVECTOR3D(-1.6f, -1.0f, 19.0f));
		WeaponSlots[11](CreateWeapon(104), sVECTOR3D(1.6f, -1.0f, 19.0f));

		WeaponSlots[12](CreateWeapon(104), sVECTOR3D(0.0f, -8.0f, 0.0f));
		WeaponSlots[13](CreateWeapon(104), sVECTOR3D(0.0f, -8.0f, 0.0f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(8.4f, 5.2f, -24.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-8.4f, 5.2f, -24.0f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(8.4f, -5.6f, -24.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-8.4f, -5.6f, -24.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(0.0f, 8.2f, -15.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(0.0f, -8.2f, -15.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(0.0f, 0.0f, -8.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 3);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(0.0f, 0.0f, -8.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 4);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 5);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 6);
		break;
	case 2:
		// оружие
		WeaponSlots[0](CreateWeapon(103), sVECTOR3D(-10.7f, 0.0f, -18.5f));
		WeaponSlots[0].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[1](CreateWeapon(103), sVECTOR3D(10.7f, 0.0f, -18.5f));
		WeaponSlots[1].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[2](CreateWeapon(103), sVECTOR3D(-10.7f, 0.0f, -18.5f));
		WeaponSlots[2].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[3](CreateWeapon(103), sVECTOR3D(10.7f, 0.0f, -18.5f));
		WeaponSlots[3].Weapon->NextFireTime /= 2.0f;

		WeaponSlots[4](CreateWeapon(106), sVECTOR3D(-10.7f, 0.0f, -18.5f));
		WeaponSlots[4].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[5](CreateWeapon(106), sVECTOR3D(10.7f, 0.0f, -18.5f));
		WeaponSlots[5].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[6](CreateWeapon(106), sVECTOR3D(-4.0f, 0.0f, 24.0f));
		WeaponSlots[6].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[7](CreateWeapon(106), sVECTOR3D(4.0f, 0.0f, 24.0f));
		WeaponSlots[7].Weapon->NextFireTime /= 2.0f;

		BossWeaponSlots[0](CreateWeapon(102), sVECTOR3D(-1.5f, 1.5f, 25.0f));
		BossWeaponSlots[1](CreateWeapon(102), sVECTOR3D(1.5f, 1.5f, 25.0f));
		BossWeaponSlots[2](CreateWeapon(102), sVECTOR3D(-1.5f, -1.5f, 25.0f));
		BossWeaponSlots[3](CreateWeapon(102), sVECTOR3D(1.5f, -1.5f, 25.0f));
		BossWeaponSlots[4](CreateWeapon(102), sVECTOR3D(-4.0f, 2.0f, 24.0f));
		BossWeaponSlots[5](CreateWeapon(102), sVECTOR3D(4.0f, 2.0f, 24.0f));
		BossWeaponSlots[6](CreateWeapon(102), sVECTOR3D(-4.0f, -2.0f, 24.0f));
		BossWeaponSlots[7](CreateWeapon(102), sVECTOR3D(4.0f, -2.0f, 24.0f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(5.6f, 7.0f, -27.6f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-5.6f, 7.0f, -27.6f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.6f, -7.0f, -27.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.6f, -7.0f, -27.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(10.7f, 0.0f, -21.5f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-10.7f, 0.0f, -21.5f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(0.0f, 0.0f, -14.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 3);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(0.0f, 0.0f, -14.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 4);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 5);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(0.0f, 0.0f, 15.0f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 6);
		break;
	case 3:
		// оружие
		WeaponSlots[0](CreateWeapon(104), sVECTOR3D(3.0f, -2.0f, 13.0f));
		WeaponSlots[1](CreateWeapon(104), sVECTOR3D(-3.0f, -2.0f, 13.0f));
		WeaponSlots[2](CreateWeapon(104), sVECTOR3D(2.0f, -1.0f, 13.0f));
		WeaponSlots[3](CreateWeapon(104), sVECTOR3D(-2.0f, -1.0f, 13.0f));
		WeaponSlots[4](CreateWeapon(104), sVECTOR3D(2.0f, -3.0f, 13.0f));
		WeaponSlots[5](CreateWeapon(104), sVECTOR3D(-2.0f, -3.0f, 13.0f));

		WeaponSlots[6](CreateWeapon(109), sVECTOR3D(7.0f, -2.0f, 13.0f));
		WeaponSlots[7](CreateWeapon(109), sVECTOR3D(-7.0f, -2.0f, 13.0f));

		BossWeaponSlots[0](CreateWeapon(110), sVECTOR3D(0.0f, -2.0f, 27.0f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(5.5f, 8.1f, -26.1f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-5.5f, 8.1f, -26.1f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.6f, -7.2f, -28.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.6f, -7.2f, -28.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(11.9f, -1.0f, -14.8f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-11.9f, -1.0f, -14.8f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(0.0f, -1.0f, -10.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 3);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(0.0f, -1.0f, -10.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 4);
		break;
	case 4:
		// оружие
		WeaponSlots[0](CreateWeapon(104), sVECTOR3D(1.0f, -1.0f, 23.0f));
		WeaponSlots[1](CreateWeapon(104), sVECTOR3D(-1.0f, -1.0f, 23.0f));
		WeaponSlots[2](CreateWeapon(104), sVECTOR3D(1.0f, 0.0f, 23.0f));
		WeaponSlots[3](CreateWeapon(104), sVECTOR3D(-1.0f, 0.0f, 23.0f));
		WeaponSlots[4](CreateWeapon(104), sVECTOR3D(1.0f, -2.0f, 23.0f));
		WeaponSlots[5](CreateWeapon(104), sVECTOR3D(-1.0f, -2.0f, 23.0f));

		WeaponSlots[6](CreateWeapon(109), sVECTOR3D(0.0f, -1.0f, 23.0f));
		WeaponSlots[7](CreateWeapon(109), sVECTOR3D(0.0f, -1.0f, 23.0f));

		BossWeaponSlots[0](CreateWeapon(108), sVECTOR3D(8.9f, -0.6f, 18.0f));
		BossWeaponSlots[0].Weapon->SetRotation(sVECTOR3D(0.0f, -15.0f, 0.0f));
		BossWeaponSlots[1](CreateWeapon(108), sVECTOR3D(-8.9f, -0.6f, 18.0f));
		BossWeaponSlots[1].Weapon->SetRotation(sVECTOR3D(0.0f, 15.0f, 0.0f));
		BossWeaponSlots[2](CreateWeapon(108), sVECTOR3D(10.0f, -5.6f, 18.0f));
		BossWeaponSlots[3](CreateWeapon(108), sVECTOR3D(-10.0f, -5.6f, 18.0f));
		BossWeaponSlots[4](CreateWeapon(108), sVECTOR3D(8.9f, -0.6f, 18.0f));
		BossWeaponSlots[4].Weapon->SetRotation(sVECTOR3D(0.0f, -5.0f, 0.0f));
		BossWeaponSlots[5](CreateWeapon(108), sVECTOR3D(-8.9f, -0.6f, 18.0f));
		BossWeaponSlots[5].Weapon->SetRotation(sVECTOR3D(0.0f, 5.0f, 0.0f));
		BossWeaponSlots[6](CreateWeapon(108), sVECTOR3D(10.0f, -5.6f, 18.0f));
		BossWeaponSlots[6].Weapon->SetRotation(sVECTOR3D(0.0f, -10.0f, 0.0f));
		BossWeaponSlots[7](CreateWeapon(108), sVECTOR3D(-10.0f, -5.6f, 18.0f));
		BossWeaponSlots[7].Weapon->SetRotation(sVECTOR3D(0.0f, 10.0f, 0.0f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(1.9f, 5.9f, -24.6f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-1.9f, 5.9f, -24.6f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(1.9f, -2.9f, -24.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-1.9f, -2.9f, -24.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(2.9f, 1.6f, -24.6f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-2.9f, 1.6f, -24.6f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(8.9f, -0.6f, -12.6f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-8.9f, -0.6f, -12.6f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(10.0f, -5.6f, -5.2f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(-10.0f, -5.6f, -5.2f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 1);
		Engines[10] = vw_CreateParticleSystem();
		EnginesLocation[10] = sVECTOR3D(0.0f, -6.0f, 10.0f);
		if (auto sharedEngine = Engines[10].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 5);
			sharedEngine->CreationSize = sVECTOR3D(6.0f,6.0f,2.0f);
			sharedEngine->DeadZone = 5.9f;
		}
		Engines[11] = vw_CreateParticleSystem();
		EnginesLocation[11] = sVECTOR3D(0.0f, -6.0f, 10.0f);
		if (auto sharedEngine = Engines[11].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 6);
			sharedEngine->CreationSize = sVECTOR3D(6.0f,6.0f,2.0f);
			sharedEngine->DeadZone = 5.9f;
		}
		break;
	case 5:
		// оружие
		WeaponSlots[0](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[1](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[1].Weapon->SetRotation(sVECTOR3D(0.0f, 3.0f, 0.0f));
		WeaponSlots[2](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[2].Weapon->SetRotation(sVECTOR3D(0.0f, -3.0f, 0.0f));
		WeaponSlots[3](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[3].Weapon->SetRotation(sVECTOR3D(0.0f, 6.0f, 0.0f));
		WeaponSlots[4](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[5](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[5].Weapon->SetRotation(sVECTOR3D(0.0f, -6.0f, 0.0f));
		WeaponSlots[6](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[6].Weapon->SetRotation(sVECTOR3D(0.0f, 9.0f, 0.0f));
		WeaponSlots[7](CreateWeapon(109), sVECTOR3D(0.0f, -2.4f, 20.0f));
		WeaponSlots[7].Weapon->SetRotation(sVECTOR3D(0.0f, -9.0f, 0.0f));

		BossWeaponSlots[0](CreateWeapon(106), sVECTOR3D(35.7f, -3.0f, -13.0f));
		BossWeaponSlots[1](CreateWeapon(106), sVECTOR3D(-35.7f, -3.0f, -13.0f));
		BossWeaponSlots[2](CreateWeapon(106), sVECTOR3D(0.0f, -2.4f, 20.0f));
		BossWeaponSlots[3](CreateWeapon(106), sVECTOR3D(0.0f, -2.4f, 20.0f));
		BossWeaponSlots[4](CreateWeapon(104), sVECTOR3D(8.85f, 5.65f, -10.2f));
		BossWeaponSlots[5](CreateWeapon(104), sVECTOR3D(-8.85f, 5.65f, -10.2f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(25.1f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-25.1f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(20.6f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-20.6f, 0.65f, -18.8f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 9);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(22.9f, 0.65f, -20.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-22.9f, 0.65f, -20.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(22.9f, -5.1f, -20.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-22.9f, -5.1f, -20.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(8.85f, 5.65f, -15.2f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(-8.85f, 5.65f, -15.2f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[10] = vw_CreateParticleSystem();
		EnginesLocation[10] = sVECTOR3D(4.0f, 7.0f, -21.2f);
		if (auto sharedEngine = Engines[10].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[11] = vw_CreateParticleSystem();
		EnginesLocation[11] = sVECTOR3D(-4.0f, 7.0f, -21.2f);
		if (auto sharedEngine = Engines[11].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[12] = vw_CreateParticleSystem();
		EnginesLocation[12] = sVECTOR3D(20.4f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[12].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[13] = vw_CreateParticleSystem();
		EnginesLocation[13] = sVECTOR3D(-20.4f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[13].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[14] = vw_CreateParticleSystem();
		EnginesLocation[14] = sVECTOR3D(25.2f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[14].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[15] = vw_CreateParticleSystem();
		EnginesLocation[15] = sVECTOR3D(-25.2f, -2.4f, -20.0f);
		if (auto sharedEngine = Engines[15].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[16] = vw_CreateParticleSystem();
		EnginesLocation[16] = sVECTOR3D(35.7f, -3.0f, -13.0f);
		if (auto sharedEngine = Engines[16].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 12);
			sharedEngine->Direction = sVECTOR3D(0.4f, 0.0f, -0.8f);
		}
		Engines[17] = vw_CreateParticleSystem();
		EnginesLocation[17] = sVECTOR3D(-35.7f, -3.0f, -13.0f);
		if (auto sharedEngine = Engines[17].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 12);
			sharedEngine->Direction = sVECTOR3D(-0.4f, 0.0f, -0.8f);
		}
		Engines[18] = vw_CreateParticleSystem();
		EnginesLocation[18] = sVECTOR3D(0.0f, -2.4f, -23.0f);
		if (auto sharedEngine = Engines[18].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 13);
		break;


	case 6:
		// оружие
		WeaponSlots[0](CreateWeapon(103), sVECTOR3D(0.0f, -4.4f, 18.0f));
		WeaponSlots[0].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[1](CreateWeapon(103), sVECTOR3D(0.0f, -4.4f, 18.0f));
		WeaponSlots[1].Weapon->SetRotation(sVECTOR3D(0.0f, 3.0f, 0.0f));
		WeaponSlots[1].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[2](CreateWeapon(103), sVECTOR3D(0.0f, -4.4f, 18.0f));
		WeaponSlots[2].Weapon->SetRotation(sVECTOR3D(0.0f, -3.0f, 0.0f));
		WeaponSlots[2].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[3](CreateWeapon(103), sVECTOR3D(0.0f, -4.4f, 18.0f));
		WeaponSlots[3].Weapon->SetRotation(sVECTOR3D(0.0f, 6.0f, 0.0f));
		WeaponSlots[3].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[4](CreateWeapon(103), sVECTOR3D(0.0f, -4.4f, 18.0f));
		WeaponSlots[4].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[5](CreateWeapon(103), sVECTOR3D(0.0f, -4.4f, 18.0f));
		WeaponSlots[5].Weapon->SetRotation(sVECTOR3D(0.0f, -6.0f, 0.0f));
		WeaponSlots[5].Weapon->NextFireTime /= 2.0f;

		WeaponSlots[6](CreateWeapon(104), sVECTOR3D(7.0f, -4.4f, 13.0f));
		WeaponSlots[6].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[7](CreateWeapon(104), sVECTOR3D(-7.0f, -4.4f, 13.0f));
		WeaponSlots[7].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[8](CreateWeapon(104), sVECTOR3D(12.0f, -4.4f, 10.0f));
		WeaponSlots[8].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[9](CreateWeapon(104), sVECTOR3D(-12.0f, -4.4f, 10.0f));
		WeaponSlots[9].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[10](CreateWeapon(104), sVECTOR3D(17.0f, -4.4f, 8.0f));
		WeaponSlots[10].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[11](CreateWeapon(104), sVECTOR3D(-17.0f, -4.4f, 8.0f));
		WeaponSlots[11].Weapon->NextFireTime /= 2.0f;

		BossWeaponSlots[0](CreateWeapon(106), sVECTOR3D(10.0f, -6.4f, 10.0f));
		BossWeaponSlots[1](CreateWeapon(106), sVECTOR3D(-10.0f, -6.4f, 10.0f));
		BossWeaponSlots[2](CreateWeapon(106), sVECTOR3D(15.0f, -6.4f, 8.0f));
		BossWeaponSlots[3](CreateWeapon(106), sVECTOR3D(-15.0f, -6.4f, 8.0f));
		BossWeaponSlots[4](CreateWeapon(106), sVECTOR3D(20.0f, -6.4f, 5.0f));
		BossWeaponSlots[5](CreateWeapon(106), sVECTOR3D(-20.0f, -6.4f, 5.0f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(0.0f, -7.0f, -23.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 13);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-25.4f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(20.6f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-20.6f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(23.4f, -1.0f, -20.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-23.4f, -1.0f, -20.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(23.4f, -7.0f, -20.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-23.4f, -7.0f, -20.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(5.3f, 4.7f, -22.9f);
		if (auto sharedEngine = Engines[8].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(-5.3f, 4.7f, -22.9f);
		if (auto sharedEngine = Engines[9].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 10);
		Engines[10] = vw_CreateParticleSystem();
		EnginesLocation[10] = sVECTOR3D(2.6f, 6.6f, -21.5f);
		if (auto sharedEngine = Engines[10].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[11] = vw_CreateParticleSystem();
		EnginesLocation[11] = sVECTOR3D(-2.6f, 6.6f, -21.5f);
		if (auto sharedEngine = Engines[11].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[12] = vw_CreateParticleSystem();
		EnginesLocation[12] = sVECTOR3D(2.6f, 2.0f, -21.5f);
		if (auto sharedEngine = Engines[12].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[13] = vw_CreateParticleSystem();
		EnginesLocation[13] = sVECTOR3D(-2.6f, 2.0f, -21.5f);
		if (auto sharedEngine = Engines[13].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		Engines[14] = vw_CreateParticleSystem();
		EnginesLocation[14] = sVECTOR3D(25.4f, -4.0f, -20.0f);
		if (auto sharedEngine = Engines[14].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 11);
		break;

	case 7:
		// оружие
		WeaponSlots[0](CreateWeapon(108), sVECTOR3D(0.0f, -1.4f, 15.0f));
		WeaponSlots[0].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[1](CreateWeapon(108), sVECTOR3D(2.0f, -1.4f, 15.0f));
		WeaponSlots[1].Weapon->SetRotation(sVECTOR3D(0.0f, 3.0f, 0.0f));
		WeaponSlots[1].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[2](CreateWeapon(108), sVECTOR3D(-2.0f, -1.4f, 15.0f));
		WeaponSlots[2].Weapon->SetRotation(sVECTOR3D(0.0f, -3.0f, 0.0f));
		WeaponSlots[2].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[3](CreateWeapon(108), sVECTOR3D(4.0f, -1.4f, 15.0f));
		WeaponSlots[3].Weapon->SetRotation(sVECTOR3D(0.0f, 6.0f, 0.0f));
		WeaponSlots[3].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[4](CreateWeapon(108), sVECTOR3D(-4.0f, -1.4f, 15.0f));
		WeaponSlots[4].Weapon->SetRotation(sVECTOR3D(0.0f, -6.0f, 0.0f));
		WeaponSlots[4].Weapon->NextFireTime /= 2.0f;

		BossWeaponSlots[0](CreateWeapon(110), sVECTOR3D(12.5f, 1.6f, -15.3f));
		BossWeaponSlots[1](CreateWeapon(110), sVECTOR3D(-12.5f, 1.6f, -15.3f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(0.0f, 2.3f, -25.0f);
		if (auto sharedEngine = Engines[0].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 15);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(12.5f, 1.6f, -20.3f);
		if (auto sharedEngine = Engines[1].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 16);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(-12.5f, 1.6f, -20.3f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 16);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 17);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 18);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(0.0f, -8.0f, 0.0f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 19);
		break;




	case 8:
		// оружие
		WeaponSlots[0](CreateWeapon(104), sVECTOR3D(2.0f, -2.4f, 15.0f));
		WeaponSlots[0].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[1](CreateWeapon(104), sVECTOR3D(-2.0f, -2.4f, 15.0f));
		WeaponSlots[1].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[2](CreateWeapon(104), sVECTOR3D(2.0f, -4.4f, 15.0f));
		WeaponSlots[2].Weapon->NextFireTime /= 2.0f;
		WeaponSlots[3](CreateWeapon(104), sVECTOR3D(-2.0f, -4.4f, 15.0f));
		WeaponSlots[3].Weapon->NextFireTime /= 2.0f;

		BossWeaponSlots[0](CreateWeapon(110), sVECTOR3D(8.0f, -1.0f, 15.0f));
		BossWeaponSlots[1](CreateWeapon(110), sVECTOR3D(-8.0f, -1.0f, 15.0f));
		BossWeaponSlots[2](CreateWeapon(107), sVECTOR3D(10.0f, -6.4f, 8.0f));
		BossWeaponSlots[3](CreateWeapon(107), sVECTOR3D(-10.0f, -6.4f, 8.0f));
		BossWeaponSlots[4](CreateWeapon(107), sVECTOR3D(15.0f, -6.4f, 5.0f));
		BossWeaponSlots[5](CreateWeapon(107), sVECTOR3D(-15.0f, -6.4f, 5.0f));

		// двигатели
		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D(15.0f, 4.4f, -29.0f);
		if (auto sharedEngine = Engines[0].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 15);
			sharedEngine->CreationSize = sVECTOR3D(4.0f,6.0f,3.0f);
		}
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D(-15.0f, 4.4f, -29.0f);
		if (auto sharedEngine = Engines[1].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 15);
			sharedEngine->CreationSize = sVECTOR3D(4.0f,6.0f,3.0f);
		}
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D(5.2f, 5.4f, -20.6f);
		if (auto sharedEngine = Engines[2].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D(-5.2f, 5.4f, -20.6f);
		if (auto sharedEngine = Engines[3].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D(5.2f, -8.8f, -22.6f);
		if (auto sharedEngine = Engines[4].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D(-5.2f, -8.8f, -22.6f);
		if (auto sharedEngine = Engines[5].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D(11.2f, -2.8f, -10.0f);
		if (auto sharedEngine = Engines[6].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D(-11.2f, -2.8f, -10.0f);
		if (auto sharedEngine = Engines[7].lock())
			SetAlienSpaceMotherShipEngine(sharedEngine, 20);
		Engines[8] = vw_CreateParticleSystem();
		EnginesLocation[8] = sVECTOR3D(0.0f, -10.0f, -3.0f);
		if (auto sharedEngine = Engines[8].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 17);
			sharedEngine->CreationSize = sVECTOR3D(17.0f,1.0f,17.0f);
			sharedEngine->DeadZone = 16.9f;
		}
		Engines[9] = vw_CreateParticleSystem();
		EnginesLocation[9] = sVECTOR3D(0.0f, -10.0f, -3.0f);
		if (auto sharedEngine = Engines[9].lock()) {
			SetAlienSpaceMotherShipEngine(sharedEngine, 18);
			sharedEngine->CreationSize = sVECTOR3D(17.0f,1.0f,17.0f);
			sharedEngine->DeadZone = 16.9f;
		}
		break;



	default:
		std::cerr << __func__ << "(): " << "wrong SpaceShipNum.\n";
		return;
	}

	for (unsigned int i = 0; i < Engines.size(); i++) {
		if (auto sharedEngine = Engines[i].lock()) {
			sharedEngine->SetStartLocation(EnginesLocation[i]);
			// находим кол-во внутренних источников света
			if (!sharedEngine->Light.expired())
				InternalLights++;
		}
	}
}

} // astromenace namespace
} // viewizard namespace
