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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments

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

#include "../game.h"
#include "../object3d/space_ship/space_ship.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

float GameCameraLastUpdate{0.0f};
float GameCameraSpeed{10.0f};
sVECTOR3D CameraCoveredDistance{0.0f, 0.0f, 0.0f};
sVECTOR3D CameraMovementDirection{0.0f, 0.0f, 1.0f};
// camera shake on explosion related vaiables
float GameCameraDeviation{0.0f};
float GameCameraDeviationTime{0.0f};
float GameCameraNeedDeviation{0.0f};
float GameCameraDeviationPower{0.0f};
float GameCameraNeedStartDeviation{0.0f};
float GameCameraDeviationAge{0.0f};

} // unnamed namespace


/*
 *
 */
void ResetGameCamera()
{
	CameraCoveredDistance = sVECTOR3D{0.0f, 0.0f, 0.0f};
	GameCameraLastUpdate = 0.0f;

	GameCameraDeviation = 0.0f;
	GameCameraDeviationTime = 0.0f;
	GameCameraNeedDeviation = 0.0f;
	GameCameraDeviationPower = 0.0f;
	GameCameraNeedStartDeviation = 0.0f;
	GameCameraDeviationAge = 0.0f;
	vw_SetCameraDeviation(sVECTOR3D{0.0f, 0.0f, 0.0f});
}

//-----------------------------------------------------------------------------
// инициализация переменных камеры для игры
//-----------------------------------------------------------------------------
void InitGameCamera()
{
	ResetGameCamera();
	GameCameraLastUpdate = vw_GetTimeThread(1);
}

//-----------------------------------------------------------------------------
// установка взрыва
//-----------------------------------------------------------------------------
void GameCameraSetExplosion(const sVECTOR3D &Location, float Power)
{
	// если корабля нет, нам тут делать нечего
	auto sharedPlayerFighter = PlayerFighter.lock();
	if (!sharedPlayerFighter)
		return;

	// вычисляем по дистанции время болтанки....
	// чуствительность начинается со 100 едениц (10000 в квадрате)

	float dist2 = (sharedPlayerFighter->Location.x - Location.x) * (sharedPlayerFighter->Location.x - Location.x) +
		      (sharedPlayerFighter->Location.y - Location.y) * (sharedPlayerFighter->Location.y - Location.y) +
		      (sharedPlayerFighter->Location.z - Location.z) * (sharedPlayerFighter->Location.z - Location.z);

	// слишком далеко
	if (dist2 > 10000.0f)
		return;
	// или очень близко
	if (dist2 <= sharedPlayerFighter->Radius * sharedPlayerFighter->Radius)
		dist2 = sharedPlayerFighter->Radius * sharedPlayerFighter->Radius;

	// время болтанки
	GameCameraDeviationAge = GameCameraDeviationTime = (10000.0f - dist2) / 10000.0f;

	if (Power > 1.0f) // очень большой взрыв
		GameCameraDeviationAge = GameCameraDeviationTime = GameCameraDeviationTime * 3.0f;

	GameCameraDeviationPower = Power * (10000.0f - dist2) / 20000.0f;

	GameCameraNeedStartDeviation = GameCameraNeedDeviation = GameCameraDeviationPower * vw_fRand0();
}

//-----------------------------------------------------------------------------
// обновление камеры в игре, учитываем (полет, поворот и т.д.)
//-----------------------------------------------------------------------------
void GameCameraUpdate(float Time)
{
	float TimeDelta = Time - GameCameraLastUpdate;
	GameCameraLastUpdate = Time;

	sVECTOR3D tmpNeedPos = CameraMovementDirection ^ (GameCameraSpeed * TimeDelta);

	// обновляем данные камеры (+ устанавливаем флаг, чтобы обновить фруструм)
	vw_IncCameraLocation(tmpNeedPos);

	// делаем действия над кораблем игрока, если он есть
	if (auto sharedPlayerFighter = PlayerFighter.lock())
		sharedPlayerFighter->SetLocationArcadePlayer(sharedPlayerFighter->Location + tmpNeedPos);

	CameraCoveredDistance += tmpNeedPos;

	GameCameraDeviationTime -= TimeDelta;
	if (GameCameraDeviationTime < 0.0f)
		GameCameraDeviationTime = 0.0f;

	bool tmpNeedStopDeviation{false};
	if ((GameCameraDeviationTime <= 0.0f) && (GameCameraDeviation != 0.0f)) {
		tmpNeedStopDeviation = true;
		GameCameraNeedStartDeviation = GameCameraNeedDeviation = 0.0f;
	}

	if ((GameCameraDeviationTime > 0.0f) || tmpNeedStopDeviation) {
		float Sign{1.0f};
		if (GameCameraNeedDeviation < GameCameraDeviation)
			Sign = -1.0f;

		float tmpIncrement = Sign * 5.0f * TimeDelta;

		if (((Sign > 0.0f) && (GameCameraNeedDeviation <= GameCameraDeviation + tmpIncrement)) ||
		    ((Sign < 0.0f) && (GameCameraNeedDeviation >= GameCameraDeviation + tmpIncrement))) {
			GameCameraDeviation = GameCameraNeedDeviation;
			GameCameraDeviationPower *= GameCameraDeviationTime / GameCameraDeviationAge;
			GameCameraNeedStartDeviation = GameCameraNeedDeviation = GameCameraDeviationPower * vw_fRand0();
		} else {
			GameCameraDeviation += tmpIncrement;
		}
	}

	vw_SetCameraDeviation(sVECTOR3D{GameCameraDeviation * 2.0f, GameCameraDeviation, 0.0f});
}

//-----------------------------------------------------------------------------
// передача параметров
//-----------------------------------------------------------------------------
float GameCameraGetDeviation()
{
	return GameCameraDeviation;
}

/*
 *
 */
float GameCameraGetSpeed()
{
	return GameCameraSpeed;
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
