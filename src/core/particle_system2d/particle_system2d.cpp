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

// TODO since we use RI_TRIANGLES, use 4 vertices + index buffer for vw_SendVertices()
//      instead of 6 vertices, so, we send 4 vertices and index buffer for 6 elements,
//      something like {1, 2, 3, 3, 4, 1}
//                               ^  ^  ^ second triangle indexes
//                      ^  ^  ^ first triangle indexes

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../math/math.h"
#include "../graphics/graphics.h"
#include "../texture/texture.h"
#include "particle_system2d.h"

namespace {

// Local draw buffer, that dynamically allocate memory at maximum required
// size only one time per game execution. Don't use std::vector here,
// since it have poor performance compared to std::unique_ptr.
std::unique_ptr<float []> DrawBuffer{};
unsigned int DrawBufferCurrentPosition{0};
unsigned int DrawBufferSize{0};

// std::forward_list, since we operate directly via pointers and
// don't really care about erase/access to particular element
std::forward_list<cParticleSystem2D> ParticleSystemsList;

} // unnamed namespace


/*
 * Update particle.
 */
bool cParticle2D::Update(float TimeDelta, const sVECTOR3D &ParentLocation, bool Magnet, float MagnetFactor)
{
	if (Age + TimeDelta >= Lifetime)
		return false;

	Age += TimeDelta;

	Location += Velocity ^ TimeDelta;

	if (NeedStop)
		Velocity -= Velocity ^ TimeDelta;

	if (Magnet) {
		sVECTOR3D tmpDirection{ParentLocation};
		tmpDirection -= Location;
		tmpDirection.Normalize();

		if (NeedStop)
			MagnetFactor -= MagnetFactor * TimeDelta;

		Velocity += tmpDirection ^ (MagnetFactor * TimeDelta);
	}

	Color.r += ColorDelta.r * TimeDelta;
	Color.g += ColorDelta.g * TimeDelta;
	Color.b += ColorDelta.b * TimeDelta;

	Size += SizeDelta * TimeDelta;

	return true;
}

/*
 * Update all particles.
 */
void cParticleSystem2D::Update(float Time)
{
	// on first update, only change TimeLastUpdate value
	if (TimeLastUpdate == -1.0f) {
		TimeLastUpdate = Time;
		return;
	}

	float TimeDelta = Time - TimeLastUpdate;
	TimeLastUpdate = Time;

	// update and remove (erase) dead particles
	for (auto iter = ParticlesList.begin(); iter != ParticlesList.end();) {
		if (!iter->Update(TimeDelta, Location, IsMagnet, MagnetFactor))
			iter = ParticlesList.erase(iter);
		else
			++iter;
	}

	// calculate, how many particles we should emit
	float ParticlesNeeded = ParticlesPerSec * TimeDelta + EmissionResidue;

	// convert to integer (we can't emit 0.2 particle)
	unsigned int ParticlesCreated = (unsigned int)ParticlesNeeded;
	// store emission residue for future Update() calls
	if (!IsSuppressed)
		EmissionResidue = ParticlesNeeded - ParticlesCreated;
	else {
		EmissionResidue = ParticlesNeeded;
		ParticlesCreated = 0;
	}

	// nothing to do
	if (ParticlesCreated < 1)
		return;

	// prevent 'time's lags' issue, when we need emit huge number of particles
	// that more than our ParticlesPerSec value
	if (ParticlesCreated > ParticlesPerSec)
		ParticlesCreated = ParticlesPerSec;

	// emit particles
	EmitParticles(ParticlesCreated);

	return;
}

/*
 * Emit particles.
 */
