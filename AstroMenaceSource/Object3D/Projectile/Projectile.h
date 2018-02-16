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
void	DrawAllProjectile(bool VertexOnlyPass, unsigned int ShadowMap);
// Удаляем все объекты в списке
void	ReleaseAllProjectile();




#endif // PROJECTILE_H
