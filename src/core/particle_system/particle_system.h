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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef CORE_PARTICLESYSTEM_PARTICLESYSTEM_H
#define CORE_PARTICLESYSTEM_PARTICLESYSTEM_H

#include "../base.h"

struct sVECTOR3D;
class cLight;

enum class eParticleCreationType {
	Point,
	Cube,
	Tube,	// we need this for maser/laser
	Sphere
};

class cParticle {
	friend class cParticleSystem;

private:
	// Update particle.
	bool Update(float TimeDelta, const sVECTOR3D &ParentLocation = sVECTOR3D{0.0f, 0.0f, 0.0f},
		    bool Magnet = false, float MagnetFactor = 25.0f);

	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	bool NeedStop{false};	// in case we need slow down and stop particles

	// color-related
	sRGBCOLOR Color{1.0f, 0.0f, 0.0f};
	sRGBCOLOR ColorDelta{0.0f, 0.0f, 0.0f};

	float Age{0.0f};	// age in seconds
	float Lifetime{0.0f};	// lifetime in seconds

	// size-related
	float Size{1.0f};
	float SizeDelta{0.0f};

	// alpha-related
	float Alpha{1.0f};
	float AlphaDelta{0.0f};

	// increase to maximum and that decrease particle alpha
	bool AlphaShowHide{false};
	// what cycle of life partition have now first (show) or last (hide)
	bool Show{true};
};

class cParticleSystem {
	friend std::weak_ptr<cParticleSystem> vw_CreateParticleSystem();

public:
	// Update all particles.
	bool Update(float Time);
	// Draw all particles.
	void Draw(GLtexture &CurrentTexture);

	GLtexture Texture{0};
	bool TextureBlend{false};	// blend (for missiles trails)

	// Set start system location.
	void SetStartLocation(const sVECTOR3D &NewLocation);
	// Move all particles and system location.
	void MoveSystem(const sVECTOR3D &NewLocation);
	// Move system location.
	void MoveSystemLocation(const sVECTOR3D &NewLocation);
	// Rotate system.
	void RotateSystemByAngle(const sVECTOR3D &NewDirection);
	// Rotate all particles and system.
	void RotateSystemAndParticlesByAngle(const sVECTOR3D &NewAngle);
	// Rotate all particles in system.
	void RotateParticlesByAngle(const sVECTOR3D &NewAngle);
	// Stop all particles in system.
	void StopAllParticles();
	// Change speed for all particles.
	void ChangeSpeed(const sVECTOR3D &Vel);

	sVECTOR3D Direction{0.0f, 0.0f, 0.0f};
	sVECTOR3D Angle{0.0f, 0.0f, 0.0f};

	unsigned int ParticlesPerSec{100};	// emission rate (particles per second)

	bool NeedStop{false};	// in case we need slow down and stop particles

	// size-related
	float SizeStart{1.0f};
	float SizeVar{0.0f};
	float SizeEnd{1.0f};

	// alpha-related
	float AlphaStart{1.0f};
	float AlphaVar{0.0f};
	float AlphaEnd{1.0f};
	bool AlphaShowHide{false};

	// color-related
	sRGBCOLOR ColorStart{1.0f, 1.0f, 1.0f};
	sRGBCOLOR ColorVar{0.0f, 0.0f, 0.0f};
	sRGBCOLOR ColorEnd{1.0f, 1.0f, 1.0f};

	// speed-related
	float Speed{1.0f};
	float SpeedVar{1.0f};
	float SpeedOnCreation{1.0f};

	eParticleCreationType CreationType{eParticleCreationType::Point};
	sVECTOR3D CreationSize{0.05f, 0.05f, 0.05f};

	float DeadZone{0.0f};	// dead zone
	// resize factor connected to camera distance (0.0f - 1.0f)
	// for Resize = 0, particle near camera will have 0 size
	// we need this feature for "space dust", that flowing near camera
	float CameraDistResize{1.0f};

	float Life{1.0f};
	float LifeVar{0.0f};

	float Theta{1.0f};		// direction deviation (in degrees) for new created particles

