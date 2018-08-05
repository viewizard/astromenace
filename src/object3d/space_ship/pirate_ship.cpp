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

// TODO add more flare weapons for big pirate ships

// TODO translate comments

#include "space_ship.h"
#include "../../config/config.h"
#include "../../assets/audio.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

struct sPirateShipData {
	unsigned int EngineQuantity;
	float Strength;
	std::string Name;
	std::string TextureName;
	std::string NormalMapName;
};

const std::vector<sPirateShipData> PresetPirateShipData{
	{2,	20,	"models/pirateship/gunship-01.vw3d",	"models/gr-04.vw2d", ""},
	{2,	30,	"models/pirateship/bomber-07.vw3d",	"models/gr-04.vw2d", ""},
	{2,	100,	"models/pirateship/gunship-03.vw3d",	"models/gr-03.vw2d", ""},
	{2,	200,	"models/pirateship/gunship-04.vw3d",	"models/gr-03.vw2d", ""},
	{6,	400,	"models/pirateship/gunship-02.vw3d",	"models/gr-03.vw2d", ""},

	{2,	600,	"models/pirateship/bomber-03.vw3d",	"models/gr-05.vw2d", ""},
	{4,	1200,	"models/pirateship/bomber-02.vw3d",	"models/gr-05.vw2d", ""},
	{4,	1000,	"models/pirateship/bomber-04.vw3d",	"models/gr-03.vw2d", ""},
	{6,	2000,	"models/pirateship/bomber-05.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"},
	{8,	2500,	"models/pirateship/bomber-06.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"},

	{2,	300,	"models/pirateship/bomber-03.vw3d",	"models/gr-05.vw2d", ""},
	{4,	600,	"models/pirateship/bomber-02.vw3d",	"models/gr-05.vw2d", ""},
	{4,	500,	"models/pirateship/bomber-04.vw3d",	"models/gr-03.vw2d", ""},
	{6,	1000,	"models/pirateship/bomber-05.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"},
	{8,	1500,	"models/pirateship/bomber-06.vw3d",	"models/gr-04.vw2d", "models/normalmap/bomber_nm.tga"}
};

} // unnamed namespace


