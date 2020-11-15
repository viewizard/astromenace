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

#include "../object3d.h"
#include "../space_ship/space_ship.h"
#include "../ground_object/ground_object.h"
#include "../projectile/projectile.h"
#include "../space_object/space_object.h"
#include <cmath>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

constexpr float RadToDeg = 180.0f / 3.14159f; // convert radian to degree

} // unnamed namespace


/*
 * Find angles to aim on target with prediction.
 */
void GetWeaponOnTargetOrientation(eObjectStatus WeaponStatus, const sVECTOR3D &WeaponLocation,
                                  const sVECTOR3D &TargetingComputerLocation, const sVECTOR3D &CurrentWeaponRotation,
                                  float MinTargetingDistance, const float (&CurrentWeaponRotationMatrix)[9],
                                  sVECTOR3D &NeedAngle, bool NeedCenterOrientation, int WeaponType)
{
    float tmpDistanceToLockedTarget2{1000.0f * 1000.0f};
    bool TargetLocked{false};

    sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
    vw_Matrix33CalcPoint(Orientation, CurrentWeaponRotationMatrix);
    sVECTOR3D PointUp{0.0f, 1.0f, 0.0f};
    vw_Matrix33CalcPoint(PointUp, CurrentWeaponRotationMatrix);
    sVECTOR3D PointRight{1.0f, 0.0f, 0.0f};
    vw_Matrix33CalcPoint(PointRight, CurrentWeaponRotationMatrix);

    // vertical plane (left/right), note, OpenGL use right-handed coordinate system
    float A, B, C, D;
    vw_GetPlaneABCD(A, B, C, D, TargetingComputerLocation,
                    TargetingComputerLocation + Orientation, TargetingComputerLocation + PointUp);
    float A2B2C2D2NormalLength = vw_sqrtf(A * A + B * B + C * C);

    // vertical plane (ahead/behind), note, OpenGL use right-handed coordinate system
    float A2, B2, C2, D2;
    vw_GetPlaneABCD(A2, B2, C2, D2, TargetingComputerLocation,
                    TargetingComputerLocation + PointRight, TargetingComputerLocation + PointUp);

    // vertical plane (ahead/behind), note, OpenGL use right-handed coordinate system
    float A3, B3, C3, D3;
    vw_GetPlaneABCD(A3, B3, C3, D3, TargetingComputerLocation,
                    TargetingComputerLocation + Orientation, TargetingComputerLocation + PointRight);
    float A3B3C3D3NormalLength = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);

    // the idea of tmpDistanceFactorByObjectType is provide targeting priority, we increase factor
    // for different types of objects, so, we need next type objects position closer than previous
    // in order to change target, but, reset factor to 1.0f in case we lock and check same type
    // objects (or don't have any target yet)
    float tmpDistanceFactorByObjectType{1.0f};

    auto FindTargetCalculateAngles = [&] (const sVECTOR3D &TargetLocation, const sVECTOR3D &TargetOrientation,
                                          sVECTOR3D TargetGeometryCenter, const float (&TargetRotationMat)[9],
    const float TargetSpeed, const float TargetRadius) {
        vw_Matrix33CalcPoint(TargetGeometryCenter, TargetRotationMat);
        sVECTOR3D RealLocation = TargetLocation + TargetGeometryCenter;

        if (TargetSpeed != 0.0f
            && WeaponType != 0
            && /* not beam */ WeaponType != 11 && WeaponType != 12 && WeaponType != 14
            && /* not missile */ WeaponType != 16 && WeaponType != 17 && WeaponType != 18 && WeaponType != 19) {
            float ProjectileSpeed = GetProjectileSpeed(WeaponType);
            float CurrentDist = (RealLocation - TargetingComputerLocation).Length();
            float ObjCurrentTime = CurrentDist / ProjectileSpeed;
            sVECTOR3D FutureLocation = TargetOrientation ^ (TargetSpeed * ObjCurrentTime);
            RealLocation = RealLocation + FutureLocation;
        }

        // check, that target is ahead of weapon and opposite to it
        if (fabs(A * RealLocation.x +
                 B * RealLocation.y +
                 C * RealLocation.z + D) <= TargetRadius
            && A2 * RealLocation.x +
               B2 * RealLocation.y +
               C2 * RealLocation.z + D2 > MinTargetingDistance) {

            sVECTOR3D tmpTargetAngle = CurrentWeaponRotation;
            sVECTOR3D tmpDistance = RealLocation - WeaponLocation;
            float tmpLength2 = tmpDistance.x * tmpDistance.x +
                               tmpDistance.y * tmpDistance.y +
                               tmpDistance.z * tmpDistance.z;
            float tmpLength = vw_sqrtf(tmpLength2);

            if (tmpLength > 0.0f && A3B3C3D3NormalLength > 0.0f) {
                // see "Angle between line and plane" (geometry) for more info about what we are doing here
                float tmpSineOfAngle = (A3 * tmpDistance.x + B3 * tmpDistance.y + C3 * tmpDistance.z) /
                                       (tmpLength * A3B3C3D3NormalLength);
                vw_Clamp(tmpSineOfAngle, -1.0f, 1.0f); // arc sine is computed in the interval [-1, +1]
                tmpTargetAngle.x = CurrentWeaponRotation.x - asinf(tmpSineOfAngle) * RadToDeg;
            }

            if (NeedCenterOrientation && tmpLength > 0.0f && A2B2C2D2NormalLength > 0.0f) {
                // see "Angle between line and plane" (geometry) for more info about what we are doing here
                float tmpSineOfAngle = (A * tmpDistance.x + B * tmpDistance.y + C * tmpDistance.z) /
                                       (tmpLength * A2B2C2D2NormalLength);
                vw_Clamp(tmpSineOfAngle, -1.0f, 1.0f); // arc sine is computed in the interval [-1, +1]
                tmpTargetAngle.y = CurrentWeaponRotation.y - asinf(tmpSineOfAngle) * RadToDeg;
            }

            if (tmpDistanceToLockedTarget2 / tmpDistanceFactorByObjectType > tmpLength2
                && fabsf(tmpTargetAngle.x) < 45.0f) {
                NeedAngle = tmpTargetAngle;
                tmpDistanceToLockedTarget2 = tmpLength2;
                TargetLocked = true;
                tmpDistanceFactorByObjectType = 1.0f;
            }
        }
    };

    ForEachSpaceShip([&] (const cSpaceShip &tmpShip) {
        if (NeedCheckCollision(tmpShip) && ObjectsStatusFoe(WeaponStatus, tmpShip.ObjectStatus)) {
            FindTargetCalculateAngles(tmpShip.Location, tmpShip.Orientation, tmpShip.GeometryCenter,
                                      tmpShip.CurrentRotationMat, tmpShip.Speed, tmpShip.Radius);
        }
    });

    if (TargetLocked) {
        tmpDistanceFactorByObjectType = 5.0f;
    }
    ForEachGroundObject([&] (const cGroundObject &tmpGround) {
        if (NeedCheckCollision(tmpGround) && ObjectsStatusFoe(WeaponStatus, tmpGround.ObjectStatus)) {
            FindTargetCalculateAngles(tmpGround.Location, tmpGround.Orientation, tmpGround.GeometryCenter,
                                      tmpGround.CurrentRotationMat, tmpGround.Speed, tmpGround.Radius);
        }
    });

    if (TargetLocked) {
        tmpDistanceFactorByObjectType = 10.0f;
    }
    ForEachSpaceObject([&] (const cSpaceObject &tmpSpace) {
        if (NeedCheckCollision(tmpSpace) && ObjectsStatusFoe(WeaponStatus, tmpSpace.ObjectStatus)) {
            FindTargetCalculateAngles(tmpSpace.Location, tmpSpace.Orientation, tmpSpace.GeometryCenter,
                                      tmpSpace.CurrentRotationMat, tmpSpace.Speed, tmpSpace.Radius);
        }
    });
}

} // astromenace namespace
} // viewizard namespace
