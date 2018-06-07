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

// TODO since we use TRIANGLES, use 4 vertices + index buffer for vw_SendVertices()
//      instead of 6 vertices, so, we send 4 vertices and index buffer for 6 elements,
//      something like {1, 2, 3, 3, 4, 1}
//                               ^  ^  ^ second triangle indexes
//                      ^  ^  ^ first triangle indexes

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../camera/camera.h"
#include "../graphics/graphics.h"
#include "../math/math.h"
#include "../light/light.h"
#include "particle_system.h"

namespace {

// Local draw buffer, that dynamically allocate memory at maximum required
// size only one time per game execution. Don't use std::vector here,
// since it have poor performance compared to std::unique_ptr.
std::unique_ptr<float[]> DrawBuffer{};
unsigned int DrawBufferCurrentPosition{0};
unsigned int DrawBufferSize{0};

// Particle system's quality (for all particle systems).
float ParticleSystemQuality{1.0f};

// Particle system's GLSL use switch (for all particle systems).
bool ParticleSystemUseGLSL{false};

// Particle system's shader (for all particle systems).
std::weak_ptr<cGLSL> ParticleSystemGLSL{};

// Uniform locations in particle system's shader (for all particle systems).
GLint UniformLocationParticleTexture{0};
GLint UniformLocationCameraPoint{0};

// All particle systems.
std::forward_list<std::shared_ptr<cParticleSystem>> ParticleSystemsList{};

} // unnamed namespace


/*
 * Update particle.
 */
bool cParticle::Update(float TimeDelta, const sVECTOR3D &ParentLocation,
		       bool Magnet, float MagnetFactor)
{
	if (Age + TimeDelta >= Lifetime) {
		Age = -1.0f;
		return false;
	} else {
		Age += TimeDelta;

		Location += Velocity ^ TimeDelta;

		if (NeedStop)
			Velocity -= Velocity ^ TimeDelta;

		if (Magnet) {
			sVECTOR3D MagnetDir = ParentLocation;
			MagnetDir -= Location;
			MagnetDir.Normalize();

			if (NeedStop)
				MagnetFactor -= MagnetFactor * TimeDelta;

			Velocity += MagnetDir ^ (MagnetFactor * TimeDelta);
		}

		Color.r += ColorDelta.r * TimeDelta;
		vw_Clamp(Color.r, 0.0f, 1.0f);
		Color.g += ColorDelta.g * TimeDelta;
		vw_Clamp(Color.g, 0.0f, 1.0f);
		Color.b += ColorDelta.b * TimeDelta;
		vw_Clamp(Color.b, 0.0f, 1.0f);

		if (!AlphaShowHide)
			Alpha += AlphaDelta * TimeDelta;
		else {
			if (Show) {
				Alpha += AlphaDelta * TimeDelta;
				if (Alpha >= 1.0f) {
					Alpha = 1.0f;
					Show = false;
				}
			} else
				Alpha -= AlphaDelta * TimeDelta;

			vw_Clamp(Alpha, 0.0f, 1.0f);
		}

		Size += SizeDelta * TimeDelta;
	}

	return true;
}

/*
 * Destructor.
 */
cParticleSystem::~cParticleSystem()
{
	vw_ReleaseLight(Light);
}

/*
 * Update all particles.
 */
bool cParticleSystem::Update(float Time)
{
	// on first update, only change TimeLastUpdate value
	if (TimeLastUpdate < 0.0f) {
		TimeLastUpdate = Time;
		return true;
	}

	float TimeDelta = Time - TimeLastUpdate;
	if (TimeDelta == 0.0f)
		return true;

	TimeLastUpdate = Time;

	// update and remove (erase) dead particles
	auto prev_iter = ParticlesList.before_begin();
	for (auto iter = ParticlesList.begin(); iter != ParticlesList.end();) {
		if (!iter->Update(TimeDelta, Location, IsMagnet, MagnetFactor)) {
			iter = ParticlesList.erase_after(prev_iter);
			ParticlesCountInList--;
		} else {
			prev_iter = iter;
			++iter;
		}
	}

	// calculate, how many particles we should emit
	float ParticlesNeeded = (static_cast<float>(ParticlesPerSec) / ParticleSystemQuality) * TimeDelta + EmissionResidue;

	// convert to integer (we can't emit 0.2 particle)
	unsigned int ParticlesCreated = static_cast<unsigned int>(ParticlesNeeded);

	if (!IsSuppressed) {
		// store emission residue for future Update() calls
		EmissionResidue = ParticlesNeeded - static_cast<float>(ParticlesCreated);
	} else {
		EmissionResidue = ParticlesNeeded;
		ParticlesCreated = 0;
	}

	// emit new particles
	if (ParticlesCreated > 0)
		EmitParticles(ParticlesCreated, TimeDelta);

	if (DestroyIfNoParticles &&
	    ParticlesList.empty())
		return false;

	// update light
	if (!Light.expired())
		UpdateLight(TimeDelta);

	// calculate current AABB
	CalculateAABB();

	return true;
}

