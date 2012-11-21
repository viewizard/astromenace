/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "ParticleSystem.h"
#include "../Camera/Camera.h"
#include "../RendererInterface/RendererInterface.h"



extern bool ParticleSystemUseGLSL;
extern float ParticleSystemQuality;




//-----------------------------------------------------------------------------
// инициализация класса
//-----------------------------------------------------------------------------
eParticleSystem::eParticleSystem()
{
	NeedDestroy = false;
	Age = 0.0f;
	TimeLastUpdate = -1.0f;
	EmissionResidue =  0.0f;
	Angle = Direction = VECTOR3D(0,0,0);
	Texture[0] = Texture[1] = Texture[2] = Texture[3] = Texture[4] = 0;
	TextureQuantity = 1; // по умолчанию у нас всегда 1 текстура
	BlendType = 0;
	AttractiveValue = 25.0f;
	IsSuppressed = false;
	DestroyIfNoParticles = false;

	// положение системы
	Location = PrevLocation = VECTOR3D( 0.0f, 0.0f, 0.0f);

	DeadZone = 0.0f; // нет мертвой зоны
	Resize = 1.0f; // нет изменений


	ColorStart.r = 1.00f;
	ColorStart.g = 1.00f;
	ColorStart.b = 1.00f;

	ColorVar.r = 0.00f;
	ColorVar.g = 0.00f;
	ColorVar.b = 0.00f;

	ColorEnd.r = 1.00f;
	ColorEnd.g = 1.00f;
	ColorEnd.b = 1.00f;

	AlphaStart = 1.00f;
	AlphaVar   = 0.00f;
	AlphaEnd   = 1.00f;
	AlphaShowHide = false;

	SizeStart  = 1.00f;
	SizeVar    = 0.00f;
	SizeEnd    = 1.00f;

	Speed      = 1.00f;
	SpeedOnCreation	   = 1.00f;
	SpeedVar   = 1.00f;
	Theta      = 1.00f;

	Life       = 1.00f;
	LifeVar    = 0.00f;

	NeedStop = false;

	ParticlesPerSec = 100;
	IsAttractive = 0;
	ParticlesCreated = 0;

	CreationType = 0;
	CreationSize = VECTOR3D(0.05f,0.05f,0.05f);

	// начальные установки для мартиц поворотов
	Matrix33Identity(CurrentRotationMat);
	Matrix33Identity(OldInvRotationMat);

	Light = 0;
	LightNeedDeviation = false;
	NextLightDeviation = 0.7f+0.3f*vw_Randf1;
	LightDeviation = 100.0f;
	LightDeviationSpeed = 3.5f+3.5f*vw_Randf1;

	tmpDATA = 0;

	// настройка массива
	Start = 0;
	End = 0;
	Next = 0;
	Prev = 0;
	vw_AttachParticleSystem(this);


	// начальныя установка коробок
	// ставим тут очень большие параметры, вне зоны прорисовки, т.к. в партиклах на функциях перемещения
	// не учитываем изменение коробки, и координаты в коробках у нас абсолютные (!!!)
	// использовать относительные координаты бокса нельзя, т.к. можем преносить только центр, а можем
	// и центр, и все частицы...
	AABB[0]=AABB[1]=AABB[2]=AABB[3]=AABB[4]=AABB[5]=AABB[6]=AABB[7]= VECTOR3D(-1000000.0f, 1000000.0f, -1000000.0f);
}


//-----------------------------------------------------------------------------
//	При разрушении класса
//-----------------------------------------------------------------------------
eParticleSystem::~eParticleSystem()
{
	// полностью освобождаем память от всех частиц в системе
	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		Detach(tmp);
		delete tmp; tmp = 0;
		tmp = tmp2;
	}
	if (Light != 0){vw_ReleaseLight(Light); Light = 0;}
	if (tmpDATA!=0){delete [] tmpDATA; tmpDATA = 0;}
	vw_DetachParticleSystem(this);
}


