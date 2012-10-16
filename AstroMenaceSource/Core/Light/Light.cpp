/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Light.cpp

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


#include "Light.h"
#include "../RendererInterface/RendererInterface.h"




//-----------------------------------------------------------------------------
// начальная инициализация
//-----------------------------------------------------------------------------
eLight::eLight()
{
	// типа нет
	LightType = 1;
	// цвета нет
	Diffuse[0] = 0.0f;
	Diffuse[1] = 0.0f;
	Diffuse[2] = 0.0f;
	Diffuse[3] = 1.0f;
	Specular[0] = 0.0f;
	Specular[1] = 0.0f;
	Specular[2] = 0.0f;
	Specular[3] = 1.0f;
	Ambient[0] = 0.0f;
	Ambient[1] = 0.0f;
	Ambient[2] = 0.0f;
	Ambient[3] = 1.0f;

	ConstantAttenuation = 0.0f;
	LinearAttenuation = 0.0f;
	QuadraticAttenuation = 0.0f;
	tmpAttenuation = -1.0f;

	Direction = Location = VECTOR3D(0.0f, 0.0f, 0.0f);

	On = true;
	RealLightNum = -1;

	vw_AttachLight(this);
}


//-----------------------------------------------------------------------------
// деструктор частицы
//-----------------------------------------------------------------------------
eLight::~eLight()
{
	vw_DetachLight(this);
}









//-----------------------------------------------------------------------------
// установка источника света
//-----------------------------------------------------------------------------
bool eLight::Activate(int CurrentLightNum, float Matrix[16])
{
	if (!On) return false;
	RealLightNum = CurrentLightNum;
	// установка данных

	vw_PushMatrix();
	vw_LoadIdentity();
	vw_SetMatrix(Matrix);

	if (LightType == 0)
	{
		float RenderDirection[4];
		float RenderLocation[4];

		RenderDirection[0] = -Direction.x;
		RenderDirection[1] = -Direction.y;
		RenderDirection[2] = -Direction.z;

		RenderLocation[0] = -Direction.x;
		RenderLocation[1] = -Direction.y;
		RenderLocation[2] = -Direction.z;
		RenderLocation[3] = 0.0f; // источник типа солнца

		vw_SetLightV(RealLightNum, RI_DIFFUSE, Diffuse);
		vw_SetLightV(RealLightNum, RI_SPECULAR, Specular);
		vw_SetLightV(RealLightNum, RI_AMBIENT, Ambient);
		vw_SetLightV(RealLightNum, RI_DIRECTION, RenderDirection);
		vw_SetLightV(RealLightNum, RI_POSITION, RenderLocation);
	}
	else
	{
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
void eLight::DeActivate()
{
	if (!On) return;
	if (RealLightNum > -1)
	{
		// делаем сброс настроек источника, иначе может быть ерунда с освещением
		float off[4];
		off[0] = 0.0f;
		off[1] = 0.0f;
		off[2] = 0.0f;
		off[3] = 1.0f;
		vw_SetLightV(RealLightNum, RI_DIFFUSE, off);
		vw_SetLightV(RealLightNum, RI_SPECULAR, off);
		vw_SetLightV(RealLightNum, RI_AMBIENT, off);
		vw_SetLightV(RealLightNum, RI_DIRECTION, off);
		vw_SetLightV(RealLightNum, RI_POSITION, off);

		vw_LightEnable(RealLightNum, false);
		RealLightNum = -1;
	}
}



//-----------------------------------------------------------------------------
// перенос источника (если это не направленный) в нужное место
//-----------------------------------------------------------------------------
void eLight::SetLocation(VECTOR3D NewLocation)
{
	if (LightType != 0)
		Location = NewLocation;
}
