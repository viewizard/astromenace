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

#include "space_object.h"
#include "../explosion/explosion.h"
#include "../../assets/texture.h"
#include "../../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// all space object list
std::list<std::shared_ptr<cSpaceObject>> SpaceObjectList{};

} // unnamed namespace


/*
 * Create cSmallAsteroid object.
 */
std::weak_ptr<cSpaceObject> CreateSmallAsteroid()
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	SpaceObjectList.emplace_front(new cSmallAsteroid, [](cSmallAsteroid *p) {delete p;});
	return SpaceObjectList.front();
}

/*
 * Create cBigAsteroid object.
 */
std::weak_ptr<cSpaceObject> CreateBigAsteroid(const int AsteroidNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	SpaceObjectList.emplace_front(new cBigAsteroid{AsteroidNum}, [](cBigAsteroid *p) {delete p;});
	return SpaceObjectList.front();
}

/*
 * Create cPlanet object.
 */
std::weak_ptr<cSpaceObject> CreatePlanet(const int PlanetNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	SpaceObjectList.emplace_front(new cPlanet{PlanetNum}, [](cPlanet *p) {delete p;});
	return SpaceObjectList.front();
}

/*
 * Create cSpaceDebris object.
 */
std::weak_ptr<cSpaceObject> CreateSpaceDebris()
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	SpaceObjectList.emplace_front(new cSpaceDebris, [](cSpaceDebris *p) {delete p;});
	return SpaceObjectList.front();
}

/*
 * Create cBasePart object.
 */
std::weak_ptr<cSpaceObject> CreateBasePart(const int BasePartNum)
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	SpaceObjectList.emplace_front(new cBasePart{BasePartNum}, [](cBasePart *p) {delete p;});
	return SpaceObjectList.front();
}

//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceObject(float Time)
{
	for (auto iter = SpaceObjectList.begin(); iter != SpaceObjectList.end();) {
		if (!iter->get()->Update(Time))
			iter = SpaceObjectList.erase(iter);
		else
			++iter;
	}
}

//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllSpaceObjects(bool VertexOnlyPass, unsigned int ShadowMap)
{
	for (auto &tmpObject : SpaceObjectList) {
		// планеты и астероиды рисуем до тайловой анимации в игре!!!
		if ((tmpObject.get()->ObjectType != eObjectType::Planet) &&
		    !((tmpObject.get()->ObjectType == eObjectType::BigAsteroid) &&
		      ((tmpObject.get()->InternalType > 10) && (tmpObject.get()->InternalType < 20))))
			tmpObject.get()->Draw(VertexOnlyPass, ShadowMap);
	}
}

/*
 * Release particular space object.
 */
void ReleaseSpaceObject(std::weak_ptr<cSpaceObject> &Object)
{
	auto sharedObject = Object.lock();
	if (!sharedObject)
		return;

	for (auto iter = SpaceObjectList.begin(); iter != SpaceObjectList.end();) {
		if (iter->get() == sharedObject.get()) {
			SpaceObjectList.erase(iter);
			return;
		}
		++iter;
	}
}

//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceObjects()
{
	SpaceObjectList.clear();
}

/*
 * Cycle for each ground object.
 * Note, caller must guarantee, that 'Object' will not released in callback function call.
 */
void ForEachSpaceObject(std::function<void (cSpaceObject &Object)> function)
{
	for (auto &tmpSpace : SpaceObjectList) {
		function(*tmpSpace);
	}
}

/*
 * Managed cycle for each ground object.
 * Note, caller must guarantee, that 'Object' will not released in callback function call.
 */
void ForEachSpaceObject(std::function<void (cSpaceObject &Object, eSpaceCycle &Command)> function)
{
	for (auto iter = SpaceObjectList.begin(); iter != SpaceObjectList.end();) {
		eSpaceCycle Command{eSpaceCycle::Continue};
		function(*iter->get(), Command);

		switch (Command) {
		case eSpaceCycle::Continue:
			++iter;
			break;
		case eSpaceCycle::Break:
			return;
		case eSpaceCycle::DeleteObjectAndContinue:
			iter = SpaceObjectList.erase(iter);
			break;
		case eSpaceCycle::DeleteObjectAndBreak:
			SpaceObjectList.erase(iter);
			return;
		}
	}
}

/*
 * Managed cycle for each space object pair.
 * Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
 */
