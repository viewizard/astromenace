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

#ifndef OBJECT3D_SPACEOBJECT_SPACEOBJECT_H
#define OBJECT3D_SPACEOBJECT_SPACEOBJECT_H

#include "../object3d.h"

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

class cSpaceObject : public cObject3D {
protected:
	// don't allow object of this class creation
	cSpaceObject();
	virtual ~cSpaceObject() = default;

public:
	virtual bool Update(float Time) override;

	float Speed{0.0f};
	sVECTOR3D RotationSpeed{0.0f, 0.0f, 0.0f};
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	sVECTOR3D LastCameraPoint{0.0f, 0.0f, 0.0f};

	float BossPartCountDown{-1.0f};
};

class cSmallAsteroid final : public cSpaceObject
{
	friend std::weak_ptr<cSpaceObject> CreateSmallAsteroid();
private:
	// Don't allow direct new/delete usage in code, only CreateSmallAsteroid()
	// allowed for cSmallAsteroid creation and release setup (deleter must be provided).
	cSmallAsteroid();
	~cSmallAsteroid() = default;
};

class cBigAsteroid final : public cSpaceObject {
	friend std::weak_ptr<cSpaceObject> CreateBigAsteroid(const int AsteroidNum);

private:
	// Don't allow direct new/delete usage in code, only CreateBigAsteroid()
	// allowed for cBigAsteroid creation and release setup (deleter must be provided).
	explicit cBigAsteroid(const int AsteroidNum);
	~cBigAsteroid() = default;
};

class cPlanet final : public cSpaceObject {
	friend std::weak_ptr<cSpaceObject> CreatePlanet(const int PlanetNum);
private:
	// Don't allow direct new/delete usage in code, only CreatePlanet()
	// allowed for cPlanet creation and release setup (deleter must be provided).
	explicit cPlanet(const int PlanetNum);
	~cPlanet() = default;
};

class cPlanetoid final : public cSpaceObject {
	friend std::weak_ptr<cSpaceObject> CreatePlanetoid(const int PlanetoidNum);
private:
	// Don't allow direct new/delete usage in code, only CreatePlanetoid()
	// allowed for cPlanetoid creation and release setup (deleter must be provided).
	explicit cPlanetoid(const int PlanetoidNum);
	~cPlanetoid() = default;
};

class cSpaceDebris final : public cSpaceObject {
	friend std::weak_ptr<cSpaceObject> CreateSpaceDebris();
private:
	// Don't allow direct new/delete usage in code, only CreateSpaceDebris()
	// allowed for cSpaceDebris creation and release setup (deleter must be provided).
	cSpaceDebris();
	~cSpaceDebris() = default;
};

class cBasePart final : public cSpaceObject {
	friend std::weak_ptr<cSpaceObject> CreateBasePart(const int BasePartNum);
private:
	// Don't allow direct new/delete usage in code, only CreateBasePart()
	// allowed for cBasePart creation and release setup (deleter must be provided).
	explicit cBasePart(const int BasePartNum);
	~cBasePart();

	void SetupBasePartType1();
	void SetupBasePartType2();
	void SetupBasePartType3();
	void SetupBasePartType4();
	void SetupBasePartType5();
	void SetupBasePartType6();
	void SetupBasePartType7();
	void SetupBasePartType8();

	void SetLocation(const sVECTOR3D &NewLocation) override;
	void SetRotation(const sVECTOR3D &NewRotation) override;

	// engine gfx
	std::vector<sVECTOR3D> GraphicFXLocation{}; // relative to 3d model center
	std::vector<std::weak_ptr<cParticleSystem>> GraphicFX{};
};


// Create cSmallAsteroid object.
std::weak_ptr<cSpaceObject> CreateSmallAsteroid();
// Create cBigAsteroid object.
std::weak_ptr<cSpaceObject> CreateBigAsteroid(const int AsteroidNum);
// Create cPlanet object.
std::weak_ptr<cSpaceObject> CreatePlanet(const int PlanetNum);
// Create cPlanetoid object.
std::weak_ptr<cSpaceObject> CreatePlanetoid(const int PlanetoidNum);
// Create cSpaceDebris object.
std::weak_ptr<cSpaceObject> CreateSpaceDebris();
// Create cBasePart object.
std::weak_ptr<cSpaceObject> CreateBasePart(const int BasePartNum);
// Update and remove (erase) dead objects.
void UpdateAllSpaceObject(float Time);
// Draw all objects.
void DrawAllSpaceObjects(bool VertexOnlyPass, unsigned int ShadowMap);
// Release particular space object.
void ReleaseSpaceObject(std::weak_ptr<cSpaceObject> &Object);
// Release all objects.
void ReleaseAllSpaceObjects();
// Cycle for each space object.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceObject(std::function<void (cSpaceObject &Object)> function);
// Managed cycle for each space object.
// Note, caller must guarantee, that 'Object' will not released in callback function call.
void ForEachSpaceObject(std::function<void (cSpaceObject &Object, eSpaceCycle &Command)> function);
// Managed cycle for each space object pair.
// Note, caller must guarantee, that 'FirstObject' and 'SecondObject' will not released in callback function call.
void ForEachSpaceObjectPair(std::function<void (cSpaceObject &FirstObject,
						cSpaceObject &SecondObject,
						eSpacePairCycle &Command)> function);
// Get object ptr by reference.
std::weak_ptr<cObject3D> GetSpaceObjectPtr(const cSpaceObject &Object);

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_SPACEOBJECT_SPACEOBJECT_H