/*
 * Emit particles.
 */
void cParticleSystem::EmitParticles(unsigned int Quantity, float TimeDelta)
{
	// care about particle system movements (need this for low FPS)
	sVECTOR3D LocationCorrection{};
	float TimeDeltaCorrection{0.0f};
	// don't calculate correction for 1, since the last one should be created in current location and current time
	if (Quantity > 1) {
		LocationCorrection = (PrevLocation - Location) / static_cast<float>(Quantity);
		TimeDeltaCorrection = TimeDelta / static_cast<float>(Quantity);
	}

	while (Quantity > 0) {
		// create new particle
		ParticlesList.emplace_front();
		// NOTE emplace_front() return reference to the inserted element (since C++17)
		//      this line could be combined with previous line, we could use
		//      ParticlesList.front() directly, but "NewParticle" usage make code more clear
		cParticle &NewParticle = ParticlesList.front();
		ParticlesCountInList++;

		// setup lifetime and age
		NewParticle.Age = 0.0f;
		NewParticle.Lifetime = Life + vw_Randf0 * LifeVar;
		if (NewParticle.Lifetime < 0.0f)
			NewParticle.Lifetime = 0.0f;

		// calculate color
		NewParticle.Color.r = ColorStart.r + vw_Randf0 * ColorVar.r;
		NewParticle.Color.g = ColorStart.g + vw_Randf0 * ColorVar.g;
		NewParticle.Color.b = ColorStart.b + vw_Randf0 * ColorVar.b;
		vw_Clamp(NewParticle.Color.r, 0.0f, 1.0f);
		vw_Clamp(NewParticle.Color.g, 0.0f, 1.0f);
		vw_Clamp(NewParticle.Color.b, 0.0f, 1.0f);
		NewParticle.ColorDelta.r = (ColorEnd.r - NewParticle.Color.r) / NewParticle.Lifetime;
		NewParticle.ColorDelta.g = (ColorEnd.g - NewParticle.Color.g) / NewParticle.Lifetime;
		NewParticle.ColorDelta.b = (ColorEnd.b - NewParticle.Color.b) / NewParticle.Lifetime;

		// calculate alpha
		NewParticle.Alpha = AlphaStart + vw_Randf0 * AlphaVar;
		vw_Clamp(NewParticle.Alpha, 0.0f, 1.0f);
		NewParticle.AlphaDelta = (AlphaEnd - NewParticle.Alpha) / NewParticle.Lifetime;
		NewParticle.AlphaShowHide = AlphaShowHide;
		if (NewParticle.AlphaShowHide) {
			NewParticle.AlphaDelta = (2.0f-AlphaEnd * 2.0f) / NewParticle.Lifetime;
			NewParticle.Alpha = AlphaEnd;
		}

		switch (CreationType) {
		case eParticleCreationType::Point:
			GenerateLocationPointType(NewParticle);
			break;

		case eParticleCreationType::Cube:
			GenerateLocationCubeType(NewParticle);
			break;

		case eParticleCreationType::Tube:
			GenerateLocationTubeType(NewParticle);
			break;

		case eParticleCreationType::Sphere:
			GenerateLocationSphereType(NewParticle);
			break;
		}

		// calculate size
		NewParticle.Size = SizeStart + vw_Randf0 * SizeVar;
		if (NewParticle.Size < 0.0f)
			NewParticle.Size = SizeStart;
		NewParticle.SizeDelta = (SizeEnd - NewParticle.Size) / NewParticle.Lifetime;
		// care about camera distance
		if (CameraDistResize < 1.0f)
			SizeCorrectionByCameraDist(NewParticle);

		if (Theta == 0.0f) {
			// emit in the same direction as particle system
			NewParticle.Velocity = Direction;
		} else {
			// emit with deviation
			NewParticle.Velocity = Direction;
			vw_RotatePoint(NewParticle.Velocity,
				       sVECTOR3D(Theta * vw_Randf0 / 2.0f, Theta * vw_Randf0 / 2.0f, 0.0f));
		}

		NewParticle.NeedStop = NeedStop;

		// calculate speed
		float NewSpeed = Speed + vw_Randf0 * SpeedVar;
		if (NewSpeed < 0.0f)
			NewSpeed = 0.0f;
		NewParticle.Velocity *= NewSpeed;

		Quantity--;

		// care about particle system movements (need this for low FPS)
		// don't change the last one, it should be created in current location and current time
		if (Quantity > 0) {
			NewParticle.Location += (LocationCorrection ^ static_cast<float>(Quantity));
			if (!NewParticle.Update(TimeDeltaCorrection * static_cast<float>(Quantity),
						Location, IsMagnet, MagnetFactor)) {
				ParticlesList.pop_front();
				ParticlesCountInList--;
			}
		}
	}
}