void ForEachSpaceObjectPair(std::function<void (cSpaceObject &FirstObject,
						cSpaceObject &SecondObject,
						eSpacePairCycle &Command)> function)
{
	for (auto iterFirst = SpaceObjectList.begin(); iterFirst != SpaceObjectList.end();) {
		eSpacePairCycle Command{eSpacePairCycle::Continue};

		for (auto iterSecond = std::next(iterFirst, 1); iterSecond != SpaceObjectList.end();) {
			Command = eSpacePairCycle::Continue;
			function(*iterFirst->get(), *iterSecond->get(), Command);

			if ((Command == eSpacePairCycle::DeleteSecondObjectAndContinue) ||
			    (Command == eSpacePairCycle::DeleteBothObjectsAndContinue))
				iterSecond = SpaceObjectList.erase(iterSecond);
			else
				++iterSecond;

			// break second cycle
			if ((Command == eSpacePairCycle::DeleteFirstObjectAndContinue) ||
			    (Command == eSpacePairCycle::DeleteBothObjectsAndContinue))
				break;
		}

		if ((Command == eSpacePairCycle::DeleteFirstObjectAndContinue) ||
		    (Command == eSpacePairCycle::DeleteBothObjectsAndContinue))
			iterFirst = SpaceObjectList.erase(iterFirst);
		else
			++iterFirst;
	}
}

/*
 * Get object ptr by reference.
 */
std::weak_ptr<cObject3D> GetSpaceObjectPtr(const cSpaceObject &Object)
{
	for (auto &tmpSpace : SpaceObjectList) {
		if (tmpSpace.get() == &Object)
			return tmpSpace;
	}

	return std::weak_ptr<cObject3D>{};
}

//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cSpaceObject::cSpaceObject()
{
	ObjectStatus = eObjectStatus::Enemy;

	LastCameraPoint = GamePoint;
}

