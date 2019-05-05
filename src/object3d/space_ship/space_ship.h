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

// TODO switch fire type to enumeration

// TODO split earth fighter and player ship classes

#ifndef OBJECT3D_SPACESHIP_SPACESHIP_H
#define OBJECT3D_SPACESHIP_SPACESHIP_H

#include "../object3d.h"
#include "../shake.h"
#include "../weapon/weapon.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eShipCycle {
    Continue,
    Break,
    DeleteObjectAndContinue,
    DeleteObjectAndBreak
};

enum class eShipPairCycle {
    Continue,
    DeleteFirstObjectAndContinue,
    DeleteSecondObjectAndContinue,
    DeleteBothObjectsAndContinue
};

struct sShipWeaponSlot {
    bool SetFire{false};
    std::weak_ptr<cWeapon> Weapon{};
    sVECTOR3D Location{};
    int Type{1};
    float YAngle{0.0f};

    sShipWeaponSlot() = default;
    sShipWeaponSlot(const std::weak_ptr<cWeapon> &_Weapon, const sVECTOR3D &_Location) :
        Weapon{_Weapon},
        Location{_Location}
    {}
    sShipWeaponSlot(int _Type, const sVECTOR3D &_Location) :
        Location{_Location},
        Type{_Type}
    {}
    ~sShipWeaponSlot()
    {
        ReleaseWeaponLazy(Weapon);
    }

    void operator () (const std::weak_ptr<cWeapon> &_Weapon, const sVECTOR3D &_Location)
    {
        Weapon = _Weapon;
        Location = _Location;
    }
};

class cSpaceShip : public cObject3D {
protected:
    // don't allow object of this class creation
    cSpaceShip() = default;
    ~cSpaceShip();

public:
    virtual bool Update(float Time) override;
    virtual void SetLocation(const sVECTOR3D &NewLocation) override;
    virtual void SetLocationArcadePlayer(const sVECTOR3D &NewLocation);
    virtual void SetRotation(const sVECTOR3D &NewRotation) override;

    sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

    sVECTOR3D NeedRotate{0.0f, 0.0f, 0.0f};
    sVECTOR3D RotationSpeed{1.0f, 1.0f, 1.0f};

    float MaxSpeed{0.0f};
    float MaxAcceler{0.0f};
    float MaxSpeedRotate{0.0f};

    float Speed{0.0f};
    float NeedSpeed{0.0f};
    float Acceler{0.0f};
    float NeedAcceler{1.0f};

    float SpeedLR{0.0f};
    float NeedSpeedLR{0.0f};
    float AccelerLR{0.0f};
    float NeedAccelerLR{1.0f};

    float SpeedUD{0.0f};
    float NeedSpeedUD{0.0f};
    float AccelerUD{0.0f};
    float NeedAccelerUD{1.0f};

    float SpeedByCamFB{0.0f};
    float NeedSpeedByCamFB{0.0f};
    float AccelerByCamFB{0.0f};
    float NeedAccelerByCamFB{1.0f};

    float SpeedByCamLR{0.0f};
    float NeedSpeedByCamLR{0.0f};
    float AccelerByCamLR{0.0f};
    float NeedAccelerByCamLR{1.0f};

    float SpeedByCamUD{0.0f};
    float NeedSpeedByCamUD{0.0f};
    float AccelerByCamUD{0.0f};
    float NeedAccelerByCamUD{1.0f};

    float MoveForward{0.0f};
    float MoveBackward{0.0f};
    float MoveLeft{0.0f};
    float MoveRight{0.0f};
    float MoveUp{0.0f};
    float MoveDown{0.0f};

    std::vector<cShake> ShipShake{};

    std::vector<sShipWeaponSlot> WeaponSlots{};
    int WeaponFireType{2}; // 1- volley fire, 2 - alternate fire
    int WeaponGroupCurrentFireNum{-1};
    float WeaponGroupCurrentFireDelay{0.0f};

    std::vector<sShipWeaponSlot> BossWeaponSlots{};
    int BossWeaponFireType{2}; // 1- volley fire, 2 - alternate fire
    int BossWeaponGroupCurrentFireNum{-1};
    float BossWeaponGroupCurrentFireDelay{0.0f};

    std::vector<sShipWeaponSlot> FlareWeaponSlots{};

    bool EngineDestroyType{false};
    std::vector<std::weak_ptr<cParticleSystem>> Engines{};
    std::vector<sVECTOR3D> EnginesLocation{};

    std::vector<std::weak_ptr<cParticleSystem>> EnginesLeft{};
    std::vector<sVECTOR3D> EnginesLeftLocation{};

