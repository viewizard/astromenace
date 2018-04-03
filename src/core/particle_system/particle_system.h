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

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "../base.h"

struct sVECTOR3D;
struct sLight;

enum class eParticleCreationType {
	Point,
	Cube,
	Tube,	// we need this for maser/laser
	Sphere
};

class cParticle {
	friend class cParticleSystem;

private:
	// обновление информации в частице
	bool Update(float TimeDelta, const sVECTOR3D &ParentLocation = sVECTOR3D{0.0f, 0.0f, 0.0f},
		    bool Magnet = false, float MagnetFactor = 25.0f);

	// текущее место расположения частицы
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	// текущая скорость частицы
	sVECTOR3D Velocity{0.0f, 0.0f, 0.0f};

	// если нужно замедлять и остановить
	bool NeedStop{false};

	// текущий цвет частицы
	sRGBCOLOR Color{1.0f, 0.0f, 0.0f};
	// значение приращение цвета
	sRGBCOLOR ColorDelta{0.0f, 0.0f, 0.0f};

	// время жизни частицы в секундах
	float Age{0.0f};

	// оставщееся время жизни частицы
	float Lifetime{0.0f};

	// размер частицы
	float Size{1.0f};
	// значение изменения размера
	float SizeDelta{0.0f};

	// прозрачность
	float Alpha{1.0f};
	// изменение прозрачности
	float AlphaDelta{0.0f};

	// сначало ув. альфу, потом уменьшаем
	bool AlphaShowHide{false};
	// какой цикл - затухаем, или только появляемся
	bool Show{true};
};

class cParticleSystem {
public:
	~cParticleSystem();

	// нужно удалить
	bool NeedDestroy{false};

	// обновить все частицы в этой системе, по времени
	bool Update(float Time);
	// прорисовка всех частиц
	void Draw(sTexture **CurrentTexture);
	// базовая текстура частиц
	sTexture *Texture{nullptr};

	// тип смешивания
	int BlendType{0};

	// начальный установки, или установки где пред. и текушее положения равны
	void SetStartLocation(const sVECTOR3D &NewLocation);
	// передвинуть все частици на указаное значение
	void MoveSystem(const sVECTOR3D &NewLocation);
	// сдвинуть только центр системы
	void MoveSystemLocation(const sVECTOR3D &NewLocation);
	// развернуть направление системы
	void RotateSystemByAngle(const sVECTOR3D &NewDirection);
	void RotateSystemAndParticlesByAngle(const sVECTOR3D &NewAngle);
	// разворот только частиц
	void RotateParticlesByAngle(const sVECTOR3D &NewAngle);

	// остановить все частицы в системе
	void StopAllParticles();

	// поставить правильный полет частиц, т.е. учет внешней скорости
	void ChangeSpeed(const sVECTOR3D &Vel);

	// текущее направление системы (используется для создания частиц+некоторые вариации)
	sVECTOR3D Direction{0.0f, 0.0f, 0.0f};
	// угол поворота системы
	sVECTOR3D Angle{0.0f, 0.0f, 0.0f};

	// кол-во создаваемых частиц в секунду
	unsigned int ParticlesPerSec{100};

	// если нужно замедлять и остановить
	bool NeedStop{false};

	// размер частиц в мировых координатах
	float SizeStart{1.0f};
	float SizeVar{0.0f};
	float SizeEnd{1.0f};

	// Прозрачность частиц. Alpha 0 = невидем, Alpha 1 = видемость 100%
	float AlphaStart{1.0f};
	float AlphaVar{0.0f};
	float AlphaEnd{1.0f};
	bool AlphaShowHide{false}; // если нужно сначало ув. потом ум.

	// Цвет частиц при старте и завершении
	// линейно интерполируется
	sRGBCOLOR ColorStart{1.0f, 1.0f, 1.0f};
	sRGBCOLOR ColorVar{0.0f, 0.0f, 0.0f};
	sRGBCOLOR ColorEnd{1.0f, 1.0f, 1.0f};

	// Скалярная скорость, с вектором направления получаем вектор движения
	float Speed{1.0f};
	float SpeedVar{1.0f};
	// Скорость при создании, сохраняем, если будут менять скорость в процессе
	float SpeedOnCreation{1.0f};

