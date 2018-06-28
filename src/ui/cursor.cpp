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
#include "../assets/texture.h"
#include "../enum.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

namespace viewizard {

namespace {

float CurrentCursorFlash{1.0f};
float CurrentCursorFlashLastTime{-1.0f};

} // unnamed namespace


// FIXME should be fixed, don't allow global scope interaction for local variables
// текущий цвет 0-зеленый, 1-можно на нажимать, 2-нельзя нажимать
int CurrentCursorStatus;
bool DrawGameCursor = true;
extern bool DragWeapon;
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
	CurrentCursorStatus = 0;
	if (CurrentCursorFlashLastTime == -1.0f) {
		CurrentCursorFlashLastTime = vw_GetTimeThread(0);
	} else {
		CurrentCursorFlash -= vw_GetTimeThread(0) - CurrentCursorFlashLastTime;
		if (CurrentCursorFlash < 0.3f)
			CurrentCursorFlash = 1.0f;
		CurrentCursorFlashLastTime = vw_GetTimeThread(0);
	}
}

// вызываем прямо перед прорисовкой курсора
static void DrawDragingWeaponIcon(int X, int Y)
{
	// если не тянем - соотв. и не рисуем
	if (!DragWeapon) return;

	// в х и у - положение точки курсора
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

	int mX, mY;
	vw_GetMousePos(mX, mY);

	// если нужно, рисуем перетягиваемое оружие
	if (MenuStatus == eMenuStatus::WORKSHOP)
		DrawDragingWeaponIcon(mX, mY);

	sRECT SrcRect(0, 0, 64, 64);
	sRECT DstRect(mX-12, mY-13, mX+64-12, mY+64-13);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor_shadow.tga"), true, 1.0f);
	switch (CurrentCursorStatus) {
	case 0:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"), true, 0.80f, 0.0f, sRGBCOLOR{0.8f, 0.7f, 0.0f});
		break;

	case 1:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"), true, CurrentCursorFlash, 0.0f, sRGBCOLOR{0.0f, 1.0f, 0.0f});
		break;

	case 2:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"), true, CurrentCursorFlash, 0.0f, sRGBCOLOR{1.0f, 0.2f, 0.0f});
		break;

	case 3:
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/cursor.tga"), true, CurrentCursorFlash, 0.0f, sRGBCOLOR{0.8f, 0.7f, 0.0f});
		break;
	}
}

} // viewizard namespace
