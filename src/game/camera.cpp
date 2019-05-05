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

// FIXME camera code should not include player's ship manipulations code...

/*

OpenGL use right-handed coordinate system. Scene axises orientation:

     ^ Y
     |
     |  / Z
     | /
<-----/
X

  ^ Y
  |     camera location (see core/camera)
  |    /
  |   *    ----> CameraMovementDirection vector
  |   | \
  |   |  \ - camera frustum (see core/camera)
  |   |   \
--|---|----\----------------> Z
  |
  |

*/

#include "../core/core.h"
#include "../object3d/space_ship/space_ship.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

constexpr float MaxInfluenceDistance2{100.0f * 100.0f};

float CameraLastUpdate{0.0f};
float CameraSpeed{10.0f};
sVECTOR3D CameraCoveredDistance{0.0f, 0.0f, 0.0f};
sVECTOR3D CameraMovementDirection{0.0f, 0.0f, 1.0f};
// camera shake on explosion related vaiables
float CameraCurrentShake{0.0f};
float CameraNeedShake{0.0f};
float CameraShakeTimeLeft{0.0f};
float CameraShakeInitialTime{0.0f};
float CameraShakePower{0.0f};

} // unnamed namespace


/*
 * Reset camera status.
 */
void ResetCamera()
{
    CameraCoveredDistance(0.0f, 0.0f, 0.0f);
    CameraLastUpdate = 0.0f;

    CameraCurrentShake = 0.0f;
    CameraNeedShake = 0.0f;
    CameraShakeTimeLeft = 0.0f;
    CameraShakeInitialTime = 0.0f;
    CameraShakePower = 0.0f;
    vw_SetCameraDeviation(sVECTOR3D{0.0f, 0.0f, 0.0f});
}

/*
 * Initialize camera.
 */
void InitCamera()
{
    ResetCamera();
    CameraLastUpdate = vw_GetTimeThread(1);
}

/*
 * Setup camera shake.
 */
void SetupCameraShake(const sVECTOR3D &Location, float Power)
{
    auto sharedPlayerFighter = PlayerFighter.lock();
    if (!sharedPlayerFighter) {
        return;
    }

    float tmpDist2 = (sharedPlayerFighter->Location.x - Location.x) * (sharedPlayerFighter->Location.x - Location.x) +
                     (sharedPlayerFighter->Location.y - Location.y) * (sharedPlayerFighter->Location.y - Location.y) +
                     (sharedPlayerFighter->Location.z - Location.z) * (sharedPlayerFighter->Location.z - Location.z);

    if (tmpDist2 > MaxInfluenceDistance2) {
        return;
    }

    if (tmpDist2 <= sharedPlayerFighter->Radius * sharedPlayerFighter->Radius) {
        tmpDist2 = sharedPlayerFighter->Radius * sharedPlayerFighter->Radius;
    }

    CameraShakeInitialTime = CameraShakeTimeLeft = (MaxInfluenceDistance2 - tmpDist2) / MaxInfluenceDistance2;
    if (Power > 1.0f) { // huge explosion
        CameraShakeInitialTime = CameraShakeTimeLeft = CameraShakeTimeLeft * 3.0f;
    }

    CameraShakePower = Power * (MaxInfluenceDistance2 - tmpDist2) / (MaxInfluenceDistance2 * 2);

    CameraNeedShake = CameraShakePower * vw_fRand0();
}

/*
 * Update camera shake.
 */
static void UpdateCameraShake(float TimeDelta)
{
    CameraShakeTimeLeft -= TimeDelta;
    if (CameraShakeTimeLeft < 0.0f) {
        CameraShakeTimeLeft = 0.0f;
    }

    bool tmpNeedStopDeviation{false};
    if (CameraShakeTimeLeft <= 0.0f && CameraCurrentShake != 0.0f) {
        tmpNeedStopDeviation = true;
        CameraNeedShake = 0.0f;
    }

    if (CameraShakeTimeLeft > 0.0f || tmpNeedStopDeviation) {
        float Sign{1.0f};
        if (CameraNeedShake < CameraCurrentShake) {
            Sign = -1.0f;
        }

        float tmpIncrement = Sign * 5.0f * TimeDelta;

        if ((Sign > 0.0f && CameraNeedShake <= CameraCurrentShake + tmpIncrement) ||
            (Sign < 0.0f && CameraNeedShake >= CameraCurrentShake + tmpIncrement)) {
            CameraCurrentShake = CameraNeedShake;
            CameraShakePower *= CameraShakeTimeLeft / CameraShakeInitialTime;
            CameraNeedShake = CameraShakePower * vw_fRand0();
        } else {
            CameraCurrentShake += tmpIncrement;
        }
    }

    vw_SetCameraDeviation(sVECTOR3D{CameraCurrentShake * 2.0f, CameraCurrentShake, 0.0f});
}

/*
 * Update camera.
 */
void CameraUpdate(float Time)
{
    float TimeDelta = Time - CameraLastUpdate;
    CameraLastUpdate = Time;

    sVECTOR3D tmpDistance = CameraMovementDirection ^ (CameraSpeed * TimeDelta);
    CameraCoveredDistance += tmpDistance;
    vw_IncCameraLocation(tmpDistance);

    if (auto sharedPlayerFighter = PlayerFighter.lock()) {
        sharedPlayerFighter->SetLocationArcadePlayer(sharedPlayerFighter->Location + tmpDistance);
    }

    UpdateCameraShake(TimeDelta);
}

/*
 * Get camera shake.
 */
float GetCameraShake()
{
    return CameraCurrentShake;
}

/*
 * Get camera speed.
 */
float GetCameraSpeed()
{
    return CameraSpeed;
}

/*
 * Get camera covered distance.
 */
const sVECTOR3D &GetCameraCoveredDistance()
{
    return CameraCoveredDistance;
}

/*
 * Get camera movement direction.
 */
const sVECTOR3D &GetCameraMovementDirection()
{
    return CameraMovementDirection;
}

} // astromenace namespace
} // viewizard namespace
