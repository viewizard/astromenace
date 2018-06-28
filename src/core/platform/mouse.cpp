/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../graphics/graphics.h"
#include "../math/math.h"

namespace viewizard {

namespace {

// right mouse button status
bool MouseRightClick{false};
// left mouse button status
bool MouseLeftClick{false};
// mouse button status array
// SDL use Uint8 value type for mouse button status, that starts from 1,
// so, this could be up to 255 buttons (in future)
std::vector<bool> MouseButtonArray{};

// X mouse pointer position
int MouseX{0};
// Y mouse pointer position
int MouseY{0};
// wheel status
int MouseWheelStatus{0};
// left mouse button double click status
bool MouseLeftDoubleClick{false};

} // unnamed namespace


/*
 * Get mouse position.
 */
void vw_GetMousePos(int &X, int &Y)
{
	float tmpViewportWidth, tmpViewportHeight;
	vw_GetViewport(nullptr, nullptr, &tmpViewportWidth, &tmpViewportHeight);

	float tmpInternalWidth, tmpInternalHeight;
	if (vw_GetInternalResolution(&tmpInternalWidth, &tmpInternalHeight)) {
		// WARNING probably, we could move mouse position's vatiables to float type (blocked by sRECT)
		X = static_cast<int>(static_cast<float>(MouseX) * tmpInternalWidth / tmpViewportWidth);
		Y = static_cast<int>(static_cast<float>(MouseY) * tmpInternalHeight / tmpViewportHeight);
	} else {
		X = MouseX;
		Y = MouseY;
	}
}

/*
 * Set left mouse button double click status.
 */
void vw_SetMouseLeftDoubleClick(bool NewStatus)
{
	MouseLeftDoubleClick = NewStatus;
}

/*
 * Get left mouse button double click status.
 */
bool vw_GetMouseLeftDoubleClick(bool ResetStatus)
{
	bool tmp = MouseLeftDoubleClick;
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
 * Set mouse button status.
 */
void vw_SetMouseButtonStatus(unsigned Button, bool NewStatus)
{
	assert(Button);

	// note, SDL provide button's number that starts from 1
	if (Button > MouseButtonArray.size())
		MouseButtonArray.resize(Button, false);

	MouseButtonArray[Button - 1] = NewStatus;
}

/*
 * Get mouse button status.
 */
bool vw_GetMouseButtonStatus(unsigned Button)
{
	assert(Button);

	// note, SDL provide button's number that starts from 1
	if (Button > MouseButtonArray.size())
		MouseButtonArray.resize(Button, false);

	return MouseButtonArray[Button - 1];
}

/*
 * Get maximum mouse button number.
 */
unsigned vw_GetMaxMouseButtonNum()
{
	return static_cast<unsigned>(MouseButtonArray.size());
}

/*
 * Reset mouse buttons.
 */
void vw_ResetMouseButtons()
{
	std::fill(MouseButtonArray.begin(), MouseButtonArray.end(), false);
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
	MouseX += X;
	MouseY += Y;

	int tmpViewportWidth, tmpViewportHeight;
	vw_GetViewport(nullptr, nullptr, &tmpViewportWidth, &tmpViewportHeight);

	vw_Clamp(MouseX, 0, tmpViewportWidth);
	vw_Clamp(MouseY, 0, tmpViewportHeight);
}

/*
 * Set mouse position in absolute coordinates.
 */
void vw_SetMousePos(int X, int Y)
{
	MouseX = X;
	MouseY = Y;
}

/*
 * Check mouse position over rectangle.
 */
bool vw_MouseOverRect(const sRECT &MDetect)
{
	int tmpMouseX, tmpMouseY;
	vw_GetMousePos(tmpMouseX, tmpMouseY);

	if  ((MDetect.right >= tmpMouseX) &&
	     (MDetect.left <= tmpMouseX) &&
	     (MDetect.bottom >= tmpMouseY) &&
	     (MDetect.top <= tmpMouseY))
		return true;

	return false;
}

} // viewizard namespace
