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

#include "../graphics/graphics.h"
#include "../math/math.h"
#include "system.h"




bool wRBMouse = false;
bool wLBDoubleMouse = false;
bool wLBMouse = false;
int MouseX = 0;
int MouseY = 0;
int MouseWheelStatus = 0;


//------------------------------------------------------------------------------------
// установка-получение статуса левой кнопки мышки
//------------------------------------------------------------------------------------
void vw_SetWindowLBMouse(bool NewStatus)
{
	wLBMouse = NewStatus;
}
bool vw_GetWindowLBMouse(bool ResetStatus)
{
	bool tmp = wLBMouse;
	if (ResetStatus) wLBMouse = false;
	return tmp;
}





//------------------------------------------------------------------------------------
// установка-получение статуса правой кнопки мышки
//------------------------------------------------------------------------------------
void vw_SetWindowRBMouse(bool NewStatus)
{
	wRBMouse = NewStatus;
}
bool vw_GetWindowRBMouse(bool ResetStatus)
{
	bool tmp = wRBMouse;
	if (ResetStatus) wRBMouse = false;
	return tmp;
}




//------------------------------------------------------------------------------------
// установка-получение статуса правой кнопки мышки - двойное нажатие
//------------------------------------------------------------------------------------
float LastTimeLBDoubleMouse  = -1;
int LastTimeLBDoubleMouseX  = -1;
int LastTimeLBDoubleMouseY  = -1;
void vw_SetWindowLBDoubleMouse(bool NewStatus)
{
	// сбрасываем просто, устанавливать - только с проверками
	if (!NewStatus) {
		wLBDoubleMouse = NewStatus;
		LastTimeLBDoubleMouse  = -1;
		LastTimeLBDoubleMouseX  = -1;
		LastTimeLBDoubleMouseY  = -1;
	} else {
		// если на том же месте, и время не вышло - ставим 2-й клик
		int X, Y;
		vw_GetMousePos(&X, &Y);
		float Time = vw_GetTime();

		if ( LastTimeLBDoubleMouseX == X && LastTimeLBDoubleMouseY == Y &&
		     Time < LastTimeLBDoubleMouse+0.5f) {
			wLBDoubleMouse = true;
			// на тот случае, если кликнем 3 и более раз
			LastTimeLBDoubleMouse = Time;
		} else {
			LastTimeLBDoubleMouseX = X;
			LastTimeLBDoubleMouseY = Y;
			LastTimeLBDoubleMouse = Time;
			wLBDoubleMouse = false;
		}
	}
}
bool vw_GetWindowLBDoubleMouse(bool ResetStatus)
{
	bool tmp = wLBDoubleMouse;
	// на тот случае, если кликнем 3 и более раз
	if (wLBDoubleMouse) {
		LastTimeLBDoubleMouseX = -1;
		LastTimeLBDoubleMouseY = -1;
	}
	if (ResetStatus) wLBDoubleMouse = false;
	return tmp;
}



void vw_ChangeWheelStatus(int Value)
{
	MouseWheelStatus += Value;
}
void vw_ResetWheelStatus()
{
	MouseWheelStatus = 0;
}
int vw_GetWheelStatus()
{
	return MouseWheelStatus;
}


//------------------------------------------------------------------------------------
// Получение данных положения курсора относительно клиентской области
//------------------------------------------------------------------------------------
void vw_SetMousePosRel(int X, int Y)
{
	// сдвинули
	if (0 != X || 0 != Y)
		wLBDoubleMouse = false;

	MouseX += X;
	MouseY += Y;

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);

	if (MouseX > W) MouseX = W;
	if (MouseY > H) MouseY = H;
	if (MouseX < 0) MouseX = 0;
	if (MouseY < 0) MouseY = 0;
}
void vw_SetMousePos(int X, int Y)
{
	// сдвинули
	if (MouseX != X || MouseY != Y)
		wLBDoubleMouse = false;

	MouseX = X;
	MouseY = Y;
}
int vw_GetMousePos(int *X, int *Y)
{
	// получаем размер клиентской области
	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AWw = W*1.0f;
	float AHw = H*1.0f;

	float ARWidth;
	float ARHeight;
	bool ARFLAG = vw_GetAspectWH(&ARWidth, &ARHeight);

	*X = MouseX;
	*Y = MouseY;

	if (ARFLAG) {
		*X = (int)(MouseX * (ARWidth/AWw));
		*Y = (int)(MouseY * (ARHeight/AHw));
	}

	return 0;
}






//------------------------------------------------------------------------------------
// находится мышка в области или нет
//------------------------------------------------------------------------------------
bool vw_OnRect(const sRECT &MDetect)
{
	int MouseXOnRect, MouseYOnRect;
	vw_GetMousePos(&MouseXOnRect, &MouseYOnRect);

	if  ((MDetect.right  >= MouseXOnRect) &&
	     (MDetect.left <= MouseXOnRect) &&
	     (MDetect.bottom >= MouseYOnRect) &&
	     (MDetect.top <= MouseYOnRect))
		return true;

	return false;
}
