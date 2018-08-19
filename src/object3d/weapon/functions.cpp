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

// TODO translate comments

#include "../object3d.h"
#include "../space_ship/space_ship.h"
#include "../projectile/projectile.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

constexpr float RadToDeg = 180.0f / 3.14159f; // convert radian to degree

} // unnamed namespace

// FIXME should be fixed, don't allow global scope interaction for local variables
// направление движения камеры
extern sVECTOR3D GameCameraMovement;
// скорость движения камеры
float GameCameraGetSpeed();


/*
 * Find turret target and angle to aim target with prediction.
 */
// FIXME should work with any object rotation (x, y, z)
// TODO no beam weapon support?
// TODO probably, we need revise all turret-related code in order to use relative angles
//      for turret rotation, as we have in FindTargetAndInterceptCourse() for missiles
//      so, we should use current barrel plane instead of object plane in order to calculate angles
//      in this case, we could stay with [-1, 1] for art sine, as we have in FindTargetAndInterceptCourse()
// NOTE NeedAngle should count on current 3d object rotation, since this is "additional" angle for barrel
bool GetTurretOnTargetOrientation(eObjectStatus ObjectStatus, // статус объекта, который целится
				  const sVECTOR3D &Location, // положение точки относительно которой будем наводить
				  const sVECTOR3D &CurrentObjectRotation, // текущие углы объекта
				  const float (&RotationMatrix)[9], // матрица вращения объекта
				  sVECTOR3D &NeedAngle, // нужные углы, чтобы получить нужное направление
				  int WeaponType) // номер оружия
{
	NeedAngle = CurrentObjectRotation;
	sVECTOR3D TargetLocation{Location};
	float DistanceToLockedTarget2{1000.0f * 1000.0f};
	bool TargetLocked{false};

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip) {
		if (!NeedCheckCollision(tmpShip) ||
		    !ObjectsStatusFoe(ObjectStatus, tmpShip.ObjectStatus))
			return;

		sVECTOR3D tmpLocation = tmpShip.GeometryCenter;
		vw_Matrix33CalcPoint(tmpLocation, tmpShip.CurrentRotationMat);
		sVECTOR3D tmpRealLocation = tmpShip.Location + tmpLocation;

		sVECTOR3D tmpRealDistance = Location - tmpRealLocation;
		float ProjectileSpeed = GetProjectileSpeed(WeaponType);
		if (ObjectStatus == eObjectStatus::Enemy)
			ProjectileSpeed = ProjectileSpeed / GameEnemyWeaponPenalty;
		float CurrentDist = tmpRealDistance.Length();
		float ObjCurrentTime = CurrentDist / ProjectileSpeed;

		sVECTOR3D FutureLocation = tmpShip.Orientation ^ (tmpShip.Speed * ObjCurrentTime);
		sVECTOR3D CamPosInfluence{0.0f, 0.0f, 0.0f};
		if (tmpShip.ObjectStatus == eObjectStatus::Player)
			CamPosInfluence = GameCameraMovement ^ (GameCameraGetSpeed() * ObjCurrentTime);

		sVECTOR3D PossibleRealLocation = tmpRealLocation + FutureLocation + CamPosInfluence;

		tmpRealDistance = Location - PossibleRealLocation;
		float PossibleDist = tmpRealDistance.Length();
		float PoprTime = PossibleDist / ProjectileSpeed;

		FutureLocation = tmpShip.Orientation ^ (tmpShip.Speed * PoprTime);

		CamPosInfluence = sVECTOR3D{0.0f, 0.0f, 0.0f};
		if (tmpShip.ObjectStatus == eObjectStatus::Player)
			CamPosInfluence = GameCameraMovement ^ (GameCameraGetSpeed() * PoprTime);

		tmpRealLocation = tmpRealLocation + FutureLocation + CamPosInfluence;

		float tmpDistanceToTarget2 = (Location.x - tmpRealLocation.x) * (Location.x - tmpRealLocation.x) +
					     (Location.y - tmpRealLocation.y) * (Location.y - tmpRealLocation.y) +
					     (Location.z - tmpRealLocation.z) * (Location.z - tmpRealLocation.z);

		if (DistanceToLockedTarget2 > tmpDistanceToTarget2) {
			TargetLocation = tmpRealLocation;
			DistanceToLockedTarget2 = tmpDistanceToTarget2;
			TargetLocked = true;
		}
	});

	if (!TargetLocked)
		return false;

	sVECTOR3D tmpDistance = TargetLocation - Location;
	float tmpLength = tmpDistance.Length();

	sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
	vw_Matrix33CalcPoint(Orientation, RotationMatrix);
	sVECTOR3D PointRight{1.0f, 0.0f, 0.0f};
	vw_Matrix33CalcPoint(PointRight, RotationMatrix);

	// horizontal plane (up/down), note, OpenGL use right-handed coordinate system
	float A, B, C, D;
	vw_GetPlaneABCD(A, B, C, D, Location, Location + Orientation, Location + PointRight);
	float ABCDNormalLength = vw_sqrtf(A * A + B * B + C * C);

	if ((tmpLength > 0.0f) && (ABCDNormalLength > 0.0f)) {
		// see "Angle between line and plane" (geometry) for more info about what we are doing here
		float tmpSineOfAngle = (A * tmpDistance.x + B * tmpDistance.y + C * tmpDistance.z) /
				       (tmpLength * ABCDNormalLength);
		// with asinf(), arc sine could be computed in the interval [-1, +1] only
		vw_Clamp(tmpSineOfAngle, -1.0f, 1.0f);
		NeedAngle.x = asinf(tmpSineOfAngle) * RadToDeg;
	}

	// find target location point projection onto horizontal plane
	if (ABCDNormalLength > 0.0f) {
		float tmpDistanceToPlane = fabs(A * TargetLocation.x + B * TargetLocation.y + C * TargetLocation.z + D) /
					   ABCDNormalLength;
		// reuse TargetLocation for point projection onto horizontal plane
		TargetLocation.x = TargetLocation.x - tmpDistanceToPlane * A;
		TargetLocation.y = TargetLocation.y - tmpDistanceToPlane * B;
		TargetLocation.z = TargetLocation.z - tmpDistanceToPlane * C;
		tmpDistance = TargetLocation - Location;
		tmpLength = tmpDistance.Length();
	}
	if (tmpLength == 0.0f)
		return true;

	sVECTOR3D PointUp{0.0f, 1.0f, 0.0f};
	vw_Matrix33CalcPoint(PointUp, RotationMatrix);

	// vertical plane (left/right), note, OpenGL use right-handed coordinate system
	float A3, B3, C3, D3;
	vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointUp);
	float A3B3C3D3NormalLength = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
	if (A3B3C3D3NormalLength == 0.0f)
		return true;

	// see "Angle between line and plane" (geometry) for more info about what we are doing here
	float tmpSineOfAngle = (A3 * tmpDistance.x + B3 * tmpDistance.y + C3 * tmpDistance.z) /
			       (tmpLength * A3B3C3D3NormalLength);
	// with asinf(), arc sine could be computed in the interval [-1, +1] only
	vw_Clamp(tmpSineOfAngle, -1.0f, 1.0f);

	// vertical plane (ahead/behind), note, OpenGL use right-handed coordinate system
	float A2, B2, C2, D2;
	vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + PointRight, Location + PointUp);
	if ((A2 * TargetLocation.x +
	     B2 * TargetLocation.y +
	     C2 * TargetLocation.z + D2) <= 0.0f)
		NeedAngle.y = 180.0f - asinf(tmpSineOfAngle) * RadToDeg;
	else
		NeedAngle.y = asinf(tmpSineOfAngle) * RadToDeg;

	return true;
}

} // astromenace namespace
} // viewizard namespace