    std::vector<std::weak_ptr<cParticleSystem>> EnginesRight{};
    std::vector<sVECTOR3D> EnginesRightLocation{};
};

class cAlienSpaceFighter final : public cSpaceShip {
    friend std::weak_ptr<cSpaceShip> CreateAlienSpaceFighter(const int SpaceShipNum);
private:
    // Don't allow direct new/delete usage in code, only CreateAlienSpaceFighter()
    // allowed for cAlienSpaceFighter creation and release setup (deleter must be provided).
    explicit cAlienSpaceFighter(const int SpaceShipNum);
    ~cAlienSpaceFighter() = default;
};

class cAlienSpaceMotherShip final : public cSpaceShip {
    friend std::weak_ptr<cSpaceShip> CreateAlienSpaceMotherShip(const int SpaceShipNum);
private:
    // Don't allow direct new/delete usage in code, only CreateAlienSpaceMotherShip()
    // allowed for cAlienSpaceMotherShip creation and release setup (deleter must be provided).
    explicit cAlienSpaceMotherShip(const int SpaceShipNum);
    ~cAlienSpaceMotherShip() = default;
};

class cEarthSpaceFighter final : public cSpaceShip {
    friend std::weak_ptr<cSpaceShip> CreateEarthSpaceFighter(const int SpaceShipNum);
private:
    // Don't allow direct new/delete usage in code, only cEarthSpaceFighter()
    // allowed for cEarthSpaceFighter creation and release setup (deleter must be provided).
    explicit cEarthSpaceFighter(const int SpaceShipNum);
    ~cEarthSpaceFighter() = default;
};

class cPirateShip final : public cSpaceShip {
    friend std::weak_ptr<cSpaceShip> CreatePirateShip(const int SpaceShipNum);
private:
    // Don't allow direct new/delete usage in code, only cPirateShip()
    // allowed for cPirateShip creation and release setup (deleter must be provided).
    explicit cPirateShip(const int SpaceShipNum);
    ~cPirateShip() = default;
};


// Create cAlienSpaceFighter object.
std::weak_ptr<cSpaceShip> CreateAlienSpaceFighter(const int SpaceShipNum);
// Create cAlienSpaceMotherShip object.
std::weak_ptr<cSpaceShip> CreateAlienSpaceMotherShip(const int SpaceShipNum);
// Create cEarthSpaceFighter object.
std::weak_ptr<cSpaceShip> CreateEarthSpaceFighter(const int SpaceShipNum);
// Create cPirateShip object.
std::weak_ptr<cSpaceShip> CreatePirateShip(const int SpaceShipNum);

// Update and remove (erase) dead objects.
void UpdateAllSpaceShip(float Time);
// Draw all objects.
void DrawAllSpaceShips(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular space ship.
void ReleaseSpaceShip(std::weak_ptr<cSpaceShip> &Ship);
// Release all objects.
void ReleaseAllSpaceShips();
// Cycle for each space ship.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceShip(std::function<void (cSpaceShip &Object)> function);
// Managed cycle for each space ship.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceShip(std::function<void (cSpaceShip &Object, eShipCycle &Command)> function);
// Managed cycle for each space ship pair.
// Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
void ForEachSpaceShipPair(std::function<void (cSpaceShip &FirstObject,
                          cSpaceShip &SecondObject,
                          eShipPairCycle &Command)> function);
// Get object ptr by reference.
std::weak_ptr<cObject3D> GetSpaceShipPtr(const cSpaceShip &Object);

// Setup engines.
void SetEarthSpaceFighterEngine(std::weak_ptr<cSpaceShip> &SpaceShip, const int EngineType);
// Setup armor.
void SetEarthSpaceFighterArmor(std::weak_ptr<cSpaceShip> &SpaceShip, const int ArmorType);
// Setup weapon.
bool SetEarthSpaceFighterWeapon(std::weak_ptr<cSpaceShip> &SpaceShip, const int WeaponSlot, const int WeaponNum);
// Get max and min weapon slot angle for particular ship.
void GetShipWeaponSlotAngle(const int ShipNum, const int SlotNum, float &Min, float &Max);
// Get max engine acceleration.
float GetEngineAcceleration(const int EngineType);
// Get engine power.
float GetEnginePower(const int EngineType);
// Get rotate engine power.
float GetEngineRotatePower(const int EngineType);
// Get armor for particular ship.
float GetShipArmor(const int SpaceShipNum);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_SPACESHIP_SPACESHIP_H
