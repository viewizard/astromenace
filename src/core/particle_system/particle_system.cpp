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


#include "particle_system.h"
#include "../camera/camera.h"
#include "../graphics/graphics.h"



extern bool ParticleSystemUseGLSL;
extern float ParticleSystemQuality;





cParticleSystem::cParticleSystem()
{
	// начальные установки для мартиц поворотов
	vw_Matrix33Identity(CurrentRotationMat);
	vw_Matrix33Identity(OldInvRotationMat);
	// настройка массива
	vw_AttachParticleSystem(this);
}
cParticleSystem::~cParticleSystem()
{
	// полностью освобождаем память от всех частиц в системе
	sParticle *tmp = Start;
	while (tmp != nullptr) {
		sParticle *tmp2 = tmp->Next;
		Detach(tmp);
		delete tmp;
		tmp = tmp2;
	}
	if (Light != nullptr)
		vw_ReleaseLight(Light);
	if (tmpDATA != nullptr)
		delete [] tmpDATA;
	vw_DetachParticleSystem(this);
}

//-----------------------------------------------------------------------------
//	подключить частицу к системе
//-----------------------------------------------------------------------------
void cParticleSystem::Attach(sParticle * NewParticle)
{
	if (NewParticle == nullptr)
		return;

	if (End == nullptr) {
		NewParticle->Prev = nullptr;
		NewParticle->Next = nullptr;
		Start = NewParticle;
		End = NewParticle;
	} else {
		NewParticle->Prev = End;
		NewParticle->Next = nullptr;
		End->Next = NewParticle;
		End = NewParticle;
	}

	ParticlesCount++;
}


