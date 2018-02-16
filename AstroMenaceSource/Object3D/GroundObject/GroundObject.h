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


#ifndef GROUNDOBJECT_H
#define GROUNDOBJECT_H


#include "../Object3D.h"
#include "../Weapon/Weapon.h"



//-----------------------------------------------------------------------------
// Класс CGroundObject
//-----------------------------------------------------------------------------
class CGroundObject : public CObject3D
{
public:

	// базовые конструктор и деструктор объекта
	CGroundObject();
	virtual ~CGroundObject();


	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Установка положения объекта
	virtual void	SetLocation(VECTOR3D NewLocation);
	// Установка углов поворота объекта
	virtual void	SetRotation(VECTOR3D NewRotation);


	// колеса, для транспорта

	// скорость вращения колес
	float	WheelTrackSpeed;

	// кол-во колес
	int		WheelQuantity;
	// перечень номеров объектов с колесами
	int		*WheelObjectsNum;

	// колеса поворотные, для установки угла поворота
	// кол-во колес
	int		WheelRotateQuantity;
	// перечень номеров объектов с колесами
	int		*WheelRotateObjectsNum;
	// скорость вращения колес
	float	WheelRotateSpeed;
	// максимальный угол поворота колес для модели
	float	MaxWheelRotateAngle;

	// если нужно, номер объекта с траком для тайловой анимации
	int		TrackObjectNum;
	int		TrackRotationDirection;// направление вращения траков


	// нужно повернуть (пересчитать мэш) при следующем проходе
	// Update + учесть эти данные в Rotation
	VECTOR3D	NeedRotate;
	// скорость поворота по каждому раправлению
	VECTOR3D	RotationSpeed;

	// максимальная скорость units/sec, зависит от двигателя
	float		MaxSpeed;
	// текущая скорость
	float		Speed;
	// нужная скорость
	float		NeedSpeed;

	// максимальное ускорение units/sec*sec, зависит от двигателя
	float		MaxAcceler;
	// макс. маневровых двигателей
	float		MaxSpeedRotate;
	// текущее ускорение
	float		Acceler;
	// нужное ускорение
	float		NeedAcceler;




	// кол-во оружия доступного на данной моделе
	int					WeaponQuantity;
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool				*WeaponSetFire;
	// указатель на массив оружия
	CWeapon				**Weapon;
	// расположение оружия (относительное)
	VECTOR3D			*WeaponLocation;
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int					WeaponFireType;
	int 				WeaponGroupCurrentFireNum;
	float 				WeaponGroupCurrentFireDelay;
	// наведение на цель
	bool				WeaponTargeting;

	// для просчета положения точки стрельбы, считаем как кости
	VECTOR3D			BaseBound;
	VECTOR3D			MiddleBound;
	VECTOR3D			*WeaponBound;
	// если точки фиксированы, не нужно менять направление точки выстрела
	// только учитываем общий поворот модели и соотв. точку выстрела
	// работает только если объекты поворота башни и ствола не заданы!
	bool				DoNotCalculateRotation;



	// коэф. преобразования скорость в скорость вращения колес
	float	SpeedToRotate;


	// набор указателей на номера объектов, которыми нацеливаемся по горизонтале
	int		TargetHorizObjectQuantity;
	int		*TargetHorizObject;
	float	TargetHorizObjectCurrentAngle;
	float	TargetHorizObjectNeedAngle;
	// набор указателей на номера объектов, которыми нацеливаемся по вертикали
	int		TargetVertObjectQuantity;
	int		*TargetVertObject;
	float	TargetVertObjectMaxAngle;
	float	TargetVertObjectMinAngle;
	float	TargetVertObjectCurrentAngle;
	float	TargetVertObjectNeedAngle;
	// если нужно вращать ствол (многоствольный пулемет)
	int		BarrelObjectQuantity;
	int		*BarrelObject;


	// для собственного списка
	CGroundObject* Next;
	CGroundObject* Prev;

};





//-----------------------------------------------------------------------------
// Менеджер GroundObject, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachGroundObject(CGroundObject* GroundObject);
// Исключаем из списка
void	DetachGroundObject(CGroundObject* GroundObject);
// Проверяем все объекты, обновляем данные
void	UpdateAllGroundObject(float Time);
// Прорисовываем все объекты
void	DrawAllGroundObject(bool VertexOnlyPass, unsigned int ShadowMap);
// Удаляем все объекты в списке
void	ReleaseAllGroundObject();





#endif // GROUNDOBJECT_H
