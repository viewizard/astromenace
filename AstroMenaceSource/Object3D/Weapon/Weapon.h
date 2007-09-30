/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Weapon.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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
	float	TargetHorizObjectMaxAngle;
	float	TargetHorizObjectMinAngle;
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
void	DrawAllWeapon();
// Удаляем все объекты в списке
void	ReleaseAllWeapon();




#endif // WEAPON_H