void cParticleSystem2D::EmitParticles(unsigned int Quantity)
{
	while (Quantity > 0) {
		// create new particle
		ParticlesList.emplace_back();
		// NOTE emplace_back() return reference to the inserted element (since C++17)
		//      this line could be combined with previous line, we could use
		//      ParticlesList.back() directly, but "NewParticle" usage make code more clear
		cParticle2D &NewParticle = ParticlesList.back();

		NewParticle.Age = 0.0f;
		NewParticle.Lifetime = Life + vw_Randf0 * LifeVar;
		if (NewParticle.Lifetime < 0.0f)
			NewParticle.Lifetime = 0.0f;

		// generate color
		NewParticle.Color.r = ColorStart.r + vw_Randf0 * ColorVar.r;
		NewParticle.Color.g = ColorStart.g + vw_Randf0 * ColorVar.g;
		NewParticle.Color.b = ColorStart.b + vw_Randf0 * ColorVar.b;
		vw_Clamp(NewParticle.Color.r, 0.0f, 1.0f);
		vw_Clamp(NewParticle.Color.g, 0.0f, 1.0f);
		vw_Clamp(NewParticle.Color.b, 0.0f, 1.0f);

		// calculate delta
		NewParticle.ColorDelta.r = (ColorEnd.r - NewParticle.Color.r) / NewParticle.Lifetime;
		NewParticle.ColorDelta.g = (ColorEnd.g - NewParticle.Color.g) / NewParticle.Lifetime;
		NewParticle.ColorDelta.b = (ColorEnd.b - NewParticle.Color.b) / NewParticle.Lifetime;

		// generate alpha
		NewParticle.Alpha = AlphaStart + vw_Randf0 * AlphaVar;
		vw_Clamp(NewParticle.Alpha, 0.0f, 1.0f);

		switch (CreationType) {
		case eParticle2DCreationType::Point:
			GenerateLocationPointType(NewParticle);
			break;

		case eParticle2DCreationType::Quad:
			GenerateLocationQuadType(NewParticle);
			break;

		case eParticle2DCreationType::Circle:
			GenerateLocationCircleType(NewParticle);
			break;
		}

		NewParticle.Size = SizeStart + vw_Randf0 * SizeVar;
		if (NewParticle.Size < 0.0f)
			NewParticle.Size = 0.0f;
		NewParticle.SizeDelta = (SizeEnd - NewParticle.Size) / NewParticle.Lifetime;

		SetupNewParticleDirection(NewParticle);

		// calculate velocity
		float NewSpeed = Speed + vw_Randf0 * SpeedVar;
		if (NewSpeed < 0.0f)
			NewSpeed = 0.0f;
		NewParticle.Velocity *= NewSpeed;

		Quantity--;
	}
}

/*
 * Generate location for new particle (point type).
 */
void cParticleSystem2D::GenerateLocationPointType(cParticle2D &NewParticle)
{
	NewParticle.Location = Location + sVECTOR3D(vw_Randf0 * CreationSize.x,
						    vw_Randf0 * CreationSize.y,
						    vw_Randf0 * CreationSize.z);
}

/*
 * Generate location for new particle (quad type).
 */
void cParticleSystem2D::GenerateLocationQuadType(cParticle2D &NewParticle)
{
	sVECTOR3D CreationPos{(1.0f - vw_Randf1 * 2) * CreationSize.x,
			      (1.0f - vw_Randf1 * 2) * CreationSize.y,
			      (1.0f - vw_Randf1 * 2) * CreationSize.z};

	if (DeadZone != 0.0f) {
		float tmpDist2 = CreationSize.x * CreationSize.x +
				 CreationSize.y * CreationSize.y +
				 CreationSize.z * CreationSize.z;
		// if DeadZone^2 more then CreationSize^2, disable DeadZone
		float DeadZone2 = DeadZone * DeadZone;
		if (tmpDist2 <= DeadZone2)
			DeadZone = 0.0f;
		else {
			while ((CreationPos.x * CreationPos.x +
				CreationPos.y * CreationPos.y +
				CreationPos.z * CreationPos.z) < DeadZone2) {
				// increase distance
				sVECTOR3D tmpPosInc = CreationPos;
				tmpPosInc.Normalize();
				tmpPosInc = tmpPosInc ^ (1 / 100.0f); // increase distance on 1%
				CreationPos += tmpPosInc;
			}
		}
	}

	NewParticle.Location = Location + CreationPos;
}

/*
 * Generate location for new particle (circle type).
 */
