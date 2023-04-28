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

// FIXME ostringstream is not so fast, move all string initialization into setup,
//       all ostringstream-related code should be called only one time in init

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../gfx/shadow_map.h"
#include "../command.h"
#include "../object3d/object3d.h"
#include "../object3d/weapon/weapon.h"
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/ground_object/ground_object.h"
#include "../object3d/projectile/projectile.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include "SDL2/SDL.h"
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

std::weak_ptr<cSpaceShip> InfoShip{};
std::weak_ptr<cWeapon> InfoWeapon{};
std::weak_ptr<cProjectile> InfoMine{};
std::weak_ptr<cGroundObject> InfoGroundObject{};


const char *GetWeaponGroupTitle(int Num);
const char *GetWeaponName(int Num);
const char *GetShipGroupTitle(int Num);
const char *GetWorkshopShipName(int Num);




float LastRotateInfoObject = 0.0f;
sVECTOR3D PointCamera;
sVECTOR3D Point(1000,-1000,0);
sVECTOR3D ObjectBaseLocation(0,0,0);
int CreateNum = 1;
float RotationSumY =0.0f;
float InfoObjectWidth = 0.0f;
float InfoObjectLength = 0.0f;
float InfoObjectHeight = 0.0f;
float InfoObjectArmor = 0.0f;
int InfoObjectWeaponQuantity = 0;
int InfoObjectEngineQuantity = 0;

float InfoObjectEnergyUse = 0;
int InfoObjectAmmo = 0;
float InfoObjectReload = 0;
float InfoObjectRange = 0;







#define InfoFighterStart 1
#define InfoFighterQuant 22

#define InfoWeaponStart (InfoFighterStart+InfoFighterQuant)
#define InfoWeaponQuant 19

#define InfoMineStart (InfoWeaponStart+InfoWeaponQuant)
#define InfoMineQuant 4

#define InfoAlienStart (InfoMineStart+InfoMineQuant)
#define InfoAlienQuant 17

#define InfoAlienMotherShipStart (InfoAlienStart+InfoAlienQuant)
#define InfoAlienMotherShipQuant 8

#define InfoPirateShipStart (InfoAlienMotherShipStart+InfoAlienMotherShipQuant)
#define InfoPirateShipQuant 15

#define InfoBuildingStart (InfoPirateShipStart+InfoPirateShipQuant)
#define InfoBuildingQuant 11

#define InfoMilitaryBuildingStart (InfoBuildingStart+InfoBuildingQuant)
#define InfoMilitaryBuildingQuant 7

#define InfoWheeledStart (InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant)
#define InfoWheeledQuant 8

#define InfoTrackedStart (InfoWheeledStart+InfoWheeledQuant)
#define InfoTrackedQuant 14


#define InfoEnd 125








const char *InfoGroupNames[10] = {
    "Earth Spaceships",
    "Weapons",
    "Space Mines",
    "Alien Spaceships",
    "Alien Motherships",
    "Pirate Spaceships",
    "Civilian Buildings",
    "Military Buildings",
    "Wheeled Vehicles",
    "Tracked Vehicles"
};




int GetInfoNextGroup()
{
    if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
        return 2;
    }
    if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
        return 3;
    }
    if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
        return 4;
    }
    if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
        return 5;
    }
    if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
        return 6;
    }
    if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
        return 7;
    }
    if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
        return 8;
    }
    if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
        return 9;
    }
    if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
        return 10;
    }
    if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
        return 1;
    }

    return 1;
}
int GetInfoPrevGroup()
{
    if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
        return 10;
    }
    if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
        return 1;
    }
    if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
        return 2;
    }
    if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
        return 3;
    }
    if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
        return 4;
    }
    if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
        return 5;
    }
    if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
        return 6;
    }
    if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
        return 7;
    }
    if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
        return 8;
    }
    if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
        return 9;
    }

    return 1;
}


int GetInfoSwitchToGroup(int Group)
{
    switch (Group) {
    case 1:
        return InfoFighterStart;
    case 2:
        return InfoWeaponStart;
    case 3:
        return InfoMineStart;
    case 4:
        return InfoAlienStart;
    case 5:
        return InfoAlienMotherShipStart;
    case 6:
        return InfoPirateShipStart;
    case 7:
        return InfoBuildingStart;
    case 8:
        return InfoMilitaryBuildingStart;
    case 9:
        return InfoWheeledStart;
    case 10:
        return InfoTrackedStart;
    }

    return 1;
}











//------------------------------------------------------------------------------------
// Destroy current object
//------------------------------------------------------------------------------------
void DestroyInfoObject()
{
    ReleaseSpaceShip(InfoShip);
    ReleaseWeapon(InfoWeapon);
    ReleaseProjectile(InfoMine);
    ReleaseGroundObject(InfoGroundObject);
}