/*
 * Particle size correction by camera distance.
 */
void cParticleSystem::SizeCorrectionByCameraDist(cParticle &NewParticle)
{
	// current camera location
	sVECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);
	// distance from system to camera
	float SystDist = (CurrentCameraLocation.x - Location.x - CreationSize.x) *
			 (CurrentCameraLocation.x - Location.x - CreationSize.x) +
			 (CurrentCameraLocation.y - Location.y - CreationSize.y) *
			 (CurrentCameraLocation.y - Location.y - CreationSize.y) +
			 (CurrentCameraLocation.z - Location.z - CreationSize.z) *
			 (CurrentCameraLocation.z - Location.z - CreationSize.z);

	// distance to particle
	float ParticleDist = (CurrentCameraLocation.x - NewParticle.Location.x) *
			     (CurrentCameraLocation.x - NewParticle.Location.x) +
			     (CurrentCameraLocation.y - NewParticle.Location.y) *
			     (CurrentCameraLocation.y - NewParticle.Location.y) +
			     (CurrentCameraLocation.z - NewParticle.Location.z) *
			     (CurrentCameraLocation.z - NewParticle.Location.z);

	if (ParticleDist < SystDist) {
		float tmpStart = SizeStart - SizeStart * (1.0f - CameraDistResize) * (SystDist-ParticleDist) / SystDist;
		float tmpEnd = SizeEnd - SizeEnd * (1.0f - CameraDistResize) * (SystDist-ParticleDist) / SystDist;
		float tmpVar = SizeVar - SizeVar * (1.0f - CameraDistResize) * (SystDist-ParticleDist) / SystDist;

		NewParticle.Size = tmpStart + vw_Randf0 * tmpVar;
		if (NewParticle.Size < 0.0f)
			NewParticle.Size = 0.0f;
		NewParticle.SizeDelta = (tmpEnd - NewParticle.Size) / NewParticle.Lifetime;
	}
}

/*
 * Generate location for new particle (point type).
 */
void cParticleSystem::GenerateLocationPointType(cParticle &NewParticle)
{
	NewParticle.Location = Location + sVECTOR3D(vw_Randf0 * CreationSize.x,
						    vw_Randf0 * CreationSize.y,
						    vw_Randf0 * CreationSize.z);
}

/*
 * Generate location for new particle (cube type).
 */
void cParticleSystem::GenerateLocationCubeType(cParticle &NewParticle)
{
	sVECTOR3D CreationPos{(1.0f - vw_fRand() * 2) * CreationSize.x,
			      (1.0f - vw_fRand() * 2) * CreationSize.y,
			      (1.0f - vw_fRand() * 2) * CreationSize.z};

	vw_Matrix33CalcPoint(CreationPos, CurrentRotationMat);
	NewParticle.Location = Location + CreationPos;
}

/*
 * Generate location for new particle (tube type).
 */
void cParticleSystem::GenerateLocationTubeType(cParticle &NewParticle)
{
	sVECTOR3D CreationPos{(0.5f - vw_fRand()) * CreationSize.x,
			      (0.5f - vw_fRand()) * CreationSize.y,
			      (0.5f - vw_fRand()) * CreationSize.z};

	vw_Matrix33CalcPoint(CreationPos, CurrentRotationMat);
	NewParticle.Location = Location + CreationPos;
}

