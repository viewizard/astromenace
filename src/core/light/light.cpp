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


#include "light.h"
#include "../graphics/graphics.h"




//-----------------------------------------------------------------------------
// начальная инициализация
//-----------------------------------------------------------------------------
sLight::sLight()
{
	vw_AttachLight(this);
}


//-----------------------------------------------------------------------------
// деструктор частицы
//-----------------------------------------------------------------------------
sLight::~sLight()
{
	vw_DetachLight(this);
}









//-----------------------------------------------------------------------------
// установка источника света
//-----------------------------------------------------------------------------
bool sLight::Activate(int CurrentLightNum, float Matrix[16])
{
	if (!On) return false;
	RealLightNum = CurrentLightNum;
	// установка данных

	vw_PushMatrix();
	vw_LoadIdentity();
	vw_SetMatrix(Matrix);

	if (LightType == 0) {
		float RenderDirection[4];
		float RenderLocation[4];

		RenderDirection[0] = -Direction.x;
		RenderDirection[1] = -Direction.y;
		RenderDirection[2] = -Direction.z;

		RenderLocation[0] = -Direction.x;
		RenderLocation[1] = -Direction.y;
		RenderLocation[2] = -Direction.z;
		RenderLocation[3] = 0.0f; // источник типа солнца

		// т.к. мы не используем "сброс" состояния источника в исходное - надо обязательно переустановить все
		vw_SetLightV(RealLightNum, RI_DIFFUSE, Diffuse);
		vw_SetLightV(RealLightNum, RI_SPECULAR, Specular);
		vw_SetLightV(RealLightNum, RI_AMBIENT, Ambient);
		vw_SetLightV(RealLightNum, RI_DIRECTION, RenderDirection);
		vw_SetLightV(RealLightNum, RI_POSITION, RenderLocation);
	} else {
		float RenderDirection[4];
		float RenderLocation[4];

		RenderDirection[0] = 0.0f;
		RenderDirection[1] = 0.0f;
		RenderDirection[2] = 0.0f;
		RenderDirection[3] = 0.0f;

		RenderLocation[0] = Location.x;
		RenderLocation[1] = Location.y;
		RenderLocation[2] = Location.z;
		RenderLocation[3] = 1.0f; // источник типа точки

		// т.к. мы не используем "сброс" состояния источника в исходное - надо обязательно переустановить все
		vw_SetLight(RealLightNum, RI_CONSTANT_ATTENUATION, ConstantAttenuation);
		vw_SetLight(RealLightNum, RI_LINEAR_ATTENUATION, LinearAttenuation);
		vw_SetLight(RealLightNum, RI_QUADRATIC_ATTENUATION, QuadraticAttenuation);

		vw_SetLightV(RealLightNum, RI_DIFFUSE, Diffuse);
		vw_SetLightV(RealLightNum, RI_SPECULAR, Specular);
		vw_SetLightV(RealLightNum, RI_AMBIENT, Ambient);
		vw_SetLightV(RealLightNum, RI_DIRECTION, RenderDirection);
		vw_SetLightV(RealLightNum, RI_POSITION, RenderLocation);
	}

	vw_LightEnable(RealLightNum, true);
	vw_PopMatrix();

	return true;
}



//-----------------------------------------------------------------------------
// выключить этот источник света
//-----------------------------------------------------------------------------
void sLight::DeActivate()
{
	if (!On) return;
	if (RealLightNum > -1) {
		vw_LightEnable(RealLightNum, false);
		RealLightNum = -1;
	}
}



//-----------------------------------------------------------------------------
// перенос источника (если это не направленный) в нужное место
//-----------------------------------------------------------------------------
void sLight::SetLocation(sVECTOR3D NewLocation)
{
	if (LightType != 0)
		Location = NewLocation;
}
