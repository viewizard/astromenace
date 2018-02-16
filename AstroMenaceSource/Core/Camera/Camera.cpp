/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
// точка на которую смотрим
VECTOR3D	CameraFocusPoint(0.0f, 0.0f, 0.0f);



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
	CameraFocusPoint += IncLocation;
	CameraUpdated = true;
}
VECTOR3D vw_GetCameraLocation(VECTOR3D *CurrentLocation)
{
	*CurrentLocation = CameraLocation;
	return CameraLocation;
}
void vw_SetCameraRotation(VECTOR3D NewRotation)
{
	CameraRotation = NewRotation;
	CameraUpdated = true;
}
VECTOR3D vw_GetCameraRotation(VECTOR3D *CurrentRotation)
{
	*CurrentRotation = CameraRotation;
	return CameraRotation;
}
VECTOR3D vw_GetCameraFocusPoint()
{
	return CameraFocusPoint;
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
	CameraFocusPoint = Point;

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
