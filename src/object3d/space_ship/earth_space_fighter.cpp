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

// TODO split earth fighter and player ship classes

// TODO don't call GetPreloadedTextureAsset() all the time, use cached texture instead

// TODO switch to enumeration EngineType in CreateRotateSpaceShipEngine(), CreateSpaceShipEngine()

#include "space_ship.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

struct sEarthSpaceFighterData {
	float Armor;
	unsigned int WeaponQuantity;
	unsigned int EngineQuantity;
	std::string Name;
};

const std::vector<sEarthSpaceFighterData> PresetEarthSpaceFighterData{
	{30,	5,	2,	"models/earthfighter/sf-01.vw3d"},
	{40,	4,	4,	"models/earthfighter/sf-02.vw3d"},
	{60,	3,	4,	"models/earthfighter/sf-03.vw3d"},
	{35,	5,	2,	"models/earthfighter/sf-04.vw3d"},
	{45,	2,	4,	"models/earthfighter/sf-05.vw3d"},
	{50,	5,	2,	"models/earthfighter/sf-06.vw3d"},
	{60,	5,	3,	"models/earthfighter/sf-07.vw3d"},
	{25,	2,	4,	"models/earthfighter/sf-08.vw3d"},
	{55,	2,	2,	"models/earthfighter/sf-09.vw3d"},
	{55,	5,	2,	"models/earthfighter/sf-10.vw3d"},
	{60,	3,	2,	"models/earthfighter/sf-11.vw3d"},
	{70,	3,	2,	"models/earthfighter/sf-12.vw3d"},
	{80,	5,	4,	"models/earthfighter/sf-13.vw3d"},
	{65,	5,	2,	"models/earthfighter/sf-14.vw3d"},
	{70,	3,	2,	"models/earthfighter/sf-15.vw3d"},
	{90,	3,	4,	"models/earthfighter/sf-16.vw3d"},
	{75,	5,	2,	"models/earthfighter/sf-17.vw3d"},
	{110,	4,	4,	"models/earthfighter/sf-18.vw3d"},
	{55,	3,	4,	"models/earthfighter/sf-19.vw3d"},
	{80,	4,	4,	"models/earthfighter/sf-20.vw3d"},
	{80,	3,	2,	"models/earthfighter/sf-21.vw3d"},
	{100,	1,	1,	"models/earthfighter/sf-22.vw3d"}
};

} // unnamed namespace


/*
 * Get engine power.
 */
float GetEnginePower(const int EngineType)
{
	switch (EngineType) {
	case 1:
		return 20.0f;
	case 2:
		return 25.0f;
	case 3:
		return 30.0f;
	case 4:
		return 35.0f;
	default:
		std::cerr << __func__ << "(): " << "wrong EngineType.\n";
		break;
	}

	return 0.0f;
}

/*
 * Get max engine acceleration.
 */
float GetEngineAcceleration(const int EngineType)
{
	switch (EngineType) {
	case 1:
		return 15.0f;
	case 2:
		return 19.0f;
	case 3:
		return 24.0f;
	case 4:
		return 28.0f;
	default:
		std::cerr << __func__ << "(): " << "wrong EngineType.\n";
		break;
	}

	return 0.0f;
}

/*
 * Get rotate engine power.
 */
float GetEngineRotatePower(const int EngineType)
{
	switch (EngineType) {
	case 1:
		return 15.0f;
	case 2:
		return 18.0f;
	case 3:
		return 20.0f;
	case 4:
		return 23.0f;
	default:
		std::cerr << __func__ << "(): " << "wrong EngineType.\n";
		break;
	}

	return 0.0f;
}

/*
 * Setup weapon.
 */
bool SetEarthSpaceFighterWeapon(std::weak_ptr<cSpaceShip> &SpaceShip, const int WeaponSlot, const int WeaponNum)
{
	auto sharedSpaceShip = SpaceShip.lock();
	if (!sharedSpaceShip)
		return false;

	if (static_cast<int>(sharedSpaceShip->WeaponSlots.size()) < WeaponSlot)
		return false;

	if (!sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Weapon.expired())
		ReleaseWeapon(sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Weapon);

	sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Weapon = CreateWeapon(WeaponNum);

	auto sharedWeapon = sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Weapon.lock();
	if (!sharedWeapon)
		return false;

	if (sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Type < sharedWeapon->WeaponLevel) {
		ReleaseWeapon(sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Weapon);
		return false;
	}

	sharedWeapon->SetLocation(sharedSpaceShip->WeaponSlots[WeaponSlot - 1].Location);
	sharedWeapon->ObjectStatus = sharedSpaceShip->ObjectStatus;

	return true;
}

/*
 * Setup engine gfx.
 */