//-----------------------------------------------------------------------------
//	отключить ее от системы
//-----------------------------------------------------------------------------
void cParticleSystem::Detach(sParticle * OldParticle)
{
	if (OldParticle == nullptr)
		return;

	if (Start == OldParticle)
		Start = OldParticle->Next;
	if (End == OldParticle)
		End = OldParticle->Prev;

	if (OldParticle->Next != nullptr)
		OldParticle->Next->Prev = OldParticle->Prev;
	else if (OldParticle->Prev != nullptr)
		OldParticle->Prev->Next = nullptr;

	if (OldParticle->Prev != nullptr)
		OldParticle->Prev->Next = OldParticle->Next;
	else if (OldParticle->Next != nullptr)
		OldParticle->Next->Prev = nullptr;

	ParticlesCount--;
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
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;



	// для всех частиц вызываем их собственный апдейт
	sParticle *ParticleTmp = Start;
	while (ParticleTmp != nullptr) {
		sParticle *ParticleTmp2 = ParticleTmp->Next;
		// функция вернет false, если частица уже мертва
		if (!ParticleTmp->Update(TimeDelta, Location, IsAttractive, AttractiveValue)) {
			Detach(ParticleTmp);
			delete ParticleTmp;
			ParticleTmp = nullptr;
		}
		ParticleTmp = ParticleTmp2;
	}





	// подсчитываем, как много частиц нам нужно создать из ParticlesPerSec
	float ParticlesNeeded = (ParticlesPerSec/ParticleSystemQuality) * TimeDelta + EmissionResidue;

	// переводим в целочисленные значения
	ParticlesCreated = (int)ParticlesNeeded;

	if ( !IsSuppressed ) {
		// запоминаем разницу между тем сколько нужно и сколько создадим
		EmissionResidue = ParticlesNeeded - ParticlesCreated;
	} else {
		// ничего создавать не нужно
		EmissionResidue = ParticlesNeeded;
		ParticlesCreated = 0;
	}

	// если нужно что-то создавать, создаем
	if ( ParticlesCreated > 0 ) {
		// если пытаемся создать за один раз больше чем можем в секунду
		// убираем этот "глюк", видно компьютер тормозит
		if (ParticlesCreated > ParticlesPerSec) ParticlesCreated = ParticlesPerSec;
		// пока не создадим все необходимые частицы
		while (ParticlesCreated > 0) {
			// создаем новую частицу
			sParticle *NewParticle = new sParticle;

			// устанавливаем номер текстуры (если нужно, случайным образом выбираем из набора текстур)
			NewParticle->TextureNum = 0;
			if (TextureQuantity > 1) NewParticle->TextureNum = vw_iRandNum(TextureQuantity-1);

			// установка жизни новой частици и проверка, что не выходит из диапахона
			NewParticle->Age = 0.0f;
			NewParticle->Lifetime = Life + vw_Randf0 * LifeVar;
			if (NewParticle->Lifetime < 0.0f) NewParticle->Lifetime = 0.0f;

			// стартовый цвет
			NewParticle->Color.r = ColorStart.r + vw_Randf0 * ColorVar.r;
			NewParticle->Color.g = ColorStart.g + vw_Randf0 * ColorVar.g;
			NewParticle->Color.b = ColorStart.b + vw_Randf0 * ColorVar.b;
			// мы не используем альфа цвет тут
			NewParticle->Color.a = 1.0f;

			// проверяем, чтобы не было переполнения цвета
			vw_Clamp(NewParticle->Color.r, 0.0f, 1.0f);
			vw_Clamp(NewParticle->Color.g, 0.0f, 1.0f);
			vw_Clamp(NewParticle->Color.b, 0.0f, 1.0f);

			// считаем delta относительно жизни частицы
			NewParticle->ColorDelta.r = (ColorEnd.r - NewParticle->Color.r) / NewParticle->Lifetime;
			NewParticle->ColorDelta.g = (ColorEnd.g - NewParticle->Color.g) / NewParticle->Lifetime;
			NewParticle->ColorDelta.b = (ColorEnd.b - NewParticle->Color.b) / NewParticle->Lifetime;

			// считаем значение альфы
			NewParticle->Alpha = AlphaStart + vw_Randf0 * AlphaVar;
			// убираем переполнение
			vw_Clamp(NewParticle->Alpha, 0.0f, 1.0f);
			// считаем дельту альфы
			NewParticle->AlphaDelta = (AlphaEnd - NewParticle->Alpha) / NewParticle->Lifetime;
			// передаем тип изменения альфы
			NewParticle->AlphaShowHide = AlphaShowHide;
			// если нужно маргнуть, не обращаем внимания на Alpha вообще
			if (NewParticle->AlphaShowHide) {
				NewParticle->AlphaDelta = (2.0f-AlphaEnd*2.0f) / NewParticle->Lifetime;
				NewParticle->Alpha = AlphaEnd;
			}


			// выпускаем частицу возле места нахождения системы
			if (CreationType == 0) // точка
				NewParticle->Location = Location + sVECTOR3D(vw_Randf0 * CreationSize.x, vw_Randf0 * CreationSize.y, vw_Randf0 * CreationSize.z);
			if (CreationType == 1) {
				// в кубе
				sVECTOR3D tmp;
				if (DeadZone != 0.0f) {
					float minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;
				}

				tmp.x = vw_Randf0 * CreationSize.x;
				tmp.y = vw_Randf0 * CreationSize.y;
				tmp.z = vw_Randf0 * CreationSize.z;
				while (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z < DeadZone*DeadZone) {
					// ув. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1^(1/100.0f);
					tmp = tmp + tmp1;
				}

				vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
				NewParticle->Location = Location + tmp;
			}
			// тип 11 только !!! для лазеров-мазеров
			if (CreationType == 11) {
				// в кубе
				sVECTOR3D tmp;
				if (DeadZone != 0.0f) {
					float minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;
				}

				tmp.x = (0.5f - vw_Randf1) * CreationSize.x;
				tmp.y = (0.5f - vw_Randf1) * CreationSize.y;
				tmp.z = (0.5f - vw_Randf1) * CreationSize.z;
				sVECTOR3D CreationSizeText = CreationSize^(0.5f);
				while (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z < DeadZone*DeadZone) {
					// ув. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1^(1/100.0f);
					tmp = tmp + tmp1;
				}

				if (tmp.x > 0.0f) {
					if (tmp.x > CreationSizeText.x) tmp.x = CreationSizeText.x;
				} else {
					if (tmp.x < -CreationSizeText.x) tmp.x = -CreationSizeText.x;
				}

				if (tmp.y > 0.0f) {
					if (tmp.y > CreationSizeText.y) tmp.y = CreationSizeText.y;
				} else {
					if (tmp.y < -CreationSizeText.y) tmp.y = -CreationSizeText.y;
				}
				// делаем ручное регулирование
				if (tmp.z > 0.0f) {
					if (tmp.z > CreationSizeText.z) tmp.z = CreationSizeText.z;
				} else {
					if (tmp.z < -CreationSizeText.z) tmp.z = -CreationSizeText.z;
				}

				vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
				NewParticle->Location = Location + tmp;
			}
			if (CreationType == 2) {
				// в сфере
				sVECTOR3D tmp;
				float minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
				// если зона больше чем радиус излучения - выключаем ее
				if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;

				tmp.x = vw_Randf0 * CreationSize.x;
				tmp.y = vw_Randf0 * CreationSize.y;
				tmp.z = vw_Randf0 * CreationSize.z;
				float ParticleDist = tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z;
				while (ParticleDist > minDist || ParticleDist < DeadZone*DeadZone) {
					if (ParticleDist > minDist) {
						// ум. радиус
						sVECTOR3D tmp1 = tmp;
						tmp1.Normalize();
						tmp1 = tmp1^(1/100.0f);
						tmp = tmp - tmp1;
					}
					if ( ParticleDist < DeadZone*DeadZone) {
						// ув. радиус
						sVECTOR3D tmp1 = tmp;
						tmp1.Normalize();
						tmp1 = tmp1^(1/100.0f);
						tmp = tmp + tmp1;

						if (tmp.x > 0.0f) {
							if (tmp.x > CreationSize.x) tmp.x = CreationSize.x;
						} else {
							if (tmp.x < -CreationSize.x) tmp.x = -CreationSize.x;
						}

						if (tmp.y > 0.0f) {
							if (tmp.y > CreationSize.y) tmp.y = CreationSize.y;
						} else {
							if (tmp.y < -CreationSize.y) tmp.y = -CreationSize.y;
						}

						if (tmp.z > 0.0f) {
							if (tmp.z > CreationSize.z) tmp.z = CreationSize.z;
						} else {
							if (tmp.z < -CreationSize.z) tmp.z = -CreationSize.z;
						}
					}

					ParticleDist = tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z;
				}

				vw_Matrix33CalcPoint(tmp, CurrentRotationMat);
				NewParticle->Location = Location + tmp;
			}


			// учитываем перемещение системы, генерируем частицы равномерно
			sVECTOR3D L = (Location - PrevLocation)^((ParticlesNeeded-ParticlesCreated)/ParticlesNeeded);
			NewParticle->Location = NewParticle->Location - L;




			// считаем размер частицы
			NewParticle->Size = SizeStart + vw_Randf0 * SizeVar;
			if (NewParticle->Size < 0.0f) NewParticle->Size = SizeStart;
			NewParticle->SizeDelta = (SizeEnd - NewParticle->Size) / NewParticle->Lifetime;
			// если есть учет расстояния, работаем с ним
			if (Resize < 1.0f) {
				// получаем текущее положение камеры
				sVECTOR3D CurrentCameraLocation;
				vw_GetCameraLocation(&CurrentCameraLocation);
				// находим расстояние от центра системы до камеры
				float SystDist = (CurrentCameraLocation.x-Location.x-CreationSize.x)*(CurrentCameraLocation.x-Location.x-CreationSize.x) +
						 (CurrentCameraLocation.y-Location.y-CreationSize.y)*(CurrentCameraLocation.y-Location.y-CreationSize.y) +
						 (CurrentCameraLocation.z-Location.z-CreationSize.z)*(CurrentCameraLocation.z-Location.z-CreationSize.z);

				float ParticleDist = (CurrentCameraLocation.x-NewParticle->Location.x)*(CurrentCameraLocation.x-NewParticle->Location.x) +
						     (CurrentCameraLocation.y-NewParticle->Location.y)*(CurrentCameraLocation.y-NewParticle->Location.y) +
						     (CurrentCameraLocation.z-NewParticle->Location.z)*(CurrentCameraLocation.z-NewParticle->Location.z);

				// если частица ближе центра к камере - нужна корректировка
				if (ParticleDist < SystDist) {
					float tmpStart = SizeStart - SizeStart*(1.0f-Resize)*(SystDist-ParticleDist)/SystDist;
					float tmpEnd = SizeEnd - SizeEnd*(1.0f-Resize)*(SystDist-ParticleDist)/SystDist;
					float tmpVar = SizeVar - SizeVar*(1.0f-Resize)*(SystDist-ParticleDist)/SystDist;

					NewParticle->Size = tmpStart + vw_Randf0 * tmpVar;
					if (NewParticle->Size < 0.0f) NewParticle->Size = 0.0f;
					NewParticle->SizeDelta = (tmpEnd - NewParticle->Size) / NewParticle->Lifetime;
				}

			}




			// испускатель имеет направление. этот код немного добавляет случайности
			if (Theta == 0.0f) {
				NewParticle->Velocity = Direction;
			} else {

				NewParticle->Velocity = Direction;
				vw_RotatePoint(NewParticle->Velocity, sVECTOR3D(Theta*vw_Randf0/2.0f, Theta*vw_Randf0/2.0f, 0.0f));
			}



			NewParticle->NeedStop = NeedStop;


			// находим перемещение
			float NewSpeed = Speed + vw_Randf0 * SpeedVar;
			if (NewSpeed < 0.0f) NewSpeed = 0.0f;
			NewParticle->Velocity *= NewSpeed ;

			// подключаем частицу к системе
			Attach(NewParticle);

			// уменьшаем необходимое количество частиц
			ParticlesCreated--;
		}
	}


	// если уже ничего нет и нужно выйти - выходим
	if (DestroyIfNoParticles)
		if (ParticlesCount == 0) {
			NeedDestroy = true;
			return false;
		}



	// работа с источником света (если он есть)
	if (Light != nullptr) {
		// просто работаем
		if (!IsSuppressed) {
			if (LightNeedDeviation) {
				// было выключено, сейчас только включили опять
				if (LightDeviation > 1.0f) LightDeviation = 0.7f;

				if (NextLightDeviation < LightDeviation) {
					LightDeviation -= LightDeviationSpeed*TimeDelta;
					if (NextLightDeviation >= LightDeviation) {
						LightDeviation = NextLightDeviation;
						LightDeviationSpeed = 3.5f+3.5f*vw_Randf1;
						NextLightDeviation = 0.7+0.3f*vw_Randf1;
					}
				} else {
					LightDeviation += LightDeviationSpeed*TimeDelta;
					if (NextLightDeviation <= LightDeviation) {
						LightDeviation = NextLightDeviation;
						LightDeviationSpeed = 3.5f+3.5f*vw_Randf1;
						NextLightDeviation = 0.7-0.3f*vw_Randf1;
					}
				}
			} else
				// было выключено, сейчас только включили опять
				if (LightDeviation > 1.0f) LightDeviation = 1.0f;
		} else
			// было включено, сейчас выключили, нужно сделать исчезание
			if (IsSuppressed) {
				if (Life != 0.0f) LightDeviation += (25.0f/Life)*TimeDelta;
			}

		if (Light->LinearAttenuationBase != 0.0f) {
			Light->LinearAttenuation = Light->LinearAttenuationBase + 2.0f*(LightDeviation-0.5f)*Light->LinearAttenuationBase;
			if (Light->LinearAttenuation < Light->LinearAttenuationBase) Light->LinearAttenuation = Light->LinearAttenuationBase;
		}
		if (Light->QuadraticAttenuationBase != 0.0f) {
			Light->QuadraticAttenuation = Light->QuadraticAttenuationBase + 2.0f*(LightDeviation-0.5f)*Light->QuadraticAttenuationBase;
			if (Light->QuadraticAttenuation < Light->QuadraticAttenuationBase) Light->QuadraticAttenuation = Light->QuadraticAttenuationBase;
		}

		// если уже ничего нет, надо выключить свет, если что-то есть то включить
		if (ParticlesCount == 0) Light->On = false;
		else Light->On = true;
	}


	// находим новые данные AABB

	// предварительная инициализация
	float MinX, MinY, MinZ, MaxX, MaxY, MaxZ;
	ParticleTmp = Start;
	if (ParticleTmp == nullptr) {
		MinX = MaxX = Location.x;
		MinY = MaxY = Location.y;
		MinZ = MaxZ = Location.z;
	} else {
		MinX = MaxX = ParticleTmp->Location.x;
		MinY = MaxY = ParticleTmp->Location.y;
		MinZ = MaxZ = ParticleTmp->Location.z;
	}


	while (ParticleTmp != nullptr) {
		// строим AABB
		if (ParticleTmp->Alpha > 0.0f && ParticleTmp->Size > 0.0f) {
			sVECTOR3D v;
			v.x = ParticleTmp->Location.x + ParticleTmp->Size;
			if (MaxX < v.x) MaxX = v.x;

			v.y = ParticleTmp->Location.y + ParticleTmp->Size;
			if (MaxY < v.y) MaxY = v.y;

			v.z = ParticleTmp->Location.z + ParticleTmp->Size;
			if (MaxZ < v.z) MaxZ = v.z;

			v.x = ParticleTmp->Location.x - ParticleTmp->Size;
			if (MinX > v.x) MinX = v.x;

			v.y = ParticleTmp->Location.y - ParticleTmp->Size;
			if (MinY > v.y) MinY = v.y;

			v.z = ParticleTmp->Location.z - ParticleTmp->Size;
			if (MinZ > v.z) MinZ = v.z;
		}

		ParticleTmp = ParticleTmp->Next;
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






//-----------------------------------------------------------------------------
// прорисовка системы
//-----------------------------------------------------------------------------
void cParticleSystem::Draw(sTexture **CurrentTexture)
{
	// если не входит в фуструм - рисовать не нужно
	if (!vw_BoxInFrustum(AABB[6], AABB[0])) return;


	// т.к. у нас может быть набор текстур, обходим все текстуры по порядку
	for (int i=0; i<TextureQuantity; i++) {

		int DrawCount = 0;
		sParticle *ParticleTmp = Start;
		while (ParticleTmp != nullptr) {
			if (ParticleTmp->TextureNum == i) DrawCount++;
			ParticleTmp = ParticleTmp->Next;
		}


		// если есть живые - рисуем их
		if (DrawCount > 0) {
			if (tmpDATA != nullptr) {
				delete [] tmpDATA;
				tmpDATA = nullptr;
			}

			GLubyte *tmpDATAub = nullptr;
			// номер float'а в последовательности
			int k=0;


			// делаем массив для всех элементов
			// RI_3f_XYZ | RI_2f_TEX | RI_4ub_COLOR
			tmpDATA = new float[4*(3+2+1)*DrawCount];
			tmpDATAub = (GLubyte *)tmpDATA;


			GLubyte R,G,B,A;

			// шейдеры не поддерживаются - рисуем по старинке
			if (!ParticleSystemUseGLSL) {
				// получаем текущее положение камеры
				sVECTOR3D CurrentCameraLocation;
				vw_GetCameraLocation(&CurrentCameraLocation);

				sParticle *tmp = Start;
				while (tmp != nullptr) {
					if (tmp->TextureNum != i) {
						tmp = tmp->Next;
						continue;
					}

					// находим вектор камера-точка
					sVECTOR3D nnTmp;
					nnTmp = CurrentCameraLocation - tmp->Location;
					//nnTmp.Normalize();// - это тут не нужно, нам нужны только пропорции

					// находим перпендикуляр к вектору nnTmp
					sVECTOR3D nnTmp2;
					nnTmp2.x = 1.0f;
					nnTmp2.y = 1.0f;
					nnTmp2.z = -(nnTmp.x + nnTmp.y)/nnTmp.z;
					nnTmp2.Normalize();

					// находим перпендикуляр к векторам nnTmp и nnTmp2
					// файтически - a x b = ( aybz - byaz , azbx - bzax , axby - bxay );
					sVECTOR3D nnTmp3;
					nnTmp3.x = nnTmp.y*nnTmp2.z - nnTmp2.y*nnTmp.z;
					nnTmp3.y = nnTmp.z*nnTmp2.x - nnTmp2.z*nnTmp.x;
					nnTmp3.z = nnTmp.x*nnTmp2.y - nnTmp2.x*nnTmp.y;
					nnTmp3.Normalize();

					// находим
					sVECTOR3D tmpAngle1,tmpAngle2,tmpAngle3,tmpAngle4;

					tmpAngle1 = nnTmp3^(tmp->Size*1.5f);
					tmpAngle3 = nnTmp3^(-tmp->Size*1.5f);
					tmpAngle2 = nnTmp2^(tmp->Size*1.5f);
					tmpAngle4 = nnTmp2^(-tmp->Size*1.5f);

					// собираем квадраты
					R = (GLubyte)(tmp->Color.r*255);
					G = (GLubyte)(tmp->Color.g*255);
					B = (GLubyte)(tmp->Color.b*255);
					A = (GLubyte)(tmp->Alpha*255);

					tmpDATA[k++] = tmp->Location.x+tmpAngle3.x;
					tmpDATA[k++] = tmp->Location.y+tmpAngle3.y;
					tmpDATA[k++] = tmp->Location.z+tmpAngle3.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 0.0f;
					tmpDATA[k++] = 1.0f;

					tmpDATA[k++] = tmp->Location.x+tmpAngle2.x;
					tmpDATA[k++] = tmp->Location.y+tmpAngle2.y;
					tmpDATA[k++] = tmp->Location.z+tmpAngle2.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 0.0f;
					tmpDATA[k++] = 0.0f;

					tmpDATA[k++] = tmp->Location.x+tmpAngle1.x;
					tmpDATA[k++] = tmp->Location.y+tmpAngle1.y;
					tmpDATA[k++] = tmp->Location.z+tmpAngle1.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 1.0f;
					tmpDATA[k++] = 0.0f;

					tmpDATA[k++] = tmp->Location.x+tmpAngle4.x;
					tmpDATA[k++] = tmp->Location.y+tmpAngle4.y;
					tmpDATA[k++] = tmp->Location.z+tmpAngle4.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 1.0f;
					tmpDATA[k++] = 1.0f;

					tmp = tmp->Next;
				}
			} else { // иначе работаем с шейдерами, в них правильно развернем билборд
				sParticle *tmp = Start;
				while (tmp != nullptr) {
					if (tmp->TextureNum != i) {
						tmp = tmp->Next;
						continue;
					}

					// собираем квадраты
					R = (GLubyte)(tmp->Color.r*255);
					G = (GLubyte)(tmp->Color.g*255);
					B = (GLubyte)(tmp->Color.b*255);
					A = (GLubyte)(tmp->Alpha*255);

					tmpDATA[k++] = tmp->Location.x;
					tmpDATA[k++] = tmp->Location.y;
					tmpDATA[k++] = tmp->Location.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 1.0f;
					tmpDATA[k++] = tmp->Size;

					tmpDATA[k++] = tmp->Location.x;
					tmpDATA[k++] = tmp->Location.y;
					tmpDATA[k++] = tmp->Location.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 2.0f;
					tmpDATA[k++] = tmp->Size;

					tmpDATA[k++] = tmp->Location.x;
					tmpDATA[k++] = tmp->Location.y;
					tmpDATA[k++] = tmp->Location.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 3.0f;
					tmpDATA[k++] = tmp->Size;

					tmpDATA[k++] = tmp->Location.x;
					tmpDATA[k++] = tmp->Location.y;
					tmpDATA[k++] = tmp->Location.z;
					tmpDATAub[k*sizeof(float)] = R;
					tmpDATAub[k*sizeof(float)+1] = G;
					tmpDATAub[k*sizeof(float)+2] = B;
					tmpDATAub[k*sizeof(float)+3] = A;
					k++;
					tmpDATA[k++] = 4.0f;
					tmpDATA[k++] = tmp->Size;

					tmp = tmp->Next;
				}
			}
		}


		if (DrawCount > 0) {

			if (*CurrentTexture != Texture[i]) {
				vw_SetTexture(0, Texture[i]);
				*CurrentTexture = Texture[i];
			}


			if (BlendType == 1) vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

			vw_SendVertices(RI_QUADS, 4*DrawCount, RI_3f_XYZ | RI_4ub_COLOR | RI_1_TEX, tmpDATA, 6*sizeof(tmpDATA[0]));

			if (BlendType != 0) vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);

		}
	}
}





//-----------------------------------------------------------------------------
// поставить правильный полет частиц, т.е. учет внешней скорости
//-----------------------------------------------------------------------------
void cParticleSystem::SetStartLocation(sVECTOR3D NewLocation)
{
	PrevLocation = NewLocation;
	Location = NewLocation;

	if (Light != nullptr)
		Light->SetLocation(Location);
}



//-----------------------------------------------------------------------------
// перемещение всех частиц и центра
//-----------------------------------------------------------------------------
void cParticleSystem::MoveSystem(sVECTOR3D NewLocation)
{
	// чтобы не сбивать PrevLocation
	if (Location == NewLocation)
		return;
	PrevLocation = Location;
	Location = NewLocation;

	sParticle *tmp = Start;
	while (tmp != nullptr) {
		// меняем каждой частице
		tmp->Location += NewLocation-PrevLocation;
		tmp = tmp->Next;
	}


	if (Light != nullptr)
		Light->SetLocation(Location);
}


//-----------------------------------------------------------------------------
// перемещение центра
//-----------------------------------------------------------------------------
void cParticleSystem::MoveSystemLocation(sVECTOR3D NewLocation)
{
	// чтобы не сбивать PrevLocation
	if (Location == NewLocation)
		return;
	PrevLocation = Location;
	Location = NewLocation;

	if (Light != nullptr)
		Light->SetLocation(Location);
}


//-----------------------------------------------------------------------------
// установка направления "движения" системы
//-----------------------------------------------------------------------------
void cParticleSystem::RotateSystemByAngle(sVECTOR3D NewAngle)
{
	Angle = NewAngle;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9*sizeof(CurrentRotationMat[0]));
	// делаем инверсную старую матрицу
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Angle);

	vw_Matrix33CalcPoint(Direction, OldInvRotationMat);
	vw_Matrix33CalcPoint(Direction, CurrentRotationMat);
}



