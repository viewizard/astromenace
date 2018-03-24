/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
cSpaceObject::cSpaceObject(void)
{

	ObjectStatus = 1; // чужой

	LastCameraPoint = GamePoint;

	// подключаем к своему списку
	AttachSpaceObject(this);

}




//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cSpaceObject::~cSpaceObject(void)
{
	if (GFXLocation != nullptr) {
		delete [] GFXLocation;
		GFXLocation = nullptr;
	}
	if (GFX != nullptr) {
		for (int i = 0; i < GFXQuantity; i++)
			if (GFX[i] != nullptr) {
				GFX[i]->IsSuppressed = true;
				GFX[i]->DestroyIfNoParticles = true;
			}
		delete [] GFX;
		GFX = nullptr;
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


	if (GFX != nullptr)
		for (int i = 0; i < GFXQuantity; i++) {
			if (GFX[i] != nullptr) {
				GFX[i]->MoveSystem(NewLocation + GFXLocation[i]);
				GFX[i]->SetStartLocation(GFXLocation[i] + NewLocation);
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


	if (GFX != nullptr)
		for (int i = 0; i < GFXQuantity; i++) {
			vw_Matrix33CalcPoint(GFXLocation[i], OldInvRotationMat);
			vw_Matrix33CalcPoint(GFXLocation[i], CurrentRotationMat);

			if (GFX[i] != nullptr) {
				if (GFX[i]->SpeedOnCreation == -1.0f) {
					GFX[i]->MoveSystem(GFXLocation[i] + Location);
					GFX[i]->SetStartLocation(GFXLocation[i] + Location);
					GFX[i]->RotateSystemAndParticlesByAngle(Rotation);
				} else {
					GFX[i]->MoveSystemLocation(GFXLocation[i] + Location);
					GFX[i]->RotateSystemByAngle(Rotation);
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
	if (ObjectType == 7 || ObjectType == 15) {
		// если большие астероиды летящие сверху
		if (ObjectType == 15 && (ObjectCreationType>20 && ObjectCreationType<30)) {
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
	if (ObjectType == 8) {
		if (Speed>0.0f) {
			Speed -= 1.0f*TimeDelta;
			if (Speed<0.0f) Speed=0.0f;
		}
		if (Speed<0.0f) {
			Speed += 1.0f*TimeDelta;
			if (Speed>0.0f) Speed=0.0f;
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
	if (ObjectType == 14 ||	ObjectType == 15) {
		sVECTOR3D Temp = GamePoint - LastCameraPoint;

		// у планет особое движение... они немного могут двигаться на камеру...
		sVECTOR3D OrientTTT = Temp^(-1);
		OrientTTT.Normalize();
		SetLocation(Location + (OrientTTT^(Speed*TimeDelta)) + Temp);
		LastCameraPoint = GamePoint;

		// вращения планет и их частей
		if (ObjectType == 14) {
			switch (ObjectCreationType) {
			// планета с астероидным кольцом
			case 1:
				ObjectsList[0].Rotation.y += 0.5f * TimeDelta;
				// кольца
				ObjectsList[1].Rotation.y += 0.7f * TimeDelta;
				ObjectsList[2].Rotation.y += 0.8f * TimeDelta;
				ObjectsList[3].Rotation.y += 0.9f * TimeDelta;
				break;
			// полу разрушенная планета
			case 2:
				Rotation.y += 0.5f * TimeDelta;
				if (Rotation.y >= 360.0f)
					Rotation.y -= 360.0f;
				break;
			// планета с атмосферой
			case 3:
				ObjectsList[0].Rotation.y += 0.5f * TimeDelta;
				// атмосфера
				ObjectsList[1].Rotation.y -= 0.7 * TimeDelta;
				break;
			// луна
			case 4:
				ObjectsList[0].Rotation.y += 1.0f * TimeDelta;
				break;
			// планета пришельцев, с подсветкой
			case 5:
				ObjectsList[0].Rotation.y += 0.5f * TimeDelta;
				break;
			// планета пришельцев
			case 6:
				ObjectsList[0].Rotation.y += 0.5f * TimeDelta;
				break;
			}

			for (int i = 0; i < ObjectsListCount; i++) {
				if (ObjectsList[i].Rotation.x >= 360.0f)
					ObjectsList[i].Rotation.x -= 360.0f;
				if (ObjectsList[i].Rotation.x <= -360.0f)
					ObjectsList[i].Rotation.x += 360.0f;
				if (ObjectsList[i].Rotation.y >= 360.0f)
					ObjectsList[i].Rotation.y -= 360.0f;
				if (ObjectsList[i].Rotation.y <= -360.0f)
					ObjectsList[i].Rotation.y += 360.0f;
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



