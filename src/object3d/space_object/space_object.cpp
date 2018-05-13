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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "space_object.h"
#include "../../game.h"




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cSpaceObject::cSpaceObject()
{
	ObjectStatus = eObjectStatus::Enemy;

	LastCameraPoint = GamePoint;

	// подключаем к своему списку
	AttachSpaceObject(this);
}




//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cSpaceObject::~cSpaceObject()
{
	if (!GraphicFX.empty()) {
		for (auto tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				sharedGFX->IsSuppressed = true;
				sharedGFX->DestroyIfNoParticles = true;
			}
		}
	}

	DetachSpaceObject(this);
}










//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void cSpaceObject::SetLocation(sVECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::cObject3D::SetLocation(NewLocation);

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
void cSpaceObject::SetRotation(sVECTOR3D NewRotation)
{
	// вызываем родительскую функцию
	::cObject3D::SetRotation(NewRotation);

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
	if (!::cObject3D::Update(Time)) return false;

	// если это часть корабля босса, взрываем через время
	if (BossPartCountDown > -1.0f) {
		BossPartCountDown -= TimeDelta;

		if (BossPartCountDown<=0.0f) {
			new cSpaceExplosion(this, 34, Location, Speed, -1);
			return false;
		}
	}


	// если астероиды
	if ((ObjectType == eObjectType::Asteroids) ||
	    (ObjectType == eObjectType::BigAsteroid)) {
		// если большие астероиды летящие сверху
		if ((ObjectType == eObjectType::BigAsteroid) &&
		    ((InternalType > 20) && (InternalType < 30))) {
			SetRotation(sVECTOR3D(RotationSpeed.x*TimeDelta, RotationSpeed.y*TimeDelta, 0.0f));
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
	if (ObjectType == eObjectType::ShipPart) {
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
			SetRotation(sVECTOR3D(-RotationSpeed.x*TimeDelta,0.0f,0.0f));
			RotationSpeed.x -= (RotationSpeed.x/4.0f)*TimeDelta;
		}
		if (RotationSpeed.y != 0.0f) {
			SetRotation(sVECTOR3D(0.0f,-RotationSpeed.y*TimeDelta,0.0f));
			RotationSpeed.y -= (RotationSpeed.y/4.0f)*TimeDelta;
		}
		if (RotationSpeed.z != 0.0f) {
			SetRotation(sVECTOR3D(0.0f,0.0f,-RotationSpeed.z*TimeDelta));
			RotationSpeed.z -= (RotationSpeed.z/4.0f)*TimeDelta;
		}
	}




	// если планеты, должны учесть положение камеры т.е. ее смещение
	if ((ObjectType == eObjectType::Planet) ||
	    (ObjectType == eObjectType::BigAsteroid)) {
		sVECTOR3D Temp = GamePoint - LastCameraPoint;

		// у планет особое движение... они немного могут двигаться на камеру...
		sVECTOR3D OrientTTT = Temp^(-1);
		OrientTTT.Normalize();
		SetLocation(Location + (OrientTTT^(Speed*TimeDelta)) + Temp);
		LastCameraPoint = GamePoint;

		// вращения планет и их частей
		if (ObjectType == eObjectType::Planet) {
			switch (InternalType) {
			// планета с астероидным кольцом
			case 1:
				ObjectBlocks[0].Rotation.y += 0.5f * TimeDelta;
				// кольца
				ObjectBlocks[1].Rotation.y += 0.7f * TimeDelta;
				ObjectBlocks[2].Rotation.y += 0.8f * TimeDelta;
				ObjectBlocks[3].Rotation.y += 0.9f * TimeDelta;
				break;
			// полу разрушенная планета
			case 2:
				Rotation.y += 0.5f * TimeDelta;
				if (Rotation.y >= 360.0f)
					Rotation.y -= 360.0f;
				break;
			// планета с атмосферой
			case 3:
				ObjectBlocks[0].Rotation.y += 0.5f * TimeDelta;
				// атмосфера
				ObjectBlocks[1].Rotation.y -= 0.7 * TimeDelta;
				break;
			// луна
			case 4:
				ObjectBlocks[0].Rotation.y += TimeDelta;
				break;
			// планета пришельцев, с подсветкой
			case 5:
				ObjectBlocks[0].Rotation.y += 0.5f * TimeDelta;
				break;
			// планета пришельцев
			case 6:
				ObjectBlocks[0].Rotation.y += 0.5f * TimeDelta;
				break;
			}

			for (auto &tmpObjectBlock : ObjectBlocks) {
				if (tmpObjectBlock.Rotation.x >= 360.0f)
					tmpObjectBlock.Rotation.x -= 360.0f;
				if (tmpObjectBlock.Rotation.x <= -360.0f)
					tmpObjectBlock.Rotation.x += 360.0f;
				if (tmpObjectBlock.Rotation.y >= 360.0f)
					tmpObjectBlock.Rotation.y -= 360.0f;
				if (tmpObjectBlock.Rotation.y <= -360.0f)
					tmpObjectBlock.Rotation.y += 360.0f;
			}
		}

	} else {
		SetLocation(Location + (Orientation^(Speed*TimeDelta)));
	}



	if (Velocity.x + Velocity.y + Velocity.z != 0.0f) {
		SetLocation(Location + (Velocity^TimeDelta));
		Velocity -= Velocity^(0.5f*TimeDelta);
	}



	// объект в порядке - удалять не нужно
	return true;
}