//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cSpaceObject::~cSpaceObject()
{
	if (!GraphicFX.empty()) {
		for (auto &tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				sharedGFX->IsSuppressed = true;
				sharedGFX->DestroyIfNoParticles = true;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cSpaceObject::SetLocation(const sVECTOR3D &NewLocation)
{
	// вызываем родительскую функцию
	cObject3D::SetLocation(NewLocation);

	if (!GraphicFX.empty()) {
		for (unsigned int i = 0; i < GraphicFX.size(); i++) {
			if (auto sharedGFX = GraphicFX[i].lock()) {
				sharedGFX->MoveSystem(NewLocation + GraphicFXLocation[i]);
				sharedGFX->SetStartLocation(GraphicFXLocation[i] + NewLocation);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void cSpaceObject::SetRotation(const sVECTOR3D &NewRotation)
{
	// вызываем родительскую функцию
	cObject3D::SetRotation(NewRotation);

	if (!GraphicFX.empty()) {
		for (unsigned int i = 0; i < GraphicFX.size(); i++) {
			if (auto sharedGFX = GraphicFX[i].lock()) {
				vw_Matrix33CalcPoint(GraphicFXLocation[i], OldInvRotationMat);
				vw_Matrix33CalcPoint(GraphicFXLocation[i], CurrentRotationMat);

				if (sharedGFX->SpeedOnCreation == -1.0f) {
					sharedGFX->MoveSystem(GraphicFXLocation[i] + Location);
					sharedGFX->SetStartLocation(GraphicFXLocation[i] + Location);
					sharedGFX->RotateSystemAndParticlesByAngle(Rotation);
				} else {
					sharedGFX->MoveSystemLocation(GraphicFXLocation[i] + Location);
					sharedGFX->RotateSystemByAngle(Rotation);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Обновление данных объектa
//-----------------------------------------------------------------------------
bool cSpaceObject::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!cObject3D::Update(Time))
		return false;

	// если это часть корабля босса, взрываем через время
	if (BossPartCountDown > -1.0f) {
		BossPartCountDown -= TimeDelta;

		if (BossPartCountDown <= 0.0f) {
			CreateSpaceExplosion(*this, 34, Location, Speed, -1);
			return false;
		}
	}

	// если астероиды
	if ((ObjectType == eObjectType::SmallAsteroid) ||
	    (ObjectType == eObjectType::BigAsteroid)) {
		// если большие астероиды летящие сверху
		if ((ObjectType == eObjectType::BigAsteroid) &&
		    ((InternalType > 20) && (InternalType < 30))) {
			SetRotation(sVECTOR3D{RotationSpeed.x * TimeDelta,
					      RotationSpeed.y * TimeDelta,
					      0.0f});
		} else {
			if (RotationSpeed.x != 0.0f) {
				Rotation.x -= RotationSpeed.x*TimeDelta;
				if (Rotation.x <= 360.0f) Rotation.x += 360.0f;
			}
			if (RotationSpeed.y != 0.0f) {
				Rotation.y -= RotationSpeed.y*TimeDelta;
				if (Rotation.y <= 360.0f) Rotation.y += 360.0f;
			}
		}
	}

	// если части корабля или техники, останавливаем
	if (ObjectType == eObjectType::SpaceDebris) {
		if (Speed > 0.0f) {
			Speed -= TimeDelta;
			if (Speed < 0.0f)
				Speed = 0.0f;
		}
		if (Speed < 0.0f) {
			Speed += TimeDelta;
			if (Speed > 0.0f)
				Speed = 0.0f;
		}

		if (RotationSpeed.x != 0.0f) {
			SetRotation(sVECTOR3D{-RotationSpeed.x * TimeDelta, 0.0f, 0.0f});
			RotationSpeed.x -= (RotationSpeed.x / 4.0f) * TimeDelta;
		}
		if (RotationSpeed.y != 0.0f) {
			SetRotation(sVECTOR3D{0.0f, -RotationSpeed.y * TimeDelta, 0.0f});
			RotationSpeed.y -= (RotationSpeed.y / 4.0f) * TimeDelta;
		}
		if (RotationSpeed.z != 0.0f) {
			SetRotation(sVECTOR3D{0.0f, 0.0f, -RotationSpeed.z * TimeDelta});
			RotationSpeed.z -= (RotationSpeed.z / 4.0f) * TimeDelta;
		}
	}

	// если планеты, должны учесть положение камеры т.е. ее смещение
	if ((ObjectType == eObjectType::Planet) ||
	    (ObjectType == eObjectType::BigAsteroid)) {
		sVECTOR3D Temp = GamePoint - LastCameraPoint;

		// у планет особое движение... они немного могут двигаться на камеру...
		sVECTOR3D OrientTTT = Temp ^ (-1);
		OrientTTT.Normalize();
		SetLocation(Location + (OrientTTT ^ (Speed * TimeDelta)) + Temp);
		LastCameraPoint = GamePoint;

		// вращения планет и их частей
		if (ObjectType == eObjectType::Planet) {
			switch (InternalType) {
			// планета с астероидным кольцом
			case 1:
				Chunks[0].Rotation.y += 0.5f * TimeDelta;
				// кольца
				Chunks[1].Rotation.y += 0.7f * TimeDelta;
				Chunks[2].Rotation.y += 0.8f * TimeDelta;
				Chunks[3].Rotation.y += 0.9f * TimeDelta;
				break;
			// полу разрушенная планета
			case 2:
				Rotation.y += 0.5f * TimeDelta;
				if (Rotation.y >= 360.0f)
					Rotation.y -= 360.0f;
				break;
			// планета с атмосферой
			case 3:
				Chunks[0].Rotation.y += 0.5f * TimeDelta;
				// атмосфера
				Chunks[1].Rotation.y -= 0.7 * TimeDelta;
				break;
			// луна
			case 4:
				Chunks[0].Rotation.y += TimeDelta;
				break;
			// планета пришельцев, с подсветкой
			case 5:
				Chunks[0].Rotation.y += 0.5f * TimeDelta;
				break;
			// планета пришельцев
			case 6:
				Chunks[0].Rotation.y += 0.5f * TimeDelta;
				break;
			}

			for (auto &tmpChunk : Chunks) {
				if (tmpChunk.Rotation.x >= 360.0f)
					tmpChunk.Rotation.x -= 360.0f;
				if (tmpChunk.Rotation.x <= -360.0f)
					tmpChunk.Rotation.x += 360.0f;
				if (tmpChunk.Rotation.y >= 360.0f)
					tmpChunk.Rotation.y -= 360.0f;
				if (tmpChunk.Rotation.y <= -360.0f)
					tmpChunk.Rotation.y += 360.0f;
			}
		}
	} else {
		SetLocation(Location + (Orientation ^ (Speed * TimeDelta)));
	}

	if (Velocity.x + Velocity.y + Velocity.z != 0.0f) {
		SetLocation(Location + (Velocity ^ TimeDelta));
		Velocity -= Velocity ^ (0.5f * TimeDelta);
	}

	// объект в порядке - удалять не нужно
	return true;
}

} // astromenace namespace
} // viewizard namespace
