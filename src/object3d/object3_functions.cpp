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

#include "../assets/model3d.h"
#include "object3d.h"
#include "space_ship/space_ship.h"
#include "ground_object/ground_object.h"
#include "projectile/projectile.h"
#include "space_object/space_object.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, don't allow global scope interaction for local variables
float GetProjectileSpeed(int Num);

std::weak_ptr<cGLSL> GLSLShaderType1{};
std::weak_ptr<cGLSL> GLSLShaderType2{};
std::weak_ptr<cGLSL> GLSLShaderType3{};

// направление движения камеры
extern sVECTOR3D GameCameraMovement;
// скорость движения камеры
float GameCameraGetSpeed();


//-----------------------------------------------------------------------------
// Проверяем, нужно ли для данного объекта проверка коллизии и наведение на него
// так же используется для определения "неубиваемых" объектов
//-----------------------------------------------------------------------------
bool NeedCheckCollision(const cObject3D &Object3D)
{
	// don't use 'default' case here, we need compiler's warning if anyone was missed
	switch (Object3D.ObjectType) {
	case eObjectType::none:
		return false;

	case eObjectType::EarthFighter:
		return true;

	case eObjectType::AlienFighter:
		return true;

	case eObjectType::AlienMotherShip:
		return true;

	case eObjectType::PirateShip:
		return true;

	case eObjectType::PirateVehicle:
		return true;

	case eObjectType::PirateBuilding:
		return true;

	case eObjectType::SmallAsteroid:
		return true;

	case eObjectType::SpaceDebris:
		return true;

	case eObjectType::ShipWeapon:
		return true;

	case eObjectType::Projectile:
		return true;

	case eObjectType::Explosion:
		return false;

	case eObjectType::CivilianBuilding:
		return false;

	case eObjectType::BasePart:
		return false;

	case eObjectType::Planet:
		return false;

	case eObjectType::BigAsteroid:
		return false;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Загрузка в модель нужной геометрии
//-----------------------------------------------------------------------------
void LoadObjectData(const std::string &FileName, cObject3D &Object3D)
{
	// получение геометрии модели
	std::weak_ptr<sModel3D> Model = GetPreloadedModel3DAsset(FileName);
	auto sharedModel = Model.lock();
	if (!sharedModel)
		return;

	// берем то, что нужно
	Object3D.GlobalVertexArray = sharedModel->GlobalVertexArray;
	Object3D.GlobalVertexArrayCount = sharedModel->GlobalVertexArrayCount;
	Object3D.GlobalVBO = sharedModel->GlobalVBO;
	Object3D.GlobalIndexArray = sharedModel->GlobalIndexArray;
	Object3D.GlobalIndexArrayCount = sharedModel->GlobalIndexArrayCount;
	Object3D.GlobalIBO = sharedModel->GlobalIBO;
	Object3D.GlobalVAO = sharedModel->GlobalVAO;
	Object3D.Chunks = sharedModel->Chunks;

	// резервируем память для текстур
	Object3D.Texture.resize(Object3D.Chunks.size(), 0);
	Object3D.TextureIllum.resize(Object3D.Chunks.size(), 0);
	Object3D.NormalMap.resize(Object3D.Chunks.size(), 0);

	// metadata
	Object3D.AABB = sharedModel->AABB;
	Object3D.OBB = sharedModel->OBB;
	Object3D.HitBB = sharedModel->HitBB;
	Object3D.GeometryCenter = sharedModel->GeometryCenter;
	Object3D.Radius = sharedModel->Radius;
	Object3D.Width = sharedModel->Width;
	Object3D.Length = sharedModel->Length;
	Object3D.Height = sharedModel->Height;
}

//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для космических кораблей
//-----------------------------------------------------------------------------
void GetShipOnTargetOrientateion(eObjectStatus ObjectStatus, // статус объекта, который целится
				 const sVECTOR3D &Location, // положение точки относительно которой будем наводить
				 const sVECTOR3D &CurrentObjectRotation, // текущие углы объекта
				 float MinDistance, // минимальное расстояние, с которого начинаем прицеливание
				 const float (&RotationMatrix)[9], // матрица вращения объекта
				 sVECTOR3D &NeedAngle,// нужные углы, чтобы получить нужное направление
				 float Width, // ширина объекта
				 bool NeedCenterOrientation, // нужен доворот на центр
				 bool NeedByWeaponOrientation, // нужно делать доворот с учетом положения орудия
				 const sVECTOR3D &WeponLocation,
				 int WeaponType) // тип орудия орудия
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
	vw_GetPlaneABCD(A, B, C, D, Location, Location + Orientation, Location + PointUp);

	// получаем вертикальную плоскость 2 (отсечения перед-зад)
	float A2, B2, C2, D2;
	vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + PointRight, Location + PointUp);

	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	sVECTOR3D TargetLocation{Location};
	sVECTOR3D TargetAngle(0.0f, 0.0f, 0.0f);
	float Tdist{1000.0f * 1000.0f};

	// тип, кто заблокировал... чтобы не сбить с активных
	int TType{0};

	bool TargetLocked{false};

	// нам нужна только половина ширины
	float Width2 = Width / 2.0f;

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip, eShipCycle &UNUSED(Command)) {
		// проверка, чтобы не считать свой корабль
		if ((NeedCheckCollision(tmpShip)) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip.ObjectStatus == eObjectStatus::Ally) || (tmpShip.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip.ObjectStatus == eObjectStatus::Enemy)))) {
			// находим настоящую точку попадания с учетом скорости объекта и пули... если надо
			sVECTOR3D tmpLocation = tmpShip.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpShip.CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpShip.Location + tmpLocation;

			if ((tmpShip.Speed != 0.0f) &&
			    (WeaponType != 0) &&
			    // это не лучевое оружие, которое бьет сразу
			    (WeaponType != 11) && (WeaponType != 12) && (WeaponType != 14) &&
			    // это не ракеты...
			    (WeaponType != 16) && (WeaponType != 17) && (WeaponType != 18) && (WeaponType != 19)) {

				// находим, за какое время снаряд долетит до объекта сейчас
				sVECTOR3D TTT = WeponLocation - RealLocation;
				float ProjectileSpeed = GetProjectileSpeed(WeaponType);
				float CurrentDist = TTT.Length();
				float ObjCurrentTime = CurrentDist / ProjectileSpeed;

				// находим где будет объект, когда пройдет это время
				sVECTOR3D FutureLocation = tmpShip.Orientation ^ (tmpShip.Speed * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				RealLocation = RealLocation + FutureLocation;
			}

			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			// + учитываем тут Width
			float tmp1 = A * (RealLocation.x + tmpShip.OBB.Box[0].x) + B * (RealLocation.y + tmpShip.OBB.Box[0].y) + C * (RealLocation.z + tmpShip.OBB.Box[0].z) + D;
			float tmp2 = A * (RealLocation.x + tmpShip.OBB.Box[1].x) + B * (RealLocation.y + tmpShip.OBB.Box[1].y) + C * (RealLocation.z + tmpShip.OBB.Box[1].z) + D;
			float tmp3 = A * (RealLocation.x + tmpShip.OBB.Box[2].x) + B * (RealLocation.y + tmpShip.OBB.Box[2].y) + C * (RealLocation.z + tmpShip.OBB.Box[2].z) + D;
			float tmp4 = A * (RealLocation.x + tmpShip.OBB.Box[3].x) + B * (RealLocation.y + tmpShip.OBB.Box[3].y) + C * (RealLocation.z + tmpShip.OBB.Box[3].z) + D;
			float tmp5 = A * (RealLocation.x + tmpShip.OBB.Box[4].x) + B * (RealLocation.y + tmpShip.OBB.Box[4].y) + C * (RealLocation.z + tmpShip.OBB.Box[4].z) + D;
			float tmp6 = A * (RealLocation.x + tmpShip.OBB.Box[5].x) + B * (RealLocation.y + tmpShip.OBB.Box[5].y) + C * (RealLocation.z + tmpShip.OBB.Box[5].z) + D;
			float tmp7 = A * (RealLocation.x + tmpShip.OBB.Box[6].x) + B * (RealLocation.y + tmpShip.OBB.Box[6].y) + C * (RealLocation.z + tmpShip.OBB.Box[6].z) + D;
			float tmp8 = A * (RealLocation.x + tmpShip.OBB.Box[7].x) + B * (RealLocation.y + tmpShip.OBB.Box[7].y) + C * (RealLocation.z + tmpShip.OBB.Box[7].z) + D;


			if (!(((tmp1 > Width2) && (tmp2 > Width2) && (tmp3 > Width2) && (tmp4 > Width2) &&
			       (tmp5 > Width2) && (tmp6 > Width2) && (tmp7 > Width2) && (tmp8 > Width2)) ||
			      ((tmp1 < -Width2) && (tmp2 < -Width2) && (tmp3 < -Width2) && (tmp4 < -Width2) &&
			       (tmp5 < -Width2) && (tmp6 < -Width2) && (tmp7 < -Width2) && (tmp8 < -Width2)))) {
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (RealLocation.x + tmpShip.OBB.Box[0].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[0].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[0].z) + D2;
				tmp2 = A2 * (RealLocation.x + tmpShip.OBB.Box[1].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[1].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[1].z) + D2;
				tmp3 = A2 * (RealLocation.x + tmpShip.OBB.Box[2].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[2].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[2].z) + D2;
				tmp4 = A2 * (RealLocation.x + tmpShip.OBB.Box[3].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[3].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[3].z) + D2;
				tmp5 = A2 * (RealLocation.x + tmpShip.OBB.Box[4].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[4].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[4].z) + D2;
				tmp6 = A2 * (RealLocation.x + tmpShip.OBB.Box[5].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[5].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[5].z) + D2;
				tmp7 = A2 * (RealLocation.x + tmpShip.OBB.Box[6].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[6].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[6].z) + D2;
				tmp8 = A2 * (RealLocation.x + tmpShip.OBB.Box[7].x) + B2 * (RealLocation.y + tmpShip.OBB.Box[7].y) + C2 * (RealLocation.z + tmpShip.OBB.Box[7].z) + D2;

				if ((tmp1 > 0.0f) && (tmp2 > 0.0f) && (tmp3 > 0.0f) && (tmp4 > 0.0f) &&
				    (tmp5 > 0.0f) && (tmp6 > 0.0f) && (tmp7 > 0.0f) && (tmp8 > 0.0f)) {

					float TargetDist2TMP = A2 * RealLocation.x + B2 * RealLocation.y + C2 * RealLocation.z + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance < TargetDist2TMP) {
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...

						sVECTOR3D TargetAngleTMP;
						TargetLocation = RealLocation;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

						float m = TargetLocation.x - Location.x;
						float n = TargetLocation.y - Location.y;
						float p = TargetLocation.z - Location.z;
						if (NeedByWeaponOrientation) {
							m = TargetLocation.x - WeponLocation.x;
							n = TargetLocation.y - WeponLocation.y;
							p = TargetLocation.z - WeponLocation.z;
						}

						// поправки к существующим углам поворота оружия
						float sss1 = vw_sqrtf(m * m + n * n + p * p);
						float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
						TargetAngleTMP.x = CurrentObjectRotation.x;
						if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
							float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
							if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
								TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A * A + B * B + C * C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (NeedCenterOrientation &&
						    (sss1 != 0.0f) && (sss4 != 0.0f)) {
							float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
							if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
								TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;

						if ((Tdist > m * m + n * n * 5 + p * p) && (fabsf(TargetAngleTMP.x) < 45.0f)) {
							TargetAngle = TargetAngleTMP;
							Tdist = m * m + n * n * 5 + p * p;
							TargetLocked = true;
							TType = 1;
						}
					}
				}
			}
		}
	});

	// проверка по наземным объектам
	// не стрелять по "мирным" постойкам
	// !!! ВАЖНО
	// у всех наземных объектов ноль на уровне пола...
	ForEachGroundObject([&] (const cGroundObject &tmpGround, eGroundCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpGround) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpGround.ObjectStatus == eObjectStatus::Ally) || (tmpGround.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpGround.ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmpGround.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpGround.CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpGround.Location + tmpLocation;

			if ((tmpGround.Speed != 0.0f) &&
			    (WeaponType != 0) &&
			    // это не лучевое оружие, которое бьет сразу
			    (WeaponType != 11) && (WeaponType != 12) && (WeaponType != 14) &&
			    // это не ракеты...
			    (WeaponType != 16) && (WeaponType != 17) && (WeaponType != 18) && (WeaponType != 19)) {

				// находим, за какое время снаряд долетит до объекта сейчас
				sVECTOR3D TTT = WeponLocation-RealLocation;
				float ProjectileSpeed = GetProjectileSpeed(WeaponType);
				float CurrentDist = TTT.Length();
				float ObjCurrentTime = CurrentDist / ProjectileSpeed;

				// находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
				sVECTOR3D FutureLocation = tmpGround.Orientation ^ (tmpGround.Speed * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				RealLocation = RealLocation + FutureLocation;
			}

			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			float tmp1 = A * (tmpGround.Location.x + tmpGround.OBB.Box[0].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[0].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[0].z) + D;
			float tmp2 = A * (tmpGround.Location.x + tmpGround.OBB.Box[1].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[1].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[1].z) + D;
			float tmp3 = A * (tmpGround.Location.x + tmpGround.OBB.Box[2].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[2].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[2].z) + D;
			float tmp4 = A * (tmpGround.Location.x + tmpGround.OBB.Box[3].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[3].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[3].z) + D;
			float tmp5 = A * (tmpGround.Location.x + tmpGround.OBB.Box[4].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[4].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[4].z) + D;
			float tmp6 = A * (tmpGround.Location.x + tmpGround.OBB.Box[5].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[5].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[5].z) + D;
			float tmp7 = A * (tmpGround.Location.x + tmpGround.OBB.Box[6].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[6].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[6].z) + D;
			float tmp8 = A * (tmpGround.Location.x + tmpGround.OBB.Box[7].x) + B * (tmpGround.Location.y + tmpGround.OBB.Box[7].y) + C * (tmpGround.Location.z + tmpGround.OBB.Box[7].z) + D;

			if (!(((tmp1 > Width2) && (tmp2 > Width2) && (tmp3 > Width2) && (tmp4 > Width2) &&
			       (tmp5 > Width2) && (tmp6 > Width2) && (tmp7 > Width2) && (tmp8 > Width2)) ||
			      ((tmp1 < -Width2) && (tmp2 < -Width2) && (tmp3 < -Width2) && (tmp4 < -Width2) &&
			       (tmp5 < -Width2) && (tmp6 < -Width2) && (tmp7 < -Width2) && (tmp8 < -Width2)))) {
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[0].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[0].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[0].z) + D2;
				tmp2 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[1].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[1].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[1].z) + D2;
				tmp3 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[2].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[2].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[2].z) + D2;
				tmp4 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[3].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[3].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[3].z) + D2;
				tmp5 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[4].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[4].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[4].z) + D2;
				tmp6 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[5].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[5].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[5].z) + D2;
				tmp7 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[6].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[6].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[6].z) + D2;
				tmp8 = A2 * (tmpGround.Location.x + tmpGround.OBB.Box[7].x) + B2 * (tmpGround.Location.y + tmpGround.OBB.Box[7].y) + C2 * (tmpGround.Location.z + tmpGround.OBB.Box[7].z) + D2;

				if ((tmp1 > 0.0f) && (tmp2 > 0.0f) && (tmp3 > 0.0f) && (tmp4 > 0.0f) &&
				    (tmp5 > 0.0f) && (tmp6 > 0.0f) && (tmp7 > 0.0f) && (tmp8 > 0.0f)) {

					float TargetDist2TMP = A2 * RealLocation.x + B2 * RealLocation.y + C2 * RealLocation.z + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance < TargetDist2TMP) {
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...
						sVECTOR3D TargetAngleTMP;
						TargetLocation = RealLocation;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

						float m = TargetLocation.x - Location.x;
						float n = TargetLocation.y - Location.y;
						float p = TargetLocation.z - Location.z;
						if (NeedByWeaponOrientation) {
							m = TargetLocation.x - WeponLocation.x;
							n = TargetLocation.y - WeponLocation.y;
							p = TargetLocation.z - WeponLocation.z;
						}

						// поправки к существующим углам поворота оружия
						float sss1 = vw_sqrtf(m * m + n * n + p * p);
						float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
						TargetAngleTMP.x = CurrentObjectRotation.x;
						if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
							float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
							if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
								TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A * A + B * B + C * C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (NeedCenterOrientation)
							if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
								float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
								if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
									TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
							}

						TargetAngleTMP.z = CurrentObjectRotation.z;

						if ((TType < 2) && TargetLocked) {
							// только если в 5 раза ближе
							if ((Tdist > m * m + n * n * 5 + p * p) && (fabsf(TargetAngleTMP.x) < 45.0f)) {
								TargetAngle = TargetAngleTMP;
								Tdist = m * m + n * n + p * p;
								TargetLocked = true;
								TType = 2;
							}
						} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x) < 45.0f)) {
							TargetAngle = TargetAngleTMP;
							Tdist = m * m + n * n + p * p;
							TargetLocked = true;
							TType = 2;
						}
					}
				}
			}
		}
	});

	// проверка по космическим объектам
	ForEachSpaceObject([&] (const cSpaceObject &tmpSpace, eSpaceCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpSpace) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpSpace.ObjectStatus == eObjectStatus::Ally) || (tmpSpace.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpSpace.ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmpSpace.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpSpace.CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpSpace.Location + tmpLocation;

			// если нужно проверить
			if ((tmpSpace.Speed != 0.0f) &&
			    (WeaponType != 0) &&
			    // это не лучевое оружие, которое бьет сразу
			    (WeaponType != 11) && (WeaponType != 12) && (WeaponType != 14) &&
			    // это не ракеты...
			    (WeaponType != 16) && (WeaponType != 17) && (WeaponType != 18) && (WeaponType != 19)) {

				// находим, за какое время снаряд долетит до объекта сейчас
				sVECTOR3D TTT = WeponLocation - RealLocation;
				float ProjectileSpeed = GetProjectileSpeed(WeaponType);
				float CurrentDist = TTT.Length();
				float ObjCurrentTime = CurrentDist / ProjectileSpeed;

				// находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
				sVECTOR3D FutureLocation = tmpSpace.Orientation ^ (tmpSpace.Speed * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				RealLocation = RealLocation + FutureLocation;
			}

			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			float tmp1 = A * (RealLocation.x + tmpSpace.OBB.Box[0].x) + B * (RealLocation.y + tmpSpace.OBB.Box[0].y) + C * (RealLocation.z + tmpSpace.OBB.Box[0].z) + D;
			float tmp2 = A * (RealLocation.x + tmpSpace.OBB.Box[1].x) + B * (RealLocation.y + tmpSpace.OBB.Box[1].y) + C * (RealLocation.z + tmpSpace.OBB.Box[1].z) + D;
			float tmp3 = A * (RealLocation.x + tmpSpace.OBB.Box[2].x) + B * (RealLocation.y + tmpSpace.OBB.Box[2].y) + C * (RealLocation.z + tmpSpace.OBB.Box[2].z) + D;
			float tmp4 = A * (RealLocation.x + tmpSpace.OBB.Box[3].x) + B * (RealLocation.y + tmpSpace.OBB.Box[3].y) + C * (RealLocation.z + tmpSpace.OBB.Box[3].z) + D;
			float tmp5 = A * (RealLocation.x + tmpSpace.OBB.Box[4].x) + B * (RealLocation.y + tmpSpace.OBB.Box[4].y) + C * (RealLocation.z + tmpSpace.OBB.Box[4].z) + D;
			float tmp6 = A * (RealLocation.x + tmpSpace.OBB.Box[5].x) + B * (RealLocation.y + tmpSpace.OBB.Box[5].y) + C * (RealLocation.z + tmpSpace.OBB.Box[5].z) + D;
			float tmp7 = A * (RealLocation.x + tmpSpace.OBB.Box[6].x) + B * (RealLocation.y + tmpSpace.OBB.Box[6].y) + C * (RealLocation.z + tmpSpace.OBB.Box[6].z) + D;
			float tmp8 = A * (RealLocation.x + tmpSpace.OBB.Box[7].x) + B * (RealLocation.y + tmpSpace.OBB.Box[7].y) + C * (RealLocation.z + tmpSpace.OBB.Box[7].z) + D;

			if (!(((tmp1 > Width2) && (tmp2 > Width2) && (tmp3 > Width2) && (tmp4 > Width2) &&
			       (tmp5 > Width2) && (tmp6 > Width2) && (tmp7 > Width2) && (tmp8 > Width2)) ||
			      ((tmp1 < -Width2) && (tmp2 < -Width2) && (tmp3 < -Width2) && (tmp4 < -Width2) &&
			       (tmp5 < -Width2) && (tmp6 < -Width2) && (tmp7 < -Width2) && (tmp8 < -Width2)))) {
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (RealLocation.x + tmpSpace.OBB.Box[0].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[0].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[0].z) + D2;
				tmp2 = A2 * (RealLocation.x + tmpSpace.OBB.Box[1].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[1].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[1].z) + D2;
				tmp3 = A2 * (RealLocation.x + tmpSpace.OBB.Box[2].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[2].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[2].z) + D2;
				tmp4 = A2 * (RealLocation.x + tmpSpace.OBB.Box[3].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[3].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[3].z) + D2;
				tmp5 = A2 * (RealLocation.x + tmpSpace.OBB.Box[4].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[4].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[4].z) + D2;
				tmp6 = A2 * (RealLocation.x + tmpSpace.OBB.Box[5].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[5].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[5].z) + D2;
				tmp7 = A2 * (RealLocation.x + tmpSpace.OBB.Box[6].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[6].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[6].z) + D2;
				tmp8 = A2 * (RealLocation.x + tmpSpace.OBB.Box[7].x) + B2 * (RealLocation.y + tmpSpace.OBB.Box[7].y) + C2 * (RealLocation.z + tmpSpace.OBB.Box[7].z) + D2;

				if ((tmp1 > 0.0f) && (tmp2 > 0.0f) && (tmp3 > 0.0f) && (tmp4 > 0.0f) &&
				    (tmp5 > 0.0f) && (tmp6 > 0.0f) && (tmp7 > 0.0f) && (tmp8 > 0.0f)) {

					float TargetDist2TMP = A2 * RealLocation.x + B2 * RealLocation.y + C2 * RealLocation.z + D2;

					// проверяем, чтобы объект находился не ближе чем MinDistance
					if (MinDistance < TargetDist2TMP) {
						// выбираем объект, так, чтобы он был наиболее длижайшим,
						// идущим по нашему курсу...
						sVECTOR3D TargetAngleTMP;
						TargetLocation = RealLocation;

						// находим угол между плоскостью и прямой
						float A3, B3, C3, D3;
						vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

						float m = TargetLocation.x - Location.x;
						float n = TargetLocation.y - Location.y;
						float p = TargetLocation.z - Location.z;
						if (NeedByWeaponOrientation) {
							m = TargetLocation.x - WeponLocation.x;
							n = TargetLocation.y - WeponLocation.y;
							p = TargetLocation.z - WeponLocation.z;
						}

						// поправки к существующим углам поворота оружия
						float sss1 = vw_sqrtf(m * m + n * n + p * p);
						float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
						TargetAngleTMP.x = CurrentObjectRotation.x;
						if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
							float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
							if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
								TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
						}

						float sss4 = vw_sqrtf(A * A + B * B + C * C);
						TargetAngleTMP.y = CurrentObjectRotation.y;
						if (NeedCenterOrientation &&
						    (sss1 != 0.0f) && (sss4 != 0.0f)) {
							float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
							if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
								TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
						}

						TargetAngleTMP.z = CurrentObjectRotation.z;

						if ((TType < 3) && TargetLocked) {
							// только если в 10 раза ближе
							if ((Tdist / 10.0f > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x) < 45.0f)) {
								TargetAngle = TargetAngleTMP;
								Tdist = m * m + n * n + p * p;
								TargetLocked = true;
								TType = 3;
							}
						} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x) < 45.0f)) {
							TargetAngle = TargetAngleTMP;
							Tdist = m * m + n * n + p * p;
							TargetLocked = true;
							TType = 3;
						}
					}
				}
			}
		}
	});

	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
		NeedAngle = TargetAngle;
}

