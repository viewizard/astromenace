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

namespace viewizard {

// FIXME should be fixed, don't allow global scope interaction for local variables
extern cSpaceShip *StartSpaceShip;
extern cSpaceShip *EndSpaceShip;
extern cProjectile *StartProjectile;
extern cProjectile *EndProjectile;
extern cGroundObject *StartGroundObject;
extern cGroundObject *EndGroundObject;
extern cSpaceObject *StartSpaceObject;
extern cSpaceObject *EndSpaceObject;
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
bool NeedCheckCollision(cObject3D *Object3D)
{
	// don't use 'default' case here, we need compiler's warning if anyone was missed
	switch (Object3D->ObjectType) {
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

	case eObjectType::Asteroids:
		return true;

	case eObjectType::ShipPart:
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
void LoadObjectData(const std::string &FileName, cObject3D *Object3D)
{
	// получение геометрии модели
	std::weak_ptr<sModel3D> Model = GetPreloadedModel3DAsset(FileName);
	auto sharedModel = Model.lock();
	if (!sharedModel)
		return;

	// берем то, что нужно
	Object3D->GlobalVertexArray = sharedModel->GlobalVertexArray;
	Object3D->GlobalVertexArrayCount = sharedModel->GlobalVertexArrayCount;
	Object3D->GlobalVBO = sharedModel->GlobalVBO;
	Object3D->GlobalIndexArray = sharedModel->GlobalIndexArray;
	Object3D->GlobalIndexArrayCount = sharedModel->GlobalIndexArrayCount;
	Object3D->GlobalIBO = sharedModel->GlobalIBO;
	Object3D->GlobalVAO = sharedModel->GlobalVAO;
	Object3D->Model3DBlocks = sharedModel->Model3DBlocks;

	// резервируем память для текстур
	Object3D->Texture.resize(Object3D->Model3DBlocks.size(), 0);
	Object3D->TextureIllum.resize(Object3D->Model3DBlocks.size(), 0);
	Object3D->NormalMap.resize(Object3D->Model3DBlocks.size(), 0);

	// metadata
	Object3D->AABB = sharedModel->AABB;
	Object3D->OBB = sharedModel->OBB;
	Object3D->HitBB = sharedModel->HitBB;
	Object3D->GeometryCenter = sharedModel->GeometryCenter;
	Object3D->Radius = sharedModel->Radius;
	Object3D->Width = sharedModel->Width;
	Object3D->Length = sharedModel->Length;
	Object3D->Height = sharedModel->Height;
}

//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для космических кораблей
//-----------------------------------------------------------------------------
void GetShipOnTargetOrientateion(eObjectStatus ObjectStatus, // статус объекта, который целится
				 sVECTOR3D Location, // положение точки относительно которой будем наводить
				 sVECTOR3D CurrentObjectRotation, // текущие углы объекта
				 float MinDistance, // минимальное расстояние, с которого начинаем прицеливание
				 float (&RotationMatrix)[9], // матрица вращения объекта
				 sVECTOR3D *NeedAngle,// нужные углы, чтобы получить нужное направление
				 float Width, // ширина объекта
				 bool NeedCenterOrientation, // нужен доворот на центр
				 bool NeedByWeaponOrientation, // нужно делать доворот с учетом положения орудия
				 sVECTOR3D WeponLocation,
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

	cSpaceShip *tmp = StartSpaceShip;
	while (tmp) {
		cSpaceShip *tmpShip2 = tmp->Next;
		// проверка, чтобы не считать свой корабль
		if ((NeedCheckCollision(tmp)) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmp->ObjectStatus == eObjectStatus::Ally) || (tmp->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmp->ObjectStatus == eObjectStatus::Enemy)))) {
			// находим настоящую точку попадания с учетом скорости объекта и пули... если надо
			sVECTOR3D tmpLocation = tmp->GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmp->CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmp->Location + tmpLocation;

			if ((tmp->Speed != 0.0f) &&
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
				sVECTOR3D FutureLocation = tmp->Orientation ^ (tmp->Speed * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				RealLocation = RealLocation + FutureLocation;
			}

			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			// + учитываем тут Width
			float tmp1 = A * (RealLocation.x + tmp->OBB.Box[0].x) + B * (RealLocation.y + tmp->OBB.Box[0].y) + C * (RealLocation.z + tmp->OBB.Box[0].z) + D;
			float tmp2 = A * (RealLocation.x + tmp->OBB.Box[1].x) + B * (RealLocation.y + tmp->OBB.Box[1].y) + C * (RealLocation.z + tmp->OBB.Box[1].z) + D;
			float tmp3 = A * (RealLocation.x + tmp->OBB.Box[2].x) + B * (RealLocation.y + tmp->OBB.Box[2].y) + C * (RealLocation.z + tmp->OBB.Box[2].z) + D;
			float tmp4 = A * (RealLocation.x + tmp->OBB.Box[3].x) + B * (RealLocation.y + tmp->OBB.Box[3].y) + C * (RealLocation.z + tmp->OBB.Box[3].z) + D;
			float tmp5 = A * (RealLocation.x + tmp->OBB.Box[4].x) + B * (RealLocation.y + tmp->OBB.Box[4].y) + C * (RealLocation.z + tmp->OBB.Box[4].z) + D;
			float tmp6 = A * (RealLocation.x + tmp->OBB.Box[5].x) + B * (RealLocation.y + tmp->OBB.Box[5].y) + C * (RealLocation.z + tmp->OBB.Box[5].z) + D;
			float tmp7 = A * (RealLocation.x + tmp->OBB.Box[6].x) + B * (RealLocation.y + tmp->OBB.Box[6].y) + C * (RealLocation.z + tmp->OBB.Box[6].z) + D;
			float tmp8 = A * (RealLocation.x + tmp->OBB.Box[7].x) + B * (RealLocation.y + tmp->OBB.Box[7].y) + C * (RealLocation.z + tmp->OBB.Box[7].z) + D;


			if (!(((tmp1 > Width2) && (tmp2 > Width2) && (tmp3 > Width2) && (tmp4 > Width2) &&
			       (tmp5 > Width2) && (tmp6 > Width2) && (tmp7 > Width2) && (tmp8 > Width2)) ||
			      ((tmp1 < -Width2) && (tmp2 < -Width2) && (tmp3 < -Width2) && (tmp4 < -Width2) &&
			       (tmp5 < -Width2) && (tmp6 < -Width2) && (tmp7 < -Width2) && (tmp8 < -Width2)))) {
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (RealLocation.x + tmp->OBB.Box[0].x) + B2 * (RealLocation.y + tmp->OBB.Box[0].y) + C2 * (RealLocation.z + tmp->OBB.Box[0].z) + D2;
				tmp2 = A2 * (RealLocation.x + tmp->OBB.Box[1].x) + B2 * (RealLocation.y + tmp->OBB.Box[1].y) + C2 * (RealLocation.z + tmp->OBB.Box[1].z) + D2;
				tmp3 = A2 * (RealLocation.x + tmp->OBB.Box[2].x) + B2 * (RealLocation.y + tmp->OBB.Box[2].y) + C2 * (RealLocation.z + tmp->OBB.Box[2].z) + D2;
				tmp4 = A2 * (RealLocation.x + tmp->OBB.Box[3].x) + B2 * (RealLocation.y + tmp->OBB.Box[3].y) + C2 * (RealLocation.z + tmp->OBB.Box[3].z) + D2;
				tmp5 = A2 * (RealLocation.x + tmp->OBB.Box[4].x) + B2 * (RealLocation.y + tmp->OBB.Box[4].y) + C2 * (RealLocation.z + tmp->OBB.Box[4].z) + D2;
				tmp6 = A2 * (RealLocation.x + tmp->OBB.Box[5].x) + B2 * (RealLocation.y + tmp->OBB.Box[5].y) + C2 * (RealLocation.z + tmp->OBB.Box[5].z) + D2;
				tmp7 = A2 * (RealLocation.x + tmp->OBB.Box[6].x) + B2 * (RealLocation.y + tmp->OBB.Box[6].y) + C2 * (RealLocation.z + tmp->OBB.Box[6].z) + D2;
				tmp8 = A2 * (RealLocation.x + tmp->OBB.Box[7].x) + B2 * (RealLocation.y + tmp->OBB.Box[7].y) + C2 * (RealLocation.z + tmp->OBB.Box[7].z) + D2;

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

		tmp = tmpShip2;
	}

	// проверка по наземным объектам
	// не стрелять по "мирным" постойкам
	// !!! ВАЖНО
	// у всех наземных объектов ноль на уровне пола...
	cGroundObject *tmpG = StartGroundObject;
	while (tmpG) {
		cGroundObject *tmpGround2 = tmpG->Next;

		// если по этому надо стрелять
		if (NeedCheckCollision(tmpG) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpG->ObjectStatus == eObjectStatus::Ally) || (tmpG->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpG->ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmpG->GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpG->CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpG->Location + tmpLocation;

			if ((tmpG->Speed != 0.0f) &&
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
				sVECTOR3D FutureLocation = tmpG->Orientation ^ (tmpG->Speed * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				RealLocation = RealLocation + FutureLocation;
			}

			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			float tmp1 = A * (tmpG->Location.x + tmpG->OBB.Box[0].x) + B * (tmpG->Location.y + tmpG->OBB.Box[0].y) + C * (tmpG->Location.z + tmpG->OBB.Box[0].z) + D;
			float tmp2 = A * (tmpG->Location.x + tmpG->OBB.Box[1].x) + B * (tmpG->Location.y + tmpG->OBB.Box[1].y) + C * (tmpG->Location.z + tmpG->OBB.Box[1].z) + D;
			float tmp3 = A * (tmpG->Location.x + tmpG->OBB.Box[2].x) + B * (tmpG->Location.y + tmpG->OBB.Box[2].y) + C * (tmpG->Location.z + tmpG->OBB.Box[2].z) + D;
			float tmp4 = A * (tmpG->Location.x + tmpG->OBB.Box[3].x) + B * (tmpG->Location.y + tmpG->OBB.Box[3].y) + C * (tmpG->Location.z + tmpG->OBB.Box[3].z) + D;
			float tmp5 = A * (tmpG->Location.x + tmpG->OBB.Box[4].x) + B * (tmpG->Location.y + tmpG->OBB.Box[4].y) + C * (tmpG->Location.z + tmpG->OBB.Box[4].z) + D;
			float tmp6 = A * (tmpG->Location.x + tmpG->OBB.Box[5].x) + B * (tmpG->Location.y + tmpG->OBB.Box[5].y) + C * (tmpG->Location.z + tmpG->OBB.Box[5].z) + D;
			float tmp7 = A * (tmpG->Location.x + tmpG->OBB.Box[6].x) + B * (tmpG->Location.y + tmpG->OBB.Box[6].y) + C * (tmpG->Location.z + tmpG->OBB.Box[6].z) + D;
			float tmp8 = A * (tmpG->Location.x + tmpG->OBB.Box[7].x) + B * (tmpG->Location.y + tmpG->OBB.Box[7].y) + C * (tmpG->Location.z + tmpG->OBB.Box[7].z) + D;

			if (!(((tmp1 > Width2) && (tmp2 > Width2) && (tmp3 > Width2) && (tmp4 > Width2) &&
			       (tmp5 > Width2) && (tmp6 > Width2) && (tmp7 > Width2) && (tmp8 > Width2)) ||
			      ((tmp1 < -Width2) && (tmp2 < -Width2) && (tmp3 < -Width2) && (tmp4 < -Width2) &&
			       (tmp5 < -Width2) && (tmp6 < -Width2) && (tmp7 < -Width2) && (tmp8 < -Width2)))) {
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (tmpG->Location.x + tmpG->OBB.Box[0].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[0].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[0].z) + D2;
				tmp2 = A2 * (tmpG->Location.x + tmpG->OBB.Box[1].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[1].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[1].z) + D2;
				tmp3 = A2 * (tmpG->Location.x + tmpG->OBB.Box[2].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[2].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[2].z) + D2;
				tmp4 = A2 * (tmpG->Location.x + tmpG->OBB.Box[3].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[3].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[3].z) + D2;
				tmp5 = A2 * (tmpG->Location.x + tmpG->OBB.Box[4].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[4].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[4].z) + D2;
				tmp6 = A2 * (tmpG->Location.x + tmpG->OBB.Box[5].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[5].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[5].z) + D2;
				tmp7 = A2 * (tmpG->Location.x + tmpG->OBB.Box[6].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[6].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[6].z) + D2;
				tmp8 = A2 * (tmpG->Location.x + tmpG->OBB.Box[7].x) + B2 * (tmpG->Location.y + tmpG->OBB.Box[7].y) + C2 * (tmpG->Location.z + tmpG->OBB.Box[7].z) + D2;

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

		tmpG = tmpGround2;
	}

	// проверка по космическим объектам
	cSpaceObject *tmpS = StartSpaceObject;
	while (tmpS) {
		cSpaceObject *tmpSpace2 = tmpS->Next;

		// если по этому надо стрелять
		if (NeedCheckCollision(tmpS) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpS->ObjectStatus == eObjectStatus::Ally) || (tmpS->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpS->ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmpS->GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpS->CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmpS->Location + tmpLocation;

			// если нужно проверить
			if ((tmpS->Speed != 0.0f) &&
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
				sVECTOR3D FutureLocation = tmpS->Orientation ^ (tmpS->Speed * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				RealLocation = RealLocation + FutureLocation;
			}

			// проверяем, если с одной стороны все точки - значит мимо, если нет - попали :)
			float tmp1 = A * (RealLocation.x + tmpS->OBB.Box[0].x) + B * (RealLocation.y + tmpS->OBB.Box[0].y) + C * (RealLocation.z + tmpS->OBB.Box[0].z) + D;
			float tmp2 = A * (RealLocation.x + tmpS->OBB.Box[1].x) + B * (RealLocation.y + tmpS->OBB.Box[1].y) + C * (RealLocation.z + tmpS->OBB.Box[1].z) + D;
			float tmp3 = A * (RealLocation.x + tmpS->OBB.Box[2].x) + B * (RealLocation.y + tmpS->OBB.Box[2].y) + C * (RealLocation.z + tmpS->OBB.Box[2].z) + D;
			float tmp4 = A * (RealLocation.x + tmpS->OBB.Box[3].x) + B * (RealLocation.y + tmpS->OBB.Box[3].y) + C * (RealLocation.z + tmpS->OBB.Box[3].z) + D;
			float tmp5 = A * (RealLocation.x + tmpS->OBB.Box[4].x) + B * (RealLocation.y + tmpS->OBB.Box[4].y) + C * (RealLocation.z + tmpS->OBB.Box[4].z) + D;
			float tmp6 = A * (RealLocation.x + tmpS->OBB.Box[5].x) + B * (RealLocation.y + tmpS->OBB.Box[5].y) + C * (RealLocation.z + tmpS->OBB.Box[5].z) + D;
			float tmp7 = A * (RealLocation.x + tmpS->OBB.Box[6].x) + B * (RealLocation.y + tmpS->OBB.Box[6].y) + C * (RealLocation.z + tmpS->OBB.Box[6].z) + D;
			float tmp8 = A * (RealLocation.x + tmpS->OBB.Box[7].x) + B * (RealLocation.y + tmpS->OBB.Box[7].y) + C * (RealLocation.z + tmpS->OBB.Box[7].z) + D;

			if (!(((tmp1 > Width2) && (tmp2 > Width2) && (tmp3 > Width2) && (tmp4 > Width2) &&
			       (tmp5 > Width2) && (tmp6 > Width2) && (tmp7 > Width2) && (tmp8 > Width2)) ||
			      ((tmp1 < -Width2) && (tmp2 < -Width2) && (tmp3 < -Width2) && (tmp4 < -Width2) &&
			       (tmp5 < -Width2) && (tmp6 < -Width2) && (tmp7 < -Width2) && (tmp8 < -Width2)))) {
				// проверяем, спереди или сзади стоит противник
				tmp1 = A2 * (RealLocation.x + tmpS->OBB.Box[0].x) + B2 * (RealLocation.y + tmpS->OBB.Box[0].y) + C2 * (RealLocation.z + tmpS->OBB.Box[0].z)  + D2;
				tmp2 = A2 * (RealLocation.x + tmpS->OBB.Box[1].x) + B2 * (RealLocation.y + tmpS->OBB.Box[1].y) + C2 * (RealLocation.z + tmpS->OBB.Box[1].z)  + D2;
				tmp3 = A2 * (RealLocation.x + tmpS->OBB.Box[2].x) + B2 * (RealLocation.y + tmpS->OBB.Box[2].y) + C2 * (RealLocation.z + tmpS->OBB.Box[2].z)  + D2;
				tmp4 = A2 * (RealLocation.x + tmpS->OBB.Box[3].x) + B2 * (RealLocation.y + tmpS->OBB.Box[3].y) + C2 * (RealLocation.z + tmpS->OBB.Box[3].z)  + D2;
				tmp5 = A2 * (RealLocation.x + tmpS->OBB.Box[4].x) + B2 * (RealLocation.y + tmpS->OBB.Box[4].y) + C2 * (RealLocation.z + tmpS->OBB.Box[4].z)  + D2;
				tmp6 = A2 * (RealLocation.x + tmpS->OBB.Box[5].x) + B2 * (RealLocation.y + tmpS->OBB.Box[5].y) + C2 * (RealLocation.z + tmpS->OBB.Box[5].z)  + D2;
				tmp7 = A2 * (RealLocation.x + tmpS->OBB.Box[6].x) + B2 * (RealLocation.y + tmpS->OBB.Box[6].y) + C2 * (RealLocation.z + tmpS->OBB.Box[6].z)  + D2;
				tmp8 = A2 * (RealLocation.x + tmpS->OBB.Box[7].x) + B2 * (RealLocation.y + tmpS->OBB.Box[7].y) + C2 * (RealLocation.z + tmpS->OBB.Box[7].z)  + D2;

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

		tmpS = tmpSpace2;
	}

	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
		(*NeedAngle) = TargetAngle;
}

//-----------------------------------------------------------------------------
// Получение угла поворота оружия на врага для космических кораблей противника
// !! почти полная копия как наведение на у турелей, но без учета выше-ниже противника
//-----------------------------------------------------------------------------
void GetEnemyShipOnTargetOrientateion(eObjectStatus ObjectStatus, // статус объекта, который целится
				      sVECTOR3D Location, // положение точки относительно которой будем наводить
				      sVECTOR3D CurrentObjectRotation, // текущие углы объекта
				      float (&RotationMatrix)[9], // матрица вращения объекта
				      sVECTOR3D *NeedAngle, // нужные углы, чтобы получить нужное направление
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

	cSpaceShip *tmp = StartSpaceShip;
	while (tmp) {
		cSpaceShip *tmpShip2 = tmp->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmp) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmp->ObjectStatus == eObjectStatus::Ally) || (tmp->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmp->ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmp->GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmp->CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmp->Location + tmpLocation;

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
				sVECTOR3D FutureLocation = tmp->Orientation ^ (tmp->Speed * ObjCurrentTime);
				// учитываем камеру...
				sVECTOR3D CamPosTTT(0.0f, 0.0f, 0.0f);
				if (tmp->ObjectStatus == eObjectStatus::Player)
					CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * ObjCurrentTime);

				// находи точку по середине... это нам и надо... туда целимся...
				sVECTOR3D PossibleRealLocation = RealLocation + FutureLocation + CamPosTTT;

				TTT = Location - PossibleRealLocation;
				float PossibleDist = TTT.Length();
				float PoprTime = PossibleDist / ProjectileSpeed;

				FutureLocation = tmp->Orientation ^ (tmp->Speed * PoprTime);
				// учитываем камеру...
				CamPosTTT = sVECTOR3D(0.0f, 0.0f, 0.0f);
				if (tmp->ObjectStatus == eObjectStatus::Player)
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

		tmp = tmpShip2;
	}

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
				(*NeedAngle).x = CurrentObjectRotation.x - asinf(ttt) * 57.32f;
		}

		(*NeedAngle).z = CurrentObjectRotation.z;

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
						(*NeedAngle).y = 180.0f - asinf(ttt) * 57.32f;
				}
			} else {
				// находим угол поворота
				sss1 = vw_sqrtf(m * m + n * n + p * p);
				float sss3 = vw_sqrtf(A2 * A2 + B2 * B2 + C2 * C2);
				if ((sss1 != 0.0f) && (sss3 != 0.0f)) {
					float ttt = (A2 * m + B2 * n + C2 * p) / (sss1 * sss3);
					if ((ttt >= -1.0f) && (ttt <= 1.0f)) {
						(*NeedAngle).y = asinf(ttt) * 57.32f;
						if ((*NeedAngle).y < 0.0f)
							(*NeedAngle).y += 360.0f;
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
				   sVECTOR3D Location, // положение точки относительно которой будем наводить
				   sVECTOR3D CurrentObjectRotation, // текущие углы объекта
				   float (&RotationMatrix)[9], // матрица вращения объекта
				   sVECTOR3D *NeedAngle, // нужные углы, чтобы получить нужное направление
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

	cSpaceShip *tmp = StartSpaceShip;
	while (tmp) {
		cSpaceShip *tmpShip2 = tmp->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmp) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmp->ObjectStatus == eObjectStatus::Ally) || (tmp->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmp->ObjectStatus == eObjectStatus::Enemy)))) {

			sVECTOR3D tmpLocation = tmp->GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmp->CurrentRotationMat); // поворачиваем в плоскость объекта
			sVECTOR3D RealLocation = tmp->Location + tmpLocation;

			// находим, за какое время снаряд долетит до объекта сейчас
			sVECTOR3D TTT = Location - RealLocation;
			float ProjectileSpeed = GetProjectileSpeed(WeaponType);
			if (ObjectStatus == eObjectStatus::Enemy)
				ProjectileSpeed = ProjectileSpeed / GameEnemyWeaponPenalty;
			float CurrentDist = TTT.Length();
			float ObjCurrentTime = CurrentDist / ProjectileSpeed;

			// находим где будет объект, когда пройдет это время (+ сразу половину считаем!)
			sVECTOR3D FutureLocation = tmp->Orientation ^ (tmp->Speed * ObjCurrentTime);
			// учитываем камеру...
			sVECTOR3D CamPosTTT{0.0f,0.0f,0.0f};
			if (tmp->ObjectStatus == eObjectStatus::Player)
				CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * ObjCurrentTime);

			// находи точку по середине... это нам и надо... туда целимся...
			sVECTOR3D PossibleRealLocation = RealLocation + FutureLocation + CamPosTTT;

			TTT = Location - PossibleRealLocation;
			float PossibleDist = TTT.Length();
			float PoprTime = PossibleDist/ProjectileSpeed;

			FutureLocation = tmp->Orientation ^ (tmp->Speed * PoprTime);
			// учитываем камеру...
			CamPosTTT = sVECTOR3D{0.0f, 0.0f, 0.0f};
			if (tmp->ObjectStatus == eObjectStatus::Player)
				CamPosTTT = GameCameraMovement ^ (GameCameraGetSpeed() * PoprTime);

			RealLocation = RealLocation + FutureLocation + CamPosTTT;

			// проверяем, если все точки выше
			float tmp1 = A * (RealLocation.x + tmp->OBB.Box[0].x) + B * (RealLocation.y + tmp->OBB.Box[0].y) + C * (RealLocation.z + tmp->OBB.Box[0].z) + D;
			float tmp2 = A * (RealLocation.x + tmp->OBB.Box[1].x) + B * (RealLocation.y + tmp->OBB.Box[1].y) + C * (RealLocation.z + tmp->OBB.Box[1].z) + D;
			float tmp3 = A * (RealLocation.x + tmp->OBB.Box[2].x) + B * (RealLocation.y + tmp->OBB.Box[2].y) + C * (RealLocation.z + tmp->OBB.Box[2].z) + D;
			float tmp4 = A * (RealLocation.x + tmp->OBB.Box[3].x) + B * (RealLocation.y + tmp->OBB.Box[3].y) + C * (RealLocation.z + tmp->OBB.Box[3].z) + D;
			float tmp5 = A * (RealLocation.x + tmp->OBB.Box[4].x) + B * (RealLocation.y + tmp->OBB.Box[4].y) + C * (RealLocation.z + tmp->OBB.Box[4].z) + D;
			float tmp6 = A * (RealLocation.x + tmp->OBB.Box[5].x) + B * (RealLocation.y + tmp->OBB.Box[5].y) + C * (RealLocation.z + tmp->OBB.Box[5].z) + D;
			float tmp7 = A * (RealLocation.x + tmp->OBB.Box[6].x) + B * (RealLocation.y + tmp->OBB.Box[6].y) + C * (RealLocation.z + tmp->OBB.Box[6].z) + D;
			float tmp8 = A * (RealLocation.x + tmp->OBB.Box[7].x) + B * (RealLocation.y + tmp->OBB.Box[7].y) + C * (RealLocation.z + tmp->OBB.Box[7].z) + D;

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

		tmp = tmpShip2;
	}

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
				(*NeedAngle).x = CurrentObjectRotation.x + asinf(ttt) * 57.32f;
		}

