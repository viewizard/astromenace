/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef OBJECT3D_WEAPON_WEAPON_H
#define OBJECT3D_WEAPON_WEAPON_H

#include "../object3d.h"
#include "../projectile/projectile.h"

enum class eGameSFX;

class cWeapon : public cObject3D {
public:
	cWeapon();
	virtual ~cWeapon();

	// Обновление данных объектa
	virtual bool Update(float Time) override;
	// Выстрел
	virtual bool WeaponFire(float Time);
	// Установка углов поворота
	virtual void SetRotation(sVECTOR3D NewRotation) override;
	// Установка положения
	virtual void SetLocation(sVECTOR3D NewLocation) override;
	// Создание нужного объекта
	virtual void Create(int WeaponNum);

	// уровень оружия, по мощности
	int WeaponLevel{1};

	// боекомплект
	int Ammo{100};
	// начальное значение боекомплекта
	int AmmoStart{100};

	// скорострельность, в мили секундах - время между выстрелами
	float NextFireTime{0.3f};
	// время последнего выстрела
	float LastFireTime{-10.0f};

	// потребление энергии на выстрел
	float EnergyUse{1.0f};
	// текущее состояние энергии
	float CurrentEnergyAccumulated{0.0f};

	// swamp
	int SwampNum{0};

	// нужно вращать-поворачивать при наведении внешне ( не нужно для турелей и ракетных систем)
	bool NeedRotateOnTargeting{true};

	// лучевое оружие
	// собственно объект=снаряд
	cProjectile *LaserMaser{nullptr};
	// номер проигрываемого звука, чтобы была возможность выключить при уничтожении
	unsigned int LaserMaserSoundNum{0};
	// флаг, это турель
	bool WeaponTurret{false};

	// для просчета положения точки стрельбы, считаем как кости
	sVECTOR3D BaseBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D MiddleBound{0.0f, 0.0f, 0.0f};
	sVECTOR3D WeaponBound{0.0f, 0.0f, 0.0f};

	// набор указателей на номера объектов, которыми нацеливаемся по горизонтале
	int TargetHorizBlocks{-1};
	float TargetHorizBlocksCurrentAngle{0.0f};
	float TargetHorizBlocksNeedAngle{0.0f};
	// набор указателей на номера объектов, которыми нацеливаемся по вертикали
	int TargetVertBlocks{-1};
	float TargetVertBlocksMaxAngle{80.0f};
	float TargetVertBlocksMinAngle{0.0f};
	float TargetVertBlocksCurrentAngle{0.0f};
	float TargetVertBlocksNeedAngle{0.0f};

	// выстрел - вылет частиц (засветка возле ствола)
	std::weak_ptr<cParticleSystem> Fire{};
	sVECTOR3D FireLocation{0.0f, 0.0f, 0.0f};
	eGameSFX SFX{static_cast<eGameSFX>(0)}; // would be value initialized to 0, eGameSFX::none

	// если оружие уничтожено, делаем вырывающийся огонь
	std::weak_ptr<cParticleSystem> DestroyedFire{};
	std::weak_ptr<cParticleSystem> DestroyedSmoke{};
	sVECTOR3D DestroyedFireLocation{0.0f, 0.0f, 0.0f};

	cWeapon *Next{nullptr};
	cWeapon *Prev{nullptr};
};


//-----------------------------------------------------------------------------
// Дополнительные функции для cWeapon
//-----------------------------------------------------------------------------

// Установка нужных данных для вспышки возле ствола
void SetWeaponFire(std::shared_ptr<cParticleSystem> &ParticleSystem, int WeaponNum);


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

#endif // OBJECT3D_WEAPON_WEAPON_H