//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для космических кораблей противника
// !! почти полная копия как наведение на у турелей, но без учета выше-ниже противника
//-----------------------------------------------------------------------------
void GetEnemyShipOnTargetOrientateion(eObjectStatus ObjectStatus, // статус объекта, который целится
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

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip, eShipCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpShip) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip.ObjectStatus == eObjectStatus::Ally) || (tmpShip.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip.ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmpShip.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpShip.CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpShip.Location + tmpLocation;

			// учитываем, если лазер - наводить не надо
			if (WeaponType != 110) {
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
				sVECTOR3D CamPosTTT(0.0f, 0.0f, 0.0f);
				if (tmpShip.ObjectStatus == eObjectStatus::Player)
					CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				sVECTOR3D PossibleRealLocation = RealLocation + FutureLocation + CamPosTTT;

				TTT = Location - PossibleRealLocation;
				float PossibleDist = TTT.Length();
				float PoprTime = PossibleDist / ProjectileSpeed;

				FutureLocation = tmpShip.Orientation ^ (tmpShip.Speed * PoprTime);
				// учитываем камеру...
				CamPosTTT = sVECTOR3D{0.0f, 0.0f, 0.0f};
				if (tmpShip.ObjectStatus == eObjectStatus::Player)
					CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * PoprTime);

				RealLocation = RealLocation + FutureLocation + CamPosTTT;
			}

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
	});

	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked) {
		// находим угол между плоскостью и прямой
		float m = TargetLocation.x - Location.x;
		float n = TargetLocation.y - Location.y;
		float p = TargetLocation.z - Location.z;

		// поправки к существующим углам поворота оружия
		float sss1 = m * m + n * n + p * p;
		float sss2 = A * A + B * B + C * C;
		if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
			float ttt = (A * m + B * n + C * p) / (vw_sqrtf(sss1) * vw_sqrtf(sss2));
			if ((ttt >= -1.0f) && (ttt <= 1.0f))
				NeedAngle.x = CurrentObjectRotation.x - asinf(ttt) * 57.32f;
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
			vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location+PointUp, Location+PointRight);

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
						NeedAngle.y = 180.0f - asinf(ttt) * 57.32f;
				}
			} else {
				// находим угол поворота
				sss1 = vw_sqrtf(m * m + n * n + p * p);
				float sss3 = vw_sqrtf(A2 * A2 + B2 * B2 + C2 * C2);
				if ((sss1 != 0.0f) && (sss3 != 0.0f)) {
					float ttt = (A2 * m + B2 * n + C2 * p) / (sss1 * sss3);
					if ((ttt >= -1.0f) && (ttt <= 1.0f)) {
						NeedAngle.y = asinf(ttt) * 57.32f;
						if (NeedAngle.y < 0.0f)
							NeedAngle.y += 360.0f;
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для турелей наземных объектов
//-----------------------------------------------------------------------------
bool GetTurretOnTargetOrientateion(eObjectStatus ObjectStatus, // статус объекта, который целится
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

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip, eShipCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpShip) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip.ObjectStatus == eObjectStatus::Ally) || (tmpShip.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip.ObjectStatus == eObjectStatus::Enemy)))) {

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
				NeedAngle.x = CurrentObjectRotation.x + asinf(ttt) * 57.32f;
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
						NeedAngle.y = 180.0f - asinf(ttt) * 57.32f;
				}
			} else {
				// находим угол поворота
				sss1 = vw_sqrtf(m * m + n * n + p * p);
				float sss3 = vw_sqrtf(A2 * A2 + B2 * B2 + C2 * C2);
				if ((sss1 != 0.0f) && (sss3 != 0.0f)) {
					float ttt = (A2 * m + B2 * n + C2 * p) / (sss1 * sss3);
					if ((ttt >= -1.0f) && (ttt <= 1.0f)) {
						NeedAngle.y = asinf(ttt) * 57.32f;
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

//-----------------------------------------------------------------------------
// Получение угла поворота ракеты, торпеды или бомбы
//-----------------------------------------------------------------------------
cObject3D *GetMissileOnTargetOrientateion(eObjectStatus ObjectStatus, // статус объекта, который целится
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
	sVECTOR3D TargetAngle = CurrentObjectRotation;
	float Tdist{1000.0f * 1000.0f};

	// тип, кто заблокировал... чтобы не сбить с активных
	int TType{0};
	bool TargetLocked{false};

	float MinDistance{0.0f};

	// цель
	cObject3D *Target{nullptr};

	// проверка по снарядам, фларес
	ForEachProjectile([&] (const cProjectile &tmpProjectile, eProjectileCycle &UNUSED(Command)) {
		// только фларес
		if ((tmpProjectile.ProjectileType == 3) && NeedCheckCollision(tmpProjectile) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpProjectile.ObjectStatus == eObjectStatus::Ally) || (tmpProjectile.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpProjectile.ObjectStatus == eObjectStatus::Enemy)))) {

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpProjectile.Location.x + B2 * tmpProjectile.Location.y + C2 * tmpProjectile.Location.z + D2;
			if (tmp1 > 0.0f) {
				float TargetDist2TMP = A2 * tmpProjectile.Location.x + B2 * tmpProjectile.Location.y + C2 * tmpProjectile.Location.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...

					sVECTOR3D TargetAngleTMP;
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
					TargetAngleTMP.x = CurrentObjectRotation.x;
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					TargetAngleTMP.y = CurrentObjectRotation.y;
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
					}

					TargetAngleTMP.z = CurrentObjectRotation.z;

					if (Tdist > m * m + n * n + p * p) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 1;
						Target = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpProjectile));
					}
				}
			}
		}
	});

	// проверка по наземным объектам
	// не стрелять по "мирным" постойкам
	ForEachGroundObject([&] (const cGroundObject &tmpGround, eGroundCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpGround) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpGround.ObjectStatus == eObjectStatus::Ally) || (tmpGround.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpGround.ObjectStatus == eObjectStatus::Enemy)))) {
			sVECTOR3D tmpLocation = tmpGround.GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpGround.CurrentRotationMat); // поворачиваем в плоскость объекта
			TargetLocation = tmpGround.Location + tmpLocation;

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;
			if (tmp1 > 0.0f) {

				float TargetDist2TMP = A2 * TargetLocation.x + B2 * TargetLocation.y + C2 * TargetLocation.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...
					sVECTOR3D TargetAngleTMP;

					// находим угол между плоскостью и прямой
					float A3, B3, C3, D3;
					vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location + Orientation, Location + PointRight);

					float m = TargetLocation.x - Location.x;
					float n = TargetLocation.y - Location.y;
					float p = TargetLocation.z - Location.z;

					// поправки к существующим углам поворота оружия
					float sss1 = vw_sqrtf(m * m + n * n + p * p);
					float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
					TargetAngleTMP.x = CurrentObjectRotation.x;
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					TargetAngleTMP.y = CurrentObjectRotation.y;
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
					}

					TargetAngleTMP.z = CurrentObjectRotation.z;

					if (TType < 2 && TargetLocked) {
						if ((Tdist / 3.0f > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
							TargetAngle = TargetAngleTMP;
							Tdist = m * m + n * n + p * p;
							TargetLocked = true;
							TType = 2;
							// FIXME we should use std::weak_ptr for target object instead
							Target = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpGround));
						}
					} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 2;
						// FIXME we should use std::weak_ptr for target object instead
						Target = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpGround));
					}
				}
			}
		}
	});

	ForEachSpaceShip([&] (const cSpaceShip &tmpShip, eShipCycle &UNUSED(Command)) {
		// проверка, чтобы не считать свой корабль
		if (NeedCheckCollision(tmpShip) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip.ObjectStatus == eObjectStatus::Ally) || (tmpShip.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip.ObjectStatus == eObjectStatus::Enemy)))) {

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpShip.Location.x + B2 * tmpShip.Location.y + C2 * tmpShip.Location.z + D2;
			if (tmp1 > 0.0f) {
				float TargetDist2TMP = A2 * tmpShip.Location.x + B2 * tmpShip.Location.y + C2 * tmpShip.Location.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...

					sVECTOR3D TargetAngleTMP;
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
					TargetAngleTMP.x = CurrentObjectRotation.x;
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					TargetAngleTMP.y = CurrentObjectRotation.y;
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
					}

					TargetAngleTMP.z = CurrentObjectRotation.z;

					if ((TType < 3) && TargetLocked) {
						// только если в 6 раза ближе
						if ((Tdist / 6.0f > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
							TargetAngle = TargetAngleTMP;
							Tdist = m * m + n * n + p * p;
							TargetLocked = true;
							TType = 3;
							// FIXME we should use std::weak_ptr for target object instead
							Target =  const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpShip));
						}
					} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 3;
						// FIXME we should use std::weak_ptr for target object instead
						Target =  const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpShip));
					}
				}
			}
		}
	});

	// проверка по космическим объектам
	ForEachSpaceObject([&] (const cSpaceObject &tmpSpace, eSpaceCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpSpace) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpSpace.ObjectStatus == eObjectStatus::Ally) || (tmpSpace.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpSpace.ObjectStatus == eObjectStatus::Enemy))) &&
		    (tmpSpace.ObjectType != eObjectType::SpaceDebris)) {
					// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpSpace.Location.x  + B2 * tmpSpace.Location.y  + C2 * tmpSpace.Location.z + D2;
			if (tmp1 > 0.0f) {

				float TargetDist2TMP = A2 * tmpSpace.Location.x + B2 * tmpSpace.Location.y + C2 * tmpSpace.Location.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...
					sVECTOR3D TargetAngleTMP;
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
					TargetAngleTMP.x = CurrentObjectRotation.x;
					if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
						float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
						if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
							TargetAngleTMP.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
					}

					float sss4 = vw_sqrtf(A * A + B * B + C * C);
					TargetAngleTMP.y = CurrentObjectRotation.y;
					if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
						float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
						if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
							TargetAngleTMP.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
					}

					TargetAngleTMP.z = CurrentObjectRotation.z;

					if ((TType < 4) && TargetLocked) {
						// только если в 10 раза ближе
						if ((Tdist / 10.0f > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
							TargetAngle = TargetAngleTMP;
							Tdist = m * m + n * n + p * p;
							TargetLocked = true;
							TType = 4;
							// FIXME we should use std::weak_ptr for target object instead
							Target = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpSpace));
						}
					} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 4;
						// FIXME we should use std::weak_ptr for target object instead
						Target = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpSpace));
					}
				}
			}
		}
	});

	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
		NeedAngle = TargetAngle;

	return Target;
}