void cParticleSystem2D::GenerateLocationCircleType(cParticle2D &NewParticle)
{
	float tmpDist2 = CreationSize.x * CreationSize.x +
			 CreationSize.y * CreationSize.y +
			 CreationSize.z * CreationSize.z;
	// if DeadZone^2 more then CreationSize^2, disable DeadZone
	float DeadZone2 = DeadZone * DeadZone;
	if (tmpDist2 <= DeadZone2)
		DeadZone = 0.0f;

	sVECTOR3D CreationPos{vw_Randf0 * CreationSize.x,
			      vw_Randf0 * CreationSize.y,
			      vw_Randf0 * CreationSize.z};
	float ParticleDist2 = CreationPos.x * CreationPos.x +
			      CreationPos.y * CreationPos.y +
			      CreationPos.z * CreationPos.z;
	while ((ParticleDist2 > tmpDist2) || (ParticleDist2 < DeadZone2)) {
		if (ParticleDist2 > tmpDist2) {
			// decrease radius
			sVECTOR3D tmpPosDec = CreationPos;
			tmpPosDec.Normalize();
			tmpPosDec = tmpPosDec ^ (1 / 100.0f); // decrease distance on 1%
			CreationPos -= tmpPosDec;
		}
		if (ParticleDist2 < DeadZone2) {
			// increase radius
			sVECTOR3D tmpPosInc = CreationPos;
			tmpPosInc.Normalize();
			tmpPosInc = tmpPosInc ^ (1 / 100.0f); // increase distance on 1%
			CreationPos += tmpPosInc;

			vw_Clamp(CreationPos.x, -CreationSize.x, CreationSize.x);
			vw_Clamp(CreationPos.y, -CreationSize.y, CreationSize.y);
			vw_Clamp(CreationPos.z, -CreationSize.z, CreationSize.z);
		}
		ParticleDist2 = CreationPos.x * CreationPos.x +
				CreationPos.y * CreationPos.y +
				CreationPos.z * CreationPos.z;
	}
	NewParticle.Location = Location + CreationPos;
}

/*
 * Setup new particle direction.
 */
void cParticleSystem2D::SetupNewParticleDirection(cParticle2D &NewParticle)
{
	if ((Direction != sVECTOR3D{0.0f, 0.0f, 0.0f}) && (360.00f != Theta)) {
		if (Theta == 0.0f) {
			// emit in the same direction as particle system
			NewParticle.Velocity = Direction;
			return;
		}

		// emit with deviation
		float RandomYaw = vw_Randf0 * 3.14159f * 2.0f; // 2π = 360°
		float RandomPitch = vw_Randf0 * Theta * 3.14159f / 180.0f ; // convert Theta to radians (1° = π/180°)

		// y
		NewParticle.Velocity.y = Direction.y * cosf(RandomPitch);
		NewParticle.Velocity.x = Direction.y * sinf(RandomPitch) * cosf(RandomYaw);
		NewParticle.Velocity.z = Direction.y * sinf(RandomPitch) * sinf(RandomYaw);
		// x
		NewParticle.Velocity.y += Direction.x * sinf(RandomPitch) * cosf(RandomYaw);
		NewParticle.Velocity.x += Direction.x * cosf(RandomPitch);
		NewParticle.Velocity.z += Direction.x * sinf(RandomPitch) * sinf(RandomYaw);
		// z
		NewParticle.Velocity.y += Direction.z * sinf(RandomPitch) * sinf(RandomYaw);
		NewParticle.Velocity.x += Direction.z * sinf(RandomPitch) * cosf(RandomYaw);
		NewParticle.Velocity.z += Direction.z * cosf(RandomPitch);

		vw_Matrix33CalcPoint(NewParticle.Velocity, RotationMatrix);
		return;
	}

	// emit in random direction
	NewParticle.Velocity.y = vw_Randf0 * Theta / 360.0f;
	NewParticle.Velocity.x = vw_Randf0 * Theta / 360.0f;
	NewParticle.Velocity.z = vw_Randf0 * Theta / 360.0f;
	NewParticle.Velocity.Normalize();
}

/*
 * Add data to local draw buffer.
 */
static inline void AddToDrawBuffer(float CoordX, float CoordY,
				   const sCOLORVALUE2D &Color, float Alpha,
				   float TextureU, float TextureV)
{
	DrawBuffer[DrawBufferCurrentPosition++] = CoordX;
	DrawBuffer[DrawBufferCurrentPosition++] = CoordY;
	DrawBuffer[DrawBufferCurrentPosition++] = Color.r;
	DrawBuffer[DrawBufferCurrentPosition++] = Color.g;
	DrawBuffer[DrawBufferCurrentPosition++] = Color.b;
	DrawBuffer[DrawBufferCurrentPosition++] = Alpha;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureU;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureV;
}

/*
 * Draw all particles.
 */