static void CreateSpaceShipEngine(std::shared_ptr<cParticleSystem> &ParticleSystem, const int EngineType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");

	static const sRGBCOLOR LightYellow{1.0f, 0.7f, 0.3f};
	static const sRGBCOLOR Red{1.0f, 0.0f, 0.0f};

	static const sRGBCOLOR MuddyYellow{0.6f, 0.6f, 0.3f};
	static const sRGBCOLOR LightGreen{0.3f, 1.0f, 0.3f};

	static const sRGBCOLOR Violet{0.5f, 0.5f, 1.0f};
	static const sRGBCOLOR LightViolet{0.0f, 0.4f, 1.0f};

	static const sRGBCOLOR Turquoise{0.0f, 0.7f, 1.0f};
	static const sRGBCOLOR LightSkyey{0.7f, 1.0f, 1.0f};

	switch (EngineType) {
	case 1:
		ParticleSystem->ColorStart = LightYellow;
		ParticleSystem->ColorEnd = Red;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeVar = 0.50f;
		ParticleSystem->SizeEnd = 0.20f;
		ParticleSystem->Speed = 7.00f;
		ParticleSystem->SpeedOnCreation = 7.00f;
		ParticleSystem->SpeedVar = 2.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.07f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 2:
		ParticleSystem->ColorStart = MuddyYellow;
		ParticleSystem->ColorEnd = LightGreen;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeVar = 0.70f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 7.00f;
		ParticleSystem->SpeedOnCreation = 7.00f;
		ParticleSystem->SpeedVar = 2.00f;
		ParticleSystem->Theta = 12.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.4f, 0.75f, 0.15f, 0.0f, 0.07f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 3:
		ParticleSystem->ColorStart = Violet;
		ParticleSystem->ColorEnd = LightViolet;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeVar = 0.50f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 7.00f;
		ParticleSystem->SpeedOnCreation = 7.00f;
		ParticleSystem->SpeedVar = 2.00f;
		ParticleSystem->Theta = 3.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->ParticlesPerSec = 80;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.25f, 0.25f, 1.0f, 0.0f, 0.07f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 4:
		ParticleSystem->ColorStart = Turquoise;
		ParticleSystem->ColorEnd = LightSkyey;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd = 0.00f;
		ParticleSystem->SizeStart = 0.30f;
		ParticleSystem->SizeVar = 0.00f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 12.00f;
		ParticleSystem->SpeedOnCreation = 12.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.30f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize(0.6f, 0.6f, 0.1f);
		ParticleSystem->IsMagnet = true;
		ParticleSystem->MagnetFactor = 50.0f;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.0f, 0.7f, 1.0f, 0.0f, 0.07f);
		ParticleSystem->LightNeedDeviation = true;
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong EngineType.\n";
		break;
	}
}

/*
 * Setup rotate engine gfx.
 */
static void CreateRotateSpaceShipEngine(std::shared_ptr<cParticleSystem> &ParticleSystem, const int EngineType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");

	static const sRGBCOLOR LightYellow{1.0f, 0.7f, 0.3f};
	static const sRGBCOLOR Red{1.0f, 0.0f, 0.0f};

	static const sRGBCOLOR MuddyYellow{0.6f, 0.6f, 0.3f};
	static const sRGBCOLOR LightGreen{0.3f, 1.0f, 0.3f};

	static const sRGBCOLOR Violet{0.5f, 0.5f, 1.0f};
	static const sRGBCOLOR LightViolet{0.0f, 0.4f, 1.0f};

	static const sRGBCOLOR Turquoise{0.0f, 0.7f, 1.0f};
	static const sRGBCOLOR LightSkyey{0.7f, 1.0f, 1.0f};

	switch (EngineType) {
	case 1:
		ParticleSystem->ColorStart = LightYellow;
		ParticleSystem->ColorEnd = Red;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.50f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 5.00f;
		ParticleSystem->SpeedVar = 5.00f;
		ParticleSystem->Theta = 40.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->IsSuppressed = true;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 1.0f, 0.35f, 0.15f, 0.0f, 0.2f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 2:
		ParticleSystem->ColorStart = MuddyYellow;
		ParticleSystem->ColorEnd = LightGreen;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.5f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 5.00f;
		ParticleSystem->SpeedVar = 5.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->IsSuppressed = true;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.4f, 0.75f, 0.15f, 0.0f, 0.2f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 3:
		ParticleSystem->ColorStart = Violet;
		ParticleSystem->ColorEnd = LightViolet;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.5f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 5.00f;
		ParticleSystem->SpeedVar = 5.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->IsSuppressed = true;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.25f, 0.25f, 1.0f, 0.0f, 0.2f);
		ParticleSystem->LightNeedDeviation = true;
		break;
	case 4:
		ParticleSystem->ColorStart = Turquoise;
		ParticleSystem->ColorEnd = LightSkyey;
		ParticleSystem->AlphaStart = 0.60f;
		ParticleSystem->AlphaEnd = 0.10f;
		ParticleSystem->SizeStart = 0.5f;
		ParticleSystem->SizeVar = 0.05f;
		ParticleSystem->SizeEnd = 0.10f;
		ParticleSystem->Speed = 5.00f;
		ParticleSystem->SpeedVar = 5.00f;
		ParticleSystem->Theta = 30.00f;
		ParticleSystem->Life = 0.50f;
		ParticleSystem->IsSuppressed = true;
		ParticleSystem->ParticlesPerSec = 50;
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.35f, 0.85f, 1.0f, 0.0f, 0.2f);
		ParticleSystem->LightNeedDeviation = true;
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong EngineType.\n";
		break;
	}
}

/*
 * Setup engines.
 */