//-----------------------------------------------------------------------------
// Получаем углы поворота для ракеты наведенной на цель
//-----------------------------------------------------------------------------
bool GetMissileOnTargetOrientateion(const sVECTOR3D &Location, // положение точки относительно которой будем наводить
				    const sVECTOR3D &CurrentObjectRotation, // текущие углы объекта
				    const float (&RotationMatrix)[9], // матрица вращения объекта
				    cObject3D *TargetObject, // объект на который прицеливаемся
				    sVECTOR3D &NeedAngle) // нужные углы, чтобы получить нужное направление
{
	if (!TargetObject)
		return false;

	// получаем точки для создания плоскости
	sVECTOR3D Orientation{0.0f, 0.0f, 1.0f};
	vw_Matrix33CalcPoint(Orientation, RotationMatrix);
	sVECTOR3D PointUp{0.0f, 1.0f, 0.0f};
	vw_Matrix33CalcPoint(PointUp, RotationMatrix);
	sVECTOR3D PointRight{1.0f, 0.0f, 0.0f};
	vw_Matrix33CalcPoint(PointRight, RotationMatrix);

	// находим плоскость, вертикальную
	float A, B, C, D;
	vw_GetPlaneABCD(A, B, C, D, Location, Location + Orientation, Location + PointUp);

	// получаем вертикальную плоскость 2 (отсечения перед-зад)
	float A2, B2, C2, D2;
	vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + PointRight, Location + PointUp);

	// для выбора - точка, куда целимся + расстояние до нее (квадрат расстояния)
	sVECTOR3D TargetLocation = Location;
	NeedAngle = CurrentObjectRotation;

	// проверяем, спереди или сзади стоит противник
	float tmp1 = A2 * TargetObject->Location.x + B2 * TargetObject->Location.y + C2 * TargetObject->Location.z + D2;
	if (tmp1 > 0.0f) {
		sVECTOR3D tmpLocation = TargetObject->GeometryCenter;
		vw_Matrix33CalcPoint(tmpLocation, TargetObject->CurrentRotationMat); // поворачиваем в плоскость объекта
		TargetLocation = TargetObject->Location + tmpLocation;

		// находим угол между плоскостью и прямой
		float A3, B3, C3, D3;
		vw_GetPlaneABCD(A3, B3, C3, D3, Location, Location+Orientation, Location + PointRight);

		float m = TargetLocation.x - Location.x;
		float n = TargetLocation.y - Location.y;
		float p = TargetLocation.z - Location.z;

		// поправки к существующим углам поворота оружия
		float sss1 = vw_sqrtf(m * m + n * n + p * p);
		float sss2 = vw_sqrtf(A3 * A3 + B3 * B3 + C3 * C3);
		NeedAngle.x = CurrentObjectRotation.x;
		if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
			float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
			if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
				NeedAngle.x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
		}

		float sss4 = vw_sqrtf(A * A + B * B + C * C);
		NeedAngle.y = CurrentObjectRotation.y;
		if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
			float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
			if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
				NeedAngle.y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Проверяем где по отношению ракеты находится объект