//------------------------------------------------------------------------------------
// Create current object
//------------------------------------------------------------------------------------
void CreateInfoObject()
{

    DestroyInfoObject();

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    RotationSumY = 140;

    Point = sVECTOR3D{1000.0f, -1000.0f, 0.0f};

    if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
        int tmpCreateNum = CreateNum-InfoFighterStart+1;
        InfoShip = CreateEarthSpaceFighter(tmpCreateNum);
        if (auto sharedShip = InfoShip.lock()) {
            sharedShip->ObjectStatus = eObjectStatus::none;
            sharedShip->EngineDestroyType = true;
            sharedShip->SetLocation(sVECTOR3D{1000.0f,
                                              -1000.0f - sharedShip->AABB[6].y,
                                              0.0f});
            ObjectBaseLocation = sharedShip->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f,
                              -1000.0f + sharedShip->Height / 3.0f,
                              0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.24f + sharedShip->Height * 0.3f,
                                    -(sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.56f - sharedShip->Height * 0.7f};
            InfoObjectWidth = sharedShip->Width;
            InfoObjectLength = sharedShip->Length;
            InfoObjectHeight = sharedShip->Height;
            InfoObjectArmor = sharedShip->ArmorInitialStatus;
            InfoObjectWeaponQuantity = sharedShip->WeaponSlots.size();
            InfoObjectEngineQuantity = sharedShip->Engines.size();

            for (auto &tmpEngine : sharedShip->Engines) {
                if (auto sharedEngine = tmpEngine.lock()) {
                    sharedEngine->SpeedOnCreation = -1.0f;
                }
            }

            sharedShip->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
        int tmpCreateNum = CreateNum-InfoWeaponStart+1;
        InfoWeapon = CreateWeapon(tmpCreateNum);
        if (auto sharedWeapon = InfoWeapon.lock()) {
            sharedWeapon->ObjectStatus = eObjectStatus::none;
            ObjectBaseLocation = sVECTOR3D{0.0f,
                                           -sharedWeapon->AABB[6].y,
                                           -(sharedWeapon->Length/2.0f + sharedWeapon->AABB[6].z)};

            Point = sVECTOR3D{1000.0f,
                              -1000.0f + sharedWeapon->Height / 3.0f,
                              0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedWeapon->Length + sharedWeapon->Width + sharedWeapon->Height) * 0.3f + sharedWeapon->Height * 0.3f,
                                    -(sharedWeapon->Length + sharedWeapon->Width + sharedWeapon->Height) * 0.7f - sharedWeapon->Height * 0.7f};
            InfoObjectWidth = sharedWeapon->Width;
            InfoObjectLength = sharedWeapon->Length;
            InfoObjectHeight = sharedWeapon->Height;
            InfoObjectArmor = sharedWeapon->ArmorInitialStatus;

            sharedWeapon->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});

            InfoObjectEnergyUse = sharedWeapon->EnergyUse;
            InfoObjectAmmo = sharedWeapon->AmmoStart;
            InfoObjectReload = sharedWeapon->NextFireTime;
            InfoObjectRange = GetProjectileRange(tmpCreateNum);
            // remove light source
            if (auto sharedFire = sharedWeapon->Fire.lock()) {
                vw_ReleaseLight(sharedFire->Light);
            }
        }
    }
    if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
        int tmpCreateNum = CreateNum-InfoMineStart+1;
        InfoMine = CreateProjectile(213 + tmpCreateNum);
        if (auto sharedMine = InfoMine.lock()) {
            sharedMine->ObjectStatus = eObjectStatus::none;
            sharedMine->SpeedStart = sharedMine->SpeedEnd = sharedMine->Speed = 0.0f;
            sharedMine->GraphicFXDestroyType = true;
            sharedMine->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedMine->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedMine->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedMine->Height / 2.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedMine->Length + sharedMine->Width + sharedMine->Height) * 0.3f + sharedMine->Height * 0.3f,
                                    -(sharedMine->Length + sharedMine->Width + sharedMine->Height) * 0.7f - sharedMine->Height * 0.7f};

            InfoObjectWidth = sharedMine->Width;
            InfoObjectLength = sharedMine->Length;
            InfoObjectHeight = sharedMine->Height;
            InfoObjectArmor = sharedMine->ArmorInitialStatus;

            sharedMine->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
        int tmpCreateNum = CreateNum-InfoAlienStart+1;
        InfoShip = CreateAlienSpaceFighter(tmpCreateNum);
        if (auto sharedShip = InfoShip.lock()) {
            sharedShip->ObjectStatus = eObjectStatus::none;
            sharedShip->EngineDestroyType = true;
            sharedShip->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedShip->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedShip->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedShip->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.24f + sharedShip->Height * 0.3f,
                                    -(sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.56f - sharedShip->Height * 0.7f};

            InfoObjectWidth = sharedShip->Width;
            InfoObjectLength = sharedShip->Length;
            InfoObjectHeight = sharedShip->Height;
            InfoObjectArmor = sharedShip->ArmorInitialStatus;

            for (auto &tmpEngine : sharedShip->Engines) {
                if (auto sharedEngine = tmpEngine.lock()) {
                    sharedEngine->SpeedOnCreation = -1.0f;
                }
            }

            sharedShip->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
        int tmpCreateNum = CreateNum-InfoAlienMotherShipStart+1;
        InfoShip = CreateAlienSpaceMotherShip(tmpCreateNum);
        if (auto sharedShip = InfoShip.lock()) {
            sharedShip->ObjectStatus = eObjectStatus::none;
            sharedShip->EngineDestroyType = true;
            sharedShip->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedShip->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedShip->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedShip->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.24f + sharedShip->Height * 0.3f,
                                    -(sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.56f - sharedShip->Height * 0.7f};
            InfoObjectWidth = sharedShip->Width;
            InfoObjectLength = sharedShip->Length;
            InfoObjectHeight = sharedShip->Height;
            InfoObjectArmor = sharedShip->ArmorInitialStatus;

            for (auto &tmpEngine : sharedShip->Engines) {
                if (auto sharedEngine = tmpEngine.lock()) {
                    sharedEngine->SpeedOnCreation = -1.0f;
                }
            }

            sharedShip->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
        int tmpCreateNum = CreateNum-InfoPirateShipStart+1;
        InfoShip = CreatePirateShip(tmpCreateNum);
        if (auto sharedShip = InfoShip.lock()) {
            sharedShip->ObjectStatus = eObjectStatus::none;
            sharedShip->EngineDestroyType = true;
            sharedShip->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedShip->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedShip->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedShip->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.24f + sharedShip->Height * 0.3f,
                                    -(sharedShip->Length + sharedShip->Width + sharedShip->Height) * 0.56f - sharedShip->Height * 0.7f};
            InfoObjectWidth = sharedShip->Width;
            InfoObjectLength = sharedShip->Length;
            InfoObjectHeight = sharedShip->Height;
            InfoObjectArmor = sharedShip->ArmorInitialStatus;

            for (auto &tmpEngine : sharedShip->Engines) {
                if (auto sharedEngine = tmpEngine.lock()) {
                    sharedEngine->SpeedOnCreation = -1.0f;
                }
            }

            sharedShip->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
        int tmpCreateNum = CreateNum-InfoBuildingStart+1;
        InfoGroundObject = CreateCivilianBuilding(tmpCreateNum);
        if (auto sharedObject = InfoGroundObject.lock()) {
            sharedObject->ObjectStatus = eObjectStatus::none;
            sharedObject->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedObject->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedObject->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedObject->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.16f + sharedObject->Height * 0.3f,
                                    -(sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.38f - sharedObject->Height * 0.7f};
            InfoObjectWidth = sharedObject->Width;
            InfoObjectLength = sharedObject->Length;
            InfoObjectHeight = sharedObject->Height;
            InfoObjectArmor = sharedObject->ArmorInitialStatus;

            sharedObject->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
        int tmpCreateNum = CreateNum-InfoMilitaryBuildingStart+1;
        InfoGroundObject = CreateMilitaryBuilding(tmpCreateNum);
        if (auto sharedObject = InfoGroundObject.lock()) {
            sharedObject->ObjectStatus = eObjectStatus::none;
            sharedObject->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedObject->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedObject->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedObject->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.24f + sharedObject->Height * 0.3f,
                                    -(sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.56f - sharedObject->Height * 0.7f};
            InfoObjectWidth = sharedObject->Width;
            InfoObjectLength = sharedObject->Length;
            InfoObjectHeight = sharedObject->Height;
            InfoObjectArmor = sharedObject->ArmorInitialStatus;

            sharedObject->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
        int tmpCreateNum = CreateNum-InfoWheeledStart+1;
        InfoGroundObject = CreateWheeled(tmpCreateNum);
        if (auto sharedObject = InfoGroundObject.lock()) {
            sharedObject->WheelShake.reserve(sharedObject->WheelChunkNums.size());
            for (const auto &tmpWheelChunkNum : sharedObject->WheelChunkNums) {
                sharedObject->WheelShake.emplace_back(sVECTOR3D{0.0f, 1.0f, 0.0f},
                                                      tmpWheelChunkNum,
                                                      0.35f,
                                                      [] () {return vw_fRand0() * 0.1f;});
            }

            sharedObject->ObjectStatus = eObjectStatus::none;
            sharedObject->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedObject->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedObject->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};
            sharedObject->WheelRotarySpeed = 350.0f;

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedObject->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.24f + sharedObject->Height * 0.3f,
                                    -(sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.56f - sharedObject->Height * 0.7f};
            InfoObjectWidth = sharedObject->Width;
            InfoObjectLength = sharedObject->Length;
            InfoObjectHeight = sharedObject->Height;
            InfoObjectArmor = sharedObject->ArmorInitialStatus;

            sharedObject->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }
    if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
        int tmpCreateNum = CreateNum-InfoTrackedStart+1;
        InfoGroundObject = CreateTracked(tmpCreateNum);
        if (auto sharedObject = InfoGroundObject.lock()) {
            sharedObject->WheelShake.reserve(sharedObject->WheelChunkNums.size());
            for (const auto &tmpWheelChunkNum : sharedObject->WheelChunkNums) {
                sharedObject->WheelShake.emplace_back(sVECTOR3D{0.0f, 1.0f, 0.0f},
                                                      tmpWheelChunkNum,
                                                      0.35f,
                                                      [] () {return vw_fRand0() * 0.1f;});
            }

            sharedObject->ObjectStatus = eObjectStatus::none;
            sharedObject->SetLocation(sVECTOR3D{1000.0f, -1000.0f - sharedObject->AABB[6].y, 0.0f});
            ObjectBaseLocation = sharedObject->Location - sVECTOR3D{1000.0f, -1000.0f, 0.0f};
            sharedObject->WheelRotarySpeed = 350.0f;

            Point = sVECTOR3D{1000.0f, -1000.0f + sharedObject->Height / 3.0f, 0.0f};

            PointCamera = sVECTOR3D{0.0f,
                                    (sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.24f + sharedObject->Height * 0.3f,
                                    -(sharedObject->Length + sharedObject->Width + sharedObject->Height) * 0.56f - sharedObject->Height * 0.7f};
            InfoObjectWidth = sharedObject->Width;
            InfoObjectLength = sharedObject->Length;
            InfoObjectHeight = sharedObject->Height;
            InfoObjectArmor = sharedObject->ArmorInitialStatus;

            sharedObject->SetRotation(sVECTOR3D{0.0f, RotationSumY, 0.0f});
        }
    }

    GameEnemyArmorPenalty = TMPGameEnemyArmorPenalty;

    LastRotateInfoObject = vw_GetTimeThread(0);
}






void InformationObject3DText(int ObjectNum)
{

    int X1 = GameConfig().InternalWidth / 2 + 68;
    int Y1 = 130;
    int Offset = 30;
    int Size = 194;
    float WScale = -177;


    // common for Earth fighters
    if (CreateNum>=InfoFighterStart && CreateNum<InfoFighterStart+InfoFighterQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Space Ship"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Ship Type:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32(GetShipGroupTitle(ObjectNum-InfoFighterStart+1)));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Codename:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32(GetWorkshopShipName(ObjectNum-InfoFighterStart+1)));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Earth Federation"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Ally"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << InfoObjectWeaponQuantity << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Engines:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << InfoObjectEngineQuantity << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
    }
    // weapons for Earth ships
    if (CreateNum>=InfoWeaponStart && CreateNum<InfoWeaponStart+InfoWeaponQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        if (CreateNum < InfoWeaponStart + 15) {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Weapon, Cannon"));
        } else {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Weapon, Launcher"));
        }
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Type:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32(GetWeaponGroupTitle(CreateNum-InfoWeaponStart+1)));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Codename:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32(GetWeaponName(CreateNum-InfoWeaponStart+1)));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Earth Federation"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage, Kinetic:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << GetProjectileDamageKinetic(CreateNum - InfoWeaponStart + 1) << " ";
        if (CreateNum - InfoWeaponStart + 1 == 11
            || CreateNum - InfoWeaponStart + 1 == 12
            || CreateNum - InfoWeaponStart + 1 == 14) {
            tmpStream << vw_GetText("units/sec");
        } else {
            tmpStream << vw_GetText("units/shot");
        }
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage, EM:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << GetProjectileDamageEM(CreateNum - InfoWeaponStart + 1) << " ";
        if (CreateNum - InfoWeaponStart + 1 == 11
            || CreateNum - InfoWeaponStart + 1 == 12
            || CreateNum - InfoWeaponStart + 1 == 14) {
            tmpStream << vw_GetText("units/sec");
        } else {
            tmpStream << vw_GetText("units/shot");
        }
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Energy Use:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectEnergyUse << " " << vw_GetText("units per shot");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Ammo:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << InfoObjectAmmo << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Reload:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectReload << " " << vw_GetText("seconds");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Range:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectRange << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
    }
    // mines
    if (CreateNum>=InfoMineStart && CreateNum<InfoMineStart+InfoMineQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Weapon"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Type:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Space Mine"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Pirates"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage, Kinetic:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage, EM:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 0 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Targeting System") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());
        if (CreateNum == InfoMineStart) {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("No"));
        } else {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Yes"));
        }

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Auto Cannon:"));
        if (CreateNum == InfoMineStart + 2) {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Yes"));
        } else {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("No"));
        }

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Auto Launcher:"));
        if (CreateNum == InfoMineStart + 3) {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Yes"));
        } else {
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("No"));
        }
    }
    // common for Aliens fighters
    if (CreateNum>=InfoAlienStart && CreateNum<InfoAlienStart+InfoAlienQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Space Ship"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Ship Type:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Unknown"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Unknown"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Unknown"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << 10 << " - " << 200 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
    }
    // common for Alien MotherShips
    if (CreateNum>=InfoAlienMotherShipStart && CreateNum<InfoAlienMotherShipStart+InfoAlienMotherShipQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("MotherShip"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Ship Type:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Unknown"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Unknown"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Unknown"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << 50 << " - " << 500 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
    }
    // common for pirats
    if (CreateNum>=InfoPirateShipStart && CreateNum<InfoPirateShipStart+InfoPirateShipQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Pirate Ship"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Ship Type:"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Pirates"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage:"));
    }
    // common for buildings
    if (CreateNum>=InfoBuildingStart && CreateNum<InfoBuildingStart+InfoBuildingQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Civilian Building"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Old Republic"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Indestructible"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
    }
    // common for ground military
    if (CreateNum>=InfoMilitaryBuildingStart && CreateNum<InfoMilitaryBuildingStart+InfoMilitaryBuildingQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Military Building"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Type:"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Pirates"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage:"));
    }
    // common for wheeled vehicles
    if (CreateNum>=InfoWheeledStart && CreateNum<InfoWheeledStart+InfoWheeledQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Wheeled Vehicle"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Type:"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Pirates"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage:"));
    }
    // common for tracked vehicle
    if (CreateNum>=InfoTrackedStart && CreateNum<InfoTrackedStart+InfoTrackedQuant) {
        Y1 += Offset+5;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Object Class:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Tracked Vehicle"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Type:"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Pirates"));
        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Status:"));
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Enemy"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << InfoObjectArmor << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectWidth << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectLength << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << InfoObjectHeight << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << vw_GetText("Weapons") << ":";
        vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("Damage:"));
    }



    Y1 = 130;

    std::ostringstream tmpStream;
    tmpStream << std::fixed << std::setprecision(0);


    switch (ObjectNum) {


    // pirates
    case InfoPirateShipStart:
    case InfoPirateShipStart+1:
    case InfoPirateShipStart+2:
    case InfoPirateShipStart+3:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Marauder"));
        Y1 += Offset*7;
        tmpStream << vw_GetText("Kinetic") << ", " << vw_GetText("Propelled");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 10 << " - " << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoPirateShipStart+4:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Warder"));
        Y1 += Offset*7;
        tmpStream << vw_GetText("Kinetic") << ", " << vw_GetText("Propelled");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 10 << " - " << 200 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoPirateShipStart+5:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Raider"));
        Y1 += Offset*7;
        tmpStream << vw_GetText("Kinetic") << ", " << vw_GetText("Propelled");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 10 << " - " << 300 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoPirateShipStart+6:
    case InfoPirateShipStart+7:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Missile Carrier"));
        Y1 += Offset*7;
        tmpStream << vw_GetText("Kinetic") << ", " << vw_GetText("Propelled");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 10 << " - " << 500 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoPirateShipStart+8:
    case InfoPirateShipStart+9:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Battle Freighter"));
        Y1 += Offset*7;
        tmpStream << vw_GetText("Kinetic") << ", " << vw_GetText("Propelled");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        Y1 += Offset;
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << 10 << " - " << 300 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoPirateShipStart+10:
    case InfoPirateShipStart+11:
    case InfoPirateShipStart+12:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Missile Carrier"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 50 << " - " << 300 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoPirateShipStart+13:
    case InfoPirateShipStart+14:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Battle Freighter"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 50 << " - " << 200 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;





    // ground military
    case InfoMilitaryBuildingStart:
    case InfoMilitaryBuildingStart+5:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Cannon"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 30 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoMilitaryBuildingStart+3:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("MachineGun"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 50 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoMilitaryBuildingStart+1:
    case InfoMilitaryBuildingStart+6:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Turbo Cannon"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoMilitaryBuildingStart+2:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Missile Launcher"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 200 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoMilitaryBuildingStart+4:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Cannon Turret"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;


    // wheeled vehicles
    case InfoWheeledStart:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Armored Jeep"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 40 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoWheeledStart+1:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Jeep"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoWheeledStart+2:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Armored Jeep"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 40 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoWheeledStart+3:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Jeep"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Ion"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 50 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoWheeledStart+4:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Armored Jeep"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Antimatter"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoWheeledStart+5:
    case InfoWheeledStart+6:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("APC"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 40 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoWheeledStart+7:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Missile Launcher"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 400 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;


    // tracked vehicles
    case InfoTrackedStart:
    case InfoTrackedStart+1:
    case InfoTrackedStart+5:
    case InfoTrackedStart+6:
    case InfoTrackedStart+8:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Tank"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 50 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoTrackedStart+2:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Tank"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Plasma"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 100 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoTrackedStart+3:
    case InfoTrackedStart+4:
    case InfoTrackedStart+7:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Tank"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Antimatter"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 200 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoTrackedStart+9:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("APC"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Kinetic"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 50 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoTrackedStart+10:
    case InfoTrackedStart+11:
    case InfoTrackedStart+12:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Missile Launcher"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Propelled"));
        Y1 += Offset;
        tmpStream << 10 << " - " << 400 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;
    case InfoTrackedStart+13:
        Y1 += Offset+Offset+5;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Engineer"));
        Y1 += Offset*7;
        vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Non-combatant"));
        Y1 += Offset;
        tmpStream << 0 << " " << vw_GetText("units");
        vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());
        break;


    }
}