		(*NeedAngle).z = CurrentObjectRotation.z;

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
						(*NeedAngle).y = 180.0f - asinf(ttt) * 57.32f;
				}
			} else {
				// находим угол поворота
				sss1 = vw_sqrtf(m * m + n * n + p * p);
				float sss3 = vw_sqrtf(A2 * A2 + B2 * B2 + C2 * C2);
				if ((sss1 != 0.0f) && (sss3 != 0.0f)) {
					float ttt = (A2 * m + B2 * n + C2 * p) / (sss1 * sss3);
					if ((ttt >= -1.0f) && (ttt <= 1.0f)) {
						(*NeedAngle).y = asinf(ttt) * 57.32f;
						if ((*NeedAngle).y < 0.0f)
							(*NeedAngle).y += 360.0f;
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
					  sVECTOR3D Location, // положение точки относительно которой будем наводить
					  sVECTOR3D CurrentObjectRotation, // текущие углы объекта
					  float (&RotationMatrix)[9], // матрица вращения объекта
					  sVECTOR3D *NeedAngle, // нужные углы, чтобы получить нужное направление
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
	cProjectile *tmpProjectile = StartProjectile;
	while (tmpProjectile) {
		cProjectile *tmpProjectile2 = tmpProjectile->Next;
		// только фларес
		if ((tmpProjectile->ProjectileType == 3) && NeedCheckCollision(tmpProjectile) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpProjectile->ObjectStatus == eObjectStatus::Ally) || (tmpProjectile->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpProjectile->ObjectStatus == eObjectStatus::Enemy)))) {

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpProjectile->Location.x + B2 * tmpProjectile->Location.y + C2 * tmpProjectile->Location.z + D2;
			if (tmp1 > 0.0f) {
				float TargetDist2TMP = A2 * tmpProjectile->Location.x + B2 * tmpProjectile->Location.y + C2 * tmpProjectile->Location.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...

					sVECTOR3D TargetAngleTMP;
					TargetLocation = tmpProjectile->Location;

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
						Target = tmpProjectile;
					}
				}
			}
		}

		tmpProjectile = tmpProjectile2;
	}

	// проверка по наземным объектам
	// не стрелять по "мирным" постойкам
	cGroundObject *tmpG = StartGroundObject;
	while (tmpG) {
		cGroundObject *tmpGround2 = tmpG->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpG) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpG->ObjectStatus == eObjectStatus::Ally) || (tmpG->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpG->ObjectStatus == eObjectStatus::Enemy)))) {
			sVECTOR3D tmpLocation = tmpG->GeometryCenter;
			vw_Matrix33CalcPoint(tmpLocation, tmpG->CurrentRotationMat); // поворачиваем в плоскость объекта
			TargetLocation = tmpG->Location + tmpLocation;

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
							Target = tmpG;
						}
					} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 2;
						Target = tmpG;
					}
				}
			}
		}

		tmpG = tmpGround2;
	}

	cSpaceShip *tmp = StartSpaceShip;
	while (tmp) {
		cSpaceShip *tmpShip2 = tmp->Next;
		// проверка, чтобы не считать свой корабль
		if (NeedCheckCollision(tmp) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmp->ObjectStatus == eObjectStatus::Ally) || (tmp->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmp->ObjectStatus == eObjectStatus::Enemy)))) {

			// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmp->Location.x + B2 * tmp->Location.y + C2 * tmp->Location.z + D2;
			if (tmp1 > 0.0f) {
				float TargetDist2TMP = A2 * tmp->Location.x + B2 * tmp->Location.y + C2 * tmp->Location.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...

					sVECTOR3D TargetAngleTMP;
					TargetLocation = tmp->Location;

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
							Target = tmp;
						}
					} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 3;
						Target = tmp;
					}
				}
			}
		}

		tmp = tmpShip2;
	}

	// проверка по космическим объектам
	cSpaceObject *tmpS = StartSpaceObject;
	while (tmpS) {
		cSpaceObject *tmpSpace2 = tmpS->Next;
		// если по этому надо стрелять
		if (NeedCheckCollision(tmpS) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmpS->ObjectStatus == eObjectStatus::Ally) || (tmpS->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmpS->ObjectStatus == eObjectStatus::Enemy))) &&
		    (tmpS->ObjectType != eObjectType::ShipPart)) {
					// проверяем, спереди или сзади стоит противник
			float tmp1 = A2 * tmpS->Location.x  + B2 * tmpS->Location.y  + C2 * tmpS->Location.z + D2;
			if (tmp1 > 0.0f) {

				float TargetDist2TMP = A2 * tmpS->Location.x + B2 * tmpS->Location.y + C2 * tmpS->Location.z + D2;

				// проверяем, чтобы объект находился не ближе чем MinDistance
				if ((MinDistance < TargetDist2TMP) && (MaxDistance > TargetDist2TMP)) {
					// выбираем объект, так, чтобы он был наиболее длижайшим,
					// идущим по нашему курсу...
					sVECTOR3D TargetAngleTMP;
					TargetLocation = tmpS->Location;

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
							Target = tmpS;
						}
					} else if ((Tdist > m * m + n * n + p * p) && (fabsf(TargetAngleTMP.x - CurrentObjectRotation.x) < 45.0f)) {
						TargetAngle = TargetAngleTMP;
						Tdist = m * m + n * n + p * p;
						TargetLocked = true;
						TType = 4;
						Target = tmpS;
					}
				}
			}
		}

		tmpS = tmpSpace2;
	}

	// находим направление и углы нацеливания на цель, если нужно
	if (TargetLocked)
		(*NeedAngle) = TargetAngle;

	return Target;
}