/*
 * Generate location for new particle (sphere type).
 */
void cParticleSystem::GenerateLocationSphereType(cParticle &NewParticle)
{
	// note, this is not really 'sphere' type, since we use
	// vector instead of radius for initial location calculation
	sVECTOR3D CreationPos{vw_Randf0 * CreationSize.x,
			      vw_Randf0 * CreationSize.y,
			      vw_Randf0 * CreationSize.z};

	if (DeadZone > 0.0f) {
		float ParticleDist2 = CreationPos.x * CreationPos.x +
				      CreationPos.y * CreationPos.y +
				      CreationPos.z * CreationPos.z;

		if (ParticleDist2 < DeadZone * DeadZone)
			CreationPos *= DeadZone / vw_sqrtf(ParticleDist2);

		vw_Clamp(CreationPos.x, -CreationSize.x, CreationSize.x);
		vw_Clamp(CreationPos.y, -CreationSize.y, CreationSize.y);
		vw_Clamp(CreationPos.z, -CreationSize.z, CreationSize.z);
	}

	vw_Matrix33CalcPoint(CreationPos, CurrentRotationMat);
	NewParticle.Location = Location + CreationPos;
}

/*
 * Update light.
 */
void cParticleSystem::UpdateLight(float TimeDelta)
{
	if (!IsSuppressed) {
		if (LightNeedDeviation) {
			// turn on (was turned off)
			if (LightDeviation > 1.0f)
				LightDeviation = 0.7f;

			if (NextLightDeviation < LightDeviation) {
				LightDeviation -= LightDeviationSpeed * TimeDelta;
				if (NextLightDeviation >= LightDeviation) {
					LightDeviation = NextLightDeviation;
					LightDeviationSpeed = 3.5f + 3.5f * vw_fRand();
					NextLightDeviation = 0.7f + 0.3f * vw_fRand();
				}
			} else {
				LightDeviation += LightDeviationSpeed * TimeDelta;
				if (NextLightDeviation <= LightDeviation) {
					LightDeviation = NextLightDeviation;
					LightDeviationSpeed = 3.5f + 3.5f * vw_fRand();
					NextLightDeviation = 0.7f - 0.3f * vw_fRand();
				}
			}
		} else
			// turn on (was turned off)
			if (LightDeviation > 1.0f)
				LightDeviation = 1.0f;
	} else {
		// turn off (was turned on)
		if (IsSuppressed && (Life != 0.0f))
			LightDeviation += (25.0f / Life) * TimeDelta;
	}

	if (auto sharedLight = Light.lock()) {
		if (sharedLight->LinearAttenuationBase != 0.0f) {
			sharedLight->LinearAttenuation = sharedLight->LinearAttenuationBase +
						   2.0f * (LightDeviation - 0.5f) * sharedLight->LinearAttenuationBase;
			if (sharedLight->LinearAttenuation < sharedLight->LinearAttenuationBase)
				sharedLight->LinearAttenuation = sharedLight->LinearAttenuationBase;
		}
		if (sharedLight->QuadraticAttenuationBase != 0.0f) {
			sharedLight->QuadraticAttenuation = sharedLight->QuadraticAttenuationBase +
						      2.0f * (LightDeviation - 0.5f) * sharedLight->QuadraticAttenuationBase;
			if (sharedLight->QuadraticAttenuation < sharedLight->QuadraticAttenuationBase)
				sharedLight->QuadraticAttenuation = sharedLight->QuadraticAttenuationBase;
		}

		// if we don't have particles, turn off light
		sharedLight->On = !ParticlesList.empty();
	}
}

/*
 * Calculate current AABB.
 */
