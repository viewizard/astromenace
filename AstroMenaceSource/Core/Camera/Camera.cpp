/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Camera.cpp

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


#include "Camera.h"
#include "../RendererInterface/RendererInterface.h"



//-----------------------------------------------------------------------------
// локальные/защищенные переменные
//-----------------------------------------------------------------------------

// флаг обновления фруструма
bool CameraUpdated = true;
// точка положения камеры
VECTOR3D	CameraLocation(0.0f, 0.0f, 0.0f);
// углы поворота камеры
VECTOR3D	CameraRotation(0.0f, 0.0f, 0.0f);
// девиация камеры
VECTOR3D	CameraDeviation(0.0f, 0.0f, 0.0f);





//-----------------------------------------------------------------------------
// получение-запись положения и поворота камеры
//-----------------------------------------------------------------------------
void vw_SetCameraLocation(VECTOR3D NewLocation)
{
	CameraLocation = NewLocation;
	CameraUpdated = true;
}
void vw_IncCameraLocation(VECTOR3D IncLocation)
{
	CameraLocation += IncLocation;
	CameraUpdated = true;
}
void vw_GetCameraLocation(VECTOR3D *CurrentLocation)
{
	*CurrentLocation = CameraLocation;
}
void vw_SetCameraRotation(VECTOR3D NewRotation)
{
	CameraRotation = NewRotation;
	CameraUpdated = true;
}
void vw_GetCameraRotation(VECTOR3D *CurrentRotation)
{
	*CurrentRotation = CameraRotation;
}




//-----------------------------------------------------------------------------
// движение камеры
//-----------------------------------------------------------------------------
void vw_SetCameraMove(VECTOR3D NewRotation, float ChangeDistance, VECTOR3D Point)
{
	// убираем поправку на точку
	CameraLocation -= Point;
	// обратный поворот
	RotatePointInv(&CameraLocation, CameraRotation);
	// делаем приращение
	CameraLocation.z += ChangeDistance;

	// присваеваем новые значения
	CameraRotation += NewRotation;
	// поворачиваем в нужную сторону
	RotatePoint(&CameraLocation, CameraRotation^(-1.0f));
	// добавляем поправку на точку
	CameraLocation += Point;

	// нужно обновить данные
	CameraUpdated = true;
}







//-----------------------------------------------------------------------------
// движение камеры вокруг точки
//-----------------------------------------------------------------------------
void vw_SetCameraMoveAroundPoint(VECTOR3D Point, float ChangeDistance, VECTOR3D ChangeRotation)
{
	// двигаем и перемещаем как нужно
	vw_SetCameraMove(ChangeRotation, ChangeDistance, Point);


	// установка углов, чтобы поворот был на нужную точку
	VECTOR3D exV(0.0f,0.0f,0.0f);
	VECTOR3D V = Point-CameraLocation;
	V.Normalize();
	if (V.x*V.x+V.y*V.y+V.z*V.z !=0) exV = V;

	// установка поворота на точку...
	float newrotY = 0.0f;
	if (exV.z != 0.0f)
	{
		if (((0.0f>exV.x)&(0.0f>-exV.z))|((0.0f<exV.x)&(0.0f<-exV.z)))
			newrotY=(atanf(fabsf(exV.x)/fabsf(exV.z)))/0.0174532925f;
		else
			newrotY=-(atanf(fabsf(exV.x)/fabsf(exV.z)))/0.0174532925f;
	}

	float newrotX = 0.0f;
	float kat=(exV.z)*(exV.z)+(exV.x)*(exV.x);
	if (kat != 0.0f)
	{
		kat=sqrtf(kat);
		if (0<exV.y)
			newrotX=-(atanf(fabsf(exV.y)/kat))/0.0174532925f;
		else
			newrotX=(atanf(fabsf(exV.y)/kat))/0.0174532925f;
	}
	if (0>-exV.z) newrotY+=180;


	CameraRotation.x = -newrotX;
	CameraRotation.y = -newrotY;

	CameraUpdated = true;
}




//-----------------------------------------------------------------------------
// установка болтания камеры
//-----------------------------------------------------------------------------
void vw_SetCameraDeviation(VECTOR3D NewCameraDeviation)
{
	CameraDeviation = NewCameraDeviation;
}



//-----------------------------------------------------------------------------
// установка камеры
//-----------------------------------------------------------------------------
void vw_CameraLookAt(void)
{
	// установка камеры
	vw_Rotate(-CameraRotation.x, 1.0f, 0.0f, 0.0f);
	vw_Rotate(-CameraRotation.y, 0.0f, 1.0f, 0.0f);
	vw_Rotate(-CameraRotation.z, 0.0f, 0.0f, 1.0f);

	vw_Translate((CameraLocation^(-1.0f))-CameraDeviation);

	// получаем фруструм
	if (CameraUpdated)
	{
		vw_CalculateFrustum();
		CameraUpdated = false;
	}
}
