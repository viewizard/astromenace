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

#include "../../core/core.h"
#include "../../config/config.h"
#include "../../assets/texture.h"
#include "../../ui/cursor.h"
#include "../../command.h"
#include "../../game.h" // FIXME "game.h" should be replaced by individual headers
#include "SDL2/SDL.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

float MissionNumberLifeTime{0.0f};
float MissionNumberLastUpdateTime{0.0f};
std::string MissionNumberString{};
GLtexture MissionNumberTexture{0};

float MissionFailedLifeTime{0.0f};
float MissionFailedLastUpdateTime{0.0f};
GLtexture MissionFailedTexture{0};

} // unnamed namespace


/*
 * Setup mission number text.
 */
void SetupMissionNumberText(float NotificationTime, int Number)
{
    MissionNumberLifeTime = NotificationTime;
    if (MissionNumberLifeTime <= 0.0f) {
        return;
    }

    MissionNumberLastUpdateTime = vw_GetTimeThread(0);
    MissionNumberString = std::to_string(Number);
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
static int CalculateNumberStringWidth()
{
    int tmpWidth{0};

    for (const auto &Symbol : MissionNumberString) {
        sRECT SrcRect = GetNumberOnImageRect(Symbol);
        tmpWidth += SrcRect.right - SrcRect.left;
    }

    return tmpWidth;
}

/*
 * Draw number string.
 */
static void DrawNumberString(int X, int Y, float Transp)
{
    // note, we use left-top as starting point (upper left is origin)
    int XStart = X;
    for (const auto &Symbol : MissionNumberString) {
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
    if (MissionNumberLifeTime <= 0.0f) {
        return;
    }

    float CurrentTime = vw_GetTimeThread(0);
    MissionNumberLifeTime -= CurrentTime - MissionNumberLastUpdateTime;
    MissionNumberLastUpdateTime = CurrentTime;

    // FIXME SrcRect/DstRect/XStart/YStart should be calculated one time during setup, change only on window resize

    sRECT SrcRect{0, 0, 226, 64}; // "Mission" image-related rectangle
    constexpr int tmpSpace{20};
    int tmpWidth = (SrcRect.right - SrcRect.left) + tmpSpace + CalculateNumberStringWidth();

    int XStart = (GameConfig().InternalWidth - tmpWidth) / 2;
    constexpr int YStart{352};
    sRECT DstRect{XStart, YStart,
                  XStart + (SrcRect.right - SrcRect.left), YStart + (SrcRect.bottom - SrcRect.top)};

    float tmpTransp = (MissionNumberLifeTime > 1.0f) ? 1.0f : MissionNumberLifeTime;
    vw_Draw2D(DstRect, SrcRect, MissionNumberTexture, true, tmpTransp);
    DrawNumberString(XStart + (SrcRect.right - SrcRect.left) + tmpSpace, YStart, tmpTransp);
}

/*
 * Setup mission failed text.
 */
void SetupMissionFailedText(float NotificationTime)
{
    MissionFailedLifeTime = NotificationTime;
    if (MissionFailedLifeTime <= 0.0f) {
        return;
    }

    MissionFailedLastUpdateTime = vw_GetTimeThread(0);
    MissionFailedTexture = GetPreloadedTextureAsset(vw_GetText("lang/en/game/missionfailed.tga"));

    SetShowGameCursor(true);
    vw_GetMouseLeftClick(true);
    SDL_SetWindowGrab(reinterpret_cast<SDL_Window*>(vw_GetSDLWindow()), SDL_FALSE); // release mouse control
}

/*
 * Draw mission failed text.
 */
// FIXME this should be reorganized into "mission fail menu" instead of just "text"
void DrawMissionFailedText()
{
    if (MissionFailedLifeTime <= 0.0f) {
        return;
    }

    float CurrentTime = vw_GetTimeThread(0);
    MissionFailedLifeTime -= CurrentTime - MissionFailedLastUpdateTime;
    MissionFailedLastUpdateTime = CurrentTime;

    // FIXME SrcRect/DstRect should be calculated one time during setup, change only on window resize

    sRECT SrcRect{0, 0, 512, 84}; // "Mission failed" image-related rectangle
    sRECT DstRect{static_cast<int>(GameConfig().InternalWidth - (SrcRect.right - SrcRect.left)) / 2,
                  342,
                  static_cast<int>(GameConfig().InternalWidth + (SrcRect.right - SrcRect.left)) / 2,
                  342 + (SrcRect.bottom - SrcRect.top)};
    vw_Draw2D(DstRect, SrcRect, MissionFailedTexture, true);

    // FIXME all code below should be moved out from rendering block,
    //       "action" code should be separated

    if (MissionFailedLifeTime <= 0.0f) {
        ExitGame(eCommand::SWITCH_FROM_GAME_TO_MISSION_MENU);
    }

    if (!isDialogBoxDrawing() && vw_GetKeyStatus(SDLK_ESCAPE)) {
        ExitGame(eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU);
        vw_SetKeyStatus(SDLK_ESCAPE, false);
    }

    constexpr int Y{180 + 270};
    int X = GameConfig().InternalWidth / 2 - 230;
    if (DrawButton200_2(X,Y, vw_GetTextUTF32("QUIT"), 1.0f, false)) {
        ExitGame(eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU);
    }

    X = GameConfig().InternalWidth / 2 + 30;
    if (DrawButton200_2(X,Y, vw_GetTextUTF32("RESTART"), 1.0f, false)) {
        ExitGame(eCommand::SWITCH_FROM_MENU_TO_GAME);
    }
}

} // astromenace namespace
} // viewizard namespace
