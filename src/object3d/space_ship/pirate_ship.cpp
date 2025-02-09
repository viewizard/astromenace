/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


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


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

// TODO add more flare weapons for big pirate ships

#include "space_ship.h"
#include "../../config/config.h"
#include "../../assets/audio.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

enum class eEngineGFX {
    SmallEngine,
    MidEngine,
    BigEngine,
    VerticalPrimaryEngine,
    VerticalAuxiliaryEngine
};

struct sPirateShipData {
    unsigned int EngineQuantity;
    float Armor;
    unsigned NameHash;
    unsigned TextureNameHash;
    bool HaveNormalMapNameHash;
    unsigned NormalMapNameHash;
};

const std::vector<sPirateShipData> PresetPirateShipData{
    {2,     20,     constexpr_hash_djb2a("models/pirateship/gunship-01.vw3d"),    constexpr_hash_djb2a("models/gr-04.vw2d"), false, 0},
    {2,     30,     constexpr_hash_djb2a("models/pirateship/bomber-07.vw3d"),     constexpr_hash_djb2a("models/gr-04.vw2d"), false, 0},
    {2,     100,    constexpr_hash_djb2a("models/pirateship/gunship-03.vw3d"),    constexpr_hash_djb2a("models/gr-03.vw2d"), false, 0},
    {2,     200,    constexpr_hash_djb2a("models/pirateship/gunship-04.vw3d"),    constexpr_hash_djb2a("models/gr-03.vw2d"), false, 0},
    {6,     400,    constexpr_hash_djb2a("models/pirateship/gunship-02.vw3d"),    constexpr_hash_djb2a("models/gr-03.vw2d"), false, 0},

    {2,     600,     constexpr_hash_djb2a("models/pirateship/bomber-03.vw3d"),    constexpr_hash_djb2a("models/gr-05.vw2d"), false, 0},
    {4,     1200,    constexpr_hash_djb2a("models/pirateship/bomber-02.vw3d"),    constexpr_hash_djb2a("models/gr-05.vw2d"), false, 0},
    {4,     1000,    constexpr_hash_djb2a("models/pirateship/bomber-04.vw3d"),    constexpr_hash_djb2a("models/gr-03.vw2d"), false, 0},
    {6,     2000,    constexpr_hash_djb2a("models/pirateship/bomber-05.vw3d"),    constexpr_hash_djb2a("models/gr-04.vw2d"), true, constexpr_hash_djb2a("models/normalmap/bomber_nm.tga")},
    {8,     2500,    constexpr_hash_djb2a("models/pirateship/bomber-06.vw3d"),    constexpr_hash_djb2a("models/gr-04.vw2d"), true, constexpr_hash_djb2a("models/normalmap/bomber_nm.tga")},

    {2,     300,     constexpr_hash_djb2a("models/pirateship/bomber-03.vw3d"),    constexpr_hash_djb2a("models/gr-05.vw2d"), false, 0},
    {4,     600,     constexpr_hash_djb2a("models/pirateship/bomber-02.vw3d"),    constexpr_hash_djb2a("models/gr-05.vw2d"), false, 0},
    {4,     500,     constexpr_hash_djb2a("models/pirateship/bomber-04.vw3d"),    constexpr_hash_djb2a("models/gr-03.vw2d"), false, 0},
    {6,     1000,    constexpr_hash_djb2a("models/pirateship/bomber-05.vw3d"),    constexpr_hash_djb2a("models/gr-04.vw2d"), true, constexpr_hash_djb2a("models/normalmap/bomber_nm.tga")},
    {8,     1500,    constexpr_hash_djb2a("models/pirateship/bomber-06.vw3d"),    constexpr_hash_djb2a("models/gr-04.vw2d"), true, constexpr_hash_djb2a("models/normalmap/bomber_nm.tga")}
};

} // unnamed namespace


/*
 * Setup engine gfx.
 */
