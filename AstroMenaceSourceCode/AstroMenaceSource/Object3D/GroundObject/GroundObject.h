/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: GroundObject.h

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
	~CGroundObject();


	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Установка положения объекта
	virtual void	SetLocation(VECTOR3D NewLocation);
	// Установка углов поворота объекта
	virtual void	SetRotation(VECTOR3D NewRotation);
	// Создание нужного объекта
	virtual void	Create(int	GroundObjectNum){};


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
	int		TrackRotationDirectionl;// направление вращения траков



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
	float	TargetHorizObjectMaxAngle;
	float	TargetHorizObjectMinAngle;
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
void	DrawAllGroundObject();
// Удаляем все объекты в списке
void	ReleaseAllGroundObject();





#endif // GROUNDOBJECT_H
