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
#include "../../assets/texture.h"
#include "../../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// Указатели на начальный и конечный объект в списке
cSpaceObject *StartSpaceObject = nullptr;
cSpaceObject *EndSpaceObject = nullptr;

} // unnamed namespace


//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
static void AttachSpaceObject(cSpaceObject *SpaceObject)
{
	if (SpaceObject == nullptr)
		return;

	if (EndSpaceObject == nullptr) {
		SpaceObject->Prev = nullptr;
		SpaceObject->Next = nullptr;
		StartSpaceObject = SpaceObject;
		EndSpaceObject = SpaceObject;
	} else {
		SpaceObject->Prev = EndSpaceObject;
		SpaceObject->Next = nullptr;
		EndSpaceObject->Next = SpaceObject;
		EndSpaceObject = SpaceObject;
	}
}

//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
static void DetachSpaceObject(cSpaceObject *SpaceObject)
{
	if (SpaceObject == nullptr)
		return;

	if (StartSpaceObject == SpaceObject)
		StartSpaceObject = SpaceObject->Next;
	if (EndSpaceObject == SpaceObject)
		EndSpaceObject = SpaceObject->Prev;


	if (SpaceObject->Next != nullptr)
		SpaceObject->Next->Prev = SpaceObject->Prev;
	else if (SpaceObject->Prev != nullptr)
		SpaceObject->Prev->Next = nullptr;

	if (SpaceObject->Prev != nullptr)
		SpaceObject->Prev->Next = SpaceObject->Next;
	else if (SpaceObject->Next != nullptr)
		SpaceObject->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
// Проверяем все объекты, обновляем данные
//-----------------------------------------------------------------------------
void UpdateAllSpaceObject(float Time)
{
	cSpaceObject *tmp = StartSpaceObject;
	while (tmp != nullptr) {
		cSpaceObject *tmp2 = tmp->Next;
		// делаем обновление данных по объекту
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Прорисовываем все объекты
//-----------------------------------------------------------------------------
void DrawAllSpaceObjects(bool VertexOnlyPass, unsigned int ShadowMap)
{
	cSpaceObject *tmp = StartSpaceObject;
	while (tmp != nullptr) {
		cSpaceObject *tmp2 = tmp->Next;

		// планеты и астероиды рисуем до тайловой анимации в игре!!!
		if ((tmp->ObjectType != eObjectType::Planet) &&
		    !((tmp->ObjectType == eObjectType::BigAsteroid) &&
		      ((tmp->InternalType > 10) && (tmp->InternalType < 20))))
			tmp->Draw(VertexOnlyPass, ShadowMap);

		tmp = tmp2;
	}
}

//-----------------------------------------------------------------------------
// Удаляем все объекты в списке
//-----------------------------------------------------------------------------
void ReleaseAllSpaceObjects()
{
	cSpaceObject *tmp = StartSpaceObject;
	while (tmp != nullptr) {
		cSpaceObject *tmp2 = tmp->Next;
		delete tmp;
		tmp = tmp2;
	}

	StartSpaceObject = nullptr;
	EndSpaceObject = nullptr;
}

/*
 * Managed cycle for each ground object.
 */
void ForEachSpaceObject(std::function<void (cSpaceObject &Object, eSpaceCycle &Command)> function)
{
	cSpaceObject *tmpSpace = StartSpaceObject;
	while (tmpSpace) {
		cSpaceObject *tmpSpaceNext = tmpSpace->Next;
		eSpaceCycle Command{eSpaceCycle::Continue};
		function(*tmpSpace, Command);
		switch (Command) {
		case eSpaceCycle::Continue:
			break;
		case eSpaceCycle::Break:
			return;
		case eSpaceCycle::DeleteObjectAndContinue:
			delete tmpSpace;
			break;
		case eSpaceCycle::DeleteObjectAndBreak:
			delete tmpSpace;
			return;
		}
		tmpSpace = tmpSpaceNext;
	}
}

/*
 * Managed cycle for each space object pair.
 */
void ForEachSpaceObjectPair(std::function<void (cSpaceObject &FirstObject,
						cSpaceObject &SecondObject,
						eSpacePairCycle &Command)> function)
{
	cSpaceObject *tmpFirstSpace = StartSpaceObject;
	while (tmpFirstSpace) {
		eSpacePairCycle Command{eSpacePairCycle::Continue};
		cSpaceObject *tmpSecondSpace = tmpFirstSpace->Next;
		while (tmpSecondSpace) {
			cSpaceObject *tmpSecondSpaceNext = tmpSecondSpace->Next;

			Command = eSpacePairCycle::Continue;
			function(*tmpFirstSpace, *tmpSecondSpace, Command);

			if ((Command == eSpacePairCycle::DeleteSecondObjectAndContinue) ||
			    (Command == eSpacePairCycle::DeleteBothObjectsAndContinue))
				delete tmpSecondSpace;

			// break second cycle
			if ((Command == eSpacePairCycle::DeleteFirstObjectAndContinue) ||
			    (Command == eSpacePairCycle::DeleteBothObjectsAndContinue))
				break;

			tmpSecondSpace = tmpSecondSpaceNext;
		}

		cSpaceObject *tmpFirstSpaceNext = tmpFirstSpace->Next;

		if ((Command == eSpacePairCycle::DeleteFirstObjectAndContinue) ||
		    (Command == eSpacePairCycle::DeleteBothObjectsAndContinue))
			delete tmpFirstSpace;

		tmpFirstSpace = tmpFirstSpaceNext;
	}
}

//-----------------------------------------------------------------------------
// Создание эффектов для космических объектов (двигатели для базы)
//-----------------------------------------------------------------------------
void SetSpaceObjectGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXType)
{
	ParticleSystem->Texture = GetPreloadedTextureAsset("gfx/flare1.tga");
	ParticleSystem->Direction = sVECTOR3D(0.0f, -1.0f, 0.0f);

	switch(GFXType) {
	// двигатели на базе пиратов
	case 1:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.30f;
		ParticleSystem->SizeEnd    = 0.60f;
		ParticleSystem->Speed      = 10.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 5.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->CreationType = eParticleCreationType::Sphere;
		ParticleSystem->CreationSize = sVECTOR3D(0.8f, 0.1f, 0.8f);
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D(0.0f, -1.0f, 0.0f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f, 0.0f, 0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.025f);
		break;
	case 2:
		ParticleSystem->ColorStart.r = 0.60f;
		ParticleSystem->ColorStart.g = 0.60f;
		ParticleSystem->ColorStart.b = 0.30f;
		ParticleSystem->ColorEnd.r = 0.30f;
		ParticleSystem->ColorEnd.g = 1.00f;
		ParticleSystem->ColorEnd.b = 0.30f;
		ParticleSystem->AlphaStart = 1.00f;
		ParticleSystem->AlphaEnd   = 0.00f;
		ParticleSystem->SizeStart  = 0.30f;
		ParticleSystem->SizeVar    = 0.20f;
		ParticleSystem->SizeEnd    = 0.10f;
		ParticleSystem->Speed      = 3.00f;
		ParticleSystem->SpeedOnCreation = -1.00f;
		ParticleSystem->SpeedVar   = 2.00f;
		ParticleSystem->Theta      = 180.00f;
		ParticleSystem->Life       = 0.50f;
		ParticleSystem->ParticlesPerSec = 100;
		ParticleSystem->AlphaShowHide= true;
		ParticleSystem->Direction = sVECTOR3D(0.0f, 1.0f, 0.0f);
		ParticleSystem->Light = vw_CreatePointLight(sVECTOR3D(0.0f, 0.0f, 0.0f), 0.45f, 0.8f, 0.3f, 0.0f, 0.3f);
		break;

	default:
		std::cerr << __func__ << "(): " << "wrong GFXType.\n";
		break;
	}

}

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
void cSpaceObject::SetRotation(sVECTOR3D NewRotation)
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
			new cSpaceExplosion(*this, 34, Location, Speed, -1);
			return false;
		}
	}

	// если астероиды
	if ((ObjectType == eObjectType::SmallAsteroid) ||
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
			SetRotation(sVECTOR3D(-RotationSpeed.x * TimeDelta, 0.0f, 0.0f));
			RotationSpeed.x -= (RotationSpeed.x / 4.0f) * TimeDelta;
		}
		if (RotationSpeed.y != 0.0f) {
			SetRotation(sVECTOR3D(0.0f, -RotationSpeed.y * TimeDelta, 0.0f));
			RotationSpeed.y -= (RotationSpeed.y / 4.0f) * TimeDelta;
		}
		if (RotationSpeed.z != 0.0f) {
			SetRotation(sVECTOR3D(0.0f, 0.0f, -RotationSpeed.z * TimeDelta));
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
				Model3DBlocks[0].Rotation.y += 0.5f * TimeDelta;
				// кольца
				Model3DBlocks[1].Rotation.y += 0.7f * TimeDelta;
				Model3DBlocks[2].Rotation.y += 0.8f * TimeDelta;
				Model3DBlocks[3].Rotation.y += 0.9f * TimeDelta;
				break;
			// полу разрушенная планета
			case 2:
				Rotation.y += 0.5f * TimeDelta;
				if (Rotation.y >= 360.0f)
					Rotation.y -= 360.0f;
				break;
			// планета с атмосферой
			case 3:
				Model3DBlocks[0].Rotation.y += 0.5f * TimeDelta;
				// атмосфера
				Model3DBlocks[1].Rotation.y -= 0.7 * TimeDelta;
				break;
			// луна
			case 4:
				Model3DBlocks[0].Rotation.y += TimeDelta;
				break;
			// планета пришельцев, с подсветкой
			case 5:
				Model3DBlocks[0].Rotation.y += 0.5f * TimeDelta;
				break;
			// планета пришельцев
			case 6:
				Model3DBlocks[0].Rotation.y += 0.5f * TimeDelta;
				break;
			}

			for (auto &tmpModel3DBlock : Model3DBlocks) {
				if (tmpModel3DBlock.Rotation.x >= 360.0f)
					tmpModel3DBlock.Rotation.x -= 360.0f;
				if (tmpModel3DBlock.Rotation.x <= -360.0f)
					tmpModel3DBlock.Rotation.x += 360.0f;
				if (tmpModel3DBlock.Rotation.y >= 360.0f)
					tmpModel3DBlock.Rotation.y -= 360.0f;
				if (tmpModel3DBlock.Rotation.y <= -360.0f)
					tmpModel3DBlock.Rotation.y += 360.0f;
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