//------------------------------------------------------------------------------------
// main draw method
//------------------------------------------------------------------------------------
void InformationMenu()
{
    sRECT SrcRect{0, 0, 2, 2};
    sRECT DstRect{0, 0, static_cast<int>(GameConfig().InternalWidth), 768};
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f * MenuContentTransp);


    SrcRect(2,2,464-2,353-2);
    DstRect((GameConfig().InternalWidth/2-432)-8,80-8+80,(GameConfig().InternalWidth/2-432)-8+464-4, 80-8+353-4+80);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/panel444_333_back.tga"), true, 0.9f * MenuContentTransp);



    // print pages count and current page number
    std::ostringstream tmpStream;
    tmpStream << CreateNum << "/" << InfoEnd;
    int Size = vw_TextWidth(tmpStream.str());
    int SizeI = (GameConfig().InternalWidth - Size) / 2;
    vw_DrawText(SizeI, 50+30*16+15, 0, 0, 1.0f, sRGBCOLOR{0.5f, 0.5f, 0.5f}, 0.6f * MenuContentTransp, tmpStream.str());


//    DrawFont(760.0f, 660.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgUp - Previous Page");
//    DrawFont(760.0f, 680.0f, 0, 0, 1, 0.2f*MenuContentTransp, "PgDown - Next Page");