static void SetupEngineGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, const eEngineGFX EngineType)
{
    constexpr unsigned tmpHash = constexpr_hash_djb2a("gfx/flare1.tga");
    ParticleSystem->Texture = GetPreloadedTextureAsset(tmpHash);
    ParticleSystem->Direction(0.0f, 0.0f, -1.0f);

    static const sRGBCOLOR MuddyYellow{0.6f, 0.6f, 0.3f};
    static const sRGBCOLOR LightGreen{0.3f, 1.0f, 0.3f};

    switch (EngineType) {
    case eEngineGFX::SmallEngine:
        ParticleSystem->ColorStart = MuddyYellow;
        ParticleSystem->ColorEnd = LightGreen;
        ParticleSystem->AlphaStart = 1.00f;
        ParticleSystem->AlphaEnd = 0.00f;
        ParticleSystem->SizeStart = 0.10f;
        ParticleSystem->SizeVar = 0.20f;
        ParticleSystem->SizeEnd = 0.40f;
        ParticleSystem->Speed = 6.00f;
        ParticleSystem->SpeedOnCreation = -1.00f;
        ParticleSystem->SpeedVar = 2.00f;
        ParticleSystem->Theta = 5.00f;
        ParticleSystem->Life = 0.40f;
        ParticleSystem->ParticlesPerSec = 100;
        ParticleSystem->CreationType = eParticleCreationType::Sphere;
        ParticleSystem->CreationSize(0.4f, 0.4f, 0.4f);
        ParticleSystem->AlphaShowHide = true;
        ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
        ParticleSystem->LightNeedDeviation = true;
        break;

    case eEngineGFX::MidEngine:
        ParticleSystem->ColorStart = MuddyYellow;
        ParticleSystem->ColorEnd = LightGreen;
        ParticleSystem->AlphaStart = 1.00f;
        ParticleSystem->AlphaEnd = 0.00f;
        ParticleSystem->SizeStart = 0.30f;
        ParticleSystem->SizeVar = 0.30f;
        ParticleSystem->SizeEnd = 0.60f;
        ParticleSystem->Speed = 8.00f;
        ParticleSystem->SpeedOnCreation = -1.00f;
        ParticleSystem->SpeedVar = 2.00f;
        ParticleSystem->Theta = 5.00f;
        ParticleSystem->Life = 0.40f;
        ParticleSystem->ParticlesPerSec = 100;
        ParticleSystem->CreationType = eParticleCreationType::Sphere;
        ParticleSystem->CreationSize(0.6f, 0.6f, 0.1f);
        ParticleSystem->AlphaShowHide = true;
        ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
        ParticleSystem->LightNeedDeviation = true;
        break;

    case eEngineGFX::BigEngine:
        ParticleSystem->ColorStart = MuddyYellow;
        ParticleSystem->ColorEnd = LightGreen;
        ParticleSystem->AlphaStart = 1.00f;
        ParticleSystem->AlphaEnd = 0.00f;
        ParticleSystem->SizeStart = 0.30f;
        ParticleSystem->SizeVar = 0.30f;
        ParticleSystem->SizeEnd = 0.60f;
        ParticleSystem->Speed = 8.00f;
        ParticleSystem->SpeedOnCreation = -1.00f;
        ParticleSystem->SpeedVar = 2.00f;
        ParticleSystem->Theta = 5.00f;
        ParticleSystem->Life = 0.40f;
        ParticleSystem->ParticlesPerSec = 100;
        ParticleSystem->CreationType = eParticleCreationType::Sphere;
        ParticleSystem->CreationSize(1.0f, 1.0f, 1.0f);
        ParticleSystem->AlphaShowHide = true;
        ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.05f);
        ParticleSystem->LightNeedDeviation = true;
        break;

    case eEngineGFX::VerticalPrimaryEngine:
        ParticleSystem->ColorStart = MuddyYellow;
        ParticleSystem->ColorEnd = LightGreen;
        ParticleSystem->AlphaStart = 1.00f;
        ParticleSystem->AlphaEnd = 0.00f;
        ParticleSystem->SizeStart = 0.30f;
        ParticleSystem->SizeVar = 0.30f;
        ParticleSystem->SizeEnd = 0.60f;
        ParticleSystem->Speed = 10.00f;
        ParticleSystem->SpeedOnCreation = -1.00f;
        ParticleSystem->SpeedVar = 2.00f;
        ParticleSystem->Theta = 5.00f;
        ParticleSystem->Life = 0.50f;
        ParticleSystem->ParticlesPerSec = 100;
        ParticleSystem->CreationType = eParticleCreationType::Sphere;
        ParticleSystem->CreationSize(1.0f, 0.1f, 1.0f);
        ParticleSystem->AlphaShowHide = true;
        ParticleSystem->Direction(0.0f, -1.0f, 0.0f);
        ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.05f);
        ParticleSystem->LightNeedDeviation = true;
        break;

    case eEngineGFX::VerticalAuxiliaryEngine:
        ParticleSystem->ColorStart = MuddyYellow;
        ParticleSystem->ColorEnd = LightGreen;
        ParticleSystem->AlphaStart = 1.00f;
        ParticleSystem->AlphaEnd = 0.00f;
        ParticleSystem->SizeStart = 0.30f;
        ParticleSystem->SizeVar = 0.20f;
        ParticleSystem->SizeEnd = 0.10f;
        ParticleSystem->Speed = 3.00f;
        ParticleSystem->SpeedOnCreation = -1.00f;
        ParticleSystem->SpeedVar = 2.00f;
        ParticleSystem->Theta = 180.00f;
        ParticleSystem->Life = 0.50f;
        ParticleSystem->ParticlesPerSec = 100;
        ParticleSystem->AlphaShowHide = true;
        ParticleSystem->Direction(0.0f, 1.0f, 0.0f);
        ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D{0.0f, 0.0f, 0.0f}, 0.45f, 0.8f, 0.3f, 0.0f, 0.1f);
        ParticleSystem->LightNeedDeviation = true;
        break;
    }
}

