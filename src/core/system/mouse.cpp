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

namespace {

// right mouse button status
bool MouseRightClick{false};
// left mouse button status
bool MouseLeftClick{false};

// X mouse pointer position
int MouseX{0};
// Y mouse pointer position
int MouseY{0};
// wheel status
int MouseWheelStatus{0};

// left mouse button double click status
bool MouseLeftDoubleClick{false};
// left mouse button double click time interval
constexpr uint32_t DoubleClickTimeInterval{500}; // 500 ms
// timer for hold double click status
uint32_t LastTimeLeftDoubleClick{0};
// X double click position
int LastTimeLeftDoubleClickX{-1};
// Y double click position
int LastTimeLeftDoubleClickY{-1};

} // unnamed namespace


/*
 * Get mouse position.
 */
void vw_GetMousePos(int &X, int &Y)
{
	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);

	float ARWidth, ARHeight;
	bool ARFLAG = vw_GetAspectWH(&ARWidth, &ARHeight);

	if (ARFLAG) {
		X = (int)(MouseX * ARWidth * 1.0f / W);
		Y = (int)(MouseY * ARHeight * 1.0f / H);
	} else {
		X = MouseX;
		Y = MouseY;
	}
}

/*
 * Set left mouse button double click status.
 */
static void SetMouseLeftDoubleClick()
{
	int X, Y;
	vw_GetMousePos(X, Y);
	uint32_t CurrentTicks = SDL_GetTicks();

	if ((LastTimeLeftDoubleClickX == X) &&
	    (LastTimeLeftDoubleClickY == Y) &&
	    (CurrentTicks < LastTimeLeftDoubleClick + DoubleClickTimeInterval)) {
		MouseLeftDoubleClick = true;
		LastTimeLeftDoubleClick = CurrentTicks;
	} else {
		LastTimeLeftDoubleClickX = X;
		LastTimeLeftDoubleClickY = Y;
		LastTimeLeftDoubleClick = CurrentTicks;
		MouseLeftDoubleClick = false;
	}
}

/*
 * Get left mouse button double click status.
 */
bool vw_GetMouseLeftDoubleClick(bool ResetStatus)
{
	bool tmp = MouseLeftDoubleClick;
	if (MouseLeftDoubleClick) {
		LastTimeLeftDoubleClick  = 0;
		LastTimeLeftDoubleClickX = -1;
		LastTimeLeftDoubleClickY = -1;
	}
	if (ResetStatus)
		MouseLeftDoubleClick = false;

	return tmp;
}

/*
 * Set left mouse button status.
 */
void vw_SetMouseLeftClick(bool NewStatus)
{
	MouseLeftClick = NewStatus;
	if (NewStatus)
		SetMouseLeftDoubleClick();
}

/*
 * Get left mouse button status.
 */
bool vw_GetMouseLeftClick(bool ResetStatus)
{
	bool tmp = MouseLeftClick;
	if (ResetStatus)
		MouseLeftClick = false;
	return tmp;
}

/*
 * Set right mouse button status.
 */
void vw_SetMouseRightClick(bool NewStatus)
{
	MouseRightClick = NewStatus;
}

/*
 * Get right mouse button status.
 */
bool vw_GetMouseRightClick(bool ResetStatus)
{
	bool tmp = MouseRightClick;
	if (ResetStatus)
		MouseRightClick = false;
	return tmp;
}

/*
 * Change mouse wheel status.
 */
void vw_ChangeWheelStatus(int Value)
{
	MouseWheelStatus += Value;
}

/*
 * Reset mouse wheel status.
 */
void vw_ResetWheelStatus()
{
	MouseWheelStatus = 0;
}

/*
 * Get mouse wheel status.
 */
int vw_GetWheelStatus()
{
	return MouseWheelStatus;
}

/*
 * Set mouse position in relative coordinates.
 */
void vw_SetMousePosRel(int X, int Y)
{
	if (X || Y)
		MouseLeftDoubleClick = false;

	MouseX += X;
	MouseY += Y;

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);

	vw_Clamp(MouseX, 0, W);
	vw_Clamp(MouseY, 0, H);
}

/*
 * Set mouse position in absolute coordinates.
 */
void vw_SetMousePos(int X, int Y)
{
	if ((MouseX != X) || (MouseY != Y))
		MouseLeftDoubleClick = false;

	MouseX = X;
	MouseY = Y;
}

/*
 * Check mouse position over rectangle.
 */
bool vw_MouseOverRect(const sRECT &MDetect)
{
	int MouseX, MouseY;
	vw_GetMousePos(MouseX, MouseY);

	if  ((MDetect.right >= MouseX) &&
	     (MDetect.left <= MouseX) &&
	     (MDetect.bottom >= MouseY) &&
	     (MDetect.top <= MouseY))
		return true;

	return false;
}
