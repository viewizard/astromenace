/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Explosion.h

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
void	DrawAllExplosion();
// Удаляем все объекты в списке
void	ReleaseAllExplosion();




//-----------------------------------------------------------------------------
// Дополнительные функции
//-----------------------------------------------------------------------------
void SetExplosionGFX(eParticleSystem *ParticleSystem, int GFXNum);






#endif // EXPLOSION_H
