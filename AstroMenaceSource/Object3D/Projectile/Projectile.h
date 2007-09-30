/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Projectile.h

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


#ifndef PROJECTILE_H
#define PROJECTILE_H


#include "../Object3D.h"




//-----------------------------------------------------------------------------
// Класс CProjectile
//-----------------------------------------------------------------------------
class CProjectile : public CObject3D
{
public:

	CProjectile();
	virtual ~CProjectile();

	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Установка углов поворота
	virtual void	SetRotation(VECTOR3D NewRotation);
	// Установка положения
	virtual void	SetLocation(VECTOR3D NewLocation);
	// Создание нужного объекта
	virtual void	Create(int ProjectileNum);


	// Номер типа снаряда при создании
	int			Num;

	// повреждение снаряда
	float		DamageHull;
	float		DamageSystems;


	// Класс снаряда
	// 0 - обычный
	// 1 - можем наводится и сбивать снаряд
	// 2 - лучевое оружие
	// 3 - фларе против ракет
	int			ProjectileType;

	// для ракет, тянем на кого навелись... чтобы цель могла принять действия
	CObject3D	*Target;

	// данные для 2-го класса (лучевого снаряда)
	// эти данные меняем только в объекте-орудии!!!
	// точка центра снаряда
	VECTOR3D	ProjectileCenter;


	// скорость снаряда
	float		Speed;
	float		SpeedStart;
	float		SpeedEnd;


	// общее время жизни снаряда
	float		Age;

	// нужно ли играть звук при уничтожении
	bool		NeedDeadSound;


	// временные данные, для подсветки мин
	float		MineIData;

	// временные данные, для мин которые стреляют - время перезарядки
	float		MineReloadTime;
	float		MineNextFireTime;


	// для прорисовки графических эффектов
	int					GraphicFXQuantity;
	// тип, как будем удалять -сразу, или глушить
	bool				GraphicFXDestroyType;
	VECTOR3D			*GraphicFXLocation;
	eParticleSystem		**GraphicFX;

	bool				NeedStopPartic;

	CProjectile *Next;
	CProjectile *Prev;

};







//-----------------------------------------------------------------------------
// Дополнительные функции для CProjectile
//-----------------------------------------------------------------------------

// Установка нужных данных для вспышки возле ствола
void SetProjectileGFX(eParticleSystem *ParticleSystem, int GFXNum);
// получаем время жизни снаряда
float GetProjectileRange(int Num);




//-----------------------------------------------------------------------------
// Менеджер CSpaceShipWeapon, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachProjectile(CProjectile* Projectile);
// Исключаем из списка
void	DetachProjectile(CProjectile* Projectile);
// Проверяем все объекты, обновляем данные
void	UpdateAllProjectile(float Time);
// Прорисовываем все объекты
void	DrawAllProjectile();
// Удаляем все объекты в списке
void	ReleaseAllProjectile();




#endif // PROJECTILE_H