//-----------------------------------------------------------------------------
// Создание двигателя
//-----------------------------------------------------------------------------
static void SetPirateShipEngine(std::shared_ptr<cParticleSystem> &ParticleSystem, int EngineType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
	ParticleSystem->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};

	switch(EngineType) {
	case 1:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.10f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.40f;
		ParticleSystem->Speed      = 6.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 5.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{0.4f, 0.4f, 0.4f};
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 2:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.30f;
		ParticleSystem->SizeEnd    = 0.60f;
		ParticleSystem->Speed      = 8.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 5.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{0.6f, 0.6f, 0.1f};
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 3:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.30f;
		ParticleSystem->SizeEnd    = 0.60f;
		ParticleSystem->Speed      = 10.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 5.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{1.0f, 0.1f, 1.0f};
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D{0.0f, -1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.05f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 4:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 180.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D{0.0f, 1.0f, 0.0f};
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 5:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.30f;
		ParticleSystem->SizeEnd    = 0.60f;
		ParticleSystem->Speed      = 8.00f;
		ParticleSystem->SpeedOnCreation	   = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 5.00f;
		ParticleSystem->Life       = 0.40f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D{1.0f, 1.0f, 1.0f};
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.05f);
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
cPirateShip::cPirateShip(const int SpaceShipNum)
{
	if ((SpaceShipNum <= 0) ||
	    (static_cast<unsigned>(SpaceShipNum) > PresetPirateShipData.size())) {
		std::cerr << __func__ << "(): "
			  << "Could not init cPirateShip object with Number "
			  << SpaceShipNum << "\n";
		return;
	}

	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::PirateShip;
	InternalType = SpaceShipNum;
	PromptDrawDist2 = 100.0f;
	if (SpaceShipNum > 5)
		PromptDrawDist2 = 20000.0f; // большие корабли пиратов

	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	MaxSpeed = 20.0f;
	MaxAcceler = 20.0f;
	MaxSpeedRotate = 40.0f;
	Strength = StrengthStart = PresetPirateShipData[SpaceShipNum - 1].Strength / GameEnemyArmorPenalty;
	unsigned int EngineQuantity = PresetPirateShipData[SpaceShipNum - 1].EngineQuantity;

	LoadObjectData(PresetPirateShipData[SpaceShipNum - 1].Name, *this);

	// всегда только эти текстуры
	for (unsigned int i = 0; i < Chunks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetPirateShipData[SpaceShipNum - 1].TextureName);
		if (!PresetPirateShipData[SpaceShipNum - 1].NormalMapName.empty() && GameConfig().UseGLSL120)
			NormalMap[i] = GetPreloadedTextureAsset(PresetPirateShipData[SpaceShipNum - 1].NormalMapName);
	}

	// начальные установки для двигателей
	EnginesLocation.resize(EngineQuantity);
	Engines.resize(EngineQuantity);

	switch (SpaceShipNum) {
	case 1:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;

		WeaponSlots.reserve(2);
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{1.8f, -1.2f, 6.0f});
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-1.8f, -1.2f, 6.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->SFX = eGameSFX::none;
		WeaponFireType = 1;

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{0.9f, 1.6f, -5.4f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-0.9f, 1.6f, -5.4f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 1);
		break;

	case 2:
		MaxSpeed = 50.0f;
		MaxAcceler = 50.0f;

		WeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{0.0f, -2.0f, 2.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.1f, 0.5f, -5.7f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.1f, 0.5f, -5.7f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 3:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;

		WeaponSlots.reserve(4);
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{3.0f, -0.2f, -0.5f});
		WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-3.0f, -0.2f, -0.5f});
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{2.0f, -0.2f, 0.0f});
		WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-2.0f, -0.2f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.0f, 1.8f, -1.2f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 1);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.0f, 1.8f, -1.2f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 1);
		break;
	case 4:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;

		WeaponSlots.reserve(4);
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{4.3f, -0.4f, -0.5f});
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{-4.3f, -0.4f, -0.5f});
		WeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{3.0f, -0.4f, 0.5f});
		WeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-3.0f, -0.4f, 0.5f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.3f, 1.9f, -1.7f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.3f, 1.9f, -1.7f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 5:
		MaxSpeed = 30.0f;
		MaxAcceler = 30.0f;

		WeaponSlots.reserve(4);
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{1.0f, 4.2f, -5.5f});
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{-1.0f, 4.2f, -5.5f});
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{0.5f, -1.8f, 5.0f});
		WeaponSlots.emplace_back(CreateWeapon(211), sVECTOR3D{-0.5f, -1.8f, 5.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{5.8f, 6.5f - 4.6f, 0.0f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 3);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-5.8f, 6.5f - 4.6f, 0.0f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 3);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{5.8f, 7.3f - 4.6f, 0.0f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 4);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-5.8f, 7.3f - 4.6f, 0.0f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 4);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D{5.8f, 6.1f - 4.6f, -2.3f};
		if (auto sharedEngine = Engines[4].lock()) {
			SetPirateShipEngine(sharedEngine, 1);
			sharedEngine->Direction = sVECTOR3D{0.0f, -0.2f, -0.8f};
		}
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D{-5.8f, 6.1f - 4.6f, -2.3f};
		if (auto sharedEngine = Engines[5].lock()) {
			SetPirateShipEngine(sharedEngine, 1);
			sharedEngine->Direction = sVECTOR3D{0.0f, -0.2f, -0.8f};
		}
		break;

	case 6:
		WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 2.2f, -0.2f});

		BossWeaponSlots.reserve(2);
		BossWeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{5.4f, 1.0f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{-5.4f, 1.0f, 2.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.7f, 6.6f - 3.83f, -14.2f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.7f, 6.6f - 3.83f, -14.2f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 7:
		WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 2.4f, -0.2f});

		BossWeaponSlots.reserve(2);
		BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{9.5f, 1.3f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{-9.5f, 1.3f, 2.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{2.2f, 7.6f - 5.25f, -14.8f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-2.2f, 7.6f - 5.25f, -14.8f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{17.2f, 6.8f - 5.25f, -9.0f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-17.2f, 6.8f - 5.25f, -9.0f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 8:
		WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 2.4f, -0.2f});

		BossWeaponSlots.reserve(6);
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{9.1f, 1.3f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{9.4f, 1.0f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{9.1f, 0.7f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-9.1f, 1.3f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-9.4f, 1.0f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-9.1f, 0.7f, 2.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 6.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{4.5f, 6.3f - 3.62f, -12.5f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-4.5f, 6.3f - 3.62f, -12.5f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{9.2f, 4.4f - 3.62f, -5.6f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-9.2f, 4.4f - 3.62f, -5.6f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 9:
		WeaponSlots.reserve(3);
		WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.2f, 7.0f});
		WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.2f, 3.5f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->SetRotation(sVECTOR3D{0.0, 180.0, 0.0});
		WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 3.4f, -5.0f});

		BossWeaponSlots.reserve(2);
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 9.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 9.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.4f, 8.7f - 4.9f, -20.4f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.4f, 8.7f - 4.9f, -20.4f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{4.7f, 7.0f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-4.7f, 7.0f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D{7.5f, 6.8f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D{-7.5f, 6.8f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 10:
		WeaponSlots.reserve(4);
		WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.4f, 6.8f});
		WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.4f, 2.6f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->SetRotation(sVECTOR3D{0.0f, 180.0f, 0.0f});
		WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 2.6f, -7.0f});
		WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 2.6f, -16.0f});
		if (auto sharedWeapon = WeaponSlots.back().Weapon.lock())
			sharedWeapon->SetRotation(sVECTOR3D{0.0f, 180.0f, 0.0f});

		BossWeaponSlots.reserve(4);
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 8.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 8.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{16.5f, 2.7f, 7.5f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-16.5f, 2.7f, 7.5f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{3.8f, 5.4f - 4.29f, -19.5f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-3.8f, 5.4f - 4.29f, -19.5f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{5.8f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-5.8f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D{8.0f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D{-8.0f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D{20.0f, 5.0f - 4.29f, -3.6f};
		if (auto sharedEngine = Engines[6].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D{-20.0f, 5.0f - 4.29f, -3.6f};
		if (auto sharedEngine = Engines[7].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 11:
		BossWeaponSlots.reserve(2);
		BossWeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{5.4f, 1.0f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{-5.4f, 1.0f, 2.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.7f, 6.6f - 3.83f, -14.2f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.7f, 6.6f - 3.83f, -14.2f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 12:
		BossWeaponSlots.reserve(2);
		BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{9.5f, 1.3f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{-9.5f, 1.3f, 2.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{2.2f, 7.6f - 5.25f, -14.8f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-2.2f, 7.6f - 5.25f, -14.8f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{17.2f, 6.8f - 5.25f, -9.0f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-17.2f, 6.8f - 5.25f, -9.0f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 13:
		BossWeaponSlots.reserve(6);
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{9.1f, 1.3f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{9.4f, 1.0f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{9.1f, 0.7f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-9.1f, 1.3f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-9.4f, 1.0f, 2.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-9.1f, 0.7f, 2.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 6.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{4.5f, 6.3f - 3.62f, -12.5f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-4.5f, 6.3f - 3.62f, -12.5f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{9.2f, 4.4f - 3.62f, -5.6f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 5);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-9.2f, 4.4f - 3.62f, -5.6f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 5);
		break;

	case 14:
		BossWeaponSlots.reserve(2);
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 9.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 9.0f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{1.4f, 8.7f - 4.9f, -20.4f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-1.4f, 8.7f - 4.9f, -20.4f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{4.7f, 7.0f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-4.7f, 7.0f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D{7.5f, 6.8f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D{-7.5f, 6.8f - 4.9f, 0.0f};
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;

	case 15:
		BossWeaponSlots.reserve(4);
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 8.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 8.0f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{16.5f, 2.7f, 7.5f});
		BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-16.5f, 2.7f, 7.5f});

		FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

		Engines[0] = vw_CreateParticleSystem();
		EnginesLocation[0] = sVECTOR3D{3.8f, 5.4f - 4.29f, -19.5f};
		if (auto sharedEngine = Engines[0].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[1] = vw_CreateParticleSystem();
		EnginesLocation[1] = sVECTOR3D{-3.8f, 5.4f - 4.29f, -19.5f};
		if (auto sharedEngine = Engines[1].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[2] = vw_CreateParticleSystem();
		EnginesLocation[2] = sVECTOR3D{5.8f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[2].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[3] = vw_CreateParticleSystem();
		EnginesLocation[3] = sVECTOR3D{-5.8f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[3].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[4] = vw_CreateParticleSystem();
		EnginesLocation[4] = sVECTOR3D{8.0f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[4].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[5] = vw_CreateParticleSystem();
		EnginesLocation[5] = sVECTOR3D{-8.0f, 6.0f - 4.29f, -0.9f};
		if (auto sharedEngine = Engines[5].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[6] = vw_CreateParticleSystem();
		EnginesLocation[6] = sVECTOR3D{20.0f, 5.0f - 4.29f, -3.6f};
		if (auto sharedEngine = Engines[6].lock())
			SetPirateShipEngine(sharedEngine, 2);
		Engines[7] = vw_CreateParticleSystem();
		EnginesLocation[7] = sVECTOR3D{-20.0f, 5.0f - 4.29f, -3.6f};
		if (auto sharedEngine = Engines[7].lock())
			SetPirateShipEngine(sharedEngine, 2);
		break;
	}

	for (unsigned int i = 0; i < EngineQuantity; i++) {
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
