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

// TODO translate comments
// TODO (?) cParticleSystem2D should be managed globally, caller should receive ID (object ID),
//      instead of allocate new memory for object and care about it

#include "../math/math.h"
#include "../graphics/graphics.h"
#include "../texture/texture.h"
#include "particle_system2d.h"

namespace {

// Local draw buffer, that dynamically allocate memory at maximum
// required size only one time per game execution.
// Never use reset(), only clear() for this buffer.
std::vector<float> DrawBuffer{};

} // unnamed namespace


/*
 * Update particle.
 */
bool sParticle2D::Update(float TimeDelta, sVECTOR3D ParentLocation, bool Attractive, float MagnetFactor)
{
	// Если частица уже мертва, ее нужно отключить - передаем в систему эти данные
	if (Age + TimeDelta >= Lifetime) {
		Age = -1.0f;
		return false;
	}

	// увеличиваем возраст частицы
	Age += TimeDelta;

	// перемещаем частицу на нужное значение
	Location += Velocity ^ TimeDelta;

	if (NeedStop)
		Velocity -= Velocity ^ TimeDelta;

	// если есть притяжение системы, просчитываем воздействие
	if (Attractive) {
		sVECTOR3D AttractLocation = ParentLocation;
		// рассчитывае вектор взаимодействия между частицей и точкой притяжения
		sVECTOR3D AttractDir = AttractLocation - Location;

		// если нужно использовать притяжения, считаем перемещение
		if (NeedStop)
			MagnetFactor -= MagnetFactor * TimeDelta;

		AttractDir.Normalize();
		Velocity += AttractDir ^ (MagnetFactor * TimeDelta);

	}

	// просчитываем текущий цвет частицы
	Color.r += ColorDelta.r * TimeDelta;
	Color.g += ColorDelta.g * TimeDelta;
	Color.b += ColorDelta.b * TimeDelta;

	// текущий размер частицы
	Size += SizeDelta * TimeDelta;

	// если пришли сюда - значит все хорошо и частица работает
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
	// пока не создадим все необходимые частицы
	while (Quantity > 0) {
		// создаем новую частицу
		ParticlesList.emplace_back();
		// TODO emplace_back() return reference to the inserted element (since C++17)
		//      this line could be combined with previous line, we could use
		//      ParticlesList.back() directly, but "NewParticle" usage make code more clear
		sParticle2D &NewParticle = ParticlesList.back();

		// установка жизни новой частици и проверка, что не выходит из диапахона
		NewParticle.Age = 0.0f;
		NewParticle.Lifetime = Life + vw_Randf0 * LifeVar;
		if (NewParticle.Lifetime < 0.0f)
			NewParticle.Lifetime = 0.0f;

		// стартовый цвет
		NewParticle.Color.r = ColorStart.r + vw_Randf0 * ColorVar.r;
		NewParticle.Color.g = ColorStart.g + vw_Randf0 * ColorVar.g;
		NewParticle.Color.b = ColorStart.b + vw_Randf0 * ColorVar.b;

		// проверяем, чтобы не было переполнения цвета
		vw_Clamp(NewParticle.Color.r, 0.0f, 1.0f);
		vw_Clamp(NewParticle.Color.g, 0.0f, 1.0f);
		vw_Clamp(NewParticle.Color.b, 0.0f, 1.0f);

		// считаем delta относительно жизни частицы
		NewParticle.ColorDelta.r = (ColorEnd.r - NewParticle.Color.r) / NewParticle.Lifetime;
		NewParticle.ColorDelta.g = (ColorEnd.g - NewParticle.Color.g) / NewParticle.Lifetime;
		NewParticle.ColorDelta.b = (ColorEnd.b - NewParticle.Color.b) / NewParticle.Lifetime;

		// считаем значение альфы
		NewParticle.Alpha = AlphaStart + vw_Randf0 * AlphaVar;
		// убираем переполнение
		vw_Clamp(NewParticle.Alpha, 0.0f, 1.0f);

		switch (CreationType) {
		case eParticle2DCreationType::Point:
			NewParticle.Location = Location + sVECTOR3D(vw_Randf0 * CreationSize.x,
								    vw_Randf0 * CreationSize.y,
								    vw_Randf0 * CreationSize.z);
			break;

		case eParticle2DCreationType::Quad: {
			// в квадрате
			sVECTOR3D tmp;
			if (DeadZone != 0.0f) {
				float minDist = CreationSize.x * CreationSize.x +
						CreationSize.y * CreationSize.y +
						CreationSize.z * CreationSize.z;
				// если зона больше чем радиус излучения - выключаем ее
				if (minDist <= (DeadZone * DeadZone))
					DeadZone = 0.0f;
			}

			tmp.x = (1.0f - vw_Randf1 * 2) * CreationSize.x;
			tmp.y = (1.0f - vw_Randf1 * 2) * CreationSize.y;
			tmp.z = (1.0f - vw_Randf1 * 2) * CreationSize.z;
			while ((tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z) < (DeadZone * DeadZone)) {
				// ув. радиус
				sVECTOR3D tmp1 = tmp;
				tmp1.Normalize();
				tmp1 = tmp1 ^ (1 / 100.0f);
				tmp = tmp + tmp1;
			}

			NewParticle.Location = Location + tmp;
			}
			break;

		case eParticle2DCreationType::Circle: {
			// в окружности
			sVECTOR3D tmp;
			float minDist = CreationSize.x * CreationSize.x +
					CreationSize.y * CreationSize.y +
					CreationSize.z * CreationSize.z;
			// если зона больше чем радиус излучения - выключаем ее
			if (minDist <= (DeadZone * DeadZone))
				DeadZone = 0.0f;

			tmp.x = vw_Randf0 * CreationSize.x;
			tmp.y = vw_Randf0 * CreationSize.y;
			tmp.z = vw_Randf0 * CreationSize.z;
			float ParticleDist = tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z;
			while ((ParticleDist > minDist) || (ParticleDist < (DeadZone * DeadZone))) {
				if (ParticleDist > minDist) {
					// ум. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1 ^ (1 / 100.0f);
					tmp = tmp - tmp1;
				}
				if (ParticleDist < DeadZone * DeadZone) {
					// ув. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1 ^ (1 / 100.0f);
					tmp = tmp + tmp1;

					if (tmp.x > 0.0f) {
						if (tmp.x > CreationSize.x)
							tmp.x = CreationSize.x;
					} else {
						if (tmp.x < -CreationSize.x)
							tmp.x = -CreationSize.x;
					}

					if (tmp.y > 0.0f) {
						if (tmp.y > CreationSize.y)
							tmp.y = CreationSize.y;
					} else {
						if (tmp.y < -CreationSize.y)
							tmp.y = -CreationSize.y;
					}

					if (tmp.z > 0.0f) {
						if (tmp.z > CreationSize.z)
							tmp.z = CreationSize.z;
					} else {
						if (tmp.z < -CreationSize.z)
							tmp.z = -CreationSize.z;
					}
				}
				ParticleDist = tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z;
			}
			NewParticle.Location = Location + tmp;
			}
			break;
		}

		// считаем размер частицы
		NewParticle.Size = SizeStart + vw_Randf0 * SizeVar;
		if (NewParticle.Size < 0.0f)
			NewParticle.Size = 0.0f;
		NewParticle.SizeDelta = (SizeEnd - NewParticle.Size) / NewParticle.Lifetime;

		// calculate new particle direction
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
 * Setup new particle direction.
 */
void cParticleSystem2D::SetupNewParticleDirection(sParticle2D &NewParticle)
{
	if ((Direction != sVECTOR3D{0.0f, 0.0f, 0.0f}) && (360.00f != Theta)) {
		if (Theta == 0.0f) {
			// emit in the same direction as particle system
			NewParticle.Velocity = Direction;
			return;
		}

		// emit in the same direction as particle system with deviation
		float RandomYaw = vw_Randf0 * 3.14159f * 2.0f;
		float RandomPitch = vw_Randf0 * Theta * 3.14159f / 180.0f ;

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
				   float R, float G, float B, float A,
				   float TextureU, float TextureV)
{
	DrawBuffer.push_back(CoordX);
	DrawBuffer.push_back(CoordY);
	DrawBuffer.push_back(R);
	DrawBuffer.push_back(G);
	DrawBuffer.push_back(B);
	DrawBuffer.push_back(A);
	DrawBuffer.push_back(TextureU);
	DrawBuffer.push_back(TextureV);
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
				tmpParticle.Color.r, tmpParticle.Color.g, tmpParticle.Color.b, tmpParticle.Alpha,
				0.0f, 1.0f);
		AddToDrawBuffer(DestRect.left, DestRect.top + tmpPosY,
				tmpParticle.Color.r, tmpParticle.Color.g, tmpParticle.Color.b, tmpParticle.Alpha,
				0.0f, 0.0f);
		AddToDrawBuffer(DestRect.left + (DestRect.right - DestRect.left), DestRect.top + tmpPosY,
				tmpParticle.Color.r, tmpParticle.Color.g, tmpParticle.Color.b, tmpParticle.Alpha,
				1.0f, 0.0f);

		// second triangle
		AddToDrawBuffer(DestRect.left + (DestRect.right - DestRect.left), DestRect.top + tmpPosY,
				tmpParticle.Color.r, tmpParticle.Color.g, tmpParticle.Color.b, tmpParticle.Alpha,
				1.0f, 0.0f);
		AddToDrawBuffer(DestRect.left + (DestRect.right - DestRect.left),
				DestRect.top + tmpPosY + (DestRect.bottom - DestRect.top),
				tmpParticle.Color.r, tmpParticle.Color.g, tmpParticle.Color.b, tmpParticle.Alpha,
				1.0f, 1.0f);
		AddToDrawBuffer(DestRect.left, DestRect.top + tmpPosY + (DestRect.bottom - DestRect.top),
				tmpParticle.Color.r, tmpParticle.Color.g, tmpParticle.Color.b, tmpParticle.Alpha,
				0.0f, 1.0f);
	}

	// texture setup
	vw_SetTexture(0, Texture);
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

	// rendering
	vw_SendVertices(RI_TRIANGLES, 6 * ParticlesList.size(), RI_2f_XY | RI_1_TEX | RI_4f_COLOR,
			DrawBuffer.data(), 8 * sizeof(DrawBuffer.data()[0]));
	DrawBuffer.clear();

	// reset rendering states
	vw_SetTextureBlend(false, 0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);
}

/*
 * Move center of particle system and all particles.
 */
void cParticleSystem2D::MoveSystem(sVECTOR3D NewLocation)
{
	sVECTOR3D PrevLocation = Location;
	Location = NewLocation;

	for (auto &tmpParticle : ParticlesList) {
		tmpParticle.Location += NewLocation - PrevLocation;
	}
}

/*
 * Move center of particle system.
 */
void cParticleSystem2D::MoveSystemLocation(sVECTOR3D NewLocation)
{
	Location = NewLocation;
}

/*
 * Set rotation.
 */
void cParticleSystem2D::SetRotation(sVECTOR3D NewAngle)
{
	vw_Matrix33CreateRotate(RotationMatrix, Angle ^ -1);
	vw_Matrix33CreateRotate(RotationMatrix, NewAngle);
	Angle = NewAngle;
}
