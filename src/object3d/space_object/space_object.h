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

#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

#include "../object3d.h"
#include "../explosion/space_explosion/space_explosion.h"

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
	float		Speed{0.0f};
	VECTOR3D	RotationSpeed{0.0f, 0.0f, 0.0f};
	VECTOR3D	Velocity{0.0f, 0.0f, 0.0f};

	// последнее положение камеры нужно для планет
	VECTOR3D	LastCameraPoint{0.0f, 0.0f, 0.0f};

	// кол-во эффектов
	int		GFXQuantity{0};
	// эффекты
	VECTOR3D	*GFXLocation{nullptr};
	eParticleSystem	**GFX{nullptr};

	// чтобы возрвать часть корабля босса пришельцев через время
	float		BossPartCountDown{-1.0f};

	// для собственного списка
	CSpaceObject	*Next{nullptr};
	CSpaceObject	*Prev{nullptr};
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