	eParticleCreationType CreationType{eParticleCreationType::Point};
	sVECTOR3D CreationSize{0.05f, 0.05f, 0.05f};

	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float DeadZone{0.0f};
	// коэффициент уменьшения от расстояния (чем ближе к камере - тем меньше)
	float Resize{1.0f}; // если 0, то у самой камеры будет нулевой, у крайней точки системы - Size

	// жизнь частицы в секундах
	float Life{1.0f};
	float LifeVar{0.0f};

	// показывает, насколько отличным будет выбор направления у создаваемой частицы
	// с направлением системы
	float Theta{1.0f};

	// система притягивает частицы или нет
	bool IsMagnet{0};
	float MagnetFactor{25.0f};

	// можем ли мы создавать частицы или нет
	bool IsSuppressed{false};
	bool DestroyIfNoParticles{false};

	// возраст системы в секундах
	float Age{0.0f};

	// последнее время обновления системы
	float TimeLastUpdate{-1.0f};

	// остаток создания частицы (если к примеру 0.5 частиц в секунду стоит)
	float EmissionResidue{0.0f};

	// источник света, если он нужен
	sLight *Light{nullptr};
	bool LightNeedDeviation{false};
	float LightDeviation{100.0f};
	float NextLightDeviation{0.7f + 0.3f * vw_Randf1};
	float LightDeviationSpeed{3.5f + 3.5f * vw_Randf1};

	// доступ к private данным
	const sVECTOR3D &GetLocation()
	{
		return Location;
	}

	// cycle for each particle in list, for external manipulations directly with particles data
	void ForEachParticle(std::function<void (sVECTOR3D &pLocation,
						 sVECTOR3D &pVelocity,
						 bool &pNeedStop)> function)
	{
		for (auto &tmpParticle : ParticlesList) {
			function(tmpParticle.Location,
				 tmpParticle.Velocity,
				 tmpParticle.NeedStop);
		}
	}

private:
	// последнее положение системы (для интерполяции)
	sVECTOR3D PrevLocation{0.0f, 0.0f, 0.0f};
	// текущее положение частиц в пространстве
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};

	// Emit particles.
	void EmitParticles(unsigned int Quantity);
	// Generate location for new particle (point type).
	void GenerateLocationPointType(cParticle &NewParticle);
	// Generate location for new particle (cube type).
	void GenerateLocationCubeType(cParticle &NewParticle);
	// Generate location for new particle (tube type).
	void GenerateLocationTubeType(cParticle &NewParticle);
	// Generate location for new particle (sphere type).
	void GenerateLocationSphereType(cParticle &NewParticle);

	// Update light.
	void UpdateLight(float TimeDelta);

	// Calculate current AABB.
	void CalculateAABB();
	// для проверки вхождения в видимую область
	// начальныя установка коробок
	// ставим тут очень большие параметры, вне зоны прорисовки, т.к. в партиклах на функциях перемещения
	// не учитываем изменение коробки, и координаты в коробках у нас абсолютные (!!!)
	// использовать относительные координаты бокса нельзя, т.к. можем преносить только центр, а можем
	// и центр, и все частицы...
	sVECTOR3D AABB[8]{{-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f},
			  {-1000000.0f, 1000000.0f, -1000000.0f}};

	// particles
	std::forward_list<cParticle> ParticlesList{};
	// we could use std::list with size(), but we don't need doubly-linked list here
	unsigned int ParticlesCountInList{0};

	// current rotation matrix for fast calculations
	float CurrentRotationMat[9]{1.0f, 0.0f, 0.0f,
				    0.0f, 1.0f, 0.0f,
				    0.0f, 0.0f, 1.0f};
	// old inversed rotation matrix for fast calculations
	float OldInvRotationMat[9]{1.0f, 0.0f, 0.0f,
				   0.0f, 1.0f, 0.0f,
				   0.0f, 0.0f, 1.0f};
};


// Инициализация менеджера частиц
void vw_InitParticleSystems(bool UseGLSL, float Quality);
// Create particle system.
cParticleSystem *vw_CreateParticleSystem();
// Release particle system.
void vw_ReleaseParticleSystem(cParticleSystem *ParticleSystem);
// Удаляем все объекты в списке
void vw_ReleaseAllParticleSystems();
// Прорисовываем все системы
void vw_DrawAllParticleSystems();
// Прорисовываем блок ParticleSystems
void vw_DrawParticleSystems(cParticleSystem **DrawParticleSystem, int Quantity);
// Проверяем все объекты, обновляем данные
void vw_UpdateAllParticleSystems(float Time);

#endif //PARTICLESYSTEM_H
