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


#include "particle_system2d.h"
#include "../graphics/graphics.h"





//-----------------------------------------------------------------------------
//	При разрушении класса
//-----------------------------------------------------------------------------
cParticleSystem2D::~cParticleSystem2D()
{
	// полностью освобождаем память от всех частиц в системе
	sParticle2D *tmp = Start;
	while (tmp != nullptr) {
		sParticle2D *tmp2 = tmp->Next;
		Detach(tmp);
		delete tmp;
		tmp = tmp2;
	}
}


//-----------------------------------------------------------------------------
//	подключить частицу к системе
//-----------------------------------------------------------------------------
void cParticleSystem2D::Attach(sParticle2D * NewParticle)
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
void cParticleSystem2D::Detach(sParticle2D * OldParticle)
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
bool cParticleSystem2D::Update(float Time)
{
	// первый раз... просто берем время
	if (TimeLastUpdate == -1.0f) {
		TimeLastUpdate = Time;
		return true;
	}

	// Time - это абсолютное время, вычисляем дельту
	float TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;


	// для всех частиц
	sParticle2D *Particle2DTmp = Start;

	while (Particle2DTmp != nullptr) {
		sParticle2D *Particle2DTmp2 = Particle2DTmp->Next;
		// функция вернет false, если частица уже мертва
		if (!Particle2DTmp->Update(TimeDelta, Location, IsAttractive, AttractiveValue)) {
			Detach(Particle2DTmp);
			delete Particle2DTmp;
		}

		Particle2DTmp = Particle2DTmp2;
	}



	// подсчитываем, как много частиц нам нужно создать из ParticlesPerSec
	float ParticlesNeeded = ParticlesPerSec * TimeDelta + EmissionResidue;

	// переводим в целочисленные значения
	unsigned int ParticlesCreated = (unsigned int)ParticlesNeeded;

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
			sParticle2D *NewParticle = new sParticle2D;

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


			// выпускаем частицу возле места нахождения системы
			if (CreationType == 0) // точка
				NewParticle->Location = Location + sVECTOR3D(vw_Randf0 * CreationSize.x, vw_Randf0 * CreationSize.y, vw_Randf0 * CreationSize.z);
			if (CreationType == 1) {
				// в квадрате
				sVECTOR3D tmp;
				if (DeadZone != 0.0f) {
					float minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;
				}

				tmp.x = (1.0f-vw_Randf1*2) * CreationSize.x;
				tmp.y = (1.0f-vw_Randf1*2) * CreationSize.y;
				tmp.z = (1.0f-vw_Randf1*2) * CreationSize.z;
				while (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z < DeadZone*DeadZone) {
					// ув. радиус
					sVECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1^(1/100.0f);
					tmp = tmp + tmp1;
				}

				NewParticle->Location = Location + tmp;
			}
			if (CreationType == 2) {
				// в окружности
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

				NewParticle->Location = Location + tmp;
			}





			// считаем размер частицы
			NewParticle->Size = SizeStart + vw_Randf0 * SizeVar;
			if (NewParticle->Size < 0.0f) NewParticle->Size = 0.0f;
			NewParticle->SizeDelta = (SizeEnd - NewParticle->Size) / NewParticle->Lifetime;




			// испускатель имеет направление. этот код немного добавляет случайности
			float RandomYaw = vw_Randf0 * 3.14159f * 2.0f;
			float RandomPitch = vw_Randf0 * Theta * 3.14159f / 180.0f ;

			// учитываем нужное нам направление, вектор Direction
			if ((Direction.x != 0.0f || Direction.y != 0.0f || Direction.z != 0.0f) &&
			    360.00f != Theta) {
				if (Theta == 0.0f) {
					NewParticle->Velocity = Direction;
				} else {
					// y
					NewParticle->Velocity.y = Direction.y * cosf(RandomPitch);
					NewParticle->Velocity.x = Direction.y * sinf(RandomPitch) * cosf(RandomYaw);
					NewParticle->Velocity.z = Direction.y * sinf(RandomPitch) * sinf(RandomYaw);
					// x
					NewParticle->Velocity.y += Direction.x * sinf(RandomPitch) * cosf(RandomYaw);
					NewParticle->Velocity.x += Direction.x * cosf(RandomPitch);
					NewParticle->Velocity.z += Direction.x * sinf(RandomPitch) * sinf(RandomYaw);
					// z
					NewParticle->Velocity.y += Direction.z * sinf(RandomPitch) * sinf(RandomYaw);
					NewParticle->Velocity.x += Direction.z * sinf(RandomPitch) * cosf(RandomYaw);
					NewParticle->Velocity.z += Direction.z * cosf(RandomPitch);

					vw_Matrix33CalcPoint(&NewParticle->Velocity, RotationMatrix);
				}
			} else {
				// без направления, излучаем во все стороны
				NewParticle->Velocity.y = vw_Randf0 * Theta / 360.0f;
				NewParticle->Velocity.x = vw_Randf0 * Theta / 360.0f;
				NewParticle->Velocity.z = vw_Randf0 * Theta / 360.0f;
				NewParticle->Velocity.Normalize();
			}


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

	return true;
}





