/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Light.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#ifndef LIGHT_H
#define LIGHT_H


#include "../Base.h"
#include "../Math/Math.h"



//-----------------------------------------------------------------------------
// Класс eLight
//-----------------------------------------------------------------------------
struct eLight
{

	// конструктор и деструктор
	eLight();
	~eLight();



	// установка источника света
	bool		Activate(int CurrentLightNum, float Matrix[16]);
	// выключить этот источник света
	void		DeActivate();
	// перенос источника (если это не направленный) в нужное место
	void		SetLocation(VECTOR3D NewLocation);


	// все характеристики источника света

	// тип 1-солнце, 2- точка, по умю =1
	int			LightType;
	// цвет испускаемый источникос света
	float		Diffuse[4];
	float		Specular[4];
	float		Ambient[4];
	// максимальный цвет (нужен для просчета девиации источника)
	float		DiffuseMax[4];
	float		SpecularMax[4];

	// коэф. затухания
	float		ConstantAttenuation;
	float		LinearAttenuation;
	float		LinearAttenuationBase;
	float		QuadraticAttenuation;
	float		QuadraticAttenuationBase;
	// временный показатель, нужен для поиска по воздействию
	float		tmpAttenuation;


	// направление
	VECTOR3D	Direction;

	// положение в мировых координатах, если нужно (не направленный свет)
	VECTOR3D	Location;




	// включен-выключен источник света
	bool		On;

	// задействован в прорисовке модели или нет + номер реального источника если задействован
	// для того, чтобы потом его выключить
	int			RealLightNum;


	// указатели на цепь источников света
	eLight *Next;
	eLight *Prev;
};






//-----------------------------------------------------------------------------
// Менеджер eLight
//-----------------------------------------------------------------------------

// Включаем в список
void	vw_AttachLight(eLight * NewLight);
// Исключаем из списка
void	vw_DetachLight(eLight * OldLight);
// включаем нужные источники света + освещение
int		vw_CheckAndActivateAllLights(int *Type1, int *Type2, VECTOR3D Location, float Radius2, int DirLimit, int PointLimit, float Matrix[16]);
// находим кол-во точечных источников
int		vw_CheckAllPointLights(VECTOR3D Location, float Radius2);
// включаем источники и освещение
void	vw_DeActivateAllLights();
// Удаляем все источники в списке
void	vw_ReleaseAllLights();


// Создаем точечный источник света
eLight 	*vw_CreatPointLight(VECTOR3D Location, float R, float G, float B, float Linear, float Quadratic);
// Удаляем источник света
void 	vw_ReleaseLight(eLight *Light);



#endif // LIGHT_H