//-----------------------------------------------------------------------------
//	подключить частицу к системе
//-----------------------------------------------------------------------------
void eParticleSystem::Attach(eParticle * NewParticle)
{
	if (NewParticle == 0) return;

	// первый в списке...
	if (End == 0)
	{
		NewParticle->Prev = 0;
		NewParticle->Next = 0;
		Start = NewParticle;
		End = NewParticle;
	}
	else // продолжаем заполнение...
	{
		NewParticle->Prev = End;
		NewParticle->Next = 0;
		End->Next = NewParticle;
		End = NewParticle;
	}

}


//-----------------------------------------------------------------------------
//	отключить ее от системы
//-----------------------------------------------------------------------------
void eParticleSystem::Detach(eParticle * OldParticle)
{
	if (OldParticle == 0) return;

	// переустанавливаем указатели...
	if (Start == OldParticle) Start = OldParticle->Next;
	if (End == OldParticle) End = OldParticle->Prev;


	if (OldParticle->Next != 0) OldParticle->Next->Prev = OldParticle->Prev;
		else if (OldParticle->Prev != 0) OldParticle->Prev->Next = 0;
	if (OldParticle->Prev != 0) OldParticle->Prev->Next = OldParticle->Next;
		else if (OldParticle->Next != 0) OldParticle->Next->Prev = 0;

}





