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

// TODO since we use RI_TRIANGLES, use 4 vertices + index buffer for vw_SendVertices()
//      instead of 6 vertices, so, we send 4 vertices and index buffer for 6 elements,
//      something like {1, 2, 3, 3, 4, 1}
//                               ^  ^  ^ second triangle indexes
//                      ^  ^  ^ first triangle indexes

// TODO (?) move to std::forward_list for particle systems management

// TODO revise work with GLSL

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
std::unique_ptr<float []> DrawBuffer{};
unsigned int DrawBufferCurrentPosition{0};
unsigned int DrawBufferSize{0};

cParticleSystem *StartParticleSystem{nullptr};
cParticleSystem *EndParticleSystem{nullptr};

sGLSL *ParticleSystemGLSL{nullptr};
int ParticleSystemUniformLocations[10];

bool ParticleSystemUseGLSL{false};
float ParticleSystemQuality{1.0f};

} // unnamed namespace


//-----------------------------------------------------------------------------
//	обновление информации частицы
//-----------------------------------------------------------------------------
bool cParticle::Update(float TimeDelta, const sVECTOR3D &ParentLocation,
		       bool Magnet, float MagnetFactor)
{
	// Если частица уже мертва, ее нужно отключить - передаем в систему эти данные
	if (Age + TimeDelta >= Lifetime) {
		Age = -1.0f;
		return false;
	} else {
		// увеличиваем возраст частицы
		Age += TimeDelta;

		// перемещаем частицу на нужное значение
		Location += Velocity ^ TimeDelta;

		if (NeedStop)
			Velocity -= Velocity ^ TimeDelta;

		// если есть притяжение системы, просчитываем воздействие
		if (Magnet) {
			// рассчитывае вектор взаимодействия между частицей и точкой притяжения
			sVECTOR3D MagnetDir = ParentLocation;
			MagnetDir -= Location;

			// если нужно использовать притяжения, считаем перемещение
			if (NeedStop)
				MagnetFactor -= MagnetFactor * TimeDelta;

			MagnetDir.Normalize();
			Velocity += MagnetDir ^ (MagnetFactor * TimeDelta);
		}

		// просчитываем текущий цвет частицы
		Color.r += ColorDelta.r * TimeDelta;
		Color.g += ColorDelta.g * TimeDelta;
		Color.b += ColorDelta.b * TimeDelta;

		// просчитываем текущую прозрачность
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

			vw_Clamp( Alpha, 0.0f, 1.0f );
		}

		// текущий размер частицы
		Size += SizeDelta * TimeDelta;
	}

	// если пришли сюда - значит все хорошо и частица работает
	return true;
}

cParticleSystem::cParticleSystem()
{
	// настройка массива
	vw_AttachParticleSystem(this);
}

cParticleSystem::~cParticleSystem()
{
	if (Light)
		vw_ReleaseLight(Light);
	vw_DetachParticleSystem(this);
}

