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

#ifndef PARTICLESYSTEM2D_H
#define PARTICLESYSTEM2D_H

#include "particle2d.h"
#include "../texture/texture.h"


class eParticleSystem2D
{
public:
	eParticleSystem2D() {};
	~eParticleSystem2D();

	// обновить все частицы в этой системе, по времени
	bool	Update(float Time);
	// прорисовка всех частиц
	void	Draw();
	// базовая текстура частиц
	eTexture	*Texture{nullptr};

	// передвинуть все частици на указаное значение
	void		MoveSystem(VECTOR3D NewLocation);
	// сдвинуть только центр системы
	void		MoveSystemLocation(VECTOR3D NewLocation);

	// кол-во создаваемых частиц в секунду
	unsigned int	ParticlesPerSec{100};

	// текущее положение частиц в пространстве
	VECTOR3D	Location{0.0f, 0.0f, 0.0f};
	// текущее направление системы (используется для создания частиц+некоторые вариации)
	VECTOR3D	Direction{0.0f, 0.0f, 0.0f};
	// угол поворота системы
	VECTOR3D	Angle{0.0f, 0.0f, 0.0f};
	void 		SetRotation(VECTOR3D NewAngle);

	// текущая матрица вращения
	float	RotationMatrix[9]{1.0f, 0.0f, 0.0f,
				  0.0f, 1.0f, 0.0f,
				  0.0f, 0.0f, 1.0f};

	// размер частиц в мировых координатах
	float	SizeStart{1.0f};
	float	SizeVar{0.0f};
	float	SizeEnd{1.0f};

	// Прозрачность частиц. Alpha 0 = невидем, Alpha 1 = видемость 100%
	float	AlphaStart{1.0f};
	float	AlphaVar{0.0f};
	float	AlphaEnd{1.0f};

	// Цвет частиц при старте и завершении
	// линейно интерполируется
	COLORVALUE2D	ColorStart{1.0f, 1.0f, 1.0f, 1.0f};
	COLORVALUE2D	ColorVar{0.0f, 0.0f, 0.0f, 0.0f};
	COLORVALUE2D	ColorEnd{1.0f, 1.0f, 1.0f, 1.0f};

	// Скалярная скорость, с вектором направления получаем вектор движения
	float	Speed{1.0f};
	float	SpeedVar{1.0f};
	// Скорость при создании, сохраняем, если будут менять скорость в процессе
	float	SpeedOnCreation{1.0f};

	// тип создания... 0-точка, 1-квадрат, 2-окружность
	int		CreationType{0};
	VECTOR3D	CreationSize{0.05f,0.05f,0.05f};

	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone{0.0f};

	// жизнь частицы в секундах
	float	Life{1.0f};
	float	LifeVar{0.0f};

	// показывает, насколько отличным будет выбор направления у создаваемой частицы
	// с направлением системы
	float	Theta{1.0f};

	// система притягивает частицы или нет
	bool	IsAttractive{false};
	float	AttractiveValue{25.0f};

	// можем ли мы создавать частицы или нет
	bool	IsSuppressed{false};

	// возраст системы в секундах
	float	Age{0.0f};

	// последнее время обновления системы
	float	TimeLastUpdate{-1.0f};

	// остаток создания частицы (если к примеру 0.5 частиц в секунду стоит)
	float	EmissionResidue{0.0f};

	// подвязка к динамическому массиву
	eParticle2D	*Start{nullptr};
	eParticle2D	*End{nullptr};
	int	ParticlesCount{0};
	void	Attach(eParticle2D *NewParticle);
	void	Detach(eParticle2D *OldParticle);
};

#endif //PARTICLESYSTEM2D_H
