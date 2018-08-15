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

// TODO codestyle should be fixed

// TODO translate comments

#include "../object3d.h"
#include "../space_ship/space_ship.h"
#include "../ground_object/ground_object.h"
#include "../projectile/projectile.h"
#include "../space_object/space_object.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

constexpr float RadToDeg = 180.0f / 3.14159f; // convert radian to degree

} // unnamed namespace


//-----------------------------------------------------------------------------
// Получение угла поворота ракеты, торпеды или бомбы
//-----------------------------------------------------------------------------
std::weak_ptr<cObject3D> GetMissileOnTargetOrientation(eObjectStatus ObjectStatus, // статус объекта, который целится
					const sVECTOR3D &Location, // положение точки относительно которой будем наводить
					const sVECTOR3D &CurrentObjectRotation, // текущие углы объекта
					const float (&RotationMatrix)[9], // матрица вращения объекта
					sVECTOR3D &NeedAngle, // нужные углы, чтобы получить нужное направление
					float MaxDistance) // максимальная дистанция, на которую может лететь снаряд
{
	// получаем точки для создания плоскости
	sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
	vw_Matrix33CalcPoint(Orientation, RotationMatrix);
	sVECTOR3D PointUp{0.0f, 1.0f, 0.0f};
	vw_Matrix33CalcPoint(PointUp, RotationMatrix);
	sVECTOR3D PointRight{1.0f, 0.0f, 0.0f};
	vw_Matrix33CalcPoint(PointRight, RotationMatrix);

	// находим плоскость, вертикальную
	float A, B, C, D;
	vw_GetPlaneABCD(A, B, C, D, Location, Location+Orientation, Location+PointUp);

	// получаем вертикальную плоскость 2 (отсечения перед-зад)
	float A2, B2, C2, D2;
	vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + PointRight, Location + PointUp);

	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	sVECTOR3D TargetLocation = Location;
	NeedAngle = CurrentObjectRotation;
	float Tdist{1000.0f * 1000.0f};

	// цель
	std::weak_ptr<cObject3D> Target{};

	ForEachProjectile([&] (const cProjectile &tmpProjectile) {
		// только фларес
		if ((tmpProjectile.ProjectileType == 3) &&
		    NeedCheckCollision(tmpProjectile) &&
		    ObjectsStatusFoe(ObjectStatus, tmpProjectile.ObjectStatus)) {

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpProjectile.Location.x + B2 * tmpProjectile.Location.y + C2 * tmpProjectile.Location.z + D2;
			if (tmp1 > 0.0f) {
				float TargetDist2TMP = A2 * tmpProjectile.Location.x + B2 * tmpProjectile.Location.y + C2 * tmpProjectile.Location.z + D2;

				if (MaxDistance > TargetDist2TMP) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...

					sVECTOR3D TargetAngleTMP{CurrentObjectRotation};
					TargetLocation = tmpProjectile.Location;

					// находим угол между плоскостью и прямой
					float A3, B3, C3, D3;
					vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

					float m = TargetLocation.x - Location.x;
					float n = TargetLocation.y - Location.y;
					float p = TargetLocation.z - Location.z;

					// поправки к существующим углам поворота оружия
					float sss1 = vw_sqrtf(m * m + n * n + p * p);
					float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * RadToDeg;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * RadToDeg;
					}

					if (Tdist > m * m + n * n + p * p) {
						NeedAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						Target = GetProjectilePtr(tmpProjectile);
					}
				}
			}
		}
	});

	// the idea of tmpDistanceFactorByObjectType is provide targeting priority, we increase factor
	// for different types of objects, so, we need next type objects position closer than previous
	// in order to change target, but, reset factor to 1.0f in case we lock and check same type
	// objects (or don't have any target yet)
	float tmpDistanceFactorByObjectType{1.0f};
	if (!Target.expired())
		tmpDistanceFactorByObjectType = 3.0f;
	ForEachGroundObject([&] (const cGroundObject &tmpGround) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpGround) &&
		    ObjectsStatusFoe(ObjectStatus, tmpGround.ObjectStatus)) {
			sVECTOR3D tmpLocation = tmpGround.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpGround.CurrentRotationMat); // поворачиваем в плоскость объекта
			TargetLocation = tmpGround.Location + tmpLocation;

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;
			if (tmp1 > 0.0f) {

				float TargetDist2TMP = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;

				if (MaxDistance > TargetDist2TMP) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...
					sVECTOR3D TargetAngleTMP{CurrentObjectRotation};

					// находим угол между плоскостью и прямой
					float A3, B3, C3, D3;
					vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

					float m = TargetLocation.x - Location.x;
					float n = TargetLocation.y - Location.y;
					float p = TargetLocation.z - Location.z;

					// поправки к существующим углам поворота оружия
					float sss1 = vw_sqrtf(m * m + n * n + p * p);
					float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * RadToDeg;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * RadToDeg;
					}

					if ((Tdist / tmpDistanceFactorByObjectType > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						NeedAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						Target = GetGroundObjectPtr(tmpGround);
						tmpDistanceFactorByObjectType = 1.0f;
					}
				}
			}
		}
	});

	if (!Target.expired())
		tmpDistanceFactorByObjectType = 6.0f;
	ForEachSpaceShip([&] (const cSpaceShip &tmpShip) {
		// проверка, чтобы не считать свой корабль
		if (NeedCheckCollision(tmpShip) &&
		    ObjectsStatusFoe(ObjectStatus, tmpShip.ObjectStatus)) {

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpShip.Location.x + B2 * tmpShip.Location.y + C2 * tmpShip.Location.z + D2;
			if (tmp1 > 0.0f) {
				float TargetDist2TMP = A2 * tmpShip.Location.x + B2 * tmpShip.Location.y + C2 * tmpShip.Location.z + D2;

				if (MaxDistance > TargetDist2TMP) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...

					sVECTOR3D TargetAngleTMP{CurrentObjectRotation};
					TargetLocation = tmpShip.Location;

					// находим угол между плоскостью и прямой
					float A3, B3, C3, D3;
					vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

					float m = TargetLocation.x - Location.x;
					float n = TargetLocation.y - Location.y;
					float p = TargetLocation.z - Location.z;

					// поправки к существующим углам поворота оружия
					float sss1 = vw_sqrtf(m * m + n * n + p * p);
					float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * RadToDeg;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * RadToDeg;
					}

					if ((Tdist / tmpDistanceFactorByObjectType > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						NeedAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						Target = GetSpaceShipPtr(tmpShip);
						tmpDistanceFactorByObjectType = 1.0f;
					}
				}
			}
		}
	});

	if (!Target.expired())
		tmpDistanceFactorByObjectType = 10.0f;
	ForEachSpaceObject([&] (const cSpaceObject &tmpSpace) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpSpace) &&
		    ObjectsStatusFoe(ObjectStatus, tmpSpace.ObjectStatus) &&
		    (tmpSpace.ObjectType != eObjectType::SpaceDebris)) {
					// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpSpace.Location.x  + B2 * tmpSpace.Location.y  + C2 * tmpSpace.Location.z + D2;
			if (tmp1 > 0.0f) {

				float TargetDist2TMP = A2 * tmpSpace.Location.x + B2 * tmpSpace.Location.y + C2 * tmpSpace.Location.z + D2;

				if (MaxDistance > TargetDist2TMP) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...
					sVECTOR3D TargetAngleTMP{CurrentObjectRotation};
					TargetLocation = tmpSpace.Location;

					// находим угол между плоскостью и прямой
					float A3, B3, C3, D3;
					vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

					float m = TargetLocation.x - Location.x;
					float n = TargetLocation.y - Location.y;
					float p = TargetLocation.z - Location.z;

					// поправки к существующим углам поворота оружия
					float sss1 = vw_sqrtf(m * m + n * n + p * p);
					float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * RadToDeg;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * RadToDeg;
					}

					if ((Tdist / tmpDistanceFactorByObjectType > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						NeedAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						Target = GetSpaceObjectPtr(tmpSpace);
						tmpDistanceFactorByObjectType = 1.0f;
					}
				}
			}
		}
	});

	return Target;
}

