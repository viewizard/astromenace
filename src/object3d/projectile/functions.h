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

#ifndef OBJECT3D_PROJECTILE_FUNCTIONS_H
#define OBJECT3D_PROJECTILE_FUNCTIONS_H

#include "../object3d.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// Find missile target and target intercept course for missile.
std::weak_ptr<cObject3D>
FindTargetAndInterceptCourse(eObjectStatus MissileObjectStatus, const sVECTOR3D &MissileLocation,
                             const sVECTOR3D &MissileRotation, const float (&MissileRotationMatrix)[9],
                             sVECTOR3D &NeedAngle, const float MaxMissileFlyDistance);
// Correct target intercept course for missile.
bool CorrectTargetInterceptCourse(const sVECTOR3D &MissileLocation, const sVECTOR3D &MissileRotation,
                                  const float (&MissileRotationMatrix)[9],
                                  std::weak_ptr<cObject3D> &Target, sVECTOR3D &NeedAngle);
// Check locked by missle target.
bool CheckMissileTarget(std::weak_ptr<cObject3D> &Target, const sVECTOR3D &MissileLocation,
                        const float (&MissileRotationMatrix)[9]);
// Get closest target to mine.
std::weak_ptr<cObject3D> GetClosestTargetToMine(eObjectStatus MineStatus, const sVECTOR3D &MineLocation);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_PROJECTILE_FUNCTIONS_H
