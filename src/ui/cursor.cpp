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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO move to SDL_GetTicks() usage

/*

Note, this code should not interact with mouse, caller should
care about all mouse interaction.
This code should care about cursor draw only, that mean cursor
itself and drag object (if drag and drop are used).

*/

#include "cursor.h"
#include "../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

bool ShowGameCursorStatus{true};

eCursorStatus CursorStatus{eCursorStatus::Undefined};
float CursorBlinking{1.0f};
float CursorBlinkingLastTime{0.0f};

GLtexture CursorFront{0};
GLtexture CursorShadow{0};
GLtexture DraggingItemIcon{0};

} // unnamed namespace


/*
 * Cursor initialization.
 * Should be called after vw_InitTimeThread(0) and LoadAllGameAssets().
 */
void CursorInit(bool ShowSystemCursor)
{
	if (!ShowSystemCursor)
		SDL_ShowCursor(SDL_DISABLE);

	CursorBlinkingLastTime = vw_GetTimeThread(0);

	CursorFront = GetPreloadedTextureAsset("menu/cursor.tga");
	CursorShadow = GetPreloadedTextureAsset("menu/cursor_shadow.tga");
}

/*
 * Release cursor.
 */
void CursorRelease()
{
	if (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)
		SDL_ShowCursor(SDL_ENABLE);
}

/*
 * Update cursor.
 */
void CursorUpdate()
{
	SetCursorStatus(eCursorStatus::Undefined);

	CursorBlinking -= vw_GetTimeThread(0) - CursorBlinkingLastTime;
	if (CursorBlinking < 0.3f)
		CursorBlinking = 1.0f;
	CursorBlinkingLastTime = vw_GetTimeThread(0);
}

/*
 * Draw dragging item icon.
 */
static void DrawDraggingItemIcon(int X, int Y)
{
	if (!DraggingItemIcon)
		return;

	// we use "fixed" icon size for now
	constexpr int IconWidth{128};
	constexpr int IconHeight{64};

	sRECT SrcRect(0, 0, IconWidth, IconHeight);
	sRECT DstRect(X - IconWidth / 2, Y - IconHeight / 2,
		      X + IconWidth / 2, Y + IconHeight / 2);

	vw_Draw2D(DstRect, SrcRect, DraggingItemIcon, true);
}

/*
 * Draw cursor.
 */
void CursorDraw()
{
	if (!ShowGameCursorStatus)
		return;

	int MouseX, MouseY;
	vw_GetMousePos(MouseX, MouseY);

	sRECT SrcRect(0, 0, 64, 64);
	sRECT DstRect(MouseX-12, MouseY-13, MouseX+64-12, MouseY+64-13);

	switch (CursorStatus) {
	case eCursorStatus::Undefined:
		vw_Draw2D(DstRect, SrcRect, CursorShadow, true);
		vw_Draw2D(DstRect, SrcRect, CursorFront, true, 0.80f, 0.0f, sRGBCOLOR{0.8f, 0.7f, 0.0f});
		break;

	case eCursorStatus::ActionAllowed:
		vw_Draw2D(DstRect, SrcRect, CursorShadow, true);
		vw_Draw2D(DstRect, SrcRect, CursorFront, true, CursorBlinking, 0.0f, sRGBCOLOR{eRGBCOLOR::green});
		break;

	case eCursorStatus::ActionProhibited:
		vw_Draw2D(DstRect, SrcRect, CursorShadow, true);
		vw_Draw2D(DstRect, SrcRect, CursorFront, true, CursorBlinking, 0.0f, sRGBCOLOR{1.0f, 0.2f, 0.0f});
		break;

	case eCursorStatus::DraggingItem:
		DrawDraggingItemIcon(MouseX, MouseY);
		vw_Draw2D(DstRect, SrcRect, CursorShadow, true);
		vw_Draw2D(DstRect, SrcRect, CursorFront, true, 0.80f, 0.0f, sRGBCOLOR{eRGBCOLOR::green});
		break;
	}
}

/*
 * Set cursor status.
 */
void SetCursorStatus(eCursorStatus Status)
{
	CursorStatus = Status;
}

/*
 * Get cursor status.
 */
eCursorStatus GetCursorStatus()
{
	return CursorStatus;
}

/*
 * Set cursor dragging item icon texture.
 */
void SetCursorDraggingItemIcon(GLtexture Icon)
{
	DraggingItemIcon = Icon;
}

/*
 * Тoggle whether or not the cursor is shown.
 */
void SetShowGameCursor(bool Toggle)
{
	ShowGameCursorStatus = Toggle;
}

/*
 * Get is cursor shown or not.
 */
bool GetShowGameCursor()
{
	return ShowGameCursorStatus;
}

} // astromenace namespace
} // viewizard namespace