//-----------------------------------------------------------------------------
// обновление системы
//-----------------------------------------------------------------------------
bool eParticleSystem::Update(float Time)
{
	// первый раз... просто берем время
	if (TimeLastUpdate < 0.0f) {TimeLastUpdate = Time;return true;}

	// Time - это абсолютное время, вычисляем дельту
	float TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;


	// очищаем счетчик подсчета кол-ва действующий частиц
	ParticlesAlive = 0;


	// для всех частиц вызываем их собственный апдейт
	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		// функция вернет false, если частица уже мертва
		if (tmp->Update(TimeDelta, Location, IsAttractive, AttractiveValue))
		{
			ParticlesAlive++;
		}
		else
		{
			Detach(tmp);
			delete tmp; tmp = 0;
		}
		tmp = tmp2;
	}





	// подсчитываем, как много частиц нам нужно создать из ParticlesPerSec
	float ParticlesNeeded = (ParticlesPerSec/ParticleSystemQuality) * TimeDelta + EmissionResidue;

	// переводим в целочисленные значения
	ParticlesCreated = (int)ParticlesNeeded;

	if ( !IsSuppressed )
	{
		// запоминаем разницу между тем сколько нужно и сколько создадим
		EmissionResidue = ParticlesNeeded - ParticlesCreated;
	}
	else
	{
		// ничего создавать не нужно
		EmissionResidue = ParticlesNeeded;
		ParticlesCreated = 0;
	}

	// если нужно что-то создавать, создаем
	if ( ParticlesCreated > 0 )
	{
		// если пытаемся создать за один раз больше чем можем в секунду
		// убираем этот "глюк", видно компьютер тормозит
		if (ParticlesCreated > ParticlesPerSec) ParticlesCreated = ParticlesPerSec;
		// пока не создадим все необходимые частицы
		while (ParticlesCreated > 0)
		{
			// создаем новую частицу
			eParticle *NewParticle = 0;
			NewParticle = new eParticle; if (NewParticle == 0) return true;

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
			Clamp( NewParticle->Color.r, 0.0f, 1.0f );
			Clamp( NewParticle->Color.g, 0.0f, 1.0f );
			Clamp( NewParticle->Color.b, 0.0f, 1.0f );

			// считаем delta относительно жизни частицы
			NewParticle->ColorDelta.r = (ColorEnd.r - NewParticle->Color.r) / NewParticle->Lifetime;
			NewParticle->ColorDelta.g = (ColorEnd.g - NewParticle->Color.g) / NewParticle->Lifetime;
			NewParticle->ColorDelta.b = (ColorEnd.b - NewParticle->Color.b) / NewParticle->Lifetime;

			// считаем значение альфы
			NewParticle->Alpha = AlphaStart + vw_Randf0 * AlphaVar;
			// убираем переполнение
			Clamp( NewParticle->Alpha, 0.0f, 1.0f );
			// считаем дельту альфы
			NewParticle->AlphaDelta = (AlphaEnd - NewParticle->Alpha) / NewParticle->Lifetime;
			// передаем тип изменения альфы
			NewParticle->AlphaShowHide = AlphaShowHide;
			// если нужно маргнуть, не обращаем внимания на Alpha вообще
			if (NewParticle->AlphaShowHide)
			{
				NewParticle->AlphaDelta = (2.0f-AlphaEnd*2.0f) / NewParticle->Lifetime;
				NewParticle->Alpha = AlphaEnd;
			}


			// выпускаем частицу возле места нахождения системы
			if (CreationType == 0) // точка
				NewParticle->Location = Location + VECTOR3D(vw_Randf0 * CreationSize.x, vw_Randf0 * CreationSize.y, vw_Randf0 * CreationSize.z);
			if (CreationType == 1)
			{
				// в кубе
				VECTOR3D tmp;
				float minDist;
				if (DeadZone != 0.0f)
				{
					minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;
				}

				tmp.x = vw_Randf0 * CreationSize.x;
				tmp.y = vw_Randf0 * CreationSize.y;
				tmp.z = vw_Randf0 * CreationSize.z;
				while (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z < DeadZone*DeadZone)
				{
					// ув. радиус
					VECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1^(1/100.0f);
					tmp = tmp + tmp1;
				}

				Matrix33CalcPoint(&tmp, CurrentRotationMat);
				NewParticle->Location = Location + tmp;
			}
			// тип 11 только !!! для лазеров-мазеров
			if (CreationType == 11)
			{
				// в кубе
				VECTOR3D tmp;
				float minDist;
				if (DeadZone != 0.0f)
				{
					minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;
				}

				tmp.x = (0.5f - vw_Randf1) * CreationSize.x;
				tmp.y = (0.5f - vw_Randf1) * CreationSize.y;
				tmp.z = (0.5f - vw_Randf1) * CreationSize.z;
				VECTOR3D CreationSizeText = CreationSize^(0.5f);
				while (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z < DeadZone*DeadZone)
				{
					// ув. радиус
					VECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1^(1/100.0f);
					tmp = tmp + tmp1;
				}

						if (tmp.x > 0.0f)
						{if (tmp.x > CreationSizeText.x) tmp.x = CreationSizeText.x;}
						else
						{if (tmp.x < -CreationSizeText.x) tmp.x = -CreationSizeText.x;}

						if (tmp.y > 0.0f)
						{if (tmp.y > CreationSizeText.y) tmp.y = CreationSizeText.y;}
						else
						{if (tmp.y < -CreationSizeText.y) tmp.y = -CreationSizeText.y;}
							// делаем ручное регулирование
						if (tmp.z > 0.0f)
						{if (tmp.z > CreationSizeText.z) tmp.z = CreationSizeText.z;}
						else
						{if (tmp.z < -CreationSizeText.z) tmp.z = -CreationSizeText.z;}

				Matrix33CalcPoint(&tmp, CurrentRotationMat);
				NewParticle->Location = Location + tmp;
			}
			if (CreationType == 2)
			{
				// в сфере
				VECTOR3D tmp;
				float minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
				// если зона больше чем радиус излучения - выключаем ее
				if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;

				tmp.x = vw_Randf0 * CreationSize.x;
				tmp.y = vw_Randf0 * CreationSize.y;
				tmp.z = vw_Randf0 * CreationSize.z;
				float ParticleDist = tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z;
				while (ParticleDist > minDist || ParticleDist < DeadZone*DeadZone)
				{
					if (ParticleDist > minDist)
					{
						// ум. радиус
						VECTOR3D tmp1 = tmp;
						tmp1.Normalize();
						tmp1 = tmp1^(1/100.0f);
						tmp = tmp - tmp1;
					}
					if ( ParticleDist < DeadZone*DeadZone)
					{
						// ув. радиус
						VECTOR3D tmp1 = tmp;
						tmp1.Normalize();
						tmp1 = tmp1^(1/100.0f);
						tmp = tmp + tmp1;

						if (tmp.x > 0.0f)
						{if (tmp.x > CreationSize.x) tmp.x = CreationSize.x;}
						else
						{if (tmp.x < -CreationSize.x) tmp.x = -CreationSize.x;}

						if (tmp.y > 0.0f)
						{if (tmp.y > CreationSize.y) tmp.y = CreationSize.y;}
						else
						{if (tmp.y < -CreationSize.y) tmp.y = -CreationSize.y;}

						if (tmp.z > 0.0f)
						{if (tmp.z > CreationSize.z) tmp.z = CreationSize.z;}
						else
						{if (tmp.z < -CreationSize.z) tmp.z = -CreationSize.z;}
					}

					ParticleDist = tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z;
				}

				Matrix33CalcPoint(&tmp, CurrentRotationMat);
				NewParticle->Location = Location + tmp;
			}


			// учитываем перемещение системы, генерируем частицы равномерно
			VECTOR3D L = (Location - PrevLocation)^((ParticlesNeeded-ParticlesCreated)/ParticlesNeeded);
			NewParticle->Location = NewParticle->Location - L;




			// считаем размер частицы
			NewParticle->Size = SizeStart + vw_Randf0 * SizeVar;
			if (NewParticle->Size < 0.0f) NewParticle->Size = 0.0f;
			NewParticle->SizeDelta = (SizeEnd - NewParticle->Size) / NewParticle->Lifetime;
			// если есть учет расстояния, работаем с ним
			if (Resize < 1.0f)
			{
				// получаем текущее положение камеры
				VECTOR3D CurrentCameraLocation;
				vw_GetCameraLocation(&CurrentCameraLocation);
				// находим расстояние от центра системы до камеры
					float SystDist = (CurrentCameraLocation.x-Location.x-CreationSize.x)*(CurrentCameraLocation.x-Location.x-CreationSize.x) +
					(CurrentCameraLocation.y-Location.y-CreationSize.y)*(CurrentCameraLocation.y-Location.y-CreationSize.y) +
					(CurrentCameraLocation.z-Location.z-CreationSize.z)*(CurrentCameraLocation.z-Location.z-CreationSize.z);

				float ParticleDist = (CurrentCameraLocation.x-NewParticle->Location.x)*(CurrentCameraLocation.x-NewParticle->Location.x) +
					(CurrentCameraLocation.y-NewParticle->Location.y)*(CurrentCameraLocation.y-NewParticle->Location.y) +
					(CurrentCameraLocation.z-NewParticle->Location.z)*(CurrentCameraLocation.z-NewParticle->Location.z);

				// если частица ближе центра к камере - нужна корректировка
				if (ParticleDist < SystDist)
				{
					float tmpStart = SizeStart - SizeStart*(1.0f-Resize)*(SystDist-ParticleDist)/SystDist;
					float tmpEnd = SizeEnd - SizeEnd*(1.0f-Resize)*(SystDist-ParticleDist)/SystDist;
					float tmpVar = SizeVar - SizeVar*(1.0f-Resize)*(SystDist-ParticleDist)/SystDist;

					NewParticle->Size = tmpStart + vw_Randf0 * tmpVar;
					if (NewParticle->Size < 0.0f) NewParticle->Size = 0.0f;
					NewParticle->SizeDelta = (tmpEnd - NewParticle->Size) / NewParticle->Lifetime;
				}

			}




			// испускатель имеет направление. этот код немного добавляет случайности
			if (Theta == 0.0f)
			{
				NewParticle->Velocity = Direction;
			}
			else
			{

				NewParticle->Velocity = Direction;
				RotatePoint(&NewParticle->Velocity, VECTOR3D(Theta*vw_Randf0/2.0f, Theta*vw_Randf0/2.0f, 0.0f));
			}



			NewParticle->NeedStop = NeedStop;


			// находим перемещение
			float NewSpeed = Speed + vw_Randf0 * SpeedVar;
			if (NewSpeed < 0.0f) NewSpeed = 0.0f;
			NewParticle->Velocity *= NewSpeed ;

			// подключаем частицу к системе
			Attach(NewParticle);
			ParticlesAlive++;

			// уменьшаем необходимое количество частиц
			ParticlesCreated--;
		}
	}


	// если уже ничего нет и нужно выйти - выходим
	if (DestroyIfNoParticles)
		if (ParticlesAlive == 0)
		{
			NeedDestroy = true;
			return false;
		}



	// работа с источником света (если он есть)
	if (Light != 0)
	{
		// просто работаем
		if (!IsSuppressed)
		{
			if (LightNeedDeviation)
			{
				// было выключено, сейчас только включили опять
				if (LightDeviation > 1.0f) LightDeviation = 0.7f;

				if (NextLightDeviation < LightDeviation)
				{
					LightDeviation -= LightDeviationSpeed*TimeDelta;
					if (NextLightDeviation >= LightDeviation)
					{
						LightDeviation = NextLightDeviation;
						LightDeviationSpeed = 3.5f+3.5f*vw_Randf1;
						NextLightDeviation = 0.7+0.3f*vw_Randf1;
					}
				}
				else
				{
					LightDeviation += LightDeviationSpeed*TimeDelta;
					if (NextLightDeviation <= LightDeviation)
					{
						LightDeviation = NextLightDeviation;
						LightDeviationSpeed = 3.5f+3.5f*vw_Randf1;
						NextLightDeviation = 0.7-0.3f*vw_Randf1;
					}
				}
			}
			else
				// было выключено, сейчас только включили опять
				if (LightDeviation > 1.0f) LightDeviation = 1.0f;
		}
		else
		// было включено, сейчас выключили, нужно сделать исчезание
		if (IsSuppressed)
		{
			if (Life != 0.0f) LightDeviation += (25.0f/Life)*TimeDelta;
		}

		if (Light->LinearAttenuationBase != 0.0f)
		{
			Light->LinearAttenuation = Light->LinearAttenuationBase + 2.0f*(LightDeviation-0.5f)*Light->LinearAttenuationBase;
			if (Light->LinearAttenuation < Light->LinearAttenuationBase) Light->LinearAttenuation = Light->LinearAttenuationBase;
		}
		if (Light->QuadraticAttenuationBase != 0.0f)
		{
			Light->QuadraticAttenuation = Light->QuadraticAttenuationBase + 2.0f*(LightDeviation-0.5f)*Light->QuadraticAttenuationBase;
			if (Light->QuadraticAttenuation < Light->QuadraticAttenuationBase) Light->QuadraticAttenuation = Light->QuadraticAttenuationBase;
		}

		// если уже ничего нет, надо выключить свет, если что-то есть то включить
		if (ParticlesAlive == 0) Light->On = false;
		else Light->On = true;
	}


	// находим новые данные AABB

	// предварительная инициализация
	float MinX, MinY, MinZ, MaxX, MaxY, MaxZ;
	tmp = Start;
	if (tmp == 0)
	{
		MinX = MaxX = Location.x;
		MinY = MaxY = Location.y;
		MinZ = MaxZ = Location.z;
	}
	else
	{
		MinX = MaxX = tmp->Location.x;
		MinY = MaxY = tmp->Location.y;
		MinZ = MaxZ = tmp->Location.z;
	}


	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;

		// строим AABB
		if (tmp->Alpha > 0.0f && tmp->Size > 0.0f)
		{
			VECTOR3D v;
			v.x = tmp->Location.x + tmp->Size;
			if (MaxX < v.x) MaxX = v.x;

			v.y = tmp->Location.y + tmp->Size;
			if (MaxY < v.y) MaxY = v.y;

			v.z = tmp->Location.z + tmp->Size;
			if (MaxZ < v.z) MaxZ = v.z;

			v.x = tmp->Location.x - tmp->Size;
			if (MinX > v.x) MinX = v.x;

			v.y = tmp->Location.y - tmp->Size;
			if (MinY > v.y) MinY = v.y;

			v.z = tmp->Location.z - tmp->Size;
			if (MinZ > v.z) MinZ = v.z;
		}

		tmp = tmp2;
	}

	AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
	AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
	AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
	AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
	AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
	AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
	AABB[6] = VECTOR3D(MinX, MinY, MinZ);
	AABB[7] = VECTOR3D(MaxX, MinY, MinZ);



    return true;
}