//-----------------------------------------------------------------------------
// прорисовка системы
//-----------------------------------------------------------------------------
void cParticleSystem2D::Draw()
{

	// загрузка текстуры, уже должна быть подключена
	if ((Texture == nullptr) || (ParticlesCount == 0))
		return;

	sRECT SrcRect, DestRect;

	SetRect(&SrcRect,0,0,64,64);


	float AW;
	float AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTexture(0, Texture);
	vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);


	float ImageHeight = Texture->Height*1.0f;
	float ImageWidth = Texture->Width*1.0f;

	float tmpPosY = 0;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = new float[(2+2+4)*4*ParticlesCount];

	int k = 0;




	// для всех частиц
	sParticle2D *tmp1 = Start;

	while (tmp1 != nullptr) {
		SetRect(&DestRect,(int)(tmp1->Location.x - tmp1->Size/2),
			(int)(tmp1->Location.y - tmp1->Size/2),
			(int)(tmp1->Location.x + tmp1->Size/2),
			(int)(tmp1->Location.y + tmp1->Size/2));

		if (ASpresent) tmpPosY = (AH - DestRect.top - DestRect.top - (DestRect.bottom - DestRect.top));
		else tmpPosY = (AHw - DestRect.top - DestRect.top - (DestRect.bottom - DestRect.top));

		float FrameHeight = (SrcRect.bottom*1.0f )/ImageHeight;
		float FrameWidth = (SrcRect.right*1.0f )/ImageWidth;

		float Yst = (SrcRect.top*1.0f)/ImageHeight;
		float Xst = (SrcRect.left*1.0f)/ImageWidth;

		tmp[k++] = DestRect.left;
		tmp[k++] = DestRect.top +tmpPosY +(DestRect.bottom - DestRect.top);
		tmp[k++] = tmp1->Color.r;
		tmp[k++] = tmp1->Color.g;
		tmp[k++] = tmp1->Color.b;
		tmp[k++] = tmp1->Alpha;
		tmp[k++] = Xst;
		tmp[k++] = 1.0f-Yst;

		tmp[k++] = DestRect.left;
		tmp[k++] = DestRect.top +tmpPosY;
		tmp[k++] = tmp1->Color.r;
		tmp[k++] = tmp1->Color.g;
		tmp[k++] = tmp1->Color.b;
		tmp[k++] = tmp1->Alpha;
		tmp[k++] = Xst;
		tmp[k++] = 1.0f-FrameHeight;

		tmp[k++] = DestRect.left + (DestRect.right - DestRect.left);
		tmp[k++] = DestRect.top +tmpPosY;
		tmp[k++] = tmp1->Color.r;
		tmp[k++] = tmp1->Color.g;
		tmp[k++] = tmp1->Color.b;
		tmp[k++] = tmp1->Alpha;
		tmp[k++] = FrameWidth;
		tmp[k++] = 1.0f-FrameHeight;

		tmp[k++] = DestRect.left + (DestRect.right - DestRect.left);
		tmp[k++] = DestRect.top +tmpPosY +(DestRect.bottom - DestRect.top);
		tmp[k++] = tmp1->Color.r;
		tmp[k++] = tmp1->Color.g;
		tmp[k++] = tmp1->Color.b;
		tmp[k++] = tmp1->Alpha;
		tmp[k++] = FrameWidth;
		tmp[k++] = 1.0f-Yst;

		tmp1 = tmp1->Next;
	}


	vw_SendVertices(RI_QUADS, 4*ParticlesCount, RI_2f_XY | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(tmp[0]));

	if (tmp != nullptr)
		delete [] tmp;

	vw_SetTextureBlend(false, 0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);

}


//-----------------------------------------------------------------------------
// перемещение всех частиц и центра
//-----------------------------------------------------------------------------
void cParticleSystem2D::MoveSystem(sVECTOR3D NewLocation)
{
	sVECTOR3D PrevLocation = Location;
	Location = NewLocation;


	sParticle2D *tmp = Start;
	while (tmp != nullptr) {
		// меняем каждой частице
		tmp->Location += NewLocation-PrevLocation;
		tmp = tmp->Next;
	}
}


//-----------------------------------------------------------------------------
// перемещение центра
//-----------------------------------------------------------------------------
void cParticleSystem2D::MoveSystemLocation(sVECTOR3D NewLocation)
{
	Location = NewLocation;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void cParticleSystem2D::SetRotation(sVECTOR3D NewAngle)
{
	vw_Matrix33CreateRotate(RotationMatrix, Angle^-1);
	vw_Matrix33CreateRotate(RotationMatrix, NewAngle);
	Angle = NewAngle;
}