//-----------------------------------------------------------------------------
// обновление системы
//-----------------------------------------------------------------------------
bool cParticleSystem::Update(float Time)
{
	// первый раз... просто берем время
	if (TimeLastUpdate < 0.0f) {
		TimeLastUpdate = Time;
		return true;
	}

	// Time - это абсолютное время, вычисляем дельту
	float TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали
	if (TimeDelta == 0.0f)
		return true;

	TimeLastUpdate = Time;

	// для всех частиц вызываем их собственный апдейт
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

	// подсчитываем, как много частиц нам нужно создать из ParticlesPerSec
	float ParticlesNeeded = (ParticlesPerSec/ParticleSystemQuality) * TimeDelta + EmissionResidue;

	// переводим в целочисленные значения
	ParticlesCreated = (int)ParticlesNeeded;

	if (!IsSuppressed) {
		// запоминаем разницу между тем сколько нужно и сколько создадим
		EmissionResidue = ParticlesNeeded - ParticlesCreated;
	} else {
		// ничего создавать не нужно
		EmissionResidue = ParticlesNeeded;
		ParticlesCreated = 0;
	}

	// если нужно что-то создавать, создаем
	if (ParticlesCreated > 0) {
		// если пытаемся создать за один раз больше чем можем в секунду
		// убираем этот "глюк", видно компьютер тормозит
		if (ParticlesCreated > ParticlesPerSec) ParticlesCreated = ParticlesPerSec;
		// пока не создадим все необходимые частицы
		while (ParticlesCreated > 0) {
			// create new particle
			ParticlesList.emplace_front();
			// NOTE emplace_front() return reference to the inserted element (since C++17)
			//      this line could be combined with previous line, we could use
			//      ParticlesList.front() directly, but "NewParticle" usage make code more clear
			cParticle &NewParticle = ParticlesList.front();
			ParticlesCountInList++;

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
			// считаем дельту альфы
			NewParticle.AlphaDelta = (AlphaEnd - NewParticle.Alpha) / NewParticle.Lifetime;
			// передаем тип изменения альфы
			NewParticle.AlphaShowHide = AlphaShowHide;
			// если нужно маргнуть, не обращаем внимания на Alpha вообще
			if (NewParticle.AlphaShowHide) {
				NewParticle.AlphaDelta = (2.0f-AlphaEnd * 2.0f) / NewParticle.Lifetime;
				NewParticle.Alpha = AlphaEnd;
			}

			// выпускаем частицу возле места нахождения системы
			if (CreationType == 0) // точка
				NewParticle.Location = Location + sVECTOR3D(vw_Randf0 * CreationSize.x,
									    vw_Randf0 * CreationSize.y,
									    vw_Randf0 * CreationSize.z);
			if (CreationType == 1) {
				// в кубе
				if (DeadZone != 0.0f) {
					float minDist = CreationSize.x * CreationSize.x +
							CreationSize.y * CreationSize.y +
							CreationSize.z * CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone)
						DeadZone = 0.0f;
				}

				sVECTOR3D tmp;
				tmp.x = vw_Randf0 * CreationSize.x;
				tmp.y = vw_Randf0 * CreationSize.y;
				tmp.z = vw_Randf0 * CreationSize.z;
				while (tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z < DeadZone * DeadZone) {
					// ув. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1 ^ (1 / 100.0f);
					tmp = tmp + tmp1;
				}

				vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
				NewParticle.Location = Location + tmp;
			}
			// тип 11 только !!! для лазеров-мазеров
			if (CreationType == 11) {
				// в кубе
				if (DeadZone != 0.0f) {
					float minDist = CreationSize.x * CreationSize.x +
							CreationSize.y * CreationSize.y +
							CreationSize.z * CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone * DeadZone)
						DeadZone = 0.0f;
				}

				sVECTOR3D tmp;
				tmp.x = (0.5f - vw_Randf1) * CreationSize.x;
				tmp.y = (0.5f - vw_Randf1) * CreationSize.y;
				tmp.z = (0.5f - vw_Randf1) * CreationSize.z;
				sVECTOR3D CreationSizeText = CreationSize ^ 0.5f;
				while (tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z < DeadZone * DeadZone) {
					// ув. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1 ^ (1 / 100.0f);
					tmp = tmp + tmp1;
				}

				if (tmp.x > 0.0f) {
					if (tmp.x > CreationSizeText.x)
						tmp.x = CreationSizeText.x;
				} else {
					if (tmp.x < -CreationSizeText.x)
						tmp.x = -CreationSizeText.x;
				}

				if (tmp.y > 0.0f) {
					if (tmp.y > CreationSizeText.y)
						tmp.y = CreationSizeText.y;
				} else {
					if (tmp.y < -CreationSizeText.y)
						tmp.y = -CreationSizeText.y;
				}
				// делаем ручное регулирование
				if (tmp.z > 0.0f) {
					if (tmp.z > CreationSizeText.z)
						tmp.z = CreationSizeText.z;
				} else {
					if (tmp.z < -CreationSizeText.z)
						tmp.z = -CreationSizeText.z;
				}

				vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
				NewParticle.Location = Location + tmp;
			}
			if (CreationType == 2) {
				// в сфере
				float minDist = CreationSize.x * CreationSize.x +
						CreationSize.y * CreationSize.y +
						CreationSize.z * CreationSize.z;
				// если зона больше чем радиус излучения - выключаем ее
				if (minDist <= DeadZone * DeadZone)
					DeadZone = 0.0f;

				sVECTOR3D tmp;
				tmp.x = vw_Randf0 * CreationSize.x;
				tmp.y = vw_Randf0 * CreationSize.y;
				tmp.z = vw_Randf0 * CreationSize.z;
				float ParticleDist = tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z;
				while ((ParticleDist > minDist) || (ParticleDist < DeadZone * DeadZone)) {
					if (ParticleDist > minDist) {
						// ум. радиус
						sVECTOR3D tmp1 = tmp;
						tmp1.Normalize();
						tmp1 = tmp1 ^ (1 / 100.0f);
						tmp = tmp - tmp1;
					}
					if ( ParticleDist < DeadZone * DeadZone) {
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

				vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
				NewParticle.Location = Location + tmp;
			}

			// учитываем перемещение системы, генерируем частицы равномерно
			sVECTOR3D L = (Location - PrevLocation) ^ ((ParticlesNeeded - ParticlesCreated) / ParticlesNeeded);
			NewParticle.Location = NewParticle.Location - L;

			// считаем размер частицы
			NewParticle.Size = SizeStart + vw_Randf0 * SizeVar;
			if (NewParticle.Size < 0.0f)
				NewParticle.Size = SizeStart;
			NewParticle.SizeDelta = (SizeEnd - NewParticle.Size) / NewParticle.Lifetime;
			// если есть учет расстояния, работаем с ним
			if (Resize < 1.0f) {
				// получаем текущее положение камеры
				sVECTOR3D CurrentCameraLocation;
				vw_GetCameraLocation(&CurrentCameraLocation);
				// находим расстояние от центра системы до камеры
				float SystDist = (CurrentCameraLocation.x - Location.x - CreationSize.x) *
						 (CurrentCameraLocation.x - Location.x - CreationSize.x) +
						 (CurrentCameraLocation.y - Location.y - CreationSize.y) *
						 (CurrentCameraLocation.y - Location.y - CreationSize.y) +
						 (CurrentCameraLocation.z - Location.z - CreationSize.z) *
						 (CurrentCameraLocation.z - Location.z - CreationSize.z);

				float ParticleDist = (CurrentCameraLocation.x - NewParticle.Location.x) *
						     (CurrentCameraLocation.x - NewParticle.Location.x) +
						     (CurrentCameraLocation.y - NewParticle.Location.y) *
						     (CurrentCameraLocation.y - NewParticle.Location.y) +
						     (CurrentCameraLocation.z - NewParticle.Location.z) *
						     (CurrentCameraLocation.z - NewParticle.Location.z);

				// если частица ближе центра к камере - нужна корректировка
				if (ParticleDist < SystDist) {
					float tmpStart = SizeStart - SizeStart * (1.0f - Resize) * (SystDist-ParticleDist) / SystDist;
					float tmpEnd = SizeEnd - SizeEnd*(1.0f - Resize) * (SystDist-ParticleDist) / SystDist;
					float tmpVar = SizeVar - SizeVar*(1.0f - Resize) * (SystDist-ParticleDist) / SystDist;

					NewParticle.Size = tmpStart + vw_Randf0 * tmpVar;
					if (NewParticle.Size < 0.0f)
						NewParticle.Size = 0.0f;
					NewParticle.SizeDelta = (tmpEnd - NewParticle.Size) / NewParticle.Lifetime;
				}
			}

			// испускатель имеет направление. этот код немного добавляет случайности
			if (Theta == 0.0f)
				NewParticle.Velocity = Direction;
			else {

				NewParticle.Velocity = Direction;
				vw_RotatePoint(NewParticle.Velocity,
					       sVECTOR3D(Theta * vw_Randf0 / 2.0f, Theta * vw_Randf0/2.0f, 0.0f));
			}

			NewParticle.NeedStop = NeedStop;

			// находим перемещение
			float NewSpeed = Speed + vw_Randf0 * SpeedVar;
			if (NewSpeed < 0.0f)
				NewSpeed = 0.0f;
			NewParticle.Velocity *= NewSpeed;

			// уменьшаем необходимое количество частиц
			ParticlesCreated--;
		}
	}

	// если уже ничего нет и нужно выйти - выходим
	if (DestroyIfNoParticles &&
	    ParticlesList.empty()) {
		NeedDestroy = true;
		return false;
	}

	// работа с источником света (если он есть)
	if (Light) {
		// просто работаем
		if (!IsSuppressed) {
			if (LightNeedDeviation) {
				// было выключено, сейчас только включили опять
				if (LightDeviation > 1.0f)
					LightDeviation = 0.7f;

				if (NextLightDeviation < LightDeviation) {
					LightDeviation -= LightDeviationSpeed * TimeDelta;
					if (NextLightDeviation >= LightDeviation) {
						LightDeviation = NextLightDeviation;
						LightDeviationSpeed = 3.5f + 3.5f * vw_Randf1;
						NextLightDeviation = 0.7 + 0.3f * vw_Randf1;
					}
				} else {
					LightDeviation += LightDeviationSpeed * TimeDelta;
					if (NextLightDeviation <= LightDeviation) {
						LightDeviation = NextLightDeviation;
						LightDeviationSpeed = 3.5f + 3.5f * vw_Randf1;
						NextLightDeviation = 0.7 - 0.3f * vw_Randf1;
					}
				}
			} else
				// было выключено, сейчас только включили опять
				if (LightDeviation > 1.0f)
					LightDeviation = 1.0f;
		} else {
			// было включено, сейчас выключили, нужно сделать исчезание
			if (IsSuppressed && (Life != 0.0f))
				LightDeviation += (25.0f / Life) * TimeDelta;
		}

		if (Light->LinearAttenuationBase != 0.0f) {
			Light->LinearAttenuation = Light->LinearAttenuationBase +
						   2.0f * (LightDeviation - 0.5f) * Light->LinearAttenuationBase;
			if (Light->LinearAttenuation < Light->LinearAttenuationBase)
				Light->LinearAttenuation = Light->LinearAttenuationBase;
		}
		if (Light->QuadraticAttenuationBase != 0.0f) {
			Light->QuadraticAttenuation = Light->QuadraticAttenuationBase +
						      2.0f * (LightDeviation - 0.5f) * Light->QuadraticAttenuationBase;
			if (Light->QuadraticAttenuation < Light->QuadraticAttenuationBase)
				Light->QuadraticAttenuation = Light->QuadraticAttenuationBase;
		}

		// если уже ничего нет, надо выключить свет, если что-то есть то включить
		Light->On = !ParticlesList.empty();
	}

	// находим новые данные AABB

	// предварительная инициализация
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

	for (auto &tmpParticle : ParticlesList) {
		// строим AABB
		if (tmpParticle.Alpha > 0.0f && tmpParticle.Size > 0.0f) {
			sVECTOR3D v;
			v.x = tmpParticle.Location.x + tmpParticle.Size;
			if (MaxX < v.x) MaxX = v.x;

			v.y = tmpParticle.Location.y + tmpParticle.Size;
			if (MaxY < v.y) MaxY = v.y;

			v.z = tmpParticle.Location.z + tmpParticle.Size;
			if (MaxZ < v.z) MaxZ = v.z;

			v.x = tmpParticle.Location.x - tmpParticle.Size;
			if (MinX > v.x) MinX = v.x;

			v.y = tmpParticle.Location.y - tmpParticle.Size;
			if (MinY > v.y) MinY = v.y;

			v.z = tmpParticle.Location.z - tmpParticle.Size;
			if (MinZ > v.z) MinZ = v.z;
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

	return true;
}

/*
 * Add data to local draw buffer.
 * Note, in case of GLSL, we use TextureU_or_GLSL and TextureV_or_GLSL
 * not for texture coordinates, but for GLSL program parameters.
 */
static inline void AddToDrawBuffer(float CoordX, float CoordY, float CoordZ,
				   const sCOLORVALUE3D &Color, float Alpha,
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

//-----------------------------------------------------------------------------
// прорисовка системы
//-----------------------------------------------------------------------------
void cParticleSystem::Draw(sTexture **CurrentTexture)
{
	if (!vw_BoxInFrustum(AABB[6], AABB[0]) ||
	    ParticlesList.empty())
		return;

	// RI_TRIANGLES * (RI_3f_XYZ + RI_2f_TEX + RI_4f_COLOR) * ParticlesCount
	unsigned int tmpDrawBufferSize = 6 * (3 + 2 + 4) * ParticlesCountInList;
	if (tmpDrawBufferSize > DrawBufferSize) {
		DrawBufferSize = tmpDrawBufferSize;
		DrawBuffer.reset(new float[DrawBufferSize]);
	}
	DrawBufferCurrentPosition = 0;

	// шейдеры не поддерживаются - рисуем по старинке
	if (!ParticleSystemUseGLSL) {
		// получаем текущее положение камеры
		sVECTOR3D CurrentCameraLocation{vw_GetCameraLocation(nullptr)};

		for (auto &tmpParticle : ParticlesList) {
			// находим вектор камера-точка
			sVECTOR3D nnTmp{CurrentCameraLocation - tmpParticle.Location};
			//nnTmp.Normalize();// - это тут не нужно, нам нужны только пропорции

			// находим перпендикуляр к вектору nnTmp
			sVECTOR3D nnTmp2{1.0f, 1.0f, -(nnTmp.x + nnTmp.y) / nnTmp.z};
			nnTmp2.Normalize();

			// находим перпендикуляр к векторам nnTmp и nnTmp2
			// файтически - a x b = ( aybz - byaz , azbx - bzax , axby - bxay );
			sVECTOR3D nnTmp3{nnTmp.y * nnTmp2.z - nnTmp2.y * nnTmp.z,
					 nnTmp.z * nnTmp2.x - nnTmp2.z * nnTmp.x,
					 nnTmp.x * nnTmp2.y - nnTmp2.x * nnTmp.y};
			nnTmp3.Normalize();

			// находим
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
	} else { // иначе работаем с шейдерами, в них правильно развернем билборд
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
	if (*CurrentTexture != Texture) {
		vw_SetTexture(0, Texture);
		*CurrentTexture = Texture;
	}

	if (BlendType == 1)
		vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

	vw_SendVertices(RI_TRIANGLES, 6 * ParticlesCountInList, RI_3f_XYZ | RI_4f_COLOR | RI_1_TEX,
			DrawBuffer.get(), 9 * sizeof(DrawBuffer.get()[0]));

	if (BlendType != 0)
		vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);
}

//-----------------------------------------------------------------------------
// поставить правильный полет частиц, т.е. учет внешней скорости
//-----------------------------------------------------------------------------
void cParticleSystem::SetStartLocation(const sVECTOR3D &NewLocation)
{
	PrevLocation = NewLocation;
	Location = NewLocation;

	if (Light)
		Light->SetLocation(Location);
}

//-----------------------------------------------------------------------------
// перемещение всех частиц и центра
//-----------------------------------------------------------------------------
void cParticleSystem::MoveSystem(const sVECTOR3D &NewLocation)
{
	// чтобы не сбивать PrevLocation
	if (Location == NewLocation)
		return;
	PrevLocation = Location;
	Location = NewLocation;

	for (auto &tmpParticle : ParticlesList) {
		// меняем каждой частице
		tmpParticle.Location += NewLocation;
		tmpParticle.Location -= PrevLocation;
	}

	if (Light)
		Light->SetLocation(Location);
}

//-----------------------------------------------------------------------------
// перемещение центра
//-----------------------------------------------------------------------------
void cParticleSystem::MoveSystemLocation(const sVECTOR3D &NewLocation)
{
	// чтобы не сбивать PrevLocation
	if (Location == NewLocation)
		return;
	PrevLocation = Location;
	Location = NewLocation;

	if (Light)
		Light->SetLocation(Location);
}

//-----------------------------------------------------------------------------
// установка направления "движения" системы
//-----------------------------------------------------------------------------
void cParticleSystem::RotateSystemByAngle(const sVECTOR3D &NewAngle)
{
	Angle = NewAngle;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	// делаем инверсную старую матрицу
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Angle);

	vw_Matrix33CalcPoint(Direction, OldInvRotationMat);
	vw_Matrix33CalcPoint(Direction, CurrentRotationMat);
}

//-----------------------------------------------------------------------------
// установка направления "движения" системы + перенос всех частиц
//-----------------------------------------------------------------------------
void cParticleSystem::RotateSystemAndParticlesByAngle(const sVECTOR3D &NewAngle)
{
	Angle = NewAngle;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	// делаем инверсную старую матрицу
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Angle);

	vw_Matrix33CalcPoint(Direction, OldInvRotationMat);
	vw_Matrix33CalcPoint(Direction, CurrentRotationMat);

	for (auto &tmpParticle : ParticlesList) {
		// меняем каждой частице
		sVECTOR3D TMP = tmpParticle.Location - Location;
		vw_Matrix33CalcPoint(TMP, OldInvRotationMat);
		vw_Matrix33CalcPoint(TMP, CurrentRotationMat);
		tmpParticle.Location = TMP + Location;
	}
}

//-----------------------------------------------------------------------------
// поворот частиц на угол без поворота системы
//-----------------------------------------------------------------------------
void cParticleSystem::RotateParticlesByAngle(const sVECTOR3D &NewAngle)
{
	// делаем обратку для отматывания назад
	float TmpOldInvRotationMat[9];
	memcpy(TmpOldInvRotationMat, CurrentRotationMat, 9 * sizeof(CurrentRotationMat[0]));
	vw_Matrix33InverseRotate(TmpOldInvRotationMat);

	// матрица поворота частиц
	float TmpRotationMat[9];
	vw_Matrix33CreateRotate(TmpRotationMat, NewAngle);

	for (auto &tmpParticle : ParticlesList) {
		// меняем каждой частице
		sVECTOR3D TMP = tmpParticle.Location - Location;
		vw_Matrix33CalcPoint(TMP, TmpOldInvRotationMat);
		// поворачиваем каждую точку
		vw_Matrix33CalcPoint(TMP, TmpRotationMat);
		vw_Matrix33CalcPoint(TMP, CurrentRotationMat);
		tmpParticle.Location = TMP + Location;
	}
}

//-----------------------------------------------------------------------------
// остановить все частицы в системе
//-----------------------------------------------------------------------------
void cParticleSystem::StopAllParticles()
{
	Speed = 0.0f;
	IsMagnet = false;

	for (auto &tmpParticle : ParticlesList) {
		// меняем каждой частице
		tmpParticle.Velocity = sVECTOR3D(0.0f,0.0f,0.0f);
	}
}

//-----------------------------------------------------------------------------
// поставить правильный полет частиц, т.е. учет внешней скорости
//-----------------------------------------------------------------------------
void cParticleSystem::ChangeSpeed(const sVECTOR3D &Vel)
{
	for (auto &tmpParticle : ParticlesList) {
		// меняем каждой частице
		tmpParticle.Velocity += Vel;
	}
}

//-----------------------------------------------------------------------------
//	Установка общих состояний
//-----------------------------------------------------------------------------
void vw_InitParticleSystems(bool UseGLSL, float Quality)
{
	ParticleSystemUseGLSL = UseGLSL;
	ParticleSystemQuality = Quality;

	// при первом присоединении устанавливаем шейдер
	if (ParticleSystemUseGLSL) {
		ParticleSystemGLSL = vw_FindShaderByName("ParticleSystem");
		ParticleSystemUniformLocations[0] = vw_GetUniformLocation(ParticleSystemGLSL, "ParticleTexture");
		ParticleSystemUniformLocations[1] = vw_GetUniformLocation(ParticleSystemGLSL, "CameraPoint");
	}
}

//-----------------------------------------------------------------------------
//	Присоеденяем ParticleSystem к списку
//-----------------------------------------------------------------------------
void vw_AttachParticleSystem(cParticleSystem *NewParticleSystem)
{
	if (NewParticleSystem == nullptr)
		return;

	// первый в списке...
	if (EndParticleSystem == nullptr) {
		NewParticleSystem->Prev = nullptr;
		NewParticleSystem->Next = nullptr;
		StartParticleSystem = NewParticleSystem;
		EndParticleSystem = NewParticleSystem;
	} else { // продолжаем заполнение...
		NewParticleSystem->Prev = EndParticleSystem;
		NewParticleSystem->Next = nullptr;
		EndParticleSystem->Next = NewParticleSystem;
		EndParticleSystem = NewParticleSystem;
	}
}

//-----------------------------------------------------------------------------
//	Удаляем ParticleSystem из списка
//-----------------------------------------------------------------------------
void vw_DetachParticleSystem(cParticleSystem *OldParticleSystem)
{
	if (OldParticleSystem == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartParticleSystem == OldParticleSystem)
		StartParticleSystem = OldParticleSystem->Next;
	if (EndParticleSystem == OldParticleSystem)
		EndParticleSystem = OldParticleSystem->Prev;

	if (OldParticleSystem->Next != nullptr)
		OldParticleSystem->Next->Prev = OldParticleSystem->Prev;
	else if (OldParticleSystem->Prev != nullptr)
		OldParticleSystem->Prev->Next = nullptr;

	if (OldParticleSystem->Prev != nullptr)
		OldParticleSystem->Prev->Next = OldParticleSystem->Next;
	else if (OldParticleSystem->Next != nullptr)
		OldParticleSystem->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
//	Удаляем все ParticleSystem в списке
//-----------------------------------------------------------------------------
void vw_ReleaseAllParticleSystems()
{
	// для всех ParticleSystem
	cParticleSystem *tmp = StartParticleSystem;
	while (tmp) {
		cParticleSystem *tmp2 = tmp->Next;
		// удаляем и очищаем всю память, в деструкторе стоит DetachShip
		delete tmp;
		tmp = tmp2;
	}

	StartParticleSystem = nullptr;
	EndParticleSystem = nullptr;

	ParticleSystemUseGLSL = false;
	ParticleSystemQuality = 1.0f;
	ParticleSystemGLSL = nullptr;
}

//-----------------------------------------------------------------------------
//	Прорисовываем все ParticleSystems
//-----------------------------------------------------------------------------
void vw_DrawAllParticleSystems()
{
	// текущая текстура
	sTexture *CurrentTexture{nullptr};

	// делаем все предустановки для прорисовки частиц, чтобы не менять каждый раз
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	// включаем шейдер, если есть возможность
	if (ParticleSystemUseGLSL && ParticleSystemGLSL) {
		// получаем текущее положение камеры
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		vw_UseShaderProgram(ParticleSystemGLSL);
		vw_Uniform1i(ParticleSystemGLSL, ParticleSystemUniformLocations[0], 0);
		vw_Uniform3f(ParticleSystemGLSL, ParticleSystemUniformLocations[1],
			     CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
	}
	// выключаем изменение буфера глубины
	glDepthMask(GL_FALSE);

	// для всех
	cParticleSystem *tmp = StartParticleSystem;
	while (tmp) {
		cParticleSystem *tmp2 = tmp->Next;
		tmp->Draw(&CurrentTexture);
		tmp = tmp2;
	}

	// включаем запись в буфер глубины
	glDepthMask(GL_TRUE);
	// выключаем шейдер
	if (ParticleSystemUseGLSL)
		vw_StopShaderProgram();

	vw_SetTextureBlend(false, 0, 0);
	// анбиндим текстуру
	vw_BindTexture(0, 0);
}

//-----------------------------------------------------------------------------
//	Прорисовываем блок ParticleSystems
//-----------------------------------------------------------------------------
void vw_DrawParticleSystems(cParticleSystem **DrawParticleSystem, int Quantity)
{
	if (!DrawParticleSystem)
		return;

	// текущая текстура
	sTexture *CurrentTexture{nullptr};

	// делаем все предустановки для прорисовки частиц, чтобы не менять каждый раз
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);
	// включаем шейдер, если есть возможность
	if (ParticleSystemUseGLSL && ParticleSystemGLSL) {
		// получаем текущее положение камеры
		sVECTOR3D CurrentCameraLocation;
		vw_GetCameraLocation(&CurrentCameraLocation);

		vw_UseShaderProgram(ParticleSystemGLSL);
		vw_Uniform1i(ParticleSystemGLSL, ParticleSystemUniformLocations[0], 0);
		vw_Uniform3f(ParticleSystemGLSL, ParticleSystemUniformLocations[1],
			     CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z);
	}
	// выключаем изменение буфера глубины
	glDepthMask(GL_FALSE);

	for (int i = 0; i < Quantity; i++) {
		if (DrawParticleSystem[i])
			DrawParticleSystem[i]->Draw(&CurrentTexture);
	}

	// включаем запись в буфер глубины
	glDepthMask(GL_TRUE);
	// выключаем шейдер
	if (ParticleSystemUseGLSL)
		vw_StopShaderProgram();

	vw_SetTextureBlend(false, 0, 0);
	// анбиндим текстуру
	vw_BindTexture(0, 0);
}

//-----------------------------------------------------------------------------
//	Апдейтим все ParticleSystems
//-----------------------------------------------------------------------------
void vw_UpdateAllParticleSystems(float Time)
{
	// для всех
	cParticleSystem *tmp = StartParticleSystem;
	while (tmp) {
		cParticleSystem *tmp2 = tmp->Next;
		if (!tmp->Update(Time))
			delete tmp;
		tmp = tmp2;
	}
}