//-----------------------------------------------------------------------------
// установка направления "движения" системы + перенос всех частиц
//-----------------------------------------------------------------------------
void cParticleSystem::RotateSystemAndParticlesByAngle(sVECTOR3D NewAngle)
{
	Angle = NewAngle;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9*sizeof(CurrentRotationMat[0]));
	// делаем инверсную старую матрицу
	vw_Matrix33InverseRotate(OldInvRotationMat);
	vw_Matrix33CreateRotate(CurrentRotationMat, Angle);

	vw_Matrix33CalcPoint(Direction, OldInvRotationMat);
	vw_Matrix33CalcPoint(Direction, CurrentRotationMat);


	sParticle *tmp = Start;
	while (tmp != nullptr) {
		// меняем каждой частице
		sVECTOR3D TMP = tmp->Location - Location;
		vw_Matrix33CalcPoint(TMP, OldInvRotationMat);
		vw_Matrix33CalcPoint(TMP, CurrentRotationMat);
		tmp->Location = TMP + Location;
		tmp = tmp->Next;
	}
}




//-----------------------------------------------------------------------------
// поворот частиц на угол без поворота системы
//-----------------------------------------------------------------------------
void cParticleSystem::RotateParticlesByAngle(sVECTOR3D NewAngle)
{
	// делаем обратку для отматывания назад
	float TmpOldInvRotationMat[9];
	memcpy(TmpOldInvRotationMat, CurrentRotationMat, 9*sizeof(CurrentRotationMat[0]));
	vw_Matrix33InverseRotate(TmpOldInvRotationMat);

	// матрица поворота частиц
	float TmpRotationMat[9];
	vw_Matrix33CreateRotate(TmpRotationMat, NewAngle);


	sParticle *tmp = Start;
	while (tmp != nullptr) {
		// меняем каждой частице
		sVECTOR3D TMP = tmp->Location - Location;
		vw_Matrix33CalcPoint(TMP, TmpOldInvRotationMat);
		// поворачиваем каждую точку
		vw_Matrix33CalcPoint(TMP, TmpRotationMat);
		vw_Matrix33CalcPoint(TMP, CurrentRotationMat);
		tmp->Location = TMP + Location;
		tmp = tmp->Next;
	}

}





//-----------------------------------------------------------------------------
// остановить все частицы в системе
//-----------------------------------------------------------------------------
void cParticleSystem::StopAllParticles()
{
	Speed = 0.0f;
	IsAttractive = false;

	sParticle *tmp = Start;
	while (tmp != nullptr) {
		// меняем каждой частице
		tmp->Velocity = sVECTOR3D(0.0f,0.0f,0.0f);
		tmp = tmp->Next;
	}
}




//-----------------------------------------------------------------------------
// поставить правильный полет частиц, т.е. учет внешней скорости
//-----------------------------------------------------------------------------
void cParticleSystem::ChangeSpeed(sVECTOR3D Vel)
{
	sParticle *tmp = Start;
	while (tmp != nullptr) {
		// меняем каждой частице
		tmp->Velocity += Vel;
		tmp = tmp->Next;
	}
}
