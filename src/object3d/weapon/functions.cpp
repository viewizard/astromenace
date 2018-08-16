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


//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для турелей наземных объектов
//-----------------------------------------------------------------------------
bool GetTurretOnTargetOrientation(eObjectStatus ObjectStatus, // статус объекта, который целится
				  const sVECTOR3D &Location, // положение точки относительно которой будем наводить
				  const sVECTOR3D &CurrentObjectRotation, // текущие углы объекта
				  const float (&RotationMatrix)[9], // матрица вращения объекта
				  sVECTOR3D &NeedAngle, // нужные углы, чтобы получить нужное направление
				  int WeaponType) // номер оружия
{
	// получаем точки для создания плоскости
	sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
	vw_Matrix33CalcPoint(Orientation, RotationMatrix);
	sVECTOR3D PointRight{1.0f, 0.0f, 0.0f};
	vw_Matrix33CalcPoint(PointRight, RotationMatrix);
	sVECTOR3D PointUp{0.0f, 1.0f, 0.0f};
	vw_Matrix33CalcPoint(PointUp, RotationMatrix);

	// находим плоскость, горизонтальную
	float A, B, C, D;
	vw_GetPlaneABCD(A, B, C, D, Location, Location + Orientation, Location + PointRight);

	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	sVECTOR3D TargetLocation{Location};
	float TargetDist2{1000.0f * 1000.0f};
	bool TargetLocked{false};

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpShip) &&
		    ObjectsStatusFoe(ObjectStatus, tmpShip.ObjectStatus)) {

			sVECTOR3D tmpLocation = tmpShip.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpShip.CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpShip.Location + tmpLocation;

			// находим, за какое время снаряд долетит до объекта сейчас
			sVECTOR3D TTT = Location - RealLocation;
			float ProjectileSpeed = GetProjectileSpeed(WeaponType);
			if (ObjectStatus == eObjectStatus::Enemy)
				ProjectileSpeed = ProjectileSpeed / GameEnemyWeaponPenalty;
			float CurrentDist = TTT.Length();
			float ObjCurrentTime = CurrentDist / ProjectileSpeed;

			// находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
			sVECTOR3D FutureLocation = tmpShip.Orientation ^ (tmpShip.Speed * ObjCurrentTime);
			// учитываем камеру...
			sVECTOR3D CamPosTTT{0.0f,0.0f,0.0f};
			if (tmpShip.ObjectStatus == eObjectStatus::Player)
				CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * ObjCurrentTime);

			// находи точку по середине... это нам и надо... туда целимся...
			sVECTOR3D PossibleRealLocation = RealLocation + FutureLocation + CamPosTTT;

			TTT = Location - PossibleRealLocation;
			float PossibleDist = TTT.Length();
			float PoprTime = PossibleDist/ProjectileSpeed;

			FutureLocation = tmpShip.Orientation ^ (tmpShip.Speed * PoprTime);
			// учитываем камеру...
			CamPosTTT = sVECTOR3D{0.0f, 0.0f, 0.0f};
			if (tmpShip.ObjectStatus == eObjectStatus::Player)
				CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * PoprTime);

			RealLocation = RealLocation + FutureLocation + CamPosTTT;

			// проверяем, если все точки выше
			float tmp1 = A * (RealLocation.x + tmpShip.OBB.Box[0].x) + B * (RealLocation.y + tmpShip.OBB.Box[0].y) + C * (RealLocation.z + tmpShip.OBB.Box[0].z) + D;
			float tmp2 = A * (RealLocation.x + tmpShip.OBB.Box[1].x) + B * (RealLocation.y + tmpShip.OBB.Box[1].y) + C * (RealLocation.z + tmpShip.OBB.Box[1].z) + D;
			float tmp3 = A * (RealLocation.x + tmpShip.OBB.Box[2].x) + B * (RealLocation.y + tmpShip.OBB.Box[2].y) + C * (RealLocation.z + tmpShip.OBB.Box[2].z) + D;
			float tmp4 = A * (RealLocation.x + tmpShip.OBB.Box[3].x) + B * (RealLocation.y + tmpShip.OBB.Box[3].y) + C * (RealLocation.z + tmpShip.OBB.Box[3].z) + D;
			float tmp5 = A * (RealLocation.x + tmpShip.OBB.Box[4].x) + B * (RealLocation.y + tmpShip.OBB.Box[4].y) + C * (RealLocation.z + tmpShip.OBB.Box[4].z) + D;
			float tmp6 = A * (RealLocation.x + tmpShip.OBB.Box[5].x) + B * (RealLocation.y + tmpShip.OBB.Box[5].y) + C * (RealLocation.z + tmpShip.OBB.Box[5].z) + D;
			float tmp7 = A * (RealLocation.x + tmpShip.OBB.Box[6].x) + B * (RealLocation.y + tmpShip.OBB.Box[6].y) + C * (RealLocation.z + tmpShip.OBB.Box[6].z) + D;
			float tmp8 = A * (RealLocation.x + tmpShip.OBB.Box[7].x) + B * (RealLocation.y + tmpShip.OBB.Box[7].y) + C * (RealLocation.z + tmpShip.OBB.Box[7].z) + D;

			if ((tmp1 > 0.0f) && (tmp2 > 0.0f) && (tmp3 > 0.0f) && (tmp4 > 0.0f) &&
			    (tmp5 > 0.0f) && (tmp6 > 0.0f) && (tmp7 > 0.0f) && (tmp8 > 0.0f)) {
				float TargetDist2TMP = (Location.x - RealLocation.x) * (Location.x - RealLocation.x) +
						       (Location.y - RealLocation.y) * (Location.y - RealLocation.y) +
						       (Location.z - RealLocation.z) * (Location.z - RealLocation.z);

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if (TargetDist2 > TargetDist2TMP) {
					// запоминаем эти данные
					TargetLocation = RealLocation;
					TargetDist2 = TargetDist2TMP;
					TargetLocked = true;
				}
			}
		}
	});

	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked) {
		// находим угол между плоскостью и прямой
		float m = TargetLocation.x - Location.x;
		float n = TargetLocation.y - Location.y;
		float p = TargetLocation.z - Location.z;

		// поправки к существующим углам поворота оружия
		float sss1 = m * m + n * n +p * p;
		float sss2 = A * A + B * B +C * C;
		if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
			float ttt = (A * m + B * n + C * p) / (vw_sqrtf(sss1) * vw_sqrtf(sss2));
			if ((ttt >= -1.0f) && (ttt <= 1.0f))
				NeedAngle.x = CurrentObjectRotation.x + asinf(ttt) * RadToDeg;
		}

		NeedAngle.z = CurrentObjectRotation.z;

		// нужно найти точку на плоскости, образованную перпендикуляром с точки TargetLocation
		// иначе не правильно будем ориентировать
		if (sss2 != 0.0f) {
			float t = -(A * TargetLocation.x + B * TargetLocation.y + C * TargetLocation.z + D) / (A * A + B * B + C * C);
			TargetLocation.x = t * A + TargetLocation.x;
			TargetLocation.y = t * B + TargetLocation.y;
			TargetLocation.z = t * C + TargetLocation.z;
			m = TargetLocation.x - Location.x;
			n = TargetLocation.y - Location.y;
			p = TargetLocation.z - Location.z;

			// находим плоскость, вертикальную
			float A2, B2, C2, D2;
			vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + PointUp, Location + PointRight);

			// смотрим в какой полуплоскости
			float tmp1_1 = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;
			vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + Orientation, Location + PointUp);

			if (tmp1_1 >= 0.0f) {
				// находим угол поворота
				sss1 = vw_sqrtf(m * m + n * n + p * p);
				float sss3 = vw_sqrtf(A2 * A2 + B2 * B2 + C2 * C2);
				if ((sss1 != 0.0f) && (sss3 != 0.0f)) {
					float ttt = (A2 * m + B2 * n + C2 * p) / (sss1 * sss3);
					if ((ttt >= -1.0f) && (ttt <= 1.0f))
						NeedAngle.y = 180.0f - asinf(ttt) * RadToDeg;
				}
			} else {
				// находим угол поворота
				sss1 = vw_sqrtf(m * m + n * n + p * p);
				float sss3 = vw_sqrtf(A2 * A2 + B2 * B2 + C2 * C2);
				if ((sss1 != 0.0f) && (sss3 != 0.0f)) {
					float ttt = (A2 * m + B2 * n + C2 * p) / (sss1 * sss3);
					if ((ttt >= -1.0f) && (ttt <= 1.0f)) {
						NeedAngle.y = asinf(ttt) * RadToDeg;
						if (NeedAngle.y < 0.0f)
							NeedAngle.y += 360.0f;
					}
				}
			}
		}
	}

	// передаем навелись или нет (нет врагов)
	return TargetLocked;
}

} // astromenace namespace
} // viewizard namespace