void cParticleSystem::CalculateAABB()
{
	// initial setup
	float MinX, MinY, MinZ, MaxX, MaxY, MaxZ;
	if (ParticlesList.empty()) {
		MinX = MaxX = Location.x;
		MinY = MaxY = Location.y;
		MinZ = MaxZ = Location.z;
	} else {
		MinX = MaxX = ParticlesList.front().Location.x;
		MinY = MaxY = ParticlesList.front().Location.y;
		MinZ = MaxZ = ParticlesList.front().Location.z;
	}

	// calculate AABB
	for (auto &tmpParticle : ParticlesList) {
		if (tmpParticle.Alpha > 0.0f && tmpParticle.Size > 0.0f) {
			sVECTOR3D v;
			v.x = tmpParticle.Location.x + tmpParticle.Size;
			if (MaxX < v.x)
				MaxX = v.x;

			v.y = tmpParticle.Location.y + tmpParticle.Size;
			if (MaxY < v.y)
				MaxY = v.y;

			v.z = tmpParticle.Location.z + tmpParticle.Size;
			if (MaxZ < v.z)
				MaxZ = v.z;

			v.x = tmpParticle.Location.x - tmpParticle.Size;
			if (MinX > v.x)
				MinX = v.x;

			v.y = tmpParticle.Location.y - tmpParticle.Size;
			if (MinY > v.y)
				MinY = v.y;

			v.z = tmpParticle.Location.z - tmpParticle.Size;
			if (MinZ > v.z)
				MinZ = v.z;
		}
	}

	AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
	AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
	AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
	AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
	AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
	AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
	AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
	AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);
}

/*
 * Add data to local draw buffer.
 * Note, in case of GLSL, we use TextureU_or_GLSL and TextureV_or_GLSL
 * not for texture coordinates, but for GLSL program parameters.
 */
static inline void AddToDrawBuffer(float CoordX, float CoordY, float CoordZ,
				   const sRGBCOLOR &Color, float Alpha,
				   float TextureU_or_GLSL, float TextureV_or_GLSL)
{
	DrawBuffer[DrawBufferCurrentPosition++] = CoordX;
	DrawBuffer[DrawBufferCurrentPosition++] = CoordY;
	DrawBuffer[DrawBufferCurrentPosition++] = CoordZ;
	DrawBuffer[DrawBufferCurrentPosition++] = Color.r;
	DrawBuffer[DrawBufferCurrentPosition++] = Color.g;
	DrawBuffer[DrawBufferCurrentPosition++] = Color.b;
	DrawBuffer[DrawBufferCurrentPosition++] = Alpha;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureU_or_GLSL;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureV_or_GLSL;
}

/*
 * Draw all particles.
 */
