/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

#ifndef CORE_PLATFORM_PLATFORM_H
#define CORE_PLATFORM_PLATFORM_H

#include "../base.h"

namespace viewizard {

struct sRECT;

/*
 * Keyboard.
 */

// Get key status (pressed or not).
bool vw_GetKeyStatus(int Key);
// Set key status (pressed or released).
void vw_SetKeyStatus(int Key, bool NewKeyStatus);
// Key status update. Should be called on SDL_KEYUP event.
void vw_KeyStatusUpdate(int Key);
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
// Set mouse button status.
void vw_SetMouseButtonStatus(unsigned Button, bool NewStatus);
// Get mouse button status.
bool vw_GetMouseButtonStatus(unsigned Button);
// Get maximum mouse button number.
unsigned vw_GetMaxMouseButtonNum();
// Reset mouse buttons.
void vw_ResetMouseButtons();
// Set left mouse button double click status.
void vw_SetMouseLeftDoubleClick(bool NewStatus);
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

/*
 * Open URL.
 */

// Open website URL.
// Note, this is wrapper for ASCII and ANSI functions, don't use URL with Unicode.
bool vw_OpenWebsiteURL(const std::string &WebsiteURL);

} // viewizard namespace

#endif // CORE_PLATFORM_PLATFORM_H
