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

// FIXME ostringstream is not so fast, move all string initialization into setup,
//       all ostringstream-related code should be called only one time in init

#include "../core/core.h"
#include "../enum.h"
#include "../config/config.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../ui/cursor.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include "SDL2/SDL.h"
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// current active profile number
int CurrentProfile = -1;


std::u32string NewProfileName;
float LastProfileNameTime = 0.0f;
float CurrentProfileNameTransp = 0.9f;
int SoundOnProfileID = -1;

unsigned int SoundTaping{0};



//------------------------------------------------------------------------------------
// create new record
//------------------------------------------------------------------------------------
void NewRecord()
{
    // add new profile as the last one
    int ProfileNum = -1;
    for (int i = 4; i >= 0; i--) {
        if (!GameConfig().Profile[i].Used) {
            ProfileNum = i;
        }
    }


    if (ProfileNum == -1) {
        // no empty slot
        SetCurrentDialogBox(eDialogBox::ProfileCreationError);
        return;
    }

    if (NewProfileName.empty()) {
        // can't create profile without profile name
        return;
    }


    // store profile data

    ChangeGameConfig().Profile[ProfileNum] = sPilotProfile{};
    ChangeGameConfig().Profile[ProfileNum].Used = true;
    strcpy(ChangeGameConfig().Profile[ProfileNum].Name, ConvertUTF8.to_bytes(NewProfileName).c_str());

    // clear for next usage
    NewProfileName.clear();

    CurrentProfile = ProfileNum;
    ChangeGameConfig().LastProfile = CurrentProfile;
    // by default, first mission only allowed and active (player will not need choose it from mission list)
    CurrentMission = 0;
    ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;
    ProfileDifficulty(ProfileNum, eDifficultyAction::Update);
}




//------------------------------------------------------------------------------------
// duplicate record
//------------------------------------------------------------------------------------
void DuplicateRecord()
{
    // find empty slot for duplicate
    int ProfileNum = -1;
    for (int i = 4; i >= 0; i--) {
        if (!GameConfig().Profile[i].Used) {
            ProfileNum = i;
        }
    }

    if (ProfileNum == -1) {
        // no empty slot
        SetCurrentDialogBox(eDialogBox::ProfileCreationError);
        return;
    }

    // copy all data into new profile record
    memcpy(&ChangeGameConfig().Profile[ProfileNum], &GameConfig().Profile[CurrentProfile], sizeof(GameConfig().Profile[0]));

    CurrentProfile = ProfileNum;
    ChangeGameConfig().LastProfile = CurrentProfile;
    CurrentMission = GameConfig().Profile[CurrentProfile].LastMission;
    ProfileDifficulty(ProfileNum, eDifficultyAction::Update);
}




//------------------------------------------------------------------------------------
// delete record
//------------------------------------------------------------------------------------
void DeleteRecord()
{
    if (CurrentProfile == -1) {
        return;
    }

    if (CurrentProfile == config::MAX_PROFILES - 1 || !GameConfig().Profile[CurrentProfile + 1].Used) {
        // delete last record in list
        ChangeGameConfig().Profile[CurrentProfile].Used = false;
        CurrentProfile -= 1;
    } else {
        // delete record from the middle of list
        for (int i = CurrentProfile; i < config::MAX_PROFILES - 1; i++) {
            memcpy(&ChangeGameConfig().Profile[i], &GameConfig().Profile[i + 1], sizeof(GameConfig().Profile[0]));
            ChangeGameConfig().Profile[i+1].Used = false;
        }
    }

    // if first profile was deleted - make first profile active again (if have it)
    if (CurrentProfile == -1 && GameConfig().Profile[0].Used) {
        CurrentProfile = 0;
    }

    ChangeGameConfig().LastProfile = CurrentProfile;
    // re-calculate all difficulty % in array for all profiles (we use pre-calculated array in order to update it only when we change something)
    ProfileDifficulty(config::MAX_PROFILES /*out of range*/, eDifficultyAction::UpdateAll);
}










