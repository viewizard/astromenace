/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: ParticleSystem2D.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#ifndef PARTICLESYSTEM2D_H
#define PARTICLESYSTEM2D_H


#include "Particle2D.h"
#include "../Texture/Texture.h"




class eParticleSystem2D
{
public:
	eParticleSystem2D();
	~eParticleSystem2D();

	// обновить все частицы в этой системе, по времени
	bool Update(float Time);
	// прорисовка всех частиц
	void Draw();
	// базовая текстура частиц
	eTexture	*Texture;

	// передвинуть все частици на указаное значение
	void		MoveSystem(VECTOR3D NewLocation);
	// сдвинуть только центр системы
	void		MoveSystemLocation(VECTOR3D NewLocation);


	// кол-во создаваемых частиц в секунду
	unsigned int	ParticlesPerSec;

	// текущее положение частиц в пространстве
	VECTOR3D		Location;
	// текущее направление системы (используется для создания частиц+некоторые вариации)
	VECTOR3D		Direction;
	// угол поворота системы
	VECTOR3D		Angle;
	void 			SetRotation(VECTOR3D NewAngle);

	// текущая матрица вращения
	float 	RotationMatrix[9];

	// размер частиц в мировых координатах
	float SizeStart;
	float SizeVar;
	float SizeEnd;

	// Прозрачность частиц. Alpha 0 = невидем, Alpha 1 = видемость 100%
	float AlphaStart;
	float AlphaVar;
	float AlphaEnd;

	// Цвет частиц при старте и завершении
	// линейно интерполируется
	COLORVALUE2D	ColorStart;
	COLORVALUE2D	ColorVar;
	COLORVALUE2D	ColorEnd;

	// Скалярная скорость, с вектором направления получаем вектор движения
	float Speed;
	float SpeedVar;
	// Скорость при создании, сохраняем, если будут менять скорость в процессе
	float SpeedOnCreation;


	// тип создания... 0-точка, 1-квадрат, 2-окружность
	int			CreationType;
	VECTOR3D	CreationSize;


	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone;


	// жизнь частицы в секундах
	float Life;
	float LifeVar;

	// показывает, насколько отличным будет выбор направления у создаваемой частицы
	// с направлением системы
	float Theta;

	// система притягивает частицы или нет
	bool IsAttractive;
	float AttractiveValue;

	// можем ли мы создавать частицы или нет
	bool IsSuppressed;
	bool DestroyIfNoParticles;

	// возраст системы в секундах
	float Age;

	// последнее время обновления системы
	float TimeLastUpdate;

	// остаток создания частицы (если к примеру 0.5 частиц в секунду стоит)
	float EmissionResidue;

	// подвязка к динамическому массиву
	eParticle2D * Start;
	eParticle2D * End;
	void Attach(eParticle2D * NewParticle);
	void Detach(eParticle2D * OldParticle);


};








#endif //PARTICLESYSTEM2D_H