void cParticleSystem2D::Draw()
{
	if (!Texture || ParticlesList.empty())
		return;

	float AW;
	float AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw = H * 1.0f;

	// RI_TRIANGLES * (RI_2f_XYZ + RI_2f_TEX + RI_4f_COLOR) * ParticlesList.size()
	unsigned int tmpDrawBufferSize = 6 * (2 + 2 + 4) * ParticlesList.size();
	if (tmpDrawBufferSize > DrawBufferSize) {
		DrawBufferSize = tmpDrawBufferSize;
		DrawBuffer.reset(new float[DrawBufferSize]);
	}
	DrawBufferCurrentPosition = 0;

	// prepare draw buffer
	for (auto &tmpParticle : ParticlesList) {
		sRECT DestRect(int(tmpParticle.Location.x - tmpParticle.Size / 2),
			       int(tmpParticle.Location.y - tmpParticle.Size / 2),
			       int(tmpParticle.Location.x + tmpParticle.Size / 2),
			       int(tmpParticle.Location.y + tmpParticle.Size / 2));

		float tmpPosY{0.0f};
		if (ASpresent)
			tmpPosY = (AH - DestRect.top - DestRect.top - (DestRect.bottom - DestRect.top));
		else
			tmpPosY = (AHw - DestRect.top - DestRect.top - (DestRect.bottom - DestRect.top));

		// first triangle
		AddToDrawBuffer(DestRect.left, DestRect.top + tmpPosY + (DestRect.bottom - DestRect.top),
				tmpParticle.Color, tmpParticle.Alpha, 0.0f, 1.0f);
		AddToDrawBuffer(DestRect.left, DestRect.top + tmpPosY,
				tmpParticle.Color, tmpParticle.Alpha, 0.0f, 0.0f);
		AddToDrawBuffer(DestRect.left + (DestRect.right - DestRect.left), DestRect.top + tmpPosY,
				tmpParticle.Color, tmpParticle.Alpha, 1.0f, 0.0f);

		// second triangle
		AddToDrawBuffer(DestRect.left + (DestRect.right - DestRect.left), DestRect.top + tmpPosY,
				tmpParticle.Color, tmpParticle.Alpha, 1.0f, 0.0f);
		AddToDrawBuffer(DestRect.left + (DestRect.right - DestRect.left),
				DestRect.top + tmpPosY + (DestRect.bottom - DestRect.top),
				tmpParticle.Color, tmpParticle.Alpha, 1.0f, 1.0f);
		AddToDrawBuffer(DestRect.left, DestRect.top + tmpPosY + (DestRect.bottom - DestRect.top),
				tmpParticle.Color, tmpParticle.Alpha, 0.0f, 1.0f);
	}

	// texture setup
	vw_SetTexture(0, Texture);
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

	// rendering
	vw_SendVertices(RI_TRIANGLES, 6 * ParticlesList.size(), RI_2f_XY | RI_1_TEX | RI_4f_COLOR,
			DrawBuffer.get(), 8 * sizeof(DrawBuffer.get()[0]));

	// reset rendering states
	vw_SetTextureBlend(false, 0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);
}

/*
 * Move center of particle system and all particles.
 */
void cParticleSystem2D::MoveSystem(const sVECTOR3D &NewLocation)
{
	for (auto &tmpParticle : ParticlesList) {
		// this increment should looks like: "+= NewLocation - Location"
		// but, in order to avoid temporary variable creation we do it in
		// two steps, see "struct sVECTOR3D" for more info
		tmpParticle.Location += NewLocation;
		tmpParticle.Location -= Location;
	}
	Location = NewLocation;
}

/*
 * Move center of particle system.
 */
void cParticleSystem2D::MoveSystemLocation(const sVECTOR3D &NewLocation)
{
	Location = NewLocation;
}

/*
 * Set rotation.
 */
void cParticleSystem2D::SetRotation(const sVECTOR3D &NewAngle)
{
	vw_Matrix33CreateRotate(RotationMatrix, Angle ^ -1);
	vw_Matrix33CreateRotate(RotationMatrix, NewAngle);
	Angle = NewAngle;
}

/*
 * Create new particle system 2D.
 */
cParticleSystem2D *vw_CreateParticleSystem2D()
{
	ParticleSystemsList.emplace_front();
	return &ParticleSystemsList.front();
}

/*
 * Release all particle systems 2D.
 */
void vw_ReleaseAllParticleSystems2D()
{
	ParticleSystemsList.clear();
}