//------------------------------------------------------------------------------------
// input text (new profile name)
//------------------------------------------------------------------------------------
void ProfileInputText()
{
    // <Ctrl>+<V> for copy text from clipboard.
    if ((vw_GetKeyStatus(SDLK_RCTRL) || vw_GetKeyStatus(SDLK_LCTRL)) && vw_GetKeyStatus(SDLK_v) && SDL_HasClipboardText() == SDL_TRUE) {
        char *tmpUTF8 = SDL_GetClipboardText();
        if (tmpUTF8) {
            for (auto &symbolUTF32 : ConvertUTF8.from_bytes(tmpUTF8)) {
                if (vw_TextWidthUTF32(NewProfileName) < 540) {
                    NewProfileName += symbolUTF32;
                } else {
                    break;
                }
            }
            SDL_free(tmpUTF8);
        }
        vw_SetKeyStatus(SDLK_v, false);
    }



    if (!vw_GetCurrentUnicodeChar().empty()) {// in case this is not 0, but unicode - key is pressed
        if (vw_TextWidthUTF32(NewProfileName) < 540) {
            NewProfileName += vw_GetCurrentUnicodeChar();

            if (vw_IsSoundAvailable(SoundTaping)) {
                vw_StopSound(SoundTaping, 0);
            }
            SoundTaping = PlayMenuSFX(eMenuSFX::TapingClick, 1.0f);
        }
        vw_SetCurrentUnicodeChar(nullptr); // reset data
    }



    if (vw_GetKeyStatus(SDLK_BACKSPACE) && !NewProfileName.empty()) {
        NewProfileName.pop_back();

        if (vw_IsSoundAvailable(SoundTaping)) {
            vw_StopSound(SoundTaping, 0);
        }
        SoundTaping = PlayMenuSFX(eMenuSFX::TapingClick, 1.0f);

        vw_SetKeyStatus(SDLK_BACKSPACE, false);
    }

    if ((vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)) && !NewProfileName.empty()) {
        NewRecord();
        //Audio_PlayMenuSound(4,1.0f);
        vw_SetKeyStatus(SDLK_KP_ENTER, false);
        vw_SetKeyStatus(SDLK_RETURN, false);
    }



    int X1 = GameConfig().InternalWidth / 2 - 372;
    int Y1 = 230;

    // draw input's blinking cursor
    int Size = vw_TextWidthUTF32(NewProfileName);
    sRECT SrcRect{0, 0, 2, 2};
    sRECT DstRect{X1 + Size + 2, Y1 - 2, X1 + 26 + Size, Y1 + 24};
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/whitepoint.tga"),
              true, CurrentProfileNameTransp * MenuContentTransp);

    float DeltaTime = vw_GetTimeThread(0) - LastProfileNameTime;
    LastProfileNameTime = vw_GetTimeThread(0);
    CurrentProfileNameTransp -= 2.0f * DeltaTime;
    if (CurrentProfileNameTransp < 0.2f) {
        CurrentProfileNameTransp = 0.9f;
    }

}

