//PgUp – Previous Page
//PgDown – Next Page

    InformationObject3DText(CreateNum);



    // check mouse wheel
    DstRect(GameConfig().InternalWidth / 2 - 440, 80, GameConfig().InternalWidth / 2 + 440,590);
    if (vw_MouseOverRect(DstRect)) {
        if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
            CreateNum += vw_GetWheelStatus();

            if (CreateNum < 1) {
                CreateNum = InfoEnd;
            }
            if (CreateNum > InfoEnd) {
                CreateNum = 1;
            }
            CreateInfoObject();

            vw_ResetWheelStatus();
        }
    } else if (vw_GetWheelStatus() != 0) {
        vw_ResetWheelStatus();
    }



    int Prir = 100;
    int X = GameConfig().InternalWidth / 2 - 432;
    int Y = 165+Prir*4;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32(InfoGroupNames[GetInfoPrevGroup()-1]), MenuContentTransp, false)) {
        CreateNum = GetInfoSwitchToGroup(GetInfoPrevGroup());
        CreateInfoObject();
    }


    X = GameConfig().InternalWidth / 2 - 209;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Page Up"), MenuContentTransp, false)) {
        CreateNum--;
        if (CreateNum < 1) {
            CreateNum = InfoEnd;
        }
        CreateInfoObject();
    }
    if (vw_GetKeyStatus(SDLK_PAGEUP)) {
        CreateNum--;
        if (CreateNum < 1) {
            CreateNum = InfoEnd;
        }
        CreateInfoObject();
        vw_SetKeyStatus(SDLK_PAGEUP, false);
    }

    X = GameConfig().InternalWidth / 2 + 9;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Page Down"), MenuContentTransp, false)) {
        CreateNum++;
        if (CreateNum > InfoEnd) {
            CreateNum = 1;
        }
        CreateInfoObject();
    }
    if (vw_GetKeyStatus(SDLK_PAGEDOWN)) {
        CreateNum++;
        if (CreateNum > InfoEnd) {
            CreateNum = 1;
        }
        CreateInfoObject();
        vw_SetKeyStatus(SDLK_PAGEDOWN, false);
    }


    X = GameConfig().InternalWidth / 2 + 432 - 200;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32(InfoGroupNames[GetInfoNextGroup()-1]), MenuContentTransp, false)) {
        CreateNum = GetInfoSwitchToGroup(GetInfoNextGroup());
        CreateInfoObject();
    }


    X = (GameConfig().InternalWidth - 384) / 2;
    Y = Y + Prir;
    if (DrawButton384(X,Y, vw_GetTextUTF32("MAIN MENU"), MenuContentTransp, Button1Transp, LastButton1UpdateTime)) {
        DestroyInfoObject();
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
    }

}













