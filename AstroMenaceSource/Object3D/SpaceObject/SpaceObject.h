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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


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
void	DrawAllSpaceObject(bool VertexOnlyPass, unsigned int ShadowMap);
int 	DrawAllSpaceObjectCount(int DrawOnlyType);
// Удаляем все объекты в списке
void	ReleaseAllSpaceObject();





#endif // SPACEOBJECT_H
