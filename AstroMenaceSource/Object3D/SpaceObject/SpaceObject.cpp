/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceObject.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


/// подключаем нужные файлы
#include "SpaceObject.h"
#include "../../Game.h"




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
CSpaceObject::CSpaceObject(void)
{

	ObjectStatus = 1; // чужой

	Speed = 0.0f;

	Velocity = RotationSpeed = VECTOR3D(0.0f,0.0f,0.0f);

	GFXQuantity = 0;
	GFX = 0;
	GFXLocation = 0;

	BossPartCountDown = -1.0f;

	LastCameraPoint = GamePoint;

	// подключаем к своему списку
	Next = Prev = 0;
	AttachSpaceObject(this);

}




//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
CSpaceObject::~CSpaceObject(void)
{
	if (GFXLocation != 0) {delete [] GFXLocation; GFXLocation = 0;}
	if (GFX != 0)
	{
		for (int i=0; i<GFXQuantity; i++)
			if (GFX[i] != 0)
			{
				GFX[i]->IsSuppressed = true;
				GFX[i]->DestroyIfNoParticles = true;
			}
		delete [] GFX; GFX = 0;
	}

	DetachSpaceObject(this);
}










//-----------------------------------------------------------------------------
// Установка положения объекта
//-----------------------------------------------------------------------------
void CSpaceObject::SetLocation(VECTOR3D NewLocation)
{
	// вызываем родительскую функцию
	::CObject3D::SetLocation(NewLocation);


	if (GFX != 0)
	for (int i=0; i<GFXQuantity; i++)
	{
		if (GFX[i] != 0)
		{
			GFX[i]->MoveSystem(NewLocation + GFXLocation[i]);
			GFX[i]->SetStartLocation(GFXLocation[i] + NewLocation);
		}
	}
}




//-----------------------------------------------------------------------------
// Установка углов поворота объекта
//-----------------------------------------------------------------------------
void CSpaceObject::SetRotation(VECTOR3D NewRotation)
{
	// вызываем родительскую функцию
	::CObject3D::SetRotation(NewRotation);


	if (GFX != 0)
	for (int i=0; i<GFXQuantity; i++)
	{
		Matrix33CalcPoint(&(GFXLocation[i]), OldInvRotationMat);
		Matrix33CalcPoint(&(GFXLocation[i]), CurrentRotationMat);

		if (GFX[i] != 0)
		{
			if (GFX[i]->SpeedOnCreation == -1.0f)
			{
				GFX[i]->MoveSystem(GFXLocation[i] + Location);
				GFX[i]->SetStartLocation(GFXLocation[i] + Location);
				GFX[i]->RotateSystemAndParticlesByAngle(Rotation);
			}
			else
			{
				GFX[i]->MoveSystemLocation(GFXLocation[i] + Location);
				GFX[i]->RotateSystemByAngle(Rotation);
			}
		}
	}
}









