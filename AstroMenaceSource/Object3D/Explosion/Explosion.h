/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#ifndef EXPLOSION_H
#define EXPLOSION_H


#include "../Object3D.h"
#include "../Projectile/Projectile.h"





//-----------------------------------------------------------------------------
// Структура CExplosionPiece
//-----------------------------------------------------------------------------
struct CExplosionPiece
{
	// вектор направления движения частицы (скорости включены)
	VECTOR3D	Velocity;
	// оставшееся время жизни частицы
	float		Life;
};








//-----------------------------------------------------------------------------
// Класс CExplosion
//-----------------------------------------------------------------------------
class CExplosion : public CObject3D
{
public:

	// базовые конструктор и деструктор объекта
	CExplosion();
	virtual ~CExplosion();

	// Обновление данных объектa
	virtual bool	Update(float Time);

	int		ExplosionType;
	int		ExplosionTypeByClass;


	// набор управления частицами
	CExplosionPiece	*ExplosionPieceData;


	float	ExplosionGeometryMoveLastTime;// последнее время изменения геометрии, нет смысла постоянно менять геометрию, делаем это 30 раз в секунду только



	VECTOR3D	VelocityOrientation;
	float		OldSpeed;
	bool		NeedStop;

	// скорость изменения AABB
	float AABBSpeed;


	// для прорисовки графических эффектов
	int					GraphicFXQuantity;
	eParticleSystem		**GraphicFX;


	// для собственного списка
	CExplosion* Next;
	CExplosion* Prev;
};









//-----------------------------------------------------------------------------
// Менеджер Explosion, дополнительный
//-----------------------------------------------------------------------------

// Включаем в список
void	AttachExplosion(CExplosion* Explosion);
// Исключаем из списка
void	DetachExplosion(CExplosion* Explosion);
// Проверяем все объекты, обновляем данные
void	UpdateAllExplosion(float Time);
// Прорисовываем все объекты
void	DrawAllExplosion(bool VertexOnlyPass);
// Удаляем все объекты в списке
void	ReleaseAllExplosion();




//-----------------------------------------------------------------------------
// Дополнительные функции
//-----------------------------------------------------------------------------
void SetExplosionGFX(eParticleSystem *ParticleSystem, int GFXNum);






#endif // EXPLOSION_H
