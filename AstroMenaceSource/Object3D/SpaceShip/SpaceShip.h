/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceShip.h

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


#ifndef SPACESHIP_H
#define SPACESHIP_H


#include "../Object3D.h"
#include "../Weapon/Weapon.h"




//-----------------------------------------------------------------------------
// Класс CSpaceShip
//-----------------------------------------------------------------------------
class CSpaceShip : public CObject3D
{
public:

	// базовые конструктор и деструктор объекта
	CSpaceShip();
	virtual ~CSpaceShip();


	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Установка положения объекта
	virtual void	SetLocation(VECTOR3D NewLocation);
	virtual void	SetLocationArcadePlayer(VECTOR3D NewLocation);
	// Установка углов поворота объекта
	virtual void	SetRotation(VECTOR3D NewRotation, bool NeedWeaponRotate = true);


	// текущей вектор движения корабля
	VECTOR3D	Velocity;


	// нужно повернуть (пересчитать мэш) при следующем проходе
	// Update + учесть эти данные в Rotation
	VECTOR3D	NeedRotate;
	// скорость поворота по каждому раправлению
	VECTOR3D	RotationSpeed;



	// максимальная скорость units/sec, зависит от двигателя
	float		MaxSpeed;
	// максимальное ускорение units/sec*sec, зависит от двигателя
	float		MaxAcceler;
	// макс. маневровых двигателей
	float		MaxSpeedRotate;



	// вперед-назад
	// текущая скорость
	float		Speed;
	// нужная скорость
	float		NeedSpeed;
	// текущее ускорение
	float		Acceler;
	// нужное ускорение
	float		NeedAcceler;

	// влево-вправо
	// текущая скорость
	float		SpeedLR;
	// нужная скорость
	float		NeedSpeedLR;
	// текущее ускорение
	float		AccelerLR;
	// нужное ускорение
	float		NeedAccelerLR;

	// вверх-вниз
	// текущая скорость
	float		SpeedUD;
	// нужная скорость
	float		NeedSpeedUD;
	// текущее ускорение
	float		AccelerUD;
	// нужное ускорение
	float		NeedAccelerUD;



	// вперед-назад по вектору камеры
	// текущая скорость
	float		SpeedByCamFB;
	// нужная скорость
	float		NeedSpeedByCamFB;
	// текущее ускорение
	float		AccelerByCamFB;
	// нужное ускорение
	float		NeedAccelerByCamFB;

	// влево-вправо
	// текущая скорость
	float		SpeedByCamLR;
	// нужная скорость
	float		NeedSpeedByCamLR;
	// текущее ускорение
	float		AccelerByCamLR;
	// нужное ускорение
	float		NeedAccelerByCamLR;

	// вверх-вниз
	// текущая скорость
	float		SpeedByCamUD;
	// нужная скорость
	float		NeedSpeedByCamUD;
	// текущее ускорение
	float		AccelerByCamUD;
	// нужное ускорение
	float		NeedAccelerByCamUD;





	// управление - куда перемещаться при сделующем Update'е
	// от 0.0 до 1.0, по "силе" движения в нужную сторону
	float	MoveForward;
	float	MoveBackward;
	float	MoveLeft;
	float	MoveRight;
	float	MoveUp;
	float	MoveDown;



	// кол-во оружия доступного на данной моделе
	int					WeaponQuantity;
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool				*WeaponSetFire;
	// указатель на массив оружия
	CWeapon				**Weapon;
	// расположение оружия на коробле (относительное)
	VECTOR3D			*WeaponLocation;
	// тип оружия по мощьности, для определенных слотов
	int					*WeaponType;
	// первоначальный доворот оружия для корабля (будем использовать только для игрка, для остальных он ноль)
	float				*WeaponYAngle;
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int					WeaponFireType;
	int 				WeaponGroupCurrentFireNum;
	float 				WeaponGroupCurrentFireDelay;


	// кол-во оружия доступного на данной моделе
	int					BossWeaponQuantity;
	// выстрел из оружия, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool				*BossWeaponSetFire;
	// указатель на массив оружия
	CWeapon				**BossWeapon;
	// расположение оружия на коробле (относительное)
	VECTOR3D			*BossWeaponLocation;
	// тип оружия по мощьности, для определенных слотов
	int					*BossWeaponType;
	// первоначальный доворот оружия для корабля (будем использовать только для игрка, для остальных он ноль)
	float				*BossWeaponYAngle;
	// тип стрельбы из оружия 1-обычный, 2-переменный (по умолчанию)
	int					BossWeaponFireType;
	int 				BossWeaponGroupCurrentFireNum;
	float 				BossWeaponGroupCurrentFireDelay;


	// выстрел из оружия-фларес, т.е. передача команды "стрелять" оружию при сделующем Update'е
	bool				WeaponFlareSetFire;
	// указатель на оружие-фларе
	CWeapon				*WeaponFlare;
	// расположение оружия на коробле (относительное)
	VECTOR3D			WeaponFlareLocation;



	// кол-во двигателей
	int				EngineQuantity;
	// тип, как будем удалять двигатели -сразу, или глушить
	bool			EngineDestroyType;
	// двигатели
	eParticleSystem	**Engine;
	// положение двигателей
	VECTOR3D		*EngineLocation;


	// вкл. двигатели торможения при повороте
	bool	NeedStopRotation;

	// двигатели поворотов, левый
	int				EngineLeftQuantity; // кол-во двигателей
	eParticleSystem	**EngineLeft; // двигатели
	VECTOR3D		*EngineLeftLocation; // положение двигателей

	// двигатели поворотов, правый
	int				EngineRightQuantity; // кол-во двигателей
	eParticleSystem	**EngineRight; // двигатели
	VECTOR3D		*EngineRightLocation; // положение двигателей



	// для собственного списка
	CSpaceShip* Next;
	CSpaceShip* Prev;

};





//-----------------------------------------------------------------------------
// Менеджер CSpaceShip, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachSpaceShip(CSpaceShip* SpaceShip);
// Исключаем из списка
void	DetachSpaceShip(CSpaceShip* SpaceShip);
// Находим нужный по ID
CSpaceShip *FindSpaceShipByID(int ID);
// Проверяем все объекты, обновляем данные
void	UpdateAllSpaceShip(float Time);
// Прорисовываем все объекты
void	DrawAllSpaceShip(bool VertexOnlyPass);
// Удаляем все объекты в списке
void	ReleaseAllSpaceShip();





#endif // SPACESHIP_H
