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

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "particle.h"
#include "../light/light.h"
#include "../texture/texture.h"


class eParticleSystem
{
public:
	eParticleSystem();
	~eParticleSystem();

	// нужно удалить
	bool	NeedDestroy{false};

	// обновить все частицы в этой системе, по времени
	bool	Update(float Time);
	// прорисовка всех частиц
	void	Draw(eTexture **CurrentTexture);
	// базовая текстура частиц
	eTexture	*Texture[5]{nullptr, nullptr, nullptr, nullptr, nullptr};
	int	TextureQuantity{1};
	// последовательность прорисовки
	float 	*tmpDATA{nullptr};

	// тип смешивания
	int	BlendType{0};

	// начальный установки, или установки где пред. и текушее положения равны
	void	SetStartLocation(VECTOR3D NewLocation);
	// передвинуть все частици на указаное значение
	void	MoveSystem(VECTOR3D NewLocation);
	// сдвинуть только центр системы
	void	MoveSystemLocation(VECTOR3D NewLocation);
	// развернуть направление системы
	void	RotateSystemByAngle(VECTOR3D NewDirection);
	void	RotateSystemAndParticlesByAngle(VECTOR3D NewAngle);
	// разворот только частиц
	void	RotateParticlesByAngle(VECTOR3D NewAngle);

	// остановить все частицы в системе
	void	StopAllParticles();

	// поставить правильный полет частиц, т.е. учет внешней скорости
	void	ChangeSpeed(VECTOR3D Vel);

	// текущее направление системы (используется для создания частиц+некоторые вариации)
	VECTOR3D	Direction{0, 0, 0};
	// угол поворота системы
	VECTOR3D	Angle{0, 0, 0};

	// кол-во создаваемых частиц в секунду
	int	ParticlesPerSec{100};
	// сколько создавать
	int	ParticlesCreated{0};

	// если нужно замедлять и остановить
	bool	NeedStop{false};

	// размер частиц в мировых координатах
	float	SizeStart{1.0f};
	float	SizeVar{0.0f};
	float	SizeEnd{1.0f};

	// Прозрачность частиц. Alpha 0 = невидем, Alpha 1 = видемость 100%
	float	AlphaStart{1.0f};
	float	AlphaVar{0.0f};
	float	AlphaEnd{1.0f};
	bool	AlphaShowHide{false}; // если нужно сначало ув. потом ум.

	// Цвет частиц при старте и завершении
	// линейно интерполируется
	COLORVALUE3D	ColorStart{1.0f, 1.0f, 1.0f, 1.0f};
	COLORVALUE3D	ColorVar{0.0f, 0.0f, 0.0f, 0.0f};
	COLORVALUE3D	ColorEnd{1.0f, 1.0f, 1.0f, 1.0f};

	// Скалярная скорость, с вектором направления получаем вектор движения
	float	Speed{1.0f};
	float	SpeedVar{1.0f};
	// Скорость при создании, сохраняем, если будут менять скорость в процессе
	float	SpeedOnCreation{1.0f};

	// тип создания... 0-точка, 1-куб, 2-сфера
	int		CreationType{0};
	VECTOR3D	CreationSize{0.05f, 0.05f, 0.05f};

	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone{0.0f};
	// коэффициент уменьшения от расстояния (чем ближе к камере - тем меньше)
	float	Resize{1.0f}; // если 0, то у самой камеры будет нулевой, у крайней точки системы - Size


	// жизнь частицы в секундах
	float	Life{1.0f};
	float	LifeVar{0.0f};

	// показывает, насколько отличным будет выбор направления у создаваемой частицы
	// с направлением системы
	float	Theta{1.0f};

	// система притягивает частицы или нет
	bool	IsAttractive{0};
	float	AttractiveValue{25.0f};

	// можем ли мы создавать частицы или нет
	bool	IsSuppressed{false};
	bool	DestroyIfNoParticles{false};

	// возраст системы в секундах
	float	Age{0.0f};

	// последнее время обновления системы
	float	TimeLastUpdate{-1.0f};

	// остаток создания частицы (если к примеру 0.5 частиц в секунду стоит)
	float	EmissionResidue{0.0f};

	// для проверки вхождения в видимую область
	// начальныя установка коробок
	// ставим тут очень большие параметры, вне зоны прорисовки, т.к. в партиклах на функциях перемещения
	// не учитываем изменение коробки, и координаты в коробках у нас абсолютные (!!!)
	// использовать относительные координаты бокса нельзя, т.к. можем преносить только центр, а можем
	// и центр, и все частицы...
	VECTOR3D AABB[8]{{-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f},
			 {-1000000.0f, 1000000.0f, -1000000.0f}};

	// подвязка к динамическому массиву
	eParticle	*Start{nullptr};
	eParticle	*End{nullptr};
	int		ParticlesCount{0};
	void Attach(eParticle *NewParticle);
	void Detach(eParticle *OldParticle);
	// указатели на цепь систем, для менеджера
	eParticleSystem	*Next{nullptr};
	eParticleSystem	*Prev{nullptr};

	// источник света, если он нужен
	eLight *Light{nullptr};
	bool LightNeedDeviation{false};
	float LightDeviation{100.0f};
	float NextLightDeviation{0.7f+0.3f*vw_Randf1};
	float LightDeviationSpeed{3.5f+3.5f*vw_Randf1};

	// доступ к private данным
	void GetLocation(VECTOR3D *Vec)
	{
		if (Vec != nullptr)
			*Vec = Location;
	};

private:
	// последнее положение системы (для интерполяции)
	VECTOR3D	PrevLocation{0.0f, 0.0f, 0.0f};
	// текущее положение частиц в пространстве
	VECTOR3D	Location{0.0f, 0.0f, 0.0f};

	// матрицы поворота, для оптимизации просчетов
	float	CurrentRotationMat[9];
	float	OldInvRotationMat[9];
};


//-----------------------------------------------------------------------------
// Менеджер CParticleSystem
//-----------------------------------------------------------------------------

// Инициализация менеджера частиц
void	vw_InitParticleSystems(bool UseGLSL, float Quality);
// Включаем в список
void	vw_AttachParticleSystem(eParticleSystem * NewParticleSystem);
// Исключаем из списка
void	vw_DetachParticleSystem(eParticleSystem * OldParticleSystem);
// Удаляем все объекты в списке
void	vw_ReleaseAllParticleSystems();
// Прорисовываем все системы
void	vw_DrawAllParticleSystems();
//	Прорисовываем блок ParticleSystems
void	vw_DrawParticleSystems(eParticleSystem **DrawParticleSystem, int Quantity);
// Проверяем все объекты, обновляем данные
void	vw_UpdateAllParticleSystems(float Time);

#endif //PARTICLESYSTEM_H