//-----------------------------------------------------------------------------
static bool GetMissileTargetPosition(cObject3D *TargetObject,
				     const sVECTOR3D &Location, // положение точки относительно которой будем наводить
				     const float (&RotationMatrix)[9]) // матрица вращения объекта
{
	// (!) TargetObject должен существовать, до вызова этой функции проверять это, в этой функции проверки не делаем

	sVECTOR3D PointUp(0.0f, 1.0f, 0.0f);
	vw_Matrix33CalcPoint(PointUp, RotationMatrix);
	sVECTOR3D PointRight(1.0f, 0.0f, 0.0f);
	vw_Matrix33CalcPoint(PointRight, RotationMatrix);

	// получаем вертикальную плоскость (отсечения перед-зад)
	float A2, B2, C2, D2;
	vw_GetPlaneABCD(A2, B2, C2, D2, Location, Location + PointRight, Location + PointUp);

	float tmp1 = A2 * TargetObject->Location.x + B2 * TargetObject->Location.y + C2 * TargetObject->Location.z + D2;
	if (tmp1 > 0.0f)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Проверяем статус цели для ракет, жива она еще или нет, и где по отношению ракеты находится
//-----------------------------------------------------------------------------
// FIXME we should use std::weak_ptr for target object instead
bool GetMissileTargetStatus(cObject3D *TargetObject,
			    const sVECTOR3D &Location, // положение точки относительно которой будем наводить
			    const float (&RotationMatrix)[9]) // матрица вращения объекта
{
	if (!TargetObject)
		return false;

	bool ObjectFound{false};
	ForEachProjectile([&TargetObject, &ObjectFound] (const cProjectile &tmpProjectile, eProjectileCycle &Command) {
		if (&tmpProjectile == TargetObject) {
			ObjectFound = true;
			Command = eProjectileCycle::Break;
		}
	});
	if (ObjectFound)
		return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);

	ForEachGroundObject([&TargetObject, &ObjectFound] (const cGroundObject &tmpGround, eGroundCycle &Command) {
		if (&tmpGround == TargetObject) {
			ObjectFound = true;
			Command = eGroundCycle::Break;
		}
	});
	if (ObjectFound)
		return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);

	ForEachSpaceShip([&TargetObject, &ObjectFound] (const cSpaceShip &tmpShip, eShipCycle &Command) {
		if (&tmpShip == TargetObject) {
			ObjectFound = true;
			Command = eShipCycle::Break;
		}
	});
	if (ObjectFound)
		return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);

	ForEachSpaceObject([&TargetObject, &ObjectFound] (const cSpaceObject &tmpSpace, eSpaceCycle &Command) {
		if (&tmpSpace == TargetObject) {
			ObjectFound = true;
			Command = eSpaceCycle::Break;
		}
	});
	if (ObjectFound)
		return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);

	return false;
}

