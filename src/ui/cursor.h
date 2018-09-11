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

#ifndef UI_CURSOR_H
#define UI_CURSOR_H

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eCursorStatus {
	Undefined,
	ActionAllowed,
	ActionProhibited,
	DraggingItem // should be used with SetCursorDraggingItemIcon()
};

// Cursor initialization.
// Should be called after vw_InitTimeThread(0) and LoadAllGameAssets().
void CursorInit(bool ShowSystemCursor);
// Release cursor.
void CursorRelease();
// Update cursor.
void CursorUpdate();
// Draw cursor.
void CursorDraw();
// Set cursor status.
void SetCursorStatus(eCursorStatus Status);
// Get cursor status.
eCursorStatus GetCursorStatus();
// Set cursor dragging item icon texture.
void SetCursorDraggingItemIcon(GLtexture Icon);
// Тoggle whether or not the cursor is shown.
void SetShowGameCursor(bool Toggle);
// Get is cursor shown or not.
bool GetShowGameCursor();

} // astromenace namespace
} // viewizard namespace

#endif // UI_CURSOR_H
