/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: ParticleSystem2D.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "ParticleSystem2D.h"
#include "../RendererInterface/RendererInterface.h"





//-----------------------------------------------------------------------------
// инициализация класса
//-----------------------------------------------------------------------------
eParticleSystem2D::eParticleSystem2D()
{

	Age = 0.0f;
	TimeLastUpdate = -1.0f;
	EmissionResidue =  0.0f;
	Angle = Direction = VECTOR3D(0,0,0);
	Matrix33Identity(RotationMatrix);
	Texture = 0;
	AttractiveValue = 25.0f;
	IsSuppressed = false;
	DestroyIfNoParticles = false;

	// положение системы
	Location = VECTOR3D( 0.0f, 0.0f, 0.0f);

	DeadZone = 0.0f; // нет мертвой зоны


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

	SizeStart  = 1.00f;
	SizeVar    = 0.00f;
	SizeEnd    = 1.00f;

	Speed      = 1.00f;
	SpeedOnCreation	   = 1.00f;
	SpeedVar   = 1.00f;
	Theta      = 1.00f;

	Life       = 1.00f;
	LifeVar    = 0.00f;

	ParticlesPerSec = 100;
	IsAttractive = 0;

	CreationType = 0;
	CreationSize = VECTOR3D(0.05f,0.05f,0.05f);

	// настройка массива
	Start = 0;
	End = 0;

}


//-----------------------------------------------------------------------------
//	При разрушении класса
//-----------------------------------------------------------------------------
eParticleSystem2D::~eParticleSystem2D()
{
	// полностью освобождаем память от всех частиц в системе
	eParticle2D *tmp = Start;
	while (tmp!=0)
	{
		eParticle2D *tmp2 = tmp->Next;
		Detach(tmp);
		delete tmp; tmp = 0;
		tmp = tmp2;
	}
}


