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
	bool		NeedDestroy;

	// обновить все частицы в этой системе, по времени
	bool Update(float Time);
	// прорисовка всех частиц
	void Draw(eTexture **CurrentTexture);
	// базовая текстура частиц
	eTexture	*Texture[5];
	int			TextureQuantity;
	// последовательность прорисовки
	float 		*tmpDATA;

	// тип смешивания
	int			BlendType;

	// начальный установки, или установки где пред. и текушее положения равны
	void		SetStartLocation(VECTOR3D NewLocation);
	// передвинуть все частици на указаное значение
	void		MoveSystem(VECTOR3D NewLocation);
	// сдвинуть только центр системы
	void		MoveSystemLocation(VECTOR3D NewLocation);
	// развернуть направление системы
	void		RotateSystemByAngle(VECTOR3D NewDirection);
	void		RotateSystemAndParticlesByAngle(VECTOR3D NewAngle);
	// разворот только частиц
	void		RotateParticlesByAngle(VECTOR3D NewAngle);

	// остановить все частицы в системе
	void		StopAllParticles();

	// поставить правильный полет частиц, т.е. учет внешней скорости
	void		ChangeSpeed(VECTOR3D Vel);


	// текущее направление системы (используется для создания частиц+некоторые вариации)
	VECTOR3D		Direction;
	// угол поворота системы
	VECTOR3D		Angle;


	// кол-во создаваемых частиц в секунду
	int		ParticlesPerSec;
	// сколько создавать
	int ParticlesCreated;

	// если нужно замедлять и остановить
	bool	NeedStop;

	// размер частиц в мировых координатах
	float SizeStart;
	float SizeVar;
	float SizeEnd;

	// Прозрачность частиц. Alpha 0 = невидем, Alpha 1 = видемость 100%
	float AlphaStart;
	float AlphaVar;
	float AlphaEnd;
	bool  AlphaShowHide; // если нужно сначало ув. потом ум.

	// Цвет частиц при старте и завершении
	// линейно интерполируется
	COLORVALUE3D	ColorStart;
	COLORVALUE3D	ColorVar;
	COLORVALUE3D	ColorEnd;

	// Скалярная скорость, с вектором направления получаем вектор движения
	float Speed;
	float SpeedVar;
	// Скорость при создании, сохраняем, если будут менять скорость в процессе
	float SpeedOnCreation;


	// тип создания... 0-точка, 1-куб, 2-сфера
	int			CreationType;
	VECTOR3D	CreationSize;


	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone;
	// коэффициент уменьшения от расстояния (чем ближе к камере - тем меньше)
	float	Resize; // если 0, то у самой камеры будет нулевой, у крайней точки системы - Size


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

	// для проверки вхождения в видимую область
	VECTOR3D AABB[8];


	// подвязка к динамическому массиву
	eParticle * Start;
	eParticle * End;
	int		ParticlesCount;
	void Attach(eParticle * NewParticle);
	void Detach(eParticle * OldParticle);
	// указатели на цепь систем, для менеджера
	eParticleSystem *Next;
	eParticleSystem *Prev;


	// источник света, если он нужен
	eLight *Light;
	bool LightNeedDeviation;
	float LightDeviation;
	float NextLightDeviation;
	float LightDeviationSpeed;

	// доступ к private данным
	void GetLocation(VECTOR3D *Vec)
	{
		*Vec = Location;
	};

private:


	// последнее положение системы (для интерполяции)
	VECTOR3D		PrevLocation;
	// текущее положение частиц в пространстве
	VECTOR3D		Location;

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