void cParticleSystem::Draw(GLtexture &CurrentTexture)
{
	if (!vw_BoxInFrustum(AABB[6], AABB[0]) ||
	    ParticlesList.empty())
		return;

	// TRIANGLES * (RI_3f_XYZ + RI_2f_TEX + RI_4f_COLOR) * ParticlesCount
	unsigned int tmpDrawBufferSize = 6 * (3 + 2 + 4) * ParticlesCountInList;
	if (tmpDrawBufferSize > DrawBufferSize) {
		DrawBufferSize = tmpDrawBufferSize;
		DrawBuffer.reset(new float[DrawBufferSize]);
	}
	DrawBufferCurrentPosition = 0;

	// without shaders, we need manually rotate each particle to camera
	if (!ParticleSystemUseGLSL) {
		sVECTOR3D CurrentCameraLocation{vw_GetCameraLocation(nullptr)};

		for (auto &tmpParticle : ParticlesList) {
			sVECTOR3D nnTmp{CurrentCameraLocation - tmpParticle.Location};

			// perpendicular to vector nnTmp
			sVECTOR3D nnTmp2{1.0f, 1.0f, -(nnTmp.x + nnTmp.y) / nnTmp.z};
			nnTmp2.Normalize();

			// perpendicular to vectors nnTmp and nnTmp2
			// a x b = ( aybz - byaz , azbx - bzax , axby - bxay );
			sVECTOR3D nnTmp3{nnTmp.y * nnTmp2.z - nnTmp2.y * nnTmp.z,
					 nnTmp.z * nnTmp2.x - nnTmp2.z * nnTmp.x,
					 nnTmp.x * nnTmp2.y - nnTmp2.x * nnTmp.y};
			nnTmp3.Normalize();

			sVECTOR3D tmpAngle1 = nnTmp3 ^ (tmpParticle.Size * 1.5f);
			sVECTOR3D tmpAngle3 = nnTmp3 ^ (-tmpParticle.Size * 1.5f);
			sVECTOR3D tmpAngle2 = nnTmp2 ^ (tmpParticle.Size * 1.5f);
			sVECTOR3D tmpAngle4 = nnTmp2 ^ (-tmpParticle.Size * 1.5f);

			// first triangle
			AddToDrawBuffer(tmpParticle.Location.x + tmpAngle3.x,
					tmpParticle.Location.y + tmpAngle3.y,
					tmpParticle.Location.z + tmpAngle3.z,
					tmpParticle.Color, tmpParticle.Alpha,
					0.0f, 1.0f);
			AddToDrawBuffer(tmpParticle.Location.x + tmpAngle2.x,
					tmpParticle.Location.y + tmpAngle2.y,
					tmpParticle.Location.z + tmpAngle2.z,
					tmpParticle.Color, tmpParticle.Alpha,
					0.0f, 0.0f);
			AddToDrawBuffer(tmpParticle.Location.x + tmpAngle1.x,
					tmpParticle.Location.y + tmpAngle1.y,
					tmpParticle.Location.z + tmpAngle1.z,
					tmpParticle.Color, tmpParticle.Alpha,
					1.0f, 0.0f);

			//second triangle
			AddToDrawBuffer(tmpParticle.Location.x + tmpAngle1.x,
					tmpParticle.Location.y + tmpAngle1.y,
					tmpParticle.Location.z + tmpAngle1.z,
					tmpParticle.Color, tmpParticle.Alpha,
					1.0f, 0.0f);
			AddToDrawBuffer(tmpParticle.Location.x + tmpAngle4.x,
					tmpParticle.Location.y + tmpAngle4.y,
					tmpParticle.Location.z + tmpAngle4.z,
					tmpParticle.Color, tmpParticle.Alpha,
					1.0f, 1.0f);
			AddToDrawBuffer(tmpParticle.Location.x + tmpAngle3.x,
					tmpParticle.Location.y + tmpAngle3.y,
					tmpParticle.Location.z + tmpAngle3.z,
					tmpParticle.Color, tmpParticle.Alpha,
					0.0f, 1.0f);
		}
	} else {
		// shader will care about particle rotation
		// instead of textures coordinates, provide to shader vertex number (in triangle)
		// and particle size, shader will care about rotation and proper texture's coordinates
		for (auto &tmpParticle : ParticlesList) {
			// first triangle
			AddToDrawBuffer(tmpParticle.Location.x, tmpParticle.Location.y, tmpParticle.Location.z,
					tmpParticle.Color, tmpParticle.Alpha,
					1.0f, tmpParticle.Size);
			AddToDrawBuffer(tmpParticle.Location.x, tmpParticle.Location.y, tmpParticle.Location.z,
					tmpParticle.Color, tmpParticle.Alpha,
					2.0f, tmpParticle.Size);
			AddToDrawBuffer(tmpParticle.Location.x, tmpParticle.Location.y, tmpParticle.Location.z,
					tmpParticle.Color, tmpParticle.Alpha,
					3.0f, tmpParticle.Size);

			//second triangle
			AddToDrawBuffer(tmpParticle.Location.x, tmpParticle.Location.y, tmpParticle.Location.z,
					tmpParticle.Color,  tmpParticle.Alpha,
					3.0f, tmpParticle.Size);
			AddToDrawBuffer(tmpParticle.Location.x, tmpParticle.Location.y, tmpParticle.Location.z,
					tmpParticle.Color, tmpParticle.Alpha,
					4.0f, tmpParticle.Size);
			AddToDrawBuffer(tmpParticle.Location.x, tmpParticle.Location.y, tmpParticle.Location.z,
					tmpParticle.Color, tmpParticle.Alpha,
					1.0f, tmpParticle.Size);
		}
	}

	// if we already setup this texture in previous rendered particle system, no need to change it
	if (CurrentTexture != Texture) {
		vw_BindTexture(0, Texture);
		CurrentTexture = Texture;
	}

	if (TextureBlend)
		vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);
	else
		vw_SetTextureBlend(true, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE);

	vw_Draw3D(ePrimitiveType::TRIANGLES, 6 * ParticlesCountInList, RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX,
		  DrawBuffer.get(), 9 * sizeof(DrawBuffer.get()[0]));

	vw_SetTextureBlend(true, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
}

/*
 * Set start system location.
 */