//-----------------------------------------------------------------------------
// прорисовка системы
//-----------------------------------------------------------------------------
void eParticleSystem::Draw(eTexture **CurrentTexture)
{
	// если не входит в фуструм - рисовать не нужно
	if (!vw_BoxInFrustum(AABB[6], AABB[0])) return;


	// т.к. у нас может быть набор текстур, обходим все текстуры по порядку
	for (int i=0; i<TextureQuantity; i++)
	{

		int DrawCount = 0;
		eParticle *tmp = Start;
		while (tmp!=0)
		{
			eParticle *tmp2 = tmp->Next;
			if (tmp->TextureNum == i) DrawCount++;
			tmp = tmp2;
		}


		// если есть живые - рисуем их
		if (DrawCount > 0)
		{
			if (tmpDATA != 0){delete [] tmpDATA; tmpDATA = 0;}

			GLubyte *tmpDATAub = 0;
			// номер float'а в последовательности
			int k=0;


			// делаем массив для всех элементов
			// RI_3f_XYZ | RI_2f_TEX | RI_4ub_COLOR
			tmpDATA = new float[4*(3+2+1)*DrawCount];
			tmpDATAub = (GLubyte *)tmpDATA;


			GLubyte R,G,B,A;

			// шейдеры не поддерживаются - рисуем по старинке
			if (!ParticleSystemUseGLSL)
			{
				// получаем текущее положение камеры
				VECTOR3D CurrentCameraLocation;
				vw_GetCameraLocation(&CurrentCameraLocation);

				eParticle *tmp = Start;
				while (tmp!=0)
				{
					eParticle *tmp2 = tmp->Next;

					if (tmp->TextureNum != i){tmp = tmp2; continue;}

					// находим вектор камера-точка
					VECTOR3D nnTmp;
					nnTmp = CurrentCameraLocation - tmp->Location;
					//nnTmp.Normalize();// - это тут не нужно, нам нужны только пропорции

					// находим перпендикуляр к вектору nnTmp
					VECTOR3D nnTmp2;
					nnTmp2.x = 1.0f;
					nnTmp2.y = 1.0f;
					nnTmp2.z = -(nnTmp.x + nnTmp.y)/nnTmp.z;
					nnTmp2.Normalize();

					// находим перпендикуляр к векторам nnTmp и nnTmp2
					// файтически - a x b = ( aybz - byaz , azbx - bzax , axby - bxay );
					VECTOR3D nnTmp3;
					nnTmp3.x = nnTmp.y*nnTmp2.z - nnTmp2.y*nnTmp.z;
					nnTmp3.y = nnTmp.z*nnTmp2.x - nnTmp2.z*nnTmp.x;
					nnTmp3.z = nnTmp.x*nnTmp2.y - nnTmp2.x*nnTmp.y;
					nnTmp3.Normalize();

					// находим
					VECTOR3D tmpAngle1,tmpAngle2,tmpAngle3,tmpAngle4;

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

					tmp = tmp2;
				}
			}
			else // иначе работаем с шейдерами, в них правильно развернем билборд
			{
				eParticle *tmp = Start;
				while (tmp!=0)
				{
					eParticle *tmp2 = tmp->Next;

					if (tmp->TextureNum != i){tmp = tmp2; continue;}

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

					tmp = tmp2;
				}
			}
		}


		if (DrawCount > 0)
		{

			if (*CurrentTexture != Texture[i])
			{
				vw_SetTexture(0, Texture[i]);
				*CurrentTexture = Texture[i];
			}


			if (BlendType == 1) vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

			vw_SendVertices(RI_QUADS, 4*DrawCount, RI_3f_XYZ | RI_4ub_COLOR | RI_1_TEX, tmpDATA, 6*sizeof(float));

			if (BlendType != 0) vw_SetTextureBlend(true, RI_BLEND_SRCALPHA, RI_BLEND_ONE);

		}
	}
}





