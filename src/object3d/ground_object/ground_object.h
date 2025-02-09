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

// TODO split code for classes (? also separate classes for ground explosions)
//      inheritance: cGroundObject -> cCivilianBuilding
//                   cGroundObject + turret code -> cMilitaryBuilding
//                   cMilitaryBuilding + wheels code -> cWheeled
//                   cWheeled + tracks code -> cTracked

// TODO WeaponFireType should be enumeration

#ifndef OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H
#define OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H

#include "../object3d.h"
#include "../shake.h"
#include "../weapon/weapon.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eGroundCycle {
    Continue,
    Break,
    DeleteObjectAndContinue,
    DeleteObjectAndBreak
};

struct sGroundWeaponSlot {
    bool SetFire{false};
    std::weak_ptr<cWeapon> Weapon{};
    sVECTOR3D Location{};
    sVECTOR3D Bound{};

    sGroundWeaponSlot() = default;
    sGroundWeaponSlot(const std::weak_ptr<cWeapon> &_Weapon, const sVECTOR3D &_Location) :
        Weapon{_Weapon},
        Location{_Location}
    {}
    ~sGroundWeaponSlot()
    {
        ReleaseWeaponLazy(Weapon);
    }

    void operator () (const std::weak_ptr<cWeapon> &_Weapon, const sVECTOR3D &_Location)
    {
        Weapon = _Weapon;
        Location = _Location;
    }
};

class cGroundObject : public cObject3D {
protected:
    // don't allow object of this class creation
    cGroundObject();
    ~cGroundObject() = default;

    // should be called in UpdateWithTimeSheetList() only
    virtual bool Update(float Time) override;

public:
    virtual void SetLocation(const sVECTOR3D &NewLocation) override;
    virtual void SetRotation(const sVECTOR3D &NewRotation) override;

    // rotary speed (deg/sec) for all wheels (for both, wheeled and tracked)
    float WheelRotarySpeed{0.0f};
    float SpeedToWheelRotarySpeedFactor{0.0f};

    // wheel chunks (for both, wheeled and tracked)
    std::vector<unsigned> WheelChunkNums{};
    // small wheel shake (for both, wheeled and tracked)
    std::vector<cShake> WheelShake{};
    // steerable wheel chunks (for wheeled only)
    std::vector<unsigned> SteerableWheelChunkNums{};
    float MaxSteerableWheelAngle{20.0f};

    int TrackChunkNum{-1};
    int TrackRotationDirection{1};

    sVECTOR3D NeedRotate{0.0f, 0.0f, 0.0f};
    sVECTOR3D RotationSpeed{1.0f, 1.0f, 1.0f};

    float MaxSpeed{0.0f};
    float Speed{0.0f};
    float NeedSpeed{0.0f};

    float MaxAcceler{0.0f};
    float MaxSpeedRotate{0.0f};
    float Acceler{0.0f};
    float NeedAcceler{1.0f};

    std::vector<sGroundWeaponSlot> WeaponSlots{};
    int WeaponFireType{2};
    int WeaponGroupCurrentFireNum{-1};
    float WeaponGroupCurrentFireDelay{0.0f};
    bool WeaponTargeting{false};

    sVECTOR3D BaseBound{0.0f, 0.0f, 0.0f};
    sVECTOR3D MiddleBound{0.0f, 0.0f, 0.0f};
    bool DoNotCalculateRotation{false};

    // horizontal targeting related (turret can rotate at 360 deg)
    std::vector<unsigned> TargetHorizChunkNums{};
    float TargetHorizChunksCurrentAngle{0.0f};
    float TargetHorizChunksNeedAngle{0.0f};
    // vertical targeting related
    std::vector<unsigned> TargetVertChunkNums{};
    float TargetVertChunksMaxAngle{80.0f};
    float TargetVertChunksMinAngle{0.0f};
    float TargetVertChunksCurrentAngle{0.0f};
    float TargetVertChunksNeedAngle{0.0f};
    // barrel chunks (we may need rotate barrels)
    std::vector<unsigned> BarrelChunkNums{};
};

class cCivilianBuilding final : public cGroundObject {
    friend std::weak_ptr<cGroundObject> CreateCivilianBuilding(const int BuildingNum);
private:
    // Don't allow direct new/delete usage in code, only CreateCivilianBuilding()
    // allowed for cCivilianBuilding creation and release setup (deleter must be provided).
    explicit cCivilianBuilding(const int BuildingNum);
    ~cCivilianBuilding() = default;
};

class cMilitaryBuilding final : public cGroundObject {
    friend std::weak_ptr<cGroundObject> CreateMilitaryBuilding(const int MilitaryBuildingNum);
private:
    // Don't allow direct new/delete usage in code, only CreateMilitaryBuilding()
    // allowed for cMilitaryBuilding creation and release setup (deleter must be provided).
    explicit cMilitaryBuilding(const int MilitaryBuildingNum);
    ~cMilitaryBuilding() = default;
};

class cTracked final : public cGroundObject {
    friend std::weak_ptr<cGroundObject> CreateTracked(const int TrackedNum);
private:
    // Don't allow direct new/delete usage in code, only CreateTracked()
    // allowed for cTracked creation and release setup (deleter must be provided).
    explicit cTracked(const int TrackedNum);
    ~cTracked() = default;
};

class cWheeled final : public cGroundObject {
    friend std::weak_ptr<cGroundObject> CreateWheeled(const int WheeledNum);
private:
    // Don't allow direct new/delete usage in code, only CreateWheeled()
    // allowed for cWheeled creation and release setup (deleter must be provided).
    explicit cWheeled(const int WheeledNum);
    ~cWheeled() = default;
};


// Create cCivilianBuilding object.
std::weak_ptr<cGroundObject> CreateCivilianBuilding(const int BuildingNum);
// Create cMilitaryBuilding object.
std::weak_ptr<cGroundObject> CreateMilitaryBuilding(const int MilitaryBuildingNum);
// Create cTracked object.
std::weak_ptr<cGroundObject> CreateTracked(const int TrackedNum);
// Create cWheeled object.
std::weak_ptr<cGroundObject> CreateWheeled(const int WheeledNum);
// Update and remove (erase) dead objects.
void UpdateAllGroundObjects(float Time);
// Draw all ground objects.
void DrawAllGroundObjects(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular ground object.
void ReleaseGroundObject(std::weak_ptr<cGroundObject> &Object);
// Release all ground objects.
void ReleaseAllGroundObjects();
// Cycle for each ground object.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachGroundObject(std::function<void (cGroundObject &Object)> function);
// Managed cycle for each ground object.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachGroundObject(std::function<void (cGroundObject &Object, eGroundCycle &Command)> function);
// Get object ptr by reference.
std::weak_ptr<cObject3D> GetGroundObjectPtr(const cGroundObject &Object);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_GROUNDOBJECT_GROUNDOBJECT_H