//------------------------------------------------------------------------------------
// choice profile from the list
//------------------------------------------------------------------------------------
void ProfileMenu()
{
    sRECT SrcRect(2, 2, 861, 482);
    sRECT DstRect(GameConfig().InternalWidth / 2 - 427, 160, GameConfig().InternalWidth / 2 + 432, 160 + 480);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/panel800_444_back.tga"), true, 0.9f*MenuContentTransp);


    int X1 = GameConfig().InternalWidth / 2 - 372;
    int Y1 = 200;
    int Prir1 = 24;

    vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, MenuContentTransp, vw_GetTextUTF32("New Pilot Profile"));


    Y1 += 30;
    SrcRect(0,0,2,2);
    DstRect(X1-2,Y1-6,X1+2+590,Y1-2+30);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f*MenuContentTransp);
    DstRect(X1,Y1-4,X1+590,Y1-4+30);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f*MenuContentTransp);
    // new profile creation button
    bool Off = false;
    if (NewProfileName.empty()) {
        Off = true;
    }
    if (DrawButton128_2(X1+616, Y1-6, vw_GetTextUTF32("Create"), MenuContentTransp, Off)) {
        NewRecord();
    }

    if (!isDialogBoxDrawing() && MenuContentTransp == 1.0f) {
        ProfileInputText();
    }

    vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, NewProfileName);

    Y1 += Prir1;







    // profile list
    Y1 += Prir1;
    vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, MenuContentTransp, vw_GetTextUTF32("Pilot Profiles"));
    int Size = vw_TextWidthUTF32(vw_GetTextUTF32("Money"));
    float WScale = 0;
    if (Size > 70) {
        Size = 70;
        WScale = -70;
    }
    int SizeI = GameConfig().InternalWidth / 2 + 2 + (130 - Size) / 2;
    vw_DrawTextUTF32(SizeI, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, MenuContentTransp, vw_GetTextUTF32("Money"));

    Size = vw_TextWidthUTF32(vw_GetTextUTF32("Exp"));
    WScale = 0;
    if (Size > 100) {
        Size = 100;
        WScale = -100;
    }
    SizeI = GameConfig().InternalWidth / 2 + 132 + (130 - Size) / 2;
    vw_DrawTextUTF32(SizeI, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, MenuContentTransp, vw_GetTextUTF32("Exp"));

    Size = vw_TextWidthUTF32(vw_GetTextUTF32("Difficulty"));
    WScale = 0;
    if (Size > 100) {
        Size = 100;
        WScale = -100;
    }
    SizeI = GameConfig().InternalWidth / 2 + 262 + (130 - Size) / 2;
    vw_DrawTextUTF32(SizeI, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, MenuContentTransp, vw_GetTextUTF32("Difficulty"));


    Y1 += 30;
    SrcRect(0,0,2,2);
    DstRect(X1-2,Y1-6,X1+2+750,Y1-2+230);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.2f*MenuContentTransp);
    DstRect(X1,Y1-4,X1+750,Y1-4+230);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f*MenuContentTransp);
    Y1 += 230;



    int TMPSoundOnProfileID = -1;
    int TmpY = Y1-230+8;
    std::ostringstream tmpStream;
    tmpStream << std::fixed << std::setprecision(0);
    for (int i = 0; i < 5; i++) {
        if (GameConfig().Profile[i].Used) {
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << i + 1 << ".";
            vw_DrawText(X1+10, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

            if (vw_TextWidth(GameConfig().Profile[i].Name) > 300) {
                vw_DrawText(X1+50, TmpY, 0, 300, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, GameConfig().Profile[i].Name);
                vw_DrawText(X1+50+310, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "...");
            } else {
                vw_DrawText(X1+50, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, GameConfig().Profile[i].Name);
            }


            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << GameConfig().Profile[i].Money;
            Size = vw_TextWidth(tmpStream.str());
            SizeI = GameConfig().InternalWidth/2+2 + (130 - Size)/2;
            vw_DrawText(SizeI, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << GameConfig().Profile[i].Experience;
            Size = vw_TextWidth(tmpStream.str());
            SizeI = GameConfig().InternalWidth/2+132 + (130 - Size)/2;
            vw_DrawText(SizeI, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());

            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << ProfileDifficulty(i) << "%";
            Size = vw_TextWidth(tmpStream.str());
            SizeI = GameConfig().InternalWidth/2+262 + (130 - Size)/2;
            vw_DrawText(SizeI, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());


            // keyboard control
            if (MenuContentTransp >= 0.99f && !isDialogBoxDrawing()) {
                CurrentActiveMenuElement++;
            }
            bool InFocusByKeyboard = false;
            if (CurrentKeyboardSelectMenuElement > 0) {
                if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
                    InFocusByKeyboard = true;
                }
            }

            SrcRect(0,0,2,2);
            DstRect(X1,Y1-233+46*i,X1+750,Y1-234+46+46*i);
            if (!isDialogBoxDrawing() && (vw_MouseOverRect(DstRect) || InFocusByKeyboard)) {
                TMPSoundOnProfileID = i;
                SetCursorStatus(eCursorStatus::ActionAllowed);
                if (SoundOnProfileID != i) {
                    SoundOnProfileID = i;
                    // don't play SFX for keyboard control
                    if (CurrentKeyboardSelectMenuElement == 0) {
                        PlayMenuSFX(eMenuSFX::OverLine, 1.0f);
                    }
                }

                if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
                    if (CurrentProfile != i) {
                        CurrentMission = GameConfig().Profile[i].LastMission;
                    }
                    CurrentProfile = i;
                    ChangeGameConfig().LastProfile = CurrentProfile;
                    PlayMenuSFX(eMenuSFX::SelectLine, 1.0f);
                    if (InFocusByKeyboard) {
                        vw_SetKeyStatus(SDLK_KP_ENTER, false);
                        vw_SetKeyStatus(SDLK_RETURN, false);
                    }
                }

                if (CurrentProfile != i) {
                    if (vw_GetMouseLeftDoubleClick(true)) {
                        CurrentProfile = i;
                        ChangeGameConfig().LastProfile = CurrentProfile;
                        CurrentMission = GameConfig().Profile[CurrentProfile].LastMission;
                        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MISSION);
                    }


                    DstRect(X1+2,Y1-233+46*i,X1+748,Y1-235+46+46*i);
                    if (CurrentProfile != i) {
                        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/whitepoint.tga"), true, 0.1f*MenuContentTransp);
                    }
                } else {
                    if (vw_GetMouseLeftDoubleClick(true)) {
                        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MISSION);
                    }
                }
            }

        } else {
            float transp = 0.3f;
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << i + 1 << ".";
            vw_DrawText(X1+10, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, transp*MenuContentTransp, tmpStream.str());
            vw_DrawTextUTF32(X1+50, TmpY, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, transp*MenuContentTransp, vw_GetTextUTF32("empty"));
        }

        TmpY += 46;
    }
    if (TMPSoundOnProfileID == -1) {
        SoundOnProfileID = -1;
    }


    // highlight chosen profile
    if (CurrentProfile != -1) {
        SrcRect(0,0,2,2);
        DstRect(X1+2,Y1-233+46*CurrentProfile,X1+748,Y1-235+46+46*CurrentProfile);
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/whitepoint.tga"), true, 0.1f*MenuContentTransp);
    }








    Y1 += Prir1;


    // duplicate profile button
    Off = true;
    if (CurrentProfile >= 0) {
        Off = false;
    }
    if (DrawButton200_2(X1+6, Y1-6, vw_GetTextUTF32("Duplicate"), MenuContentTransp, Off)) {
        DuplicateRecord();
    }

    // delete profile button
    Off = true;
    if (CurrentProfile >= 0) {
        Off = false;
    }
    if (DrawButton128_2(X1+240, Y1-6, vw_GetTextUTF32("Delete"), MenuContentTransp, Off)) {
        SetCurrentDialogBox(eDialogBox::DeleteProfile);
    }


    // profile difficulty button
    Off = true;
    if (CurrentProfile >= 0) {
        Off = false;
    }
    if (DrawButton200_2(X1+544, Y1-6, vw_GetTextUTF32("Difficulty"), MenuContentTransp, Off)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_DIFFICULTY);
    }



    int X = GameConfig().InternalWidth / 2 - 284;
    int Y = 165+100*5;
    if (DrawButton256(X,Y, vw_GetTextUTF32("MAIN MENU"), MenuContentTransp, Button10Transp, LastButton10UpdateTime)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
    }

    Off = true;
    if (CurrentProfile >= 0) {
        Off = false;
    }
    X = GameConfig().InternalWidth / 2 + 28;
    if (DrawButton256(X,Y, vw_GetTextUTF32("MISSION LIST"), MenuContentTransp, Button11Transp, LastButton11UpdateTime, Off)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MISSION);
    }

}

} // astromenace namespace
} // viewizard namespace
