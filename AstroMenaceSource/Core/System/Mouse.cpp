/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Mouse.cpp

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


#include "System.h"
#include "../RendererInterface/RendererInterface.h"





bool wRBMouse = false;
bool wLBDoubleMouse = false;
bool wLBMouse = false;
int MouseX = 0;
int MouseY = 0;



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
	if (!NewStatus)
	{
		wLBDoubleMouse = NewStatus;
		LastTimeLBDoubleMouse  = -1;
		LastTimeLBDoubleMouseX  = -1;
		LastTimeLBDoubleMouseY  = -1;
	}
	else
	{
		// если на том же месте, и время не вышло - ставим 2-й клик
		int X, Y;
		vw_GetMousePos(&X, &Y);
		float Time = vw_GetTime();

		if ( LastTimeLBDoubleMouseX == X && LastTimeLBDoubleMouseY == Y &&
			Time < LastTimeLBDoubleMouse+0.5f)
		{
			wLBDoubleMouse = true;
			// на тот случае, если кликнем 3 и более раз
			LastTimeLBDoubleMouse = Time;
		}
		else
		{
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
	if (wLBDoubleMouse)
	{
		LastTimeLBDoubleMouseX = -1;
		LastTimeLBDoubleMouseY = -1;
	}
	if (ResetStatus) wLBDoubleMouse = false;
	return tmp;
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
	vw_GetViewport(0, 0, &W, &H);

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
	vw_GetViewport(0, 0, &W, &H);
	float AWw = W*1.0f;
	float AHw = H*1.0f;

	float ARWidth;
	float ARHeight;
	bool ARFLAG = vw_GetAspectWH(&ARWidth, &ARHeight);

	*X = MouseX;
	*Y = MouseY;

	if (ARFLAG)
	{
		*X = (int)(MouseX * (ARWidth/AWw));
		*Y = (int)(MouseY * (ARHeight/AHw));
	}

	return 0;
}






//------------------------------------------------------------------------------------
// находится мышка в области или нет
//------------------------------------------------------------------------------------
bool vw_OnRect(RECT *MDetect)
{
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	if  (((MDetect->right  >= MouseX)&
		(MDetect->left<= MouseX)&
		(MDetect->bottom >= MouseY)&
		(MDetect->top<= MouseY)))
	{
		return true;
	}

	return false;
}





