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

// FIXME split backgroind big asteroid code, render with separate function ('planet' and
//       'planetoid' (backgroind big asteroid)), remove related code from StarSystemDraw()

// TODO translate comments

#ifndef OBJECT3D_SPACEOBJECT_SPACEOBJECT_H
#define OBJECT3D_SPACEOBJECT_SPACEOBJECT_H

#include "../object3d.h"
#include "../explosion/space_explosion/space_explosion.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eSpaceCycle {
	Continue,
	Break,
	DeleteObjectAndContinue,
	DeleteObjectAndBreak
};

enum class eSpacePairCycle {
	Continue,
	DeleteFirstObjectAndContinue,
	DeleteSecondObjectAndContinue,
	DeleteBothObjectsAndContinue
};

class cSpaceObject : public cObject3D
{
protected:
	// don't allow object of this class creation
	cSpaceObject();
	virtual ~cSpaceObject();

public:
	// Обновление данных объектa
	virtual bool Update(float Time) override;
	// Установка положения объекта
	virtual void SetLocation(sVECTOR3D NewLocation) override;
	// Установка углов поворота объекта
	virtual void SetRotation(sVECTOR3D NewRotation) override;

	// скорость
	float Speed{0.0f};
	sVECTOR3D RotationSpeed{0.0f, 0.0f, 0.0f};
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	// последнее положение камеры нужно для планет
	sVECTOR3D LastCameraPoint{0.0f, 0.0f, 0.0f};

	// эффекты
	std::vector<sVECTOR3D> GraphicFXLocation{};
	std::vector<std::weak_ptr<cParticleSystem>> GraphicFX{};

	// чтобы возрвать часть корабля босса пришельцев через время
	float BossPartCountDown{-1.0f};
};

class cSmallAsteroid final : public cSpaceObject
{
	friend std::weak_ptr<cSpaceObject> CreateSmallAsteroid();
private:
	// Don't allow direct new/delete usage in code, only CreateSmallAsteroid()
	// allowed for cSmallAsteroid creation and release setup (deleter must be provided).
	cSmallAsteroid();
};

class cBigAsteroid final : public cSpaceObject
{
	friend std::weak_ptr<cSpaceObject> CreateBigAsteroid(int AsteroidNum);
private:
	// Don't allow direct new/delete usage in code, only CreateBigAsteroid()
	// allowed for cBigAsteroid creation and release setup (deleter must be provided).
	explicit cBigAsteroid(int AsteroidNum);
};

class cPlanet final : public cSpaceObject
{
	friend std::weak_ptr<cSpaceObject> CreatePlanet(int PlanetNum);
private:
	// Don't allow direct new/delete usage in code, only CreatePlanet()
	// allowed for cPlanet creation and release setup (deleter must be provided).
	explicit cPlanet(int PlanetNum);
};

class cSpaceDebris final : public cSpaceObject
{
	friend std::weak_ptr<cSpaceObject> CreateSpaceDebris();
private:
	// Don't allow direct new/delete usage in code, only CreateSpaceDebris()
	// allowed for cSpaceDebris creation and release setup (deleter must be provided).
	cSpaceDebris();
};

class cBasePart final : public cSpaceObject
{
	friend std::weak_ptr<cSpaceObject> CreateBasePart(int BasePartNum);
private:
	// Don't allow direct new/delete usage in code, only CreateBasePart()
	// allowed for cBasePart creation and release setup (deleter must be provided).
	explicit cBasePart(int BasePartNum);
};


// Create cSmallAsteroid object.
std::weak_ptr<cSpaceObject> CreateSmallAsteroid();
// Create cBigAsteroid object.
std::weak_ptr<cSpaceObject> CreateBigAsteroid(int AsteroidNum);
// Create cPlanet object.
std::weak_ptr<cSpaceObject> CreatePlanet(int PlanetNum);
// Create cSpaceDebris object.
std::weak_ptr<cSpaceObject> CreateSpaceDebris();
// Create cBasePart object.
std::weak_ptr<cSpaceObject> CreateBasePart(int BasePartNum);
// Проверяем все объекты, обновляем данные
void UpdateAllSpaceObject(float Time);
// Прорисовываем все объекты
void DrawAllSpaceObjects(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular space object.
void ReleaseSpaceObject(std::weak_ptr<cSpaceObject> &Object);
// Удаляем все объекты в списке
void ReleaseAllSpaceObjects();
// установка эффекта, если нужно
void SetSpaceObjectGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXType);
// Managed cycle for each space object.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceObject(std::function<void (cSpaceObject &Object, eSpaceCycle &Command)> function);
// Managed cycle for each space object pair.
// Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
void ForEachSpaceObjectPair(std::function<void (cSpaceObject &FirstObject,
						cSpaceObject &SecondObject,
						eSpacePairCycle &Command)> function);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_SPACEOBJECT_SPACEOBJECT_H