//-----------------------------------------------------------------------------
// Получение положения ближайшего врага, для мин
//-----------------------------------------------------------------------------
cObject3D *GetCloserTargetPosition(eObjectStatus ObjectStatus, // статус объекта, который целится
				   const sVECTOR3D &Location) // положение точки относительно которой будем наводить
{
	// результат
	cObject3D *Res{nullptr};
	// пока ставим отрицательный, т.е. вообще ничего нет
	float MinRatius2{-1.0f};

	// перебираем только корабли...
	ForEachSpaceShip([&] (const cSpaceShip &tmpShip, eShipCycle &UNUSED(Command)) {
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpShip) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpShip.ObjectStatus == eObjectStatus::Ally) || (tmpShip.ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpShip.ObjectStatus == eObjectStatus::Enemy)))) {
			// получаем квадрат радиуса
			float MinRatius2TMP = (tmpShip.Location.x - Location.x) * (tmpShip.Location.x - Location.x) +
					      (tmpShip.Location.y - Location.y) * (tmpShip.Location.y - Location.y) +
					      (tmpShip.Location.z - Location.z) * (tmpShip.Location.z - Location.z);

			if (MinRatius2 < 0.0f) {
				MinRatius2 = MinRatius2TMP;
				// FIXME we should use std::weak_ptr for target object instead
				Res = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpShip));
			} else if (MinRatius2TMP < MinRatius2) {
				MinRatius2 = MinRatius2TMP;
				// FIXME we should use std::weak_ptr for target object instead
				Res = const_cast<cObject3D*>(static_cast<const cObject3D*>(&tmpShip));
			}
		}
	});

	return Res;
}

