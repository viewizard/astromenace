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


#ifndef WEAPON_H
#define WEAPON_H


#include "../Object3D.h"
#include "../Projectile/Projectile.h"



//-----------------------------------------------------------------------------
// Класс CWeapon
//-----------------------------------------------------------------------------
class CWeapon : public CObject3D
{
public:

	CWeapon();
	virtual ~CWeapon();


	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Выстрел
	virtual bool	WeaponFire(float Time);
	// Установка углов поворота
	virtual void	SetRotation(VECTOR3D NewRotation);
	// Установка положения
	virtual void	SetLocation(VECTOR3D NewLocation);
	// Создание нужного объекта
	virtual void	Create(int	WeaponNum);


	// уровень оружия, по мощьности
	int			WeaponLevel;

	// боекомплект
	int			Ammo;
	// начальное значение боекомплекта
	int			AmmoStart;

	// скорострельность, в мили секундах - время между выстрелами
	float		NextFireTime;
	// время последнего выстрела
	float		LastFireTime;

	// потребление энергии на выстрел
	float		EnergyUse;
	// текущее состояние энергии
	float		CurrentEnergyAccumulated;

	// swamp
	int			SwampNum;

	// нужно вращать-поворачивать при наведении внешне ( не нужно для турелей и ракетных систем)
	bool		NeedRotateOnTargeting;

	// лучевое оружие
	// собственно объект=снаряд
	CProjectile	*LaserMaser;
	// номер проигрываемого звука, чтобы была возможность выключить при уничтожении
	int			LaserMaserSoundNum;





	// флаг, это турель
	bool	WeaponTurret;

	// для просчета положения точки стрельбы, считаем как кости
	VECTOR3D			BaseBound;
	VECTOR3D			MiddleBound;
	VECTOR3D			WeaponBound;

	// набор указателей на номера объектов, которыми нацеливаемся по горизонтале
	int		TargetHorizObject;
	float	TargetHorizObjectCurrentAngle;
	float	TargetHorizObjectNeedAngle;
	// набор указателей на номера объектов, которыми нацеливаемся по вертикали
	int		TargetVertObject;
	float	TargetVertObjectMaxAngle;
	float	TargetVertObjectMinAngle;
	float	TargetVertObjectCurrentAngle;
	float	TargetVertObjectNeedAngle;







	// выстрел - вылет частиц (засветка возле ствола)
	eParticleSystem	*Fire;
	VECTOR3D		FireLocation;
	int		SoundNum; // нужный номер

	// если оружие уничтожено, делаем вырывающийся огонь
	eParticleSystem	*DestroyedFire;
	eParticleSystem	*DestroyedSmoke;
	VECTOR3D		DestroyedFireLocation;


	CWeapon *Next;
	CWeapon *Prev;

};







//-----------------------------------------------------------------------------
// Дополнительные функции для CWeapon
//-----------------------------------------------------------------------------

// Установка нужных данных для вспышки возле ствола
void SetWeaponFire(eParticleSystem *ParticleSystem, int WeaponNum);






//-----------------------------------------------------------------------------
// Менеджер CWeapon, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachWeapon(CWeapon* Weapon);
// Исключаем из списка
void	DetachWeapon(CWeapon* Weapon);
// Проверяем все объекты, обновляем данные
void	UpdateAllWeapon(float Time);
// Прорисовываем все объекты
void	DrawAllWeapon(bool VertexOnlyPass, unsigned int ShadowMap);
// Удаляем все объекты в списке
void	ReleaseAllWeapon();




#endif // WEAPON_H
