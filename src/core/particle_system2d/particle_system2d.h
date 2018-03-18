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

#ifndef PARTICLESYSTEM2D_H
#define PARTICLESYSTEM2D_H

#include "../base.h"

struct sCOLORVALUE2D {
	float r;
	float g;
	float b;
	float a;
};

struct sParticle2D {
	// обновление информации в частице
	bool Update(float TimeDelta, sVECTOR3D ParentLocation = sVECTOR3D(0.0f,0.0f,0.0f),
		    bool Attractive = false, float AttractiveValue = 25.0f);

	// текущее место расположения частицы
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	// текущая скорость частицы
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	// текущий цвет частицы
	sCOLORVALUE2D Color{1.0f, 0.0f, 0.0f, 0.5f};
	// значение приращение цвета
	sCOLORVALUE2D ColorDelta{0.0f, 0.0f, 0.0f, 0.0f};

	float Age{0.0f};	// время жизни частицы в секундах
	float Lifetime{0.0f};	// оставщееся время жизни частицы

	float Size{1.0f};	// размер частицы
	float SizeDelta{0.0f};	// значение изменения размера

	float Alpha{1.0f};	// прозрачность
	bool NeedStop{false};	// если нужно замедлять и остановить
};

struct sParticleSystem2D
{
	bool Update(float Time);
	void Draw();
	void SetRotation(sVECTOR3D NewAngle);
	// передвинуть все частици на указаное значение
	void MoveSystem(sVECTOR3D NewLocation);
	// сдвинуть только центр системы
	void MoveSystemLocation(sVECTOR3D NewLocation);

	// кол-во создаваемых частиц в секунду
	unsigned int ParticlesPerSec{100};

	// базовая текстура частиц
	sTexture *Texture{nullptr};

	sVECTOR3D Location{0.0f, 0.0f, 0.0f};	// текущее положение частиц в пространстве
	sVECTOR3D Direction{0.0f, 0.0f, 0.0f};	// текущее направление системы (используется для создания частиц+некоторые вариации)
	sVECTOR3D Angle{0.0f, 0.0f, 0.0f};	// угол поворота системы

	// текущая матрица вращения
	float RotationMatrix[9]{1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f};

	float SizeStart{1.0f};		// размер частиц в мировых координатах
	float SizeVar{0.0f};
	float SizeEnd{1.0f};

	float AlphaStart{1.0f};		// Прозрачность частиц. Alpha 0 = невидем, Alpha 1 = видемость 100%
	float AlphaVar{0.0f};
	float AlphaEnd{1.0f};

	sCOLORVALUE2D ColorStart{1.0f, 1.0f, 1.0f, 1.0f}; // Цвет частиц при старте и завершении линейно интерполируется
	sCOLORVALUE2D ColorVar{0.0f, 0.0f, 0.0f, 0.0f};
	sCOLORVALUE2D ColorEnd{1.0f, 1.0f, 1.0f, 1.0f};

	float Speed{1.0f};		// Скалярная скорость, с вектором направления получаем вектор движения
	float SpeedVar{1.0f};
	float SpeedOnCreation{1.0f};	// Скорость при создании, сохраняем, если будут менять скорость в процессе

	int CreationType{0};		// тип создания... 0-точка, 1-квадрат, 2-окружность
	sVECTOR3D CreationSize{0.05f, 0.05f, 0.05f};

	float Life{1.0f};		// жизнь частицы в секундах
	float LifeVar{0.0f};

	bool IsAttractive{false};	// система притягивает частицы или нет
	float AttractiveValue{25.0f};	// если притягивает, с каким коэф.

	float DeadZone{0.0f};		// мертвая зона (радиус, где вообще не рисуем)
	float Theta{1.0f};		// показывает, насколько отличным будет выбор направления у создаваемой частицы с направлением системы
	bool IsSuppressed{false};	// можем ли мы создавать частицы или нет
	float Age{0.0f};		// возраст системы в секундах
	float TimeLastUpdate{-1.0f};	// последнее время обновления системы
	float EmissionResidue{0.0f};	// остаток создания частицы (если к примеру 0.5 частиц в секунду стоит)

	std::list<sParticle2D> ParticlesList;
};

#endif //PARTICLESYSTEM2D_H