/*
 * Setup shader's uniform location.
 */
static bool SetupShaderUniformLocation(std::weak_ptr<cGLSL> &Shader,
				       const std::string &ShaderName,
				       const std::vector<std::string> &UniformLocationNameArray)
{
	// find and store uniform location for all shaders, we don't really need
	// store internal storage number for uniforms usage, since we load them
	// one-by-one, and we know sequence, we could use 0-1-2-3-4 numbers directly
	for (const auto &tmpName : UniformLocationNameArray) {
		if (vw_FindShaderUniformLocation(Shader, tmpName) < 0) {
			std::cerr << __func__ << "(): " << "failed to find uniform location " << tmpName
				  << " in shader " << ShaderName << ".\n";
			return false;
		}
	}
	return true;
}

/*
 * Setup shader.
 */
static bool SetupShader(std::weak_ptr<cGLSL> &Shader,
			const std::string &ShaderName,
			const std::vector<std::string> &UniformLocationNameArray)
{
	Shader = vw_FindShaderByName(ShaderName);
	if (Shader.expired()) {
		std::cerr << __func__ << "(): " << "failed to find " << ShaderName << " shader.\n";
		return false;
	}
	if (!SetupShaderUniformLocation(Shader, ShaderName, UniformLocationNameArray))
		return false;

	return true;
}

