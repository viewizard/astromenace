/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

#include "ground_object.h"
#include "../../assets/texture.h"
#include "../weapon/weapon.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

struct sWheeledData {
    float Armor;
    float SpeedToWheelRotarySpeedFactor;
    unsigned Model3DFileNameHash;
    unsigned TextureFileNameHash;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
const std::vector<sWheeledData> PresetWheeledData {
    {120,   50.0f,    constexpr_hash_djb2a("models/wheeled/jeep-01.vw3d"), constexpr_hash_djb2a("models/gr-02.vw2d")},
    {70,    50.0f,    constexpr_hash_djb2a("models/wheeled/jeep-02.vw3d"), constexpr_hash_djb2a("models/gr-02.vw2d")},
    {100,   50.0f,    constexpr_hash_djb2a("models/wheeled/jeep-03.vw3d"), constexpr_hash_djb2a("models/gr-02.vw2d")},
    {100,   50.0f,    constexpr_hash_djb2a("models/wheeled/jeep-04.vw3d"), constexpr_hash_djb2a("models/gr-07.vw2d")},
    {150,   50.0f,    constexpr_hash_djb2a("models/wheeled/jeep-05.vw3d"), constexpr_hash_djb2a("models/gr-03.vw2d")},
    {250,   30.0f,    constexpr_hash_djb2a("models/wheeled/apc-02.vw3d"), constexpr_hash_djb2a("models/gr-03.vw2d")},
    {200,   30.0f,    constexpr_hash_djb2a("models/wheeled/apc-04.vw3d"), constexpr_hash_djb2a("models/gr-01.vw2d")},
    {400,   34.0f,    constexpr_hash_djb2a("models/wheeled/r-launcher-01.vw3d"), constexpr_hash_djb2a("models/gr-02.vw2d")}
};
#pragma GCC diagnostic pop


/*
 * Constructor.
 */
cWheeled::cWheeled(const int WheeledNum)
{
    if (WheeledNum <= 0 || static_cast<unsigned>(WheeledNum) > PresetWheeledData.size()) {
        std::cerr << __func__ << "(): "
                  << "Could not init cWheeled object with Number "
                  << WheeledNum << "\n";
        return;
    }

    ObjectType = eObjectType::PirateVehicle;
    InternalType = WheeledNum;
    MaxSpeed = 20.0f;
    MaxAcceler = 5.0f;
    MaxSpeedRotate = 20.0f;

    LoadObjectData(PresetWheeledData[WheeledNum - 1].Model3DFileNameHash, *this);

    for (unsigned int i = 0; i < Chunks.size(); i++) {
        Texture[i] = GetPreloadedTextureAsset(PresetWheeledData[WheeledNum - 1].TextureFileNameHash);
    }
    SpeedToWheelRotarySpeedFactor = PresetWheeledData[WheeledNum - 1].SpeedToWheelRotarySpeedFactor;

    ArmorCurrentStatus = ArmorInitialStatus = PresetWheeledData[WheeledNum - 1].Armor / GameEnemyArmorPenalty;

    switch (WheeledNum) {
    case 1:
        WeaponSlots.reserve(4);
        WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{0.3f, 4.5f, -1.5f});
        WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-0.3f, 4.5f, -1.5f});
        WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{0.9f, 4.5f, -1.5f});
        WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-0.9f, 4.5f, -1.5f});
        WeaponFireType = 3;

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(0);
        WheelChunkNums.emplace_back(2);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(3);
        SteerableWheelChunkNums.emplace_back(4);
        MaxSteerableWheelAngle = 30.0f;

        TargetVertChunkNums.emplace_back(5);
        TargetVertChunksMaxAngle = 30.0f;
        break;

    case 2:
        WeaponSlots.reserve(2);
        WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{1.3f, 3.5f, -1.5f});
        WeaponSlots.emplace_back(CreateWeapon(206), sVECTOR3D{-1.3f, 3.5f, -1.5f});
        WeaponFireType = 3;
        DoNotCalculateRotation = true;

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(2);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(1);
        SteerableWheelChunkNums.emplace_back(4);
        MaxSteerableWheelAngle = 20.0f;
        break;

    case 3:
        WeaponSlots.reserve(2);
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.8f, 4.2f, -1.0f});
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-0.8f, 4.2f, -1.0f});
        WeaponFireType = 2;

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(0);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(2);
        WheelChunkNums.emplace_back(5);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(0);
        SteerableWheelChunkNums.emplace_back(1);
        MaxSteerableWheelAngle = 20.0f;

        TargetHorizChunkNums.reserve(2);
        TargetHorizChunkNums.emplace_back(3);
        TargetHorizChunkNums.emplace_back(6);

        TargetVertChunkNums.emplace_back(6);
        TargetVertChunksMaxAngle = 60.0f;
        break;

    case 4:
        WeaponSlots.emplace_back(CreateWeapon(207), sVECTOR3D{0.0f, 4.5f, -4.0f});

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(0);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(0);
        SteerableWheelChunkNums.emplace_back(3);
        MaxSteerableWheelAngle = 20.0f;
        break;

    case 5:
        WeaponSlots.emplace_back(CreateWeapon(208), sVECTOR3D{0.0f, 4.5f, -4.0f});

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(2);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(1);
        SteerableWheelChunkNums.emplace_back(2);
        MaxSteerableWheelAngle = 30.0f;
        break;

    case 6:
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.0f, 6.0f, -3.0f});

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(2);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(1);
        SteerableWheelChunkNums.emplace_back(4);
        MaxSteerableWheelAngle = 30.0f;

        TargetHorizChunkNums.reserve(2);
        TargetHorizChunkNums.emplace_back(5);
        TargetHorizChunkNums.emplace_back(6);

        TargetVertChunkNums.emplace_back(6);
        TargetVertChunksMaxAngle = 40.0f;
        break;

    case 7:
        WeaponSlots.reserve(2);
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{0.1f, 5.0f, -1.0f});
        WeaponSlots.emplace_back(CreateWeapon(204), sVECTOR3D{-0.1f, 5.0f, -1.0f});
        WeaponFireType = 2;

        WheelChunkNums.reserve(4);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(2);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);

        SteerableWheelChunkNums.reserve(2);
        SteerableWheelChunkNums.emplace_back(1);
        SteerableWheelChunkNums.emplace_back(4);
        MaxSteerableWheelAngle = 20.0f;

        TargetHorizChunkNums.reserve(2);
        TargetHorizChunkNums.emplace_back(5);
        TargetHorizChunkNums.emplace_back(6);

        TargetVertChunkNums.emplace_back(6);
        TargetVertChunksMaxAngle = 40.0f;
        break;

    case 8:
        WeaponSlots.reserve(2);
        WeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{1.5f, 5.2f, 7.0f});
        WeaponSlots.emplace_back(CreateWeapon(210), sVECTOR3D{-1.5f, 5.2f, 7.0f});
        WeaponFireType = 3;

        WheelChunkNums.reserve(8);
        WheelChunkNums.emplace_back(1);
        WheelChunkNums.emplace_back(3);
        WheelChunkNums.emplace_back(4);
        WheelChunkNums.emplace_back(5);
        WheelChunkNums.emplace_back(6);
        WheelChunkNums.emplace_back(7);
        WheelChunkNums.emplace_back(8);
        WheelChunkNums.emplace_back(9);

        SteerableWheelChunkNums.reserve(4);
        SteerableWheelChunkNums.emplace_back(1);
        SteerableWheelChunkNums.emplace_back(3);
        SteerableWheelChunkNums.emplace_back(6);
        SteerableWheelChunkNums.emplace_back(7);
        MaxSteerableWheelAngle = 20.0f;

        TargetVertChunkNums.emplace_back(2);
        TargetVertChunksMaxAngle = 90.0f;
        break;
    }

    // randomize wheels initial rotation for better look
    for (const auto &WheelChunkNum : WheelChunkNums) {
        Chunks[WheelChunkNum].Rotation.x = vw_fRandNum(360.0f);
    }

    if (!TargetHorizChunkNums.empty()) {
        BaseBound = Chunks[TargetHorizChunkNums[0]].Location;
    }

    if (!TargetVertChunkNums.empty()) {
        if (!TargetHorizChunkNums.empty()) {
            MiddleBound = Chunks[TargetVertChunkNums[0]].Location - Chunks[TargetHorizChunkNums[0]].Location;
        } else {
            MiddleBound = Chunks[TargetVertChunkNums[0]].Location;
        }
    }

    for (auto &tmpWeaponSlot : WeaponSlots) {
        if (!TargetVertChunkNums.empty()) {
            tmpWeaponSlot.Bound = tmpWeaponSlot.Location - Chunks[TargetVertChunkNums[0]].Location;
        } else if (!TargetHorizChunkNums.empty()) {
            tmpWeaponSlot.Bound = tmpWeaponSlot.Location - Chunks[TargetHorizChunkNums[0]].Location;
        } else {
            tmpWeaponSlot.Bound = tmpWeaponSlot.Location;
        }
    }
}

} // astromenace namespace
} // viewizard namespace