void cParticleSystem::SetStartLocation(const sVECTOR3D &NewLocation)
{
	Location = NewLocation;
	PrevLocation = Location;

	if (auto sharedLight = Light.lock())
		sharedLight->SetLocation(Location);
}

/*
 * Move all particles and system location.
 */
void cParticleSystem::MoveSystem(const sVECTOR3D &NewLocation)
{
	if (Location == NewLocation)
		return;

	sVECTOR3D tmpLocation{NewLocation - Location};
	Location = NewLocation;
	PrevLocation = Location;

	for (auto &tmpParticle : ParticlesList) {
		tmpParticle.Location += tmpLocation;
	}

	if (auto sharedLight = Light.lock())
		sharedLight->SetLocation(Location);
}

/*
 * Move system location.
 */
void cParticleSystem::MoveSystemLocation(const sVECTOR3D &NewLocation)
{
	if (Location == NewLocation)
		return;

	PrevLocation = Location;
	Location = NewLocation;

	if (auto sharedLight = Light.lock())
		sharedLight->SetLocation(Location);
}

/*
 * Rotate system.
 */
void cParticleSystem::RotateSystemByAngle(const sVECTOR3D &NewAngle)
{
	Angle = NewAngle;

	// generate inverse rotation matrix
	memcpy(OldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Angle);

	vw_Matrix33CalcPoint(Direction, OldInvRotationMat);
	vw_Matrix33CalcPoint(Direction, CurrentRotationMat);
}

/*
 * Rotate all particles and system.
 */
void cParticleSystem::RotateSystemAndParticlesByAngle(const sVECTOR3D &NewAngle)
{
	Angle = NewAngle;

	// generate inverse rotation matrix
	memcpy(OldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Angle);

	vw_Matrix33CalcPoint(Direction, OldInvRotationMat);
	vw_Matrix33CalcPoint(Direction, CurrentRotationMat);

	for (auto &tmpParticle : ParticlesList) {
		sVECTOR3D TMP = tmpParticle.Location - Location;
		vw_Matrix33CalcPoint(TMP, OldInvRotationMat);
		vw_Matrix33CalcPoint(TMP, CurrentRotationMat);
		tmpParticle.Location = TMP + Location;
	}
}

/*
 * Rotate all particles in system.
 */
