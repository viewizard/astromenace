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

#ifndef CoreSystem_H
#define CoreSystem_H

#include "../base.h"

struct sRECT;

/*
 * Time threads.
 */

// Initialize or re-initialize time for particular thread.
void vw_InitTimeThread(int TimeThread);
// Get time for particular thread.
float vw_GetTimeThread(int TimeThread);
// Start all time threads.
void vw_StartTimeThreads();
// Stop all time threads.
void vw_StopTimeThreads();
// Set time thread speed for particular thread.
void vw_SetTimeThreadSpeed(int TimeThread, float NewSpeed = 1.0f);
// Get time thread speed for particular thread.
float vw_GetTimeThreadSpeed(int TimeThread);

/*
 * Keyboard.
 */

// Get key status (pressed or not).
bool vw_GetKeyStatus(int Key);
// Set key status (pressed or released).
void vw_SetKeyStatus(int Key, bool NewKeyStatus);
// Get libSDL2 keystate array size.
int vw_GetKeyStateArraySize();
// Set current unicode (UTF8) character.
void vw_SetCurrentUnicodeChar(const char *NewUnicodeChar);
// Get current unicode (UTF32) character.
const std::u32string &vw_GetCurrentUnicodeChar();

/*
 * Mouse.
 */

// Get left mouse button status.
bool vw_GetMouseLeftClick(bool ResetStatus);
// Get right mouse button status.
bool vw_GetMouseRightClick(bool ResetStatus);
// Set left mouse button status.
void vw_SetMouseLeftClick(bool NewStatus);
// Set right mouse button status.
void vw_SetMouseRightClick(bool NewStatus);
// Get left mouse button double click status.
bool vw_GetMouseLeftDoubleClick(bool ResetStatus);
// Change mouse wheel status.
void vw_ChangeWheelStatus(int Value);
// Reset mouse wheel status.
void vw_ResetWheelStatus();
// Get mouse wheel status.
int vw_GetWheelStatus();
// Set mouse position in absolute coordinates.
void vw_SetMousePos(int X, int Y);
// Set mouse position in relative coordinates.
void vw_SetMousePosRel(int X, int Y);
// Get mouse position.
void vw_GetMousePos(int &X, int &Y);
// Check mouse position over rectangle.
bool vw_MouseOverRect(const sRECT &MDetect);

#endif // CoreSystem_H
