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

#ifndef WEAPON_H
#define WEAPON_H

#include "../object3d.h"
#include "../projectile/projectile.h"


//-----------------------------------------------------------------------------
// Класс cWeapon
//-----------------------------------------------------------------------------
class cWeapon : public cObject3D
{
public:

	cWeapon();
	virtual ~cWeapon();

	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Выстрел
	virtual bool	WeaponFire(float Time);
	// Установка углов поворота
	virtual void	SetRotation(sVECTOR3D NewRotation);
	// Установка положения
	virtual void	SetLocation(sVECTOR3D NewLocation);
	// Создание нужного объекта
	virtual void	Create(int WeaponNum);

	// уровень оружия, по мощьности
	int	WeaponLevel{1};

	// боекомплект
	int	Ammo{100};
	// начальное значение боекомплекта
	int	AmmoStart{100};

	// скорострельность, в мили секундах - время между выстрелами
	float	NextFireTime{0.3f};
	// время последнего выстрела
	float	LastFireTime{-10.0f};

	// потребление энергии на выстрел
	float	EnergyUse{1.0f};
	// текущее состояние энергии
	float	CurrentEnergyAccumulated{0.0f};

	// swamp
	int	SwampNum{0};

	// нужно вращать-поворачивать при наведении внешне ( не нужно для турелей и ракетных систем)
	bool	NeedRotateOnTargeting{true};

	// лучевое оружие
	// собственно объект=снаряд
	cProjectile	*LaserMaser{nullptr};
	// номер проигрываемого звука, чтобы была возможность выключить при уничтожении
	unsigned int LaserMaserSoundNum{0};
	// флаг, это турель
	bool	WeaponTurret{false};

	// для просчета положения точки стрельбы, считаем как кости
	sVECTOR3D	BaseBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D	MiddleBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D	WeaponBound{0.0f, 0.0f, 0.0f};

	// набор указателей на номера объектов, которыми нацеливаемся по горизонтале
	int	TargetHorizObject{-1};
	float	TargetHorizObjectCurrentAngle{0.0f};
	float	TargetHorizObjectNeedAngle{0.0f};
	// набор указателей на номера объектов, которыми нацеливаемся по вертикали
	int	TargetVertObject{-1};
	float	TargetVertObjectMaxAngle{80.0f};
	float	TargetVertObjectMinAngle{0.0f};
	float	TargetVertObjectCurrentAngle{0.0f};
	float	TargetVertObjectNeedAngle{0.0f};

	// выстрел - вылет частиц (засветка возле ствола)
	cParticleSystem	*Fire{nullptr};
	sVECTOR3D	FireLocation{0.0f, 0.0f, 0.0f};
	int		SoundNum{0}; // нужный номер

	// если оружие уничтожено, делаем вырывающийся огонь
	cParticleSystem	*DestroyedFire{nullptr};
	cParticleSystem	*DestroyedSmoke{nullptr};
	sVECTOR3D	DestroyedFireLocation{0.0f, 0.0f, 0.0f};

	cWeapon	*Next{nullptr};
	cWeapon	*Prev{nullptr};
};


//-----------------------------------------------------------------------------
// Дополнительные функции для cWeapon
//-----------------------------------------------------------------------------

// Установка нужных данных для вспышки возле ствола
void SetWeaponFire(cParticleSystem *ParticleSystem, int WeaponNum);


//-----------------------------------------------------------------------------
// Менеджер cWeapon, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachWeapon(cWeapon* Weapon);
// Исключаем из списка
void	DetachWeapon(cWeapon* Weapon);
// Проверяем все объекты, обновляем данные
void	UpdateAllWeapon(float Time);
// Прорисовываем все объекты
void	DrawAllWeapon(bool VertexOnlyPass, unsigned int ShadowMap);
// Удаляем все объекты в списке
void	ReleaseAllWeapon();

#endif // WEAPON_H