void SetEarthSpaceFighterEngine(std::weak_ptr<cSpaceShip> &SpaceShip, const int EngineType)
{
	auto sharedSpaceShip = SpaceShip.lock();
	if (!sharedSpaceShip)
		return;

	if (EngineType == 0) {
		for (auto &tmpEngine : sharedSpaceShip->Engines) {
			vw_ReleaseParticleSystem(tmpEngine);
		}

		if (!sharedSpaceShip->EnginesLeft.empty()) {
			for (auto &tmpEngineLeft : sharedSpaceShip->EnginesLeft) {
				vw_ReleaseParticleSystem(tmpEngineLeft);
			}
		}

		if (!sharedSpaceShip->EnginesRight.empty()) {
			for (auto &tmpEngineRight : sharedSpaceShip->EnginesRight) {
				vw_ReleaseParticleSystem(tmpEngineRight);
			}
		}

		sharedSpaceShip->MaxSpeed = 0.0f;
		sharedSpaceShip->MaxAcceler = 0.0f;
		sharedSpaceShip->MaxSpeedRotate = 0.0f;
		return;
	}

	for (unsigned int i = 0; i < sharedSpaceShip->Engines.size(); i++) {
		vw_ReleaseParticleSystem(sharedSpaceShip->Engines[i]);
		sharedSpaceShip->Engines[i] = vw_CreateParticleSystem();
		if (auto sharedEngine = sharedSpaceShip->Engines[i].lock()) {
			CreateSpaceShipEngine(sharedEngine, EngineType);
			sharedEngine->SetStartLocation(sharedSpaceShip->EnginesLocation[i]);
			sharedEngine->Direction = sVECTOR3D{0.0f, 0.0f, -1.0f};
		}
	}

	if (!sharedSpaceShip->EnginesLeft.empty()) {
		for (unsigned int i = 0; i < sharedSpaceShip->EnginesLeft.size(); i++) {
			vw_ReleaseParticleSystem(sharedSpaceShip->EnginesLeft[i]);
			sharedSpaceShip->EnginesLeft[i] = vw_CreateParticleSystem();
			if (auto sharedEngineLeft = sharedSpaceShip->EnginesLeft[i].lock()) {
				CreateRotateSpaceShipEngine(sharedEngineLeft, EngineType);
				sharedEngineLeft->SetStartLocation(sharedSpaceShip->EnginesLeftLocation[i]);
				sharedEngineLeft->Direction = sVECTOR3D{1.0f, 0.0f, 0.6f};
				sharedEngineLeft->IsSuppressed = true;
			}
		}
	}

	if (!sharedSpaceShip->EnginesRight.empty()) {
		for (unsigned int i = 0; i < sharedSpaceShip->EnginesRight.size(); i++) {
			vw_ReleaseParticleSystem(sharedSpaceShip->EnginesRight[i]);
			sharedSpaceShip->EnginesRight[i] = vw_CreateParticleSystem();
			if (auto sharedEngineRight = sharedSpaceShip->EnginesRight[i].lock()) {
				CreateRotateSpaceShipEngine(sharedEngineRight, EngineType);
				sharedEngineRight->SetStartLocation(sharedSpaceShip->EnginesRightLocation[i]);
				sharedEngineRight->Direction = sVECTOR3D{-1.0f, 0.0f, 0.6f};
				sharedEngineRight->IsSuppressed = true;
			}
		}
	}

	sharedSpaceShip->MaxSpeed = GetEnginePower(EngineType) * sharedSpaceShip->Engines.size();
	sharedSpaceShip->MaxAcceler = GetEngineAcceleration(EngineType) * sharedSpaceShip->Engines.size();
	sharedSpaceShip->MaxSpeedRotate = GetEngineRotatePower(EngineType) * sharedSpaceShip->Engines.size();
}

/*
 * Get texture for particular armor.
 * Note, we use only 4 for player ship, but all 8 could be used by script.
 */
static GLtexture GetArmourTexture(const int ArmourType)
{
	switch (ArmourType) {
	case 0:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text00.vw2d");

	case 1:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text04.vw2d");

	case 2:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text05.vw2d");

	case 3:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text07.vw2d");

	case 4:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text08.vw2d");

	case 5:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text09.vw2d");

	case 6:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text10.vw2d");

	case 7:
		return GetPreloadedTextureAsset("models/earthfighter/sf-text06.vw2d");

	default:
		std::cerr << __func__ << "(): " << "wrong ArmourType.\n";
		break;
	}

	return 0;
}

/*
 * Get illumination texture for particular armor.
 * Note, we use only 4 for player ship, but all 8 could be used by script.
 */
static GLtexture GetArmourIllumTexture(const int ArmourType)
{
	switch (ArmourType) {
	case 0:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum01.vw2d");

	case 1:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum02.vw2d");

	case 2:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum03.vw2d");

	case 3:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum03.vw2d");

	case 4:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum03.vw2d");

	case 5:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum02.vw2d");

	case 6:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum04.vw2d");

	case 7:
		return GetPreloadedTextureAsset("models/earthfighter/sf-illum03.vw2d");

	default:
		std::cerr << __func__ << "(): " << "wrong ArmourType.\n";
		break;
	}

	return 0;
}

/*
 * Setup armor.
 */
void SetEarthSpaceFighterArmour(std::weak_ptr<cSpaceShip> &SpaceShip, const int ArmourType)
{
	auto sharedSpaceShip = SpaceShip.lock();
	if (!sharedSpaceShip)
		return;

	for (unsigned int i = 0; i < sharedSpaceShip->Chunks.size(); i++) {
		sharedSpaceShip->Texture[i] = GetArmourTexture(ArmourType);
		sharedSpaceShip->TextureIllum[i] = GetArmourIllumTexture(ArmourType);
	}
}

/*
 * Get max and min weapon slot angle for particular ship.
 */