	bool IsMagnet{0};
	float MagnetFactor{25.0f};

	bool IsSuppressed{false};	// if suppressed, particle system can't emit new particles
	bool DestroyIfNoParticles{false};

	float Age{0.0f};		// system age in seconds

	float TimeLastUpdate{-1.0f};	// last update time

	float EmissionResidue{0.0f};	// real rest of emission

	// light-related
	std::weak_ptr<cLight> Light{};
	bool LightNeedDeviation{false};
	float LightDeviation{100.0f};
	float NextLightDeviation{0.7f + 0.3f * vw_fRand()};
	float LightDeviationSpeed{3.5f + 3.5f * vw_fRand()};

	const sVECTOR3D &GetLocation()
	{
		return Location;
	}

	// cycle for each particle in list, for external manipulations directly with particles data
	void ForEachParticle(std::function<void (sVECTOR3D &pLocation,
						 sVECTOR3D &pVelocity,
						 bool &pNeedStop)> function)
	{
		for (auto &tmpParticle : ParticlesList) {
			function(tmpParticle.Location,
				 tmpParticle.Velocity,
				 tmpParticle.NeedStop);
		}
	}

private:
	// Don't allow direct new/delete usage in code, only vw_CreateParticleSystem()
	// allowed for particle creation and release setup (deleter must be provided).
	cParticleSystem() = default;
	~cParticleSystem();

	// current location
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	// previous system location
	sVECTOR3D PrevLocation{0.0f, 0.0f, 0.0f};

	// Emit particles.
	void EmitParticles(unsigned int Quantity, float TimeDelta);
	// Particle size correction by camera distance.
	void SizeCorrectionByCameraDist(cParticle &NewParticle);
	// Generate location for new particle (point type).
	void GenerateLocationPointType(cParticle &NewParticle);
	// Generate location for new particle (cube type).
	void GenerateLocationCubeType(cParticle &NewParticle);
	// Generate location for new particle (tube type).
	void GenerateLocationTubeType(cParticle &NewParticle);
	// Generate location for new particle (sphere type).
	void GenerateLocationSphereType(cParticle &NewParticle);

	// Update light.
	void UpdateLight(float TimeDelta);

	// Calculate current AABB.
	void CalculateAABB();
	// In order to avoid rendering non visible particle system we
	// use AABB check with camera frustum.
	// Initialized with huge numbers, to be sure, that particle system
	// will be rendered for sure (in case we pre generate particles and
	// call Draw() before Update()).
	sVECTOR3D AABB[8]{{-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f}};

	// particles
	std::forward_list<cParticle> ParticlesList{};
	// we could use std::list with size(), but we don't need doubly-linked list here
	unsigned int ParticlesCountInList{0};

	// current rotation matrix for fast calculations
	float CurrentRotationMat[9]{1.0f, 0.0f, 0.0f,
				    0.0f, 1.0f, 0.0f,
				    0.0f, 0.0f, 1.0f};
	// old inversed rotation matrix for fast calculations
	float OldInvRotationMat[9]{1.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f,
				   0.0f, 0.0f, 1.0f};
};


// Initialization. 'Quality' is particle emission factor from 1.0f.
void vw_InitParticleSystems(bool UseGLSL, float Quality);
// Create particle system.
std::weak_ptr<cParticleSystem> vw_CreateParticleSystem();
// Release particle system, provided by shared_ptr.
void vw_ReleaseParticleSystem(std::shared_ptr<cParticleSystem> &ParticleSystem);
// Release particle system, provided by weak_ptr.
void vw_ReleaseParticleSystem(std::weak_ptr<cParticleSystem> &ParticleSystem);
// Release all particle systems.
void vw_ReleaseAllParticleSystems();
// Draw all particle systems.
void vw_DrawAllParticleSystems();
// Draw particle systems block, provided by caller.
void vw_DrawParticleSystems(std::vector<std::weak_ptr<cParticleSystem>> &DrawParticleSystem);
// Update all particle systems.
void vw_UpdateAllParticleSystems(float Time);

#endif // CORE_PARTICLESYSTEM_PARTICLESYSTEM_H
