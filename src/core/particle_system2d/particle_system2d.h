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

#ifndef PARTICLESYSTEM2D_H
#define PARTICLESYSTEM2D_H

#include "../base.h"

class cParticleSystem2D;

enum class eParticle2DCreationType {
	Point,
	Quad,
	Circle
};

class cParticle2D {
	friend class cParticleSystem2D;

private:
	// Update particle.
	bool Update(float TimeDelta, const sVECTOR3D &ParentLocation, bool Magnet = false, float MagnetFactor = 25.0f);

	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	sRGBCOLOR Color{1.0f, 0.0f, 0.0f};
	sRGBCOLOR ColorDelta{0.0f, 0.0f, 0.0f};

	float Age{0.0f};
	float Lifetime{0.0f};

	float Size{1.0f};
	float SizeDelta{0.0f};

	float Alpha{1.0f};
	bool NeedStop{false};
};

class cParticleSystem2D {
public:
	// Update all particles.
	void Update(float Time);
	// Draw all particles.
	void Draw();
	// Set rotation.
	void SetRotation(const sVECTOR3D &NewAngle);
	// Move center of particle system and all particles.
	void MoveSystem(const sVECTOR3D &NewLocation);
	// Move center of particle system.
	void MoveSystemLocation(const sVECTOR3D &NewLocation);

	unsigned int ParticlesPerSec{100};	// emission rate (particles per second)

	sTexture *Texture{nullptr};

	sVECTOR3D Direction{0.0f, 0.0f, 0.0f};

	float SizeStart{1.0f};
	float SizeVar{0.0f};
	float SizeEnd{1.0f};

	float AlphaStart{1.0f};
	float AlphaVar{0.0f};
	float AlphaEnd{1.0f};

	sRGBCOLOR ColorStart{1.0f, 1.0f, 1.0f};
	sRGBCOLOR ColorVar{0.0f, 0.0f, 0.0f};
	sRGBCOLOR ColorEnd{1.0f, 1.0f, 1.0f};

	float Speed{1.0f};
	float SpeedVar{1.0f};
	float SpeedOnCreation{1.0f};

	eParticle2DCreationType CreationType{eParticle2DCreationType::Point};
	sVECTOR3D CreationSize{0.05f, 0.05f, 0.05f};

	float Life{1.0f};
	float LifeVar{0.0f};

	bool IsMagnet{false};		// is particle system have magnet effect?
	float MagnetFactor{25.0f};	// magnet factor

	float DeadZone{0.0f};		// dead zone
	float Theta{1.0f};		// direction deviation (in degrees) for new created particles
	bool IsSuppressed{false};	// if suppressed, particle system can't emit new particles

private:
	// Emit particles.
	void EmitParticles(unsigned int Quantity);
	// Setup new particle direction.
	void SetupNewParticleDirection(cParticle2D &NewParticle);
	// Generate location for new particle (point type).
	void GenerateLocationPointType(cParticle2D &NewParticle);
	// Generate location for new particle (quad type).
	void GenerateLocationQuadType(cParticle2D &NewParticle);
	// Generate location for new particle (circle type).
	void GenerateLocationCircleType(cParticle2D &NewParticle);

	float RotationMatrix[9]{1.0f, 0.0f, 0.0f, // current rotation matrix
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f};

	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	sVECTOR3D Angle{0.0f, 0.0f, 0.0f};

	float TimeLastUpdate{-1.0f};
	float EmissionResidue{0.0f};	// emission residue, for next update cycle

	std::list<cParticle2D> ParticlesList;
};


// Create new particle system 2D.
cParticleSystem2D *vw_CreateParticleSystem2D();
// Release all particle systems 2D.
void vw_ReleaseAllParticleSystems2D();

#endif //PARTICLESYSTEM2D_H
