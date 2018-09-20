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

// TODO translate comments

// FIXME we should call GetPreloadedTextureAsset() here for MissionNumberTexture
//       on changed game language in game menu

#include "../../core/core.h"
#include "../../config/config.h"
#include "../../assets/texture.h"
#include "../../ui/cursor.h"
#include "../../command.h"
#include "../../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

float MissionNumberLifeTime{0.0f};
float MissionNumberLastUpdateTime{0.0f};
int MissionNumber{0};
GLtexture MissionNumberTexture{0};

float MissionFailedLifeTime{0.0f};
float LastMissionFailedUpdateTime{0.0f};

} // unnamed namespace


/*
 * Setup mission number text.
 */
void SetupMissionNumberText(float NotificationTime, int Number)
{
	MissionNumberLifeTime = NotificationTime;
	MissionNumberLastUpdateTime = vw_GetTimeThread(0);
	MissionNumber = Number;
	MissionNumberTexture = GetPreloadedTextureAsset(vw_GetText("lang/en/game/mission.tga"));
}

/*
 * Get image-related rectangle for particular number's symbol.
 */
static sRECT GetNumberOnImageRect(const char Symbol)
{
	switch (Symbol) {
	case '1':
		return sRECT{15, 1, 48, 63};
	case '2':
		return sRECT{48, 1, 86, 63};
	case '3':
		return sRECT{86, 1, 124, 63};
	case '4':
		return sRECT{124, 1, 160, 63};
	case '5':
		return sRECT{161, 1, 200, 63};
	case '6':
		return sRECT{200, 1, 238, 63};
	case '7':
		return sRECT{239, 1, 275, 63};
	case '8':
		return sRECT{276, 1, 315, 63};
	case '9':
		return sRECT{319, 1, 385, 63};
	case '0':
		return sRECT{360, 1, 399, 63};
	}

	return sRECT{0, 0, 0, 0};
}

/*
 * Calculate number string width.
 */
static int CalculateNumberStringWidth(const std::string &NumberString)
{
	int tmpWidth{0};

	for (const auto &Symbol : NumberString) {
		sRECT SrcRect = GetNumberOnImageRect(Symbol);
		tmpWidth += SrcRect.right - SrcRect.left;
	}

	return tmpWidth;
}

/*
 * Draw number string.
 */
static void DrawNumberString(int X, int Y, const std::string &NumberString, float Transp)
{
	// note, we use left-top as starting point (upper left is origin)
	int XStart = X;
	for (const auto &Symbol : NumberString) {
		sRECT SrcRect = GetNumberOnImageRect(Symbol);
		sRECT DstRect{XStart, Y,
			      XStart + (SrcRect.right - SrcRect.left), Y + (SrcRect.bottom - SrcRect.top)};
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("game/nums.tga"), true, Transp);
		XStart += SrcRect.right - SrcRect.left;
	}
}

/*
 * Draw mission number text.
 */
void DrawMissionNumberText()
{
	if (MissionNumberLifeTime <= 0.0f)
		return;

	float CurrentTime = vw_GetTimeThread(0);
	MissionNumberLifeTime -= CurrentTime - MissionNumberLastUpdateTime;
	MissionNumberLastUpdateTime = CurrentTime;

	sRECT SrcRect{0, 0, 226, 64}; // "Mission" image-related rectangle
	std::string NumberString{std::to_string(MissionNumber)};
	constexpr int tmpSpace{20};
	int tmpWidth = SrcRect.right + tmpSpace + CalculateNumberStringWidth(NumberString);

	int XStart = (GameConfig().InternalWidth - tmpWidth) / 2;
	constexpr int YStart{352};
	sRECT DstRect{XStart, YStart, XStart + SrcRect.right, YStart + SrcRect.bottom};

	float tmpTransp = (MissionNumberLifeTime > 1.0f) ? 1.0f : MissionNumberLifeTime;
	vw_Draw2D(DstRect, SrcRect, MissionNumberTexture, true, tmpTransp);
	DrawNumberString(XStart + SrcRect.right + tmpSpace, YStart, NumberString, tmpTransp);
}

//------------------------------------------------------------------------------------
// установка времени отображения ShowTime миссия провалена
//------------------------------------------------------------------------------------
void GameSetMissionFailedData(float ShowTime)
{
	MissionFailedLifeTime = ShowTime;
	LastMissionFailedUpdateTime = vw_GetTimeThread(0);
	// выводим курсор
	SetShowGameCursor(true);
	// сброс кнопки мышки, чтобы случайно не нажали
	vw_GetMouseLeftClick(true);
	// release mouse control
	SDL_SetWindowGrab(vw_GetSDLWindow(), SDL_FALSE);
}

//------------------------------------------------------------------------------------
// прорисовка миссия провалена
//------------------------------------------------------------------------------------
void GameDrawMissionFailed()
{
	if (MissionFailedLifeTime <= 0.0f)
		return;

	float TimeDelta = vw_GetTimeThread(0) - LastMissionFailedUpdateTime;
	LastMissionFailedUpdateTime = vw_GetTimeThread(0);

	// считаем только если не отображается меню
	if (GameContentTransp <= 0.0f)
		MissionFailedLifeTime -= TimeDelta;

	// все, время вышло... нужно сделать сброс и загрузить меню
	if (MissionFailedLifeTime <= 0.0f)
		ExitGame(eCommand::SWITCH_FROM_GAME_TO_MISSION_MENU);

	sRECT SrcRect, DstRect;

	// вывод надписи Mission
	SrcRect(0,0,512,84);
	DstRect(GameConfig().InternalWidth / 2 - 256, 342, GameConfig().InternalWidth / 2 + 256, 342 + 84);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(vw_GetText("lang/en/game/missionfailed.tga")), true);

	int Y = 180 + 270;
	int X = GameConfig().InternalWidth / 2 - 230;
	if (DrawButton200_2(X,Y, vw_GetText("QUIT"), 1.0f, false))
		ExitGame(eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU);

	X = GameConfig().InternalWidth / 2 + 30;
	if (DrawButton200_2(X,Y, vw_GetText("RESTART"), 1.0f, false))
		ExitGame(eCommand::SWITCH_FROM_MENU_TO_GAME);
}

} // astromenace namespace
} // viewizard namespace
