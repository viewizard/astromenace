/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: SpaceStars.h

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


#ifndef SPACESTARS_H
#define SPACESTARS_H


#include "Star.h"




class CSpaceStars
{
public:
	CSpaceStars();
	~CSpaceStars();

	// обновить все частицы в этой системе, по времени
	virtual		bool Update(float Time);
	// прорисовка всех частиц
	virtual		void Draw();
	// базовая текстура частиц
	eTexture	*Texture;

	// текущее положение частиц в пространстве
	VECTOR3D		Location;

	// размер частиц в мировых координатах
	float Size;


	VECTOR3D	CreationSize;


	// если нужно - корректировка размера частицы при создании относительно камеры
	// мертвая зона (радиус, где вообще не рисуем)
	float	DeadZone;


	// возраст системы в секундах
	float Age;

	// последнее время обновления системы
	float TimeLastUpdate;


	// матрицы поворота, для оптимизации просчетов
	float	CurrentRotationMat[9];
	float	OldInvRotationMat[9];


	// подвязка к динамическому массиву
	CStar * Start;
	CStar * End;
	void Attach(CStar * NewParticle);
	void Detach(CStar * OldParticle);


	// данные, храним их, чтобы каждый раз не пересчитывать
	CStar **list;
	float *tmpDATA;
	unsigned int *VBO; // указатель на номер VBO массива
	int PrimitCount;
	float LastCameraAngleX, LastCameraAngleY, LastCameraAngleZ;
};










#endif //SPACESTARS_H
