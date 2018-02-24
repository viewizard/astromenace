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

#ifndef GROUNDOBJECT_H
#define GROUNDOBJECT_H

#include "../object3d.h"
#include "../weapon/weapon.h"


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
	float	WheelTrackSpeed{0.0f};

	// кол-во колес
	int	WheelQuantity{0};
	// перечень номеров объектов с колесами
	int	*WheelObjectsNum{nullptr};

	// колеса поворотные, для установки угла поворота
	// кол-во колес
	int	WheelRotateQuantity{0};
	// перечень номеров объектов с колесами
	int	*WheelRotateObjectsNum{nullptr};
	// скорость вращения колес
	float	WheelRotateSpeed{0.0f};
	// максимальный угол поворота колес для модели
	float	MaxWheelRotateAngle{20.0f};

	// если нужно, номер объекта с траком для тайловой анимации
	int	TrackObjectNum{-1};
	int	TrackRotationDirection{1};// направление вращения траков

	// нужно повернуть (пересчитать мэш) при следующем проходе
	// Update + учесть эти данные в Rotation
	VECTOR3D	NeedRotate{0.0f, 0.0f, 0.0f};
	// скорость поворота по каждому раправлению
	VECTOR3D	RotationSpeed{1.0f, 1.0f, 1.0f};

	// максимальная скорость units/sec, зависит от двигателя
	float	MaxSpeed{0.0f};
	// текущая скорость
	float	Speed{0.0f};
	// нужная скорость
	float	NeedSpeed{0.0f};

	// максимальное ускорение units/sec*sec, зависит от двигателя
	float	MaxAcceler{0.0f};
	// макс. маневровых двигателей
	float	MaxSpeedRotate{0.0f};
	// текущее ускорение
	float	Acceler{0.0f};
	// нужное ускорение
	float	NeedAcceler{1.0f};

	// кол-во оружия доступного на данной моделе
	int		WeaponQuantity{0};
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool		*WeaponSetFire{nullptr};
	// указатель на массив оружия
	CWeapon		**Weapon{nullptr};
	// расположение оружия (относительное)
	VECTOR3D	*WeaponLocation{nullptr};
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int		WeaponFireType{2};
	int		WeaponGroupCurrentFireNum{-1};
	float		WeaponGroupCurrentFireDelay{0.0f};
	// наведение на цель
	bool		WeaponTargeting{false};

	// для просчета положения точки стрельбы, считаем как кости
	VECTOR3D	BaseBound{0.0f, 0.0f, 0.0f};
	VECTOR3D	MiddleBound{0.0f, 0.0f, 0.0f};
	VECTOR3D	*WeaponBound{nullptr};
	// если точки фиксированы, не нужно менять направление точки выстрела
	// только учитываем общий поворот модели и соотв. точку выстрела
	// работает только если объекты поворота башни и ствола не заданы!
	bool	DoNotCalculateRotation{false};

	// коэф. преобразования скорость в скорость вращения колес
	float	SpeedToRotate{0.0f};

	// набор указателей на номера объектов, которыми нацеливаемся по горизонтале
	int	TargetHorizObjectQuantity{0};
	int	*TargetHorizObject{nullptr};
	float	TargetHorizObjectCurrentAngle{0.0f};
	float	TargetHorizObjectNeedAngle{0.0f};
	// набор указателей на номера объектов, которыми нацеливаемся по вертикали
	int	TargetVertObjectQuantity{0};
	int	*TargetVertObject{nullptr};
	float	TargetVertObjectMaxAngle{80.0f};
	float	TargetVertObjectMinAngle{0.0f};
	float	TargetVertObjectCurrentAngle{0.0f};
	float	TargetVertObjectNeedAngle{0.0f};
	// если нужно вращать ствол (многоствольный пулемет)
	int	BarrelObjectQuantity{0};
	int	*BarrelObject{nullptr};

	// для собственного списка
	CGroundObject	*Next{nullptr};
	CGroundObject	*Prev{nullptr};
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