/*
 * Correct target intercept course for missile.
 * See "Dihedral angle" (geometry) for more info about what we are doing here.
 */
bool CorrectTargetInterceptCourse(const sVECTOR3D &MissileLocation, const sVECTOR3D &MissileRotation,
				  const float (&MissileRotationMatrix)[9],
				  std::weak_ptr<cObject3D> &Target, sVECTOR3D &NeedAngle)
{
	auto sharedTarget = Target.lock();
	if (!sharedTarget)
		return false;

	sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
	vw_Matrix33CalcPoint(Orientation, MissileRotationMatrix);
	sVECTOR3D PointUp{0.0f, 1.0f, 0.0f};
	vw_Matrix33CalcPoint(PointUp, MissileRotationMatrix);
	sVECTOR3D PointRight{1.0f, 0.0f, 0.0f};
	vw_Matrix33CalcPoint(PointRight, MissileRotationMatrix);

	// vertical plane (ahead/behind)
	float A, B, C, D;
	vw_GetPlaneABCD(A, B, C, D, MissileLocation, MissileLocation + PointRight, MissileLocation + PointUp);
	if ((A * sharedTarget->Location.x +
	     B * sharedTarget->Location.y +
	     C * sharedTarget->Location.z + D) <= 0.0f)
		return false;

	NeedAngle = MissileRotation;

	sVECTOR3D tmpTargetGeometryCenter = sharedTarget->GeometryCenter;
	vw_Matrix33CalcPoint(tmpTargetGeometryCenter, sharedTarget->CurrentRotationMat);
	sVECTOR3D tmpDistance = sharedTarget->Location + tmpTargetGeometryCenter - MissileLocation;
	float tmpLength = tmpDistance.Length();

	// missile "targeting" zone (missile targeting system's "view" zone)
	// TODO probably, we should connect this parameter to missile type (?)
	constexpr float TargetingZone = 1.0f; // in radians (!)

	// horizontal plane (up/down)
	vw_GetPlaneABCD(A, B, C, D, MissileLocation, MissileLocation + Orientation, MissileLocation + PointRight);
	float tmpNormalLength = vw_sqrtf(A * A + B * B + C * C);
	NeedAngle.x = MissileRotation.x;
	if ((tmpLength > 0.0f) && (tmpNormalLength > 0.0f)) {
		float tmpDihedralAngle = (A * tmpDistance.x + B * tmpDistance.y + C * tmpDistance.z) /
					 (tmpLength * tmpNormalLength);
		if ((tmpDihedralAngle >= -TargetingZone) &&
		    (tmpDihedralAngle <= TargetingZone))
			NeedAngle.x = MissileRotation.x - asinf(tmpDihedralAngle) * RadToDeg;
	}

	// vertical plane (left/right)
	vw_GetPlaneABCD(A, B, C, D, MissileLocation, MissileLocation + Orientation, MissileLocation + PointUp);
	tmpNormalLength = vw_sqrtf(A * A + B * B + C * C);
	NeedAngle.y = MissileRotation.y;
	if ((tmpLength > 0.0f) && (tmpNormalLength > 0.0f)) {
		float tmpDihedralAngle = (A * tmpDistance.x + B * tmpDistance.y + C * tmpDistance.z) /
					 (tmpLength * tmpNormalLength);
		if ((tmpDihedralAngle >= -TargetingZone) &&
		    (tmpDihedralAngle <= TargetingZone))
			NeedAngle.y = MissileRotation.y - asinf(tmpDihedralAngle) * RadToDeg;
	}

	return true;
}

