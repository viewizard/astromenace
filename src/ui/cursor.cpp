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

// TODO translate comments

/*

Note, this code should not interact with mouse, caller should
care about all mouse interaction.
This code should care about cursor draw only, that mean cursor
itself and drag object (if drag and drop are used).

*/

#include "../core/core.h"
#include "cursor.h"
#include "../assets/texture.h"
#include "../enum.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

eCursorStatus CursorStatus{eCursorStatus::Undefined};
float CursorBlinking{1.0f};
float CursorBlinkingLastTime{-1.0f};

} // unnamed namespace


// FIXME should be fixed, don't allow global scope interaction for local variables
bool DrawGameCursor = true;
extern int DragWeaponNum;


/*
 *
 */
void CursorInit(bool ShowSystemCursor)
{
	if (!ShowSystemCursor)
		SDL_ShowCursor(SDL_DISABLE);
}

/*
 *
 */
void CursorRelease()
{
	if (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)
		SDL_ShowCursor(SDL_ENABLE);
}

/*
 *
 */
void CursorUpdate()
{
	SetCursorStatus(eCursorStatus::Undefined);
	if (CursorBlinkingLastTime == -1.0f) {
		CursorBlinkingLastTime = vw_GetTimeThread(0);
	} else {
		CursorBlinking -= vw_GetTimeThread(0) - CursorBlinkingLastTime;
		if (CursorBlinking < 0.3f)
			CursorBlinking = 1.0f;
		CursorBlinkingLastTime = vw_GetTimeThread(0);
	}
}

/*
 *
 */
static void DrawDragingWeaponIcon(int X, int Y)
{
	sRECT SrcRect(0, 0, 128, 64);
	sRECT DstRect(X - 64, Y - 32, X + 64, Y + 32);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(GetWeaponIconName(DragWeaponNum)), true, MenuContentTransp);
}

/*
 *
 */
void CursorDraw()
{
	if (!DrawGameCursor)
		return;

	int MouseX, MouseY;
	vw_GetMousePos(MouseX, MouseY);

	sRECT SrcRect(0, 0, 64, 64);
	sRECT DstRect(MouseX-12, MouseY-13, MouseX+64-12, MouseY+64-13);

	switch (CursorStatus) {
	case eCursorStatus::Undefined:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor_shadow.tga"), true, 1.0f);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"),
			  true, 0.80f, 0.0f, sRGBCOLOR{0.8f, 0.7f, 0.0f});
		break;

	case eCursorStatus::ActionAllowed:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor_shadow.tga"), true, 1.0f);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"),
			  true, CursorBlinking, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
		break;

	case eCursorStatus::ActionProhibited:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor_shadow.tga"), true, 1.0f);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"),
			  true, CursorBlinking, 0.0f, sRGBCOLOR{1.0f, 0.2f, 0.0f});
		break;

	case eCursorStatus::DraggingItem:
		DrawDragingWeaponIcon(MouseX, MouseY);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor_shadow.tga"), true, 1.0f);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"),
			  true, CursorBlinking, 0.0f, sRGBCOLOR{0.0f, 0.8f, 0.7f});
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

} // astromenace namespace
} // viewizard namespace