/*
 * Constructor.
 */
cPirateShip::cPirateShip(const int SpaceShipNum)
{
    if (SpaceShipNum <= 0 || static_cast<unsigned>(SpaceShipNum) > PresetPirateShipData.size()) {
        std::cerr << __func__ << "(): "
                  << "Could not init cPirateShip object with Number "
                  << SpaceShipNum << "\n";
        return;
    }

    ObjectStatus = eObjectStatus::Enemy;
    ObjectType = eObjectType::PirateShip;
    InternalType = SpaceShipNum;
    PromptDrawDist2 = 100.0f;
    if (SpaceShipNum > 5) { // big pirate ships
        PromptDrawDist2 = 20000.0f;
    }

    MaxSpeed = 20.0f;
    MaxAcceler = 20.0f;
    MaxSpeedRotate = 40.0f;
    ArmorCurrentStatus = ArmorInitialStatus = PresetPirateShipData[SpaceShipNum - 1].Armor / GameEnemyArmorPenalty;
    unsigned int EngineQuantity = PresetPirateShipData[SpaceShipNum - 1].EngineQuantity;

    LoadObjectData(PresetPirateShipData[SpaceShipNum - 1].NameHash, *this);

    for (unsigned int i = 0; i < Chunks.size(); i++) {
        Texture[i] = GetPreloadedTextureAsset(PresetPirateShipData[SpaceShipNum - 1].TextureNameHash);
        if (PresetPirateShipData[SpaceShipNum - 1].HaveNormalMapNameHash && GameConfig().UseGLSL120) {
            NormalMap[i] = GetPreloadedTextureAsset(PresetPirateShipData[SpaceShipNum - 1].NormalMapNameHash);
        }
    }

    EnginesLocation.resize(EngineQuantity);
    Engines.resize(EngineQuantity);

    switch (SpaceShipNum) {
    case 1:
        MaxSpeed = 30.0f;
        MaxAcceler = 30.0f;

        WeaponSlots.reserve(2);
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{1.8f, -1.2f, 6.0f});
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-1.8f, -1.2f, 6.0f});
        if (auto sharedWeapon = WeaponSlots.back().Weapon.lock()) {
            sharedWeapon->SFX = eGameSFX::none;
        }
        WeaponFireType = 1;

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{0.9f, 1.6f, -5.4f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::SmallEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-0.9f, 1.6f, -5.4f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::SmallEngine);
        }
        break;

    case 2:
        MaxSpeed = 50.0f;
        MaxAcceler = 50.0f;

        WeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{0.0f, -2.0f, 2.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{1.1f, 0.5f, -5.7f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.1f, 0.5f, -5.7f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::SmallEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.0f, 1.8f, -1.2f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::SmallEngine);
        }
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.3f, 1.9f, -1.7f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::VerticalPrimaryEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-5.8f, 6.5f - 4.6f, 0.0f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::VerticalPrimaryEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{5.8f, 7.3f - 4.6f, 0.0f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::VerticalAuxiliaryEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-5.8f, 7.3f - 4.6f, 0.0f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::VerticalAuxiliaryEngine);
        }
        Engines[4] = vw_CreateParticleSystem();
        EnginesLocation[4] = sVECTOR3D{5.8f, 6.1f - 4.6f, -2.3f};
        if (auto sharedEngine = Engines[4].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::SmallEngine);
            sharedEngine->Direction = sVECTOR3D{0.0f, -0.2f, -0.8f};
        }
        Engines[5] = vw_CreateParticleSystem();
        EnginesLocation[5] = sVECTOR3D{-5.8f, 6.1f - 4.6f, -2.3f};
        if (auto sharedEngine = Engines[5].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::SmallEngine);
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.7f, 6.6f - 3.83f, -14.2f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        break;

    case 7:
        WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 2.4f, -0.2f});

        BossWeaponSlots.reserve(2);
        BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{9.5f, 1.3f, 2.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{-9.5f, 1.3f, 2.0f});

        FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{2.2f, 7.6f - 5.25f, -14.8f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-2.2f, 7.6f - 5.25f, -14.8f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{17.2f, 6.8f - 5.25f, -9.0f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-17.2f, 6.8f - 5.25f, -9.0f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-4.5f, 6.3f - 3.62f, -12.5f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{9.2f, 4.4f - 3.62f, -5.6f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-9.2f, 4.4f - 3.62f, -5.6f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
        break;

    case 9:
        WeaponSlots.reserve(3);
        WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.2f, 7.0f});
        WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.2f, 3.5f});
        if (auto sharedWeapon = WeaponSlots.back().Weapon.lock()) {
            sharedWeapon->SetRotation(sVECTOR3D{0.0, 180.0, 0.0});
        }
        WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 3.4f, -5.0f});

        BossWeaponSlots.reserve(2);
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 9.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 9.0f});

        FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{1.4f, 8.7f - 4.9f, -20.4f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.4f, 8.7f - 4.9f, -20.4f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{4.7f, 7.0f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-4.7f, 7.0f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[4] = vw_CreateParticleSystem();
        EnginesLocation[4] = sVECTOR3D{7.5f, 6.8f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[4].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[5] = vw_CreateParticleSystem();
        EnginesLocation[5] = sVECTOR3D{-7.5f, 6.8f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[5].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        break;

    case 10:
        WeaponSlots.reserve(4);
        WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.4f, 6.8f});
        WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 4.4f, 2.6f});
        if (auto sharedWeapon = WeaponSlots.back().Weapon.lock()) {
            sharedWeapon->SetRotation(sVECTOR3D{0.0f, 180.0f, 0.0f});
        }
        WeaponSlots.emplace_back(CreateWeapon(202), sVECTOR3D{0.0f, 2.6f, -7.0f});
        WeaponSlots.emplace_back(CreateWeapon(201), sVECTOR3D{0.0f, 2.6f, -16.0f});
        if (auto sharedWeapon = WeaponSlots.back().Weapon.lock()) {
            sharedWeapon->SetRotation(sVECTOR3D{0.0f, 180.0f, 0.0f});
        }

        BossWeaponSlots.reserve(4);
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 8.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 8.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{16.5f, 2.7f, 7.5f});
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-16.5f, 2.7f, 7.5f});

        FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{3.8f, 5.4f - 4.29f, -19.5f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-3.8f, 5.4f - 4.29f, -19.5f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{5.8f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-5.8f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[4] = vw_CreateParticleSystem();
        EnginesLocation[4] = sVECTOR3D{8.0f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[4].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[5] = vw_CreateParticleSystem();
        EnginesLocation[5] = sVECTOR3D{-8.0f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[5].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[6] = vw_CreateParticleSystem();
        EnginesLocation[6] = sVECTOR3D{20.0f, 5.0f - 4.29f, -3.6f};
        if (auto sharedEngine = Engines[6].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[7] = vw_CreateParticleSystem();
        EnginesLocation[7] = sVECTOR3D{-20.0f, 5.0f - 4.29f, -3.6f};
        if (auto sharedEngine = Engines[7].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        break;

    case 11:
        BossWeaponSlots.reserve(2);
        BossWeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{5.4f, 1.0f, 2.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(209), sVECTOR3D{-5.4f, 1.0f, 2.0f});

        FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{1.7f, 6.6f - 3.83f, -14.2f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.7f, 6.6f - 3.83f, -14.2f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        break;

    case 12:
        BossWeaponSlots.reserve(2);
        BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{9.5f, 1.3f, 2.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{-9.5f, 1.3f, 2.0f});

        FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 5.0f, 0.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{2.2f, 7.6f - 5.25f, -14.8f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-2.2f, 7.6f - 5.25f, -14.8f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{17.2f, 6.8f - 5.25f, -9.0f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-17.2f, 6.8f - 5.25f, -9.0f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-4.5f, 6.3f - 3.62f, -12.5f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{9.2f, 4.4f - 3.62f, -5.6f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-9.2f, 4.4f - 3.62f, -5.6f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::BigEngine);
        }
        break;

    case 14:
        BossWeaponSlots.reserve(2);
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{11.1f, 2.7f, 9.0f});
        BossWeaponSlots.emplace_back(CreateWeapon(205), sVECTOR3D{-11.1f, 2.7f, 9.0f});

        FlareWeaponSlots.emplace_back(CreateWeapon(203), sVECTOR3D{0.0f, 7.0f, 0.0f});

        Engines[0] = vw_CreateParticleSystem();
        EnginesLocation[0] = sVECTOR3D{1.4f, 8.7f - 4.9f, -20.4f};
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-1.4f, 8.7f - 4.9f, -20.4f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{4.7f, 7.0f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-4.7f, 7.0f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[4] = vw_CreateParticleSystem();
        EnginesLocation[4] = sVECTOR3D{7.5f, 6.8f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[4].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[5] = vw_CreateParticleSystem();
        EnginesLocation[5] = sVECTOR3D{-7.5f, 6.8f - 4.9f, 0.0f};
        if (auto sharedEngine = Engines[5].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
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
        if (auto sharedEngine = Engines[0].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[1] = vw_CreateParticleSystem();
        EnginesLocation[1] = sVECTOR3D{-3.8f, 5.4f - 4.29f, -19.5f};
        if (auto sharedEngine = Engines[1].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[2] = vw_CreateParticleSystem();
        EnginesLocation[2] = sVECTOR3D{5.8f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[2].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[3] = vw_CreateParticleSystem();
        EnginesLocation[3] = sVECTOR3D{-5.8f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[3].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[4] = vw_CreateParticleSystem();
        EnginesLocation[4] = sVECTOR3D{8.0f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[4].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[5] = vw_CreateParticleSystem();
        EnginesLocation[5] = sVECTOR3D{-8.0f, 6.0f - 4.29f, -0.9f};
        if (auto sharedEngine = Engines[5].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[6] = vw_CreateParticleSystem();
        EnginesLocation[6] = sVECTOR3D{20.0f, 5.0f - 4.29f, -3.6f};
        if (auto sharedEngine = Engines[6].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        Engines[7] = vw_CreateParticleSystem();
        EnginesLocation[7] = sVECTOR3D{-20.0f, 5.0f - 4.29f, -3.6f};
        if (auto sharedEngine = Engines[7].lock()) {
            SetupEngineGFX(sharedEngine, eEngineGFX::MidEngine);
        }
        break;
    }

    for (unsigned int i = 0; i < EngineQuantity; i++) {
        if (auto sharedEngine = Engines[i].lock()) {
            sharedEngine->SetStartLocation(EnginesLocation[i]);
            if (!sharedEngine->Light.expired()) {
                InternalLights++;
            }
        }
    }
}

} // astromenace namespace
} // viewizard namespace
