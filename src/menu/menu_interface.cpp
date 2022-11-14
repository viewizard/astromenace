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

#include "../core/core.h"
#include "../enum.h"
#include "../config/config.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../ui/font.h"
#include "../ui/fps_counter.h"
#include "../game/weapon_panel.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {


static std::string GetWeaponPanelViewTitle(eWeaponPanelView WeaponPanelView)
{
    switch (WeaponPanelView) {
    case eWeaponPanelView::full:
        return "Full";

    case eWeaponPanelView::flat:
        return "Flat";

    case eWeaponPanelView::slim:
        return "Slim";

    case eWeaponPanelView::hide:
        return "Off"; // FIXME this should be "Hide"
    }

    return {};
}



void InterfaceMenu(float ContentTransp, float &ButtonTransp1, float &LastButtonUpdateTime1)
{

    sRECT SrcRect, DstRect;
    SrcRect(0, 0, 2, 2);
    DstRect(0, 0, GameConfig().InternalWidth, 768);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f * ContentTransp);

    int X1 = GameConfig().InternalWidth / 2 - 375;
    int Y1 = 65;
    int Prir1 = 55;



    // Menu Language
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Menu Language"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, false)) {
        if (GameConfig().MenuLanguage == 0) {
            ChangeGameConfig().MenuLanguage = vw_GetLanguageListCount() - 1;
        } else {
            ChangeGameConfig().MenuLanguage--;
        }

        vw_SetTextLanguage(GameConfig().MenuLanguage);
        // forced to regenerate textures (base texture connected to language-related characters set)
        vw_ReleaseAllFontChars();
        GenerateFonts();
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, false)) {
        if (GameConfig().MenuLanguage >= vw_GetLanguageListCount() - 1) {
            ChangeGameConfig().MenuLanguage = 0;
        } else {
            ChangeGameConfig().MenuLanguage++;
        }

        vw_SetTextLanguage(GameConfig().MenuLanguage);
        // forced to regenerate textures (base texture connected to language-related characters set)
        vw_ReleaseAllFontChars();
        GenerateFonts();
    }

    int Size, SizeI;
    Size = vw_TextWidthUTF32(vw_GetTextUTF32("English", GameConfig().MenuLanguage));
    SizeI = (170-Size)/2;
    vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("English", GameConfig().MenuLanguage));




    // Voice Language
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Voice Language"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, false)) {
        if (GameConfig().VoiceLanguage == 0) {
            ChangeGameConfig().VoiceLanguage = vw_GetLanguageListCount() - 1;
        } else {
            ChangeGameConfig().VoiceLanguage--;
        }
        ReloadVoiceAssets();
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, false)) {
        if (GameConfig().VoiceLanguage >= vw_GetLanguageListCount() - 1) {
            ChangeGameConfig().VoiceLanguage = 0;
        } else {
            ChangeGameConfig().VoiceLanguage++;
        }
        ReloadVoiceAssets();
    }

    Size = vw_TextWidthUTF32(vw_GetTextUTF32("English", GameConfig().VoiceLanguage));
    SizeI = (170-Size)/2;
    vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("English", GameConfig().VoiceLanguage));



    // Menu Font
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Menu Font"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Prev"), ContentTransp, GameConfig().FontNumber <= 0)) {
        ChangeGameConfig().FontNumber--;

        // reinitialize fonts and regenerate textures (no need in vw_ShutdownFont() call)
        vw_ReleaseAllFontChars();
        vw_InitFont(GetFontMetadata(GameConfig().FontNumber).FontFileName);
        GenerateFonts();
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Next"), ContentTransp, (GameConfig().FontNumber + 1) >= GetFontQuantity())) {
        ChangeGameConfig().FontNumber++;

        // reinitialize fonts and regenerate textures (no need in vw_ShutdownFont() call)
        vw_ReleaseAllFontChars();
        vw_InitFont(GetFontMetadata(GameConfig().FontNumber).FontFileName);
        GenerateFonts();
    }
    int Scale = 0;
    Size = vw_TextWidth(GetFontMetadata(GameConfig().FontNumber).FontTitle);
    if (Size > 170) {
        Scale = -170;
        Size = 170;
    }
    SizeI = (170-Size)/2;
    vw_DrawText(X1+438+SizeI, Y1, Scale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, GetFontMetadata(GameConfig().FontNumber).FontTitle);




    // 'Tips and hints' Status
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("'Tips and hints' Status"));
    // check, do we have Hints, that previously was hided
    bool NeedReset = false;
    for (int i = 0; i < 10; i++) {
        if (!GameConfig().NeedShowHint[i]) {
            NeedReset = true;
        }
    }
    if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_GetTextUTF32("Reset"), ContentTransp, !NeedReset)) {
        if (NeedCheck == 0) {
            for (int i = 0; i < 10; i++) {
                ChangeGameConfig().NeedShowHint[i] = true;
            }
        }
    }




    // FPS Counter
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("FPS Counter"));
    if (DrawButton128_2(X1+300, Y1-6, vw_GetTextUTF32("Off"), ContentTransp, !ChangeGameConfig().ShowFPS)
        || DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("On"), ContentTransp, ChangeGameConfig().ShowFPS)) {
        cFPS::GetInstance().Switch();
    }

    Size = vw_TextWidthUTF32(GameConfig().ShowFPS ? vw_GetTextUTF32("On") : vw_GetTextUTF32("Off"));
    SizeI = (170 - Size) / 2;
    vw_DrawTextUTF32(X1+438+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, GameConfig().ShowFPS ? vw_GetTextUTF32("On") : vw_GetTextUTF32("Off"));




    // Weapon Panels View
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, -280, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32("Weapon Panels View"));
    if (DrawButton128_2(X1 + 300, Y1 - 6, vw_GetTextUTF32("Prev"), ContentTransp, GameConfig().WeaponPanelView == eWeaponPanelView::hide)) {
        WeaponPanelViewPrev(ChangeGameConfig().WeaponPanelView);
    }
    if (DrawButton128_2(X1 + 616, Y1 - 6, vw_GetTextUTF32("Next"), ContentTransp, GameConfig().WeaponPanelView == eWeaponPanelView::full)) {
        WeaponPanelViewNext(ChangeGameConfig().WeaponPanelView);
    }
    Size = vw_TextWidthUTF32(vw_GetTextUTF32(GetWeaponPanelViewTitle(GameConfig().WeaponPanelView)));
    SizeI = (170 - Size) / 2;
    vw_DrawTextUTF32(X1 + 438 + SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, ContentTransp, vw_GetTextUTF32(GetWeaponPanelViewTitle(GameConfig().WeaponPanelView)));















    int Prir = 100;
    int X;
    int Y = 165+Prir*4;


    X = GameConfig().InternalWidth/2 - 366;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Advanced"), ContentTransp, false)) {
        if (MenuStatus == eMenuStatus::GAME) {
            SetOptionsMenu(eMenuStatus::OPTIONS_ADVANCED);
            GameMenuStatus = eGameMenuStatus::OPTIONS_ADVANCED;
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_OPTIONS_ADVANCED);
        }
    }

    X = GameConfig().InternalWidth/2 - 100;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Video & Audio"), ContentTransp, false)) {
        if (MenuStatus == eMenuStatus::GAME) {
            SetOptionsMenu(eMenuStatus::OPTIONS);
            GameMenuStatus = eGameMenuStatus::OPTIONS;
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_OPTIONS);
        }
    }

    X = GameConfig().InternalWidth/2 + 166;
    if (DrawButton200_2(X,Y+28, vw_GetTextUTF32("Config Controls"), ContentTransp, false)) {
        if (MenuStatus == eMenuStatus::GAME) {
            SetOptionsMenu(eMenuStatus::CONFCONTROL);
            GameMenuStatus = eGameMenuStatus::CONFCONTROL;
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_CONFCONTROL);
        }
    }






    X = (GameConfig().InternalWidth - 384) / 2;
    Y = Y+Prir;
    if (MenuStatus == eMenuStatus::GAME) {
        if (DrawButton384(X,Y, vw_GetTextUTF32("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1)) {
            GameMenuStatus = eGameMenuStatus::GAME_MENU;
        }
    } else {
        if (DrawButton384(X,Y, vw_GetTextUTF32("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1)) {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
        }
    }
}

} // astromenace namespace
} // viewizard namespace
