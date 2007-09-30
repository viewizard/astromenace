/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceObject.h

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


#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H


#include "../Object3D.h"
#include "../Explosion/SpaceExplosion/SpaceExplosion.h"


//-----------------------------------------------------------------------------
// Класс CSpaceObject
//-----------------------------------------------------------------------------
class CSpaceObject : public CObject3D
{
public:

	// базовые конструктор и деструктор объекта
	CSpaceObject();
	virtual ~CSpaceObject();

	// Обновление данных объектa
	virtual bool	Update(float Time);
	// Установка положения объекта
	virtual void	SetLocation(VECTOR3D NewLocation);
	// Установка углов поворота объекта
	virtual void	SetRotation(VECTOR3D NewRotation);

	// скорость
	float	Speed;
	VECTOR3D	RotationSpeed;
	VECTOR3D	Velocity;

	// последнее положение камеры нужно для планет
	VECTOR3D	LastCameraPoint;

	// кол-во эффектов
	int				GFXQuantity;
	// эффекты
	VECTOR3D		*GFXLocation;
	eParticleSystem	**GFX;


	// чтобы возрвать часть корабля босса пришельцев через время
	float BossPartCountDown;



	// для собственного списка
	CSpaceObject* Next;
	CSpaceObject* Prev;

};


// установка эффекта, если нужно
void SetSpaceObjectGFX(eParticleSystem *ParticleSystem, int GFXType);



//-----------------------------------------------------------------------------
// Менеджер SpaceObject, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachSpaceObject(CSpaceObject* SpaceObject);
// Исключаем из списка
void	DetachSpaceObject(CSpaceObject* SpaceObject);
// Проверяем все объекты, обновляем данные
void	UpdateAllSpaceObject(float Time);
// Прорисовываем все объекты
void	DrawAllSpaceObject();
// Удаляем все объекты в списке
void	ReleaseAllSpaceObject();





#endif // SPACEOBJECT_H