//-----------------------------------------------------------------------------
//	подключить частицу к системе
//-----------------------------------------------------------------------------
void eParticleSystem2D::Attach(eParticle2D * NewParticle)
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
void eParticleSystem2D::Detach(eParticle2D * OldParticle)
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
bool eParticleSystem2D::Update(float Time)
{
	// первый раз... просто берем время
	if (TimeLastUpdate == -1.0f) {TimeLastUpdate = Time;return true;}

	// Time - это абсолютное время, вычисляем дельту
	float TimeDelta = Time - TimeLastUpdate;
	// быстро вызвали еще раз... время не изменилось, или почти не изменилось
	if (TimeDelta == 0.0f) return true;

	TimeLastUpdate = Time;


	// очищаем счетчик подсчета кол-ва действующий частиц
	int ParticlesAlive = 0;


	// для всех частиц
	eParticle2D *tmp = Start;

	while (tmp!=0)
	{
		eParticle2D *tmp2 = tmp->Next;
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
	float ParticlesNeeded = ParticlesPerSec * TimeDelta + EmissionResidue;

	// переводим в целочисленные значения
	unsigned int ParticlesCreated = (unsigned int)ParticlesNeeded;

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
			eParticle2D *NewParticle = 0;
			NewParticle = new eParticle2D; if (NewParticle == 0) return true;

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


			// выпускаем частицу возле места нахождения системы
			if (CreationType == 0) // точка
				NewParticle->Location = Location + VECTOR3D(vw_Randf0 * CreationSize.x, vw_Randf0 * CreationSize.y, vw_Randf0 * CreationSize.z);
			if (CreationType == 1)
			{
				// в квадрате
				VECTOR3D tmp;
				float minDist;
				if (DeadZone != 0.0f)
				{
					minDist = CreationSize.x*CreationSize.x+CreationSize.y*CreationSize.y+CreationSize.z*CreationSize.z;
					// если зона больше чем радиус излучения - выключаем ее
					if (minDist <= DeadZone*DeadZone) DeadZone = 0.0f;
				}

				tmp.x = (1.0f-vw_Randf1*2) * CreationSize.x;
				tmp.y = (1.0f-vw_Randf1*2) * CreationSize.y;
				tmp.z = (1.0f-vw_Randf1*2) * CreationSize.z;
				while (tmp.x*tmp.x + tmp.y*tmp.y + tmp.z*tmp.z < DeadZone*DeadZone)
				{
					// ув. радиус
					VECTOR3D tmp1 = tmp;
					tmp1.Normalize();
					tmp1 = tmp1^(1/100.0f);
					tmp = tmp + tmp1;
				}

				NewParticle->Location = Location + tmp;
			}
			if (CreationType == 2)
			{
				// в окружности
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
				360.00f != Theta)
			{
				if (Theta == 0.0f)
				{
					NewParticle->Velocity = Direction;
				}
				else
				{
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

					Matrix33CalcPoint(&NewParticle->Velocity, RotationMatrix);
				}
			}
			else
			{
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


	// проверка, если уже ничего нет и нужно выйти - выходим
	if (DestroyIfNoParticles)
		if (ParticlesAlive == 0) return false;

    return true;
}





//-----------------------------------------------------------------------------
// прорисовка системы
//-----------------------------------------------------------------------------
void eParticleSystem2D::Draw()
{

	// загрузка текстуры, уже должна быть подключена
	if (Texture == 0) return;

	RECT SrcRest, DestRest;

	SetRect(&SrcRest,0,0,64,64);



	// подсчет кол-ва частиц
	eParticle2D *tmp1 = Start;
	int CurrentCount = 0;

	while (tmp1!=0)
	{
		eParticle2D *tmp2 = tmp1->Next;
		CurrentCount++;
		tmp1 = tmp2;
	}

	if (CurrentCount == 0) return;


	float AW;
	float AH;
	bool ASpresent=false;
	ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(0, 0, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_SetTextureV(0, Texture);
	vw_SetTexAlpha(true, 0.01f);
	vw_SetTexAddressMode(0, RI_CLAMP);
	vw_SetTexBlend(RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

    // не можем ставить другое! если нет мипмапа
    vw_SetTexFiltering(0, RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE, 1);

	float ImageHeight = Texture->Height*1.0f;
	float ImageWidth = Texture->Width*1.0f;

	float tmpPosY = 0;

	// выделяем память
	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX | RI_4f_COLOR
	float *tmp = 0;
	tmp = new float[(2+2+4)*4*CurrentCount]; if (tmp == 0) return;
	int k=0;





	// для всех частиц
	tmp1 = Start;

	while (tmp1!=0)
	{
		eParticle2D *tmp2 = tmp1->Next;

		SetRect(&DestRest,(int)(tmp1->Location.x - tmp1->Size/2),
			(int)(tmp1->Location.y - tmp1->Size/2),
			(int)(tmp1->Location.x + tmp1->Size/2),
			(int)(tmp1->Location.y + tmp1->Size/2));

			if (ASpresent) tmpPosY = (AH - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));
			else tmpPosY = (AHw - DestRest.top - DestRest.top - (DestRest.bottom - DestRest.top));

			float FrameHeight = (SrcRest.bottom*1.0f )/ImageHeight;
			float FrameWidth = (SrcRest.right*1.0f )/ImageWidth;

			float Yst = (SrcRest.top*1.0f)/ImageHeight;
			float Xst = (SrcRest.left*1.0f)/ImageWidth;

				tmp[k++] = DestRest.left;
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);
				tmp[k++] = tmp1->Color.r;
				tmp[k++] = tmp1->Color.g;
				tmp[k++] = tmp1->Color.b;
				tmp[k++] = tmp1->Alpha;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-Yst;

				tmp[k++] = DestRest.left;
				tmp[k++] = DestRest.top +tmpPosY;
				tmp[k++] = tmp1->Color.r;
				tmp[k++] = tmp1->Color.g;
				tmp[k++] = tmp1->Color.b;
				tmp[k++] = tmp1->Alpha;
				tmp[k++] = Xst;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);
				tmp[k++] = DestRest.top +tmpPosY;
				tmp[k++] = tmp1->Color.r;
				tmp[k++] = tmp1->Color.g;
				tmp[k++] = tmp1->Color.b;
				tmp[k++] = tmp1->Alpha;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-FrameHeight;

				tmp[k++] = DestRest.left + (DestRest.right - DestRest.left);
				tmp[k++] = DestRest.top +tmpPosY +(DestRest.bottom - DestRest.top);
				tmp[k++] = tmp1->Color.r;
				tmp[k++] = tmp1->Color.g;
				tmp[k++] = tmp1->Color.b;
				tmp[k++] = tmp1->Alpha;
				tmp[k++] = FrameWidth;
				tmp[k++] = 1.0f-Yst;

		tmp1 = tmp2;
	}


	vw_SendVertices(RI_QUADS, 4*CurrentCount, RI_2f_XY | RI_1_TEX | RI_4f_COLOR, tmp, 8*sizeof(float));

	if (tmp != 0){delete [] tmp; tmp = 0;}
	vw_SetTexAlpha(false, 0.5f);
    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_SetTextureDef(0);

}


//-----------------------------------------------------------------------------
// перемещение всех частиц и центра
//-----------------------------------------------------------------------------
void eParticleSystem2D::MoveSystem(VECTOR3D NewLocation)
{
	VECTOR3D PrevLocation = Location;
	Location = NewLocation;


	eParticle2D *tmp = Start;
	while (tmp!=0)
	{
		eParticle2D *tmp2 = tmp->Next;
		// меняем каждой частице
		tmp->Location += NewLocation-PrevLocation;
		tmp = tmp2;
	}
}


//-----------------------------------------------------------------------------
// перемещение центра
//-----------------------------------------------------------------------------
void eParticleSystem2D::MoveSystemLocation(VECTOR3D NewLocation)
{
	Location = NewLocation;
}



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void eParticleSystem2D::SetRotation(VECTOR3D NewAngle)
{
	Matrix33CreateRotate(RotationMatrix, Angle^-1);
	Matrix33CreateRotate(RotationMatrix, NewAngle);
	Angle = NewAngle;
}