//-----------------------------------------------------------------------------
// поставить правильный полет частиц, т.е. учет внешней скорости
//-----------------------------------------------------------------------------
void eParticleSystem::SetStartLocation(VECTOR3D NewLocation)
{
	PrevLocation = NewLocation;
	Location = NewLocation;

	if (Light != 0) Light->SetLocation(Location);
}



//-----------------------------------------------------------------------------
// перемещение всех частиц и центра
//-----------------------------------------------------------------------------
void eParticleSystem::MoveSystem(VECTOR3D NewLocation)
{
	// чтобы не сбивать PrevLocation
	if (Location == NewLocation) return;
	PrevLocation = Location;
	Location = NewLocation;

	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		// меняем каждой частице
		tmp->Location += NewLocation-PrevLocation;
		tmp = tmp2;
	}


	if (Light != 0) Light->SetLocation(Location);
}


//-----------------------------------------------------------------------------
// перемещение центра
//-----------------------------------------------------------------------------
void eParticleSystem::MoveSystemLocation(VECTOR3D NewLocation)
{
	// чтобы не сбивать PrevLocation
	if (Location == NewLocation) return;
	PrevLocation = Location;
	Location = NewLocation;

	if (Light != 0) Light->SetLocation(Location);
}


//-----------------------------------------------------------------------------
// установка направления "движения" системы
//-----------------------------------------------------------------------------
void eParticleSystem::RotateSystemByAngle(VECTOR3D NewAngle)
{
	Angle = NewAngle;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9*sizeof(float));
	// делаем инверсную старую матрицу
	Matrix33InverseRotate(OldInvRotationMat);
	Matrix33CreateRotate(CurrentRotationMat, Angle);

	Matrix33CalcPoint(&Direction, OldInvRotationMat);
	Matrix33CalcPoint(&Direction, CurrentRotationMat);
}



