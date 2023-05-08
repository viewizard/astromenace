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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

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

namespace {

struct cMission {
    std::string Title{};
    std::string Descr{};
    sRGBCOLOR TitleColor{sRGBCOLOR{eRGBCOLOR::orange}};
    sRGBCOLOR DescrColor{sRGBCOLOR{eRGBCOLOR::white}};
    unsigned IconHash = 0;
    std::string File{};
};

std::vector<cMission> MissionList{};

} // unnamed namespace



int SoundOnMissionID = -1;

int CurrentMission = -1;
// total available missions
int AllMission;
// start and end of displayed missions
int StartMission = 0;
int EndMission = 4;

bool SliderUnderMouseControl = false;



// get mission file name
std::string GetCurrentMissionFileName()
{
    if (CurrentMission >= 0 && MissionList.size() > static_cast<unsigned>(CurrentMission)) {
        return MissionList[CurrentMission].File;
    }

    return std::string{};
}














//------------------------------------------------------------------------------------
// mission list data initialization
//------------------------------------------------------------------------------------
void MissionListInit()
{
    if (!MissionList.empty()) {
        return;
    }

    std::string ScriptName{"script/list.xml"};

    // look what should be download from script + count how many positions we have
    std::unique_ptr<cXMLDocument> xmlDoc{new cXMLDocument{ScriptName}};

    // check root element
    if (!xmlDoc->GetRootEntry() || "AstroMenaceMissionList" != xmlDoc->GetRootEntry()->Name) {
        std::cerr << __func__ << "(): " << "Can't find AstroMenaceMissionList element in the: " << ScriptName << "\n";
        return;
    }

    for (const auto &xmlEntry : xmlDoc->GetRootEntry()->ChildrenList) {
        MissionList.emplace_back();

        // get data for each mission
        if (xmlEntry.Name == "Mission") {
            for (const auto &TMission : xmlEntry.ChildrenList) {
                if (TMission.Name == "Title") {
                    int tmpColor{0};
                    if (xmlDoc->iGetEntryAttribute(TMission, "color", tmpColor)) {
                        switch (tmpColor) {
                        case 1:
                            MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::yellow};
                            break;
                        case 2:
                            MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::red};
                            break;
                        case 3:
                            MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::green};
                            break;
                        case 4:
                            MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::orange};
                            break;
                        case 5: // grey
                            MissionList.back().TitleColor = sRGBCOLOR{0.5f, 0.5f, 0.5f};
                            break;
                        case 6: // dark orange
                            MissionList.back().TitleColor = sRGBCOLOR{1.0f, 0.3f, 0.0f};
                            break;
                        default:
                            MissionList.back().TitleColor = sRGBCOLOR{eRGBCOLOR::white};
                            break;
                        }
                    }
                    MissionList.back().Title = TMission.Content;
                } else if (TMission.Name == "Descr") {
                    int tmpColor{0};
                    if (xmlDoc->iGetEntryAttribute(TMission, "color", tmpColor)) {
                        switch (tmpColor) {
                        case 1:
                            MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::yellow};
                            break;
                        case 2:
                            MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::red};
                            break;
                        case 3:
                            MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::green};
                            break;
                        case 4:
                            MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::orange};
                            break;
                        case 5: // grey
                            MissionList.back().DescrColor = sRGBCOLOR{0.5f, 0.5f, 0.5f};
                            break;
                        case 6: // dark orange
                            MissionList.back().DescrColor = sRGBCOLOR{1.0f, 0.3f, 0.0f};
                            break;
                        default:
                            MissionList.back().DescrColor = sRGBCOLOR{eRGBCOLOR::white};
                            break;
                        }
                    }
                    MissionList.back().Descr = TMission.Content;
                } else if (TMission.Name == "Icon") {
                    MissionList.back().IconHash = constexpr_hash_djb2a(TMission.Content.c_str());
                } else if (TMission.Name == "File") {
                    MissionList.back().File = TMission.Content;
                }
            }
        }
    }

    AllMission = MissionList.size();
}