/*
 * Setup shaders.
 */
bool SetupObject3DShaders()
{
	const std::vector<std::string> GLSLShaderType1UniformLocationNames{
		{"Texture1"},
		{"Texture2"},
		{"DirectLightCount"},
		{"PointLightCount"},
		{"NeedMultitexture"},
		{"NormalMap"},
		{"NeedNormalMapping"}
	};
	if (!SetupShader(GLSLShaderType1, "PerPixelLight", GLSLShaderType1UniformLocationNames))
		return false;

	const std::vector<std::string> GLSLShaderType2UniformLocationNames{
		{"Texture1"},
		{"DirectLightCount"},
		{"PointLightCount"},
		{"SpeedData1"},
		{"SpeedData2"}
	};
	if (!SetupShader(GLSLShaderType2, "PerPixelLight_Explosion", GLSLShaderType2UniformLocationNames))
		return false;

	const std::vector<std::string> GLSLShaderType3UniformLocationNames{
		{"Texture1"},
		{"Texture2"},
		{"DirectLightCount"},
		{"PointLightCount"},
		{"NeedMultitexture"},
		{"ShadowMap"},
		{"xPixelOffset"},
		{"yPixelOffset"},
		{"NormalMap"},
		{"NeedNormalMapping"},
		{"PCFMode"}
	};
	if (!SetupShader(GLSLShaderType3, "PerPixelLight_ShadowMap", GLSLShaderType3UniformLocationNames))
		return false;

	return true;
}

} // astromenace namespace
} // viewizard namespace
