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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef OBJECT3D_EXPLOSION_EXPLOSION_H
#define OBJECT3D_EXPLOSION_EXPLOSION_H

#include "../object3d.h"
#include "../projectile/projectile.h"





//-----------------------------------------------------------------------------
// Структура sExplosionPiece
//-----------------------------------------------------------------------------
struct sExplosionPiece {
	// вектор направления движения частицы (скорости включены)
	sVECTOR3D	Velocity;
	// оставшееся время жизни частицы
	float		Life;
};








//-----------------------------------------------------------------------------
// Класс cExplosion
//-----------------------------------------------------------------------------
class cExplosion : public cObject3D
{
public:
	// базовые конструктор и деструктор объекта
	cExplosion();
	virtual ~cExplosion();

	// Обновление данных объектa
	virtual bool	Update(float Time) override;

	int		ExplosionType{0};
	int		ExplosionTypeByClass{0};

	// набор управления частицами
	sExplosionPiece	*ExplosionPieceData{nullptr};

	float	ExplosionGeometryMoveLastTime{-1}; // последнее время изменения геометрии, нет смысла постоянно менять геометрию, делаем это 30 раз в секунду только

	sVECTOR3D	VelocityOrientation{0.0f, 0.0f, 0.0f};
	float		OldSpeed{0.0f};
	bool		NeedStop{true};

	// скорость изменения AABB
	float		AABBSpeed{0.0f};

	// для прорисовки графических эффектов
	std::vector<std::weak_ptr<cParticleSystem>> GraphicFX{};

	// для собственного списка
	cExplosion	*Next{nullptr};
	cExplosion	*Prev{nullptr};
};









//-----------------------------------------------------------------------------
// Менеджер Explosion, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachExplosion(cExplosion* Explosion);
// Исключаем из списка
void	DetachExplosion(cExplosion* Explosion);
// Проверяем все объекты, обновляем данные
void	UpdateAllExplosion(float Time);
// Прорисовываем все объекты
void	DrawAllExplosion(bool VertexOnlyPass);
// Удаляем все объекты в списке
void	ReleaseAllExplosion();




//-----------------------------------------------------------------------------
// Дополнительные функции
//-----------------------------------------------------------------------------
void SetExplosionGFX(std::shared_ptr<cParticleSystem> &ParticleSystem, int GFXNum);






#endif // OBJECT3D_EXPLOSION_EXPLOSION_H