void GetShipWeaponSlotAngle(const int ShipNum, const int SlotNum, float &Min, float &Max)
{
	Min = 0.0f;
	Max = 0.0f;

	switch (ShipNum) {
	case 1:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 5.0f;
			break;
		case 1:
			Min = -5.0f;
			Max = 45.0f;
			break;
		case 2:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 3:
			Min = -10.0f;
			Max = 45.0f;
			break;
		case 4:
			Min = -30.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 2:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 5.0f;
			break;
		case 1:
			Min = -5.0f;
			Max = 45.0f;
			break;
		case 2:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 3:
			Min = -10.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 3:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 45.0f;
			break;
		case 1:
			Min = -20.0f;
			Max = 20.0f;
			break;
		case 2:
			Min = -20.0f;
			Max = 20.0f;
			break;
		}
		break;
	case 4:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -10.0f;
			Max = 45.0f;
			break;
		case 2:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 3:
			Min = -10.0f;
			Max = 45.0f;
			break;
		case 4:
			Min = -30.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 5:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 5.0f;
			break;
		case 1:
			Min = -5.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 6:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 5.0f;
			break;
		case 1:
			Min = -5.0f;
			Max = 45.0f;
			break;
		case 2:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 3:
			Min = -10.0f;
			Max = 45.0f;
			break;
		case 4:
			Min = -30.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 7:
		switch (SlotNum) {
		case 0:
			Min = -10.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -45.0f;
			Max = 25.0f;
			break;
		case 2:
			Min = -25.0f;
			Max = 45.0f;
			break;
		case 3:
			Min = -45.0f;
			Max = 15.0f;
			break;
		case 4:
			Min = -15.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 8:
		switch (SlotNum) {
		case 0:
			Min = -10.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -10.0f;
			Max = 10.0f;
			break;
		}
		break;
	case 9:
		switch (SlotNum) {
		case 0:
			Min = -10.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -10.0f;
			Max = 10.0f;
			break;
		}
		break;
	case 10:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -10.0f;
			Max = 45.0f;
			break;
		case 2:
			Min = -20.0f;
			Max = 5.0f;
			break;
		case 3:
			Min = -5.0f;
			Max = 20.0f;
			break;
		case 4:
			Min = -30.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 11:
		switch (SlotNum) {
		case 0:
			Min = -30.0f;
			Max = 0.0f;
			break;
		case 1:
			Min = -0.0f;
			Max = 30.0f;
			break;
		case 2:
			Min = -30.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 12:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 25.0f;
			break;
		case 1:
			Min = -25.0f;
			Max = 45.0f;
			break;
		case 2:
			Min = -30.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 13:
		switch (SlotNum) {
		case 0:
			Min = -10.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -45.0f;
			Max = 20.0f;
			break;
		case 2:
			Min = -20.0f;
			Max = 45.0f;
			break;
		case 3:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 4:
			Min = -10.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 14:
		switch (SlotNum) {
		case 0:
			Min = -30.0f;
			Max = 5.0f;
			break;
		case 1:
			Min = -5.0f;
			Max = 30.0f;
			break;
		case 2:
			Min = -35.0f;
			Max = 5.0f;
			break;
		case 3:
			Min = -5.0f;
			Max = 35.0f;
			break;
		case 4:
			Min = -15.0f;
			Max = 15.0f;
			break;
		}
		break;
	case 15:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 45.0f;
			break;
		case 1:
			Min = -45.0f;
			Max = 20.0f;
			break;
		case 2:
			Min = -20.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 16:
		switch (SlotNum) {
		case 0:
			Min = -20.0f;
			Max = 20.0f;
			break;
		case 1:
			Min = -30.0f;
			Max = 0.0f;
			break;
		case 2:
			Min = -0.0f;
			Max = 30.0f;
			break;
		}
		break;
	case 17:
		switch (SlotNum) {
		case 0:
			Min = -30.0f;
			Max = 5.0f;
			break;
		case 1:
			Min = -5.0f;
			Max = 30.0f;
			break;
		case 2:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 3:
			Min = -10.0f;
			Max = 45.0f;
			break;
		case 4:
			Min = -20.0f;
			Max = 20.0f;
			break;
		}
		break;
	case 18:
		switch (SlotNum) {
		case 0:
			Min = -20.0f;
			Max = 20.0f;
			break;
		case 1:
			Min = -20.0f;
			Max = 20.0f;
			break;
		case 2:
			Min = -25.0f;
			Max = 5.0f;
			break;
		case 3:
			Min = -5.0f;
			Max = 25.0f;
			break;
		}
		break;
	case 19:
		switch (SlotNum) {
		case 0:
			Min = -45.0f;
			Max = 45.0f;
			break;
		case 1:
			Min = -45.0f;
			Max = 10.0f;
			break;
		case 2:
			Min = -10.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 20:
		switch (SlotNum) {
		case 0:
			Min = -10.0f;
			Max = 10.0f;
			break;
		case 1:
			Min = -10.0f;
			Max = 10.0f;
			break;
		case 2:
			Min = -45.0f;
			Max = 20.0f;
			break;
		case 3:
			Min = -20.0f;
			Max = 45.0f;
			break;
		}
		break;
	case 21:
		switch (SlotNum) {
		case 0:
			Min = -25.0f;
			Max = 0.0f;
			break;
		case 1:
			Min = -0.0f;
			Max = 25.0f;
			break;
		case 2:
			Min = -20.0f;
			Max = 20.0f;
			break;
		}
		break;
	case 22:
		switch (SlotNum) {
		case 0:
			Min = -25.0f;
			Max = 25.0f;
			break;
		}
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong ShipNum.\n";
		break;
	}
}

/*
 * Get armor strength for particular ship.
 */
float GetShipArmor(const int SpaceShipNum)
{
	return PresetEarthSpaceFighterData[SpaceShipNum - 1].Armor;
}

/*
 * Constructor.
 */
cEarthSpaceFighter::cEarthSpaceFighter(const int SpaceShipNum)
{
	if ((SpaceShipNum <= 0) ||
	    (static_cast<unsigned>(SpaceShipNum) > PresetEarthSpaceFighterData.size())) {
		std::cerr << __func__ << "(): "
			  << "Could not init cEarthSpaceFighter object with Number "
			  << SpaceShipNum << "\n";
		return;
	}

	ObjectStatus = eObjectStatus::Ally;
	ObjectType = eObjectType::EarthFighter;
	InternalType = SpaceShipNum;
	PromptDrawDist2 = 100.0f;

	ArmorCurrentStatus = ArmorInitialStatus =
			PresetEarthSpaceFighterData[SpaceShipNum - 1].Armor; // GameEnemyArmorPenalty for enemies only
	WeaponSlots.resize(PresetEarthSpaceFighterData[SpaceShipNum - 1].WeaponQuantity);
	LoadObjectData(PresetEarthSpaceFighterData[SpaceShipNum - 1].Name, *this);

	Engines.resize(PresetEarthSpaceFighterData[SpaceShipNum - 1].EngineQuantity);
	EnginesLocation.resize(Engines.size());

	EnginesLeftLocation.resize(1);
	EnginesLeft.resize(1);

	EnginesRightLocation.resize(1);
	EnginesRight.resize(1);

	switch (SpaceShipNum) {
	case 1:
		WeaponSlots[0].Type = 1;
		WeaponSlots[0].Location = sVECTOR3D{4.4f, -0.1f, -6.0f};
		WeaponSlots[1].Type = 1;
		WeaponSlots[1].Location = sVECTOR3D{-4.4f, -0.1f, -6.0f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{1.9f, -1.25f, 6.5f};
		WeaponSlots[3].Type = 1;
		WeaponSlots[3].Location = sVECTOR3D{-1.9f, -1.25f, 6.5f};
		WeaponSlots[4].Type = 4;
		WeaponSlots[4].Location = sVECTOR3D{0.0f, -1.20f, 1.0f};
		EnginesLocation[0] = sVECTOR3D{3.0f, -1.2f, -4.7f};
		EnginesLocation[1] = sVECTOR3D{-3.0f, -1.2f, -4.7f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, -0.7f, 3.7f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, -0.7f, 3.7f};
		break;

	case 2:
		WeaponSlots[0].Type = 1;
		WeaponSlots[0].Location = sVECTOR3D{3.45f, 1.03f, -8.5f};
		WeaponSlots[1].Type = 1;
		WeaponSlots[1].Location = sVECTOR3D{-3.45f, 1.03f, -8.5f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{1.7f, -1.03f, 3.9f};
		WeaponSlots[3].Type = 1;
		WeaponSlots[3].Location = sVECTOR3D{-1.7f, -1.03f, 3.9f};
		EnginesLocation[0] = sVECTOR3D{4.9f, -1.1f, -8.9f};
		EnginesLocation[1] = sVECTOR3D{-4.9f, -1.1f, -8.9f};
		EnginesLocation[2] = sVECTOR3D{0.75f, 0.7f, -9.6f};
		EnginesLocation[3] = sVECTOR3D{-0.75f, 0.7f, -9.6f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, -0.7f, 5.7f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, -0.7f, 5.7f};
		break;

	case 3:
		WeaponSlots[0].Type = 5;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, -1.75f, 3.5f};
		WeaponSlots[1].Type = 1;
		WeaponSlots[1].Location = sVECTOR3D{7.15f, -0.38f, -4.4f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{-7.15f, -0.38f, -4.4f};
		EnginesLocation[0] = sVECTOR3D{4.4f, -3.0f, -9.7f};
		EnginesLocation[1] = sVECTOR3D{-4.4f, -3.0f, -9.7f};
		EnginesLocation[2] = sVECTOR3D{4.4f, 3.0f, -9.7f};
		EnginesLocation[3] = sVECTOR3D{-4.4f, 3.0f, -9.7f};
		EnginesLeftLocation[0] = sVECTOR3D{0.8f, -1.6f, 8.7f};
		EnginesRightLocation[0] = sVECTOR3D{-0.8f, -1.6f, 8.7f};
		break;

	case 4:
		WeaponSlots[0].Type = 1;
		WeaponSlots[0].Location = sVECTOR3D{4.10f, 1.4f, -2.5f};
		WeaponSlots[1].Type = 1;
		WeaponSlots[1].Location = sVECTOR3D{-4.10f, 1.4f, -2.5f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{5.10f, -1.2f, -2.5f};
		WeaponSlots[3].Type = 1;
		WeaponSlots[3].Location = sVECTOR3D{-5.10f, -1.2f, -2.5f};
		WeaponSlots[4].Type = 4;
		WeaponSlots[4].Location = sVECTOR3D{0.0f, -1.5f, 5.3f};
		EnginesLocation[0] = sVECTOR3D{0.8f, 0.1f, -9.6f};
		EnginesLocation[1] = sVECTOR3D{-0.8f, 0.1f, -9.6f};
		EnginesLeftLocation[0] = sVECTOR3D{1.6f, -0.9f, 5.7f};
		EnginesRightLocation[0] = sVECTOR3D{-1.6f, -0.9f, 5.7f};
		break;

	case 5:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{4.0f, -0.5f, 1.0f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-4.0f, -0.5f, 1.0f};
		EnginesLocation[0] = sVECTOR3D{6.72f, -0.28f, -7.98f};
		EnginesLocation[1] = sVECTOR3D{-6.72f, -0.28f, -7.98f};
		EnginesLocation[2] = sVECTOR3D{6.72f, -1.96f, -7.28f};
		EnginesLocation[3] = sVECTOR3D{-6.72f, -1.96f, -7.28f};
		EnginesLeftLocation[0] = sVECTOR3D{1.4f, 0.0f, 3.92f};
		EnginesRightLocation[0] = sVECTOR3D{-1.4f, 0.0f, 3.92f};
		break;

	case 6:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{2.80f, 0.25f, -7.0f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-2.80f, 0.25f, -7.0f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{5.20f, 0.6f, -6.0f};
		WeaponSlots[3].Type = 1;
		WeaponSlots[3].Location = sVECTOR3D{-5.20f, 0.6f, -6.0f};
		WeaponSlots[4].Type = 4;
		WeaponSlots[4].Location = sVECTOR3D{0.0f, -1.3f, 2.6f};
		EnginesLocation[0] = sVECTOR3D{0.6f, 0.45f, -9.9f};
		EnginesLocation[1] = sVECTOR3D{-0.6f, 0.45f, -9.9f};
		EnginesLeftLocation[0] = sVECTOR3D{1.6f, -0.9f, 4.7f};
		EnginesRightLocation[0] = sVECTOR3D{-1.6f, -0.9f, 4.7f};
		break;

	case 7:
		WeaponSlots[0].Type = 5;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, -0.2f, 0.0f};
		WeaponSlots[1].Type = 1;
		WeaponSlots[1].Location = sVECTOR3D{4.3f, -1.25f, -1.4f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{-4.3f, -1.25f, -1.4f};
		WeaponSlots[3].Type = 1;
		WeaponSlots[3].Location = sVECTOR3D{8.2f, -0.75f, -3.6f};
		WeaponSlots[4].Type = 1;
		WeaponSlots[4].Location = sVECTOR3D{-8.2f, -0.75f, -3.6f};
		EnginesLocation[0] = sVECTOR3D{3.75f, -0.5f, -10.9f};
		EnginesLocation[1] = sVECTOR3D{0.0f, 0.4f, -8.3f};
		EnginesLocation[2] = sVECTOR3D{-3.75f, -0.5f, -10.9f};
		EnginesLeftLocation[0] = sVECTOR3D{1.4f, 0.1f, 1.1f};
		EnginesRightLocation[0] = sVECTOR3D{-1.4f, 0.1f, 1.1f};
		break;

	case 8:
		WeaponSlots[0].Type = 1;
		WeaponSlots[0].Location = sVECTOR3D{1.0f, -0.3f, 0.4f};
		WeaponSlots[1].Type = 1;
		WeaponSlots[1].Location = sVECTOR3D{-1.0f, -0.3f, 0.4f};
		EnginesLocation[0] = sVECTOR3D{3.5f, 1.4f, -7.4f};
		EnginesLocation[1] = sVECTOR3D{-3.5f, 1.4f, -7.4f};
		EnginesLocation[2] = sVECTOR3D{3.65f, 0.5f, -7.9f};
		EnginesLocation[3] = sVECTOR3D{-3.65f, 0.5f, -7.9f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, 0.0f, 2.4f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, 0.0f, 2.4f};
		break;

	case 9:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{1.2f, 0.2f, 1.6f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-1.2f, 0.2f, 1.6f};
		EnginesLocation[0] = sVECTOR3D{3.30f, -1.20f, -0.7f};
		EnginesLocation[1] = sVECTOR3D{-3.30f, -1.20f, -0.7f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, 0.2f, 2.2f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, 0.2f, 2.2f};
		break;

	case 10:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{5.05f, -1.8f, -1.7f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-5.05f, -1.8f, -1.7f};
		WeaponSlots[2].Type = 2;
		WeaponSlots[2].Location = sVECTOR3D{2.55f, -0.7f, -0.7f};
		WeaponSlots[3].Type = 2;
		WeaponSlots[3].Location = sVECTOR3D{-2.55f, -0.7f, -0.7f};
		WeaponSlots[4].Type = 4;
		WeaponSlots[4].Location = sVECTOR3D{0.0f, -0.9f, 6.1f};
		EnginesLocation[0] = sVECTOR3D{0.8f, 0.55f, -9.9f};
		EnginesLocation[1] = sVECTOR3D{-0.8f, 0.55f, -9.9f};
		EnginesLeftLocation[0] = sVECTOR3D{1.6f, -0.4f, 5.3f};
		EnginesRightLocation[0] = sVECTOR3D{-1.6f, -0.4f, 5.3f};
		break;

	case 11:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{2.0f, -0.55f, -2.2f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-2.0f, -0.55f, -2.2f};
		WeaponSlots[2].Type = 1;
		WeaponSlots[2].Location = sVECTOR3D{0.0f, -0.7f, 2.2f};
		EnginesLocation[0] = sVECTOR3D{4.35f, -0.35f, -4.4f};
		EnginesLocation[1] = sVECTOR3D{-4.35f, -0.35f, -4.4f};
		EnginesLeftLocation[0] = sVECTOR3D{1.5f, -0.6f, 2.0f};
		EnginesRightLocation[0] = sVECTOR3D{-1.5f, -0.6f, 2.0f};
		break;

	case 12:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{2.25f, -0.85f, 3.3f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-2.25f, -0.85f, 3.3f};
		WeaponSlots[2].Type = 4;
		WeaponSlots[2].Location = sVECTOR3D{0.0f, -0.35f, 2.8f};
		EnginesLocation[0] = sVECTOR3D{4.8f, -1.2f, -2.8f};
		EnginesLocation[1] = sVECTOR3D{-4.8f, -1.2f, -2.8f};
		EnginesLeftLocation[0] = sVECTOR3D{1.2f, -0.2f, 6.3f};
		EnginesRightLocation[0] = sVECTOR3D{-1.2f, -0.2f, 6.3f};
		break;

	case 13:
		WeaponSlots[0].Type = 5;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, -2.15f, -4.4f};
		WeaponSlots[1].Type = 4;
		WeaponSlots[1].Location = sVECTOR3D{7.2f, 1.85f, -5.6f};
		WeaponSlots[2].Type = 4;
		WeaponSlots[2].Location = sVECTOR3D{-7.2f, 1.85f, -5.6f};
		WeaponSlots[3].Type = 1;
		WeaponSlots[3].Location = sVECTOR3D{3.05f, -1.95f, -0.4f};
		WeaponSlots[4].Type = 1;
		WeaponSlots[4].Location = sVECTOR3D{-3.05f, -1.95f, -0.4f};
		EnginesLocation[0] = sVECTOR3D{2.2f, -1.35f, -9.8f};
		EnginesLocation[1] = sVECTOR3D{-2.2f, -1.35f, -9.8f};
		EnginesLocation[2] = sVECTOR3D{1.1f, -1.0f, -9.8f};
		EnginesLocation[3] = sVECTOR3D{-1.1f, -1.0f, -9.8f};
		EnginesLeftLocation[0] = sVECTOR3D{1.4f, -0.1f, 0.5f};
		EnginesRightLocation[0] = sVECTOR3D{-1.4f, -0.1f, 0.5f};
		break;

	case 14:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{1.70f, -1.1f, 5.5f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-1.70f, -1.1f, 5.5f};
		WeaponSlots[2].Type = 2;
		WeaponSlots[2].Location = sVECTOR3D{4.1f, 0.2f, -6.3f};
		WeaponSlots[3].Type = 2;
		WeaponSlots[3].Location = sVECTOR3D{-4.1f, 0.2f, -6.3f};
		WeaponSlots[4].Type = 4;
		WeaponSlots[4].Location = sVECTOR3D{0.0f, -0.9f, 3.3f};
		EnginesLocation[0] = sVECTOR3D{0.85f, 0.1f, -8.1f};
		EnginesLocation[1] = sVECTOR3D{-0.85f, 0.1f, -8.1f};
		EnginesLeftLocation[0] = sVECTOR3D{1.3f, -0.7f, 6.3f};
		EnginesRightLocation[0] = sVECTOR3D{-1.3f, -0.7f, 6.3f};
		break;

	case 15:
		WeaponSlots[0].Type = 4;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, -0.7f, 1.3f};
		WeaponSlots[1].Type = 3;
		WeaponSlots[1].Location = sVECTOR3D{7.1f, -0.2f, -5.8f};
		WeaponSlots[2].Type = 3;
		WeaponSlots[2].Location = sVECTOR3D{-7.1f, -0.2f, -5.8f};
		EnginesLocation[0] = sVECTOR3D{4.75f, -0.5f, -7.2f};
		EnginesLocation[1] = sVECTOR3D{-4.75f, -0.5f, -7.2f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, -0.6f, 6.25f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, -0.6f, 6.25f};
		break;

	case 16:
		WeaponSlots[0].Type = 3;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, -1.2f, 7.7f};
		WeaponSlots[1].Type = 4;
		WeaponSlots[1].Location = sVECTOR3D{2.0f, -1.05f, 0.4f};
		WeaponSlots[2].Type = 4;
		WeaponSlots[2].Location = sVECTOR3D{-2.0f, -1.05f, 0.4f};
		EnginesLocation[0] = sVECTOR3D{3.8f, -0.75f, -11.4f};
		EnginesLocation[1] = sVECTOR3D{-3.8f, -0.75f, -11.4f};
		EnginesLocation[2] = sVECTOR3D{3.5f, 0.85f, -10.85f};
		EnginesLocation[3] = sVECTOR3D{-3.5f, 0.85f, -10.85f};
		EnginesLeftLocation[0] = sVECTOR3D{1.6f, -0.6f, 6.25f};
		EnginesRightLocation[0] = sVECTOR3D{-1.6f, -0.6f, 6.25f};
		break;

	case 17:
		WeaponSlots[0].Type = 3;
		WeaponSlots[0].Location = sVECTOR3D{3.10f, 0.2f, -4.6f};
		WeaponSlots[1].Type = 3;
		WeaponSlots[1].Location = sVECTOR3D{-3.10f, 0.2f, -4.6f};
		WeaponSlots[2].Type = 2;
		WeaponSlots[2].Location = sVECTOR3D{5.85f, -0.5f, -5.6f};
		WeaponSlots[3].Type = 2;
		WeaponSlots[3].Location = sVECTOR3D{-5.85f, -0.5f, -5.6f};
		WeaponSlots[4].Type = 4;
		WeaponSlots[4].Location = sVECTOR3D{0.0f, -0.5f, -0.5f};
		EnginesLocation[0] = sVECTOR3D{0.65f, 0.2f, -8.2f};
		EnginesLocation[1] = sVECTOR3D{-0.65f, 0.2f, -8.2f};
		EnginesLeftLocation[0] = sVECTOR3D{1.4f, -0.5f, 6.25f};
		EnginesRightLocation[0] = sVECTOR3D{-1.4f, -0.5f, 6.25f};
		break;

	case 18:
		WeaponSlots[0].Type = 5;
		WeaponSlots[0].Location = sVECTOR3D{1.7f, -2.5f, -4.4f};
		WeaponSlots[1].Type = 5;
		WeaponSlots[1].Location = sVECTOR3D{-1.7f, -2.5f, -4.4f};
		WeaponSlots[2].Type = 3;
		WeaponSlots[2].Location = sVECTOR3D{5.1f, -1.0f, -5.4f};
		WeaponSlots[3].Type = 3;
		WeaponSlots[3].Location = sVECTOR3D{-5.1f, -1.0f, -5.4f};
		EnginesLocation[0] = sVECTOR3D{7.5f, 0.15f, -9.1f};
		EnginesLocation[1] = sVECTOR3D{-7.5f, 0.15f, -9.1f};
		EnginesLocation[2] = sVECTOR3D{7.7f, -1.15f, -9.6f};
		EnginesLocation[3] = sVECTOR3D{-7.7f, -1.15f, -9.6f};
		EnginesLeftLocation[0] = sVECTOR3D{1.9f, -1.6f, 6.25f};
		EnginesRightLocation[0] = sVECTOR3D{-1.9f, -1.6f, 6.25f};
		break;

	case 19:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, -0.75f, 0.0f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{4.7f, -0.4f, -5.0f};
		WeaponSlots[2].Type = 2;
		WeaponSlots[2].Location = sVECTOR3D{-4.7f, -0.4f, -5.0f};
		EnginesLocation[0] = sVECTOR3D{6.3f, 0.60f, -8.1f};
		EnginesLocation[1] = sVECTOR3D{-6.3f, 0.60f, -8.1f};
		EnginesLocation[2] = sVECTOR3D{6.7f, -0.60f, -8.8f};
		EnginesLocation[3] = sVECTOR3D{-6.7f, -0.60f, -8.8f};
		EnginesLeftLocation[0] = sVECTOR3D{1.45f, 0.05f, 5.0f};
		EnginesRightLocation[0] = sVECTOR3D{-1.45f, 0.05f, 5.0f};
		break;

	case 20:
		WeaponSlots[0].Type = 3;
		WeaponSlots[0].Location = sVECTOR3D{0.80f, -0.8f, -2.3f};
		WeaponSlots[1].Type = 3;
		WeaponSlots[1].Location = sVECTOR3D{-0.80f, -0.8f, -2.3f};
		WeaponSlots[2].Type = 4;
		WeaponSlots[2].Location = sVECTOR3D{3.9f, -1.25f, -3.2f};
		WeaponSlots[3].Type = 4;
		WeaponSlots[3].Location = sVECTOR3D{-3.9f, -1.25f, -3.2f};
		EnginesLocation[0] = sVECTOR3D{3.45f, -0.3f, -7.3f};
		EnginesLocation[1] = sVECTOR3D{-3.45f, -0.3f, -7.3f};
		EnginesLocation[2] = sVECTOR3D{4.55f, -0.5f, -7.0f};
		EnginesLocation[3] = sVECTOR3D{-4.55f, -0.5f, -7.0f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, -0.4f, 1.45f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, -0.4f, 1.45f};
		break;

	case 21:
		WeaponSlots[0].Type = 2;
		WeaponSlots[0].Location = sVECTOR3D{2.05f, -0.22f, -1.2f};
		WeaponSlots[1].Type = 2;
		WeaponSlots[1].Location = sVECTOR3D{-2.05f, -0.22f, -1.2f};
		WeaponSlots[2].Type = 2;
		WeaponSlots[2].Location = sVECTOR3D{0.0f, -0.5f, 0.5f};
		EnginesLocation[0] = sVECTOR3D{0.7f, 0.0f, -4.4f};
		EnginesLocation[1] = sVECTOR3D{-0.7f, 0.0f, -4.4f};
		EnginesLeftLocation[0] = sVECTOR3D{1.0f, -0.4f, 3.0f};
		EnginesRightLocation[0] = sVECTOR3D{-1.0f, -0.4f, 3.0f};
		break;

	case 22:
		WeaponSlots[0].Type = 3;
		WeaponSlots[0].Location = sVECTOR3D{0.0f, 0.05f, 3.6f};
		EnginesLocation[0] = sVECTOR3D{0.0f, 0.3f, -5.2f};
		EnginesLeftLocation[0] = sVECTOR3D{1.9f, -0.3f, 2.6f};
		EnginesRightLocation[0] = sVECTOR3D{-1.9f, -0.3f, 2.6f};
		break;
	}
}

} // astromenace namespace
} // viewizard namespace
