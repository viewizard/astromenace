/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
// Класс cSpaceObject
//-----------------------------------------------------------------------------
class cSpaceObject : public cObject3D
{
public:
	// базовые конструктор и деструктор объекта
	cSpaceObject();
	virtual ~cSpaceObject();

	// Обновление данных объектa
	virtual bool	Update(float Time) override;
	// Установка положения объекта
	virtual void	SetLocation(sVECTOR3D NewLocation) override;
	// Установка углов поворота объекта
	virtual void	SetRotation(sVECTOR3D NewRotation) override;

	// скорость
	float		Speed{0.0f};
	sVECTOR3D	RotationSpeed{0.0f, 0.0f, 0.0f};
	sVECTOR3D	Velocity{0.0f, 0.0f, 0.0f};

	// последнее положение камеры нужно для планет
	sVECTOR3D	LastCameraPoint{0.0f, 0.0f, 0.0f};

	// эффекты
	std::vector<sVECTOR3D> GraphicFXLocation{};
	std::vector<std::weak_ptr<cParticleSystem>> GraphicFX{};

	// чтобы возрвать часть корабля босса пришельцев через время
	float		BossPartCountDown{-1.0f};

	// для собственного списка
	cSpaceObject	*Next{nullptr};
	cSpaceObject	*Prev{nullptr};
};


// установка эффекта, если нужно
void SetSpaceObjectGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXType);


//-----------------------------------------------------------------------------
// Менеджер SpaceObject, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachSpaceObject(cSpaceObject* SpaceObject);
// Исключаем из списка
void	DetachSpaceObject(cSpaceObject* SpaceObject);
// Проверяем все объекты, обновляем данные
void	UpdateAllSpaceObject(float Time);
// Прорисовываем все объекты
void	DrawAllSpaceObject(bool VertexOnlyPass, unsigned int ShadowMap);
// Удаляем все объекты в списке
void	ReleaseAllSpaceObject();

#endif // SPACEOBJECT_H