//------------------------------------------------------------------------------------
// 3D part rendering
//------------------------------------------------------------------------------------
void InformationDrawObject()
{
    int MouseX, MouseY;
    vw_GetMousePos(MouseX, MouseY);

    float tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight;
    vw_GetViewport(&tmpViewportX, &tmpViewportY, &tmpViewportWidth, &tmpViewportHeight);

    vw_SetViewport((GLint)((GameConfig().InternalWidth / 2 - 432) * (tmpViewportWidth / GameConfig().InternalWidth)),
                   (GLint)(160 * (tmpViewportHeight / GameConfig().InternalHeight)),
                   (GLsizei)(444 * (tmpViewportWidth / GameConfig().InternalWidth)),
                   (GLsizei)(333 * (tmpViewportHeight / GameConfig().InternalHeight)));
    vw_ResizeScene(45.0f, 444.0f / 333.0f, 1.0f, 2000.0f);
    vw_Clear(RI_DEPTH_BUFFER);


    vw_LoadIdentity();
    vw_SetCameraLocation(sVECTOR3D{PointCamera.x + 1000.0f,
                                   PointCamera.y - 1000.0f,
                                   PointCamera.z});
    vw_SetCameraMoveAroundPoint(Point, 0.0f, sVECTOR3D{0.0f, 0.0f, 0.0f});
    vw_CameraLookAt();


    // object rotation

    float RotateInfoObjectY = 15.0f * (vw_GetTimeThread(0) - LastRotateInfoObject);
    LastRotateInfoObject = vw_GetTimeThread(0);

    RotationSumY += RotateInfoObjectY;
    if (RotationSumY >= 360.0f) {
        RotationSumY -= 360.0f;
    }

    // adjust object position (in case objects located not at zero point, but with some offset, for example, ship's weapons)
    sVECTOR3D TMPLocation = ObjectBaseLocation;
    float tmp_matrix[9];
    vw_Matrix33CreateRotate(tmp_matrix, sVECTOR3D{0.0f, RotationSumY, 0.0f});
    vw_Matrix33CalcPoint(TMPLocation, tmp_matrix);
    TMPLocation += sVECTOR3D{1000.0f, -1000.0f, 0.0f};


    // draw lines

    // make an array for all elements RI_3f_XYZ | RI_2f_TEX
    float *tmpDATA = new float[4*(3+2)];


    int SizeCell = static_cast<int>(vw_sqrtf((InfoObjectLength * InfoObjectLength / 4.0f +
                                    InfoObjectWidth * InfoObjectWidth / 4.0f +
                                    InfoObjectHeight * InfoObjectHeight / 4.0f)));

    float LineSize = 0.004f * vw_sqrtf(PointCamera.x * PointCamera.x + PointCamera.y * PointCamera.y +
                                       PointCamera.z * PointCamera.z);

    if (SizeCell < 2) {
        SizeCell = 2;
    }


    vw_PushMatrix();
    vw_CullFace(eCullFace::NONE);

    vw_Translate(sVECTOR3D{1000.0f, -1000.0f, 0.0f});
    vw_Rotate(RotationSumY, 0.0f, 1.0f, 0.0f);

    vw_SetColor(0.7f, 0.7f, 1.0f, 0.3f * MenuContentTransp);
    vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);
    vw_BindTexture(0, GetPreloadedTextureAsset("menu/line.tga"));
    for (int i = -SizeCell; i < SizeCell + 2; i += 2) {
        int k=0;

        tmpDATA[k++] = i*1.0f+LineSize;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = SizeCell*1.0f;
        tmpDATA[k++] = 1.0f;
        tmpDATA[k++] = 1.0f;

        tmpDATA[k++] = i*1.0f+LineSize;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = -SizeCell*1.0f;
        tmpDATA[k++] = 1.0f;
        tmpDATA[k++] = 0.0f;

        tmpDATA[k++] = i*1.0f-LineSize;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = SizeCell*1.0f;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = 1.0f;

        tmpDATA[k++] = i*1.0f-LineSize;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = -SizeCell*1.0f;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k] = 0.0f;

        vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_1_TEX, tmpDATA, 5 * sizeof(tmpDATA[0]));


        k = 0;

        tmpDATA[k++] = SizeCell*1.0f;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = i*1.0f+LineSize;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = 0.0f;

        tmpDATA[k++] = SizeCell*1.0f;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = i*1.0f-LineSize;
        tmpDATA[k++] = 1.0f;
        tmpDATA[k++] = 0.0f;

        tmpDATA[k++] = - SizeCell*1.0f;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = i*1.0f + LineSize;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = 1.0f;

        tmpDATA[k++] = - SizeCell*1.0f;
        tmpDATA[k++] = 0.0f;
        tmpDATA[k++] = i*1.0f - LineSize;
        tmpDATA[k++] = 1.0f;
        tmpDATA[k] = 1.0f;

        vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4, RI_3f_XYZ | RI_1_TEX, tmpDATA, 5 * sizeof(tmpDATA[0]));
    }
    if (tmpDATA != nullptr) {
        delete [] tmpDATA;
    }
    vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
    vw_BindTexture(0, 0);
    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    vw_CullFace(eCullFace::BACK);
    vw_PopMatrix();



    if (auto sharedShip = InfoShip.lock()) {
        sharedShip->SetLocation(TMPLocation);
        sharedShip->SetRotation(sVECTOR3D{0.0f, RotateInfoObjectY, 0.0f});
    }
    if (auto sharedWeapon = InfoWeapon.lock()) {
        sharedWeapon->SetLocation(TMPLocation);
        sharedWeapon->SetRotation(sVECTOR3D{0.0f, RotateInfoObjectY, 0.0f});
    }
    if (auto sharedMine = InfoMine.lock()) {
        sharedMine->SetLocation(TMPLocation);
        sharedMine->SetRotation(sVECTOR3D{0.0f, RotateInfoObjectY, 0.0f});
    }
    if (auto sharedObject = InfoGroundObject.lock()) {
        sharedObject->SetLocation(TMPLocation);
        sharedObject->SetRotation(sVECTOR3D{0.0f, RotateInfoObjectY, 0.0f});
    }



    bool ShadowMap{false};

    if (GameConfig().ShadowMap > 0) {
        float EffectiveDistance = PointCamera.Length();
        ShadowMap_StartRenderToFBO(sVECTOR3D{0.0f, 0.0f, 0.0f}, EffectiveDistance, EffectiveDistance*2);

        // since some 3D models don't have 'back' sides - tank's tracks, covers elements for tires
        // and tracks, etc., we are forced to disable face's culling during shadows map generation
        vw_CullFace(eCullFace::NONE);

        if (auto sharedShip = InfoShip.lock()) {
            sharedShip->Draw(true);
            // render weapon
            if (!sharedShip->WeaponSlots.empty()) {
                for (auto &tmpWeaponSlot : sharedShip->WeaponSlots) {
                    if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                        sharedWeapon->Draw(true);
                    }
                }
            }
        }
        if (auto sharedWeapon = InfoWeapon.lock()) {
            sharedWeapon->Draw(true);
        }
        if (auto sharedMine = InfoMine.lock()) {
            sharedMine->Draw(true);
        }
        if (auto sharedObject = InfoGroundObject.lock()) {
            sharedObject->Draw(true);
        }

        vw_CullFace(eCullFace::BACK);
        ShadowMap_EndRenderToFBO();

        ShadowMap = true;
        ShadowMap_StartFinalRender();
    }



    if (auto sharedShip = InfoShip.lock()) {
        sharedShip->Draw(false, ShadowMap);
        // render weapon
        if (!sharedShip->WeaponSlots.empty()) {
            for (auto &tmpWeaponSlot : sharedShip->WeaponSlots) {
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    sharedWeapon->Draw(false, ShadowMap);
                }
            }
        }
        // render engines for this ship only
        vw_DrawParticleSystems(sharedShip->Engines);
    }
    if (auto sharedWeapon = InfoWeapon.lock()) {
        sharedWeapon->Draw(false, ShadowMap);
    }
    if (auto sharedMine = InfoMine.lock()) {
        sharedMine->Draw(false, ShadowMap);
        vw_DrawParticleSystems(sharedMine->GraphicFX); // visual effects for this ship only
    }
    if (auto sharedObject = InfoGroundObject.lock()) {
        sharedObject->Draw(false, ShadowMap);
    }

    if (GameConfig().ShadowMap > 0) {
        ShadowMap_EndFinalRender();
    }



    vw_SetCameraLocation(sVECTOR3D{-50.0f, 30.0f, -50.0f});
    vw_SetViewport(tmpViewportX, tmpViewportY, tmpViewportWidth, tmpViewportHeight);
    vw_ResizeScene(45.0f, GameConfig().InternalWidth / GameConfig().InternalHeight, 1.0f, 2000.0f);

    // border with shadow
    vw_Start2DMode(-1,1);
    sRECT SrcRect;
    SrcRect(2,2,482,371);
    sRECT DstRect(GameConfig().InternalWidth/2-450, 80-18+80, GameConfig().InternalWidth/2+30, 80+351+80);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/panel444_333_border.tga"), true, MenuContentTransp);

    vw_End2DMode();
}

} // astromenace namespace
} // viewizard namespace