//-----------------------------------------------------------------------------
// Получаем углы поворота для ракеты наведенной на цель
//-----------------------------------------------------------------------------
bool GetMissileOnTargetOrientateion(sVECTOR3D Location, // положение точки относительно которой будем наводить
				    sVECTOR3D CurrentObjectRotation, // текущие углы объекта
				    float (&RotationMatrix)[9], // матрица вращения объекта
				    cObject3D *TargetObject, // объект на который прицеливаемся
				    sVECTOR3D *NeedAngle) // нужные углы, чтобы получить нужное направление
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
	sVECTOR3D TargetLocation = Location;
	*NeedAngle = CurrentObjectRotation;

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
		(*NeedAngle).x = CurrentObjectRotation.x;
		if ((sss1 != 0.0f) && (sss2 != 0.0f)) {
			float sss3 = (A3 * m + B3 * n + C3 * p) / (sss1 * sss2);
			if ((sss3 >= -1.0f) && (sss3 <= 1.0f))
				(*NeedAngle).x = CurrentObjectRotation.x - asinf(sss3) * 57.32f;
		}

		float sss4 = vw_sqrtf(A * A + B * B + C * C);
		(*NeedAngle).y = CurrentObjectRotation.y;
		if ((sss1 != 0.0f) && (sss4 != 0.0f)) {
			float sss5 = (A * m + B * n + C * p) / (sss1 * sss4);
			if ((sss5 >= -1.0f) && (sss5 <= 1.0f))
				(*NeedAngle).y = CurrentObjectRotation.y - asinf(sss5) * 57.32f;
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Проверяем где по отношению ракеты находится объект
//-----------------------------------------------------------------------------
static bool GetMissileTargetPosition(cObject3D *TargetObject,
				     sVECTOR3D Location, // положение точки относительно которой будем наводить
				     float (&RotationMatrix)[9]) // матрица вращения объекта
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
bool GetMissileTargetStatus(cObject3D *TargetObject,
			    sVECTOR3D Location, // положение точки относительно которой будем наводить
			    float (&RotationMatrix)[9]) // матрица вращения объекта
{
	cProjectile *tmpProjectile = StartProjectile;
	while (tmpProjectile) {
		cProjectile *tmpProjectile2 = tmpProjectile->Next;
		if (tmpProjectile == TargetObject)
			return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);
		tmpProjectile = tmpProjectile2;
	}

	cGroundObject *tmpG = StartGroundObject;
	while (tmpG) {
		cGroundObject *tmpGround2 = tmpG->Next;
		if (tmpG == TargetObject)
			return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);
		tmpG = tmpGround2;
	}

	cSpaceShip *tmp = StartSpaceShip;
	while (tmp) {
		cSpaceShip *tmpShip2 = tmp->Next;
		if (tmp == TargetObject)
			return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);
		tmp = tmpShip2;
	}

	cSpaceObject *tmpS = StartSpaceObject;
	while (tmpS) {
		cSpaceObject *tmpSpace2 = tmpS->Next;
		if (tmpS == TargetObject)
			return GetMissileTargetPosition(TargetObject, Location, RotationMatrix);
		tmpS = tmpSpace2;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Получение положения ближайшего врага, для мин
//-----------------------------------------------------------------------------
cObject3D *GetCloserTargetPosition(eObjectStatus ObjectStatus, // статус объекта, который целится
				   sVECTOR3D Location) // положение точки относительно которой будем наводить
{
	// результат
	cObject3D *Res{nullptr};
	// пока ставим отрицательный, т.е. вообще ничего нет
	float MinRatius2{-1.0f};

	// перебираем только корабли...
	cSpaceShip *tmp = StartSpaceShip;
	while (tmp) {
		cSpaceShip *tmpShip2 = tmp->Next;

		// если по этому надо стрелять
		if (NeedCheckCollision(tmp) &&
		    (((ObjectStatus == eObjectStatus::Enemy) && ((tmp->ObjectStatus == eObjectStatus::Ally) || (tmp->ObjectStatus == eObjectStatus::Player))) ||
		     (((ObjectStatus == eObjectStatus::Ally) || (ObjectStatus == eObjectStatus::Player)) && (tmp->ObjectStatus == eObjectStatus::Enemy)))) {
			// получаем квадрат радиуса
			float MinRatius2TMP = (tmp->Location.x - Location.x) * (tmp->Location.x - Location.x) +
					      (tmp->Location.y - Location.y) * (tmp->Location.y - Location.y) +
					      (tmp->Location.z - Location.z) * (tmp->Location.z - Location.z);

			if (MinRatius2 < 0.0f) {
				MinRatius2 = MinRatius2TMP;
				Res = tmp;
			} else if (MinRatius2TMP < MinRatius2) {
				MinRatius2 = MinRatius2TMP;
				Res = tmp;
			}
		}

		tmp = tmpShip2;
	}

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
	for (auto tmpName : UniformLocationNameArray) {
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

} // viewizard namespace