//-----------------------------------------------------------------------------
// установка направления "движения" системы + перенос всех частиц
//-----------------------------------------------------------------------------
void eParticleSystem::RotateSystemAndParticlesByAngle(VECTOR3D NewAngle)
{
	Angle = NewAngle;

	// сохраняем старые значения + пересчет новых
	memcpy(OldInvRotationMat, CurrentRotationMat, 9*sizeof(float));
	// делаем инверсную старую матрицу
	Matrix33InverseRotate(OldInvRotationMat);
	Matrix33CreateRotate(CurrentRotationMat, Angle);

	Matrix33CalcPoint(&Direction, OldInvRotationMat);
	Matrix33CalcPoint(&Direction, CurrentRotationMat);


	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		// меняем каждой частице
		VECTOR3D TMP = tmp->Location - Location;
		Matrix33CalcPoint(&TMP, OldInvRotationMat);
		Matrix33CalcPoint(&TMP, CurrentRotationMat);
		tmp->Location = TMP + Location;
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// поворот частиц на угол без поворота системы
//-----------------------------------------------------------------------------
void eParticleSystem::RotateParticlesByAngle(VECTOR3D NewAngle)
{
	// делаем обратку для отматывания назад
	float TmpOldInvRotationMat[9];
	memcpy(TmpOldInvRotationMat, CurrentRotationMat, 9*sizeof(float));
	Matrix33InverseRotate(TmpOldInvRotationMat);

	// матрица поворота частиц
	float TmpRotationMat[9];
	Matrix33CreateRotate(TmpRotationMat, NewAngle);


	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		// меняем каждой частице
		VECTOR3D TMP = tmp->Location - Location;
		Matrix33CalcPoint(&TMP, TmpOldInvRotationMat);
		// поворачиваем каждую точку
		Matrix33CalcPoint(&TMP, TmpRotationMat);
		Matrix33CalcPoint(&TMP, CurrentRotationMat);
		tmp->Location = TMP + Location;
		tmp = tmp2;
	}

}





//-----------------------------------------------------------------------------
// остановить все частицы в системе
//-----------------------------------------------------------------------------
void eParticleSystem::StopAllParticles()
{
	Speed = 0.0f;
	IsAttractive = false;

	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		// меняем каждой частице
		tmp->Velocity = VECTOR3D(0.0f,0.0f,0.0f);
		tmp = tmp2;
	}
}




//-----------------------------------------------------------------------------
// поставить правильный полет частиц, т.е. учет внешней скорости
//-----------------------------------------------------------------------------
void eParticleSystem::ChangeSpeed(VECTOR3D Vel)
{
	eParticle *tmp = Start;
	while (tmp!=0)
	{
		eParticle *tmp2 = tmp->Next;
		// меняем каждой частице
		tmp->Velocity += Vel;
		tmp = tmp2;
	}
}
