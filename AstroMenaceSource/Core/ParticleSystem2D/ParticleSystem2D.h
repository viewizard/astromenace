/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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

	// возраст системы в секундах
	float Age;

	// последнее время обновления системы
	float TimeLastUpdate;

	// остаток создания частицы (если к примеру 0.5 частиц в секунду стоит)
	float EmissionResidue;

	// подвязка к динамическому массиву
	eParticle2D * Start;
	eParticle2D * End;
	int ParticlesCount;
	void Attach(eParticle2D * NewParticle);
	void Detach(eParticle2D * OldParticle);


};








#endif //PARTICLESYSTEM2D_H