//------------------------------------------------------------------------------------
// mission selection
//------------------------------------------------------------------------------------
void MissionMenu()
{

    // check range
    if (GameConfig().Profile[CurrentProfile].OpenLevelNum > AllMission-1) {
        ChangeGameConfig().Profile[CurrentProfile].OpenLevelNum = AllMission-1;
    }



    sRECT SrcRect, DstRect;
    SrcRect(2, 2, 863-2, 484-2);
    DstRect(GameConfig().InternalWidth/2-427, 175-15, GameConfig().InternalWidth/2-427+863-4, 175-15+484-4);
    constexpr unsigned tmpHash1 = constexpr_hash_djb2a("menu/panel800_444_back.tga");
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash1), true, 0.9f * MenuContentTransp);



    int X1 = GameConfig().InternalWidth/2 - 372;
    int Y1 = 270;




    // display the current pilot profile
    std::ostringstream tmpStream;
    tmpStream << vw_GetText("Pilot Profile") << ": ";
    int Size = vw_TextWidth(tmpStream.str());
    vw_DrawText(X1, 208+12, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, MenuContentTransp, tmpStream.str());

    if (Size + vw_TextWidth(GameConfig().Profile[CurrentProfile].Name) > 500) {
        vw_DrawText(X1+Size, 208+12, 0, 500-Size, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, GameConfig().Profile[CurrentProfile].Name);
        vw_DrawText(X1+510, 208+12, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, "...");
    } else {
        vw_DrawText(X1+Size, 208+12, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, GameConfig().Profile[CurrentProfile].Name);
    }

    if (DrawButton200_2(X1+616-72, 212, vw_GetTextUTF32("Change Profile"), MenuContentTransp, false)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_PROFILE);
    }




    SrcRect(0,0,2,2);
    DstRect(X1-2,Y1-2,X1+2+710,Y1+2+320);
    constexpr unsigned tmpHash2 = constexpr_hash_djb2a("menu/blackpoint.tga");
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash2), true, 0.2f*MenuContentTransp);
    DstRect(X1,Y1,X1+710,Y1+320);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash2), true, 0.5f*MenuContentTransp);




    // highlight the selected level
    if (CurrentMission != -1 && StartMission<=CurrentMission && CurrentMission<=EndMission) {
        int ShowLine = CurrentMission;
        if (ShowLine>=StartMission) {
            ShowLine -= StartMission;
        }

        SrcRect(0,0,2,2);
        DstRect(X1+1,Y1 + 64*ShowLine+1,X1+709,Y1 + 64*ShowLine+63);
        constexpr unsigned tmpHash3 = constexpr_hash_djb2a("menu/whitepoint.tga");
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash3), true, 0.1f*MenuContentTransp);
    }




    // display the missions of the current sheet
    int TMPSoundOnMissionID = -1;
    for (int i=StartMission; i<=EndMission; i++) {
        if (AllMission > i) {
            // if we can't choose...
            if (i > GameConfig().Profile[CurrentProfile].OpenLevelNum) {
                SrcRect(0,0,64,64);
                DstRect(X1+2,Y1+2,X1+62,Y1+62);

                vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(MissionList[i].IconHash), true, 0.3f*MenuContentTransp);
                vw_DrawTextUTF32(X1+20+64, Y1+9, -610, 0, 1.0f, MissionList[i].TitleColor, 0.3f*MenuContentTransp, vw_GetTextUTF32(MissionList[i].Title));
                vw_DrawTextUTF32(X1+20+64, Y1+33, -610, 0, 1.0f, MissionList[i].DescrColor, 0.3f*MenuContentTransp, vw_GetTextUTF32(MissionList[i].Descr));
            }


            DstRect(X1, Y1+1, X1+710, Y1+64);
            if (i <= GameConfig().Profile[CurrentProfile].OpenLevelNum) {
                // keyboard
                if (MenuContentTransp >= 0.99f && !isDialogBoxDrawing()) {
                    CurrentActiveMenuElement++;
                }
                bool InFocusByKeyboard = false;
                if (CurrentKeyboardSelectMenuElement > 0 && CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
                    InFocusByKeyboard = true;
                }


                if ((vw_MouseOverRect(DstRect) || InFocusByKeyboard) && !isDialogBoxDrawing()) {
                    TMPSoundOnMissionID = i;
                    SetCursorStatus(eCursorStatus::ActionAllowed);
                    if (SoundOnMissionID != i) {
                        SoundOnMissionID = i;
                        // don't play SFX for keyboard control
                        if (CurrentKeyboardSelectMenuElement == 0) {
                            PlayMenuSFX(eMenuSFX::OverLine, 1.0f);
                        }
                    }

                    SrcRect(0,0,64,64);
                    DstRect(X1,Y1,X1+64,Y1+64);
                    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(MissionList[i].IconHash), true, MenuContentTransp);
                    vw_DrawTextUTF32(X1+20+64, Y1+9, -610, 0, 1.0f, MissionList[i].TitleColor, MenuContentTransp, vw_GetTextUTF32(MissionList[i].Title));
                    vw_DrawTextUTF32(X1+20+64, Y1+33, -610, 0, 1.0f, MissionList[i].DescrColor, MenuContentTransp, vw_GetTextUTF32(MissionList[i].Descr));


                    if (CurrentMission != i) {
                        SrcRect(0,0,2,2);
                        DstRect(X1+64,Y1+1,X1+709,Y1+63);
                        constexpr unsigned tmpHash4 = constexpr_hash_djb2a("menu/whitepoint.tga");
                        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash4), true, 0.1f*MenuContentTransp);
                    }
                    if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {

                        CurrentMission = i;
                        ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;
                        PlayMenuSFX(eMenuSFX::SelectLine, 1.0f);
                        if (InFocusByKeyboard) {
                            vw_SetKeyStatus(SDLK_KP_ENTER, false);
                            vw_SetKeyStatus(SDLK_RETURN, false);
                        }
                    }

                    if (vw_GetMouseLeftDoubleClick(true)) {
                        CurrentMission = i;
                        ChangeGameConfig().Profile[CurrentProfile].LastMission = CurrentMission;
                        // if player already played this mission
                        if (GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
                            if (GameConfig().NeedShowHint[5]) {
                                SetCurrentDialogBox(eDialogBox::StartMissionSecondTime);
                            } else {
                                cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
                                CurrentWorkshop = 3;
                                WorkshopCreate();
                            }
                        } else {
                            cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
                            CurrentWorkshop = 3;
                            WorkshopCreate();
                        }
                    }
                } else {
                    SrcRect(0,0,64,64);
                    DstRect(X1+2,Y1+2,X1+62,Y1+62);
                    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(MissionList[i].IconHash), true, 0.8f*MenuContentTransp);
                    vw_DrawTextUTF32(X1+20+64, Y1+9, -610, 0, 1.0f, MissionList[i].TitleColor, 0.8f*MenuContentTransp, vw_GetTextUTF32(MissionList[i].Title));
                    vw_DrawTextUTF32(X1+20+64, Y1+33, -610, 0, 1.0f, MissionList[i].DescrColor, 0.8f*MenuContentTransp, vw_GetTextUTF32(MissionList[i].Descr));
                }
            }

            Y1 += 64;
        }
    }
    if (TMPSoundOnMissionID == -1) {
        SoundOnMissionID = -1;
    }





    Y1 = 270;
    // arrow up
    if (DrawListUpButton(X1+718, Y1, MenuContentTransp, !(StartMission > 0))) {
        StartMission--;
        EndMission--;
    }
    // arrow down
    if (DrawListDownButton(X1+718,Y1+320-32, MenuContentTransp, !(StartMission < AllMission-5))) {
        StartMission++;
        EndMission++;
    }
    // check mouse wheel, if cursor under
    DstRect(X1,Y1,X1+750,Y1+320);
    if (vw_MouseOverRect(DstRect)) {
        if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
            StartMission += vw_GetWheelStatus();
            EndMission += vw_GetWheelStatus();

            if (StartMission < 0) {
                StartMission = 0;
                EndMission = 4;
            }
            if (EndMission > AllMission-1) {
                EndMission = AllMission-1;
                StartMission = EndMission-4;
            }

            vw_ResetWheelStatus();
        }
    } else if (vw_GetWheelStatus() != 0) {
        vw_ResetWheelStatus();
    }

    // display position in the list on the strip with arrows
    SrcRect(0,0,32,32);
    DstRect(X1+750-32+4,Y1+32+((320.0f-64)/AllMission)*StartMission,X1+750-4,Y1+32+((320.0f-64)/AllMission)*(EndMission+1));
    constexpr unsigned tmpHash5 = constexpr_hash_djb2a("menu/whitepoint.tga");
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset(tmpHash5), true, 0.3f*MenuContentTransp);

    // handle dragging the slider to display the list position
    // if we stand on the slider and press the mouse button - "capture"
    if (!SliderUnderMouseControl && vw_MouseOverRect(DstRect) && vw_GetMouseLeftClick(false) && !isDialogBoxDrawing()) {
        SliderUnderMouseControl = true;
        PlayMenuSFX(eMenuSFX::Click, 1.0f);
    }
    // if the slider was captured, but is no longer over the section where it can be dragged or the mouse is released - release
    sRECT DstRect2;
    DstRect2(X1+750-32+4,Y1+32,X1+750-4,Y1+32+(320.0f-64));
    if ((SliderUnderMouseControl && (!vw_MouseOverRect(DstRect2) || !vw_GetMouseLeftClick(false))) || isDialogBoxDrawing()) {
        SliderUnderMouseControl = false;
    }
    // just clicked on the drag zone, not on the slider
    if (!vw_MouseOverRect(DstRect) && vw_MouseOverRect(DstRect2) && vw_GetMouseLeftClick(false) && !isDialogBoxDrawing()) {
        SliderUnderMouseControl = true;
        PlayMenuSFX(eMenuSFX::Click, 1.0f);
        vw_SetMouseLeftClick(false);
    }
    // switch cursor status that you can click on the scrollbar
    if (vw_MouseOverRect(DstRect2)) {
        SetCursorStatus(eCursorStatus::ActionAllowed);
    }
    // adjust its slider position according to the position of the mouse
    if (SliderUnderMouseControl) {
        int MouseX, MouseY;
        vw_GetMousePos(MouseX, MouseY);
        int SliderNewPosition = (MouseY - Y1-32)/((320.0f-64)/AllMission);

        StartMission = 0;
        EndMission = 4;
        if (SliderNewPosition > 2) {
            StartMission = SliderNewPosition-2;
            EndMission = SliderNewPosition+2;

            if (SliderNewPosition >= AllMission-2) {
                StartMission = AllMission-5;
                EndMission = AllMission-1;
            }
        }
    }






    int X = GameConfig().InternalWidth / 2 - 284;
    int Y = 165 + 100 * 5;
    if (DrawButton256(X,Y, vw_GetTextUTF32("MAIN MENU"), MenuContentTransp, Button10Transp, LastButton10UpdateTime)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
    }

    X = GameConfig().InternalWidth / 2 + 28;
    if (DrawButton256(X,Y, vw_GetTextUTF32("NEXT"), MenuContentTransp, Button11Transp, LastButton11UpdateTime, !(CurrentMission >= 0))) {
        if (GameConfig().Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
            if (GameConfig().NeedShowHint[5]) {
                SetCurrentDialogBox(eDialogBox::StartMissionSecondTime);
            } else {
                cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
                CurrentWorkshop = 3;
                WorkshopCreate();
            }
        } else {
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
            CurrentWorkshop = 3;
            WorkshopCreate();
        }
    }




}

} // astromenace namespace
} // viewizard namespace