void cParticleSystem::RotateParticlesByAngle(const sVECTOR3D &NewAngle)
{
	// generate inverse rotation matrix
	float TmpOldInvRotationMat[9];
	memcpy(TmpOldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	vw_Matrix33InverseRotate(TmpOldInvRotationMat);

	// generate rotation matrix
	float TmpRotationMat[9];
	vw_Matrix33CreateRotate(TmpRotationMat, NewAngle);

	for (auto &tmpParticle : ParticlesList) {
		sVECTOR3D TMP = tmpParticle.Location - Location;
		vw_Matrix33CalcPoint(TMP, TmpOldInvRotationMat);

		vw_Matrix33CalcPoint(TMP, TmpRotationMat);
		vw_Matrix33CalcPoint(TMP, CurrentRotationMat);
		tmpParticle.Location = TMP + Location;
	}
}

/*
 * Stop all particles in system.
 */
void cParticleSystem::StopAllParticles()
{
	Speed = 0.0f;
	IsMagnet = false;

	for (auto &tmpParticle : ParticlesList) {
		tmpParticle.Velocity = sVECTOR3D(0.0f,0.0f,0.0f);
	}
}

/*
 * Change speed for all particles.
 */
void cParticleSystem::ChangeSpeed(const sVECTOR3D &Vel)
{
	for (auto &tmpParticle : ParticlesList) {
		tmpParticle.Velocity += Vel;
	}
}

/*
 * Initialization. 'Quality' is particle emission factor from 1.0f.
 */
void vw_InitParticleSystems(bool UseGLSL, float Quality)
{
	ParticleSystemUseGLSL = UseGLSL;
	ParticleSystemQuality = Quality;

	// find proper shaders and setup uniforms locations
	if (ParticleSystemUseGLSL) {
		ParticleSystemGLSL = vw_FindShaderByName("ParticleSystem");
		UniformLocationParticleTexture = vw_GetUniformLocation(ParticleSystemGLSL, "ParticleTexture");
		UniformLocationCameraPoint = vw_GetUniformLocation(ParticleSystemGLSL, "CameraPoint");
	}
}

/*
 * Create particle system.
 * Note, we don't provide shared_ptr, only weak_ptr, since all memory management
 * should be internal only. Caller should operate with weak_ptr and use lock()
 * (shared_ptr) only during access to object.
 */
std::weak_ptr<cParticleSystem> vw_CreateParticleSystem()
{
	// NOTE emplace_front() return reference to the inserted element (since C++17)
	//      this two lines could be combined
	ParticleSystemsList.emplace_front(new cParticleSystem, [](cParticleSystem *p) {delete p;});
	return ParticleSystemsList.front();
}

/*
 * Release particle system, provided by shared_ptr.
 * We remove particle system from the list (release shared_ptr), but real release
 * will be done as soon, as caller will release shared_ptr on this particle system.
 */
void vw_ReleaseParticleSystem(std::shared_ptr<cParticleSystem> &ParticleSystem)
{
	auto prev_iter = ParticleSystemsList.before_begin();
	for (auto iter = ParticleSystemsList.begin(); iter != ParticleSystemsList.end();) {
		if (*iter == ParticleSystem) {
			ParticleSystemsList.erase_after(prev_iter);
			return;
		}
		prev_iter = iter;
		++iter;
	}
}

/*
 * Release particle system, provided by weak_ptr.
 */
void vw_ReleaseParticleSystem(std::weak_ptr<cParticleSystem> &ParticleSystem)
{
	if (auto sharedParticleSystem = ParticleSystem.lock())
		vw_ReleaseParticleSystem(sharedParticleSystem);
}

/*
 * Release all particle systems.
 */
void vw_ReleaseAllParticleSystems()
{
	ParticleSystemsList.clear();

	ParticleSystemUseGLSL = false;
	ParticleSystemQuality = 1.0f;
	ParticleSystemGLSL.reset();
}

/*
 * Draw all particle systems.
 */
void vw_DrawAllParticleSystems()
{
	// current texture
	// we store current texture in order to minimize texture's states changes
	GLtexture CurrentTexture{0};

	// setup shaders
	if (ParticleSystemUseGLSL && !ParticleSystemGLSL.expired()) {
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		vw_UseShaderProgram(ParticleSystemGLSL);
		vw_Uniform1i(UniformLocationParticleTexture, 0);
		vw_Uniform3f(UniformLocationCameraPoint,
			     CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
	}
	glDepthMask(GL_FALSE);

	for (auto &tmpParticleSystems : ParticleSystemsList) {
		tmpParticleSystems->Draw(CurrentTexture);
	}

	// reset rendering states
	glDepthMask(GL_TRUE);
	if (ParticleSystemUseGLSL)
		vw_StopShaderProgram();
	vw_BindTexture(0, 0);
}

/*
 * Draw particle systems block, provided by caller.
 */
void vw_DrawParticleSystems(std::vector<std::weak_ptr<cParticleSystem>> &DrawParticleSystem)
{
	if (DrawParticleSystem.empty())
		return;

	// current texture
	// we store current texture in order to minimize texture's states changes
	GLtexture CurrentTexture{0};

	// setup shaders
	if (ParticleSystemUseGLSL && !ParticleSystemGLSL.expired()) {
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		vw_UseShaderProgram(ParticleSystemGLSL);
		vw_Uniform1i(UniformLocationParticleTexture, 0);
		vw_Uniform3f(UniformLocationCameraPoint,
			     CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
	}
	glDepthMask(GL_FALSE);

	for (auto &tmpParticleSystem : DrawParticleSystem) {
		if (auto sharedParticleSystem = tmpParticleSystem.lock())
			sharedParticleSystem->Draw(CurrentTexture);
	}

	// reset rendering states
	glDepthMask(GL_TRUE);
	if (ParticleSystemUseGLSL)
		vw_StopShaderProgram();
	vw_BindTexture(0, 0);
}

/*
 * Update all particle systems.
 */
void vw_UpdateAllParticleSystems(float Time)
{
	auto prev_iter = ParticleSystemsList.before_begin();
	for (auto iter = ParticleSystemsList.begin(); iter != ParticleSystemsList.end();) {
		if (!(*iter)->Update(Time))
			iter = ParticleSystemsList.erase_after(prev_iter);
		else {
			prev_iter = iter;
			++iter;
		}
	}
}