//-----------------------------------------------------------------------------
// Обновление данных объектa
//-----------------------------------------------------------------------------
bool CSpaceObject::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!::CObject3D::Update(Time)) return false;

	// если это часть корабля босса, взрываем через время
	if (BossPartCountDown > -1.0f)
	{
		BossPartCountDown -= TimeDelta;

		if (BossPartCountDown<=0.0f)
		{
			CSpaceExplosion *TMPExplosion;
			TMPExplosion = new CSpaceExplosion;
			TMPExplosion->Create(this, 34, Location, Speed, -1);

			return false;
		}
	}


	// если астероиды
	if (ObjectType == 7 || ObjectType == 15)
	{
		// если большие астероиды летящие сверху
		if (ObjectType == 15 && (ObjectCreationType>20 && ObjectCreationType<30))
		{
			SetRotation(VECTOR3D(RotationSpeed.x*TimeDelta, RotationSpeed.y*TimeDelta, 0.0f));
		}
		else
		{
			if (RotationSpeed.x != 0.0f)
			{
				Rotation.x -= RotationSpeed.x*TimeDelta;
				if (Rotation.x <= 360.0f) Rotation.x += 360.0f;
			}
			if (RotationSpeed.y != 0.0f)
			{
				Rotation.y -= RotationSpeed.y*TimeDelta;
				if (Rotation.y <= 360.0f) Rotation.y += 360.0f;
			}
		}
	}





	// если части корабля или техники, останавливаем
	if (ObjectType == 8)
	{
		if (Speed>0.0f)
		{
			Speed -= 1.0f*TimeDelta;
			if (Speed<0.0f) Speed=0.0f;
		}
		if (Speed<0.0f)
		{
			Speed += 1.0f*TimeDelta;
			if (Speed>0.0f) Speed=0.0f;
		}

		if (RotationSpeed.x != 0.0f)
		{
			SetRotation(VECTOR3D(-RotationSpeed.x*TimeDelta,0.0f,0.0f));
			RotationSpeed.x -= (RotationSpeed.x/4.0f)*TimeDelta;
		}
		if (RotationSpeed.y != 0.0f)
		{
			SetRotation(VECTOR3D(0.0f,-RotationSpeed.y*TimeDelta,0.0f));
			RotationSpeed.y -= (RotationSpeed.y/4.0f)*TimeDelta;
		}
		if (RotationSpeed.z != 0.0f)
		{
			SetRotation(VECTOR3D(0.0f,0.0f,-RotationSpeed.z*TimeDelta));
			RotationSpeed.z -= (RotationSpeed.z/4.0f)*TimeDelta;
		}
	}




	// если планеты, должны учесть положение камеры т.е. ее смещение
	if (ObjectType == 14 ||	ObjectType == 15)
	{
		VECTOR3D Temp = GamePoint - LastCameraPoint;

		// у планет особое движение... они немного могут двигаться на камеру...
		VECTOR3D OrientTTT = Temp^(-1);
		OrientTTT.Normalize();
		SetLocation(Location + (OrientTTT^(Speed*TimeDelta)) + Temp);
		LastCameraPoint = GamePoint;

		// вращения планет и их частей
		if (ObjectType == 14)
		{
			switch (ObjectCreationType)
			{
				// планета с астероидным кольцом
				case 1:
					DrawObjectList[0].Rotation.y += 0.5f*TimeDelta;
					// кольца
					DrawObjectList[1].Rotation.y += 0.7f*TimeDelta;
					DrawObjectList[2].Rotation.y += 0.8f*TimeDelta;
					DrawObjectList[3].Rotation.y += 0.9f*TimeDelta;
					break;
				// полу разрушенная планета
				case 2:
					Rotation.y += 0.5f*TimeDelta;
					if (Rotation.y >= 360.0f) Rotation.y -= 360.0f;
					break;
				// планета с атмосферой
				case 3:
					DrawObjectList[0].Rotation.y += 0.5f*TimeDelta;
					// атмосфера
					DrawObjectList[1].Rotation.y -= 1.2f*TimeDelta;
					DrawObjectList[1].Rotation.y -= 1.0f*TimeDelta;
					break;
				// луна
				case 4:
					DrawObjectList[0].Rotation.y += 1.0f*TimeDelta;
					break;
				// планета пришельцев, с подсветкой
				case 5:
					DrawObjectList[0].Rotation.y += 0.5f*TimeDelta;
					break;
			}

			for (int i=0; i<DrawObjectQuantity; i++)
			{
				if (DrawObjectList[i].Rotation.x >= 360.0f) DrawObjectList[i].Rotation.x -= 360.0f;
				if (DrawObjectList[i].Rotation.x <= -360.0f) DrawObjectList[i].Rotation.x += 360.0f;
				if (DrawObjectList[i].Rotation.y >= 360.0f) DrawObjectList[i].Rotation.y -= 360.0f;
				if (DrawObjectList[i].Rotation.y <= -360.0f) DrawObjectList[i].Rotation.y += 360.0f;
			}
		}

	}
	else
	{
		SetLocation(Location + (Orientation^(Speed*TimeDelta)));
	}



	if (Velocity.x + Velocity.y + Velocity.z != 0.0f)
	{
		SetLocation(Location + (Velocity^TimeDelta));
		Velocity -= Velocity^(0.5f*TimeDelta);
	}



	// объект в порядке - удалять не нужно
	return true;
}