/*
 * Check, that missile target stay ahead.
 */
static bool MissileTargetStayAhead(const cObject3D &Target,
				   const sVECTOR3D &MissileLocation,
				   const float (&MissileRotationMatrix)[9])
{
	sVECTOR3D PointUp(0.0f, 1.0f, 0.0f);
	vw_Matrix33CalcPoint(PointUp, MissileRotationMatrix);
	sVECTOR3D PointRight(1.0f, 0.0f, 0.0f);
	vw_Matrix33CalcPoint(PointRight, MissileRotationMatrix);

	float A, B, C, D;
	vw_GetPlaneABCD(A, B, C, D, MissileLocation, MissileLocation + PointRight, MissileLocation + PointUp);

	if ((A * Target.Location.x +
	     B * Target.Location.y +
	     C * Target.Location.z + D) > 0.0f)
		return true;

	return false;
}

/*
 * Check locked by missle target.
 */
bool CheckMissileTarget(std::weak_ptr<cObject3D> &Target, const sVECTOR3D &MissileLocation,
			const float (&MissileRotationMatrix)[9])
{
	auto sharedTarget = Target.lock();
	if (!sharedTarget)
		return false;

	bool ObjectFound{false};
	ForEachProjectile([&sharedTarget, &ObjectFound] (const cProjectile &tmpProjectile, eProjectileCycle &Command) {
		if (&tmpProjectile == sharedTarget.get()) {
			ObjectFound = true;
			Command = eProjectileCycle::Break;
		}
	});
	if (ObjectFound)
		return MissileTargetStayAhead(*sharedTarget, MissileLocation, MissileRotationMatrix);

	ForEachGroundObject([&sharedTarget, &ObjectFound] (const cGroundObject &tmpGround, eGroundCycle &Command) {
		if (&tmpGround == sharedTarget.get()) {
			ObjectFound = true;
			Command = eGroundCycle::Break;
		}
	});
	if (ObjectFound)
		return MissileTargetStayAhead(*sharedTarget, MissileLocation, MissileRotationMatrix);

	ForEachSpaceShip([&sharedTarget, &ObjectFound] (const cSpaceShip &tmpShip, eShipCycle &Command) {
		if (&tmpShip == sharedTarget.get()) {
			ObjectFound = true;
			Command = eShipCycle::Break;
		}
	});
	if (ObjectFound)
		return MissileTargetStayAhead(*sharedTarget, MissileLocation, MissileRotationMatrix);

	ForEachSpaceObject([&sharedTarget, &ObjectFound] (const cSpaceObject &tmpSpace, eSpaceCycle &Command) {
		if (&tmpSpace == sharedTarget.get()) {
			ObjectFound = true;
			Command = eSpaceCycle::Break;
		}
	});
	if (ObjectFound)
		return MissileTargetStayAhead(*sharedTarget, MissileLocation, MissileRotationMatrix);

	return false;
}

/*
 * Get closest target to mine.
 */
std::weak_ptr<cObject3D> GetClosestTargetToMine(eObjectStatus MineStatus, const sVECTOR3D &MineLocation)
{
	std::weak_ptr<cObject3D> ClosestTarget{};
	float MinDistance2{-1.0f};

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip) {
		if (NeedCheckCollision(tmpShip) &&
		    ObjectsStatusFoe(MineStatus, tmpShip.ObjectStatus)) {
			float tmpDistance2 = (tmpShip.Location.x - MineLocation.x) * (tmpShip.Location.x - MineLocation.x) +
					     (tmpShip.Location.y - MineLocation.y) * (tmpShip.Location.y - MineLocation.y) +
					     (tmpShip.Location.z - MineLocation.z) * (tmpShip.Location.z - MineLocation.z);

			if (MinDistance2 < 0.0f) {
				MinDistance2 = tmpDistance2;
				ClosestTarget = GetSpaceShipPtr(tmpShip);
			} else if (tmpDistance2 < MinDistance2) {
				MinDistance2 = tmpDistance2;
				ClosestTarget = GetSpaceShipPtr(tmpShip);
			}
		}
	});

	return ClosestTarget;
}

} // astromenace namespace
} // viewizard namespace
